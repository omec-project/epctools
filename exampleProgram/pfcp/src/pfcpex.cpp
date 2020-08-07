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

#include <iostream>
#include <fstream>

#include "pfcpex.h"
#include "pfcpr15inl.h"

#define STEP_SESSION_CREATE                        0
#define STEP_SESSION_ESTABLISHMENT_REQ_CREATE      1
#define STEP_SESSION_ESTABLISHMENT_RSP_CREATE      2
#define STEP_SESSION_ESTABLISHMENT_REQ_POPULATE    3
#define STEP_SESSION_ESTABLISHMENT_RSP_POPULATE    4
#define STEP_SESSION_DELETION_REQ_CREATE           5
#define STEP_SESSION_DELETION_RSP_CREATE           6
#define STEP_SESSION_DELETION_REQ_POPULATE         7
#define STEP_SESSION_DELETION_RSP_POPULATE         8

ExamplePfcpApplicationWorkGroup *ExamplePfcpApplicationWorkGroup::this_ = nullptr;

ExamplePfcpApplicationWorkGroup::ExamplePfcpApplicationWorkGroup()
{
   static EString __method__ = __METHOD_NAME__;
   this_ = this;
}

Void ExamplePfcpApplicationWorkGroup::shutdown()
{
   static EString __method__ = __METHOD_NAME__;

   this_->rn_.reset(static_cast<PFCP::RemoteNode*>(nullptr));
   this_->ln_.reset(static_cast<PFCP::LocalNode*>(nullptr));

#ifdef PFCP_ANALYSIS
   {
      cpStr fn = this_->sndAssnSetup_ ? "cp_pfcp_analysis.csv" : "up_pfcp_analysis.csv";
      std::ofstream f(fn, std::ios_base::trunc);
      if (f.is_open())
      {
         for (Int tp=0; tp < PFCP_ANALYSIS_TP_DIM; tp++)
         {
            switch(tp)
            {
               case STEP_SESSION_CREATE:                       { f << "SESSION_CREATE"; break; }
               case STEP_SESSION_ESTABLISHMENT_REQ_CREATE:     { f << ",SESSION_ESTAB_REQ_CREATE"; break; }
               case STEP_SESSION_ESTABLISHMENT_RSP_CREATE:     { f << ",SESSION_ESTAB_RSP_CREATE"; break; }
               case STEP_SESSION_ESTABLISHMENT_REQ_POPULATE:   { f << ",SESSION_ESTAB_REQ_POPULATE"; break; }
               case STEP_SESSION_ESTABLISHMENT_RSP_POPULATE:   { f << ",SESSION_ESTAB_RSP_POPULATE"; break; }
               case STEP_SESSION_DELETION_REQ_CREATE:          { f << ",SESSION_DEL_REQ_CREATE"; break; }
               case STEP_SESSION_DELETION_RSP_CREATE:          { f << ",SESSION_DEL_RSP_CREATE"; break; }
               case STEP_SESSION_DELETION_REQ_POPULATE:        { f << ",SESSION_DEL_REQ_POPULATE"; break; }
               case STEP_SESSION_DELETION_RSP_POPULATE:        { f << ",SESSION_DEL_RSP_POPULATE"; break; }
               default:                                        { break; }
            }
         }

         for (Int ses=0; ses < this_->sesCreateCnt_; ses++)
         {
            for (Int tp=0; tp < PFCP_ANALYSIS_TP_DIM; tp++)
               f << (tp==0?"":",") << PFCP::analysis[ses].tp[tp];
            f << std::endl;
         }
         f.close();
      }
   }
#endif
   
   PFCP::Uninitialize();
}

Void ExamplePfcpApplicationWorkGroup::startup(EGetOpt &opt)
{
   static EString __method__ = __METHOD_NAME__;
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
   sesCreateCnt_ = opt.get("/PfcpExample/sessionCreateCount", 100);
   sesConcurrent_ = opt.get("/PfcpExample/sessionConcurrent", 5);
   sesCreateStarted_ = 0;
   sesCreateCompleted_ = 0;
   sesCreateFailed_ = 0;
   sesDeleteStarted_ = 0;
   sesDeleteCompleted_ = 0;
   sesDeleteFailed_ = 0;

   PFCP_ANALYSIS_ALLOC(sesCreateCnt_);

   init(1, 1, minWorkers, maxWorkers, 100000);

   ln_ = ExamplePfcpApplicationWorkGroup::createLocalNode(lnip_.c_str(), port_);
   
   if (sndAssnSetup_)
   {
      rn_ = ln_->createRemoteNode(rnip_, port_);
      sendAssnSetupReq();
   }

   // {
   //    #define LOG_PFCP_MSG_SIZE(a,b) ELogger::log(LOG_PFCP).info("sizeof({})={} sizeof({})={}",#a,sizeof(a),#b,sizeof(b))
   //    LOG_PFCP_MSG_SIZE(PFCP_R15::HeartbeatReq, pfcp_hrtbeat_req_t);
   //    LOG_PFCP_MSG_SIZE(PFCP_R15::HeartbeatRsp, pfcp_hrtbeat_rsp_t);
   //    LOG_PFCP_MSG_SIZE(PFCP_R15::PfdMgmtReq, pfcp_pfd_mgmt_req_t);
   //    LOG_PFCP_MSG_SIZE(PFCP_R15::PfdMgmtRsp, pfcp_pfd_mgmt_rsp_t);
   //    LOG_PFCP_MSG_SIZE(PFCP_R15::AssnSetupReq, pfcp_assn_setup_req_t);
   //    LOG_PFCP_MSG_SIZE(PFCP_R15::AssnSetupRsp, pfcp_assn_setup_rsp_t);
   //    LOG_PFCP_MSG_SIZE(PFCP_R15::AssnUpdateReq, pfcp_assn_upd_req_t);
   //    LOG_PFCP_MSG_SIZE(PFCP_R15::AssnUpdateRsp, pfcp_assn_upd_rsp_t);
   //    LOG_PFCP_MSG_SIZE(PFCP_R15::AssnReleaseReq, pfcp_assn_rel_req_t);
   //    LOG_PFCP_MSG_SIZE(PFCP_R15::AssnReleaseRsp, pfcp_assn_rel_rsp_t);
   //    LOG_PFCP_MSG_SIZE(PFCP_R15::VersionNotSupportedRsp, pfcp_header_t);
   //    LOG_PFCP_MSG_SIZE(PFCP_R15::NodeReportReq, pfcp_node_rpt_req_t);
   //    LOG_PFCP_MSG_SIZE(PFCP_R15::NodeReportRsp, pfcp_node_rpt_rsp_t);
   //    LOG_PFCP_MSG_SIZE(PFCP_R15::SessionSetDeletionReq, pfcp_sess_set_del_req_t);
   //    LOG_PFCP_MSG_SIZE(PFCP_R15::SessionSetDeletionRsp, pfcp_sess_set_del_rsp_t);
   //    LOG_PFCP_MSG_SIZE(PFCP_R15::SessionEstablishmentReq, pfcp_sess_estab_req_t);
   //    LOG_PFCP_MSG_SIZE(PFCP_R15::SessionEstablishmentRsp, pfcp_sess_estab_rsp_t);
   //    LOG_PFCP_MSG_SIZE(PFCP_R15::SessionModificationReq, pfcp_sess_mod_req_t);
   //    LOG_PFCP_MSG_SIZE(PFCP_R15::SessionModificationRsp, pfcp_sess_mod_rsp_t);
   //    LOG_PFCP_MSG_SIZE(PFCP_R15::SessionDeletionReq, pfcp_sess_del_req_t);
   //    LOG_PFCP_MSG_SIZE(PFCP_R15::SessionDeletionRsp, pfcp_sess_del_rsp_t);
   //    LOG_PFCP_MSG_SIZE(PFCP_R15::SessionReportReq, pfcp_sess_rpt_req_t);
   //    LOG_PFCP_MSG_SIZE(PFCP_R15::SessionReportRsp, pfcp_sess_rpt_rsp_t);
   // }
}

Void ExamplePfcpApplicationWorkGroup::startProcessing()
{
   static EString __method__ = __METHOD_NAME__;
   ELogger::log(LOG_SYSTEM).info("{} - starting session creates", __method__);
   Int cnt = std::min(sesConcurrent_,sesCreateCnt_);
   elapsedTimer_.Start();
   totalTimer_.Start();
   for (int i=0; i<cnt; i++)
      addSession();
}

Void ExamplePfcpApplicationWorkGroup::addSession()
{
   static EString __method__ = __METHOD_NAME__;
   if (sesCreateStarted_ < sesCreateCnt_)
   {
      ++sesCreateStarted_;
      ETimer t;
      PFCP::SessionBaseSPtr ses = ln_->createSession(ln_, rn_);
      PFCP_ANALYSIS_SET_TP2(ses->localSeid(), STEP_SESSION_CREATE, t.MicroSeconds());
      sendSessionEstablishmentReq(ses);
   }
}

Void ExamplePfcpApplicationWorkGroup::delSession()
{
   static EString __method__ = __METHOD_NAME__;

   EMutexLock l(sessionsMutex_);
   auto sesit = sessions_.begin();
   if (sesit != sessions_.end())
   {
      sendSessionDeletionReq(sesit->second);
      ++sesDeleteStarted_;
      sessions_.erase(sesit);
   }
}

Void ExamplePfcpApplicationWorkGroup::onSessionEstablished(PFCP_R15::SessionEstablishmentRsp &rsp)
{
   static EString __method__ = __METHOD_NAME__;
   Bool callAddSession = False;
   Bool startDeletes = False;

   if (rsp.cause().cause() == PFCP_R15::CauseEnum::RequestAccepted)
   {
      EMutexLock l(sessionsMutex_);
      if (sessions_.find(rsp.req()->session()->localSeid()) == sessions_.end())
      {
         sessions_[rsp.req()->session()->localSeid()] = rsp.req()->session();
         ++sesCreateCompleted_;
         ELogger::log(LOG_SYSTEM).debug(
            "{} - session created localNode={} remoteNode={} localSeid={}",
            __method__, rsp.req()->session()->localNode()->address().getAddress(),
            rsp.req()->session()->remoteNode()->address().getAddress(),
            rsp.req()->session()->localSeid());
      }
      else
      {
         ELogger::log(LOG_SYSTEM).minor(
            "{} - session already exists localNode={} remoteNode={} localSeid={}",
            __method__, rsp.req()->session()->localNode()->address().getAddress(),
            rsp.req()->session()->remoteNode()->address().getAddress(),
            rsp.req()->session()->localSeid());
      }

      if (sesCreateStarted_ < sesCreateCnt_)
      {
         callAddSession = True;
      }
      else if (sesCreateCompleted_ == sesCreateCnt_)
      {
         startDeletes = True;
      }
   }
   else
   {
      ++sesCreateFailed_;
      ELogger::log(LOG_SYSTEM).debug(
         "{} - session create failed localNode={} remoteNode={} localSeid={} cause={}",
         __method__, rsp.req()->session()->localNode()->address().getAddress(),
         rsp.req()->session()->remoteNode()->address().getAddress(),
         rsp.req()->session()->localSeid(), rsp.cause().cause());
   }

   if (callAddSession)
   {
      addSession();
   }
   else if (startDeletes)
   {
         // all of the sessions have been created, so start the deletes
         ELogger::log(LOG_SYSTEM).info("{} - session creation complete succeeded={} failed={} elapsed={}ms",
            __method__, sesCreateCompleted_, sesCreateFailed_, elapsedTimer_.MilliSeconds());
         elapsedTimer_.Start();
         ELogger::log(LOG_SYSTEM).info("{} - starting session deletions", __method__);
      for (int i=0; i<sesConcurrent_; i++)
         delSession();
   }
}

Void ExamplePfcpApplicationWorkGroup::onSessionDeleted(PFCP_R15::SessionDeletionRsp &rsp)
{
   static EString __method__ = __METHOD_NAME__;
   Bool callDelSession = False;

   {
      EMutexLock l(sessionsMutex_);
      
      if (rsp.cause().cause() == PFCP_R15::CauseEnum::RequestAccepted)
         ++sesDeleteCompleted_;
      else
         ++sesDeleteFailed_;
      rsp.req()->session()->destroy(rsp.req()->session());
      if (sesDeleteCompleted_ + sesDeleteFailed_ == sesCreateCnt_)
      {
         elapsedTimer_.Stop();
         totalTimer_.Stop();
         ELogger::log(LOG_SYSTEM).info("{} - session deletion complete succeeded={} failed={} elapsed={}ms",
            __method__, sesDeleteCompleted_, sesDeleteFailed_, elapsedTimer_.MilliSeconds());
         ELogger::log(LOG_SYSTEM).info("{} - total elapsed time {}ms",
            __method__, totalTimer_.MilliSeconds());
         sendAssnReleaseReq();
      }
      else
      {
         callDelSession = True;
      }
   }

   if (callDelSession)
      delSession();
}

Void ExamplePfcpApplicationWorkGroup::onCreateWorker(ExamplePfcpApplicationWorker &worker)
{
   static EString __method__ = __METHOD_NAME__;
   worker.group(*this);
}

Void ExamplePfcpApplicationWorkGroup::sendAssnSetupReq()
{
   static EString __method__ = __METHOD_NAME__;
   PFCP_R15::AssnSetupReq *req = new PFCP_R15::AssnSetupReq(ln_, rn_);

   req->node_id().node_id_value(ln_->address());
   req->rcvry_time_stmp().rcvry_time_stmp_val(ln_->startTime());

   // Send to the translation layer for encoding and subsquent transmission
   SEND_TO_TRANSLATION(SndMsg, req);
}

Void ExamplePfcpApplicationWorkGroup::sendAssnSetupRsp(PFCP_R15::AssnSetupReq *req)
{
   static EString __method__ = __METHOD_NAME__;
   PFCP_R15::AssnSetupRsp *rsp = new PFCP_R15::AssnSetupRsp();

   ELogger::log(LOG_SYSTEM).debug(
      "{} - sending PFCP Association Setup Response local={} remote={}",
      __method__, req->localNode()->address().getAddress(), req->remoteNode()->address().getAddress());

   rsp->setReq(req);

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

Void ExamplePfcpApplicationWorkGroup::sendAssnReleaseReq()
{
   static EString __method__ = __METHOD_NAME__;
   PFCP_R15::AssnReleaseReq *req = new PFCP_R15::AssnReleaseReq(ln_, rn_);

   req->node_id().node_id_value(ln_->address());

   // Send to the translation layer for encoding and subsquent transmission
   SEND_TO_TRANSLATION(SndMsg, req);
}

Void ExamplePfcpApplicationWorkGroup::sendAssnReleaseRsp(PFCP_R15::AssnReleaseReq *req)
{
   static EString __method__ = __METHOD_NAME__;
   PFCP_R15::AssnReleaseRsp *rsp = new PFCP_R15::AssnReleaseRsp();

   ELogger::log(LOG_SYSTEM).debug(
      "{} - sending PFCP Association Release Response local={} remote={}",
      __method__, req->localNode()->address().getAddress(), req->remoteNode()->address().getAddress());

   rsp->setReq(req);

   rsp->node_id().node_id_value(rsp->localNode()->address());
   rsp->cause().cause(PFCP_R15::CauseEnum::RequestAccepted);

   SEND_TO_TRANSLATION(SndMsg, rsp);
}

Void ExamplePfcpApplicationWorkGroup::sendSessionEstablishmentReq(PFCP::SessionBaseSPtr &session)
{
   static EString __method__ = __METHOD_NAME__;

   PFCP_ANALYSIS_START(session->localSeid());
   PFCP_R15::SessionEstablishmentReq *req = new PFCP_R15::SessionEstablishmentReq(session);
   PFCP_ANALYSIS_SET_TP(session->localSeid(), STEP_SESSION_ESTABLISHMENT_REQ_CREATE);

   ELogger::log(LOG_SYSTEM).debug(
      "{} - sending PFCP Session Establishment Request localSeid={} remoteSeid={} localNode={} remoteNode={}",
      __method__, session->localSeid(), session->remoteSeid(), session->localNode()->address().getAddress(),
      session->remoteNode()->address().getAddress());

   req->cp_fseid().ip_address(ln_->ipAddress());
   req->cp_fseid().seid(session->localSeid());
   req->node_id().node_id_value(EIpAddress("8.8.8.8"));
   UChar apn[] = { 4, 'a', 'p', 'n', '1' };
   req->apn_dnn().apn_dnn(apn, sizeof(apn));

   PFCP_ANALYSIS_SET_TP(session->localSeid(), STEP_SESSION_ESTABLISHMENT_REQ_POPULATE);

   // Send to the translation layer for encoding and subsquent transmission
   SEND_TO_TRANSLATION(SndMsg, req);
}

Void ExamplePfcpApplicationWorkGroup::sendSessionEstablishmentRsp(PFCP_R15::SessionEstablishmentReq *req)
{
   static EString __method__ = __METHOD_NAME__;

   PFCP_ANALYSIS_START(req->session()->remoteSeid());
   PFCP_R15::SessionEstablishmentRsp *rsp = new PFCP_R15::SessionEstablishmentRsp();
   PFCP_ANALYSIS_SET_TP(req->session()->remoteSeid(), STEP_SESSION_ESTABLISHMENT_RSP_CREATE);

   PFCP_R15::CauseEnum cause = PFCP_R15::CauseEnum::RequestAccepted;

   {
      EMutexLock l(sessionsMutex_);
      if (sessions_.find(req->session()->localSeid()) == sessions_.end())
      {
         sessions_[req->session()->localSeid()] = req->session();
         rsp->up_fseid().ip_address(ln_->ipAddress());
         rsp->up_fseid().seid(req->session()->localSeid());
         ELogger::log(LOG_SYSTEM).debug(
            "{} - session established localNode={} remoteNode={} localSeid={} remoteSeid={}",
            __method__, req->localNode()->address().getAddress(), req->remoteNode()->address().getAddress(),
            req->session()->localSeid(), req->session()->remoteSeid());
      }
      else
      {
         ELogger::log(LOG_SYSTEM).debug(
            "{} - session established failed (session alrady exists) localNode={} remoteNode={} localSeid={} remoteSeid={}",
            __method__, req->localNode()->address().getAddress(), req->remoteNode()->address().getAddress(),
            req->session()->localSeid(), req->session()->remoteSeid());
         cause = PFCP_R15::CauseEnum::RequestRejected;
      }
   }

   ELogger::log(LOG_SYSTEM).debug(
      "{} - sending PFCP Session Establishment Response localNode={} remoteNode={} localSeid={} remoteSeid={}",
      __method__, req->localNode()->address().getAddress(), req->remoteNode()->address().getAddress(),
      req->session()->localSeid(), req->session()->remoteSeid());

   rsp->setReq(req);

   rsp->cause().cause(cause);

   PFCP_ANALYSIS_SET_TP(req->session()->remoteSeid(), STEP_SESSION_ESTABLISHMENT_RSP_POPULATE);

   SEND_TO_TRANSLATION(SndMsg, rsp);
}

Void ExamplePfcpApplicationWorkGroup::sendSessionDeletionReq(PFCP::SessionBaseSPtr &session)
{
   static EString __method__ = __METHOD_NAME__;
   PFCP_ANALYSIS_START(session->localSeid());
   PFCP_R15::SessionDeletionReq *req = new PFCP_R15::SessionDeletionReq(session);
   PFCP_ANALYSIS_SET_TP(session->localSeid(), STEP_SESSION_DELETION_REQ_CREATE);

   ELogger::log(LOG_SYSTEM).debug(
      "{} - sending PFCP Session Deletion Request localNode={} remoteNode={} localSeid={} remoteSeid={}",
      __method__, req->localNode()->address().getAddress(), req->remoteNode()->address().getAddress(),
      session->localSeid(), session->remoteSeid());

   PFCP_ANALYSIS_SET_TP(session->localSeid(), STEP_SESSION_DELETION_REQ_POPULATE);

   SEND_TO_TRANSLATION(SndMsg, req);
}

Void ExamplePfcpApplicationWorkGroup::sendSessionDeletionRsp(PFCP_R15::SessionDeletionReq *req)
{
   static EString __method__ = __METHOD_NAME__;
   PFCP_ANALYSIS_START(req->session()->localSeid());
   PFCP_R15::SessionDeletionRsp *rsp = new PFCP_R15::SessionDeletionRsp();
   PFCP_ANALYSIS_SET_TP(req->session()->remoteSeid(), STEP_SESSION_DELETION_RSP_CREATE);

   PFCP_R15::CauseEnum cause = PFCP_R15::CauseEnum::RequestAccepted;

   {
      EMutexLock l(sessionsMutex_);
      if (sessions_.find(req->session()->localSeid()) != sessions_.end())
      {
         req->session()->destroy(req->session());
         sessions_.erase(req->session()->localSeid());
         ELogger::log(LOG_SYSTEM).debug(
            "{} - session deleted localNode={} remoteNode={} localSeid={} remoteSeid={}",
            __method__, req->localNode()->address().getAddress(), req->remoteNode()->address().getAddress(),
            req->session()->localSeid(), req->session()->remoteSeid());
      }
      else
      {
         cause = PFCP_R15::CauseEnum::RequestRejected;
         ELogger::log(LOG_SYSTEM).debug(
            "{} - session not found localNode={} remoteNode={} localSeid={} remoteSeid={}",
            __method__, req->localNode()->address().getAddress(), req->remoteNode()->address().getAddress(),
            req->session()->localSeid(), req->session()->remoteSeid());
      }
   }

   ELogger::log(LOG_SYSTEM).debug(
      "{} - sending PFCP Session Deletion Response localNode={} remoteNode={} localSeid={} remoteSeid={}",
      __method__, req->localNode()->address().getAddress(), req->remoteNode()->address().getAddress(),
      req->session()->localSeid(), req->session()->remoteSeid());

   rsp->setReq(req);

   rsp->cause().cause(cause);

   PFCP_ANALYSIS_SET_TP(req->session()->remoteSeid(), STEP_SESSION_DELETION_RSP_POPULATE);

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

Void ExamplePfcpApplicationWorker::onTimer(EThreadEventTimer *pTimer)
{
   static EString __method__ = __METHOD_NAME__;
   ELogger::log(LOG_SYSTEM).debug("{} workerId={}", __method__, workerId());
   group().addSession();
}

Void ExamplePfcpApplicationWorker::onRcvdReq(PFCP::AppMsgReqPtr req)
{
   static EString __method__ = __METHOD_NAME__;
   if (req->msgClass() == PFCP::MsgClass::Session)
   {
      ELogger::log(LOG_SYSTEM).debug(
         "{} workerId={} - request received local={} remote={} msgType={} msgClass=SESSION seqNbr={} seid={}",
         __method__, workerId(), req->localNode()->address().getAddress(), req->remoteNode()->address().getAddress(),
         req->msgType(), req->seqNbr(), static_cast<PFCP::AppMsgSessionReqPtr>(req)->session()->localSeid());
   }
   else
   {
      ELogger::log(LOG_SYSTEM).debug(
         "{} workerId={} - request received local={} remote={} msgType={} msgClass={} seqNbr={}",
         __method__, workerId(), req->localNode()->address().getAddress(), req->remoteNode()->address().getAddress(),
         req->msgType(), req->msgClass()==PFCP::MsgClass::Node?"NODE":"UNKNOWN", req->seqNbr());
   }

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
         ELogger::log(LOG_SYSTEM).debug("{} workerId={} - received PFCP_ASSN_SETUP_REQ", __method__, workerId());
         group().sendAssnSetupRsp(am);
         break;
      }
      case PFCP_ASSN_UPD_REQ:
      {
         // PFCP_R15::AssnUpdateReq *am = static_cast<PFCP_R15::AssnUpdateReq*>(req);
         break;
      }
      case PFCP_ASSN_REL_REQ:
      {
         PFCP_R15::AssnReleaseReq *am = static_cast<PFCP_R15::AssnReleaseReq*>(req);
         ELogger::log(LOG_SYSTEM).debug("{} workerId={} - received PFCP_ASSN_REL_REQ", __method__, workerId());
         group().sendAssnReleaseRsp(am);
         sleep(1000); // wait for the message to be sent
         ExamplePfcpApplicationWorkGroup::setShutdownEvent();
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
         PFCP_R15::SessionEstablishmentReq *am = static_cast<PFCP_R15::SessionEstablishmentReq*>(req);
         ELogger::log(LOG_SYSTEM).debug("{} workerId={} - received PFCP_SESS_ESTAB_REQ", __method__, workerId());
         group().sendSessionEstablishmentRsp(am);
         break;
      }
      case PFCP_SESS_MOD_REQ:
      {
         // PFCP_R15::SessionModificationReq *am = static_cast<PFCP_R15::SessionModificationReq*>(req);
         break;
      }
      case PFCP_SESS_DEL_REQ:
      {
         PFCP_R15::SessionDeletionReq *am = static_cast<PFCP_R15::SessionDeletionReq*>(req);
         ELogger::log(LOG_SYSTEM).debug("{} workerId={} - received PFCP_SESS_ESTAB_REQ", __method__, workerId());
         group().sendSessionDeletionRsp(am);
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
   if (rsp->msgClass() == PFCP::MsgClass::Session)
   {
      ELogger::log(LOG_SYSTEM).debug(
         "{} workerId={} - received response local={} remote={} msgType={} msgClass=SESSION seqNbr={} seid={}",
         __method__, workerId(), rsp->localNode()->address().getAddress(), rsp->remoteNode()->address().getAddress(),
         rsp->msgType(), rsp->seqNbr(), static_cast<PFCP::AppMsgSessionReqPtr>(rsp->req())->session()->localSeid());
   }
   else
   {
      ELogger::log(LOG_SYSTEM).debug(
         "{} workerId={} - received response local={} remote={} msgType={} msgClass={} seqNbr={}",
         __method__, workerId(), rsp->localNode()->address().getAddress(), rsp->remoteNode()->address().getAddress(),
         rsp->msgType(), rsp->msgClass()==PFCP::MsgClass::Node?"NODE":"UNKNOWN", rsp->seqNbr());
   }
   
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
         group_->startProcessing();
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
         ELogger::log(LOG_SYSTEM).debug("{} workerId={} - received PFCP_ASSN_SETUP_RSP", __method__, workerId());
         ExamplePfcpApplicationWorkGroup::setShutdownEvent();
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
         PFCP_R15::SessionEstablishmentRsp *am = static_cast<PFCP_R15::SessionEstablishmentRsp*>(rsp);
         ELogger::log(LOG_SYSTEM).debug("{} workerId={} - received PFCP_SESS_ESTAB_RSP", __method__, workerId());
         group_->onSessionEstablished(*am);
         break;
      }
      case PFCP_SESS_MOD_RSP:
      {
         // PFCP_R15::SessionModificationRsp *am = static_cast<PFCP_R15::SessionModificationRsp*>(rsp);
         break;
      }
      case PFCP_SESS_DEL_RSP:
      {
         PFCP_R15::SessionDeletionRsp *am = static_cast<PFCP_R15::SessionDeletionRsp*>(rsp);
         group_->onSessionDeleted(*am);
         ELogger::log(LOG_SYSTEM).debug("{} workerId={} - received PFCP_SESS_DEL_RSP localNode={} remoteNode={} localSeid={} remoteSeid={}",
            __method__, workerId(), am->req()->localNode()->address().getAddress(), am->req()->remoteNode()->address().getAddress(),
            am->req()->session()->localSeid(), am->req()->session()->remoteSeid());
         break;
      }
      case PFCP_SESS_RPT_RSP:
      {
         // PFCP_R15::SessionReportRsp *am = static_cast<PFCP_R15::SessionReportRsp*>(rsp);
         break;
      }
      default:
      {
         PFCP::Configuration().logger().minor("{} workerId={} - unsupported response message type {}", __method__, workerId(), rsp->msgType());
         throw PFCP::EncodeRspException();
      }
   }

   delete rsp;
}

Void ExamplePfcpApplicationWorker::onReqTimeout(PFCP::AppMsgReqPtr req)
{
   static EString __method__ = __METHOD_NAME__;
   if (req->msgClass() == PFCP::MsgClass::Session)
   {
      ELogger::log(LOG_SYSTEM).debug(
         "{} workerId={} - timeout waiting for request local={} remote={} msgType={} msgClass=SESSION seqNbr={} seid={} isReq={}",
         __method__, workerId(), req->localNode()->address().getAddress(), req->remoteNode()->address().getAddress(),
         req->msgType(), req->seqNbr(), static_cast<PFCP::AppMsgSessionReqPtr>(req)->session()->localSeid(),
         req->isReq()?"TRUE":"FALSE");
   }
   else
   {
      ELogger::log(LOG_SYSTEM).debug(
         "{} workerId={} - timeout waiting for request local={} remote={} msgType={} msgClass={} seqNbr={} isReq={}",
         __method__, workerId(), req->localNode()->address().getAddress(), req->remoteNode()->address().getAddress(),
         req->msgType(), req->msgClass()==PFCP::MsgClass::Node?"NODE":"UNKNOWN", req->seqNbr(), req->isReq()?"TRUE":"FALSE");
   }
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
   if (req->msgClass() == PFCP::MsgClass::Session)
   {
      ELogger::log(LOG_SYSTEM).debug(
         "{} workerId={} - msgType={} msgClass=SESSION seqNbr={} seid={} - {}",__method__, workerId(),
         req->msgType(), req->seqNbr(), static_cast<PFCP::AppMsgSessionReqPtr>(req)->session()->localSeid(), err.what());
   }
   else
   {
      ELogger::log(LOG_SYSTEM).debug(
         "{} workerId={} - msgType={} msgClass={} seqNbr={} - {}",
         __method__, workerId(), req->msgType(), req->msgClass()==PFCP::MsgClass::Node?"NODE":"UNKNOWN",
         req->seqNbr(), err.what());
   }
   delete req;
}

Void ExamplePfcpApplicationWorker::onSndRspError(PFCP::AppMsgRspPtr rsp, PFCP::SndRspException &err)
{
   static EString __method__ = __METHOD_NAME__;
   if (rsp->msgClass() == PFCP::MsgClass::Session)
   {
      ELogger::log(LOG_SYSTEM).debug(
         "{} workerId={} - msgType={} msgClass=SESSION seqNbr={} seid={} = {}",
         __method__, workerId(), rsp->msgType(), rsp->seqNbr(),
         static_cast<PFCP::AppMsgSessionReqPtr>(rsp->req())->session()->localSeid(), err.what());
   }
   else
   {
      ELogger::log(LOG_SYSTEM).debug(
         "{} workerId={} - msgType={} msgClass={} seqNbr={} - {}",
         __method__, workerId(), rsp->msgType(), rsp->msgClass()==PFCP::MsgClass::Node?"NODE":"UNKNOWN",
         rsp->seqNbr(), err.what());
   }
   delete rsp;
}

Void ExamplePfcpApplicationWorker::onEncodeReqError(PFCP::AppMsgReqPtr req, PFCP::EncodeReqException &err)
{
   static EString __method__ = __METHOD_NAME__;
   if (req->msgClass() == PFCP::MsgClass::Session)
   {
      ELogger::log(LOG_SYSTEM).debug(
         "{} workerId={} - msgType={} msgClass=SESSION seqNbr={} seid={} - {}",__method__, workerId(),
         req->msgType(), req->seqNbr(), static_cast<PFCP::AppMsgSessionReqPtr>(req)->session()->localSeid(), err.what());
   }
   else
   {
      ELogger::log(LOG_SYSTEM).debug(
         "{} workerId={} - msgType={} msgClass={} seqNbr={} - {}",
         __method__, workerId(), req->msgType(), req->msgClass()==PFCP::MsgClass::Node?"NODE":"UNKNOWN",
         req->seqNbr(), err.what());
   }
   delete req;
}

Void ExamplePfcpApplicationWorker::onEncodeRspError(PFCP::AppMsgRspPtr rsp, PFCP::EncodeRspException &err)
{
   static EString __method__ = __METHOD_NAME__;
   if (rsp->msgClass() == PFCP::MsgClass::Session)
   {
      ELogger::log(LOG_SYSTEM).debug(
         "{} workerId={} - msgType={} msgClass=SESSION seqNbr={} seid={} - {}",
         __method__, workerId(), rsp->msgType(), rsp->seqNbr(),
         static_cast<PFCP::AppMsgSessionReqPtr>(rsp->req())->session()->localSeid(), err.what());
   }
   else
   {
      ELogger::log(LOG_SYSTEM).debug(
         "{} workerId={} - msgType={} msgClass={} seqNbr={} - {}",
         __method__, workerId(), rsp->msgType(), rsp->msgClass()==PFCP::MsgClass::Node?"NODE":"UNKNOWN",
         rsp->seqNbr(), err.what());
   }
   delete rsp;
}
