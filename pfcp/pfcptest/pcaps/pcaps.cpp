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

#include "pcaps.h"

#include <stdlib.h>
#include <vector>
#include <set>
#include <signal.h>

#include "Packet.h"
#include "UdpLayer.h"
#include "PcapFileDevice.h"
#include "PayloadLayer.h"

#include "pfcpr15.h"
#include "epctools.h"
#include "epfcp.h"
#include "eutil.h"

#include "test.h"

#define LOG_SYSTEM 1
#define LOG_PFCP 2
#define LOG_TEST 3

#define MAX_PACKET_COUNT 65536

namespace PFCPTest::pcaps
{
   void InitTests()
   {
      // Load pcap tests from disk
      std::set<EString> pcapFiles;
      {
         EString pcapsPath = "./pcaps/originals";
         EDirectory pcapsDir;
         cpStr pcapFile = pcapsDir.getFirstEntry(pcapsPath, "*.pcap");
         while (pcapFile)
         {
            pcapFiles.emplace(pcapFile);

            EString name = EPath::getFileNameWithoutExtension(pcapFile);
            std::unique_ptr<Test> pcapTest(new PcapTest(name));
            TestSuite::add(name, std::move(pcapTest));

            pcapFile = pcapsDir.getNextEntry();
         }
      }

      // Create the results folder if it doesn't exist
      EString resultsPath = "./pcaps/results/";
      EPath::verify(resultsPath);

      // Remove previous results from the results folder
      EDirectory dir;
      cpStr fn = dir.getFirstEntry(resultsPath, "*");
      while (fn)
      {
         EString file(fn);
         if (pcapFiles.count(file) > 0)
         {
            EString filePath;
            EPath::combine(resultsPath, file, filePath);
            if (!EUtility::delete_file(filePath))
               ELogger::log(LOG_SYSTEM).minor("Couldn't remove file {}", filePath);
         }
         fn = dir.getNextEntry();
      }
   }

   pcpp::RawPacketVector GetPackets(const EString &pcap)
   {
      pcpp::IFileReaderDevice *reader = pcpp::IFileReaderDevice::getReader(pcap.c_str());

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
      PFCP_R15::Translator trans;

      // Extract PFCP header data
      PFCP::TranslatorMsgInfo info;
      trans.getMsgInfo(info, payload.data(), payload.size());

      PFCP::LocalNodeSPtr ln = std::make_shared<PFCP::LocalNode>();
      PFCP::RemoteNodeSPtr rn = std::make_shared<PFCP::RemoteNode>();

      if (info.isReq())
      {
         std::unique_ptr<PFCP::ReqIn> msgIn(new PFCP::ReqIn(ln, rn, info, payload.data(), payload.size()));

         // Decode the PFCP message
         return std::unique_ptr<PFCP::AppMsg>(trans.decodeReq(msgIn.get()));
      }
      else
      {
         std::unique_ptr<PFCP::RspIn> msgIn(new PFCP::RspIn(ln, rn, info, payload.data(), payload.size(), new PFCP::AppMsgReq()));

         // Decode the PFCP message
         return std::unique_ptr<PFCP::AppMsg>(trans.decodeRsp(msgIn.get()));
      }
   }

   std::vector<uint8_t> EncodeAppMsg(PFCP::AppMsgPtr appMsg)
   {
      PFCP_R15::Translator trans;
      PFCP::InternalMsgPtr msg;

      if (appMsg->isReq())
         msg = trans.encodeReq(dynamic_cast<PFCP::AppMsgReqPtr>(appMsg));
      else
         msg = trans.encodeRsp(dynamic_cast<PFCP::AppMsgRspPtr>(appMsg));

      std::vector<uint8_t> payload;
      payload.assign(msg->data(), msg->data() + msg->len());

      return payload;
   }

   void WritePcap(const EString &pcap, pcpp::RawPacketVector &packets)
   {
      pcpp::PcapFileWriterDevice writer(pcap.c_str(), packets.front()->getLinkLayerType());
      writer.open();
      for (auto packet : packets)
         writer.writePacket(*packet);
      writer.close();
   }

   bool RunPcapTest(Test &test)
   {
      PcapTest &pcapTest = CAST_TEST(PcapTest);

      EString originalPcap = "./pcaps/originals/" + pcapTest.name() + ".pcap";
      EString baselinePcap = "./pcaps/baselines/" + pcapTest.name() + ".pcap";
      EString resultPcap = "./pcaps/results/" + pcapTest.name() + ".pcap";

      // Load each packet from the original pcap, decode and then encode it and
      // add it to result packets
      pcpp::RawPacketVector originalPackets = GetPackets(originalPcap);
      pcpp::RawPacketVector resultPackets;
      int packetNumber = 1;
      for (auto originalPacket : originalPackets)
      {
         // Add to result packets (the packet vector owns the packet pointer)
         pcpp::RawPacket *resultPacket = new pcpp::RawPacket(*originalPacket);
         resultPackets.pushBack(resultPacket);

         pcpp::Packet packet(resultPacket);

         std::vector<uint8_t> payload = ExtractPFCPPayload(packet);
         std::unique_ptr<PFCP::AppMsg> appMsg = DecodeAppMsg(payload);
         if (!appMsg)
         {
            ELogger::log(LOG_TEST).major("Unhandled PFCP message type in packet {}", packetNumber);
            ++packetNumber;
            continue;
         }

         // Cast it to the proper application layer type
         switch (appMsg.get()->msgType())
         {
         case PFCP_PFD_MGMT_REQ:
         {
            //PFCP_R15::PfdMgmtReq &pfdMgmtReq = *(static_cast<PFCP_R15::PfdMgmtReq *>(appMsg.get()));

            // // TEMP fixup for Application ID spare byte
            // pfdMgmtReq.data().header.message_len += 1;
            // pfdMgmtReq.data().app_ids_pfds[0].header.len += 1;
            // pfdMgmtReq.data().app_ids_pfds[0].pfd_context[0].header.len += 1;
            // pfdMgmtReq.data().app_ids_pfds[0].pfd_context[0].pfd_contents[0].header.len += 1;
            break;
         }
         case PFCP_SESS_ESTAB_RSP:
         {
            //PFCP_R15::SessionEstablishmentRsp &sessEstRsp = *(static_cast<PFCP_R15::SessionEstablishmentRsp *>(appMsg.get()));
            break;
         }
         default:
            //ELogger::log(LOG_PFCP).major("Unhandled PFCP message type {}", appMsg->msgType());
            break;
         }

         std::vector<uint8_t> payloadOut = EncodeAppMsg(appMsg.get());
         ReplacePFCPPayload(packet, payloadOut);

         ++packetNumber;
      }

      // Write the result pcap
      WritePcap(resultPcap, resultPackets);

      // If a baseline pcap exists, compare all the result pcaps for equality
      bool result = true;
      if (EUtility::file_exists(baselinePcap.c_str()))
      {
         pcpp::RawPacketVector baselinePackets = GetPackets(originalPcap);

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
      }

      return result;
   }
} // namespace PFCPTest::pcaps