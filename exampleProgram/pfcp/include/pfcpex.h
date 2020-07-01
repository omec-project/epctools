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

   Void sendAssociationSetupReq();
   Void sendAssociationSetupRsp(PFCP_R15::AssnSetupReq *am);

   Void onCreateWorker(ExamplePfcpApplicationWorker &worker);

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
};

#endif // #ifndef __PFCPEX_H