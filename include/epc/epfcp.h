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

#ifndef __EPFCP_H
#define __EPFCP_H

#include <atomic>
#include <unordered_map>
#include <utility>

#include "epctools.h"
#include "eip.h"
#include "esocket.h"
#include "eteid.h"
#include "etimerpool.h"

namespace PFCP
{
   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   Void Initialize();
   Void Uninitialize();

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   class Translator;
   class ApplicationThread;
   class TranslationThread;
   class CommunicationThread;

   class RemoteNode;
   typedef std::shared_ptr<RemoteNode> RemoteNodeSPtr;

   class LocalNode;
   typedef std::shared_ptr<LocalNode> LocalNodeSPtr;

   class ReqOut;
   typedef ReqOut *ReqOutPtr;
   typedef std::unordered_map<ULong,ReqOutPtr> ReqOutUMap;

   class RspOut;
   typedef RspOut *RspOutPtr;

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   #define SEND_TO_APPLICATION(a,b)                                                       \
      (PFCP::ApplicationThread::Instance().sendMessage(EThreadMessage(                    \
         static_cast<UInt>(PFCP::ApplicationThread::Events::a),static_cast<pVoid>(b))))
   #define SEND_TO_TRANSLATION(a,b)                                                       \
      (PFCP::TranslationThread::Instance().sendMessage(EThreadMessage(                    \
         static_cast<UInt>(PFCP::TranslationThread::Events::a),static_cast<pVoid>(b))))
   #define SEND_TO_COMMUNICATION(a,b)                                                     \
      (PFCP::CommunicationThread::Instance().sendMessage(EThreadMessage(                  \
         static_cast<UInt>(PFCP::CommunicationThread::Events::a),static_cast<pVoid>(b)))) 

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   typedef ULongLong Seid;
   typedef UChar MsgType;

   const MsgType PfcpHeartbeatReq    = 1;
   const MsgType PfcpHeartbeatRsp   = 2;

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   DECLARE_ERROR(Configuration_LoggerNotDefined);

   class Configuration
   {
   public:

      static UShort port()                                  { return port_; }
      static UShort setPort(UShort port)                    { return port_ = port; }

      static Int socketBufferSize()                         { return bufsize_; }
      static Int setSocketBufferSize(Int sz)                { return bufsize_ = sz; }

      static LongLong t1()                                  { return t1_; }
      static LongLong setT1(LongLong t1)                    { return t1_ = t1; }

      static LongLong heartbeatT1()                         { return hbt1_; }
      static LongLong setHeartbeatT1(LongLong hbt1)         { return hbt1_ = hbt1; }

      static Int n1()                                       { return n1_; }
      static Int setN1(Int n1)                              { return n1_ = n1; }

      static Int heartbeatN1()                              { return hbn1_; }
      static Int setHeartbeatN1(Int hbn1)                   { return hbn1_ = hbn1; }

      static Long maxRspWait()                              { return static_cast<Long>(std::max(t1_,hbt1_) * std::max(n1_,hbn1_)); }

      static size_t nbrActivityWnds()                       { return naw_; }
      static size_t setNnbrActivityWnds(size_t naw)         { return naw_ = naw; }

      static Long lenActivityWnd()                          { return law_; }
      static Long setLenActivityWnd(Long law)               { return law_ = law; }

      static ELogger &logger()                              { if (logger_ == nullptr) throw Configuration_LoggerNotDefined(); return *logger_; }
      static ELogger &setLogger(ELogger &log)               { logger_ = &log; return *logger_; }

      static Bool assignTeidRange()                         { return atr_; }
      static Bool setAssignTeidRange(Bool atr)              { return atr_ = atr; }

      static Int teidRangeBits()                            { return trb_; }
      static Int setTeidRangeBits(Int trb)                  { return trb_ = trb; }

      static Translator &translator()                       { return *xlator_; }
      static Translator &setTranslator(Translator &xlator)  { return *(xlator_ = &xlator); }

   private:
      static UShort port_;
      static Int bufsize_;
      static LongLong t1_;
      static LongLong hbt1_;
      static Int n1_;
      static Int hbn1_;
      static ELogger *logger_;
      static size_t naw_;
      static Long law_;
      static Int trb_;
      static Bool atr_;
      static Translator *xlator_;
   };

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   class SequenceManager
   {
   public:
      SequenceManager(): next_(SEQUENCE_MIN) {}
      ULong alloc()
      {
         unsigned long sn;
         while ((sn = next_++) > SEQUENCE_MAX)
            next_.compare_exchange_strong(++sn, SEQUENCE_MIN);
         return sn;
      }
      static Void free(ULong sn)
      {
         // currently, nothing is done to "free" a sequence number
      }
   private:
      static const ULong SEQUENCE_MIN = 0;
      static const ULong SEQUENCE_MAX = 0x00ffffffff;
      std::atomic_ulong next_;
   };

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   class SeidManager
   {
   public:
      SeidManager() : next_(SEID_MINIMUM) {}
      Seid alloc()
      {
         Seid seid;
         // this logic depends on the numeric overflow from a max value to zero
         // when the value is incremented.
         while ((seid = next_++) < SEID_MINIMUM);
         return seid;
      }
      static Void free(Seid seid)
      {
         // currently, nothing is done to "free" a SEID
      }
   private:
      static const Seid SEID_MINIMUM = 1;
      std::atomic_ullong next_;
   };

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   class FqSeid
   {
   public:
      FqSeid()
         : addr_(),
           seid_(0)
      {
      }
      FqSeid(const FqSeid &fqseid)
         : addr_(fqseid.addr_),
           seid_(fqseid.seid_)
      {
      }

      FqSeid &operator=(const FqSeid &fqseid)
      {
         addr_ = fqseid.addr_;
         seid_ = fqseid.seid_;
         return *this;
      }

      const EIpAddress &address() const   { return addr_; }
      EIpAddress &setAddress()            { return addr_; }

      const Seid seid() const             { return seid_; }
      FqSeid &setSeid(Seid seid)          { seid_ = seid; return *this; }

   private:
      EIpAddress addr_;
      Seid seid_;
   };

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   class RcvdReq
   {
   public:
      RcvdReq()
         : sn_(0),
           rw_(0)
      {
      }
      RcvdReq(ULong seqNbr)
         : sn_(seqNbr),
           rw_(0)
      {
      }
      RcvdReq(const RcvdReq &rr)
         : sn_(rr.sn_),
           rw_(rr.rw_)
      {
      }

      ULong seqNbr() const             { return sn_; }
      RcvdReq &setSeqNbr(ULong seqNbr) { sn_ = seqNbr; return *this; }

      Int rspWnd() const               { return rw_; }
      RcvdReq &setRspWnd(Int rw)       { rw_ = rw; return *this; }

   private:
      ULong sn_;
      Int rw_;
   };

   typedef std::unordered_map<ULong,RcvdReq> RcvdReqUMap;

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   class NodeSocket : public ESocket::UdpPrivate
   {
   public:
      NodeSocket();
      virtual ~NodeSocket();

      Void onReceive(const ESocket::Address &src, const ESocket::Address &dst, cpUChar data, Int len);
      Void onError();

      NodeSocket &setLocalNode(LocalNodeSPtr &ln)  { ln_ = ln; return *this; }
      NodeSocket &clearLocalNode()                 { ln_.reset(); return *this; }
   
   private:
      LocalNodeSPtr ln_;
   };

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   class Session
   {
   public:
      Session(LocalNodeSPtr &ln, RemoteNodeSPtr &rn)
         : ln_(ln),
           rn_(rn),
           ls_(0),
           rs_(0)
      {
      }
      Session(const Session &s)
         : ln_(s.ln_),
           rn_(s.rn_),
           ls_(s.ls_),
           rs_(s.rs_)
      {
      }
      virtual ~Session()
      {
      }

      LocalNodeSPtr &localNode()    { return ln_; }
      RemoteNodeSPtr &remoteNode()  { return rn_; }
      const Seid seid() const       { return ls_; }
      const Seid remoteSeid() const { return rs_; }

   private:
      Session();
      LocalNodeSPtr &ln_;
      RemoteNodeSPtr &rn_;
      Seid ls_;
      Seid rs_;
   };
   typedef std::shared_ptr<Session> SessionSPtr;
   typedef std::unordered_map<Seid,SessionSPtr> SessionSPtrUMap;

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   class Node
   {
   public:
      Node() {}
      virtual ~Node() {}

      
      EIpAddress &ipAddress()                            { return ipaddr_; }
      Node &setIpAddress(const EIpAddress &ipaddr)       {  ipaddr_ = ipaddr; return *this; }

      ESocket::Address &address()                        { return addr_; }
      Node &setAddress(const ESocket::Address &addr)
      {
          addr_ = addr;
          ipaddr_ = addr.getSockAddrStorage();
          return *this;
      }

      const ETime &startTime() const                     { return st_; }
      Node &setStartTime(const ETime &st = ETime::Now()) { st_ = st; return *this; }

      SessionSPtr getSession(Seid seid)
      {
         auto it = sessions_.find(seid);
         if (it != sessions_.end())
            return SessionSPtr();
         return it->second;
      }

      Node &addSession(SessionSPtr &session)
      {
         auto it = sessions_.find(session->seid());
         if (it == sessions_.end())
            sessions_[session->seid()] = session;
         return *this;
      }

   private:
      ETime st_;
      EIpAddress ipaddr_;
      ESocket::Address addr_;
      SessionSPtrUMap sessions_;
   };

   typedef std::shared_ptr<Node> NodeSPtr;

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   DECLARE_ERROR(RemoteNodeException_UnableToAssignTeidRangeValue);

   class RemoteNode : public Node
   {
   public:
      RemoteNode();
      virtual ~RemoteNode();

      Int teidRangeValue() const                            { return trv_; }
      RemoteNode &setTeidRangeValue(Int trv)                { trv_ = trv; return *this; }

      // methods related to receiving a req
      Bool addRcvdReq(ULong sn);
      Bool delRcvdReq(ULong sn)                             { return rrumap_.erase(sn) == 1; }
      Bool setRcvdReqRspWnd(ULong sn, Int wnd);
      Bool setRcvdReqRspWnd(ULong sn);
      Void removeRcvdRqstEntries(Int wnd);
      Bool rcvdReqExists(ULong sn) const                    { return rrumap_.find(sn) != rrumap_.end(); }

      // activity related methods
      RemoteNode &setNbrActivityWnds(size_t nbr);
      Void nextActivityWnd(Int wnd);
      Bool checkActivity();
      Void incrementActivity()                              { awnds_[aw_]++; }

      Void removeOldReqs(Int rw);

   private:
      ESocket::Address addr_;
      Int trv_;
      RcvdReqUMap rrumap_;
      std::vector<ULong> awnds_;
      size_t awndcnt_;
      size_t aw_;
   };

   typedef std::unordered_map<EIpAddress,RemoteNodeSPtr> RemoteNodeUMap;
   typedef std::pair<EIpAddress,RemoteNodeSPtr> RemoteNodeUMapPair;

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   DECLARE_ERROR(LocalNodeException_UnableToCreateRemoteNode);
   DECLARE_ERROR(LocalNodeException_RemoteNodeUMapInsertFailed);

   class LocalNode : public Node
   {
      friend NodeSocket;
      friend CommunicationThread;
   public:
      LocalNode();
      virtual ~LocalNode();

      Seid allocSeid();
      Void freeSeid(Seid seid);
      ULong allocSeqNbr();
      Void freeSeqNbr(ULong sn);

      Bool rqstOutExists(ULong seqnbr) const;
      Bool addRqstOut(ReqOut *ro);
      Bool setRqstOutRespWnd(ULong seqnbr, Int wnd);
      Void removeRqstOutEntries(Int wnd);

      Void setNbrActivityWnds(size_t nbr);
      Void nextActivityWnd(Int wnd);
      Void checkActivity(LocalNodeSPtr &ln);

      RemoteNodeSPtr createRemoteNode(cpStr addr, UShort port)
      {
         EIpAddress ipaddr(addr);
         return createRemoteNode(ipaddr, port);
      }
      RemoteNodeSPtr createRemoteNode(EIpAddress &address, UShort port);

      NodeSocket &socket() { return socket_; }

   protected:
      Void onReceive(LocalNodeSPtr &ln, const ESocket::Address &src, const ESocket::Address &dst, cpUChar msg, Int len);
      Bool onReqOutTimeout(ReqOutPtr ro);
      Void removeOldReqs(Int rw);

      Void sndInitialReq(ReqOutPtr ro);
      Bool sndReq(ReqOutPtr ro);
      Void sndRsp(RspOutPtr ro);

   private:
      SeidManager seidmgr_;
      SequenceManager seqmgr_;
      NodeSocket socket_;
      ReqOutUMap roumap_;
      RemoteNodeUMap rns_;
   };

   typedef std::unordered_map<EIpAddress,LocalNodeSPtr> LocalNodeUMap;
   typedef std::pair<EIpAddress,LocalNodeSPtr> LocalNodeUMapEIpAddressPair;
   typedef std::pair<ULong,LocalNodeSPtr> LocalNodeUMapULongPair;

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   class AppMsg
   {
   public:
      virtual ~AppMsg()
      {
      }

      Seid seid() const                     { return seid_; }
      ULong seqNbr() const                  { return seq_; }
      MsgType msgType() const               { return mt_; }
      Bool isReq() const                    { return rqst_; }

      AppMsg &setSeid(const Seid seid)      { seid_ = seid; return *this; }
      AppMsg &setSeqNbr(const ULong sn)     { seq_ = sn; return *this; }
      AppMsg &setMsgType(const MsgType mt)  { mt_ = mt; return*this; }

   protected:
      AppMsg()
         : seid_(0),
           seq_(0),
           mt_(0),
           rqst_(False)
      {
      }
      AppMsg(const AppMsg &dm)
         : seid_(dm.seid_),
           seq_(dm.seq_),
           mt_(dm.mt_),
           rqst_(dm.rqst_)
      {
      }

      AppMsg &setIsReq(const Bool rqst)       { rqst_ = rqst; return *this; }

   private:
      Seid seid_;
      ULong seq_;
      MsgType mt_;
      Bool rqst_;
   };

   typedef AppMsg *AppMsgPtr;

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   class AppMsgReq : public AppMsg
   {
   public:
      AppMsgReq()
      {
         setIsReq(True);
      }
      AppMsgReq(LocalNodeSPtr &ln, RemoteNodeSPtr &rn)
         : ln_(ln),
           rn_(rn)
      {
         setIsReq(True);
      }
      virtual ~AppMsgReq()
      {
      }

      LocalNodeSPtr &localNode()    { return ln_; }
      RemoteNodeSPtr &remoteNode()  { return rn_; }

      //Bool sendToTranslator();

   private:
      LocalNodeSPtr ln_;
      RemoteNodeSPtr rn_;
   };

   typedef AppMsgReq *AppMsgReqPtr;

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   class AppMsgRsp : public AppMsg
   {
   public:
      AppMsgRsp()
         : amrq_(nullptr)
      {
         setIsReq(False);
      }
      AppMsgRsp(AppMsgReqPtr &amrq)
         : amrq_(amrq)
      {
         setIsReq(False);
      }
      virtual ~AppMsgRsp()
      {
         if (amrq_)
            delete amrq_;
      }

      AppMsgReqPtr req()           { return amrq_; }
      LocalNodeSPtr &localNode()    { return amrq_->localNode(); }
      RemoteNodeSPtr &remoteNode()  { return amrq_->remoteNode(); }

      AppMsgRsp &setReq(AppMsgReqPtr req) { amrq_ = req; return *this; }

      //Bool sendToTranslator();

   private:
      AppMsgReqPtr amrq_;
   };

   typedef AppMsgRsp *AppMsgRspPtr;

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   class SndHeartbeatReqData
   {
   public:
      SndHeartbeatReqData()
      {
      }
      SndHeartbeatReqData(LocalNodeSPtr &ln, RemoteNodeSPtr &rn)
         : ln_(ln),
           rn_(rn)
      {
      }
      SndHeartbeatReqData(const SndHeartbeatReqData &hb)
      {
         ln_ = hb.ln_;
         rn_ = hb.rn_;
      }

      LocalNodeSPtr &localNode()                               { return ln_; }
      RemoteNodeSPtr &remoteNode()                             { return rn_; }

      SndHeartbeatReqData &setLocalNode(LocalNodeSPtr &ln)     { ln_ = ln; return *this; }
      SndHeartbeatReqData &setRemoteNode(RemoteNodeSPtr &rn)   { rn_ = rn; return *this; }

   private:
      LocalNodeSPtr ln_;
      RemoteNodeSPtr rn_;
   };

   typedef SndHeartbeatReqData *SndHeartbeatReqDataPtr;

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   class RcvdHeartbeatReqData
   {
   public:
      RcvdHeartbeatReqData()
         : am_(nullptr)
      {
      }
      RcvdHeartbeatReqData(const RcvdHeartbeatReqData &hb)
      {
         am_ = hb.am_;
         started_ = hb.started_;
      }

      AppMsgReqPtr req() const                              { return am_; }
      const ETime &startTime() const                        { return started_; }

      RcvdHeartbeatReqData &setReq(AppMsgReqPtr am)             { am_ = am; return *this;}
      RcvdHeartbeatReqData &setStartTime(const ETime &started)  { started_ = started; return *this; }

   private:
      AppMsgReqPtr am_;
      ETime started_;
   };

   typedef RcvdHeartbeatReqData *RcvdHeartbeatReqDataPtr;

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   class SndHeartbeatRspData
   {
   public:
      SndHeartbeatRspData()
         : am_(nullptr)
      {
      }
      SndHeartbeatRspData(AppMsgReqPtr am)
         : am_(am)
      {
      }

      AppMsgReq &req()   { return *am_; }

      SndHeartbeatRspData &setReq(AppMsgReqPtr am) { am_ = am; return *this; }

   private:
      AppMsgReqPtr am_;
   };

   typedef SndHeartbeatRspData *SndHeartbeatRspDataPtr;

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   class RcvdHeartbeatRspData
   {
   public:
      RcvdHeartbeatRspData()
         : am_(nullptr)
      {
      }
      RcvdHeartbeatRspData(AppMsgReqPtr am)
         : am_(am)
      {
      }

      AppMsgReq &req()                                      { return *am_; }
      const ETime &startTime() const                        { return started_; }

      RcvdHeartbeatRspData &setReq(AppMsgReqPtr am)             { am_ = am; return *this; }
      RcvdHeartbeatRspData &setStartTime(const ETime &started)  { started_ = started; return *this; }

   private:
      AppMsgReqPtr am_;
      ETime started_;
   };

   typedef RcvdHeartbeatRspData *RcvdHeartbeatRspDataPtr;

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   DECLARE_ERROR(InternalMsg_OutOfMemory);

   class InternalMsg
   {
   public:
      InternalMsg()
         : seid_(0),
           seq_(0),
           mt_(0),
           rqst_(False),
           data_(nullptr),
           len_(0)
      {
      }
      InternalMsg(const InternalMsg &im)
         : ln_(im.ln_),
           rn_(im.rn_),
           seid_(im.seid_),
           seq_(im.seq_),
           mt_(im.mt_),
           rqst_(im.rqst_),
           data_(nullptr),
           len_(0)
      {
         assign(im.data_, im.len_);
      }
      virtual ~InternalMsg()
      {
         if (data_ != nullptr)
            delete [] data_;
      }

      InternalMsg &operator=(const InternalMsg &im)
      {
         ln_ = im.ln_;
         rn_ = im.rn_;
         seid_ = im.seid_;
         seq_ = im.seq_;
         mt_ = im.mt_;
         rqst_ = im.rqst_;
         assign(im.data_, im.len_);
         return *this;
      }

      LocalNodeSPtr &localNode()          { return ln_; }
      RemoteNodeSPtr &remoteNode()        { return rn_; }
      Seid seid() const                   { return seid_; }
      ULong seqNbr() const                { return seq_; }
      MsgType msgType() const             { return mt_; }
      Bool isReq() const                  { return rqst_; }
      UChar version() const               { return ver_; }
      cpUChar data() const                { return data_; }
      UShort len() const                  { return len_; }

      InternalMsg &setLocalNode(const LocalNodeSPtr &ln)    { ln_ = ln; return *this; }
      InternalMsg &setRemoteNode(const RemoteNodeSPtr &rn)  { rn_ = rn; return *this; }
      InternalMsg &setSeid(const Seid seid)                 { seid_ = seid; return *this; }
      InternalMsg &setSeqNbr(const ULong sn)                { seq_ = sn; return *this; }
      InternalMsg &setMsgType(const MsgType mt)             { mt_ = mt; return*this; }
      InternalMsg &setIsReq(const Bool rqst)                { rqst_ = rqst; return *this; }
      InternalMsg &setVersion(const UChar ver)              { ver_ = ver; return *this; }

      InternalMsg &assign(cpUChar data, UShort len)
      {
         // allocate the memory if necessary
         if (len_ != len)
         {
            if (data_ != nullptr)
               delete [] data_;
            len_ = len;
            try
            {
               data_ = new UChar[len_];
            }
            catch(const std::bad_alloc& e)
            {
               throw InternalMsg_OutOfMemory();
            }
         }
         // copy the data
         std::memcpy(data_, data, len);
         return *this;
      }

   private:
      LocalNodeSPtr ln_;
      RemoteNodeSPtr rn_;
      Seid seid_;
      ULong seq_;
      MsgType mt_;
      Bool rqst_;
      UChar ver_;
      pUChar data_;
      UShort len_;
   };

   typedef InternalMsg *InternalMsgPtr;

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   class RspOut : public InternalMsg
   {
   public:
      RspOut()
         : am_(nullptr)
      {
      }
      RspOut(const RspOut &sr)
         : InternalMsg(sr)
      {
      }
      virtual ~RspOut()
      {
         if (am_ != nullptr)
            delete am_;
      }

      RspOut &operator=(const RspOut &ro)
      {
         InternalMsg::operator=(ro);
         am_ = ro.am_;
         return *this;
      }

      AppMsgRspPtr rsp() const              { return am_; }
      RspOut &setRsp(AppMsgRspPtr am)  { am_ = am; return *this; }

   private:
      AppMsgRspPtr am_;
   };

   typedef RspOut *RspOutPtr;

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   class RspIn : public InternalMsg
   {
   public:
      RspIn()
      {
      }
      RspIn(const RspIn &ri)
         : InternalMsg(ri),
           am_(ri.am_)
      {
      }
      virtual ~RspIn()
      {
      }

      RspIn &operator=(const RspIn &ri)
      {
         InternalMsg::operator=(ri);
         am_ = ri.am_;
         return *this;
      }

      AppMsgReqPtr req() const         { return am_; }
      RspIn &setReq(AppMsgReqPtr am)   { am_ = am; return *this; }

   private:
      AppMsgReqPtr am_;
   };

   typedef RspIn *RspInPtr;

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   class ReqOut : public InternalMsg
   {
   public:
      ReqOut()
         : InternalMsg(),
           am_(),
           n1_(Configuration::n1()),
           t1_(Configuration::t1()),
           t1id_(0),
           rw_(0)
      {
      }
      ReqOut(const ReqOut &ro)
         : InternalMsg(ro),
           am_(ro.am_),
           n1_(ro.n1_),
           t1_(ro.t1_),
           t1id_(ro.t1id_),
           rw_(ro.rw_)
      {
      }
      virtual ~ReqOut()
      {
      }

      ReqOut &operator=(const ReqOut &ro)
      {
         InternalMsg::operator=(ro);
         am_ = ro.am_;
         n1_ = ro.n1_;
         t1_ = ro.t1_;
         t1id_ = ro.t1id_;
         rw_ = ro.rw_;
         return *this;
      }

      AppMsgReqPtr appMsg()               { return am_; }
      ReqOut &setAppMsg(AppMsgReqPtr am)  { am_ = am; return *this; }

      Bool okToSnd()                      { if (n1_ < 1) return False; n1_--; return True; }

      Int n1() const                      { return n1_; }
      ReqOut &setN1(Int n1)               { n1_ = n1; return *this; }

      LongLong t1() const                 { return t1_; }
      ReqOut &setT1(LongLong t1)          { t1_ = t1; return *this; }

      Int rspWnd() const                  { return rw_; }
      ReqOut &setRspWnd(Int rw)           { rw_ = rw; return *this; }

      Void startT1();
      Void stopT1()
      {
         if (t1id_ > 0)
         {
            ETimerPool::Instance().unregisterTimer(t1id_);
            t1id_ = 0;
         }
      }

   private:
      AppMsgReqPtr am_;
      Int n1_;
      LongLong t1_;
      ULong t1id_;
      Int rw_;
   };

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   class ReqIn : public InternalMsg
   {
   public:
      ReqIn()
      {
      }
      ReqIn(const ReqIn &ri)
         : InternalMsg(ri)
      {
      }
      virtual ~ReqIn()
      {
      }
   };

   typedef ReqIn *ReqInPtr;

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

#if 0
   InternalMsg
      SEID
      seqNbr
      MsgType
      isReq
      buffer
      len
      application structure allocated by EncoderDecoder based on msg type

   RspOut
      InternalMsg

   RspIn
      InternalMsg
      AppMsgSPtr

   ReqOut
      InternalMsg
      AppMsgSPtr
      timerId
      sendCount
      rspWnd (0,1,2)

   ReqIn
      InternalMsg

   RcvdReq
      seqNbr
      rspWnd (0,1,2)


Snding a Req (ReqOut)
   add ReqOut object local Node retransmission list (seqnbr is key)
   set sendCount = 0
   perform "retransmission timer expiration" procedure
   start retransmission timer

   retransmission timer expiration
      sendCount >= N1
         send ReqMsgPtr to application layer indicating rsp timeout
         remove SentReq from retransmission list
      sendCount < N1
         send the req
         increment sendCount
         start expiration timer

Receiving a Req
   Lookup seqNbr in ReqIn list of remote Node
   entry found
      log duplicate req
      discard msg
   entry not found
      create RcvdReq with seqNbr, rspWnd=0
      add RcvdReq to remove Node RcvdReq list (seqNbr)
      send EncodedMsg to EcDc layer to decoding

Snding a Rsp
   Lookup sequence in ReqIn list of the remote node
   entry found
      update rspWnd in ReqIn to the current wnd
      send the msg
   entry not found
      discard msg(?)

Receiving a Rsp
   lookup ReqOut in local Node retransmission list (seqNbr)
   ReqOut found
      set ReqOut rspWnd to current wnd
      create RspIn and send to EcDc layer to decode
   ReqOut not found
      log error
      discard msg


Receiving a msg
   perform partial header parsing (seqNbr, isReq)
   isReq is TRUE
      go to Receiving a Req
   isReq is FALSE
      go to Receiving a Reponse

#endif

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   DECLARE_ERROR_ADVANCED(TeidRangeManager_InvalidRangeBits);
   inline TeidRangeManager_InvalidRangeBits::TeidRangeManager_InvalidRangeBits() {
      setText("The number of range bits must be between 0 and 7");
   }

   class TeidRangeManager
   {
   public:
      TeidRangeManager(Int rangeBits=0);

      Bool assign(RemoteNodeSPtr &n);
      Void release(RemoteNodeSPtr &n);

   private:
      Int bits_;
      std::list<Int> free_;
      std::unordered_map<Int,RemoteNodeSPtr> used_;
   };

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   class TranslatorMsgInfo
   {
   public:
      TranslatorMsgInfo()
         : seid_(0),
           mt_(0),
           sn_(0),
           ver_(0),
           rqst_(False)
      {
      }

      Seid seid() const                               { return seid_; }
      MsgType msgType() const                         { return mt_; }
      ULong seqNbr() const                            { return sn_; }
      UChar version() const                           { return ver_; }
      Bool isReq() const                              { return rqst_; }

      TranslatorMsgInfo &setSeid(const Seid seid)     { seid_ = seid; return *this; }
      TranslatorMsgInfo &setMsgType(const MsgType mt) { mt_ = mt; return *this; }
      TranslatorMsgInfo &setSeqNbr(const ULong sn)    { sn_ = sn; return *this; }
      TranslatorMsgInfo &setVersion(UChar ver)        { ver_ = ver; return *this; }
      TranslatorMsgInfo &setReq(Bool rqst)            { rqst_ = rqst; return *this; }

   private:
      Seid seid_;
      MsgType mt_;
      ULong sn_;
      UChar ver_;
      Bool rqst_;
   };

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   class Translator
   {
   public:
      Translator();
      virtual ~Translator();

      virtual ReqOutPtr encodeHeartbeatReq(SndHeartbeatReqData &hb) = 0;
      virtual RspOutPtr encodeHeartbeatRsp(SndHeartbeatRspData &hb) = 0;
      virtual RspOutPtr encodeVersionNotSupportedRsp(ReqInPtr msg) = 0;

      virtual ReqOutPtr encodeReq(AppMsgReqPtr msg) = 0;
      virtual RspOutPtr encodeRsp(AppMsgRspPtr msg) = 0;
      virtual AppMsgReqPtr decodeReq(ReqInPtr msg) = 0;
      virtual AppMsgRspPtr decodeRsp(RspInPtr msg) = 0;
      virtual RcvdHeartbeatReqDataPtr decodeHeartbeatReq(ReqInPtr msg) = 0;
      virtual RcvdHeartbeatRspDataPtr decodeHeartbeatRsp(RspInPtr msg) = 0;

      virtual Void getMsgInfo(TranslatorMsgInfo &info, cpUChar msg, Int len) = 0;
      virtual Bool isVersionSupported(UChar ver) = 0;

   protected:
      pUChar data() { return data_; }

   private:
      UChar data_[ESocket::UPD_MAX_MSG_LENGTH];
   };

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   DECLARE_ERROR(SndReqException);
   struct SndReqExceptionData
   {
      AppMsgReqPtr req;
      SndReqException err;
   };
   typedef SndReqExceptionData *SndReqExceptionDataPtr;

   DECLARE_ERROR(SndRspException);
   struct SndRspExceptionData
   {
      AppMsgRspPtr rsp;
      SndRspException err;
   };
   typedef SndRspExceptionData *SndRspExceptionDataPtr;

   DECLARE_ERROR(SndHeartbeatReqException);
   struct SndHeartbeatReqExceptionData
   {
      SndHeartbeatReqDataPtr req;
      SndHeartbeatReqException err;
   };
   typedef SndHeartbeatReqExceptionData *SndHeartbeatReqExceptionDataPtr;

   DECLARE_ERROR(SndHeartbeatRspException);
   struct SndHeartbeatRspExceptionData
   {
      SndHeartbeatRspDataPtr rsp;
      SndHeartbeatRspException err;
   };
   typedef SndHeartbeatRspExceptionData *SndHeartbeatRspExceptionDataPtr;

   DECLARE_ERROR(RcvdReqException);
   struct RcvdReqExceptionData
   {
      ReqInPtr req;
      RcvdReqException err;
   };
   typedef RcvdReqExceptionData *RcvdReqExceptionDataPtr;

   DECLARE_ERROR(RcvdRspException);
   struct RcvdRspExceptionData
   {
      RspInPtr rsp;
      RcvdRspException err;
   };
   typedef RcvdRspExceptionData *RcvdRspExceptionDataPtr;

   DECLARE_ERROR(EncodeReqException);
   struct EncodeReqExceptionData
   {
      AppMsgReqPtr req;
      EncodeReqException err;
   };
   typedef EncodeReqExceptionData *EncodeReqExceptionDataPtr;

   DECLARE_ERROR(EncodeRspException);
   struct EncodeRspExceptionData
   {
      AppMsgRspPtr rsp;
      EncodeRspException err;
   };
   typedef EncodeRspExceptionData *EncodeRspExceptionDataPtr;

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   DECLARE_ERROR(ApplicationThread_UnrecognizedAddressFamily);

   #define APPLICATION_BASE_EVENT (EM_USER + 10000)

   class ApplicationThread : public EThreadPrivate
   {
      friend Void Uninitialize();
   public:
      enum class Events : UInt
      {
         RcvdReq              = (APPLICATION_BASE_EVENT + 1),     // TranslationThread --> ApplicationThread - AppMsgReqPtr
         RcvdRsp              = (APPLICATION_BASE_EVENT + 2),     // TranslationThread --> ApplicationThread - AppMsgRspPtr
         ReqTimeout           = (APPLICATION_BASE_EVENT + 3),     // CommunicationThread --> ApplicationThread - AppMsgReqPtr
         RemoteNodeAdded      = (APPLICATION_BASE_EVENT + 4),     // CommunicationThread --> ApplicationThread - *RemoteNodeSPtr
         RemoteNodeFailure    = (APPLICATION_BASE_EVENT + 5),     // CommunicationThread --> ApplicationThread - *RemoteNodeSPtr
         RemoteNodeRestart    = (APPLICATION_BASE_EVENT + 6),     // CommunicationThread --> ApplicationThread - *RemoteNodeSPtr
         RemoteNodeRemoved    = (APPLICATION_BASE_EVENT + 7),     // CommunicationThread --> ApplicationThread - *RemoteNodeSPtr
         SndReqError          = (APPLICATION_BASE_EVENT + 8),     // CommunicationThread --> ApplicationThread - SndReqExceptionDataPtr
         SndRspError          = (APPLICATION_BASE_EVENT + 9),     // CommunicationThread --> ApplicationThread - SndRspExceptionDataPtr
         EncodeReqError       = (APPLICATION_BASE_EVENT + 10),    // TranslationThread --> ApplicationThread - EncodeReqExceptionDataPtr
         EncodeRspError       = (APPLICATION_BASE_EVENT + 11)     // TranslationThread --> ApplicationThread - EncodeRspExceptionDataPtr
      };

      static ApplicationThread &Instance() { return *this_; }

      Void onInit();
      Void onQuit();

      virtual Void onRcvdReq(AppMsgReqPtr req);
      virtual Void onRcvdRsp(AppMsgRspPtr rsp);
      virtual Void onReqTimeout(AppMsgReqPtr req);
      virtual Void onRemoteNodeAdded(RemoteNodeSPtr &rmtNode);
      virtual Void onRemoteNodeFailure(RemoteNodeSPtr &rmtNode);
      virtual Void onRemoteNodeRestart(RemoteNodeSPtr &rmtNode);
      virtual Void onRemoteNodeRemoved(RemoteNodeSPtr &rmtNode);
      virtual Void onSndReqError(AppMsgReqPtr req, SndReqException &err);
      virtual Void onSndRspError(AppMsgRspPtr rsp, SndRspException &err);
      virtual Void onEncodeReqError(AppMsgReqPtr req, EncodeReqException &err);
      virtual Void onEncodeRspError(AppMsgRspPtr rsp, EncodeRspException &err);

      LocalNodeSPtr createLocalNode(cpStr ipaddr, UShort port)
      {
         ESocket::Address addr(ipaddr, port);
         return createLocalNode(addr);
      }
      LocalNodeSPtr createLocalNode(EIpAddress ipaddr, UShort port)
      {
         ESocket::Address addr;
         switch (ipaddr.family())
         {
            case AF_INET:  { addr.setAddress(ipaddr.ipv4Address(), port); break; }
            case AF_INET6: { addr.setAddress(ipaddr.ipv6Address(), port); break; }
            default:
            {
               throw ApplicationThread_UnrecognizedAddressFamily();
            }
         }
         return createLocalNode(addr);
      }
      LocalNodeSPtr createLocalNode(ESocket::Address &addr);

      DECLARE_MESSAGE_MAP()

   protected:
      ApplicationThread();
      ~ApplicationThread();

      Void releaseLocalNodes();

      Void onRcvdReq(EThreadMessage &msg);
      Void onRcvdRsp(EThreadMessage &msg);
      Void onReqTimeout(EThreadMessage &msg);
      Void onRemoteNodeAdded(EThreadMessage &msg);
      Void onRemoteNodeFailure(EThreadMessage &msg);
      Void onRemoteNodeRestart(EThreadMessage &msg);
      Void onRemoteNodeRemoved(EThreadMessage &msg);
      Void onSndReqError(EThreadMessage &msg);
      Void onSndRspError(EThreadMessage &msg);
      Void onEncodeReqError(EThreadMessage &msg);
      Void onEncodeRspError(EThreadMessage &msg);

   private:
      static ApplicationThread *this_;
   };

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   #define TRANSLATION_BASE_EVENT (EM_USER + 20000)

   class TranslationThread : public EThreadPrivate
   {
      friend Void Uninitialize();
      friend AppMsgReq;
      friend AppMsgRsp;
   public:
      enum class Events : UInt
      {
         SndMsg            = (TRANSLATION_BASE_EVENT + 1),  // ApplicationThread --> TranslationThread - AppMsgPtr
         RcvdReq           = (TRANSLATION_BASE_EVENT + 2),  // CommunicationThread --> TranslationThread - ReqInPtr
         RcvdRsp           = (TRANSLATION_BASE_EVENT + 3),  // CommunicationThread --> TranslationThread - RspInPtr
         SndHeartbeatReq   = (TRANSLATION_BASE_EVENT + 4),  // CommunicationThread --> TranslationThread - SndHeartbeatReqDataPtr
         SndHeartbeatRsp   = (TRANSLATION_BASE_EVENT + 5)   // CommunicationThread --> TranslationThread - SndHeartbeatRspDataPtr
      };

      ~TranslationThread();

      static TranslationThread &Instance()
      {
         if (this_ == nullptr)
            this_ = new TranslationThread();
         return *this_;
      }

      Void onInit();
      Void onQuit();

      // virtual Void onRcv(AppMsgPtr &am);

      DECLARE_MESSAGE_MAP()

   protected:
      Void onSndPfcpMsg(EThreadMessage &msg);
      Void onRcvdReq(EThreadMessage &msg);
      Void onRcvdRsp(EThreadMessage &msg);
      Void onSndHeartbeatReq(EThreadMessage &msg);
      Void onSndHeartbeatRsp(EThreadMessage &msg);

      // static Bool send(AppMsgReqPtr &amrq)
      // {
      //    return this_->sendMsg(new EThreadMessage(TRANSLATE_SEND_REQUEST_EVENT, amrq.release()));
      // }

      // static Bool send(AppMsgRspUPtr &amrs)
      // {
      //    return this_->sendMsg(new EThreadMessage(TRANSLATE_SEND_RESPONSE_EVENT, amrs.release()));
      // }

      static Void cleanup()
      {
         delete this_;
         this_ = NULL;
      }

   private:
      static TranslationThread *this_;
      TranslationThread();
      Translator &xlator_;
   };

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   #define COMMUNICATION_BASE_EVENT (EM_USER + 30000)

   class CommunicationThread : public ESocket::ThreadPrivate
   {
      friend Void Uninitialize();
      friend TranslationThread;
   public:
      enum class Events : UInt
      {
         SndReq               = (COMMUNICATION_BASE_EVENT + 1),   // TranslationThread --> CommunicationThread - ReqOutPtr
         SndRsp               = (COMMUNICATION_BASE_EVENT + 2),   // TranslationThread --> CommunicationThread - RspOutPtr
         HeartbeatReq         = (COMMUNICATION_BASE_EVENT + 3),   // TranslationThread --> CommunicationThread - RcvdHeartbeatReqDataPtr
         HeartbeatRsp         = (COMMUNICATION_BASE_EVENT + 4),   // TranslationThread --> CommunicationThread - SndHeartbeatRspDataPtr
         SndHeartbeatReqError = (COMMUNICATION_BASE_EVENT + 5),   // TranslationThread --> CommunicationThread - SndHeartbeatReqExceptionDataPtr - (Translator failure to create Heartbeat Req)
         SndHeartbeatRspError = (COMMUNICATION_BASE_EVENT + 6),   // TranslationThread --> CommunicationThread - SndHeartbeatRspExceptionDataPtr - (Translator failure to create Heartbeat Rsp)
         RcvdReqError         = (COMMUNICATION_BASE_EVENT + 7),   // TranslationThread --> CommunicationThread - RcvdReqExceptionDataPtr - (Translator failure to parse req)
         RcvdRspError         = (COMMUNICATION_BASE_EVENT + 8),   // TranslationThread --> CommunicationThread - RcvdRspExceptionDataPtr - (Translator failure to parse resposne)
         ReqTimeout           = (COMMUNICATION_BASE_EVENT + 9)    // ETimerPool --> CommunicationThread - ReqOutPtr
      };

      ~CommunicationThread();

      static CommunicationThread &Instance()
      {
         if (this_ == nullptr)
            this_ = new CommunicationThread();
         return *this_;
      }

      Void onInit();
      Void onQuit();
      Void onTimer(EThreadEventTimer *ptimer);

      Void errorHandler(EError &err, ESocket::BasePrivate *psocket);

      const ESocket::Address &address()                                 { return address_; }
      CommunicationThread &setAddress(const ESocket::Address &address)  { address_ = address; return *this; }
      ESocket::Address &setAddress()                                    { return address_; }

      Bool addLocalNode(LocalNodeSPtr &ln)
      {
         EWRLock lck(lnslck_);
         return lns_.insert(std::make_pair(ln->ipAddress(), ln)).second;
      }

      Bool assignTeidRangeValue(RemoteNodeSPtr &rn)
      {
         return trm_.assign(rn);
      }

      Void releaseTeidRangeValue(RemoteNodeSPtr &rn)
      {
         trm_.release(rn);
      }

      LocalNodeSPtr createLocalNode(ESocket::Address &addr);

      LocalNodeUMap &localNodes()                                       { return lns_; }

      Void setNbrActivityWnds(size_t nbr);
      Void nextActivityWnd();
      size_t currentActivityWnd() const                                 { return caw_; }

      Int currentRspWnd() const                                         { return crw_; }

      DECLARE_MESSAGE_MAP()

   protected:
      Void releaseLocalNodes();

      Void onSndReq(EThreadMessage &msg);
      Void onSndRsp(EThreadMessage &msg);
      Void onHeartbeatReq(EThreadMessage &msg);
      Void onHeartbeatRsp(EThreadMessage &msg);
      Void onSndHeartbeatReqError(EThreadMessage &msg);
      Void onSndHeartbeatRspError(EThreadMessage &msg);
      Void onRcvdReqError(EThreadMessage &msg);
      Void onRcvdRspError(EThreadMessage &msg);
      Void onReqTimeout(EThreadMessage &msg);

      Void onHeartbeatReqTimtout(AppMsgReqPtr am);

      static Void cleanup()
      {
         delete this_;
         this_ = NULL;
      }

   private:
      static const Int rwOne_ = 1;
      static const Int rwTwo_ = 2;
      static const Int rwToggle_ = (rwOne_ ^ rwTwo_);
      static CommunicationThread *this_;
      CommunicationThread();
      ESocket::Address address_;
      TeidRangeManager trm_;
      ERWLock lnslck_;
      LocalNodeUMap lns_;
      EThreadEventTimer atmr_;
      EThreadEventTimer rsptmr_;
      size_t caw_;
      Int crw_;
   };

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   inline Void ReqOut::startT1()
   {
      stopT1();
      EThreadMessage *msg = new EThreadMessage(
         static_cast<UInt>(CommunicationThread::Events::ReqTimeout),
         static_cast<pVoid>(this));
      t1id_ = ETimerPool::Instance().registerTimer(t1(), msg, CommunicationThread::Instance());
   }

   inline Bool RemoteNode::setRcvdReqRspWnd(ULong sn)
   {
      return setRcvdReqRspWnd(sn, CommunicationThread::Instance().currentRspWnd());
   }

   inline LocalNodeSPtr ApplicationThread::createLocalNode(ESocket::Address &addr)
   {
      return CommunicationThread::Instance().createLocalNode(addr);
   }
}

namespace std
{
template <>
struct hash<PFCP::RemoteNodeSPtr>
{
   std::size_t operator()(const PFCP::RemoteNodeSPtr &rn) const noexcept
   {
      return std::hash<ESocket::Address>{}(rn->address());
   }
};
}

#endif // #ifndef __EPFCP_H