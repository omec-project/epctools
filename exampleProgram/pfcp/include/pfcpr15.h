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

#ifndef __PFCPR15_H
#define __PFCPR15_H

#include "epfcp.h"
#include "pfcp_messages.h"

namespace PFCP_R15
{

////////////////////////////////////////////////////////////////////////////////

class HeartbeatReq : public PFCP::AppMsgReq
{
public:
   HeartbeatReq(PFCP::LocalNodeSPtr &ln, PFCP::RemoteNodeSPtr &rn)
      : PFCP::AppMsgReq(ln,rn)
   {
      data_ = {};
      setMsgType(PFCP_HRTBEAT_REQ);
      data_.header.message_type = msgType();
      data_.header.version = 1;
   }
   
   pfcp_hrtbeat_req_t &data() { return data_; }
private:
   pfcp_hrtbeat_req_t data_;
};

class HeartbeatRsp : public PFCP::AppMsgRsp
{
public:
   HeartbeatRsp()
   {
      data_ = {};
      setMsgType(PFCP_HRTBEAT_RSP);
      data_.header.message_type = msgType();
      data_.header.version = 1;
   }

   pfcp_hrtbeat_rsp_t &data() { return data_; }
private:
   pfcp_hrtbeat_rsp_t data_;
};

////////////////////////////////////////////////////////////////////////////////

class PfdMgmtReq : public PFCP::AppMsgReq
{
public:
   PfdMgmtReq(PFCP::LocalNodeSPtr &ln, PFCP::RemoteNodeSPtr &rn)
      : PFCP::AppMsgReq(ln,rn)
   {
      setMsgType(PFCP_PFD_MGMT_REQ);
      data_ = {};
   }

   pfcp_pfd_mgmt_req_t &data() { return data_; }
private:
   pfcp_pfd_mgmt_req_t data_;
};

class PfdMgmtRsp : public PFCP::AppMsgRsp
{
public:
   PfdMgmtRsp()
   {
      setMsgType(PFCP_PFD_MGMT_RSP);
      data_ = {};
   }

   pfcp_pfd_mgmt_rsp_t &data() { return data_; }
private:
   pfcp_pfd_mgmt_rsp_t data_;
};

////////////////////////////////////////////////////////////////////////////////

class AssnSetupReq : public PFCP::AppMsgReq
{
public:
   AssnSetupReq(PFCP::LocalNodeSPtr &ln, PFCP::RemoteNodeSPtr &rn)
      : PFCP::AppMsgReq(ln,rn)
   {
      setMsgType(PFCP_ASSN_SETUP_REQ);
      data_ = {};
   }

   pfcp_assn_setup_req_t &data() { return data_; }
private:
   pfcp_assn_setup_req_t data_;
};

class AssnSetupRsp : public PFCP::AppMsgRsp
{
public:
   AssnSetupRsp()
   {
      setMsgType(PFCP_ASSN_SETUP_RSP);
      data_ = {};
   }

   pfcp_assn_setup_rsp_t &data() { return data_; }
private:
   pfcp_assn_setup_rsp_t data_;
};

////////////////////////////////////////////////////////////////////////////////

class AssnUpdateReq : public PFCP::AppMsgReq
{
public:
   AssnUpdateReq(PFCP::LocalNodeSPtr &ln, PFCP::RemoteNodeSPtr &rn)
      : PFCP::AppMsgReq(ln,rn)
   {
      setMsgType(PFCP_ASSN_UPD_REQ);
      data_ = {};
   }

   pfcp_assn_upd_req_t &data() { return data_; }
private:
   pfcp_assn_upd_req_t data_;
};

class AssnUpdateRsp : public PFCP::AppMsgRsp
{
public:
   AssnUpdateRsp()
   {
      setMsgType(PFCP_ASSN_UPD_RSP);
      data_ = {};
   }

   pfcp_assn_upd_rsp_t &data() { return data_; }
private:
   pfcp_assn_upd_rsp_t data_;
};

////////////////////////////////////////////////////////////////////////////////

class AssnReleaseReq : public PFCP::AppMsgReq
{
public:
   AssnReleaseReq(PFCP::LocalNodeSPtr &ln, PFCP::RemoteNodeSPtr &rn)
      : PFCP::AppMsgReq(ln,rn)
   {
      setMsgType(PFCP_ASSN_REL_REQ);
      data_ = {};
   }

   pfcp_assn_rel_req_t &data() { return data_; }
private:
   pfcp_assn_rel_req_t data_;
};

class AssnReleaseRsp : public PFCP::AppMsgRsp
{
public:
   AssnReleaseRsp()
   {
      setMsgType(PFCP_ASSN_REL_RSP);
      data_ = {};
   }

   pfcp_assn_rel_rsp_t &data() { return data_; }
private:
   pfcp_assn_rel_rsp_t data_;
};

////////////////////////////////////////////////////////////////////////////////

#define PFCP_VERSION_NOT_SUPPORTED (11)
class VersionNotSupportedRsp : public PFCP::AppMsgRsp
{
public:
   VersionNotSupportedRsp()
   {
      data_ = {};
      setMsgType(PFCP_VERSION_NOT_SUPPORTED);
   }
   
   pfcp_header_t &data() { return data_; }
private:
   pfcp_header_t data_;
};

////////////////////////////////////////////////////////////////////////////////

class NodeReportReq : public PFCP::AppMsgReq
{
public:
   NodeReportReq(PFCP::LocalNodeSPtr &ln, PFCP::RemoteNodeSPtr &rn)
      : PFCP::AppMsgReq(ln,rn)
   {
      setMsgType(PFCP_NODE_RPT_REQ);
      data_ = {};
   }

   pfcp_node_rpt_req_t &data() { return data_; }
private:
   pfcp_node_rpt_req_t data_;
};

class NodeReportRsp : public PFCP::AppMsgRsp
{
public:
   NodeReportRsp()
   {
      setMsgType(PFCP_NODE_RPT_RSP);
      data_ = {};
   }

   pfcp_node_rpt_rsp_t &data() { return data_; }
private:
   pfcp_node_rpt_rsp_t data_;
};

////////////////////////////////////////////////////////////////////////////////

class SessionSetDeletionReq : public PFCP::AppMsgReq
{
public:
   SessionSetDeletionReq(PFCP::LocalNodeSPtr &ln, PFCP::RemoteNodeSPtr &rn)
      : PFCP::AppMsgReq(ln,rn)
   {
      setMsgType(PFCP_SESS_SET_DEL_REQ);
      data_ = {};
   }

   pfcp_sess_set_del_req_t &data() { return data_; }
private:
   pfcp_sess_set_del_req_t data_;
};

class SessionSetDeletionRsp : public PFCP::AppMsgRsp
{
public:
   SessionSetDeletionRsp()
   {
      setMsgType(PFCP_SESS_SET_DEL_RSP);
      data_ = {};
   }

   pfcp_sess_set_del_rsp_t &data() { return data_; }
private:
   pfcp_sess_set_del_rsp_t data_;
};

////////////////////////////////////////////////////////////////////////////////

class SessionEstablishmentReq : public PFCP::AppMsgReq
{
public:
   SessionEstablishmentReq(PFCP::LocalNodeSPtr &ln, PFCP::RemoteNodeSPtr &rn)
      : PFCP::AppMsgReq(ln,rn)
   {
      setMsgType(PFCP_SESS_ESTAB_REQ);
      data_ = {};
   }

   pfcp_sess_estab_req_t &data() { return data_; }
private:
   pfcp_sess_estab_req_t data_;
};

class SessionEstablishmentRsp : public PFCP::AppMsgRsp
{
public:
   SessionEstablishmentRsp()
   {
      setMsgType(PFCP_SESS_ESTAB_RSP);
      data_ = {};
   }

   pfcp_sess_estab_rsp_t &data() { return data_; }
private:
   pfcp_sess_estab_rsp_t data_;
};

////////////////////////////////////////////////////////////////////////////////

class SessionModificationReq : public PFCP::AppMsgReq
{
public:
   SessionModificationReq(PFCP::LocalNodeSPtr &ln, PFCP::RemoteNodeSPtr &rn)
      : PFCP::AppMsgReq(ln,rn)
   {
      setMsgType(PFCP_SESS_MOD_REQ);
      data_ = {};
   }

   pfcp_sess_mod_req_t &data() { return data_; }
private:
   pfcp_sess_mod_req_t data_;
};

class SessionModificationRsp : public PFCP::AppMsgRsp
{
public:
   SessionModificationRsp()
   {
      setMsgType(PFCP_SESS_MOD_RSP);
      data_ = {};
   }
   pfcp_sess_mod_rsp_t &data() { return data_; }
private:
   pfcp_sess_mod_rsp_t data_;
};

////////////////////////////////////////////////////////////////////////////////

class SessionDeletionReq : public PFCP::AppMsgReq
{
public:
   SessionDeletionReq(PFCP::LocalNodeSPtr &ln, PFCP::RemoteNodeSPtr &rn)
      : PFCP::AppMsgReq(ln,rn)
   {
      setMsgType(PFCP_SESS_DEL_REQ);
      data_ = {};
   }

   pfcp_sess_del_req_t &data() { return data_; }
private:
   pfcp_sess_del_req_t data_;
};

class SessionDeletionRsp : public PFCP::AppMsgRsp
{
public:
   SessionDeletionRsp()
   {
      setMsgType(PFCP_SESS_DEL_RSP);
      data_ = {};
   }

   pfcp_sess_del_rsp_t &data() { return data_; }
private:
   pfcp_sess_del_rsp_t data_;
};

////////////////////////////////////////////////////////////////////////////////

class SessionReportReq : public PFCP::AppMsgReq
{
public:
   SessionReportReq(PFCP::LocalNodeSPtr &ln, PFCP::RemoteNodeSPtr &rn)
      : PFCP::AppMsgReq(ln,rn)
   {
      setMsgType(PFCP_SESS_RPT_REQ);
      data_ = {};
   }

   pfcp_sess_rpt_req_t &data() { return data_; }
private:
   pfcp_sess_rpt_req_t data_;
};

class SessionReportRsp : public PFCP::AppMsgRsp
{
public:
   SessionReportRsp()
   {
      setMsgType(PFCP_SESS_RPT_RSP);
      data_ = {};
   }

   pfcp_sess_rpt_rsp_t &data() { return data_; }
private:
   pfcp_sess_rpt_rsp_t data_;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class Translator : public PFCP::Translator
{
public:
   Translator();
   ~Translator();

   PFCP::ReqOutPtr encodeHeartbeatReq(PFCP::SndHeartbeatReqData &hb);
   PFCP::RspOutPtr encodeHeartbeatRsp(PFCP::SndHeartbeatRspData &hb);
   PFCP::RspOutPtr encodeVersionNotSupportedRsp(PFCP::ReqInPtr msg);

   PFCP::ReqOutPtr encodeReq(PFCP::AppMsgReqPtr msg);
   PFCP::RspOutPtr encodeRsp(PFCP::AppMsgRspPtr msg);
   PFCP::AppMsgReqPtr decodeReq(PFCP::ReqInPtr msg);
   PFCP::AppMsgRspPtr decodeRsp(PFCP::RspInPtr msg);
   PFCP::RcvdHeartbeatReqDataPtr decodeHeartbeatReq(PFCP::ReqInPtr msg);
   PFCP::RcvdHeartbeatRspDataPtr decodeHeartbeatRsp(PFCP::RspInPtr msg);

   Void getMsgInfo(PFCP::TranslatorMsgInfo &info, cpUChar msg, Int len);
   Bool isVersionSupported(UChar ver);
};

}

#endif