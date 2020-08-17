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

#include <set>

#include "pfcpr15inl.h"
#include "epctools.h"
#include "eutil.h"

#include "pcpp.h"
#include "test.h"

namespace PFCPTest
{
   namespace pcaps
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
         EString originalPcap = "./pcaps/originals/" + test.name() + ".pcap";
         EString baselinePcap = "./pcaps/baselines/" + test.name() + ".pcap";
         EString resultPcap = "./pcaps/results/" + test.name() + ".pcap";

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
               // PFCP_R15::PfdMgmtReq &pfdMgmtReq = *(static_cast<PFCP_R15::PfdMgmtReq *>(appMsg.get()));

               // // TEMP fixup for Application ID spare byte
               // pfdMgmtReq.data().header.message_len += 1;
               // pfdMgmtReq.data().app_ids_pfds[0].header.len += 1;
               // pfdMgmtReq.data().app_ids_pfds[0].pfd_context[0].header.len += 1;
               // pfdMgmtReq.data().app_ids_pfds[0].pfd_context[0].pfd_contents[0].header.len += 1;
               break;
            }
            case PFCP_SESS_ESTAB_REQ:
            {
               // PFCP_R15::SessionEstablishmentReq &sessEstReq = *(static_cast<PFCP_R15::SessionEstablishmentReq *>(appMsg.get()));
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

         return ComparePackets(baselinePcap, resultPcap);
      }
   } // namespace pcaps
} // namespace PFCPTest