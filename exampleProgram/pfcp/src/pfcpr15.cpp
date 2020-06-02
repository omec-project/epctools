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

#include <arpa/inet.h>
#include "pfcpr15.h"
#include "pfcp_messages_encoder.h"
#include "pfcp_messages_decoder.h"

namespace PFCP_R15
{

Translator::Translator()
{
}

Translator::~Translator()
{
}

PFCP::ReqOutPtr Translator::encodeHeartbeatReq(PFCP::SndHeartbeatReqData &hb)
{
   PFCP::ReqOutPtr ro = new PFCP::ReqOut();
   ro->setLocalNode(hb.localNode());
   ro->setRemoteNode(hb.remoteNode());
   ro->setN1(PFCP::Configuration::heartbeatN1());
   ro->setT1(PFCP::Configuration::heartbeatT1());
   ro->setSeqNbr(hb.localNode()->allocSeqNbr());

   HeartbeatReq *req = new HeartbeatReq(ro->localNode(), ro->remoteNode());
   req->setSeqNbr(ro->seqNbr());
   req->data().header.version = 1;
   req->data().header.message_type = req->msgType();
   req->data().header.seid_seqno.no_seid.seq_no = req->seqNbr();

   req->data().rcvry_time_stmp.header.type = PFCP_IE_RCVRY_TIME_STMP;
   req->data().rcvry_time_stmp.header.len = sizeof(req->data().rcvry_time_stmp.rcvry_time_stmp_val);
   req->data().rcvry_time_stmp.rcvry_time_stmp_val = ro->localNode()->startTime().getNTPTimeSeconds();

   ro->setMsgType(req->msgType());
   ro->setAppMsg(req);

   UShort len = encode_pfcp_hrtbeat_req_t(&req->data(), data());
   reinterpret_cast<pfcp_header_t*>(data())->message_len = htons(len - 4);

   ro->assign(data(), len);

   return ro;
}

PFCP::RspOutPtr Translator::encodeHeartbeatRsp(PFCP::SndHeartbeatRspData &hb)
{
   PFCP::RspOutPtr ro = new PFCP::RspOut();
   ro->setLocalNode(hb.req().localNode());
   ro->setRemoteNode(hb.req().remoteNode());
   ro->setSeqNbr(hb.req().seqNbr());

   HeartbeatRsp *rsp = new HeartbeatRsp();
   rsp->setSeqNbr(ro->seqNbr());
   rsp->data().header.version = 1;
   rsp->data().header.message_type = rsp->msgType();
   rsp->data().header.seid_seqno.no_seid.seq_no = ro->seqNbr();

   rsp->data().rcvry_time_stmp.header.type = PFCP_IE_RCVRY_TIME_STMP;
   rsp->data().rcvry_time_stmp.header.len = sizeof(rsp->data().rcvry_time_stmp.rcvry_time_stmp_val);
   rsp->data().rcvry_time_stmp.rcvry_time_stmp_val = ro->localNode()->startTime().getNTPTimeSeconds();

   ro->setMsgType(rsp->msgType());
   ro->setRsp(rsp);

   UShort len = encode_pfcp_hrtbeat_rsp_t(&rsp->data(), data());
   reinterpret_cast<pfcp_header_t*>(data())->message_len = htons(len - 4);

   ro->assign(data(), len);

   return ro;
}

PFCP::RspOutPtr Translator::encodeVersionNotSupportedRsp(PFCP::ReqInPtr msg)
{
   PFCP::RspOutPtr ro = new PFCP::RspOut();
   ro->setLocalNode(msg->localNode());
   ro->setRemoteNode(msg->remoteNode());
   ro->setSeqNbr(msg->seqNbr());

   VersionNotSupportedRsp *rsp = new VersionNotSupportedRsp();
   rsp->setSeqNbr(ro->seqNbr());
   rsp->data().version = 1;
   rsp->data().message_type = rsp->msgType();
   rsp->data().seid_seqno.no_seid.seq_no = ro->seqNbr();

   ro->setMsgType(rsp->msgType());
   ro->setRsp(rsp);

   UShort len = encode_pfcp_header_t(&rsp->data(), data());
   reinterpret_cast<pfcp_header_t*>(data())->message_len = htons(len - 4);

   ro->assign(data(), len);

   return ro;
}

PFCP::ReqOutPtr Translator::encodeReq(PFCP::AppMsgReqPtr req)
{
   static EString __method__ = __METHOD_NAME__;
   UShort len = 0;

   PFCP::ReqOutPtr ro = new PFCP::ReqOut();
   ro->setLocalNode(req->localNode());
   ro->setRemoteNode(req->remoteNode());
   ro->setN1(PFCP::Configuration::n1());
   ro->setT1(PFCP::Configuration::t1());
   ro->setSeqNbr(req->seqNbr());
   ro->setMsgType(req->msgType());
   ro->setAppMsg(req);

   switch (req->msgType())
   {
      case PFCP_PFD_MGMT_REQ:
      {
         PfdMgmtReq *am = static_cast<PfdMgmtReq*>(req);
         am->data().header.version = 1;
         am->data().header.message_type = req->msgType();
         am->data().header.seid_seqno.no_seid.seq_no = req->seqNbr();
         len = encode_pfcp_pfd_mgmt_req_t(&am->data(), data());
         break;
      }
      case PFCP_ASSN_SETUP_REQ:
      {
         AssnSetupReq *am = static_cast<AssnSetupReq*>(req);
         am->data().header.version = 1;
         am->data().header.message_type = req->msgType();
         am->data().header.seid_seqno.no_seid.seq_no = req->seqNbr();
         len = encode_pfcp_assn_setup_req_t(&am->data(), data());
         break;
      }
      case PFCP_ASSN_UPD_REQ:
      {
         AssnUpdateReq *am = static_cast<AssnUpdateReq*>(req);
         am->data().header.version = 1;
         am->data().header.message_type = req->msgType();
         am->data().header.seid_seqno.no_seid.seq_no = req->seqNbr();
         len = encode_pfcp_assn_upd_req_t(&am->data(), data());
         break;
      }
      case PFCP_ASSN_REL_REQ:
      {
         AssnReleaseReq *am = static_cast<AssnReleaseReq*>(req);
         am->data().header.version = 1;
         am->data().header.message_type = req->msgType();
         am->data().header.seid_seqno.no_seid.seq_no = req->seqNbr();
         len = encode_pfcp_assn_rel_req_t(&am->data(), data());
         break;
      }
      case PFCP_NODE_RPT_REQ:
      {
         NodeReportReq *am = static_cast<NodeReportReq*>(req);
         am->data().header.version = 1;
         am->data().header.message_type = req->msgType();
         am->data().header.seid_seqno.no_seid.seq_no = req->seqNbr();
         len = encode_pfcp_node_rpt_req_t(&am->data(), data());
         break;
      }
      case PFCP_SESS_SET_DEL_REQ:
      {
         SessionSetDeletionReq *am = static_cast<SessionSetDeletionReq*>(req);
         am->data().header.version = 1;
         am->data().header.message_type = req->msgType();
         am->data().header.seid_seqno.no_seid.seq_no = req->seqNbr();
         len = encode_pfcp_sess_set_del_req_t(&am->data(), data());
         break;
      }
      case PFCP_SESS_ESTAB_REQ:
      {
         SessionEstablishmentReq *am = static_cast<SessionEstablishmentReq*>(req);
         am->data().header.version = 1;
         am->data().header.s = 1;
         am->data().header.message_type = req->msgType();
         // force a '0' SEID for the remote SEID since the remote has not seen the session yet
         req->setSeid(0);
         am->data().header.seid_seqno.has_seid.seid = req->seid();
         am->data().header.seid_seqno.has_seid.seq_no = req->seqNbr();
         len = encode_pfcp_sess_estab_req_t(&am->data(), data());
         break;
      }
      case PFCP_SESS_MOD_REQ:
      {
         SessionModificationReq *am = static_cast<SessionModificationReq*>(req);
         am->data().header.version = 1;
         am->data().header.s = 1;
         am->data().header.message_type = req->msgType();
         am->data().header.seid_seqno.has_seid.seid = req->seid();
         am->data().header.seid_seqno.has_seid.seq_no = req->seqNbr();
         len = encode_pfcp_sess_mod_req_t(&am->data(), data());
         break;
      }
      case PFCP_SESS_DEL_REQ:
      {
         SessionDeletionReq *am = static_cast<SessionDeletionReq*>(req);
         am->data().header.version = 1;
         am->data().header.s = 1;
         am->data().header.message_type = req->msgType();
         am->data().header.seid_seqno.has_seid.seid = req->seid();
         am->data().header.seid_seqno.has_seid.seq_no = req->seqNbr();
         len = encode_pfcp_sess_del_req_t(&am->data(), data());
         break;
      }
      case PFCP_SESS_RPT_REQ:
      {
         SessionReportReq *am = static_cast<SessionReportReq*>(req);
         am->data().header.version = 1;
         am->data().header.s = 1;
         am->data().header.message_type = req->msgType();
         am->data().header.seid_seqno.has_seid.seid = req->seid();
         am->data().header.seid_seqno.has_seid.seq_no = req->seqNbr();
         len = encode_pfcp_sess_rpt_req_t(&am->data(), data());
         break;
      }
      default:
      {
         PFCP::Configuration().logger().minor("{} - Unsupported request message type {}", req->msgType());
         delete ro;
         throw PFCP::EncodeReqException();
      }
   }

   // update the header length
   reinterpret_cast<pfcp_header_t*>(data())->message_len = htons(len - 4);

   // copy the data to the output request object
   ro->assign(data(), len);

   return ro;
}

PFCP::RspOutPtr Translator::encodeRsp(PFCP::AppMsgRspPtr rsp)
{
   static EString __method__ = __METHOD_NAME__;
   UShort len = 0;

   std::memset(data(), 0, ESocket::UPD_MAX_MSG_LENGTH);

   PFCP::RspOutPtr ro = new PFCP::RspOut();
   ro->setLocalNode(rsp->localNode());
   ro->setRemoteNode(rsp->remoteNode());
   ro->setSeqNbr(rsp->req()->seqNbr());
   ro->setMsgType(rsp->msgType());
   ro->setRsp(rsp);

   switch (rsp->msgType())
   {
      case PFCP_PFD_MGMT_RSP:
      {
         PfdMgmtRsp *am = static_cast<PfdMgmtRsp*>(rsp);
         am->data().header.version = 1;
         am->data().header.message_type = rsp->msgType();
         am->data().header.seid_seqno.no_seid.seq_no = rsp->seqNbr();
         len = encode_pfcp_pfd_mgmt_rsp_t(&am->data(), data());
         break;
      }
      case PFCP_ASSN_SETUP_RSP:
      {
         AssnSetupRsp *am = static_cast<AssnSetupRsp*>(rsp);
         am->data().header.version = 1;
         am->data().header.message_type = rsp->msgType();
         am->data().header.seid_seqno.no_seid.seq_no = rsp->seqNbr();
         len = encode_pfcp_assn_setup_rsp_t(&am->data(), data());
         break;
      }
      case PFCP_ASSN_UPD_RSP:
      {
         AssnUpdateRsp *am = static_cast<AssnUpdateRsp*>(rsp);
         am->data().header.version = 1;
         am->data().header.message_type = rsp->msgType();
         am->data().header.seid_seqno.no_seid.seq_no = rsp->seqNbr();
         len = encode_pfcp_assn_upd_rsp_t(&am->data(), data());
         break;
      }
      case PFCP_ASSN_REL_RSP:
      {
         AssnUpdateRsp *am = static_cast<AssnUpdateRsp*>(rsp);
         am->data().header.version = 1;
         am->data().header.message_type = rsp->msgType();
         am->data().header.seid_seqno.no_seid.seq_no = rsp->seqNbr();
         len = encode_pfcp_assn_upd_rsp_t(&am->data(), data());
         break;
      }
      case PFCP_NODE_RPT_RSP:
      {
         NodeReportRsp *am = static_cast<NodeReportRsp*>(rsp);
         am->data().header.version = 1;
         am->data().header.message_type = rsp->msgType();
         am->data().header.seid_seqno.no_seid.seq_no = rsp->seqNbr();
         len = encode_pfcp_node_rpt_rsp_t(&am->data(), data());
         break;
      }
      case PFCP_SESS_SET_DEL_RSP:
      {
         SessionSetDeletionRsp *am = static_cast<SessionSetDeletionRsp*>(rsp);
         am->data().header.version = 1;
         am->data().header.message_type = rsp->msgType();
         am->data().header.seid_seqno.no_seid.seq_no = rsp->seqNbr();
         len = encode_pfcp_sess_set_del_rsp_t(&am->data(), data());
         break;
      }
      case PFCP_SESS_ESTAB_RSP:
      {
         SessionEstablishmentRsp *am = static_cast<SessionEstablishmentRsp*>(rsp);
         am->data().header.version = 1;
         am->data().header.s = 1;
         am->data().header.message_type = rsp->msgType();
         am->data().header.seid_seqno.has_seid.seid = rsp->seid();
         am->data().header.seid_seqno.has_seid.seq_no = rsp->seqNbr();
         len = encode_pfcp_sess_estab_rsp_t(&am->data(), data());
         break;
      }
      case PFCP_SESS_MOD_RSP:
      {
         SessionModificationRsp *am = static_cast<SessionModificationRsp*>(rsp);
         am->data().header.version = 1;
         am->data().header.s = 1;
         am->data().header.message_type = rsp->msgType();
         am->data().header.seid_seqno.has_seid.seid = rsp->seid();
         am->data().header.seid_seqno.has_seid.seq_no = rsp->seqNbr();
         len = encode_pfcp_sess_mod_rsp_t(&am->data(), data());
         break;
      }
      case PFCP_SESS_DEL_RSP:
      {
         SessionDeletionRsp *am = static_cast<SessionDeletionRsp*>(rsp);
         am->data().header.version = 1;
         am->data().header.s = 1;
         am->data().header.message_type = rsp->msgType();
         am->data().header.seid_seqno.has_seid.seid = rsp->seid();
         am->data().header.seid_seqno.has_seid.seq_no = rsp->seqNbr();
         len = encode_pfcp_sess_del_rsp_t(&am->data(), data());
         break;
      }
      case PFCP_SESS_RPT_RSP:
      {
         SessionReportRsp *am = static_cast<SessionReportRsp*>(rsp);
         am->data().header.version = 1;
         am->data().header.s = 1;
         am->data().header.message_type = rsp->msgType();
         am->data().header.seid_seqno.has_seid.seid = rsp->seid();
         am->data().header.seid_seqno.has_seid.seq_no = rsp->seqNbr();
         len = encode_pfcp_sess_rpt_rsp_t(&am->data(), data());
         break;
      }
      default:
      {
         PFCP::Configuration().logger().minor("{} - Unsupported response message type {}", rsp->msgType());
         delete ro;
         throw PFCP::EncodeRspException();
      }
   }

   // update the header length
   reinterpret_cast<pfcp_header_t*>(data())->message_len = htons(len - 4);

   // copy the data to the output request object
   ro->assign(data(), len);

   return ro;
}

PFCP::AppMsgReqPtr Translator::decodeReq(PFCP::ReqInPtr req)
{
   PFCP::AppMsgReqPtr am = nullptr;

   switch (req->msgType())
   {
      case PFCP_PFD_MGMT_REQ:
      {
         PfdMgmtReq *tmp = new PfdMgmtReq(req->localNode(), req->remoteNode());
         decode_pfcp_pfd_mgmt_req_t((pUChar)req->data(), &tmp->data());
         am = tmp;
         break;
      }
      case PFCP_ASSN_SETUP_REQ:
      {
         AssnSetupReq *tmp = new AssnSetupReq(req->localNode(), req->remoteNode());
         decode_pfcp_assn_setup_req_t((pUChar)req->data(), &tmp->data());
         am = tmp;
         break;
      }
      case PFCP_ASSN_UPD_REQ:
      {
         AssnUpdateReq *tmp = new AssnUpdateReq(req->localNode(), req->remoteNode());
         decode_pfcp_assn_upd_req_t((pUChar)req->data(), &tmp->data());
         am = tmp;
         break;
      }
      case PFCP_ASSN_REL_REQ:
      {
         AssnReleaseReq *tmp = new AssnReleaseReq(req->localNode(), req->remoteNode());
         decode_pfcp_assn_rel_req_t((pUChar)req->data(), &tmp->data());
         am = tmp;
         break;
      }
      case PFCP_NODE_RPT_REQ:
      {
         NodeReportReq *tmp = new NodeReportReq(req->localNode(), req->remoteNode());
         decode_pfcp_node_rpt_req_t((pUChar)req->data(), &tmp->data());
         am = tmp;
         break;
      }
      case PFCP_SESS_SET_DEL_REQ:
      {
         SessionSetDeletionReq *tmp = new SessionSetDeletionReq(req->localNode(), req->remoteNode());
         decode_pfcp_sess_set_del_req_t((pUChar)req->data(), &tmp->data());
         am = tmp;
         break;
      }
      case PFCP_SESS_ESTAB_REQ:
      {
         SessionEstablishmentReq *tmp = new SessionEstablishmentReq(req->localNode(), req->remoteNode());
         tmp->setSeid(req->seid());
         decode_pfcp_sess_estab_req_t((pUChar)req->data(), &tmp->data());
         am = tmp;
         break;
      }
      case PFCP_SESS_MOD_REQ:
      {
         SessionModificationReq *tmp = new SessionModificationReq(req->localNode(), req->remoteNode());
         tmp->setSeid(req->seid());
         decode_pfcp_sess_mod_req_t((pUChar)req->data(), &tmp->data());
         am = tmp;
         break;
      }
      case PFCP_SESS_DEL_REQ:
      {
         SessionDeletionReq *tmp = new SessionDeletionReq(req->localNode(), req->remoteNode());
         tmp->setSeid(req->seid());
         decode_pfcp_sess_del_req_t((pUChar)req->data(), &tmp->data());
         am = tmp;
         break;
      }
      case PFCP_SESS_RPT_REQ:
      {
         SessionReportReq *tmp = new SessionReportReq(req->localNode(), req->remoteNode());
         tmp->setSeid(req->seid());
         decode_pfcp_sess_rpt_req_t((pUChar)req->data(), &tmp->data());
         am = tmp;
         break;
      }
      default:
      {
         PFCP::Configuration().logger().minor("{} - Unsupported request message type {}", req->msgType());
         break;
      }
   }

   if (am != nullptr)
   {
      // assign the message type and sequence number
      am->setMsgType(req->msgType());
      am->setSeqNbr(req->seqNbr());
   }

   return am;
}

PFCP::AppMsgRspPtr Translator::decodeRsp(PFCP::RspInPtr rsp)
{
   PFCP::AppMsgRspPtr am = nullptr;

   switch (rsp->msgType())
   {
      case PFCP_PFD_MGMT_RSP:
      {
         PfdMgmtRsp *tmp = new PfdMgmtRsp();
         decode_pfcp_pfd_mgmt_rsp_t((pUChar)rsp->data(), &tmp->data());
         am = tmp;
         break;
      }
      case PFCP_ASSN_SETUP_RSP:
      {
         AssnSetupRsp *tmp = new AssnSetupRsp();
         decode_pfcp_assn_setup_rsp_t((pUChar)rsp->data(), &tmp->data());
         am = tmp;
         break;
      }
      case PFCP_ASSN_UPD_RSP:
      {
         AssnUpdateRsp *tmp = new AssnUpdateRsp();
         decode_pfcp_assn_upd_rsp_t((pUChar)rsp->data(), &tmp->data());
         am = tmp;
         break;
      }
      case PFCP_ASSN_REL_RSP:
      {
         AssnReleaseRsp *tmp = new AssnReleaseRsp();
         decode_pfcp_assn_rel_rsp_t((pUChar)rsp->data(), &tmp->data());
         am = tmp;
         break;
      }
      case PFCP_VERSION_NOT_SUPPORTED:
      {
         VersionNotSupportedRsp *tmp = new VersionNotSupportedRsp();
         decode_pfcp_header_t((pUChar)rsp->data(), &tmp->data());
         am = tmp;
         break;
      }
      case PFCP_NODE_RPT_RSP:
      {
         NodeReportRsp *tmp = new NodeReportRsp();
         decode_pfcp_node_rpt_rsp_t((pUChar)rsp->data(), &tmp->data());
         am = tmp;
         break;
      }
      case PFCP_SESS_SET_DEL_RSP:
      {
         SessionSetDeletionRsp *tmp = new SessionSetDeletionRsp();
         decode_pfcp_sess_set_del_rsp_t((pUChar)rsp->data(), &tmp->data());
         am = tmp;
         break;
      }
      case PFCP_SESS_ESTAB_RSP:
      {
         SessionEstablishmentRsp *tmp = new SessionEstablishmentRsp();
         tmp->setSeid(rsp->seid());
         decode_pfcp_sess_estab_rsp_t((pUChar)rsp->data(), &tmp->data());
         am = tmp;
         break;
      }
      case PFCP_SESS_MOD_RSP:
      {
         SessionModificationRsp *tmp = new SessionModificationRsp();
         tmp->setSeid(rsp->seid());
         decode_pfcp_sess_mod_rsp_t((pUChar)rsp->data(), &tmp->data());
         am = tmp;
         break;
      }
      case PFCP_SESS_DEL_RSP:
      {
         SessionDeletionRsp *tmp = new SessionDeletionRsp();
         tmp->setSeid(rsp->seid());
         decode_pfcp_sess_del_rsp_t((pUChar)rsp->data(), &tmp->data());
         am = tmp;
         break;
      }
      case PFCP_SESS_RPT_RSP:
      {
         SessionReportRsp *tmp = new SessionReportRsp();
         tmp->setSeid(rsp->seid());
         decode_pfcp_sess_rpt_rsp_t((pUChar)rsp->data(), &tmp->data());
         am = tmp;
         break;
      }
      default:
      {
         PFCP::Configuration().logger().minor("{} - Unsupported response message type {}", rsp->msgType());
         break;
      }
   }

   if (am != nullptr)
   {
      // assign the request, message type and sequence number
      am->setReq(rsp->req());
      am->setMsgType(rsp->msgType());
      am->setSeqNbr(rsp->seqNbr());
   }

   return am;
}

PFCP::RcvdHeartbeatReqDataPtr Translator::decodeHeartbeatReq(PFCP::ReqInPtr msg)
{
   PFCP::RcvdHeartbeatReqDataPtr hb = new PFCP::RcvdHeartbeatReqData();
   PFCP_R15::HeartbeatReq *am = new PFCP_R15::HeartbeatReq(msg->localNode(), msg->remoteNode());

   am->setMsgType(msg->msgType());
   am->setSeqNbr(msg->seqNbr());
   am->setSeid(msg->seid());

   hb->setReq(am);

   decode_pfcp_hrtbeat_req_t((pUChar)msg->data(), &am->data());
   if (am->data().rcvry_time_stmp.header.len)
   {
      ETime ntptm;
      ntp_time_t ntp;
      ntp.second = am->data().rcvry_time_stmp.rcvry_time_stmp_val;
      ntp.fraction = 0;
      ntptm.setNTPTime(ntp);
      hb->setStartTime(ntptm);
   }
   else
   {
      hb->setStartTime(msg->remoteNode()->startTime());
   }

   return hb;
}

PFCP::RcvdHeartbeatRspDataPtr Translator::decodeHeartbeatRsp(PFCP::RspInPtr msg)
{
   PFCP::RcvdHeartbeatRspDataPtr hb = new PFCP::RcvdHeartbeatRspData();
   PFCP_R15::HeartbeatRsp rsp;
   hb->setReq(msg->req());

   decode_pfcp_hrtbeat_rsp_t((pUChar)msg->data(), &rsp.data());
   if (rsp.data().rcvry_time_stmp.header.len)
   {
      ETime ntptm;
      ntp_time_t ntp;
      ntp.second = rsp.data().rcvry_time_stmp.rcvry_time_stmp_val;
      ntp.fraction = 0;
      ntptm.setNTPTime(ntp);
      hb->setStartTime(ntptm);
   }
   else
   {
      hb->setStartTime(msg->remoteNode()->startTime());
   }

   return hb;
}

Void Translator::getMsgInfo(PFCP::TranslatorMsgInfo &info, cpUChar msg, Int len)
{
   const pfcp_header_t *header = reinterpret_cast<const pfcp_header_t*>(msg);
   info.setVersion(header->version);
   info.setMsgType(header->message_type);
   info.setSeid(header->s ? header->seid_seqno.has_seid.seid : 0);

   ULong sn = ntohl(header->s ? header->seid_seqno.has_seid.seq_no : header->seid_seqno.no_seid.seq_no) >> 8;
   info.setSeqNbr(sn);

   switch (info.msgType())
   {
      case PFCP_HRTBEAT_REQ:
      case PFCP_PFD_MGMT_REQ:
      case PFCP_ASSN_SETUP_REQ:
      case PFCP_ASSN_UPD_REQ:
      case PFCP_ASSN_REL_REQ:
      case PFCP_NODE_RPT_REQ:
      case PFCP_SESS_SET_DEL_REQ:
      case PFCP_SESS_ESTAB_REQ:
      case PFCP_SESS_MOD_REQ:
      case PFCP_SESS_DEL_REQ:
      case PFCP_SESS_RPT_REQ:
      {
         info.setReq(True);
         break;
      }
      default:
      {
         info.setReq(False);
         break;
      }
   }
}

Bool Translator::isVersionSupported(UChar ver)
{
   return ver == 1;
}

} // namespace PFCP_R15
