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

/// @file
/// @brief Contains the class definitions to support the PFCP protocol stack.

#include <atomic>
#include <unordered_map>
#include <utility>

#include "epctools.h"
#include "eip.h"
#include "esocket.h"
#include "eteid.h"
#include "etimerpool.h"
#include "ememory.h"

/// @brief PFCP stack namespace
namespace PFCP
{
   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   /// @brief Initializes/starts the PFCP stack.  This should be called after setting
   ///   the initial configuration values.
   Void Initialize();
   /// @brief Uninitializes/stops the PFCP stack.
   Void Uninitialize();

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   /// @cond DOXYGEN_EXCLUDE

   class ApplicationWorkGroupBase;

   class Translator;
   template<class TWorker> class ApplicationWorkGroup;
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
      (PFCP::Configuration::threadApplication()._sendThreadMessage(EThreadMessage(        \
         static_cast<UInt>(PFCP::ApplicationEvents::a),static_cast<pVoid>(b))))
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

   enum class MsgClass 
   {
      Unknown,
      Node,
      Session
   };

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   class EventBase
   {
   public:
      static void* operator new(size_t sz);
      static void operator delete(void* m);
   private:
      static EMemory::Pool pool_;
   };

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   DECLARE_ERROR(Configuration_LoggerNotDefined);
   DECLARE_ERROR(Configuration_TranslatorNotDefined);
   DECLARE_ERROR(Configuration_ApplicationNotDefined);

   /// @endcond

   /// @brief Contains all of the configuration values used in the PFCP stack.
   class Configuration
   {
      friend LocalNode;
      friend RemoteNode;
      friend TranslationThread;
      friend CommunicationThread;
   public:
      static UShort port()                                           { return port_; }
      static UShort setPort(UShort port)                             { return port_ = port; }

      static Int socketBufferSize()                                  { return bufsize_; }
      static Int setSocketBufferSize(Int sz)                         { return bufsize_ = sz; }

      static LongLong t1()                                           { return t1_; }
      static LongLong setT1(LongLong t1)                             { return t1_ = t1; }

      static LongLong heartbeatT1()                                  { return hbt1_; }
      static LongLong setHeartbeatT1(LongLong hbt1)                  { return hbt1_ = hbt1; }

      static Int n1()                                                { return n1_; }
      static Int setN1(Int n1)                                       { return n1_ = n1; }

      static Int heartbeatN1()                                       { return hbn1_; }
      static Int setHeartbeatN1(Int hbn1)                            { return hbn1_ = hbn1; }

      static Long maxRspWait()                                       { return static_cast<Long>(std::max(t1_,hbt1_) * std::max(n1_,hbn1_)); }

      static size_t nbrActivityWnds()                                { return naw_; }
      static size_t setNnbrActivityWnds(size_t naw)                  { return naw_ = naw; }

      static Long lenActivityWnd()                                   { return law_; }
      static Long setLenActivityWnd(Long law)                        { return law_ = law; }

      static ELogger &logger()                                       { if (logger_ == nullptr) throw Configuration_LoggerNotDefined(); return *logger_; }
      static ELogger &setLogger(ELogger &log)                        { logger_ = &log; return *logger_; }

      static Bool assignTeidRange()                                  { return atr_; }
      static Bool setAssignTeidRange(Bool atr)                       { return atr_ = atr; }

      static Int teidRangeBits()                                     { return trb_; }
      static Int setTeidRangeBits(Int trb)                           { return trb_ = trb; }

      static Translator &translator()                                { if (xlator_ == nullptr) throw Configuration_TranslatorNotDefined(); return *xlator_; }
      static Translator &setTranslator(Translator &xlator)           { return *(xlator_ = &xlator); }

      static Int minApplicationWorkers()                             { return aminw_; }
      static Int setMinApplicationWorkers(Int w)                     { return aminw_ = w; }

      static Int maxApplicationWorkers()                             { return amaxw_; }
      static Int setMaxApplicationWorkers(Int w)                     { return amaxw_ = w; }

      static Int minTranslatorWorkers()                              { return tminw_; }
      static Int setMinTranslatorWorkers(Int w)                      { return tminw_ = w; }

      static Int maxTranslatorWorkers()                              { return tmaxw_; }
      static Int setMaxTranslatorWorkers(Int w)                      { return tmaxw_ = w; }

      template<class TWorker>
      static Void setApplication(ApplicationWorkGroup<TWorker> &app) { app_ = &app; baseapp_ = &app; }

      static MsgType pfcpHeartbeatReq;
      static MsgType pfcpHeartbeatRsp;
      static MsgType pfcpSessionEstablishmentReq;
      static MsgType pfcpSessionEstablishmentRsp;
      static MsgType pfcpAssociationSetupReq;
      static MsgType pfcpAssociationSetupRsp;

   protected:
      static _EThreadEventNotification &threadApplication()          { if (app_ == nullptr) throw Configuration_ApplicationNotDefined(); return *app_; }
      static ApplicationWorkGroupBase &baseApplication()             { if (baseapp_ == nullptr) throw Configuration_ApplicationNotDefined(); return *baseapp_; }

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
      static Int aminw_;
      static Int amaxw_;
      static Int tminw_;
      static Int tmaxw_;
      static _EThreadEventNotification *app_;
      static ApplicationWorkGroupBase *baseapp_;
   };

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   /// @brief Allocates and deallocates sequence numbers used in PFCP request
   ///   messages.  Each "slice" should have it's own instance of this object.
   class SequenceManager
   {
   public:
      /// @brief Default constructor.
      SequenceManager(): next_(SEQUENCE_MIN) {}
      /// @brief Assigns the next available sequence number.  This operation
      ///   is thread safe.
      ULong alloc()
      {
         unsigned long sn;
         while ((sn = next_++) > SEQUENCE_MAX)
            next_.compare_exchange_strong(++sn, SEQUENCE_MIN);
         return sn;
      }
      /// @brief Releases a previously allocated sequence number.
      Void free(ULong sn)
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

   /// @brief Allocates and deallocates SEID's (PFCP session ID's).  Each "slice"
   ///   should have it's own instance of this object.
   class SeidManager
   {
   public:
      /// @brief Default constructor.
      SeidManager() : next_(SEID_MINIMUM) {}
      /// @brief Assigns the next available SEID.  This operation is thread safe.
      Seid alloc()
      {
         Seid seid;
         // this logic depends on the numeric overflow from a max value to zero
         // when the value is incremented.
         while ((seid = next_++) < SEID_MINIMUM);
         return seid;
      }
      /// @brief Releases a previously allocated SEID.
      Void free(Seid seid)
      {
         // currently, nothing is done to "free" a SEID
      }
   private:
      static const Seid SEID_MINIMUM = 1;
      std::atomic_ullong next_;
   };

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   /// @brief Represents a Fully Qualified SEID.  This combines an IP address
   ///   with a SEID.
   class FqSeid
   {
   public:
      /// @brief Default constructor.
      FqSeid()
         : addr_(),
           seid_(0)
      {
      }
      /// @brief Copy constructor.
      /// @param fqseid the FqSeid object to copy.
      FqSeid(const FqSeid &fqseid)
         : addr_(fqseid.addr_),
           seid_(fqseid.seid_)
      {
      }

      /// @brief Assignment operator.
      /// @param fqseid the FqSeid object to copy.
      FqSeid &operator=(const FqSeid &fqseid)
      {
         addr_ = fqseid.addr_;
         seid_ = fqseid.seid_;
         return *this;
      }

      /// @brief Retrieves the IP address object associated with the FqSeid.
      /// @return the IP address object associated with the FqSeid.
      const EIpAddress &address() const   { return addr_; }
      /// @brief Returns a modifiable reference to the IP address.
      /// @return a reference to the IP address object.
      EIpAddress &setAddress()            { return addr_; }

      /// @brief Returns the SEID.
      /// @return the SEID.
      const Seid seid() const             { return seid_; }
      /// @brief Assigns the SEID value.
      /// @param seid the SEID value to assign.
      /// @return a reference to this object.
      FqSeid &setSeid(Seid seid)          { seid_ = seid; return *this; }

   private:
      EIpAddress addr_;
      Seid seid_;
   };

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   /// @cond DOXYGEN_EXCLUDE
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
   /// @endcond

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   /// @brief Encapsulates the UDP Socket functionality used to communicate with
   ///   a PFCP peer.
   class NodeSocket : public ESocket::UdpPrivate
   {
      friend class LocalNode;
   public:
      /// @brief Default constructor.
      NodeSocket();
      /// @brief Class destructor.
      virtual ~NodeSocket();

      /// @brief Assigns the LocalNode that this socket is associated with.
      /// @param ln a shared pointer to the LocalNode object.
      /// @return a reference to this object.
      NodeSocket &setLocalNode(LocalNodeSPtr &ln)  { ln_ = ln; return *this; }
      /// @brief Clears the assigned LocalNode.
      /// @return a reference to this object.
      NodeSocket &clearLocalNode()                 { ln_.reset(); return *this; }

   protected:
      /// @brief Called by the ESocket infrastructure when a UDP message is
      ///   received.
      /// @param src the originating message IP address.
      /// @param dst the destination (local) IP address where the message was received.
      /// @param data a pointer to the received data buffer.
      /// @param len the number of bytes received (and present in the buffer).
      Void onReceive(const ESocket::Address &src, const ESocket::Address &dst, cpUChar data, Int len);
      /// @brief Called by the ESocket infrastructure when an error has been
      ///   detected on the socket.
      Void onError();

   private:
      LocalNodeSPtr ln_;
   };

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   DECLARE_ERROR(SessionBase_LocalSeidAlreadySet);
   DECLARE_ERROR(SessionBase_RemoteSeidAlreadySet);

   class SessionBase;
   typedef std::shared_ptr<SessionBase> SessionBaseSPtr;

   /// @brief Represents a PFCP session.  It is expected that a developer
   ///   utilizing this library will derive their own specialized session
   ///   object from this class.
   class SessionBase
   {
   public:
      /// @brief Class constructor.
      /// @param ln the LocalNode for this session.
      /// @param rn the RemoteNode for this session.
      SessionBase(LocalNodeSPtr &ln, RemoteNodeSPtr &rn)
         : ln_(ln),
           rn_(rn),
           ls_(0),
           rs_(0)
      {
         created_++;
      }
      /// @brief Copy constructor.
      /// @param s the SessionBase object to copy.
      SessionBase(const SessionBase &s)
         : ln_(s.ln_),
           rn_(s.rn_),
           ls_(s.ls_),
           rs_(s.rs_)
      {
         created_++;
      }
      /// @brief Class destructor.
      virtual ~SessionBase();

      /// @brief Returns the LocalNode object associated with this session.
      /// @return the LocalNode object associated with this session.
      LocalNodeSPtr &localNode()    { return ln_; }
      /// @brief Returns the RemoteNode object associated with this session.
      /// @return the RemoteNode object associated with this session.
      RemoteNodeSPtr &remoteNode()  { return rn_; }
      /// @brief Returns the SEID associated with this session.
      /// @return the SEID associated with this session.
      const Seid localSeid() const       { return ls_; }
      /// @brief Returns the remote SEID associated with this session.
      /// @return the remote SEID associated with this session.
      const Seid remoteSeid() const { return rs_; }
      /// @brief Sets the local and remote SEID for this session.
      /// @param s a shared pointer associated with this session object.
      /// @param ls the local SEID.
      /// @param rs the remote SEID.
      /// @return a reference to this object.
      SessionBase &setSeid(SessionBaseSPtr &s, Seid ls, Seid rs, Bool notify = True);
      /// @brief Sets the local SEID for this session.
      /// @param s a shared pointer associated with this session object.
      /// @param ls the local SEID.
      /// @return a reference to this object.
      SessionBase &setLocalSeid(SessionBaseSPtr &s, Seid ls)   { return setSeid(s, ls, 0); }
      /// @brief Sets the remote SEID for this session.
      /// @param s a shared pointer associated with this session object.
      /// @param rs the remote SEID.
      /// @return a reference to this object.
      SessionBase &setRemoteSeid(SessionBaseSPtr &s, Seid rs)  { return setSeid(s, 0, rs); }
      /// @brief Starts the destruction process for the session.
      virtual Void destroy(SessionBaseSPtr &s);

      static ULongLong sessionsCreated() { return created_; }
      static ULongLong sessionsDeleted() { return deleted_; }

      static void* operator new(size_t sz)
      {
         if (pool_.allocSize() == 0)
         {
            if (sz >= (32768 - sizeof(EMemory::Node)))
            {
               pool_.setSize(sz, 0, 5);
            }
            else
            {
               size_t ns = 32768 - sizeof(EMemory::Node);
               size_t bs = sz + sizeof(EMemory::Block);
               bs += bs % sizeof(pVoid);
               size_t bc = ns / bs;
               if (bc < 5)
               {
                  pool_.setSize(sz, 0, 5);
               }
               else
               {
                  ns = sizeof(EMemory::Node) + bc * bs;
                  pool_.setSize(sz, ns);
               }
            }
         }
         if (sz > pool_.allocSize())
         {
            EError ex;
            ex.setSevere();
            ex.setText("session allocation size is larger than memory pool block size");
            throw ex;
         }
         return pool_.allocate();
      }
      static void operator delete(void* m)
      {
         pool_.deallocate(m);
      }
private:                                                 \

   private:
      SessionBase();
      static EMemory::Pool pool_;
      static ULongLong created_;
      static ULongLong deleted_;
      LocalNodeSPtr ln_;
      RemoteNodeSPtr rn_;
      Seid ls_;
      Seid rs_;
   };
   typedef std::unordered_map<Seid,SessionBaseSPtr> SessionBaseSPtrUMap;

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   /// @brief Contains the basic Node functionality common to both a LocalNode
   ///   and a RemoteNode.
   class Node
   {
   public:
      /// @brief Default constructor.
      Node() { ++created_; }
      /// @brief Class destructor.
      virtual ~Node() { ++deleted_; }

      /// @brief Returns the IP address associated with this node.
      /// @return the IP address associated with this node.
      EIpAddress &ipAddress()                            { return ipaddr_; }
      /// @brief Assigns the IP address for this node.
      /// @param ipaddr the IP address for this node.
      /// @return a reference to this object.
      Node &setIpAddress(const EIpAddress &ipaddr)       {  ipaddr_ = ipaddr; return *this; }

      /// @brief Returns the ESocket::Address object representing the IP
      ///   address for this node.
      /// @return the ESocket::Address object representing the IP address
      ///   for this node.
      ESocket::Address &address()                        { return addr_; }
      /// @brief Assigns the ESocket::Address object representing the IP
      ///   address for this node.
      /// @param addr a reference to the ESocket::Address to assign.
      /// @return a reference to this object.
      Node &setAddress(const ESocket::Address &addr)
      {
          addr_ = addr;
          ipaddr_ = addr.getSockAddrStorage();
          return *this;
      }

      /// @brief Returns the Node start time.
      /// @return the Node start time.
      const ETime &startTime() const                     { return st_; }
      /// @brief Assigns the Node start time.
      /// @param st the start time object to assign.
      /// @return a reference to this object.
      Node &setStartTime(const ETime &st = ETime::Now()) { st_ = st; return *this; }

      /// @brief Returns the session object for the specified SEID.
      /// @param seid the SEID of the session to return.
      /// @return the session object.
      SessionBaseSPtr getSession(Seid seid)
      {
         auto it = sessions_.find(seid);
         if (it == sessions_.end())
            return SessionBaseSPtr();
         return it->second;
      }

      static ULongLong nodesCreated() { return created_; }
      static ULongLong nodesDeleted() { return deleted_; }

   protected:
      /// @cond DOXYGEN_EXCLUDE
      Node &_addSession(Seid seid, SessionBaseSPtr &session)
      {
         auto it = sessions_.find(seid);
         if (it == sessions_.end())
            sessions_[seid] = session;
         return *this;
      }
      Node &_delSession(Seid seid)
      {
         if (seid != 0)
            sessions_.erase(seid);
         return *this;
      }
      SessionBaseSPtr getFirstSession()
      {
         if (sessions_.empty())
            return SessionBaseSPtr();
         return sessions_.begin()->second;
      }
      /// @endcond

   private:
      static ULongLong created_;
      static ULongLong deleted_;

      ETime st_;
      EIpAddress ipaddr_;
      ESocket::Address addr_;
      SessionBaseSPtrUMap sessions_;
   };

   typedef std::shared_ptr<Node> NodeSPtr;

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   /// @cond DOXYGEN_EXCLUDE
   DECLARE_ERROR(RemoteNodeException_UnableToAssignTeidRangeValue);
   /// @endcond

   /// @brief Represents a remote or peer PFCP node or host.
   class RemoteNode : public Node
   {
      friend LocalNode;
      friend CommunicationThread;
   public:
      enum class State { Initialized, Started, Stopping, Stopped, Failed, Restarted };

      /// @brief Defalut constructor.
      RemoteNode();
      /// @brief Class destructor.
      virtual ~RemoteNode();

      /// @brief Returns the currently configured TEID range value.
      /// @return the currently configured TEID range value.
      Int teidRangeValue() const                            { return trv_; }
      /// @brief Sets the TEID range value.
      /// @param trv the new TEID range value.
      /// @return a reference to this object.
      RemoteNode &setTeidRangeValue(Int trv)                { trv_ = trv; return *this; }

      /// @brief Sets the number of activity windows.
      /// @param nbr the number of activity windows.
      /// @return a reference to this object.
      RemoteNode &setNbrActivityWnds(size_t nbr);

      /// @brief Initiates the process to delete all of the sessions
      ///   associated with this remote node.
      /// @param rn a reference to the RemoteNode shared pointer to this object.
      /// @return a reference to this object.
      RemoteNode &deleteAllSesssions(RemoteNodeSPtr &rn);

      /// @brief Disconnects the remote peer and deletes all associated sessions.
      /// @param rn a reference to the RemoteNode shared pointer to this object.
      /// @return a reference to this object.
      RemoteNode &disconnect(RemoteNodeSPtr &rn);

      /// @brief Returns the current state of the local node.
      /// @return the current state of the local node.
      State state() const { return state_; }

   protected:
      /// @cond DOXYGEN_EXCLUDE
      RemoteNode &changeState(RemoteNodeSPtr &rn, State state);
      RemoteNode &restarted(RemoteNodeSPtr &rn, const ETime &restartTime);
      Bool addRcvdReq(ULong sn);
      Bool delRcvdReq(ULong sn)                             { return rrumap_.erase(sn) == 1; }
      Bool setRcvdReqRspWnd(ULong sn, Int wnd);
      Bool setRcvdReqRspWnd(ULong sn);
      Void removeRcvdRqstEntries(Int wnd);
      Bool rcvdReqExists(ULong sn) const                    { return rrumap_.find(sn) != rrumap_.end(); }

      Void nextActivityWnd(Int wnd);
      Bool checkActivity();
      Void incrementActivity()                              { awnds_[aw_]++; }

      Void removeOldReqs(Int rw);

      RemoteNode &addSession(SessionBaseSPtr &s);
      RemoteNode &delSession(SessionBaseSPtr &s);
      /// @endcond

   private:
      State state_;
      ESocket::Address addr_;
      Int trv_;
      RcvdReqUMap rrumap_;
      std::vector<ULong> awnds_;
      size_t awndcnt_;
      size_t aw_;
   };
   typedef std::unordered_map<EIpAddress,RemoteNodeSPtr> RemoteNodeUMap;
   typedef std::pair<EIpAddress,RemoteNodeSPtr> RemoteNodeUMapPair;

   class RemoteNodeStateChangeEvent : public EventBase
   {
   public:
      RemoteNodeStateChangeEvent(RemoteNodeSPtr &rn, RemoteNode::State oldst, RemoteNode::State newst)
         : rn_(rn),
           os_(oldst),
           ns_(newst)
      {
      }

      RemoteNodeSPtr &remoteNode() { return rn_; }
      RemoteNode::State oldState() const { return os_; }
      RemoteNode::State newState() const { return ns_; }

   private:
      RemoteNodeStateChangeEvent();
      RemoteNodeSPtr rn_;
      RemoteNode::State os_;
      RemoteNode::State ns_;
   };

   class RemoteNodeRestartEvent : public EventBase
   {
   public:
      RemoteNodeRestartEvent(RemoteNodeSPtr &rn, RemoteNode::State oldst,
         RemoteNode::State newst, const ETime &restartTime)
         : rn_(rn),
           os_(oldst),
           ns_(newst),
           rt_(restartTime)
      {
      }

      RemoteNodeSPtr &remoteNode() { return rn_; }
      RemoteNode::State oldState() const { return os_; }
      RemoteNode::State newState() const { return ns_; }
      const ETime &restartTime() const { return rt_; }

   private:
      RemoteNodeRestartEvent();
      RemoteNodeSPtr rn_;
      RemoteNode::State os_;
      RemoteNode::State ns_;
      ETime rt_;
   };

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   /// @cond DOXYGEN_EXCLUDE
   DECLARE_ERROR(LocalNodeException_UnableToCreateRemoteNode);
   DECLARE_ERROR(LocalNodeException_RemoteNodeUMapInsertFailed);
   /// @endcond

   /// @brief Represents the local PFCP node.
   class LocalNode : public Node
   {
      friend NodeSocket;
      friend CommunicationThread;
   public:
      enum class State { Initialized, Started, Stopping, Stopped };

      /// @brief Default constructor.
      LocalNode();
      /// @brief Class destructor.
      virtual ~LocalNode();

      /// @brief Gets the state of the local node.
      /// @return the state of the local node.
      State state() { return state_; }

      /// @brief Creates a new SEID.
      /// @return the newly created SEID.
      Seid allocSeid();
      /// @brief Deallocates a SEID.
      /// @param seid the SEID to deallocate.
      Void freeSeid(Seid seid);
      /// @brief Allocates a message sequence number for a request message.
      /// @return the newly created sequence number.
      ULong allocSeqNbr();
      /// @brief Deallocates a sequence number.
      /// @param sn the sequence number to deallocate.
      Void freeSeqNbr(ULong sn);

      /// @brief Sets the number of activity windows.
      /// @param nbr the number of activity windows.
      Void setNbrActivityWnds(size_t nbr);

      /// @brief Creates a remote Node that this local node will "talk" to.
      /// @param addr the IP address of the remote host.
      /// @param port the port that messages will be sent to.
      RemoteNodeSPtr createRemoteNode(cpStr addr, UShort port)
      {
         EIpAddress ipaddr(addr);
         return createRemoteNode(ipaddr, port);
      }
      /// @brief Creates a remote Node that this local node will "talk" to.
      /// @param address the IP address of the remote host.
      /// @param port the port that messages will be sent to.
      /// @return a reference to this object.
      RemoteNodeSPtr createRemoteNode(EIpAddress &address, UShort port);

      /// @brief Creates a new Session object and registers it with the local
      ///   and remote nodes.
      /// @param ln the local node shared pointer.
      /// @param rn the remote node shared pointer.
      /// @return a shared pointer to the newly created session object.
      SessionBaseSPtr createSession(LocalNodeSPtr &ln, RemoteNodeSPtr &rn);

      /// @brief Returns a reference to the underlying socket object for this local host.
      /// @return a reference to the underlying socket object.
      NodeSocket &socket() { return socket_; }

      /// @brief Returns the current state of the local node.
      /// @return the current state of the local node.
      State state() const { return state_; }

   protected:

      /// @cond DOXYGEN_EXCLUDE
      LocalNode &changeState(LocalNodeSPtr &ln, State state);
      Bool rqstOutExists(ULong seqnbr) const;
      Bool addRqstOut(ReqOut *ro);
      Bool setRqstOutRespWnd(ULong seqnbr, Int wnd);
      Void removeRqstOutEntries(Int wnd);
      Void clearRqstOutEntries();

      Void nextActivityWnd(Int wnd);
      Void checkActivity(LocalNodeSPtr &ln);

      Void onReceive(LocalNodeSPtr &ln, const ESocket::Address &src, const ESocket::Address &dst, cpUChar msg, Int len);
      Bool onReqOutTimeout(ReqOutPtr ro);
      Void removeOldReqs(Int rw);

      Void sndInitialReq(ReqOutPtr ro);
      Bool sndReq(ReqOutPtr ro);
      Void sndRsp(RspOutPtr ro);

      LocalNode &addSession(SessionBaseSPtr &s)
      {
         if (s && s->localSeid() != 0)
            _addSession(s->localSeid(), s);
         return *this;
      }
      LocalNode &delSession(SessionBaseSPtr &s)
      {
         if (s && s->localSeid() != 0)
            _delSession(s->localSeid());
         return *this;
      }
      SessionBaseSPtr createSession(LocalNodeSPtr &ln, Seid rs, RemoteNodeSPtr &rn);
      /// @endcond

   private:
      State state_;
      SeidManager seidmgr_;
      SequenceManager seqmgr_;
      NodeSocket socket_;
      ReqOutUMap roumap_;
      RemoteNodeUMap rns_;
   };

   typedef std::unordered_map<EIpAddress,LocalNodeSPtr> LocalNodeUMap;
   typedef std::pair<EIpAddress,LocalNodeSPtr> LocalNodeUMapEIpAddressPair;
   typedef std::pair<ULong,LocalNodeSPtr> LocalNodeUMapULongPair;

   class LocalNodeStateChangeEvent : public EventBase
   {
   public:
      LocalNodeStateChangeEvent(LocalNodeSPtr &ln, LocalNode::State oldst, LocalNode::State newst)
         : ln_(ln),
           os_(oldst),
           ns_(newst)
      {
      }

      LocalNodeSPtr &localNode() { return ln_; }
      LocalNode::State oldState() const { return os_; }
      LocalNode::State newState() const { return ns_; }

   private:
      LocalNodeStateChangeEvent();
      LocalNodeSPtr ln_;
      LocalNode::State os_;
      LocalNode::State ns_;
   };
   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   /// @brief Contains the base functionality for all aplication messages.
   class AppMsg
   {
   public:
      /// @brief Class destructor.
      virtual ~AppMsg()
      {
      }

      /// @brief Returns the sequence number associated wtih this message.
      /// @return the sequence number associated wtih this message.
      ULong seqNbr() const                   { return seq_; }
      /// @brief Returns the message type for this message.
      /// @return the message type for this message.
      MsgType msgType() const                { return mt_; }
      /// @brief Returns the message class for this message (Node or Session).
      /// @return the message type for this message.
      MsgClass msgClass() const              { return mc_; }
      /// @brief Returns True if this message is a request message, otherwise False.
      Bool isReq() const                     { return rqst_; }

      /// @brief Assigns the sequence number for this message.
      /// @return a reference to this object.
      AppMsg &setSeqNbr(const ULong sn)      { seq_ = sn; return *this; }

      /// @brief Returns the class name for this object.
      /// @return the class name for this object.
      virtual const EString &className()
      {
         static EString cn_ = ::__CLASS_NAME__;
         return cn_;
      }

      /// @cond DOXYGEN_EXCLUDE
      virtual Void postDecode() {}
      /// @endcond

   protected:
      AppMsg()
         : seq_(0),
           mt_(0),
           mc_(MsgClass::Unknown),
           rqst_(False)
      {
      }
      AppMsg(const AppMsg &dm)
         : seq_(dm.seq_),
           mt_(dm.mt_),
           mc_(dm.mc_),
           rqst_(dm.rqst_)
      {
      }

      AppMsg &setMsgType(const MsgType mt)   { mt_ = mt; return*this; }
      AppMsg &setMsgClass(const MsgClass mc) { mc_ = mc; return*this; }
      AppMsg &setIsReq(const Bool rqst)      { rqst_ = rqst; return *this; }

   private:
      ULong seq_;
      MsgType mt_;
      MsgClass mc_;
      Bool rqst_;
   };

   typedef AppMsg *AppMsgPtr;

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   /// @brief Represents a request application message.
   class AppMsgReq : public AppMsg
   {
   public:
      /// @brief Class destructor.
      virtual ~AppMsgReq()
      {
      }

      /// @brief Returns a reference to the local node object for this message.
      /// @return a reference to the local node object for this message.
      LocalNodeSPtr &localNode()    { return ln_; }
      /// @brief Returns a reference to the remote node object for this message.
      /// @return a reference to the remote node object for this message.
      RemoteNodeSPtr &remoteNode()  { return rn_; }

   protected:
      /// @brief Default constructor.
      AppMsgReq()
      {
         setIsReq(True);
      }
      /// @brief Class constructor.
      /// @param ln the local node this message is associated with.
      /// @param rn the remote node this message is associated with.
      AppMsgReq(LocalNodeSPtr &ln, RemoteNodeSPtr &rn, Bool allocSeqNbr)
         : ln_(ln),
           rn_(rn)
      {
         setIsReq(True);
         if (allocSeqNbr)
            setSeqNbr(ln->allocSeqNbr());
      }

   private:
      LocalNodeSPtr ln_;
      RemoteNodeSPtr rn_;
   };
   typedef AppMsgReq *AppMsgReqPtr;

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   /// @brief Represents a request application node message
   ///   (not associated with a session).
   class AppMsgNodeReq : public AppMsgReq
   {
   public:
      /// @brief Default constructor.
      AppMsgNodeReq()
      {
         setMsgClass(PFCP::MsgClass::Node);
      }
      /// @brief Class constructor.
      /// @param ln the local node this message is associated with.
      /// @param rn the remote node this message is associated with.
      AppMsgNodeReq(LocalNodeSPtr &ln, RemoteNodeSPtr &rn, Bool allocSeqNbr)
         : AppMsgReq(ln, rn, allocSeqNbr)
      {
         setMsgClass(PFCP::MsgClass::Node);
      }
      /// @brief Class destructor.
      virtual ~AppMsgNodeReq()
      {
      }
   };

   typedef AppMsgNodeReq *AppMsgNodeReqPtr;

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   /// @brief Represents a request application session message.
   class AppMsgSessionReq : public AppMsgReq
   {
   public:
      /// @brief Default constructor.
      AppMsgSessionReq()
      {
         setMsgClass(PFCP::MsgClass::Session);
      }
      /// @brief Class constructor.
      /// @param ln the local node this message is associated with.
      /// @param rn the remote node this message is associated with.
      AppMsgSessionReq(SessionBaseSPtr &ses, Bool allocSeqNbr)
         : AppMsgReq(ses->localNode(), ses->remoteNode(), allocSeqNbr),
           ses_(ses)
      {
         setMsgClass(PFCP::MsgClass::Session);
      }
      /// @brief Class destructor.
      virtual ~AppMsgSessionReq()
      {
      }

      /// @brief Returs a reference to the session shaerd pointer.
      /// @return a reference to the session shaerd pointer.
      SessionBaseSPtr &session()    { return ses_; }

   private:
      SessionBaseSPtr ses_;
   };

   typedef AppMsgSessionReq *AppMsgSessionReqPtr;

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   /// @brief Represents a response application message.
   class AppMsgRsp : public AppMsg
   {
   public:
      /// @brief Default construtor.
      AppMsgRsp()
         : amrq_(nullptr)
      {
         setIsReq(False);
      }
      /// @brief Class constructor.
      /// @param amrq a shared pointer reference to the request this response is
      ///   associated with.
      AppMsgRsp(AppMsgReqPtr amrq)
         : amrq_(amrq)
      {
         setIsReq(False);
      }
      /// @brief Class destructor.
      virtual ~AppMsgRsp()
      {
         if (amrq_)
            delete amrq_;
      }

      /// @brief Returns a shared pointer to the request application message.
      /// @return a shared pointer to the request application message.
      AppMsgReqPtr req()            { return amrq_; }
      /// @brief Returns a reference to the local node object for this message.
      /// @return a reference to the local node object for this message.
      LocalNodeSPtr &localNode()    { return amrq_->localNode(); }
      /// @brief Returns a reference to the remote node object for this message.
      /// @return a reference to the remote node object for this message.
      RemoteNodeSPtr &remoteNode()  { return amrq_->remoteNode(); }

      /// @brief Sets the request message that this response is associated with.
      /// @return a reference to this object.
      AppMsgRsp &setReq(AppMsgReq *req)
      {
         amrq_ = req;
         if (amrq_)
            setSeqNbr(amrq_->seqNbr());
         return *this;
      }

   private:
      AppMsgReqPtr amrq_;
   };
   typedef AppMsgRsp *AppMsgRspPtr;

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   /// @brief Represents a response application message.
   class AppMsgNodeRsp : public AppMsgRsp
   {
   public:
      /// @brief Default construtor.
      AppMsgNodeRsp()
         : AppMsgRsp(nullptr)
      {
         setMsgClass(PFCP::MsgClass::Node);
         setIsReq(False);
      }
      /// @brief Class constructor.
      /// @param amrq a shared pointer reference to the request this response is
      ///   associated with.
      AppMsgNodeRsp(AppMsgNodeReqPtr &amrq)
         : AppMsgRsp(amrq)
      {
         setMsgClass(PFCP::MsgClass::Node);
         setIsReq(False);
      }
      /// @brief Class destructor.
      virtual ~AppMsgNodeRsp()
      {
      }

      /// @brief Returns a shared pointer to the request application message.
      /// @return a shared pointer to the request application message.
      AppMsgNodeReqPtr req()            { return static_cast<AppMsgNodeReqPtr>(AppMsgRsp::req()); }

      /// @brief Sets the request message that this response is associated with.
      /// @return a reference to this object.
      AppMsgNodeRsp &setReq(AppMsgNodeReqPtr req) { AppMsgRsp::setReq(req); return *this; }
   };

   typedef AppMsgNodeRsp *AppMsgNodeRspPtr;

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   /// @brief Represents a response application message.
   class AppMsgSessionRsp : public AppMsgRsp
   {
   public:
      /// @brief Default construtor.
      AppMsgSessionRsp()
         : AppMsgRsp(nullptr)
      {
         setMsgClass(PFCP::MsgClass::Session);
      }
      /// @brief Class constructor.
      /// @param amrq a shared pointer reference to the request this response is
      ///   associated with.
      AppMsgSessionRsp(AppMsgSessionReqPtr &amrq)
         : AppMsgRsp(amrq)
      {
         setMsgClass(PFCP::MsgClass::Session);
      }
      /// @brief Class destructor.
      virtual ~AppMsgSessionRsp()
      {
      }

      /// @brief Returns a shared pointer to the request application message.
      /// @return a shared pointer to the request application message.
      AppMsgSessionReqPtr req()     { return static_cast<AppMsgSessionReqPtr>(AppMsgRsp::req()); }

      /// @brief Sets the request message that this response is associated with.
      /// @return a reference to this object.
      AppMsgSessionRsp &setReq(AppMsgSessionReqPtr req) { AppMsgRsp::setReq(req); return *this; }
   };

   typedef AppMsgSessionRsp *AppMsgSessionRspPtr;

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   /// @cond DOXYGEN_EXCLUDE
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
   /// @endcond

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   /// @cond DOXYGEN_EXCLUDE
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

      AppMsgNodeReqPtr req() const                             { return am_; }
      const ETime &startTime() const                           { return started_; }

      RcvdHeartbeatReqData &setReq(AppMsgNodeReqPtr am)        { am_ = am; return *this;}
      RcvdHeartbeatReqData &setStartTime(const ETime &started) { started_ = started; return *this; }

   private:
      AppMsgNodeReqPtr am_;
      ETime started_;
   };
   typedef RcvdHeartbeatReqData *RcvdHeartbeatReqDataPtr;
   /// @endcond

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   /// @cond DOXYGEN_EXCLUDE
   class SndHeartbeatRspData
   {
   public:
      SndHeartbeatRspData()
         : am_(nullptr)
      {
      }
      SndHeartbeatRspData(AppMsgNodeReqPtr am)
         : am_(am)
      {
      }

      AppMsgNodeReq &req()                               { return *am_; }

      SndHeartbeatRspData &setReq(AppMsgNodeReqPtr am)   { am_ = am; return *this; }

   private:
      AppMsgNodeReqPtr am_;
   };
   typedef SndHeartbeatRspData *SndHeartbeatRspDataPtr;
   /// @endcond

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   /// @cond DOXYGEN_EXCLUDE
   class RcvdHeartbeatRspData
   {
   public:
      RcvdHeartbeatRspData()
         : am_(nullptr)
      {
      }
      RcvdHeartbeatRspData(AppMsgNodeReqPtr am)
         : am_(am)
      {
      }
      ~RcvdHeartbeatRspData()
      {
         if (am_)
         {
            delete am_;
            am_ = nullptr;
         }
      }

      AppMsgNodeReq &req()                                     { return *am_; }
      const ETime &startTime() const                           { return started_; }

      RcvdHeartbeatRspData &setReq(AppMsgNodeReqPtr am)        { am_ = am; return *this; }
      RcvdHeartbeatRspData &setStartTime(const ETime &started) { started_ = started; return *this; }

   private:
      AppMsgNodeReqPtr am_;
      ETime started_;
   };
   typedef RcvdHeartbeatRspData *RcvdHeartbeatRspDataPtr;
   /// @endcond

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   /// @cond DOXYGEN_EXCLUDE
   DECLARE_ERROR(InternalMsg_OutOfMemory);

   class TranslatorMsgInfo;

   class InternalMsg
   {
   public:
      InternalMsg()
         : seq_(0),
           mt_(0),
           mc_(MsgClass::Unknown),
           rqst_(False),
           data_(nullptr),
           len_(0)
      {
      }
      InternalMsg(const InternalMsg &im)
         : ln_(im.ln_),
           rn_(im.rn_),
           ses_(im.ses_),
           seq_(im.seq_),
           mt_(im.mt_),
           mc_(im.mc_),
           rqst_(im.rqst_),
           data_(nullptr),
           len_(0)
      {
         assign(im.data_, im.len_);
      }
      InternalMsg(const LocalNodeSPtr &ln, const RemoteNodeSPtr &rn, const TranslatorMsgInfo &tmi, cpUChar data, UShort len);
      virtual ~InternalMsg()
      {
         if (data_ != nullptr)
            delete [] data_;
      }

      InternalMsg &operator=(const InternalMsg &im)
      {
         ln_ = im.ln_;
         rn_ = im.rn_;
         ses_ = im.ses_;
         seq_ = im.seq_;
         mt_ = im.mt_;
         mc_ = im.mc_;
         rqst_ = im.rqst_;
         assign(im.data_, im.len_);
         return *this;
      }

      LocalNodeSPtr &localNode()          { return ln_; }
      RemoteNodeSPtr &remoteNode()        { return rn_; }
      SessionBaseSPtr &session()          { return ses_; }
      ULong seqNbr() const                { return seq_; }
      MsgType msgType() const             { return mt_; }
      MsgClass msgClass() const           { return mc_; }
      Bool isReq() const                  { return rqst_; }
      UChar version() const               { return ver_; }
      cpUChar data() const                { return data_; }
      UShort len() const                  { return len_; }

      InternalMsg &setLocalNode(const LocalNodeSPtr &ln)    { ln_ = ln; return *this; }
      InternalMsg &setRemoteNode(const RemoteNodeSPtr &rn)  { rn_ = rn; return *this; }
      InternalMsg &setSession(const SessionBaseSPtr &ses)   { ses_ = ses; return *this; }
      InternalMsg &setSeqNbr(const ULong sn)                { seq_ = sn; return *this; }
      InternalMsg &setMsgType(const MsgType mt)             { mt_ = mt; return *this; }
      InternalMsg &setMsgClass(const MsgClass mc)           { mc_ = mc; return *this; }
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

      static void* operator new(size_t sz);
      static void operator delete(void* m);

   private:
      static EMemory::Pool pool_;
      LocalNodeSPtr ln_;
      RemoteNodeSPtr rn_;
      SessionBaseSPtr ses_;
      ULong seq_;
      MsgType mt_;
      MsgClass mc_;
      Bool rqst_;
      UChar ver_;
      pUChar data_;
      UShort len_;
   };

   typedef InternalMsg *InternalMsgPtr;
   /// @endcond

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   /// @cond DOXYGEN_EXCLUDE
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

      AppMsgRspPtr rsp() const         { return am_; }
      RspOut &setRsp(AppMsgRspPtr am)  { am_ = am; return *this; }

   private:
      AppMsgRspPtr am_;
   };
   typedef RspOut *RspOutPtr;
   /// @endcond

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   /// @cond DOXYGEN_EXCLUDE
   class RspIn : public InternalMsg
   {
   public:
      RspIn()
         : am_(nullptr),
           rs_(0)
      {
      }
      RspIn(const RspIn &ri)
         : InternalMsg(ri),
           am_(ri.am_),
           rs_(ri.rs_)
      {
      }
      RspIn(const LocalNodeSPtr &ln, const RemoteNodeSPtr &rn, const TranslatorMsgInfo &tmi, cpUChar data, UShort len, AppMsgReqPtr am)
         : InternalMsg(ln, rn, tmi, data, len),
           am_(am)
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

      AppMsgReqPtr req() const                  { return am_; }
      RspIn &setReq(AppMsgReqPtr am)            { am_ = am; return *this; }

      Seid remoteSeid() const                   { return rs_; }
      RspIn &remoteSeid(Seid rs)                { rs_ = rs; return *this; }

      const ETime &remoteStartTime()            { return rst_; }
      RspIn &remoteStartTime(const ETime &rst)  { rst_ = rst; return *this; }
   private:
      AppMsgReqPtr am_;
      Seid rs_;
      ETime rst_;
   };
   typedef RspIn *RspInPtr;
   /// @endcond

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   /// @cond DOXYGEN_EXCLUDE
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
         if (am_)
         {
            delete am_;
            am_ = nullptr;
         }
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
      ReqOut &setAppMsg(AppMsgReqPtr am)
      {
         am_ = am;
         if (am)
         {
            setMsgType(am->msgType());
            setMsgClass(am->msgClass());
         }
         return *this;
      }

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
   /// @endcond

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   /// @cond DOXYGEN_EXCLUDE
   class ReqIn : public InternalMsg
   {
   public:
      ReqIn()
         : rs_(0)
      {
      }
      ReqIn(const ReqIn &ri)
         : InternalMsg(ri),
           rs_(ri.rs_)
      {
      }
      ReqIn(const LocalNodeSPtr &ln, const RemoteNodeSPtr &rn, const TranslatorMsgInfo &tmi, cpUChar data, UShort len)
         : InternalMsg(ln, rn, tmi, data, len)
      {
      }
      virtual ~ReqIn()
      {
      }

      Seid remoteSeid()                         { return rs_; }
      ReqIn &remoteSeid(Seid rs)                { rs_ = rs; return *this; }

      const ETime &remoteStartTime()            { return rst_; }
      ReqIn &remoteStartTime(const ETime &rst)  { rst_ = rst; return *this; }
   private:
      Seid rs_;
      ETime rst_;
   };
   typedef ReqIn *ReqInPtr;
   /// @endcond

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

   /// @cond DOXYGEN_EXCLUDE
   DECLARE_ERROR_ADVANCED(TeidRangeManager_InvalidRangeBits);
   inline TeidRangeManager_InvalidRangeBits::TeidRangeManager_InvalidRangeBits() {
      setText("The number of range bits must be between 0 and 7");
   }

   class TeidRangeManager
   {
   public:
      TeidRangeManager(Int rangeBits=0);
      ~TeidRangeManager();

      Bool assign(RemoteNodeSPtr &n);
      Void release(RemoteNodeSPtr &n);

   private:
      Int bits_;
      std::list<Int> free_;
      std::unordered_map<Int,RemoteNodeSPtr> used_;
   };
   /// @endcond

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   /// @cond DOXYGEN_EXCLUDE
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

      Seid seid() const                                  { return seid_; }
      MsgType msgType() const                            { return mt_; }
      MsgClass msgClass() const                          { return mc_; }
      ULong seqNbr() const                               { return sn_; }
      UChar version() const                              { return ver_; }
      Bool isReq() const                                 { return rqst_; }
      Bool createSession() const                         { return create_; }

      TranslatorMsgInfo &setSeid(const Seid seid)        { seid_ = seid; return *this; }
      TranslatorMsgInfo &setMsgType(const MsgType mt)    { mt_ = mt; return *this; }
      TranslatorMsgInfo &setMsgClass(const MsgClass mc)  { mc_ = mc; return *this; }
      TranslatorMsgInfo &setSeqNbr(const ULong sn)       { sn_ = sn; return *this; }
      TranslatorMsgInfo &setVersion(UChar ver)           { ver_ = ver; return *this; }
      TranslatorMsgInfo &setReq(Bool rqst)               { rqst_ = rqst; return *this; }
      TranslatorMsgInfo &setCreateSession(Bool create)   { create_ = create; return *this; }

   private:
      Seid seid_;
      MsgType mt_;
      MsgClass mc_;
      ULong sn_;
      UChar ver_;
      Bool rqst_;
      Bool create_;
   };
   /// @endcond

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   /// @brief The PFCP Translator is used to encode and decode PFCP messages.
   ///   This is a pure virtual base class that needs to be overridden to support
   ///   each 3GPP PFCP release.
   class Translator
   {
   public:
      /// @brief Default constructor.
      Translator();
      /// @brief Class destructor.
      virtual ~Translator();

      /// @brief Encodes a heartbeat request message.
      /// @param hb the data associated with this heartbeat request message.
      /// @return a pointer to the encoded heartbeat request message.
      virtual ReqOutPtr encodeHeartbeatReq(SndHeartbeatReqData &hb) = 0;
      /// @brief Encodes a heartbeat response message.
      /// @param hb the data associated with this heartbeat response message.
      /// @return a pointer to the encoded heartbeat response message.
      virtual RspOutPtr encodeHeartbeatRsp(SndHeartbeatRspData &hb) = 0;
      /// @brief Encodes a version not supported message.
      /// @param msg a pointer to the request messge that is not supported.
      /// @return a pointer to the encoded version not supported message.
      virtual RspOutPtr encodeVersionNotSupportedRsp(ReqInPtr msg) = 0;

      /// @brief Encodes the PFCP request message specified by the application
      ///   message request.
      /// @param msg a pointer to the application request message.
      /// @return a pointer to the encoded request.
      virtual ReqOutPtr encodeReq(AppMsgReqPtr msg) = 0;
      /// @brief Encodes the PFCP response message specified by the application
      ///   message response.
      /// @param msg a pointer to the application response message.
      /// @return a pointer to the encoded response.
      virtual RspOutPtr encodeRsp(AppMsgRspPtr msg) = 0;

      /// @brief Decodes the PFCP request message.
      /// @param msg a pointer to the application message request.
      /// @return a pointer to the decoded application message request.
      virtual AppMsgReqPtr decodeReq(ReqInPtr msg) = 0;
      /// @brief Decodes the PFCP response message.
      /// @param msg a pointer to the application message response.
      /// @return a pointer to the decoded application message response.
      virtual AppMsgRspPtr decodeRsp(RspInPtr msg) = 0;
      
      /// @brief Decodes the PFCP heartbeat request message.
      /// @param msg a pointer to the raw request message.
      /// @return a pointer to the decoded heartbeat request message.
      virtual RcvdHeartbeatReqDataPtr decodeHeartbeatReq(ReqInPtr msg) = 0;
      /// @brief Decodes the PFCP heartbeat response message.
      /// @param msg a pointer to the raw response message.
      /// @return a pointer to the decoded heartbeat response message.
      virtual RcvdHeartbeatRspDataPtr decodeHeartbeatRsp(RspInPtr msg) = 0;

      /// @brief Returns basic information about a message.
      /// @param info a reference to message info object to populate.
      /// @param msg a pointer to the raw message buffer.
      /// @param len the length of the raw message buffer.
      virtual Void getMsgInfo(TranslatorMsgInfo &info, cpUChar msg, Int len) = 0;
      /// @brief Returns True if the specified version is supported by the
      ///   Translator.
      virtual Bool isVersionSupported(UChar ver) = 0;
      /// @brief Returns the message class associated with the provided message type.
      /// @return the message class associated with the provided message type.
      virtual MsgClass messageClass(MsgType mt) = 0;

      /// @brief Returns the message type value for a heartbeat request message.
      /// @return the message type value for a heartbeat request message.
      virtual MsgType pfcpHeartbeatReq() = 0;
      /// @brief Returns the message type value for a heartbeat response message.
      /// @return the message type value for a heartbeat response message.
      virtual MsgType pfcpHeartbeatRsp() = 0;
      /// @brief Returns the message type value for a session establishment request message.
      /// @return the message type value for a session establishment request message.
      virtual MsgType pfcpSessionEstablishmentReq() = 0;
      /// @brief Returns the message type value for a session establishment response message.
      /// @return the message type value for a session establishment response message.
      virtual MsgType pfcpSessionEstablishmentRsp() = 0;
      /// @brief Returns the message type value for a association setup request message.
      /// @return the message type value for a association setup request message.
      virtual MsgType pfcpAssociationSetupReq() = 0;
      /// @brief Returns the message type value for a association setup response message.
      /// @return the message type value for a association setup response message.
      virtual MsgType pfcpAssociationSetupRsp() = 0;

   protected:
      /// @cond DOXYGEN_EXCLUDE
      pUChar data() { return data_; }
      /// @endcond

   private:
      UChar data_[ESocket::UPD_MAX_MSG_LENGTH];
   };

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   /// @cond DOXYGEN_EXCLUDE
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
   /// @endcond

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   /// @cond DOXYGEN_EXCLUDE
   #define APPLICATION_BASE_EVENT (EM_USER + 10000)
   /// @endcond

   /// @brief The events that will be received by the application work group.
   enum class ApplicationEvents : UInt
   {
      /// RcvdReq - TranslationThread --> ApplicationWorkGroup - AppMsgReqPtr
      RcvdReq                 = (APPLICATION_BASE_EVENT + 1),
      /// RcvdRsp - TranslationThread --> ApplicationWorkGroup - AppMsgRspPtr
      RcvdRsp                 = (APPLICATION_BASE_EVENT + 2),
      /// ReqTimeout - CommunicationThread --> ApplicationWorkGroup - AppMsgReqPtr
      ReqTimeout              = (APPLICATION_BASE_EVENT + 3),
      /// LocalNodeStateChange - CommunicationThread --> ApplicationWorkGroup - *LocalNodeStateChangeEvent
      LocalNodeStateChange    = (APPLICATION_BASE_EVENT + 4),
      /// RemoteNodeStateChange - CommunicationThread --> ApplicationWorkGroup - *RemoteNodeStateChangeEvent
      RemoteNodeStateChange   = (APPLICATION_BASE_EVENT + 5),
      /// RemoteNodeRestart - CommunicationThread --> ApplicationWorkGroup - *RemoteNodeRestartEvent
      RemoteNodeRestart       = (APPLICATION_BASE_EVENT + 6),
      /// SndReqError - CommunicationThread --> ApplicationWorkGroup - SndReqExceptionDataPtr
      SndReqError             = (APPLICATION_BASE_EVENT + 9),
      /// SndRspError - CommunicationThread --> ApplicationWorkGroup - SndRspExceptionDataPtr
      SndRspError             = (APPLICATION_BASE_EVENT + 8),
      /// EncodeReqError - TranslationThread --> ApplicationWorkGroup - EncodeReqExceptionDataPtr
      EncodeReqError          = (APPLICATION_BASE_EVENT + 9),
      /// EncodeRspError - TranslationThread --> ApplicationWorkGroup - EncodeRspExceptionDataPtr
      EncodeRspError          = (APPLICATION_BASE_EVENT + 10),
      /// DecodeReqError - TranslationThread --> ApplicationWorkGroup - DecodeReqExceptionDataPtr
      DecodeReqError          = (APPLICATION_BASE_EVENT + 11),
      /// DecodeRspError - TranslationThread --> ApplicationWorkGroup - EncodeRspExceptionDataPtr
      DecodeRspError          = (APPLICATION_BASE_EVENT + 12)
   };

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   /// @cond DOXYGEN_EXCLUDE
   DECLARE_ERROR(ApplicationWorkGroup_UnrecognizedAddressFamily);
   /// @endcond

   class ApplicationWorkGroupBase
   {
      friend LocalNode;
      friend CommunicationThread;
   protected:
      /// @brief Creates a local node.
      /// @return a shared pointer to the local node.
      virtual LocalNodeSPtr _createLocalNode() = 0;

      /// @brief Creates a remote node.
      /// @return a shared pointer to the local node.
      virtual RemoteNodeSPtr _createRemoteNode() = 0;

      /// @brief Creates a session object.
      /// @return a shared pointer to the session object
      virtual SessionBaseSPtr _createSession(LocalNodeSPtr &ln, RemoteNodeSPtr &rn) = 0;
   };
   
   /// @brief The PFCP application work group template.  This template contains
   ///   the common event queue for the application worker threads.
   template <class TWorker>
   class ApplicationWorkGroup : public EThreadWorkGroupPrivate<TWorker>, public ApplicationWorkGroupBase
   {
      friend CommunicationThread;
   public:
      /// @brief Creates a local node.
      /// @param ipaddr the IP address of the local node.
      /// @param port the port of the local node.
      /// @return a shared pointer to the local node.
      /// @{
      LocalNodeSPtr createLocalNode(cpStr ipaddr, UShort port = PFCP::Configuration::port(), Bool start = True)
      {
         ESocket::Address addr(ipaddr, port);
         return createLocalNode(addr, start);
      }
      LocalNodeSPtr createLocalNode(const EIpAddress &ipaddr, UShort port = PFCP::Configuration::port(), Bool start = True)
      {
         ESocket::Address addr;
         switch (ipaddr.family())
         {
            case AF_INET:  { addr.setAddress(ipaddr.ipv4Address(), port); break; }
            case AF_INET6: { addr.setAddress(ipaddr.ipv6Address(), port); break; }
            default:
            {
               throw ApplicationWorkGroup_UnrecognizedAddressFamily();
            }
         }
         return createLocalNode(addr, start);
      }
      /// @}

      /// @brief Creates a local node.
      /// @param addr the IP address/port of the local node.
      /// @return a shared pointer to hte local node.
      LocalNodeSPtr createLocalNode(ESocket::Address &addr, Bool start = True);

      /// @brief Startes an externally constructed local node.
      /// @param ln a shared pointer to the LocalNode.
      Void startLocalNode(LocalNodeSPtr &ln);
      /// @brief Stops the local node.
      /// @param ln a shared pointer to the LocalNode.
      Void stopLocalNode(LocalNodeSPtr &ln);
   };

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   /// @brief The application worker thread class.  The various virtual methods
   ///   should be overridden to implement application specific behavior.  Any
   ///   additional event handlers can be added to handle application specific
   ///   (non PFCP) events.
   class ApplicationWorker : public EThreadWorkerPrivate
   {
      friend Void Uninitialize();
   public:
      /// @brief Called when the worker thread is initialized.  When overriding,
      ///   the overriding method should call the associated base class method.
      virtual Void onInit();
      /// @brief Called when the worker thread is has received the EM_QUIT event.
      ///   When overriding, the overriding method should call the associated
      ///   base class method.
      virtual Void onQuit();

      /// @brief Called when a PFCP request message has been received.
      /// @param req a pointer to the request message.
      virtual Void onRcvdReq(AppMsgReqPtr req);
      /// @brief Called when a PFCP response message has been received.
      /// @param rsp a pointer to the response message.
      virtual Void onRcvdRsp(AppMsgRspPtr rsp);
      /// @brief Called when a PFCP request message has timed out waiting
      ///   for the associated response message.
      /// @param req a pointer to the request message that did not receive
      ///   a response.
      virtual Void onReqTimeout(AppMsgReqPtr req);
      /// @brief Called when the local node state changes.
      /// @param ln a shared pointer to the local node object.
      /// @param oldState the previous state of the local node.
      /// @param newState the new state of the local node.
      virtual Void onLocalNodeStateChange(LocalNodeSPtr &ln, LocalNode::State oldState, LocalNode::State newState);
      /// @brief Called when a new remote node/peer has been added.
      /// @param rn a shared pointer to the remote node object.
      /// @param oldState the previous state of the remote node.
      /// @param newState the new state of the remote node.
      virtual Void onRemoteNodeStateChange(RemoteNodeSPtr &rn, RemoteNode::State oldState, RemoteNode::State newState);
      /// @brief Called when a new remote node/peer has been added.
      /// @param rmtNode a shared pointer to the remote node object.
      virtual Void onRemoteNodeRestart(RemoteNodeSPtr &rn, const ETime &restartTime);
      /// @brief Called when a remote node/peer object is removed.
      /// @param rn a shared pointer to the remote node object.
      /// @param restartTime the time the remote node restarted.
      virtual Void onSndReqError(AppMsgReqPtr req, SndReqException &err);
      /// @brief Called when an error is encountered while sending a response
      ///   message.
      /// @param rsp the appication response message that failed.
      /// @param err a reference to the associated exception.
      virtual Void onSndRspError(AppMsgRspPtr rsp, SndRspException &err);
      /// @brief Called when an error is encountered while encoding a request
      ///   message.
      /// @param req the appication request message that failed.
      /// @param err a reference to the associated exception.
      virtual Void onEncodeReqError(AppMsgReqPtr req, EncodeReqException &err);
      /// @brief Called when an error is encountered while encoding a response
      ///   message.
      /// @param rsp the appication response message that failed.
      /// @param err a reference to the associated exception.
      virtual Void onEncodeRspError(AppMsgRspPtr rsp, EncodeRspException &err);

   protected:
      /// @cond DOXYGEN_EXCLUDE
      ApplicationWorker();
      ~ApplicationWorker();

      Void _onRcvdReq(EThreadMessage &msg);
      Void _onRcvdRsp(EThreadMessage &msg);
      Void _onReqTimeout(EThreadMessage &msg);
      Void _onLocalNodeStateChange(EThreadMessage &msg);
      Void _onRemoteNodeStateChange(EThreadMessage &msg);
      Void _onRemoteNodeRestart(EThreadMessage &msg);
      Void _onSndReqError(EThreadMessage &msg);
      Void _onSndRspError(EThreadMessage &msg);
      Void _onEncodeReqError(EThreadMessage &msg);
      Void _onEncodeRspError(EThreadMessage &msg);

      BEGIN_MESSAGE_MAP2(ApplicationWorker, EThreadWorkerPrivate)
         ON_MESSAGE2(static_cast<UInt>(ApplicationEvents::RcvdReq), ApplicationWorker::_onRcvdReq)
         ON_MESSAGE2(static_cast<UInt>(ApplicationEvents::RcvdRsp), ApplicationWorker::_onRcvdRsp)
         ON_MESSAGE2(static_cast<UInt>(ApplicationEvents::ReqTimeout), ApplicationWorker::_onReqTimeout)
         ON_MESSAGE2(static_cast<UInt>(ApplicationEvents::LocalNodeStateChange), ApplicationWorker::_onLocalNodeStateChange)
         ON_MESSAGE2(static_cast<UInt>(ApplicationEvents::RemoteNodeStateChange), ApplicationWorker::_onRemoteNodeStateChange)
         ON_MESSAGE2(static_cast<UInt>(ApplicationEvents::RemoteNodeRestart), ApplicationWorker::_onRemoteNodeRestart)
         ON_MESSAGE2(static_cast<UInt>(ApplicationEvents::SndReqError), ApplicationWorker::_onSndReqError)
         ON_MESSAGE2(static_cast<UInt>(ApplicationEvents::SndRspError), ApplicationWorker::_onSndRspError)
         ON_MESSAGE2(static_cast<UInt>(ApplicationEvents::EncodeReqError), ApplicationWorker::_onEncodeReqError)
         ON_MESSAGE2(static_cast<UInt>(ApplicationEvents::EncodeRspError), ApplicationWorker::_onEncodeRspError)
      END_MESSAGE_MAP2()
      /// @endcond
   private:
   };

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   /// @cond DOXYGEN_EXCLUDE
   #define TRANSLATION_BASE_EVENT (EM_USER + 20000)

   class TranslationThread : public EThreadPrivate
   {
      friend Void Uninitialize();
      friend AppMsgReq;
      friend AppMsgRsp;
   public:
      enum class Events : UInt
      {
         SndMsg            = (TRANSLATION_BASE_EVENT + 1),  // ApplicationWorkGroup --> TranslationThread - AppMsgPtr
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
   /// @endcond

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   /// @cond DOXYGEN_EXCLUDE
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
         RcvdRspError         = (COMMUNICATION_BASE_EVENT + 8),   // TranslationThread --> CommunicationThread - RcvdRspExceptionDataPtr - (Translator failure to parse response)
         ReqTimeout           = (COMMUNICATION_BASE_EVENT + 9),   // ETimerPool --> CommunicationThread - ReqOutPtr
         AddSession           = (COMMUNICATION_BASE_EVENT + 10),  // ApplicationThread --> CommunicationThread - *SessionBaseSPtr
         DelSession           = (COMMUNICATION_BASE_EVENT + 11),  // ApplicationThread --> CommunicationThread - *SessionBaseSPtr
         DelNxtRmtSession     = (COMMUNICATION_BASE_EVENT + 12)   // ApplicationThread/CommunicationThread --> CommunicationThread - *RemoteNodeSPtr
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

      Void startLocalNode(LocalNodeSPtr &ln);
      Void stopLocalNode(LocalNodeSPtr &ln);
      LocalNodeSPtr createLocalNode(ESocket::Address &addr, Bool start = True);
      LocalNodeSPtr registerLocalNode(ESocket::Address &addr);

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
      Void onAddSession(EThreadMessage &msg);
      Void onDelSession(EThreadMessage &msg);
      Void onDelNxtRmtSession(EThreadMessage &msg);

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
      Void addSession(SessionBaseSPtr &s);
      Void delSession(SessionBaseSPtr &s);

      ESocket::Address address_;
      TeidRangeManager trm_;
      ERWLock lnslck_;
      LocalNodeUMap lns_;
      EThreadEventTimer atmr_;
      EThreadEventTimer rsptmr_;
      size_t caw_;
      Int crw_;
   };
   /// @endcond

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   /// @cond DOXYGEN_EXCLUDE
   inline SessionBase::~SessionBase()
   {
      static EString __method__ = __METHOD_NAME__;
      // Configuration::logger().debug(
      //    "{} - destroying session localNode={} remoteNode={} localSeid={} remoteSeid={}",
      //    __method__, localNode()->address().getAddress(),
      //    remoteNode()->address().getAddress(), localSeid(), remoteSeid());
      ++deleted_;
   }

   inline SessionBase &SessionBase::setSeid(SessionBaseSPtr &s, Seid ls, Seid rs, Bool notify)
   {
      if (ls != 0)
      {
         if (ls_ != 0)
            throw SessionBase_LocalSeidAlreadySet();
         ls_ = ls;
      }
      if (rs != 0)
      {
         if (rs_ != 0)
            throw SessionBase_RemoteSeidAlreadySet();
         rs_ = rs;
      }
      if (notify)
      {
         auto s2 = new SessionBaseSPtr(s);
         SEND_TO_COMMUNICATION(AddSession, s2);
      }
      return *this;
   }
   
   inline Void SessionBase::destroy(SessionBaseSPtr &s)
   {
      auto s2 = new SessionBaseSPtr(s);
      SEND_TO_COMMUNICATION(DelSession, s2);
   }

   inline InternalMsg::InternalMsg(const LocalNodeSPtr &ln, const RemoteNodeSPtr &rn, const TranslatorMsgInfo &tmi, cpUChar data, UShort len)
      : ln_(ln),
        rn_(rn),
        seq_(tmi.seqNbr()),
        mt_(tmi.msgType()),
        mc_(tmi.msgClass()),
        rqst_(tmi.isReq()),
        ver_(tmi.version()),
        data_(nullptr),
        len_(0)
   {
      assign(data, len);
   }

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
      return setRcvdReqRspWnd(sn, PFCP::CommunicationThread::Instance().currentRspWnd());
   }

   template<class TWorker>
   inline LocalNodeSPtr ApplicationWorkGroup<TWorker>::createLocalNode(ESocket::Address &addr, Bool start)
   {
      return CommunicationThread::Instance().createLocalNode(addr, start);
   }

   template<class TWorker>
   inline Void ApplicationWorkGroup<TWorker>::startLocalNode(LocalNodeSPtr &ln)
   {
      CommunicationThread::Instance().startLocalNode(ln);
   }

   template<class TWorker>
   inline Void ApplicationWorkGroup<TWorker>::stopLocalNode(LocalNodeSPtr &ln)
   {
      CommunicationThread::Instance().stopLocalNode(ln);
   }

   inline void* InternalMsg::operator new(size_t sz)
   {
      if (pool_.allocSize() == 0)
      {
         size_t as = 0;
         if (sizeof(RspOut) > as)   as = sizeof(RspOut);
         if (sizeof(RspIn) > as)   as = sizeof(RspIn);
         if (sizeof(ReqOut) > as)   as = sizeof(ReqOut);
         if (sizeof(ReqIn) > as)   as = sizeof(ReqIn);

         size_t ns = 32768 - sizeof(EMemory::Node);
         size_t bs = as + sizeof(EMemory::Block);
         bs += bs % sizeof(pVoid);
         size_t bc = ns / bs;
         ns = sizeof(EMemory::Node) + bc * bs;
         pool_.setSize(as, ns);
      }
      if (sz > pool_.allocSize())
      {
         EError ex;
         ex.setSevere();
         ex.setText("internal message allocation size is larger than memory pool block size");
         throw ex;
      }
      return pool_.allocate();
   }

   inline void InternalMsg::operator delete(void* m)
   {
      pool_.deallocate(m);
   }
   /// @endcond
}

/// @cond DOXYGEN_EXCLUDE
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
/// @endcond

}

#endif // #ifndef __EPFCP_H