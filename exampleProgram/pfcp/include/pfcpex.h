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

#ifndef __PFCPEX_H
#define __PFCPEX_H

#include "etimer.h"
#include "epfcp.h"

#include "pfcpr15.h"

#define LOG_SYSTEM   1
#define LOG_PFCP     2

class ExamplePfcpApplicationWorkGroup;

class ExamplePfcpApplicationWorker : public PFCP::ApplicationWorker
{
public:
   ExamplePfcpApplicationWorker() : group_(nullptr) {}

   ExamplePfcpApplicationWorkGroup &group() { return *group_; }
   ExamplePfcpApplicationWorkGroup &group(ExamplePfcpApplicationWorkGroup &group)
      { return *(group_ = &group); }

   Void onInit();
   Void onQuit();
   Void onTimer(EThreadEventTimer *pTimer);

   Void onRcvdReq(PFCP::AppMsgReqPtr req);
   Void onRcvdRsp(PFCP::AppMsgRspPtr rsp);
   Void onReqTimeout(PFCP::AppMsgReqPtr req);
   Void onRemoteNodeAdded(PFCP::RemoteNodeSPtr &rmtNode);
   Void onRemoteNodeFailure(PFCP::RemoteNodeSPtr &rmtNode);
   Void onRemoteNodeRestart(PFCP::RemoteNodeSPtr &rmtNode);
   Void onRemoteNodeRemoved(PFCP::RemoteNodeSPtr &rmtNode);
   Void onSndReqError(PFCP::AppMsgReqPtr req, PFCP::SndReqException &err);
   Void onSndRspError(PFCP::AppMsgRspPtr rsp, PFCP::SndRspException &err);
   Void onEncodeReqError(PFCP::AppMsgReqPtr req, PFCP::EncodeReqException &err);
   Void onEncodeRspError(PFCP::AppMsgRspPtr rsp, PFCP::EncodeRspException &err);

   BEGIN_MESSAGE_MAP2(ExamplePfcpApplicationWorker, PFCP::ApplicationWorker)
   END_MESSAGE_MAP2()
protected:
private:
   ExamplePfcpApplicationWorkGroup *group_;
};

class ExamplePfcpApplicationWorkGroup : public PFCP::ApplicationWorkGroup<ExamplePfcpApplicationWorker>
{
public:
   ExamplePfcpApplicationWorkGroup();

   static Void setShutdownEvent() { if (this_ != nullptr) this_->m_shutdown.set(); }
   Void waitForShutdown() { m_shutdown.wait(); }

   static Void shutdown();
   Void startup(EGetOpt &opt);

   Void startProcessing();

   Void addSession();
   Void delSession();
   // Void delSession(PFCP::SessionBaseSPtr &ses);
   Void onSessionEstablished(PFCP_R15::SessionEstablishmentRsp &rsp);
   Void onSessionDeleted(PFCP_R15::SessionDeletionRsp &rsp);

   Void sendAssnSetupReq();
   Void sendAssnSetupRsp(PFCP_R15::AssnSetupReq *req);
   Void sendAssnReleaseReq();
   Void sendAssnReleaseRsp(PFCP_R15::AssnReleaseReq *req);
   Void sendSessionEstablishmentReq(PFCP::SessionBaseSPtr &session);
   Void sendSessionEstablishmentRsp(PFCP_R15::SessionEstablishmentReq *req);
   Void sendSessionDeletionReq(PFCP::SessionBaseSPtr &session);
   Void sendSessionDeletionRsp(PFCP_R15::SessionDeletionReq *req);

   Void onCreateWorker(ExamplePfcpApplicationWorker &worker);

   PFCP::LocalNodeSPtr _createLocalNode() override
      { return std::make_shared<PFCP::LocalNode>(); }
   PFCP::RemoteNodeSPtr _createRemoteNode() override
      { return std::make_shared<PFCP::RemoteNode>(); }
   PFCP::SessionBaseSPtr _createSession(PFCP::LocalNodeSPtr &ln, PFCP::RemoteNodeSPtr &rn) override
      { return std::make_shared<PFCP::SessionBase>(ln, rn); }

private:
   static ExamplePfcpApplicationWorkGroup *this_;
   EEvent m_shutdown;
   PFCP_R15::Translator xlator_;
   PFCP::LocalNodeSPtr ln_;
   PFCP::RemoteNodeSPtr rn_;

   EString lnip_;
   EString rnip_;
   UShort port_;
   Bool sndAssnSetup_;
   Int sesCreateCnt_;
   Int sesConcurrent_;
   ETimer elapsedTimer_;
   ETimer totalTimer_;
   Int sesCreateStarted_;
   Int sesCreateCompleted_;
   Int sesCreateFailed_;
   Int sesDeleteStarted_;
   Int sesDeleteCompleted_;
   Int sesDeleteFailed_;

   EMutexPrivate sessionsMutex_;
   PFCP::SessionBaseSPtrUMap sessions_;
};

#endif // #ifndef __PFCPEX_H