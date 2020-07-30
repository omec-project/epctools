/*
* Copyright (c) 2020 Sprint
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

#include "test.h"

#define LOG_SYSTEM 1
#define LOG_PFCP 2
#define LOG_TEST 3

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

   bool RunPcapTest(Test &test)
   {
      PcapTest &pcapTest = CAST_TEST(PcapTest);

      EString originalPcap = "./pcaps/originals/" + pcapTest.name() + ".pcap";
      EString baselinePcap = "./pcaps/baselines/" + pcapTest.name() + ".pcap";
      EString resultPcap =   "./pcaps/results/"   + pcapTest.name() + ".pcap";

      // Open the original pcap
      pcpp::IFileReaderDevice *originalReader = pcpp::IFileReaderDevice::getReader(originalPcap.c_str());

      if (reader == nullptr || !reader->open())
         throw EError(EError::Error, errno, "Can't open pcap");

      bool result = true;

      PFCP_R15::Translator trans;

      // Process each packet in the pcap
      pcpp::RawPacket rawPacket;
      pcpp::RawPacketVector resultPackets;
      while (reader->getNextPacket(rawPacket))
      {
         pcpp::Packet parsedPacket(&rawPacket);

         // Find the UDP layer
         pcpp::UdpLayer *udpLayer = parsedPacket.getLayerOfType<pcpp::UdpLayer>();
         if (udpLayer == nullptr)
            throw EError(EError::Error, errno, "Missing UDP layer");

         // Get the PFCP payload
         std::vector<uint8_t> payload;
         payload.assign(udpLayer->getLayerPayload(), udpLayer->getLayerPayload() + udpLayer->getLayerPayloadSize());

         // Extract PFCP header data
         PFCP::TranslatorMsgInfo info;
         trans.getMsgInfo(info, payload.data(), payload.size());

         PFCP::ReqInPtr msgIn = new PFCP::ReqIn();
         msgIn->setSeid(info.seid());
         msgIn->setSeqNbr(info.seqNbr());
         msgIn->setMsgType(info.msgType());
         msgIn->setIsReq(info.isReq());
         msgIn->setVersion(info.version());
         msgIn->assign(payload.data(), payload.size());

         // Decode the PFCP message
         PFCP::AppMsgReqPtr appMsg = trans.decodeReq(msgIn);

         // Cast it to the proper application layer type
         switch (appMsg->msgType())
         {
         case PFCP_PFD_MGMT_REQ:
         {
            //PFCP_R15::PfdMgmtReq &pfdMgmtReq = *(static_cast<PFCP_R15::PfdMgmtReq *>(appMsg));

            // // TEMP fixup for Application ID spare byte
            // pfdMgmtReq.data().header.message_len += 1;
            // pfdMgmtReq.data().app_ids_pfds[0].header.len += 1;
            // pfdMgmtReq.data().app_ids_pfds[0].pfd_context[0].header.len += 1;
            // pfdMgmtReq.data().app_ids_pfds[0].pfd_context[0].pfd_contents[0].header.len += 1;
            break;
         }
         default:
            ELogger::log(LOG_PFCP).major("Unhandled PFCP message type {}", appMsg->msgType());
         }

         // Encode the PFCP message
         PFCP::ReqOutPtr msgOut = trans.encodeReq(appMsg);

         std::vector<uint8_t> payloadOut;
         payloadOut.assign(msgOut->data(), msgOut->data() + msgOut->len());

         // Compare the original PFCP message with the encoded message
         if (payload.size() != payloadOut.size() ||
             memcmp(payload.data(), payloadOut.data(), payload.size()) != 0)
            result = false;

         // Replace the PFCP message in the packet
         parsedPacket.removeAllLayersAfter(udpLayer);

         pcpp::PayloadLayer *newLayer = new pcpp::PayloadLayer(payloadOut.data(), payloadOut.size(), false);
         parsedPacket.addLayer(newLayer, true);

         parsedPacket.computeCalculateFields();
         resultPackets.pushBack(new pcpp::RawPacket(*parsedPacket.getRawPacket()));
      }

      originalReader->close();

      // pcpp::IFileReaderDevice *baselineReader = pcpp::IFileReaderDevice::getReader(baselinePcap.c_str());
      // baselineReader->close();

      // Write the result pcap
      pcpp::PcapFileWriterDevice writer(resultPcap.c_str(), resultPackets.front()->getLinkLayerType());
      writer.open();
      for (auto packet : resultPackets)
         writer.writePacket(*packet);
      writer.close();

      return result;
   }
} // namespace PFCPTest::pcaps