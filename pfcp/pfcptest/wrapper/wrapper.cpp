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

#include "pfcpr15inl.h"
#include "epctools.h"
#include "epfcp.h"
#include "eutil.h"

#include "test.h"
#include "pcpp.h"

namespace PFCPTest
{
   namespace wrapper
   {
      void InitWrapperTest(const EString &resultPcap)
      {
         // Create the results folder if it doesn't exist
         EString resultsPath = "./wrapper/results/";
         EPath::verify(resultsPath);

         // Remove previous result from the results folder
         if (EUtility::file_exists(resultPcap))
         {
            if (!EUtility::delete_file(resultPcap))
               ELogger::log(LOG_SYSTEM).minor("Couldn't remove file {}", resultPcap);
         }
      }

      bool WrapperTest(Test &test, std::function<std::unique_ptr<PFCP::AppMsg>(PFCP::LocalNodeSPtr, PFCP::RemoteNodeSPtr)> buildAppMsg)
      {
         EString templatePcap = "./wrapper/template.pcap";
         EString baselinePcap = "./wrapper/baselines/" + test.name() + ".pcap";
         EString resultPcap = "./wrapper/results/" + test.name() + ".pcap";

         InitWrapperTest(resultPcap);

         ESocket::Address addr("1.2.3.4", 5);
         PFCP::LocalNodeSPtr ln = std::make_shared<PFCP::LocalNode>();
         ln->setAddress(addr);
         PFCP::RemoteNodeSPtr rn = std::make_shared<PFCP::RemoteNode>();
         rn->setAddress(addr);

         std::unique_ptr<PFCP::AppMsg> appMsg = buildAppMsg(ln, rn);

         std::vector<uint8_t> payloadOut = EncodeAppMsg(appMsg.get());

         pcpp::RawPacketVector tempalatePackets = GetPackets(templatePcap);
         pcpp::RawPacketVector resultPackets;
         // Add to result packets (the packet vector owns the packet pointer)
         pcpp::RawPacket *resultPacket = new pcpp::RawPacket(*tempalatePackets.front());
         resultPackets.pushBack(resultPacket);
         pcpp::Packet packet(resultPacket);
         ReplacePFCPPayload(packet, payloadOut);

         // Write the result pcap
         WritePcap(resultPcap, resultPackets);

         return ComparePackets(baselinePcap, resultPcap);
      }

      TEST(wrapper_pfcp_assn_setup_req)
      {
         auto buildAppMsg = [](PFCP::LocalNodeSPtr ln, PFCP::RemoteNodeSPtr rn) {
            PFCP_R15::AssnSetupReq *msg = new PFCP_R15::AssnSetupReq(ln, rn);

            ESocket::Address addr_ipv4("1.2.3.4", 5);
            ESocket::Address addr_ipv6("1111:2222:3333:4444:5555:6666:7777:8888", 9999);
            msg->node_id().node_id_value(addr_ipv4);

            ETime recvyTimeStmp(2020, 8, 13, 5, 4, 31, False);
            msg->rcvry_time_stmp().rcvry_time_stmp_val(recvyTimeStmp);

            msg->up_func_feat().bucp(True);
            msg->up_func_feat().dlbd(True);
            msg->up_func_feat().frrt(True);
            
            msg->cp_func_feat().ovrl(True);

            for(int iupip = 0; iupip < 4; ++iupip)
            {
               int idx = msg->next_user_plane_ip_rsrc_info();
               msg->user_plane_ip_rsrc_info(idx).teid_range(4, 15);
               msg->user_plane_ip_rsrc_info(idx).ip_address(addr_ipv4);
               msg->user_plane_ip_rsrc_info(idx).ip_address(addr_ipv6);
               UChar apn[] = { 4, 'a', 'p', 'n', '1' };
               apn[4] += iupip;
               msg->user_plane_ip_rsrc_info(idx).ntwk_inst((pUChar) apn, (UShort) strlen((cpStr) apn));
               msg->user_plane_ip_rsrc_info(idx).src_intfc(PFCP_R15::SourceInterfaceEnum::CPFunction);
            }

            return std::unique_ptr<PFCP::AppMsg>(msg);
         };

         return WrapperTest(test, buildAppMsg);
      }

      TEST(wrapper_pfcp_assn_setup_rsp)
      {
         auto buildAppMsg = [](PFCP::LocalNodeSPtr ln, PFCP::RemoteNodeSPtr rn) {
            PFCP_R15::AssnSetupRsp *msg = new PFCP_R15::AssnSetupRsp();

            ESocket::Address addr_ipv4("1.2.3.4", 5);
            ESocket::Address addr_ipv6("1111:2222:3333:4444:5555:6666:7777:8888", 9999);
            msg->node_id().node_id_value(addr_ipv4);

            msg->cause().cause(PFCP_R15::CauseEnum::RequestAccepted);

            ETime recvyTimeStmp(2020, 8, 13, 5, 4, 31, False);
            msg->rcvry_time_stmp().rcvry_time_stmp_val(recvyTimeStmp);

            msg->up_func_feat().bucp(True);
            msg->up_func_feat().dlbd(True);
            msg->up_func_feat().frrt(True);
            
            msg->cp_func_feat().ovrl(True);

            for(int iupip = 0; iupip < 4; ++iupip)
            {
               int idx = msg->next_user_plane_ip_rsrc_info();
               msg->user_plane_ip_rsrc_info(idx).teid_range(4, 15);
               msg->user_plane_ip_rsrc_info(idx).ip_address(addr_ipv4);
               msg->user_plane_ip_rsrc_info(idx).ip_address(addr_ipv6);
               UChar apn[] = { 4, 'a', 'p', 'n', '1' };
               apn[4] += iupip;
               msg->user_plane_ip_rsrc_info(idx).ntwk_inst((pUChar) apn, (UShort) strlen((cpStr) apn));
               msg->user_plane_ip_rsrc_info(idx).src_intfc(PFCP_R15::SourceInterfaceEnum::CPFunction);
            }

            PFCP::AppMsgNodeReqPtr dummyReq = new PFCP::AppMsgNodeReq();
            msg->setReq(dummyReq);

            return std::unique_ptr<PFCP::AppMsg>(msg);
         };

         return WrapperTest(test, buildAppMsg);
      }
   } // namespace wrapper
} // namespace PFCPTest