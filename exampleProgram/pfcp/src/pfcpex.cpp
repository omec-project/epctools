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

#include "pfcpex.h"

ExamplePfcpApplication *ExamplePfcpApplication::this_ = nullptr;

BEGIN_MESSAGE_MAP(ExamplePfcpApplication, PFCP::ApplicationThread)
END_MESSAGE_MAP()

ExamplePfcpApplication::ExamplePfcpApplication()
{
   this_ = this;
}

Void ExamplePfcpApplication::shutdown()
{
   PFCP::Uninitialize();
}

Void ExamplePfcpApplication::startup(EGetOpt &opt)
{
   lnip_ = opt.get("/PfcpExample/localIp", "127.0.0.1");
   rnip_ = opt.get("/PfcpExample/remoteIp", "127.0.0.1");
   sndAssnSetup_ = opt.get("/PfcpExample/sendAssnSetup", False);
   port_ = opt.get("/PfcpExample/PFCP/pfcpPort", 8805);

   PFCP::Configuration::setPort(opt.get("/PfcpExample/PFCP/pfcpPort", 8805));
   PFCP::Configuration::setSocketBufferSize(opt.get("/PfcpExample/PFCP/socketBufferSize", 2097152));
   PFCP::Configuration::setT1(opt.get("/PfcpExample/PFCP/T1",3000));
   PFCP::Configuration::setN1(opt.get("/PfcpExample/PFCP/N1",2));
   PFCP::Configuration::setHeartbeatT1(opt.get("/PfcpExample/PFCP/heartbeatT1",5000));
   PFCP::Configuration::setHeartbeatN1(opt.get("/PfcpExample/PFCP/heartbeatN1",3));
   PFCP::Configuration::setNnbrActivityWnds(opt.get("/PfcpExample/PFCP/nbrActivityWindows",10));
   PFCP::Configuration::setLenActivityWnd(opt.get("/PfcpExample/PFCP/lenActivityWindow",6000));
   PFCP::Configuration::setAssignTeidRange(opt.get("/PfcpExample/PFCP/assignTeidRange",False));
   PFCP::Configuration::setTeidRangeBits(opt.get("/PfcpExample/PFCP/nbrTeidRangeBits",0));

   PFCP::Configuration::setLogger(ELogger::log(LOG_PFCP));
   PFCP::Configuration::setTranslator(xlator_);

   PFCP::Initialize();

   init(1, 1, NULL, 100000);
}

Void ExamplePfcpApplication::sendAssociationSetupReq()
{
   static EString __method__ = __METHOD_NAME__;
   PFCP_R15::AssnSetupReq *req = new PFCP_R15::AssnSetupReq(ln_, rn_);
   Int ielen = sizeof(req->data().node_id) - sizeof(req->data().node_id.header) - sizeof(req->data().node_id.node_id_value);

   req->setSeqNbr(ln_->allocSeqNbr());
   req->data().header.version = 1;
   req->data().header.message_type = req->msgType();
   req->data().header.seid_seqno.no_seid.seq_no = req->seqNbr();

   // Node ID
   if (ln_->address().getFamily() == ESocket::Family::INET)
   {
      req->data().node_id.node_id_type = NODE_ID_TYPE_TYPE_IPV4ADDRESS;
      std::memcpy(&req->data().node_id.node_id_value,
         &ln_->address().getInet().sin_addr.s_addr,
         sizeof(ln_->address().getInet().sin_addr.s_addr));
      ielen += sizeof(ln_->address().getInet().sin_addr.s_addr);
   }
   else if (ln_->address().getFamily() == ESocket::Family::INET6)
   {
      req->data().node_id.node_id_type = NODE_ID_TYPE_TYPE_IPV6ADDRESS;
      std::memcpy(&req->data().node_id.node_id_value,
         ln_->address().getInet6().sin6_addr.s6_addr,
         sizeof(ln_->address().getInet6().sin6_addr.s6_addr));
      ielen += sizeof(ln_->address().getInet6().sin6_addr.s6_addr);
   }

   req->data().node_id.header.type = PFCP_IE_NODE_ID;
   req->data().node_id.header.len = ielen;

   // Recovery Time Stamp
   req->data().rcvry_time_stmp.header.type = PFCP_IE_RCVRY_TIME_STMP;
   req->data().rcvry_time_stmp.header.len = sizeof(req->data().rcvry_time_stmp.rcvry_time_stmp_val);
   req->data().rcvry_time_stmp.rcvry_time_stmp_val = ln_->startTime().getNTPTimeSeconds();

   // Send to the translation layer for encoding and subsquent transmission
   SEND_TO_TRANSLATION(SndMsg, req);
}

Void ExamplePfcpApplication::sendAssociationSetupRsp(PFCP_R15::AssnSetupReq *req)
{
   static EString __method__ = __METHOD_NAME__;
   PFCP_R15::AssnSetupRsp *rsp = new PFCP_R15::AssnSetupRsp();
   Int ielen = 0;

   ELogger::log(LOG_SYSTEM).debug(
      "{} - sending PFCP Association Setup Response local={} remote={}",
      __method__, req->localNode()->address().getAddress(), req->remoteNode()->address().getAddress());

   rsp->setReq(req);
   rsp->setSeqNbr(req->seqNbr());
   rsp->setSeid(req->seid());

   rsp->data().header.version = 1;
   rsp->data().header.message_type = rsp->msgType();
   rsp->data().header.seid_seqno.no_seid.seq_no = rsp->seqNbr();

   // Node ID
   ielen = sizeof(rsp->data().node_id) - sizeof(rsp->data().node_id.header) - sizeof(rsp->data().node_id.node_id_value);
   if (rsp->localNode()->address().getFamily() == ESocket::Family::INET)
   {
      rsp->data().node_id.node_id_type = NODE_ID_TYPE_TYPE_IPV4ADDRESS;
      std::memcpy(&rsp->data().node_id.node_id_value,
         &rsp->localNode()->address().getInet().sin_addr.s_addr,
         sizeof(rsp->localNode()->address().getInet().sin_addr.s_addr));
      ielen += sizeof(rsp->localNode()->address().getInet().sin_addr.s_addr);
   }
   else if (rsp->localNode()->address().getFamily() == ESocket::Family::INET6)
   {
      rsp->data().node_id.node_id_type = NODE_ID_TYPE_TYPE_IPV6ADDRESS;
      std::memcpy(&rsp->data().node_id.node_id_value,
         rsp->localNode()->address().getInet6().sin6_addr.s6_addr,
         sizeof(rsp->localNode()->address().getInet6().sin6_addr.s6_addr));
      ielen += sizeof(rsp->localNode()->address().getInet6().sin6_addr.s6_addr);
   }

   rsp->data().node_id.header.type = PFCP_IE_NODE_ID;
   rsp->data().node_id.header.len = ielen;

   // Cause
   ielen = sizeof(pfcp_cause_ie_t) - sizeof(rsp->data().cause.header);
   rsp->data().cause.header.type = PFCP_IE_CAUSE;
   rsp->data().cause.header.len = ielen;
   rsp->data().cause.cause_value = REQUESTACCEPTED;

   // Recovery Time Stamp
   rsp->data().rcvry_time_stmp.header.type = PFCP_IE_RCVRY_TIME_STMP;
   rsp->data().rcvry_time_stmp.header.len = sizeof(rsp->data().rcvry_time_stmp.rcvry_time_stmp_val);
   rsp->data().rcvry_time_stmp.rcvry_time_stmp_val = req->localNode()->startTime().getNTPTimeSeconds();

   // UP Function Features
   rsp->data().up_func_feat.header.type = PFCP_IE_UP_FUNC_FEAT;
   rsp->data().up_func_feat.header.len = sizeof(UShort);
   rsp->data().up_func_feat.sup_feat = 0;

   // User Plane IP Resource Information
   rsp->data().user_plane_ip_rsrc_info_count = 1;
   ielen = 1;
   if (PFCP::Configuration::assignTeidRange() && PFCP::Configuration::teidRangeBits() > 0)
   {
      rsp->data().user_plane_ip_rsrc_info[0].teidri = PFCP::Configuration::teidRangeBits();
      rsp->data().user_plane_ip_rsrc_info[0].teid_range = rsp->remoteNode()->teidRangeValue();
      ielen += 1;
   }

   rsp->data().user_plane_ip_rsrc_info[0].v4 = 1;
   ULong ipaddr = 0x01020304;
   rsp->data().user_plane_ip_rsrc_info[0].ipv4_address = htonl(ipaddr);
   ielen += sizeof(ipaddr);

   rsp->data().user_plane_ip_rsrc_info[0].assosi = 1;
   rsp->data().user_plane_ip_rsrc_info[0].src_intfc = 1;
   ielen += 1;

   rsp->data().user_plane_ip_rsrc_info[0].header.type = PFCP_IE_USER_PLANE_IP_RSRC_INFO;
   rsp->data().user_plane_ip_rsrc_info[0].header.len = ielen;

   // calculate the message header length
   rsp->data().header.message_len =
      sizeof(rsp->data().header.seid_seqno.no_seid) +
      rsp->data().node_id.header.len +
      rsp->data().rcvry_time_stmp.header.len +
      rsp->data().cause.header.len +
      rsp->data().user_plane_ip_rsrc_info[0].header.len;

   // Send to the translation layer for encoding and subsquent transmission
   SEND_TO_TRANSLATION(SndMsg, rsp);
}

Void ExamplePfcpApplication::onInit()
{
   static EString __method__ = __METHOD_NAME__;

   PFCP::ApplicationThread::onInit();

   ln_ = createLocalNode(lnip_.c_str(), port_);
   
   if (sndAssnSetup_)
   {
      rn_ = ln_->createRemoteNode(rnip_, port_);
      sendAssociationSetupReq();
   }
}

Void ExamplePfcpApplication::onQuit()
{
      static EString __method__ = __METHOD_NAME__;

   PFCP::Uninitialize();

   PFCP::ApplicationThread::onQuit();
}

Void ExamplePfcpApplication::onRcvdReq(PFCP::AppMsgReqPtr req)
{
   static EString __method__ = __METHOD_NAME__;
   ELogger::log(LOG_SYSTEM).debug(
      "{} - request received local={} remote={} msgType={} seqNbr={} seid={}",
      __method__, req->localNode()->address().getAddress(), req->remoteNode()->address().getAddress(),
      req->msgType(), req->seqNbr(), req->seid());

   switch (req->msgType())
   {
      case PFCP_PFD_MGMT_REQ:
      {
         // PFCP_R15::PfdMgmtReq *am = static_cast<PFCP_R15::PfdMgmtReq*>(req);
         break;
      }
      case PFCP_ASSN_SETUP_REQ:
      {
         PFCP_R15::AssnSetupReq *am = static_cast<PFCP_R15::AssnSetupReq*>(req);
         ELogger::log(LOG_SYSTEM).debug("{} - received PFCP_ASSN_SETUP_REQ", __method__);
         sendAssociationSetupRsp(am);
         break;
      }
      case PFCP_ASSN_UPD_REQ:
      {
         // PFCP_R15::AssnUpdateReq *am = static_cast<PFCP_R15::AssnUpdateReq*>(req);
         break;
      }
      case PFCP_ASSN_REL_REQ:
      {
         // PFCP_R15::AssnReleaseReq *am = static_cast<PFCP_R15::AssnReleaseReq*>(req);
         break;
      }
      case PFCP_NODE_RPT_REQ:
      {
         // PFCP_R15::NodeReportReq *am = static_cast<PFCP_R15::NodeReportReq*>(req);
         break;
      }
      case PFCP_SESS_SET_DEL_REQ:
      {
         // PFCP_R15::SessionSetDeletionReq *am = static_cast<PFCP_R15::SessionSetDeletionReq*>(req);
         break;
      }
      case PFCP_SESS_ESTAB_REQ:
      {
         // PFCP_R15::SessionEstablishmentReq *am = static_cast<PFCP_R15::SessionEstablishmentReq*>(req);
         break;
      }
      case PFCP_SESS_MOD_REQ:
      {
         // PFCP_R15::SessionModificationReq *am = static_cast<PFCP_R15::SessionModificationReq*>(req);
         break;
      }
      case PFCP_SESS_DEL_REQ:
      {
         // PFCP_R15::SessionDeletionReq *am = static_cast<PFCP_R15::SessionDeletionReq*>(req);
         break;
      }
      case PFCP_SESS_RPT_REQ:
      {
         // PFCP_R15::SessionReportReq *am = static_cast<PFCP_R15::SessionReportReq*>(req);
         break;
      }
      default:
      {
         PFCP::Configuration().logger().minor("{} - Unsupported request message type {}", req->msgType());
         delete req;
      }
   }
}

Void ExamplePfcpApplication::onRcvdRsp(PFCP::AppMsgRspPtr rsp)
{
   static EString __method__ = __METHOD_NAME__;
   ELogger::log(LOG_SYSTEM).debug(
      "{} - received response local={} remote={} msgType={} seqNbr={} seid={}",
      __method__, rsp->localNode()->address().getAddress(), rsp->remoteNode()->address().getAddress(),
      rsp->msgType(), rsp->seqNbr(), rsp->seid());
   
   switch (rsp->msgType())
   {
      case PFCP_PFD_MGMT_RSP:
      {
         // PFCP_R15::PfdMgmtRsp *am = static_cast<PFCP_R15::PfdMgmtRsp*>(rsp);
         break;
      }
      case PFCP_ASSN_SETUP_RSP:
      {
         // PFCP_R15::AssnSetupRsp *am = static_cast<PFCP_R15::AssnSetupRsp*>(rsp);
         ELogger::log(LOG_SYSTEM).debug("{} - received PFCP_ASSN_SETUP_RSP", __method__);
         break;
      }
      case PFCP_ASSN_UPD_RSP:
      {
         // PFCP_R15::AssnUpdateRsp *am = static_cast<PFCP_R15::AssnUpdateRsp*>(rsp);
         break;
      }
      case PFCP_ASSN_REL_RSP:
      {
         // PFCP_R15::AssnUpdateRsp *am = static_cast<PFCP_R15::AssnUpdateRsp*>(rsp);
         break;
      }
      case PFCP_NODE_RPT_RSP:
      {
         // PFCP_R15::NodeReportRsp *am = static_cast<PFCP_R15::NodeReportRsp*>(rsp);
         break;
      }
      case PFCP_SESS_SET_DEL_RSP:
      {
         // PFCP_R15::SessionSetDeletionRsp *am = static_cast<PFCP_R15::SessionSetDeletionRsp*>(rsp);
         break;
      }
      case PFCP_SESS_ESTAB_RSP:
      {
         // PFCP_R15::SessionEstablishmentRsp *am = static_cast<PFCP_R15::SessionEstablishmentRsp*>(rsp);
         break;
      }
      case PFCP_SESS_MOD_RSP:
      {
         // PFCP_R15::SessionModificationRsp *am = static_cast<PFCP_R15::SessionModificationRsp*>(rsp);
         break;
      }
      case PFCP_SESS_DEL_RSP:
      {
         // PFCP_R15::SessionDeletionRsp *am = static_cast<PFCP_R15::SessionDeletionRsp*>(rsp);
         break;
      }
      case PFCP_SESS_RPT_RSP:
      {
         // PFCP_R15::SessionReportRsp *am = static_cast<PFCP_R15::SessionReportRsp*>(rsp);
         break;
      }
      default:
      {
         PFCP::Configuration().logger().minor("{} - unsupported response message type {}", rsp->msgType());
         throw PFCP::EncodeRspException();
      }
   }

   delete rsp;
}

Void ExamplePfcpApplication::onReqTimeout(PFCP::AppMsgReqPtr req)
{
   static EString __method__ = __METHOD_NAME__;
   ELogger::log(LOG_SYSTEM).debug(
      "{} - timeout waiting for request local={} remote={} msgType={} seqNbr={} seid={} isReq={}",
      __method__, req->localNode()->address().getAddress(), req->remoteNode()->address().getAddress(),
      req->msgType(), req->seqNbr(), req->seid(), req->isReq()?"TRUE":"FALSE");
   delete req;
}

Void ExamplePfcpApplication::onRemoteNodeAdded(PFCP::RemoteNodeSPtr &rmtNode)
{
   static EString __method__ = __METHOD_NAME__;
   ELogger::log(LOG_SYSTEM).debug(
      "{} - remote node added remote={}",
      __method__, rmtNode->address().getAddress());
}

Void ExamplePfcpApplication::onRemoteNodeFailure(PFCP::RemoteNodeSPtr &rmtNode)
{
   static EString __method__ = __METHOD_NAME__;
   ELogger::log(LOG_SYSTEM).debug(
      "{} - remote node is unresponsive remote={}",
      __method__, rmtNode->address().getAddress());
}

Void ExamplePfcpApplication::onRemoteNodeRestart(PFCP::RemoteNodeSPtr &rmtNode)
{
   static EString __method__ = __METHOD_NAME__;
}

Void ExamplePfcpApplication::onRemoteNodeRemoved(PFCP::RemoteNodeSPtr &rmtNode)
{
   static EString __method__ = __METHOD_NAME__;
}

Void ExamplePfcpApplication::onSndReqError(PFCP::AppMsgReqPtr req, PFCP::SndReqException &err)
{
   static EString __method__ = __METHOD_NAME__;
}

Void ExamplePfcpApplication::onSndRspError(PFCP::AppMsgRspPtr rsp, PFCP::SndRspException &err)
{
   static EString __method__ = __METHOD_NAME__;
}

Void ExamplePfcpApplication::onEncodeReqError(PFCP::AppMsgReqPtr req, PFCP::EncodeReqException &err)
{
   static EString __method__ = __METHOD_NAME__;
}

Void ExamplePfcpApplication::onEncodeRspError(PFCP::AppMsgRspPtr rsp, PFCP::EncodeRspException &err)
{
   static EString __method__ = __METHOD_NAME__;
}
