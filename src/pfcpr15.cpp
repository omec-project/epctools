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
#include <endian.h>
#include "pfcpr15.h"
#include "pfcpr15inl.h"
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
   req->recoveryTimeStamp().rcvry_time_stmp_val(ro->localNode()->startTime());
   req->encode(data());

   ro->setAppMsg(req);
   ro->assign(data(), req->length());

   return ro;
}

PFCP::RspOutPtr Translator::encodeHeartbeatRsp(PFCP::SndHeartbeatRspData &hb)
{
   PFCP::RspOutPtr ro = new PFCP::RspOut();
   ro->setLocalNode(hb.req().localNode());
   ro->setRemoteNode(hb.req().remoteNode());
   ro->setSeqNbr(hb.req().seqNbr());

   HeartbeatRsp *rsp = new HeartbeatRsp();
   rsp->setReq(&hb.req());
   rsp->recoveryTimeStamp().rcvry_time_stmp_val(ro->localNode()->startTime());
   rsp->encode(data());

   ro->setRsp(rsp);
   ro->assign(data(), rsp->length());

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
   ro->setMsgClass(req->msgClass());
   ro->setAppMsg(req);

   switch (req->msgType())
   {
      case PFCP_HRTBEAT_REQ:
      {
         HeartbeatReq *am = static_cast<HeartbeatReq*>(req);
         am->encode(data());         
         ro->assign(data(), am->length());
         break;
      }
      case PFCP_PFD_MGMT_REQ:
      {
         PfdMgmtReq *am = static_cast<PfdMgmtReq*>(req);
         am->encode(data());
         ro->assign(data(), am->length());
         break;
      }
      case PFCP_ASSN_SETUP_REQ:
      {
         AssnSetupReq *am = static_cast<AssnSetupReq*>(req);
         am->encode(data());
         ro->assign(data(), am->length());
         break;
      }
      case PFCP_ASSN_UPD_REQ:
      {
         AssnUpdateReq *am = static_cast<AssnUpdateReq*>(req);
         am->encode(data());
         ro->assign(data(), am->length());
         break;
      }
      case PFCP_ASSN_REL_REQ:
      {
         AssnReleaseReq *am = static_cast<AssnReleaseReq*>(req);
         am->encode(data());
         ro->assign(data(), am->length());
         break;
      }
      case PFCP_NODE_RPT_REQ:
      {
         NodeReportReq *am = static_cast<NodeReportReq*>(req);
         am->encode(data());
         ro->assign(data(), am->length());
         break;
      }
      case PFCP_SESS_SET_DEL_REQ:
      {
         SessionSetDeletionReq *am = static_cast<SessionSetDeletionReq*>(req);
         am->encode(data());
         ro->assign(data(), am->length());
         break;
      }
      case PFCP_SESS_ESTAB_REQ:
      {
         SessionEstablishmentReq *am = static_cast<SessionEstablishmentReq*>(req);
         am->encode(data());
         ro->assign(data(), am->length());
         break;
      }
      case PFCP_SESS_MOD_REQ:
      {
         SessionModificationReq *am = static_cast<SessionModificationReq*>(req);
         am->encode(data());
         ro->assign(data(), am->length());
         break;
      }
      case PFCP_SESS_DEL_REQ:
      {
         SessionDeletionReq *am = static_cast<SessionDeletionReq*>(req);
         am->encode(data());
         ro->assign(data(), am->length());
         break;
      }
      case PFCP_SESS_RPT_REQ:
      {
         SessionReportReq *am = static_cast<SessionReportReq*>(req);
         am->encode(data());
         ro->assign(data(), am->length());
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

   return ro;
}

PFCP::RspOutPtr Translator::encodeRsp(PFCP::AppMsgRspPtr rsp)
{
   static EString __method__ = __METHOD_NAME__;

   std::memset(data(), 0, ESocket::UPD_MAX_MSG_LENGTH);

   PFCP::RspOutPtr ro = new PFCP::RspOut();
   ro->setLocalNode(rsp->localNode());
   ro->setRemoteNode(rsp->remoteNode());
   ro->setSeqNbr(rsp->req()->seqNbr());
   ro->setMsgType(rsp->msgType());
   ro->setMsgClass(rsp->msgClass());
   ro->setRsp(rsp);

   switch (rsp->msgType())
   {
      case PFCP_HRTBEAT_RSP:
      {
         HeartbeatRsp *am = static_cast<HeartbeatRsp*>(rsp);
         am->encode(data());
         ro->assign(data(), am->length());
         break;
      }
      case PFCP_PFD_MGMT_RSP:
      {
         PfdMgmtRsp *am = static_cast<PfdMgmtRsp*>(rsp);
         am->encode(data());
         ro->assign(data(), am->length());
         break;
      }
      case PFCP_ASSN_SETUP_RSP:
      {
         AssnSetupRsp *am = static_cast<AssnSetupRsp*>(rsp);
         am->encode(data());
         ro->assign(data(), am->length());
         break;
      }
      case PFCP_ASSN_UPD_RSP:
      {
         AssnUpdateRsp *am = static_cast<AssnUpdateRsp*>(rsp);
         am->encode(data());
         ro->assign(data(), am->length());
         break;
      }
      case PFCP_ASSN_REL_RSP:
      {
         AssnUpdateRsp *am = static_cast<AssnUpdateRsp*>(rsp);
         am->encode(data());
         ro->assign(data(), am->length());
         break;
      }
      case PFCP_NODE_RPT_RSP:
      {
         NodeReportRsp *am = static_cast<NodeReportRsp*>(rsp);
         am->encode(data());
         ro->assign(data(), am->length());
         break;
      }
      case PFCP_SESS_SET_DEL_RSP:
      {
         SessionSetDeletionRsp *am = static_cast<SessionSetDeletionRsp*>(rsp);
         am->encode(data());
         ro->assign(data(), am->length());
         break;
      }
      case PFCP_SESS_ESTAB_RSP:
      {
         SessionEstablishmentRsp *am = static_cast<SessionEstablishmentRsp*>(rsp);
         am->encode(data());
         ro->assign(data(), am->length());
         break;
      }
      case PFCP_SESS_MOD_RSP:
      {
         SessionModificationRsp *am = static_cast<SessionModificationRsp*>(rsp);
         am->encode(data());
         ro->assign(data(), am->length());
         break;
      }
      case PFCP_SESS_DEL_RSP:
      {
         SessionDeletionRsp *am = static_cast<SessionDeletionRsp*>(rsp);
         am->encode(data());
         ro->assign(data(), am->length());
         break;
      }
      case PFCP_SESS_RPT_RSP:
      {
         SessionReportRsp *am = static_cast<SessionReportRsp*>(rsp);
         am->encode(data());
         ro->assign(data(), am->length());
         break;
      }
      default:
      {
         PFCP::Configuration().logger().minor("{} - Unsupported response message type {}", rsp->msgType());
         delete ro;
         throw PFCP::EncodeRspException();
      }
   }

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
         if (tmp->rcvry_time_stmp().present())
            req->remoteStartTime(tmp->rcvry_time_stmp().rcvry_time_stmp_val());
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
         SessionEstablishmentReq *tmp = new SessionEstablishmentReq(req->session());
         decode_pfcp_sess_estab_req_t((pUChar)req->data(), &tmp->data());
         if (tmp->cp_fseid().present())
            req->remoteSeid(tmp->cp_fseid().seid());
         am = tmp;
         break;
      }
      case PFCP_SESS_MOD_REQ:
      {
         SessionModificationReq *tmp = new SessionModificationReq(req->session());
         decode_pfcp_sess_mod_req_t((pUChar)req->data(), &tmp->data());
         am = tmp;
         break;
      }
      case PFCP_SESS_DEL_REQ:
      {
         SessionDeletionReq *tmp = new SessionDeletionReq(req->session());
         decode_pfcp_sess_del_req_t((pUChar)req->data(), &tmp->data());
         am = tmp;
         break;
      }
      case PFCP_SESS_RPT_REQ:
      {
         SessionReportReq *tmp = new SessionReportReq(req->session());
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

   // assign the message type and sequence number
   if (am != nullptr)
      am->setSeqNbr(req->seqNbr());

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
         if (tmp->rcvry_time_stmp().present())
            rsp->remoteStartTime(tmp->rcvry_time_stmp().rcvry_time_stmp_val());
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
         decode_pfcp_sess_estab_rsp_t((pUChar)rsp->data(), &tmp->data());
         if (tmp->up_fseid().present())
            rsp->remoteSeid(tmp->up_fseid().seid());
         am = tmp;
         break;
      }
      case PFCP_SESS_MOD_RSP:
      {
         SessionModificationRsp *tmp = new SessionModificationRsp();
         decode_pfcp_sess_mod_rsp_t((pUChar)rsp->data(), &tmp->data());
         am = tmp;
         break;
      }
      case PFCP_SESS_DEL_RSP:
      {
         SessionDeletionRsp *tmp = new SessionDeletionRsp();
         decode_pfcp_sess_del_rsp_t((pUChar)rsp->data(), &tmp->data());
         am = tmp;
         break;
      }
      case PFCP_SESS_RPT_RSP:
      {
         SessionReportRsp *tmp = new SessionReportRsp();
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
      am->setSeqNbr(rsp->seqNbr());
   }

   return am;
}

PFCP::RcvdHeartbeatReqDataPtr Translator::decodeHeartbeatReq(PFCP::ReqInPtr msg)
{
   PFCP::RcvdHeartbeatReqDataPtr hb = new PFCP::RcvdHeartbeatReqData();
   PFCP_R15::HeartbeatReq *am = new PFCP_R15::HeartbeatReq(msg->localNode(), msg->remoteNode());

   am->setSeqNbr(msg->seqNbr());

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
   hb->setReq(static_cast<PFCP::AppMsgNodeReqPtr>(msg->req()));

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
   info.setMsgClass(messageClass(header->message_type));
   info.setSeid(header->s ? be64toh(header->seid_seqno.has_seid.seid) : 0);

   ULong sn = be32toh(header->s ? header->seid_seqno.has_seid.seq_no : header->seid_seqno.no_seid.seq_no) >> 8;
   info.setSeqNbr(sn);

   switch (info.msgType())
   {
      case PFCP_SESS_ESTAB_REQ:
      {
         info.setReq(True);
         info.setCreateSession(True);
         break;
      }
      case PFCP_HRTBEAT_REQ:
      case PFCP_PFD_MGMT_REQ:
      case PFCP_ASSN_SETUP_REQ:
      case PFCP_ASSN_UPD_REQ:
      case PFCP_ASSN_REL_REQ:
      case PFCP_NODE_RPT_REQ:
      case PFCP_SESS_SET_DEL_REQ:
      case PFCP_SESS_MOD_REQ:
      case PFCP_SESS_DEL_REQ:
      case PFCP_SESS_RPT_REQ:
      {
         info.setReq(True);
         info.setCreateSession(False);
         break;
      }
      default:
      {
         info.setReq(False);
         info.setCreateSession(False);
         break;
      }
   }
}

Bool Translator::isVersionSupported(UChar ver)
{
   return ver == 1;
}

PFCP::MsgClass Translator::messageClass(PFCP::MsgType mt)
{
   PFCP::MsgClass mc;

   switch (mt)
   {
      case PFCP_HRTBEAT_REQ:
      case PFCP_HRTBEAT_RSP:
      case PFCP_PFD_MGMT_REQ:
      case PFCP_PFD_MGMT_RSP:
      case PFCP_ASSN_SETUP_REQ:
      case PFCP_ASSN_SETUP_RSP:
      case PFCP_ASSN_UPD_REQ:
      case PFCP_ASSN_UPD_RSP:
      case PFCP_ASSN_REL_REQ:
      case PFCP_ASSN_REL_RSP:
      case PFCP_VERSION_NOT_SUPPORTED:
      case PFCP_NODE_RPT_REQ:
      case PFCP_NODE_RPT_RSP:
      case PFCP_SESS_SET_DEL_REQ:
      case PFCP_SESS_SET_DEL_RSP:
      {
         mc = PFCP::MsgClass::Node;
         break;
      }
      case PFCP_SESS_ESTAB_REQ:
      case PFCP_SESS_ESTAB_RSP:
      case PFCP_SESS_MOD_REQ:
      case PFCP_SESS_MOD_RSP:
      case PFCP_SESS_DEL_REQ:
      case PFCP_SESS_DEL_RSP:
      case PFCP_SESS_RPT_REQ:
      case PFCP_SESS_RPT_RSP:
      {
         mc = PFCP::MsgClass::Session;
         break;
      }
      default:
      {
         mc = PFCP::MsgClass::Unknown;
         break;
      }
   }

   return mc;
}

PFCP::MsgType Translator::pfcpHeartbeatReq() { return PFCP_HRTBEAT_REQ; }
PFCP::MsgType Translator::pfcpHeartbeatRsp() { return PFCP_HRTBEAT_RSP; }
PFCP::MsgType Translator::pfcpSessionEstablishmentReq() { return PFCP_SESS_ESTAB_REQ; }
PFCP::MsgType Translator::pfcpSessionEstablishmentRsp() { return PFCP_SESS_ESTAB_RSP; }
PFCP::MsgType Translator::pfcpAssociationSetupReq() { return PFCP_ASSN_SETUP_REQ; }
PFCP::MsgType Translator::pfcpAssociationSetupRsp() { return PFCP_ASSN_SETUP_RSP; }

} // namespace PFCP_R15
