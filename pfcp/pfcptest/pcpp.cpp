/*
* Copyright (c) 2020 T-Mobile
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include "pcpp.h"

#include "UdpLayer.h"
#include "PcapFileDevice.h"
#include "PayloadLayer.h"

#include "pfcpr15.h"
#include "epctools.h"
#include "epfcp.h"
#include "eutil.h"

#include "test.h"

#define MAX_PACKET_COUNT 65536

namespace PFCPTest
{
   PFCP_R15::Translator &GetTranslator()
   {
      static PFCP_R15::Translator trans;
      return trans;
   }

   pcpp::RawPacketVector GetPackets(const EString &pcap)
   {
      pcpp::IFileReaderDevice *reader = pcpp::IFileReaderDevice::getReader(pcap);

      if (reader == nullptr || !reader->open())
         throw EError(EError::Error, errno, "Can't open pcap");

      pcpp::RawPacketVector packets;
      reader->getNextPackets(packets, MAX_PACKET_COUNT);
      reader->close();

      return packets;
   }

   std::vector<uint8_t> ExtractPFCPPayload(const pcpp::Packet &packet)
   {
      // Find the UDP layer
      const pcpp::UdpLayer *udpLayer = packet.getLayerOfType<pcpp::UdpLayer>();
      if (udpLayer == nullptr)
         throw EError(EError::Error, "Missing UDP layer");

      // Get the PFCP payload
      std::vector<uint8_t> payload;
      payload.assign(udpLayer->getLayerPayload(), udpLayer->getLayerPayload() + udpLayer->getLayerPayloadSize());

      return payload;
   }

   void ReplacePFCPPayload(pcpp::Packet &packet, const std::vector<uint8_t> &payload)
   {
      // Find the UDP layer
      pcpp::UdpLayer *udpLayer = packet.getLayerOfType<pcpp::UdpLayer>();
      if (udpLayer == nullptr)
         throw EError(EError::Error, "Missing UDP layer");

      // Remove the PFCP layer
      packet.removeAllLayersAfter(udpLayer);

      // Add the new layer (the packet owns the layer pointer)
      pcpp::PayloadLayer *newLayer = new pcpp::PayloadLayer(payload.data(), payload.size(), false);
      packet.addLayer(newLayer, true);

      // Update the other layer lengths
      packet.computeCalculateFields();
   }

   std::unique_ptr<PFCP::AppMsg> DecodeAppMsg(const std::vector<uint8_t> &payload)
   {
      // Extract PFCP header data
      PFCP::TranslatorMsgInfo tmi;
      GetTranslator().getMsgInfo(tmi, payload.data(), payload.size());

      ESocket::Address addr("0.0.0.0", 0);
      PFCP::LocalNodeSPtr ln = std::make_shared<PFCP::LocalNode>();
      ln->setAddress(addr);
      PFCP::RemoteNodeSPtr rn = std::make_shared<PFCP::RemoteNode>();
      rn->setAddress(addr);

      if (tmi.isReq())
      {
         std::unique_ptr<PFCP::ReqIn> msgIn(new PFCP::ReqIn(ln, rn, tmi, payload.data(), payload.size()));
         if (tmi.msgClass() == PFCP::MsgClass::Session)
         {
            PFCP::SessionBaseSPtr ses = std::make_shared<PFCP::SessionBase>(ln, rn);
            ses->setSeid(ses, tmi.seid(), tmi.seid(), False);
            msgIn->setSession(ses);
         }

         // Decode the PFCP message
         return std::unique_ptr<PFCP::AppMsg>(GetTranslator().decodeReq(msgIn.get()));
      }
      else
      {
         PFCP::AppMsgReqPtr dummyReq;
         if (tmi.msgClass() == PFCP::MsgClass::Session)
         {
            PFCP::SessionBaseSPtr ses = std::make_shared<PFCP::SessionBase>(ln, rn);
            ses->setSeid(ses, tmi.seid(), tmi.seid(), False);
            dummyReq = new PFCP::AppMsgSessionReq(ses,False);
         }
         else
         {
            dummyReq = new PFCP::AppMsgNodeReq();
         }

         dummyReq->setSeqNbr(tmi.seqNbr());

         std::unique_ptr<PFCP::RspIn> msgIn(new PFCP::RspIn(ln, rn, tmi, payload.data(), payload.size(), dummyReq));
         if (tmi.msgClass() == PFCP::MsgClass::Session)
         {
            msgIn->setSession(static_cast<PFCP::AppMsgSessionReqPtr>(dummyReq)->session());
         }

         // Decode the PFCP message
         return std::unique_ptr<PFCP::AppMsg>(GetTranslator().decodeRsp(msgIn.get()));
      }
   }

   std::vector<uint8_t> EncodeAppMsg(PFCP::AppMsgPtr appMsg)
   {
      PFCP::InternalMsgPtr msg;

      if (appMsg->isReq())
         msg = GetTranslator().encodeReq(static_cast<PFCP::AppMsgReqPtr>(appMsg));
      else
         msg = GetTranslator().encodeRsp(static_cast<PFCP::AppMsgRspPtr>(appMsg));

      std::vector<uint8_t> payload;
      payload.assign(msg->data(), msg->data() + msg->len());

      if (appMsg->isReq())
         static_cast<PFCP::ReqOutPtr>(msg)->setAppMsg(nullptr);
      else
         static_cast<PFCP::RspOutPtr>(msg)->setRsp(nullptr);

      delete msg;

      return payload;
   }

   void WritePcap(const EString &pcap, pcpp::RawPacketVector &packets)
   {
      pcpp::PcapFileWriterDevice writer(pcap, packets.front()->getLinkLayerType());
      writer.open();
      for (auto packet : packets)
         writer.writePacket(*packet);
      writer.close();
   }

   bool ComparePackets(const EString &baselinePcap, const EString &resultPcap)
   {
      if (!EUtility::file_exists(baselinePcap))
      {
         ELogger::log(LOG_TEST).minor("Missing baseline pcap file {}", baselinePcap);
         return false;
      }

      // If a baseline pcap exists, compare all the result pcaps for equality
      bool result = true;

      pcpp::RawPacketVector baselinePackets = GetPackets(baselinePcap);
      pcpp::RawPacketVector resultPackets = GetPackets(resultPcap);

      if (baselinePackets.size() != resultPackets.size())
      {
         ELogger::log(LOG_TEST).major("Missing packets in pcap. Expected {}, found {}.", baselinePackets.size(), resultPackets.size());
         return false;
      }

      for (size_t ipacket = 0; ipacket < baselinePackets.size(); ++ipacket)
      {
         pcpp::Packet baselinePacket(baselinePackets.at(ipacket));
         pcpp::Packet resultPacket(resultPackets.at(ipacket));

         std::vector<uint8_t> baselinePayload = ExtractPFCPPayload(baselinePacket);
         std::vector<uint8_t> resultPayload = ExtractPFCPPayload(resultPacket);

         // Compare the original PFCP message with the encoded message
         if (baselinePayload.size() != resultPayload.size() ||
             memcmp(baselinePayload.data(), resultPayload.data(), baselinePayload.size()) != 0)
         {
            result = false;
            ELogger::log(LOG_TEST).major("Differences found in packet {}", ipacket + 1);
         }
      }

      return result;
   }
} // namespace PFCPTest