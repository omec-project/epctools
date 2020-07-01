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
#include "pfcpr15inl.h"

ExamplePfcpApplicationWorkGroup *ExamplePfcpApplicationWorkGroup::this_ = nullptr;

ExamplePfcpApplicationWorkGroup::ExamplePfcpApplicationWorkGroup()
{
   this_ = this;
}

Void ExamplePfcpApplicationWorkGroup::shutdown()
{
   PFCP::Uninitialize();
}

Void ExamplePfcpApplicationWorkGroup::startup(EGetOpt &opt)
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
   PFCP::Configuration::setApplication(*this);
   PFCP::Configuration::setTranslator(xlator_);

   PFCP::Initialize();

   Int minWorkers = opt.get("/PfcpExample/minApplicationWorkers", 1);
   Int maxWorkers = opt.get("/PfcpExample/maxApplicationWorkers", 1);

   init(1, 1, minWorkers, maxWorkers, 100000);

   ln_ = createLocalNode(lnip_.c_str(), port_);
   
   if (sndAssnSetup_)
   {
      rn_ = ln_->createRemoteNode(rnip_, port_);
      sendAssociationSetupReq();
   }
}

Void ExamplePfcpApplicationWorkGroup::onCreateWorker(ExamplePfcpApplicationWorker &worker)
{
   worker.group(*this);
}

Void ExamplePfcpApplicationWorkGroup::sendAssociationSetupReq()
{
   static EString __method__ = __METHOD_NAME__;
   PFCP_R15::AssnSetupReq *req = new PFCP_R15::AssnSetupReq(ln_, rn_);

   req->sequenceNumber(ln_->allocSeqNbr());
   req->node_id().node_id_value(ln_->address());
   req->rcvry_time_stmp().rcvry_time_stmp_val(ln_->startTime());

   // Send to the translation layer for encoding and subsquent transmission
   SEND_TO_TRANSLATION(SndMsg, req);
}

Void ExamplePfcpApplicationWorkGroup::sendAssociationSetupRsp(PFCP_R15::AssnSetupReq *req)
{
   static EString __method__ = __METHOD_NAME__;
   PFCP_R15::AssnSetupRsp *rsp = new PFCP_R15::AssnSetupRsp();

   ELogger::log(LOG_SYSTEM).debug(
      "{} - sending PFCP Association Setup Response local={} remote={}",
      __method__, req->localNode()->address().getAddress(), req->remoteNode()->address().getAddress());

   rsp->setReq(req);
   rsp->sequenceNumber(req->seqNbr());

   rsp->node_id().node_id_value(rsp->localNode()->address());
   rsp->cause().cause(PFCP_R15::CauseEnum::RequestAccepted);
   rsp->rcvry_time_stmp().rcvry_time_stmp_val(req->localNode()->startTime());
   rsp->up_func_feat()
      .bucp(True)
      .pfdm(True)
      .trst(True);
   uint16_t idx = rsp->next_user_plane_ip_rsrc_info();
   if (idx != -1)
   {
      rsp->user_plane_ip_rsrc_info(idx)
         .teid_range(PFCP::Configuration::teidRangeBits(), rsp->remoteNode()->teidRangeValue())
         .ip_address(EIpAddress("1.2.3.4"))
         .src_intfc(PFCP_R15::SourceInterfaceEnum::Core);
   }
   SEND_TO_TRANSLATION(SndMsg, rsp);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Void ExamplePfcpApplicationWorker::onInit()
{
   static EString __method__ = __METHOD_NAME__;
   PFCP::ApplicationWorker::onInit();
}

Void ExamplePfcpApplicationWorker::onQuit()
{
   static EString __method__ = __METHOD_NAME__;
   PFCP::ApplicationWorker::onQuit();
}

Void ExamplePfcpApplicationWorker::onRcvdReq(PFCP::AppMsgReqPtr req)
{
   static EString __method__ = __METHOD_NAME__;
   ELogger::log(LOG_SYSTEM).debug(
      "{} workerId={} - request received local={} remote={} msgType={} seqNbr={} seid={}",
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
         ELogger::log(LOG_SYSTEM).debug("{} workerId={} - received PFCP_ASSN_SETUP_REQ", __method__);
         group().sendAssociationSetupRsp(am);
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
         PFCP::Configuration().logger().minor("{} workerId={} - Unsupported request message type {}",
            __method__, workerId(), req->msgType());
         delete req;
      }
   }
}

Void ExamplePfcpApplicationWorker::onRcvdRsp(PFCP::AppMsgRspPtr rsp)
{
   static EString __method__ = __METHOD_NAME__;
   ELogger::log(LOG_SYSTEM).debug(
      "{} workerId={} - received response local={} remote={} msgType={} seqNbr={} seid={}",
      __method__, workerId(), rsp->localNode()->address().getAddress(), rsp->remoteNode()->address().getAddress(),
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
         ELogger::log(LOG_SYSTEM).debug("{} workerId={} - received PFCP_ASSN_SETUP_RSP", __method__, workerId());
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
         PFCP::Configuration().logger().minor("{} workerId={} - unsupported response message type {}", workerId(), rsp->msgType());
         throw PFCP::EncodeRspException();
      }
   }

   delete rsp;
}

Void ExamplePfcpApplicationWorker::onReqTimeout(PFCP::AppMsgReqPtr req)
{
   static EString __method__ = __METHOD_NAME__;
   ELogger::log(LOG_SYSTEM).debug(
      "{} workerId={} - timeout waiting for request local={} remote={} msgType={} seqNbr={} seid={} isReq={}",
      __method__, workerId(), req->localNode()->address().getAddress(), req->remoteNode()->address().getAddress(),
      req->msgType(), req->seqNbr(), req->seid(), req->isReq()?"TRUE":"FALSE");
   delete req;
}

Void ExamplePfcpApplicationWorker::onRemoteNodeAdded(PFCP::RemoteNodeSPtr &rmtNode)
{
   static EString __method__ = __METHOD_NAME__;
   ELogger::log(LOG_SYSTEM).debug(
      "{} workerId={} - remote node added remote={}",
      __method__, workerId(), rmtNode->address().getAddress());
}

Void ExamplePfcpApplicationWorker::onRemoteNodeFailure(PFCP::RemoteNodeSPtr &rmtNode)
{
   static EString __method__ = __METHOD_NAME__;
   ELogger::log(LOG_SYSTEM).debug(
      "{} workerId={} - remote node is unresponsive remote={}",
      __method__, workerId(), rmtNode->address().getAddress());
}

Void ExamplePfcpApplicationWorker::onRemoteNodeRestart(PFCP::RemoteNodeSPtr &rmtNode)
{
   static EString __method__ = __METHOD_NAME__;
   ELogger::log(LOG_SYSTEM).debug(
      "{} workerId={} - remote node has restarted remote={}",
      __method__, workerId(), rmtNode->address().getAddress());
}

Void ExamplePfcpApplicationWorker::onRemoteNodeRemoved(PFCP::RemoteNodeSPtr &rmtNode)
{
   static EString __method__ = __METHOD_NAME__;
   ELogger::log(LOG_SYSTEM).debug(
      "{} workerId={} - remote node has been removed remote={}",
      __method__, workerId(), rmtNode->address().getAddress());
}

Void ExamplePfcpApplicationWorker::onSndReqError(PFCP::AppMsgReqPtr req, PFCP::SndReqException &err)
{
   static EString __method__ = __METHOD_NAME__;
   ELogger::log(LOG_SYSTEM).debug(
      "{} workerId={} - msgType={} seqNbr={} seid={}",
      __method__, workerId(), req->msgType(), req->seqNbr(), req->seid(), err.what());
   delete req;
}

Void ExamplePfcpApplicationWorker::onSndRspError(PFCP::AppMsgRspPtr rsp, PFCP::SndRspException &err)
{
   static EString __method__ = __METHOD_NAME__;
   ELogger::log(LOG_SYSTEM).debug(
      "{} workerId={} - msgType={} seqNbr={} seid={}",
      __method__, workerId(), rsp->msgType(), rsp->seqNbr(), rsp->seid(), err.what());
   delete rsp;
}

Void ExamplePfcpApplicationWorker::onEncodeReqError(PFCP::AppMsgReqPtr req, PFCP::EncodeReqException &err)
{
   static EString __method__ = __METHOD_NAME__;
   ELogger::log(LOG_SYSTEM).debug(
      "{} workerId={} - msgType={} seqNbr={} seid={}",
      __method__,  req->msgType(), req->seqNbr(), req->seid(), err.what());
   delete req;
}

Void ExamplePfcpApplicationWorker::onEncodeRspError(PFCP::AppMsgRspPtr rsp, PFCP::EncodeRspException &err)
{
   static EString __method__ = __METHOD_NAME__;
   ELogger::log(LOG_SYSTEM).debug(
      "{} workerId={} - msgType={} seqNbr={} seid={}",
      __method__, workerId(), workerId(), rsp->msgType(), rsp->seqNbr(), rsp->seid(), err.what());
   delete rsp;
}
