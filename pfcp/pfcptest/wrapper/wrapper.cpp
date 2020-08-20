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

         pcpp::RawPacketVector templatePackets = GetPackets(templatePcap);
         pcpp::RawPacketVector resultPackets;
         // Add to result packets (the packet vector owns the packet pointer)
         pcpp::RawPacket *resultPacket = new pcpp::RawPacket(*templatePackets.front());
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

            for (int iupip = 0; iupip < 4; ++iupip)
            {
               int idx = msg->next_user_plane_ip_rsrc_info();
               msg->user_plane_ip_rsrc_info(idx).teid_range(4, 15);
               msg->user_plane_ip_rsrc_info(idx).ip_address(addr_ipv4);
               msg->user_plane_ip_rsrc_info(idx).ip_address(addr_ipv6);
               UChar apn[] = {4, 'a', 'p', 'n', '1'};
               apn[4] += iupip;
               msg->user_plane_ip_rsrc_info(idx).ntwk_inst((pUChar)apn, (UShort)strlen((cpStr)apn));
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

            for (int iupip = 0; iupip < 4; ++iupip)
            {
               int idx = msg->next_user_plane_ip_rsrc_info();
               msg->user_plane_ip_rsrc_info(idx).teid_range(4, 15);
               msg->user_plane_ip_rsrc_info(idx).ip_address(addr_ipv4);
               msg->user_plane_ip_rsrc_info(idx).ip_address(addr_ipv6);
               UChar apn[] = {4, 'a', 'p', 'n', '1'};
               apn[4] += iupip;
               msg->user_plane_ip_rsrc_info(idx).ntwk_inst((pUChar)apn, (UShort)strlen((cpStr)apn));
               msg->user_plane_ip_rsrc_info(idx).src_intfc(PFCP_R15::SourceInterfaceEnum::CPFunction);
            }

            PFCP::AppMsgNodeReqPtr dummyReq = new PFCP::AppMsgNodeReq();
            msg->setReq(dummyReq);

            return std::unique_ptr<PFCP::AppMsg>(msg);
         };

         return WrapperTest(test, buildAppMsg);
      }

      TEST(wrapper_pfcp_assn_update_req)
      {
         auto buildAppMsg = [](PFCP::LocalNodeSPtr ln, PFCP::RemoteNodeSPtr rn) {
            PFCP_R15::AssnUpdateReq *msg = new PFCP_R15::AssnUpdateReq(ln, rn);

            ESocket::Address addr_ipv4("1.2.3.4", 5);
            ESocket::Address addr_ipv6("1111:2222:3333:4444:5555:6666:7777:8888", 9999);
            msg->node_id().node_id_value(addr_ipv4);

            msg->up_func_feat().bucp(True);
            msg->up_func_feat().dlbd(True);
            msg->up_func_feat().frrt(True);

            msg->cp_func_feat().ovrl(True);

            msg->up_assn_rel_req().sarr(True);

            msg->graceful_rel_period().timer_value(4, PFCP_R15::GracefulReleasePeriodTimerUnitEnum::infinite);

            for (int iupip = 0; iupip < 4; ++iupip)
            {
               int idx = msg->next_user_plane_ip_rsrc_info();
               msg->user_plane_ip_rsrc_info(idx).teid_range(4, 15);
               msg->user_plane_ip_rsrc_info(idx).ip_address(addr_ipv4);
               msg->user_plane_ip_rsrc_info(idx).ip_address(addr_ipv6);
               UChar apn[] = {4, 'a', 'p', 'n', '1'};
               apn[4] += iupip;
               msg->user_plane_ip_rsrc_info(idx).ntwk_inst((pUChar)apn, (UShort)strlen((cpStr)apn));
               msg->user_plane_ip_rsrc_info(idx).src_intfc(PFCP_R15::SourceInterfaceEnum::CPFunction);
            }

            return std::unique_ptr<PFCP::AppMsg>(msg);
         };

         return WrapperTest(test, buildAppMsg);
      }

      TEST(wrapper_pfcp_assn_update_rsp)
      {
         auto buildAppMsg = [](PFCP::LocalNodeSPtr ln, PFCP::RemoteNodeSPtr rn) {
            PFCP_R15::AssnUpdateRsp *msg = new PFCP_R15::AssnUpdateRsp();

            ESocket::Address addr_ipv4("1.2.3.4", 5);
            msg->node_id().node_id_value(addr_ipv4);

            msg->cause().cause(PFCP_R15::CauseEnum::RequestAccepted);

            msg->up_func_feat().bucp(True);
            msg->up_func_feat().dlbd(True);
            msg->up_func_feat().frrt(True);

            msg->cp_func_feat().ovrl(True);

            PFCP::AppMsgNodeReqPtr dummyReq = new PFCP::AppMsgNodeReq();
            msg->setReq(dummyReq);

            return std::unique_ptr<PFCP::AppMsg>(msg);
         };

         return WrapperTest(test, buildAppMsg);
      }

      TEST(wrapper_pfcp_assn_release_req)
      {
         auto buildAppMsg = [](PFCP::LocalNodeSPtr ln, PFCP::RemoteNodeSPtr rn) {
            PFCP_R15::AssnReleaseReq *msg = new PFCP_R15::AssnReleaseReq(ln, rn);

            ESocket::Address addr_ipv4("1.2.3.4", 5);
            msg->node_id().node_id_value(addr_ipv4);

            return std::unique_ptr<PFCP::AppMsg>(msg);
         };

         return WrapperTest(test, buildAppMsg);
      }

      TEST(wrapper_pfcp_assn_release_rsp)
      {
         auto buildAppMsg = [](PFCP::LocalNodeSPtr ln, PFCP::RemoteNodeSPtr rn) {
            PFCP_R15::AssnReleaseRsp *msg = new PFCP_R15::AssnReleaseRsp();

            ESocket::Address addr_ipv4("1.2.3.4", 5);
            msg->node_id().node_id_value(addr_ipv4);

            msg->cause().cause(PFCP_R15::CauseEnum::RequestAccepted);

            PFCP::AppMsgNodeReqPtr dummyReq = new PFCP::AppMsgNodeReq();
            msg->setReq(dummyReq);

            return std::unique_ptr<PFCP::AppMsg>(msg);
         };

         return WrapperTest(test, buildAppMsg);
      }

      TEST(wrapper_pfcp_version_not_supported_rsp)
      {
         auto buildAppMsg = [](PFCP::LocalNodeSPtr ln, PFCP::RemoteNodeSPtr rn) {
            PFCP_R15::VersionNotSupportedRsp *msg = new PFCP_R15::VersionNotSupportedRsp();

            PFCP::AppMsgNodeReqPtr dummyReq = new PFCP::AppMsgNodeReq();
            msg->setReq(dummyReq);

            return std::unique_ptr<PFCP::AppMsg>(msg);
         };

         return WrapperTest(test, buildAppMsg);
      }

      TEST(wrapper_pfcp_node_report_req)
      {
         auto buildAppMsg = [](PFCP::LocalNodeSPtr ln, PFCP::RemoteNodeSPtr rn) {
            PFCP_R15::NodeReportReq *msg = new PFCP_R15::NodeReportReq(ln, rn);

            ESocket::Address addr_ipv4("1.2.3.4", 5);
            ESocket::Address addr_ipv6("1111:2222:3333:4444:5555:6666:7777:8888", 9999);
            msg->node_id().node_id_value(addr_ipv4);

            msg->node_rpt_type().upfr(True);
            
            for (int ipeer = 0; ipeer < 4; ++ipeer)
            {
               int idx = msg->user_plane_path_fail_rpt().next_rmt_gtpu_peer();
               msg->user_plane_path_fail_rpt().rmt_gtpu_peer(idx).ip_address(addr_ipv4);
               msg->user_plane_path_fail_rpt().rmt_gtpu_peer(idx).ip_address(addr_ipv6);

               UChar apn[] = {4, 'a', 'p', 'n', '1'};
               apn[4] += ipeer;
               msg->user_plane_path_fail_rpt().rmt_gtpu_peer(idx).ntwk_instc((pUChar)apn, (UShort)strlen((cpStr)apn));
               
               msg->user_plane_path_fail_rpt().rmt_gtpu_peer(idx).dst_intfc_fld((PFCP_R15::DestinationInterfaceEnum)(ipeer + 1));
            }

            return std::unique_ptr<PFCP::AppMsg>(msg);
         };

         return WrapperTest(test, buildAppMsg);
      }

      TEST(wrapper_pfcp_node_report_rsp)
      {
         auto buildAppMsg = [](PFCP::LocalNodeSPtr ln, PFCP::RemoteNodeSPtr rn) {
            PFCP_R15::NodeReportRsp *msg = new PFCP_R15::NodeReportRsp();

            ESocket::Address addr_ipv4("1.2.3.4", 5);
            msg->node_id().node_id_value(addr_ipv4);

            msg->cause().cause(PFCP_R15::CauseEnum::MandatoryIeMissing);

            msg->offending_ie().type_of_the_offending_ie(PFCP_IE_NODE_RPT_TYPE);

            PFCP::AppMsgNodeReqPtr dummyReq = new PFCP::AppMsgNodeReq();
            msg->setReq(dummyReq);

            return std::unique_ptr<PFCP::AppMsg>(msg);
         };

         return WrapperTest(test, buildAppMsg);
      }

      TEST(wrapper_pfcp_session_set_deletion_req)
      {
         auto buildAppMsg = [](PFCP::LocalNodeSPtr ln, PFCP::RemoteNodeSPtr rn) {
            PFCP_R15::SessionSetDeletionReq *msg = new PFCP_R15::SessionSetDeletionReq(ln, rn);

            ESocket::Address addr_ipv4("1.2.3.4", 5);
            ESocket::Address addr_ipv6("1111:2222:3333:4444:5555:6666:7777:8888", 9999);
            msg->node_id().node_id_value(addr_ipv4);

            uint8_t csidCount = 3;

            msg->sgw_c_fqcsid().node_address(addr_ipv6);
            msg->pgw_c_fqcsid().node_address(addr_ipv6);
            msg->up_fqcsid().node_address(addr_ipv6);
            msg->twan_fqcsid().node_address(addr_ipv6);
            msg->epdg_fqcsid().node_address(addr_ipv6);
            msg->mme_fqcsid().node_address(addr_ipv6);
            
            for (uint8_t idx = 0u; idx < csidCount; ++idx)
            {
               msg->sgw_c_fqcsid().next_pdn_conn_set_ident();
               msg->pgw_c_fqcsid().next_pdn_conn_set_ident();
               msg->up_fqcsid().next_pdn_conn_set_ident();
               msg->twan_fqcsid().next_pdn_conn_set_ident();
               msg->epdg_fqcsid().next_pdn_conn_set_ident();
               msg->mme_fqcsid().next_pdn_conn_set_ident();
               
               msg->sgw_c_fqcsid().pdn_conn_set_ident(idx) = idx + 1;
               msg->pgw_c_fqcsid().pdn_conn_set_ident(idx) = idx + 1;
               msg->up_fqcsid().pdn_conn_set_ident(idx) = idx + 1;
               msg->twan_fqcsid().pdn_conn_set_ident(idx) = idx + 1;
               msg->epdg_fqcsid().pdn_conn_set_ident(idx) = idx + 1;
               msg->mme_fqcsid().pdn_conn_set_ident(idx) = idx + 1;
            }

            return std::unique_ptr<PFCP::AppMsg>(msg);
         };

         return WrapperTest(test, buildAppMsg);
      }

      TEST(wrapper_pfcp_session_set_deletion_rsp)
      {
         auto buildAppMsg = [](PFCP::LocalNodeSPtr ln, PFCP::RemoteNodeSPtr rn) {
            PFCP_R15::SessionSetDeletionRsp *msg = new PFCP_R15::SessionSetDeletionRsp();

            ESocket::Address addr_ipv4("1.2.3.4", 5);
            msg->node_id().node_id_value(addr_ipv4);

            msg->cause().cause(PFCP_R15::CauseEnum::MandatoryIeMissing);

            msg->offending_ie().type_of_the_offending_ie(PFCP_IE_NODE_ID);

            PFCP::AppMsgNodeReqPtr dummyReq = new PFCP::AppMsgNodeReq();
            msg->setReq(dummyReq);

            return std::unique_ptr<PFCP::AppMsg>(msg);
         };

         return WrapperTest(test, buildAppMsg);
      }
   } // namespace wrapper
} // namespace PFCPTest