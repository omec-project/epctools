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

#include "epfcp.h"

namespace PFCP
{
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#ifdef PFCP_ANALYSIS
Analysis *analysis = nullptr;
#endif

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

MessageStats::MessageStats(MessageId id, cpStr name)
   : id_( id ),
     name_( name )
{
   reset();
}

MessageStats::MessageStats(MessageId id, const EString &name)
   : id_( id ),
     name_( name )
{
   reset();
}

MessageStats::MessageStats(const MessageStats &m)
   : id_( m.id_ ),
     name_( m.name_ ),
     received_( m.received_.load() ),
     timeout_( m.timeout_.load() )
{
   sent_ = m.sent_;
}

Void MessageStats::reset()
{
   for (auto &sent : sent_)
      sent = 0;

   received_ = 0;
   timeout_ = 0;
}

UInt MessageStats::incSent(UInt attempt)
{
   if (attempt > MAX_ATTEMPS)
      attempt = MAX_ATTEMPS;

   return ++sent_[attempt];
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Void Stats::collectNodeStats(EJsonBuilder &builder)
{
   static EString __method__ = __METHOD_NAME__;

   try
   {
      std::vector<LocalNodeSPtr> localNodes;
      {
         ERDLock lck(CommunicationThread::Instance().localNodesLock());
         localNodes.reserve(CommunicationThread::Instance().localNodes().size());
         for (auto &iln : CommunicationThread::Instance().localNodes())
            localNodes.emplace_back(iln.second);
      }

      std::sort(localNodes.begin(), localNodes.end(),
         [](const LocalNodeSPtr &a, const LocalNodeSPtr &b) -> bool
         {
            return a->ipAddress().address().compare(b->ipAddress().address()) == 0;
         }
      );

      EJsonBuilder::StackArray pushLocalNodes(builder, "local_nodes");

      for (auto &localNodeSPtr : localNodes)
      {
         auto &localNode = *localNodeSPtr.get();

         EJsonBuilder::StackObject pushLocalNode(builder);

         localNode.collectStats(builder);

         std::vector<RemoteNodeSPtr> remoteNodes;
         {
            ERDLock lck(localNode.remoteNodesLock());
            remoteNodes.reserve(localNode.remoteNodes().size());
            for (auto &irn : localNode.remoteNodes())
               remoteNodes.emplace_back(irn.second);
         }

         std::sort(remoteNodes.begin(), remoteNodes.end(),
            [](const RemoteNodeSPtr &a, const RemoteNodeSPtr &b) -> bool
            {
               return a->ipAddress().address().compare(b->ipAddress().address()) == 0;
            }
         );

         EJsonBuilder::StackArray pushRemoteNodes(builder, "remote_nodes");

         for (auto &remoteNodeSPtr : remoteNodes)
         {
            auto &remoteNode = *remoteNodeSPtr.get();

            EJsonBuilder::StackObject pushRemoteNode(builder);

            remoteNode.collectStats(builder);

            std::vector<UInt> messages;
            {
               ERDLock lck(remoteNode.stats().getLock());
               for (auto imsg : remoteNode.stats().messageStats())
                  messages.push_back(imsg.first);
            }

            std::sort(messages.begin(), messages.end());

            EJsonBuilder::StackObject pushMessages(builder, "messages");
            for (auto msg : messages)
            {
               MessageStats *m = nullptr;
               {
                  ERDLock l(remoteNode.stats().getLock());
                  auto found = remoteNode.stats().messageStats().find(msg);
                  m = &found->second;
               }

               if (m == nullptr)
                  continue;

               EJsonBuilder::StackObject pushMsgObj(builder, m->getName());
               EJsonBuilder::StackUInt pushId(builder, m->getId(), "id");
               EJsonBuilder::StackUInt pushReceived(builder, m->getReceived(), "received");
               EJsonBuilder::StackUInt pushTimeout(builder, m->getTimeout(), "timeout");
               EJsonBuilder::StackArray pushSentArray(builder, "sent");
               for (auto sent : m->getSent())
                  EJsonBuilder::StackUInt pushSent(builder, sent);
            }
         }
      }
   }
   catch(std::exception &e)
   {
      Configuration::logger().major("{} - Unhandled exception", __method__);
   }
}

Void Stats::reset()
{
   std::vector<LocalNodeSPtr> localNodes;
   {
      ERDLock lck(CommunicationThread::Instance().localNodesLock());
      localNodes.reserve(CommunicationThread::Instance().localNodes().size());
      for (auto &iln : CommunicationThread::Instance().localNodes())
         localNodes.emplace_back(iln.second);
   }

   for (auto &localNodeSPtr : localNodes)
   {
      auto &localNode = *localNodeSPtr.get();
      
      std::vector<RemoteNodeSPtr> remoteNodes;
      {
         ERDLock lck(localNode.remoteNodesLock());
         remoteNodes.reserve(localNode.remoteNodes().size());
         for (auto &irn : localNode.remoteNodes())
            remoteNodes.emplace_back(irn.second);
      }

      for (auto &remoteNodeSPtr : remoteNodes)
      {
         auto &remoteNode = *remoteNodeSPtr.get();
         remoteNode.stats().reset();
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

UShort Configuration::port_                        = 8805;
Int Configuration::bufsize_                        = 2097152;
LongLong Configuration::t1_                        = 3000;
LongLong Configuration::hbt1_                      = 5000;
Int Configuration::n1_                             = 2;
Int Configuration::hbn1_                           = 3;
ELogger *Configuration::logger_                    = nullptr;
size_t Configuration::naw_                         = 10;
Long Configuration::law_                           = 6000; // 6 seconds
Int Configuration::trb_                            = 0;
Bool Configuration::atr_                           = False;
Translator *Configuration::xlator_                 = nullptr;
// Int Configuration::aminw_                       = 1;
// Int Configuration::amaxw_                       = 1;
Int Configuration::tminw_                          = 1;
Int Configuration::tmaxw_                          = 1;
_EThreadEventNotification *Configuration::app_     = nullptr;
ApplicationWorkGroupBase *Configuration::baseapp_  = nullptr;
MessageStatsMap Configuration::msgstats_template_;
MsgType Configuration::pfcpHeartbeatReq            = 1;
MsgType Configuration::pfcpHeartbeatRsp            = 2;
MsgType Configuration::pfcpSessionEstablishmentReq = 50;
MsgType Configuration::pfcpSessionEstablishmentRsp = 51;
MsgType Configuration::pfcpAssociationSetupReq     = 5;
MsgType Configuration::pfcpAssociationSetupRsp     = 6;

/// @cond DOXYGEN_EXCLUDE
TranslationThread *TranslationThread::this_     = nullptr;
CommunicationThread *CommunicationThread::this_ = nullptr;
EMemory::Pool SessionBase::pool_;
ULongLong SessionBase::created_                 = 0;
ULongLong SessionBase::deleted_                 = 0;
ULongLong Node::created_                        = 0;
ULongLong Node::deleted_                        = 0;
EMemory::Pool InternalMsg::pool_;
EMemory::Pool EventBase::pool_;
/// @endcond

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void* EventBase::operator new(size_t sz)
{
   if (pool_.allocSize() == 0)
   {
      size_t rawns = 4096;
      size_t as=sizeof(EThreadMessage), ns, bs, bc;

      as = std::max(as, sizeof(LocalNodeStateChangeEvent));
      as = std::max(as, sizeof(RemoteNodeStateChangeEvent));
      as = std::max(as, sizeof(RemoteNodeRestartEvent));

      while (rawns <= 32768)
      {
         // subtract the size of the node header (EMemory::Node)
         ns = rawns - sizeof(EMemory::Node);
         // set the block size equal to the allocation size + the block header
         bs = as + sizeof(EMemory::Block);
         // round the block size up to the CPU word size
         bs += bs % sizeof(pVoid);
         // calculate the number of blocks that can fit in a node
         bc = ns / bs;
         if (bc >= 10)
            break;
         rawns += 4096;
      }

      if (sz >= ns)
         pool_.setSize(sz, 0, 5);
      else
         pool_.setSize(as, rawns, bc);
   }
   if (sz > pool_.allocSize())
   {
      EError ex;
      ex.setSevere();
      ex.setTextf("EventBase allocation size (%u) is larger than memory pool block size", sz);
      throw ex;
   }
   return pool_.allocate();
}

void EventBase::operator delete(void* m)
{
   pool_.deallocate(m);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Void Initialize()
{
   static EString __method__ = __METHOD_NAME__;

   Configuration::logger().startup("{} - initializing the timer pool", __method__);
   ETimerPool::Instance().init();
   Configuration::logger().startup("{} - initializing the communication thread", __method__);
   CommunicationThread::Instance().init(1, 101, NULL, 100000);
   Configuration::logger().startup("{} - initializing the translation thread", __method__);
   TranslationThread::Instance().init(1, 102, NULL, 100000);
   #if 0
   Configuration::logger().startup("{} - sizeof(RspOut)={}", __method__, sizeof(RspOut));
   Configuration::logger().startup("{} - sizeof(RspIn)={}", __method__, sizeof(RspIn));
   Configuration::logger().startup("{} - sizeof(ReqOut)={}", __method__, sizeof(ReqOut));
   Configuration::logger().startup("{} - sizeof(ReqIn)={}", __method__, sizeof(ReqIn));
   #endif

   Configuration::pfcpHeartbeatReq = Configuration::translator().pfcpHeartbeatReq();
   Configuration::pfcpHeartbeatRsp = Configuration::translator().pfcpHeartbeatRsp();
   Configuration::pfcpSessionEstablishmentReq = Configuration::translator().pfcpSessionEstablishmentReq();
   Configuration::pfcpSessionEstablishmentRsp = Configuration::translator().pfcpSessionEstablishmentRsp();
   Configuration::pfcpAssociationSetupReq = Configuration::translator().pfcpAssociationSetupReq();
   Configuration::pfcpAssociationSetupRsp = Configuration::translator().pfcpAssociationSetupRsp();
}

Void Uninitialize()
{
   static EString __method__ = __METHOD_NAME__;

   Configuration::logger().startup("{} - releasing local nodes", __method__);
   CommunicationThread::Instance().releaseLocalNodes();

   Configuration::logger().startup("{} - stopping the translation thread", __method__);
   TranslationThread::Instance().quit();
   TranslationThread::Instance().join();
   TranslationThread::cleanup();

   Configuration::logger().startup("{} - stopping the communication thread", __method__);
   CommunicationThread::Instance().quit();
   CommunicationThread::Instance().join();
   CommunicationThread::cleanup();

   Configuration::logger().startup("{} - Session counts created={} deleted={}",
      __method__, SessionBase::sessionsCreated(), SessionBase::sessionsDeleted());
   Configuration::logger().startup("{} - Node counts created={} deleted={}",
      __method__, Node::nodesCreated(), Node::nodesDeleted());
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/// @cond DOXYGEN_EXCLUDE
TeidRangeManager::TeidRangeManager(Int rangeBits)
   : bits_(rangeBits)
{
   static EString __method__ = __METHOD_NAME__;

   if (bits_ < 0 || bits_ > 7)
      throw TeidRangeManager_InvalidRangeBits();
   
   Int range = 1 << rangeBits;
   for (Int i=0; i<range; i++)
      free_.push_back(i);
}

TeidRangeManager::~TeidRangeManager()
{
}

Bool TeidRangeManager::assign(RemoteNodeSPtr &n)
{
   static EString __method__ = __METHOD_NAME__;

   if (free_.size() > 0)
   {
      Int trv = free_.front();
      free_.pop_front();
      n->setTeidRangeValue(trv);
      used_[trv] = n;
      return True;
   }
   return False;
}

Void TeidRangeManager::release(RemoteNodeSPtr &n)
{
   static EString __method__ = __METHOD_NAME__;

   Int trv = n->teidRangeValue();
   if (used_.erase(trv) == 1)
      free_.push_back(trv);
   n->setTeidRangeValue(-1);
}
/// @endcond

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

NodeSocket::NodeSocket()
   : ESocket::UdpPrivate(CommunicationThread::Instance(), Configuration::socketBufferSize())
{
   static EString __method__ = __METHOD_NAME__;
}

NodeSocket::~NodeSocket()
{
   static EString __method__ = __METHOD_NAME__;
}

Void NodeSocket::onReceive(const ESocket::Address &src, const ESocket::Address &dst, cpUChar msg, Int len)
{
   static EString __method__ = __METHOD_NAME__;

   ln_->onReceive(ln_, src, dst, msg, len);
}

Void NodeSocket::onError()
{
   static EString __method__ = __METHOD_NAME__;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

RemoteNode::RemoteNode()
   : state_(RemoteNode::State::Initialized),
     trv_(-1),
     awndcnt_(0),
     aw_(0)
{
   static EString __method__ = __METHOD_NAME__;
   setStartTime(ETime(0));
   stats_.messageStats() = Configuration::messageStatsTemplate();
   stats_.reset();
}

RemoteNode::~RemoteNode()
{
   static EString __method__ = __METHOD_NAME__;
}

RemoteNode &RemoteNode::setNbrActivityWnds(size_t nbr)
{
   static EString __method__ = __METHOD_NAME__;

   awnds_.clear();
   for (size_t idx=0; idx<nbr; idx++)
      awnds_.push_back(0);
   awnds_.shrink_to_fit();
   awndcnt_ = 0;
   return *this;
}

RemoteNode &RemoteNode::deleteAllSesssions(RemoteNodeSPtr &rn)
{
   auto rn2 = new RemoteNodeSPtr(rn);
   SEND_TO_COMMUNICATION(DelNxtRmtSession, rn2);
   return *this;
}

ETime RemoteNode::Stats::getLastActivity()
{
   ERDLock l(lock_);
   return lastactivity_;
}

Void RemoteNode::Stats::setLastActivity()
{
   EWRLock l(lock_);
   lastactivity_ = ETime::Now();
}

Void RemoteNode::Stats::reset()
{
   EWRLock l(lock_);
   for (auto &msgstats : msgstats_)
      msgstats.second.reset();
}

/// @cond DOXYGEN_EXCLUDE
#define INCREMENT_MESSAGE_STAT(__id,__func,...) \
{                                               \
   MessageStats *msgstats = nullptr;            \
   {                                            \
      ERDLock l(lock_);                         \
      auto found = msgstats_.find(__id);        \
      if (found != msgstats_.end())             \
         msgstats = &found->second;             \
   }                                            \
   if (msgstats == nullptr)                     \
      return 0;                                 \
   setLastActivity();                           \
   return msgstats->__func(__VA_ARGS__);        \
}
/// @endcond

UInt RemoteNode::Stats::incReceived(MessageId msgid)            { INCREMENT_MESSAGE_STAT(msgid, incReceived) }
UInt RemoteNode::Stats::incSent(MessageId msgid, UInt attempt)  { INCREMENT_MESSAGE_STAT(msgid, incSent, attempt) }
UInt RemoteNode::Stats::incTimeout(MessageId msgid)             { INCREMENT_MESSAGE_STAT(msgid, incTimeout)}

#undef INCREMENT_MESSAGE_STAT

Void RemoteNode::collectStats(EJsonBuilder &builder)
{
   static EString __method__ = __METHOD_NAME__;

   try
   {
      EJsonBuilder::StackString pushRemoteAddress(builder, ipAddress().address(), "remote_address");            
      EJsonBuilder::StackString pushLastActivity(builder, stats().getLastActivity().Format("%i",True), "last_activity");
   }
   catch(std::exception &e)
   {
      Configuration::logger().major("{} - Unhandled exception", __method__);
   }
}

RemoteNode &RemoteNode::disconnect(RemoteNodeSPtr &rn)
{
   changeState(rn, RemoteNode::State::Stopping);
   deleteAllSesssions(rn);
   return *this;
}

/// @cond DOXYGEN_EXCLUDE
RemoteNode &RemoteNode::changeState(RemoteNodeSPtr &rn, State state)
{
   RemoteNodeStateChangeEvent *evnt = new RemoteNodeStateChangeEvent(
      rn, state_, state);
   state_ = state;
   SEND_TO_APPLICATION(RemoteNodeStateChange, evnt);
   return *this;
}

RemoteNode &RemoteNode::restarted(RemoteNodeSPtr &rn, const ETime &restartTime)
{
   RemoteNodeRestartEvent *evnt = new RemoteNodeRestartEvent(
      rn, state_, RemoteNode::State::Restarted, restartTime);
   state_ = RemoteNode::State::Restarted;
   SEND_TO_APPLICATION(RemoteNodeRestart, evnt);
   return *this;
}

Bool RemoteNode::addRcvdReq(ULong sn)
{
   static EString __method__ = __METHOD_NAME__;

   if (rcvdReqExists(sn))
      return False;

   RcvdReq rr(sn);

   auto result = rrumap_.insert(std::make_pair(rr.seqNbr(), rr));

   return result.second;
}

Bool RemoteNode::setRcvdReqRspWnd(ULong sn, Int wnd)
{
   static EString __method__ = __METHOD_NAME__;

   auto it = rrumap_.find(sn);
   if (it == rrumap_.end())
      return False;
   it->second.setRspWnd(wnd);
   return True;
}

Void RemoteNode::removeRcvdRqstEntries(Int wnd)
{
   static EString __method__ = __METHOD_NAME__;

   auto it = rrumap_.begin();
   while (it != rrumap_.end())
   {
      if (it->second.rspWnd() == wnd)
         it = rrumap_.erase(it);
      else
         it++;
   }
}

Void RemoteNode::nextActivityWnd(Int wnd)
{
   static EString __method__ = __METHOD_NAME__;

   aw_ = wnd;
   if (awndcnt_ < awnds_.size())
      awndcnt_++;
   awnds_[aw_] = 0;  // clear the previous activity in the new wnd
}

Bool RemoteNode::checkActivity()
{
   static EString __method__ = __METHOD_NAME__;

   if (awndcnt_ < awnds_.size())
   {
      // return True to indicate that there has been activity since
      // a full set of wnds has not been processed yet
      return True;
   }

   for (auto &val : awnds_)
   {
      if (val > 0)
         return True;
   }

   return False;
}

Void RemoteNode::removeOldReqs(Int rw)
{
   static EString __method__ = __METHOD_NAME__;

   auto entry = rrumap_.begin();
   while (entry != rrumap_.end())
   {
      if (entry->second.rspWnd() == rw)
         entry = rrumap_.erase(entry);
      else
         entry++;
   }
}

RemoteNode &RemoteNode::addSession(SessionBaseSPtr &s)
{
   static EString __method__ = __METHOD_NAME__;
   if (s && s->remoteSeid() != 0)
      _addSession(s->remoteSeid(), s);
   return *this;
}

RemoteNode &RemoteNode::delSession(SessionBaseSPtr &s)
{
   static EString __method__ = __METHOD_NAME__;
   if (s && s->remoteSeid() != 0)
      _delSession(s->remoteSeid());
   return *this;
}
/// @endcond

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

LocalNode::LocalNode()
   : state_(LocalNode::State::Initialized)
{
   static EString __method__ = __METHOD_NAME__;
}

LocalNode::~LocalNode()
{
   static EString __method__ = __METHOD_NAME__;

   auto entry = roumap_.begin();
   while (entry != roumap_.end())
   {
      delete entry->second;
      entry = roumap_.erase(entry);
   }
}

Seid LocalNode::allocSeid()
{
   static EString __method__ = __METHOD_NAME__;
   return seidmgr_.alloc();
}

Void LocalNode::freeSeid(Seid seid)
{
   static EString __method__ = __METHOD_NAME__;
   seidmgr_.free(seid);
}

ULong LocalNode::allocSeqNbr()
{
   static EString __method__ = __METHOD_NAME__;
   return seqmgr_.alloc();
}

Void LocalNode::freeSeqNbr(ULong sn)
{
   static EString __method__ = __METHOD_NAME__;
   seqmgr_.free(sn);
}

/// @cond DOXYGEN_EXCLUDE
LocalNode &LocalNode::changeState(LocalNodeSPtr &ln, LocalNode::State state)
{
   LocalNodeStateChangeEvent *evnt = new LocalNodeStateChangeEvent(ln, state_, state);
   state_ = state;
   SEND_TO_APPLICATION(LocalNodeStateChange, evnt);
   return *this;
}

Bool LocalNode::rqstOutExists(ULong seqnbr) const
{
   static EString __method__ = __METHOD_NAME__;

   auto it = roumap_.find(seqnbr);
   return it != roumap_.end();
}

Bool LocalNode::addRqstOut(ReqOut *ro)
{
   static EString __method__ = __METHOD_NAME__;

   if (rqstOutExists(ro->seqNbr()))
      return False;
   
   auto result = roumap_.insert(std::make_pair(ro->seqNbr(), ro));

   return result.second;
}

Bool LocalNode::setRqstOutRespWnd(ULong seqnbr, Int wnd)
{
   static EString __method__ = __METHOD_NAME__;

   auto it = roumap_.find(seqnbr);
   if (it == roumap_.end())
      return False;
   it->second->setRspWnd(wnd);
   return True;
}

Void LocalNode::removeRqstOutEntries(Int wnd)
{
   static EString __method__ = __METHOD_NAME__;

   auto it = roumap_.begin();
   while (it != roumap_.end())
   {
      if (it->second->rspWnd() == wnd)
      {
         ReqOut *ro = it->second;
         it = roumap_.erase(it);
         delete ro;
      }
      else
      {
         it++;
      }
   }
}

Void LocalNode::clearRqstOutEntries()
{
   auto it = roumap_.begin();
   while (it != roumap_.end())
   {
      ReqOutPtr ro = it->second;
      it = roumap_.erase(it);
      delete ro;
   }
}
/// @endcond

Void LocalNode::setNbrActivityWnds(size_t nbr)
{
   static EString __method__ = __METHOD_NAME__;

   ERDLock lck(rnslck_);
   for (auto &kv : rns_)
      kv.second->setNbrActivityWnds(nbr);
}

/// @cond DOXYGEN_EXCLUDE
Void LocalNode::nextActivityWnd(Int wnd)
{
   static EString __method__ = __METHOD_NAME__;

   ERDLock lck(rnslck_);
   for (auto &kv : rns_)
      kv.second->nextActivityWnd(wnd);
}

Void LocalNode::checkActivity(LocalNodeSPtr &ln)
{
   static EString __method__ = __METHOD_NAME__;

   // check for activity from all of the RemoteNode's
   ERDLock lck(rnslck_);
   for (auto &kv : rns_)
   {
      if (kv.second->state() == RemoteNode::State::Started && !kv.second->checkActivity())
      {
         // Configuration::logger().info("{} - remote {} is inactive", __method__, kv.second->ipAddress().address());
         SndHeartbeatReqDataPtr shb = new SndHeartbeatReqData(ln, kv.second);
         SEND_TO_TRANSLATION(SndHeartbeatReq, shb);

         // increment the activity for the RemoteNode to prevent a heartbeat from going out 
         // until there is no activity in all windows
         kv.second->incrementActivity();
      }
   }
}
/// @endcond

RemoteNodeSPtr LocalNode::createRemoteNode(EIpAddress &address, UShort port)
{
   static EString __method__ = __METHOD_NAME__;

   try
   {
      EWRLock lck(rnslck_);

      // if the remote node exists and is in a non-started state, start it
      RemoteNodeUMap::iterator rnit = rns_.find(address);
      if (rnit != rns_.end())
      {
         if (rnit->second->state() == RemoteNode::State::Started)
            throw EError(EError::Severity::Warning, "Unable to start remote node, already started");
         Configuration::logger().debug("{} - restarting remote node address={}",
            __method__, address.address());
         rnit->second->changeState(rnit->second, RemoteNode::State::Started);
         return rnit->second;
      }

      // create the RemoteNode shared pointer
      RemoteNodeSPtr rn = Configuration::baseApplication()._createRemoteNode();

      // set the IP address for the RemoteNode
      if (address.family() == AF_INET)
         rn->setAddress(ESocket::Address(address.ipv4Address(), port));
      else
         rn->setAddress(ESocket::Address(address.ipv6Address(), port));
      rn->setIpAddress(address);

      // assign the TEID range value for the RemotNode
      if (!CommunicationThread::Instance().assignTeidRangeValue(rn))
         throw RemoteNodeException_UnableToAssignTeidRangeValue();

      // set the number of activity wnds for the RemoteNode
      rn->setNbrActivityWnds(Configuration::nbrActivityWnds());

      // set the activity wnd
      rn->nextActivityWnd(CommunicationThread::Instance().currentActivityWnd());

      // add the RemoteNode to the RemodeNode collection for this LocalNode
      auto result = rns_.insert(std::make_pair(rn->ipAddress(), rn));

      if (!result.second)
         throw LocalNodeException_RemoteNodeUMapInsertFailed();
      
      rn->changeState(rn, RemoteNode::State::Started);

      return rn;
   }
   catch (const std::exception &e)
   {
      Configuration::logger().minor("{} - address={} exception - {}",
         __method__, address.address(), e.what());
      throw LocalNodeException_UnableToCreateRemoteNode();
   }
}

Void LocalNode::collectStats(EJsonBuilder &builder)
{
   static EString __method__ = __METHOD_NAME__;

   try
   {
      EJsonBuilder::StackString pushLocalAddress(builder, ipAddress().address(), "local_address");
   }
   catch(std::exception &e)
   {
      Configuration::logger().major("{} - Unhandled exception", __method__);
   }
}

SessionBaseSPtr LocalNode::createSession(LocalNodeSPtr &ln, RemoteNodeSPtr &rn)
{
   SessionBaseSPtr s = Configuration::baseApplication()._createSession(ln, rn);
   s->setLocalSeid(s, allocSeid());
   return s;
}

SessionBaseSPtr LocalNode::createSession(LocalNodeSPtr &ln, Seid rs, RemoteNodeSPtr &rn)
{
   SessionBaseSPtr s = Configuration::baseApplication()._createSession(ln, rn);
   s->setSeid(s, allocSeid(), rs);
   return s;
}

/// @cond DOXYGEN_EXCLUDE
Void LocalNode::onReceive(LocalNodeSPtr &ln, const ESocket::Address &src, const ESocket::Address &dst, cpUChar msg, Int len)
{
   static EString __method__ = __METHOD_NAME__;
   TranslatorMsgInfo tmi;
   RemoteNodeSPtr rn;

   try
   {
      // get the msg header info
      Configuration::translator().getMsgInfo(tmi, msg, len);

      // lookup the remote node and create it if it does not exist
      {
         EIpAddress remoteIpAddress(src.getSockAddrStorage());
         Bool exists = False;
         {
            ERDLock lck(rnslck_);
            RemoteNodeUMap::iterator rnit = rns_.find(remoteIpAddress);
            // Configuration::logger().debug("{} - looking up remoteNode {}", __method__, remoteIpAddress.address());
            if (rnit != rns_.end())
            {
               // Configuration::logger().debug("{} - remoteNode {} found", __method__, remoteIpAddress.address());
               rn = rnit->second;
               if (rn->state() != RemoteNode::State::Started)
                  rn->changeState(rn, RemoteNode::State::Started);
               exists = True;
            }
         }
         
         if (!exists)
         {
            // Configuration::logger().debug("{} - remoteNode {} NOT found", __method__, remoteIpAddress.address());
            rn = createRemoteNode(remoteIpAddress, Configuration::port());
         }
      }

      // increment the activity for the RemoteNode
      rn->incrementActivity();

      // increment the stats for the RemoteNode
      rn->stats().incReceived(tmi.msgType());

      if (tmi.isReq())
      {
         // check to see if this is a duplicate req
         if (!rn->rcvdReqExists(tmi.seqNbr()))
         {
            // create and populate ReqIn
            ReqInPtr ri = new ReqIn(ln, rn, tmi, msg, len);

            // lookup or create the session
            if (tmi.createSession())
            {
               ri->setSession(ln->createSession(ln, rn));
               if(!ri->session())
               {
                  // session not found but required
                  Configuration::logger().debug(
                     "{} - unable to create the session, discarding the message"
                     " localNode={} remoteNode={} localSeid={} msgType={} msgClass=SESSION seqNbr={} version={} msgLen={}",
                     __method__, ln->ipAddress().address(), rn->ipAddress().address(), tmi.seid(),
                     tmi.msgType(), tmi.seqNbr(), tmi.version(), len);
                  return;
               }
            }
            else if (tmi.msgClass() == MsgClass::Session)
            {
               ri->setSession(ln->getSession(tmi.seid()));
               if (!ri->session())
               {
                  // session not found but required
                  Configuration::logger().debug(
                     "{} - session not found, discarding the message"
                     " localNode={} remoteNode={} localSeid={} msgType={} msgClass=SESSION seqNbr={} version={} msgLen={}",
                     __method__, ln->ipAddress().address(), rn->ipAddress().address(), tmi.seid(),
                     tmi.msgType(), tmi.seqNbr(), tmi.version(), len);
                  return;
               }
            }

            // add RcvdReqeust
            if (rn->addRcvdReq(ri->seqNbr()))
            {
               // snd ReqIn to TranslationThread
               SEND_TO_TRANSLATION(RcvdReq, ri);
            }
            else
            {
               if (tmi.msgClass() == MsgClass::Session)
               {
                  Configuration::logger().debug(
                     "{} - unable to insert RcvdReq in the RemoteNode,"
                     " discarding req local={} remote={} seid={} msgType={} msgClass=SESSION seqNbr={} version={} msgLen={}",
                     __method__, ln->ipAddress().address(), rn->ipAddress().address(), tmi.seid(),
                     tmi.msgType(), tmi.seqNbr(), tmi.version(), len);
               }
               else
               {
                  Configuration::logger().debug(
                     "{} - unable to insert RcvdReq in the RemoteNode,"
                     " discarding req local={} remote={} msgType={} msgClass={} seqNbr={} version={} msgLen={}",
                     __method__, ln->ipAddress().address(), rn->ipAddress().address(), tmi.msgType(),
                     tmi.msgClass()==MsgClass::Node?"NODE":"UNKNOWN", tmi.seqNbr(), tmi.version(), len);
               }
               delete ri;
            }
         }
         else
         {
            // duplicate msg, so discard it
            if (tmi.msgClass() == MsgClass::Session)
            {
               Configuration::logger().debug(
                  "{} - discarding duplicate req local={} remote={} seid={} msgType={} msgClass=SESSION seqNbr={} version={} msgLen={}",
                  __method__, ln->ipAddress().address(), rn->ipAddress().address(), tmi.seid(),
                  tmi.msgType(), tmi.seqNbr(), tmi.version(), len);
            }
            else
            {
               Configuration::logger().debug(
                  "{} - discarding duplicate req local={} remote={} msgType={} msgClass={} seqNbr={} version={} msgLen={}",
                  __method__, ln->ipAddress().address(), rn->ipAddress().address(), tmi.msgType(),
                  tmi.msgClass()==MsgClass::Node?"NODE":"UNKNOWN", tmi.seqNbr(), tmi.version(), len);
            }
         }
      }
      else
      {
         // locate the corresponding ReqOut entry
         auto roit = roumap_.find(tmi.seqNbr());
         if (roit != roumap_.end())
         {
            // ReqOut entry found, set the rsp wnd for the req
            roit->second->setRspWnd(CommunicationThread::Instance().currentRspWnd());

            // stop the retransmit timer
            roit->second->stopT1();

            // create and poulate RspIn
            RspInPtr ri = new RspIn(ln, rn, tmi, msg, len, roit->second->appMsg());

            roit->second->setAppMsg(nullptr);

            // snd RspIn to TranslationThread
            SEND_TO_TRANSLATION(RcvdRsp, ri);
         }
         else
         {
            // ReqOut entry NOT found, discard the rsp msg
            if (tmi.msgClass() == MsgClass::Session)
            {
               Configuration::logger().info(
                  "{} - corresponding ReqOut entry not found, discarding rsp "
                  "local={} remote={} seid={} msgType={} msgClass=SESSION seqNbr={} version={} msgLen={}",
                  __method__, ln->ipAddress().address(), rn->ipAddress().address(), tmi.seid(),
                  tmi.msgType(), tmi.seqNbr(), tmi.version(), len);
            }
            else
            {
               Configuration::logger().info(
                  "{} - corresponding ReqOut entry not found, "
                  "discarding rsp local={} remote={} msgType={} msgClass={} seqNbr={} version={} msgLen={}",
                  __method__, ln->ipAddress().address(), rn->ipAddress().address(), tmi.msgType(),
                  tmi.msgClass()==MsgClass::Node?"NODE":"UNKNOWN", tmi.seqNbr(), tmi.version(), len);
            }
         }
      }
   }
   catch (const LocalNodeException_UnableToCreateRemoteNode &e)
   {
      if (tmi.msgClass() == MsgClass::Session)
      {
         Configuration::logger().minor(
            "{} - {} - discarding msg local={} remote={} seid={} msgType={} msgClass=SESSION seqNbr={} version={} msgLen={}",
            __method__, e.what(), dst.getAddress(), src.getAddress(), tmi.seid(),
            tmi.msgType(), tmi.seqNbr(), tmi.version(), len);
      }
      else
      {
         Configuration::logger().minor(
            "{} - {} - discarding msg local={} remote={} msgType={} msgClass={} seqNbr={} version={} msgLen={}",
            __method__, e.what(), dst.getAddress(), src.getAddress(), tmi.msgType(),
            tmi.msgClass()==MsgClass::Node?"NODE":"UNKNOWN", tmi.seqNbr(), tmi.version(), len);
      }
   }
}

Bool LocalNode::onReqOutTimeout(ReqOutPtr ro)
{
   static EString __method__ = __METHOD_NAME__;
   if (ro == nullptr)
   {
      Configuration::logger().minor("{} - the ReqOutPtr is NULL", __method__);
      return False;
   }

   // lookup the ReqOut entry
   auto entry = roumap_.find(ro->seqNbr());
   if (entry != roumap_.end()) // found the entry
   {
      if (sndReq(ro))
         return True;
      roumap_.erase(entry);
   }
   else
   {
      // ReqOut entry NOT found, do not surface the timeout error
      Configuration::logger().debug(
         "{} - corresponding ReqOut entry not found,"
         " discarding timeout local={} remote={}",
         __method__, ipAddress().address(), ro->remoteNode()->address().getAddress());
   }

   ro->remoteNode()->stats().incTimeout(ro->msgType());

   return False;
}

Void LocalNode::removeOldReqs(Int rw)
{
   static EString __method__ = __METHOD_NAME__;

   // remove the old ReqOut entries
   auto entry = roumap_.begin();
   while (entry != roumap_.end())
   {
      if (entry->second->rspWnd() == rw)
      {
         ReqOutPtr ro = entry->second;
         entry = roumap_.erase(entry);
         delete ro;
      }
      else
      {
         entry++;
      }
   }

   // remove the old RcvdReq entries
   ERDLock lck(rnslck_);
   for (auto &kv : rns_)
      kv.second->removeOldReqs(rw);
}

Void LocalNode::sndInitialReq(ReqOutPtr ro)
{
   static EString __method__ = __METHOD_NAME__;

   // check to see if the ReqOut entry already exists
   auto roit = roumap_.find(ro->seqNbr());
   if (roit == roumap_.end())
   {
      // add the ReqOut entry to retransmit collection
      roumap_.insert(std::make_pair(ro->seqNbr(), ro));
      sndReq(ro);
   }
   else
   {
      // log the error
      if (ro->msgClass() == MsgClass::Session)
      {
         Configuration::logger().major(
            "{} - seqNbr {} already exists in retransmission collection "
            "local={} remote={} localSeid={} remoteSeid={} msgType={} msgClass=SESSION seqNbr={} version={} msgLen={}",
            __method__, ipAddress().address(), ro->remoteNode()->ipAddress().address(),
            static_cast<AppMsgSessionReqPtr>(ro->appMsg())->session()->localSeid(),
            static_cast<AppMsgSessionReqPtr>(ro->appMsg())->session()->remoteSeid(),
            ro->msgType(), ro->seqNbr(), ro->version(), ro->len());
      }
      else
      {
         Configuration::logger().major(
            "{} - seqNbr {} already exists in retransmission collection "
            "local={} remote={} msgType={} msgClass={} seqNbr={} version={} msgLen={}",
            __method__, ipAddress().address(), ro->remoteNode()->ipAddress().address(), ro->msgType(),
            ro->msgClass()==MsgClass::Node?"NODE":"UNKNOWN", ro->seqNbr(), ro->version(), ro->len());
      }
      // snd the error to the application thread
      SndReqExceptionData *exdata = new SndReqExceptionData();
      exdata->req = ro->appMsg();
      SEND_TO_APPLICATION(SndReqError, exdata);
      // delete the ReqOut object
      delete ro;
   }
}

Bool LocalNode::sndReq(ReqOutPtr ro)
{
   static EString __method__ = __METHOD_NAME__;

   if (ro->okToSnd()) // implicitly decrements the retransmission count
   {
      // snd the data
      socket_.write(ro->remoteNode()->address(), ro->data(), ro->len());
      ro->startT1();

      UInt attempt = (ro->msgType() == Configuration::pfcpHeartbeatReq ? Configuration::heartbeatN1() : Configuration::n1()) - (ro->n1() + 1);
      ro->remoteNode()->stats().incSent(ro->msgType(), attempt);

      return True;
   }

   if (ro->msgType() == Configuration::pfcpHeartbeatReq)
   {
      // send remote node failure notice to the ApplicationThread
      ro->remoteNode()->changeState(ro->remoteNode(), RemoteNode::State::Failed);

      Configuration::logger().major(
         "{} - remote node is non-responsive local={} remote={}",
         __method__, address().getAddress(), ro->remoteNode()->address().getAddress());

      // delete the AppMsgReq object since the communication thread is responsible for it
      delete ro->appMsg();
   }
   else
   {
      // send the timeout notice for the AppMsgReq to the ApplicationThread
      SEND_TO_APPLICATION(ReqTimeout, ro->appMsg());
   }
   // clear the AppMsgReqPtr (doesn't do anything since the ReqOut destructor doesn't try to delete it)
   ro->setAppMsg(nullptr);

   return False;
}

Void LocalNode::sndRsp(RspOutPtr ro)
{
   static EString __method__ = __METHOD_NAME__;

   if (ro->remoteNode()->setRcvdReqRspWnd(ro->seqNbr()))
   {
      // snd the data
      socket_.write(ro->remoteNode()->address(), ro->data(), ro->len());

      ro->remoteNode()->stats().incSent(ro->msgType());
   }
   else
   {
      // the corresponding req does not exist, so don't snd the rsp
      SEND_TO_APPLICATION(SndRspError, ro->appMsg());
      ro->setAppMsg(nullptr);
   }
   // delete the RspOut object
   delete ro;
}
/// @endcond

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Translator::Translator()
{
}

Translator::~Translator()
{
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/// @cond DOXYGEN_EXCLUDE
ApplicationWorker::ApplicationWorker()
{
   static EString __method__ = __METHOD_NAME__;
}

ApplicationWorker::~ApplicationWorker()
{
   static EString __method__ = __METHOD_NAME__;
}
/// @endcond

Void ApplicationWorker::onInit()
{
   static EString __method__ = __METHOD_NAME__;

   EThreadWorkerPrivate::onInit();
}

Void ApplicationWorker::onQuit()
{
   static EString __method__ = __METHOD_NAME__;

   EThreadWorkerPrivate::onQuit();
}

Void ApplicationWorker::onRcvdReq(AppMsgReqPtr req)
{
   static EString __method__ = __METHOD_NAME__;
   if (req->msgClass() == PFCP::MsgClass::Session)
      Configuration::logger().debug(
         "{} - workerId={}"
         " seid={}"
         " seqNbr={}"
         " msgType={}"
         " msgClass=SESSION"
         " isReq={}",
         __method__, workerId(), static_cast<AppMsgSessionReqPtr>(req)->session()->localSeid(),
         req->seqNbr(), req->msgType(), (req->isReq()?"True":"False"));
   else
      Configuration::logger().debug(
         "{} - workerId={}"
         " seqNbr={}"
         " msgType={}"
         " msgClass={}"
         " isReq={}",
         __method__, workerId(), req->seqNbr(), req->msgType(), req->msgClass()==MsgClass::Node?"NODE":"UNKNOWN",
         (req->isReq()?"True":"False"));
}

Void ApplicationWorker::onRcvdRsp(AppMsgRspPtr rsp)
{
   static EString __method__ = __METHOD_NAME__;
   if (rsp->msgClass() == PFCP::MsgClass::Session)
      Configuration::logger().debug(
         "{} - workerId={}"
         " seid={}"
         " seqNbr={}"
         " msgType={}"
         " msgClass=SESSION"
         " isReq={}",
         __method__, workerId(), static_cast<AppMsgSessionRspPtr>(rsp)->req()->session()->localSeid(),
         rsp->seqNbr(), rsp->msgType(), (rsp->isReq()?"True":"False"));
   else
      Configuration::logger().debug(
         "{} - workerId={}"
         " seqNbr={}"
         " msgType={}"
         " msgClass={}"
         " isReq={}",
         __method__, workerId(), rsp->seqNbr(), rsp->msgType(), rsp->msgClass()==MsgClass::Node?"NODE":"UNKNOWN",
         (rsp->isReq()?"True":"False"));
}

Void ApplicationWorker::onReqTimeout(AppMsgReqPtr req)
{
   static EString __method__ = __METHOD_NAME__;
   if (req->msgClass() == PFCP::MsgClass::Session)
      Configuration::logger().debug(
         "{} - workerId={}"
         " seid={}"
         " seqNbr={}"
         " msgType={}"
         " msgClass=SESSION"
         " isReq={}",
         __method__, workerId(), static_cast<AppMsgSessionReqPtr>(req)->session()->localSeid(),
         req->seqNbr(), req->msgType(), (req->isReq()?"True":"False"));
   else
      Configuration::logger().debug(
         "{} - workerId={}"
         " seqNbr={}"
         " msgType={}"
         " msgClass={}"
         " isReq={}",
         __method__, workerId(), req->seqNbr(), req->msgType(), req->msgClass()==MsgClass::Node?"NODE":"UNKNOWN",
         (req->isReq()?"True":"False"));
}

Void ApplicationWorker::onLocalNodeStateChange(LocalNodeSPtr &ln, LocalNode::State oldState, LocalNode::State newState)
{
   static EString __method__ = __METHOD_NAME__;
   Configuration::logger().debug(
      "{}"
      " workerId={}"
      " address={} oldState={} newState={}",
      __method__, workerId(), ln->ipAddress().address(), oldState, newState);
}

Void ApplicationWorker::onRemoteNodeStateChange(RemoteNodeSPtr &rn, RemoteNode::State oldState, RemoteNode::State newState)
{
   static EString __method__ = __METHOD_NAME__;
   Configuration::logger().debug(
      "{}"
      " workerId={}"
      " address={} oldState={} newState={}",
      __method__, workerId(), rn->ipAddress().address(), oldState, newState);
}

Void ApplicationWorker::onRemoteNodeRestart(RemoteNodeSPtr &rn, const ETime &restartTime)
{
   static EString __method__ = __METHOD_NAME__;
   Configuration::logger().debug(
      "{}"
      " workerId={}"
      " address={} restartTime={}",
      __method__, workerId(), rn->ipAddress().address(), restartTime.Format("%i",False));
}

Void ApplicationWorker::onSndReqError(AppMsgReqPtr req, SndReqException &err)
{
   static EString __method__ = __METHOD_NAME__;
   if (req->msgClass() == PFCP::MsgClass::Session)
      Configuration::logger().debug(
         "{} - workerId={}"
         " seid={}"
         " seqNbr={}"
         " msgType={}"
         " msgClass=SESSION"
         " isReq={}"
         " exception={}",
         __method__, workerId(), static_cast<AppMsgSessionReqPtr>(req)->session()->localSeid(),
         req->seqNbr(), req->msgType(), (req->isReq()?"True":"False"), err.what());
   else
      Configuration::logger().debug(
         "{} - workerId={}"
         " seqNbr={}"
         " msgType={}"
         " msgClass={}"
         " isReq={}"
         " exception={}",
         __method__, workerId(), req->seqNbr(), req->msgType(), req->msgClass()==MsgClass::Node?"NODE":"UNKNOWN",
         (req->isReq()?"True":"False"), err.what());
}

Void ApplicationWorker::onSndRspError(AppMsgRspPtr rsp, SndRspException &err)
{
   static EString __method__ = __METHOD_NAME__;
   if (rsp->msgClass() == PFCP::MsgClass::Session)
      Configuration::logger().debug(
         "{} - workerId={}"
         " seid={}"
         " seqNbr={}"
         " msgType={}"
         " msgClass=SESSION"
         " isReq={}"
         " exception={}",
         __method__, workerId(), static_cast<AppMsgSessionRspPtr>(rsp)->req()->session()->localSeid(),
         rsp->seqNbr(), rsp->msgType(), (rsp->isReq()?"True":"False"), err.what());
   else
      Configuration::logger().debug(
         "{} - workerId={}"
         " seqNbr={}"
         " msgType={}"
         " msgClass={}"
         " isReq={}"
         " exception={}",
         __method__, workerId(), rsp->seqNbr(), rsp->msgType(), rsp->msgClass()==MsgClass::Node?"NODE":"UNKNOWN",
         (rsp->isReq()?"True":"False"), err.what());
}

Void ApplicationWorker::onEncodeReqError(PFCP::AppMsgReqPtr req, PFCP::EncodeReqException &err)
{
   static EString __method__ = __METHOD_NAME__;
   if (req->msgClass() == PFCP::MsgClass::Session)
      Configuration::logger().debug(
         "{} - workerId={}"
         " seid={}"
         " seqNbr={}"
         " msgType={}"
         " msgClass=SESSION"
         " isReq={}"
         " exception={}",
         __method__, workerId(), static_cast<AppMsgSessionReqPtr>(req)->session()->localSeid(),
         req->seqNbr(), req->msgType(), (req->isReq()?"True":"False"), err.what());
   else
      Configuration::logger().debug(
         "{} - workerId={}"
         " seqNbr={}"
         " msgType={}"
         " msgClass={}"
         " isReq={}"
         " exception={}",
         __method__, workerId(), req->seqNbr(), req->msgType(), req->msgClass()==MsgClass::Node?"NODE":"UNKNOWN",
         (req->isReq()?"True":"False"), err.what());
}

Void ApplicationWorker::onEncodeRspError(PFCP::AppMsgRspPtr rsp, PFCP::EncodeRspException &err)
{
   static EString __method__ = __METHOD_NAME__;
   if (rsp->msgClass() == PFCP::MsgClass::Session)
      Configuration::logger().debug(
         "{} - workerId={}"
         " seid={}"
         " seqNbr={}"
         " msgType={}"
         " msgClass=SESSION"
         " isReq={}"
         " exception={}",
         __method__, workerId(), static_cast<AppMsgSessionRspPtr>(rsp)->req()->session()->localSeid(),
         rsp->seqNbr(), rsp->msgType(), (rsp->isReq()?"True":"False"), err.what());
   else
      Configuration::logger().debug(
         "{} - workerId={}"
         " seqNbr={}"
         " msgType={}"
         " msgClass={}"
         " isReq={}"
         " exception={}",
         __method__, workerId(), rsp->seqNbr(), rsp->msgType(), rsp->msgClass()==MsgClass::Node?"NODE":"UNKNOWN",
         (rsp->isReq()?"True":"False"), err.what());
}

/// @cond DOXYGEN_EXCLUDE
Void ApplicationWorker::_onRcvdReq(EThreadMessage &msg)
{
   static EString __method__ = __METHOD_NAME__;
   AppMsgReqPtr req = static_cast<AppMsgReqPtr>(msg.getVoidPtr());
   onRcvdReq(req);
}

Void ApplicationWorker::_onRcvdRsp(EThreadMessage &msg)
{
   static EString __method__ = __METHOD_NAME__;
   AppMsgRspPtr rsp =  static_cast<AppMsgRspPtr>(msg.getVoidPtr());
   onRcvdRsp(rsp);
}

Void ApplicationWorker::_onReqTimeout(EThreadMessage &msg)
{
   static EString __method__ = __METHOD_NAME__;
   AppMsgReqPtr req = static_cast<AppMsgReqPtr>(msg.getVoidPtr());
   onReqTimeout(req);
}

Void ApplicationWorker::_onLocalNodeStateChange(EThreadMessage &msg)
{
   static EString __method__ = __METHOD_NAME__;
   LocalNodeStateChangeEvent *evnt = static_cast<LocalNodeStateChangeEvent*>(msg.getVoidPtr());
   onLocalNodeStateChange(evnt->localNode(), evnt->oldState(), evnt->newState());
   delete evnt;
}

Void ApplicationWorker::_onRemoteNodeStateChange(EThreadMessage &msg)
{
   static EString __method__ = __METHOD_NAME__;
   RemoteNodeStateChangeEvent *evnt = static_cast<RemoteNodeStateChangeEvent*>(msg.getVoidPtr());
   onRemoteNodeStateChange(evnt->remoteNode(), evnt->oldState(), evnt->newState());
   delete evnt;
}

Void ApplicationWorker::_onRemoteNodeRestart(EThreadMessage &msg)
{
   static EString __method__ = __METHOD_NAME__;
   RemoteNodeRestartEvent *evnt = static_cast<RemoteNodeRestartEvent*>(msg.getVoidPtr());
   onRemoteNodeRestart(evnt->remoteNode(), evnt->restartTime());
   delete evnt;
}

Void ApplicationWorker::_onSndReqError(EThreadMessage &msg)
{
   static EString __method__ = __METHOD_NAME__;
   SndReqExceptionDataPtr data = static_cast<SndReqExceptionDataPtr>(msg.getVoidPtr());
   onSndReqError(data->req, data->err);
   delete data;
}

Void ApplicationWorker::_onSndRspError(EThreadMessage &msg)
{
   static EString __method__ = __METHOD_NAME__;
   SndRspExceptionDataPtr data = static_cast<SndRspExceptionDataPtr>(msg.getVoidPtr());
   onSndRspError(data->rsp, data->err);
   delete data;
}

Void ApplicationWorker::_onEncodeReqError(EThreadMessage &msg)
{
   static EString __method__ = __METHOD_NAME__;
   EncodeReqExceptionDataPtr data = static_cast<EncodeReqExceptionDataPtr>(msg.getVoidPtr());
   onEncodeReqError(data->req, data->err);
   delete data;
}

Void ApplicationWorker::_onEncodeRspError(EThreadMessage &msg)
{
   static EString __method__ = __METHOD_NAME__;
   EncodeRspExceptionDataPtr data = static_cast<EncodeRspExceptionDataPtr>(msg.getVoidPtr());
   onEncodeRspError(data->rsp, data->err);
   delete data;
}
/// @endcond

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(TranslationThread, EThreadPrivate)
   ON_MESSAGE(static_cast<UInt>(TranslationThread::Events::SndMsg), TranslationThread::onSndPfcpMsg)
   ON_MESSAGE(static_cast<UInt>(TranslationThread::Events::RcvdReq), TranslationThread::onRcvdReq)
   ON_MESSAGE(static_cast<UInt>(TranslationThread::Events::RcvdRsp), TranslationThread::onRcvdRsp)
   ON_MESSAGE(static_cast<UInt>(TranslationThread::Events::SndHeartbeatReq), TranslationThread::onSndHeartbeatReq)
   ON_MESSAGE(static_cast<UInt>(TranslationThread::Events::SndHeartbeatRsp), TranslationThread::onSndHeartbeatRsp)
END_MESSAGE_MAP()

/// @cond DOXYGEN_EXCLUDE
TranslationThread::TranslationThread()
   : xlator_(Configuration::translator())
{
   static EString __method__ = __METHOD_NAME__;

   this_ = this;
}

TranslationThread::~TranslationThread()
{
   static EString __method__ = __METHOD_NAME__;

   this_ = nullptr;
}

Void TranslationThread::onInit()
{
   static EString __method__ = __METHOD_NAME__;

   EThreadPrivate::onInit();

   Configuration::logger().startup("{} - the translation thread has been started", __method__);
}

Void TranslationThread::onQuit()
{
   static EString __method__ = __METHOD_NAME__;

   EThreadPrivate::onQuit();
}

Void TranslationThread::onSndPfcpMsg(EThreadMessage &msg)
{
   static EString __method__ = __METHOD_NAME__;
   AppMsgPtr am = static_cast<AppMsgPtr>(msg.getVoidPtr());

   if (am->isReq())
   {
      AppMsgReqPtr amrq = static_cast<AppMsgReqPtr>(am);
      ReqOutPtr reqout = nullptr;

      if (amrq->msgClass() == PFCP::MsgClass::Node)
      {
         Configuration::logger().debug("{} - sending request msgType={} msgClass=NODE seqNbr={}",
            __method__, amrq->msgType(), am->seqNbr());
      }
      else if (amrq->msgClass() == PFCP::MsgClass::Session)
      {
         Configuration::logger().debug("{} - sending request seid={} msgType={} msgClass=SESSION seqNbr={}",
            __method__, static_cast<AppMsgSessionReqPtr>(amrq)->session()->localSeid(), amrq->msgType(), am->seqNbr());
      }
      else
      {
         Configuration::logger().debug("{} - sending request msgType={} msgClass=UNKNOWN seqNbr={}",
            __method__, amrq->msgType(), am->seqNbr());
      }

      try
      {
         if (amrq->msgClass() == MsgClass::Node ||
               amrq->msgType() == Configuration::pfcpSessionEstablishmentReq ||
               static_cast<AppMsgSessionReqPtr>(amrq)->session()->remoteSeid() != 0)
         {
            reqout = xlator_.encodeReq(amrq);
            SEND_TO_COMMUNICATION(SndReq, reqout);
         }
         else
         {
            EncodeReqException ex;
            ex.appendText(" - invalid session remote SEID (0)");
            throw ex;
         }
      }
      catch(SndReqException &e)
      {
         auto data = new SndReqExceptionData();
         data->req = amrq;
         data->err = e;
         SEND_TO_APPLICATION(SndReqError, data);
         if (reqout != nullptr)
            delete reqout;
      }
      catch(EncodeReqException &e)
      {
         auto data = new EncodeReqExceptionData();
         data->req = amrq;
         data->err = e;
         SEND_TO_APPLICATION(EncodeReqError, data);
         if (reqout != nullptr)
            delete reqout;
      }
   }
   else
   {
      AppMsgRspPtr amrs = static_cast<AppMsgRspPtr>(am);
      RspOutPtr rspout = nullptr;

      if (am->msgClass() == PFCP::MsgClass::Node)
         Configuration::logger().debug("{} - sending response msgType={} msgClass=NODE seqNbr={}",
            __method__, amrs->msgType(), amrs->seqNbr());
      else if (am->msgClass() == PFCP::MsgClass::Session)
         Configuration::logger().debug("{} - sending response seid={} msgType={} msgClass=SESSION seqNbr={}",
            __method__, static_cast<AppMsgSessionRspPtr>(amrs)->req()->session()->localSeid(), amrs->msgType(), amrs->seqNbr());
      else
         Configuration::logger().debug("{} - sending response msgType={} msgClass=UNKNOWN seqNbr={}",
            __method__, amrs->msgType(), amrs->seqNbr());

      try
      {
         rspout = xlator_.encodeRsp(amrs);
         SEND_TO_COMMUNICATION(SndRsp, rspout);
      }
      catch(SndRspException &e)
      {
         auto data = new SndRspExceptionData();
         data->rsp = amrs;
         data->err = e;
         SEND_TO_APPLICATION(SndReqError, data);
         if (rspout != nullptr)
            delete rspout;
      }
      catch(EncodeRspException &e)
      {
         auto data = new EncodeRspExceptionData();
         data->rsp = amrs;
         data->err = e;
         SEND_TO_APPLICATION(EncodeReqError, data);
         if (rspout != nullptr)
            delete rspout;
      }
   }
}

Void TranslationThread::onRcvdReq(EThreadMessage &msg)
{
   static EString __method__ = __METHOD_NAME__;
   ReqInPtr ri = static_cast<ReqInPtr>(msg.getVoidPtr());
   AppMsgReqPtr req = nullptr;
   RcvdHeartbeatReqDataPtr hb = nullptr;

   // statistics will be handled by the TranslationThread

   try
   {
      if (xlator_.isVersionSupported(ri->version()))
      {
         if (ri->msgType() == Configuration::pfcpHeartbeatReq)
         {
            hb = xlator_.decodeHeartbeatReq(ri);
            // Configuration::logger().debug(
            //    "{} - received heartbeat request local={} remote={} seqNbr={}",
            //    __method__, ri->localNode()->address().getAddress(),
            //    ri->remoteNode()->address().getAddress(), ri->seqNbr());
            SEND_TO_COMMUNICATION(HeartbeatReq, hb);
         }
         else
         {
            if (ri->msgClass() == MsgClass::Session)
            {
               Configuration::logger().debug("{} - received request local={} remote={} msgType={} msgClass=SESSION seid={} seqNbr={}",
                  __method__, ri->localNode()->address().getAddress(), ri->remoteNode()->address().getAddress(),
                  ri->msgType(), (ri)->session()->localSeid(), ri->seqNbr());
            }
            else
            {
               Configuration::logger().debug("{} - received request local={} remote={} msgType={} msgClass={} seqNbr={}",
                  __method__, ri->localNode()->address().getAddress(), ri->remoteNode()->address().getAddress(),
                  ri->msgType(), ri->msgClass()==MsgClass::Node?"NODE":"UNKNOWN", ri->seqNbr());
            }
            req = xlator_.decodeReq(ri);
            if (req == nullptr)
               throw RcvdReqException();
            
            req->postDecode();

            if (req->msgType() == Configuration::pfcpSessionEstablishmentReq)
            {
               if (ri->remoteSeid() == 0)
               {
                  RcvdReqException ex;
                  ex.appendText("control plane FSEID is missing");
                  throw ex;
               }

               static_cast<AppMsgSessionReqPtr>(req)->session()->setRemoteSeid(
                  static_cast<AppMsgSessionReqPtr>(req)->session(), ri->remoteSeid());
            }
            else if (req->msgType() == Configuration::pfcpAssociationSetupReq)
            {
               if (ri->remoteNode()->startTime() == ETime(0))
                  ri->remoteNode()->setStartTime(ri->remoteStartTime());
            }

            SEND_TO_APPLICATION(RcvdReq, req);
         }
      }
      else
      {
         // if the version is not supported, snd version not supported
         RspOutPtr ro = xlator_.encodeVersionNotSupportedRsp(ri);
         SEND_TO_COMMUNICATION(SndRsp, ro);
      }
      delete ri;
   }
   catch (RcvdReqException &e)
   {
      auto data = new RcvdReqExceptionData();
      data->req = ri;
      data->err = e;
      SEND_TO_COMMUNICATION(RcvdReqError, data);
      if (req != nullptr)
         delete req;
      if (hb != nullptr)
         delete hb;
   }
}

Void TranslationThread::onRcvdRsp(EThreadMessage &msg)
{
   static EString __method__ = __METHOD_NAME__;
   RspInPtr ri = static_cast<RspInPtr>(msg.getVoidPtr());
   AppMsgRspPtr rsp = nullptr;
   RcvdHeartbeatRspDataPtr hb = nullptr;

   // statistics will be handled by the TranslationThread
   try
   {
      if (ri->msgType() == Configuration::pfcpHeartbeatRsp)
      {
         Configuration::logger().debug(
            "{} - received heartbeat response local={} remote={} seqNbr={}",
            __method__, ri->localNode()->address().getAddress(),
            ri->remoteNode()->address().getAddress(), ri->seqNbr());
         hb = xlator_.decodeHeartbeatRsp(ri);
         SEND_TO_COMMUNICATION(HeartbeatRsp, hb);
      }
      else
      {
         if (ri->msgClass() == MsgClass::Session)
         {
            Configuration::logger().debug("{} - received response local={} remote={} msgType={} msgClass=SESSION seid={} seqNbr={}",
               __method__, ri->localNode()->address().getAddress(), ri->remoteNode()->address().getAddress(),
               ri->msgType(), static_cast<AppMsgSessionReqPtr>(ri->req())->session()->localSeid(), ri->seqNbr());
         }
         else
         {
            Configuration::logger().debug("{} - received response local={} remote={} msgType={} msgClass={} seqNbr={}",
               __method__, ri->localNode()->address().getAddress(), ri->remoteNode()->address().getAddress(),
               ri->msgType(), ri->msgClass()==MsgClass::Node?"NODE":"SESSION", ri->seqNbr());
         }
         
         rsp = xlator_.decodeRsp(ri);
         if (rsp == nullptr)
            throw RcvdRspException();

         rsp->postDecode();


         if (rsp->msgType() == Configuration::pfcpSessionEstablishmentRsp)
         {
            if (ri->remoteSeid() == 0)
            {
               RcvdRspException ex;
               ex.appendText("user plane FSEID is missing");
               throw ex;
            }

            static_cast<AppMsgSessionReqPtr>(rsp->req())->session()->setRemoteSeid(
               static_cast<AppMsgSessionReqPtr>(rsp->req())->session(), ri->remoteSeid());
         }
         else if (rsp->msgType() == Configuration::pfcpAssociationSetupRsp)
         {
            if (ri->remoteNode()->startTime() == ETime(0))
               ri->remoteNode()->setStartTime(ri->remoteStartTime());
         }

         SEND_TO_APPLICATION(RcvdRsp, rsp);
      }

      delete ri;
   }
   catch (RcvdRspException &e)
   {
      auto data = new RcvdRspExceptionData();
      data->rsp = ri;
      data->err = e;
      SEND_TO_COMMUNICATION(RcvdRspError, data);
      if (rsp != nullptr)
         delete rsp;
      if (hb != nullptr)
         delete hb;
   }
}

Void TranslationThread::onSndHeartbeatReq(EThreadMessage &msg)
{
   static EString __method__ = __METHOD_NAME__;
   SndHeartbeatReqDataPtr req = static_cast<SndHeartbeatReqDataPtr>(msg.getVoidPtr());
   ReqOutPtr reqout = nullptr;
   try
   {
      Configuration::logger().debug("{} - sending heartbeat request to {}",
         __method__, req->remoteNode()->address().getAddress());
      reqout = xlator_.encodeHeartbeatReq(*req);
      SEND_TO_COMMUNICATION(SndReq, reqout);
      delete req;
   }
   catch(SndHeartbeatReqException &e)
   {
      auto data = new SndHeartbeatReqExceptionData();
      data->req = req;
      data->err = e;
      SEND_TO_COMMUNICATION(SndHeartbeatReqError, data);
      if (reqout != nullptr)
         delete reqout;
   }
}

Void TranslationThread::onSndHeartbeatRsp(EThreadMessage &msg)
{
   static EString __method__ = __METHOD_NAME__;
   SndHeartbeatRspDataPtr rsp = static_cast<SndHeartbeatRspDataPtr>(msg.getVoidPtr());
   RspOutPtr rspout = nullptr;
   try
   {
      Configuration::logger().debug("{} - sending heartbeat response to {}",
         __method__, rsp->req().remoteNode()->address().getAddress());
      rspout = xlator_.encodeHeartbeatRsp(*rsp);
      SEND_TO_COMMUNICATION(SndRsp, rspout);
      delete rsp;
   }
   catch(SndHeartbeatRspException &e)
   {
      auto data = new SndHeartbeatRspExceptionData();
      data->rsp = rsp;
      data->err = e;
      SEND_TO_COMMUNICATION(SndHeartbeatRspError, data);
      if (rspout != nullptr)
         delete rspout;
   }
}
/// @endcond

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CommunicationThread, ESocket::ThreadPrivate)
   ON_MESSAGE(static_cast<UInt>(CommunicationThread::Events::SndReq), CommunicationThread::onSndReq)
   ON_MESSAGE(static_cast<UInt>(CommunicationThread::Events::SndRsp), CommunicationThread::onSndRsp)
   ON_MESSAGE(static_cast<UInt>(CommunicationThread::Events::HeartbeatReq), CommunicationThread::onHeartbeatReq)
   ON_MESSAGE(static_cast<UInt>(CommunicationThread::Events::HeartbeatRsp), CommunicationThread::onHeartbeatRsp)
   ON_MESSAGE(static_cast<UInt>(CommunicationThread::Events::SndHeartbeatReqError), CommunicationThread::onSndHeartbeatReqError)
   ON_MESSAGE(static_cast<UInt>(CommunicationThread::Events::SndHeartbeatRspError), CommunicationThread::onSndHeartbeatRspError)
   ON_MESSAGE(static_cast<UInt>(CommunicationThread::Events::RcvdReqError), CommunicationThread::onRcvdReqError)
   ON_MESSAGE(static_cast<UInt>(CommunicationThread::Events::RcvdRspError), CommunicationThread::onRcvdRspError)
   ON_MESSAGE(static_cast<UInt>(CommunicationThread::Events::ReqTimeout), CommunicationThread::onReqTimeout)
   ON_MESSAGE(static_cast<UInt>(CommunicationThread::Events::AddSession), CommunicationThread::onAddSession)
   ON_MESSAGE(static_cast<UInt>(CommunicationThread::Events::DelSession), CommunicationThread::onDelSession)
   ON_MESSAGE(static_cast<UInt>(CommunicationThread::Events::DelNxtRmtSession), CommunicationThread::onDelNxtRmtSession)
END_MESSAGE_MAP()

/// @cond DOXYGEN_EXCLUDE
CommunicationThread::CommunicationThread()
   : trm_(Configuration::teidRangeBits()),
     caw_(0),
     crw_(rwOne_)
{
   static EString __method__ = __METHOD_NAME__;
   this_ = this;
}

CommunicationThread::~CommunicationThread()
{
   static EString __method__ = __METHOD_NAME__;
   this_ = nullptr;
}

Void CommunicationThread::onInit()
{
   static EString __method__ = __METHOD_NAME__;

   ESocket::ThreadPrivate::onInit();

   setNbrActivityWnds(Configuration::nbrActivityWnds());

   // initialize the heartbeat wnd timer
   atmr_.setInterval(Configuration::lenActivityWnd());
   atmr_.setOneShot(False);
   initTimer(atmr_);
   atmr_.start();

   // initialize the rsp wnd timer
   rsptmr_.setInterval(Configuration::maxRspWait());
   rsptmr_.setOneShot(False);
   initTimer(rsptmr_);
   rsptmr_.start();

   Configuration::logger().startup("{} - the communication thread has been started", __method__);
}

Void CommunicationThread::onQuit()
{
   static EString __method__ = __METHOD_NAME__;

   atmr_.stop();

   ESocket::ThreadPrivate::onQuit();
}

Void CommunicationThread::onTimer(EThreadEventTimer *ptimer)
{
   static EString __method__ = __METHOD_NAME__;

   if (ptimer->getId() == atmr_.getId())
   {
      // Configuration::logger().debug("{} - checking LocalNode activity", __method__);
      ERDLock lck(lnslck_);
      for (auto &kv : lns_)
         kv.second->checkActivity(kv.second);
      nextActivityWnd();
   }
   else if (ptimer->getId() == rsptmr_.getId())
   {
      crw_ ^= rwToggle_;
      ERDLock lck(lnslck_);
      for (auto &kv : lns_)
         kv.second->removeOldReqs(crw_);
   }
}

Void CommunicationThread::errorHandler(EError &err, ESocket::BasePrivate *psocket)
{
   static EString __method__ = __METHOD_NAME__;

   switch (psocket->getSocketType())
   {
      case ESocket::SocketType::Udp:
      {
         ESocket::UdpPrivate *s = static_cast<ESocket::UdpPrivate*>(psocket);
         Configuration::logger().major("CommunicationThread socket exception for [{} : {}] error - {}",
            s->getLocalAddress(), s->getLocalPort(), err.what());
         break;
      }
      default:
      {
         Configuration::logger().major("CommunicationThread socket exception - unexpected socket type ({}) - {}",
            psocket->getSocketType(), err.what());
      }
   }
}

LocalNodeSPtr CommunicationThread::createLocalNode(ESocket::Address &addr, Bool start)
{
   static EString __method__ = __METHOD_NAME__;

   LocalNodeSPtr ln = Configuration::baseApplication()._createLocalNode();

   ln->setAddress(addr);
   ln->setStartTime();

   ln->socket().setLocalNode(ln);
   ln->socket().bind(addr);

   if (start)
      startLocalNode(ln);

   return ln;
}

Void CommunicationThread::startLocalNode(LocalNodeSPtr &ln)
{
   EWRLock lck(lnslck_);
   auto it = lns_.find(ln->ipAddress());
   if (it == lns_.end())
      lns_.insert(std::make_pair(ln->ipAddress(),ln));
   ln->changeState(ln, LocalNode::State::Started);
}

Void CommunicationThread::stopLocalNode(LocalNodeSPtr &ln)
{
   ln->changeState(ln, LocalNode::State::Stopped);
}

Void CommunicationThread::releaseLocalNodes()
{
   static EString __method__ = __METHOD_NAME__;
   EWRLock lck(lnslck_);
   auto entry = lns_.begin();
   while (entry != lns_.end())
   {
      stopLocalNode(entry->second);
      entry->second->clearRqstOutEntries();
      entry->second->socket().disconnect();
      entry->second->socket().clearLocalNode();
      Configuration::logger().info("{} - localNode={} use_count={}", __method__,
         entry->second->address().getAddress(), entry->second.use_count());
      entry = lns_.erase(entry);
   }
}

Void CommunicationThread::setNbrActivityWnds(size_t nbr)
{
   static EString __method__ = __METHOD_NAME__;

   caw_ = 0;

   ERDLock lck(lnslck_);
   for (auto &kv : lns_)
      kv.second->setNbrActivityWnds(nbr);
}

Void CommunicationThread::nextActivityWnd()
{
   static EString __method__ = __METHOD_NAME__;

   caw_++;
   if (caw_ >= Configuration::nbrActivityWnds())
      caw_ = 0;
   
   ERDLock lck(lnslck_);
   for (auto &kv : lns_)
      kv.second->nextActivityWnd(caw_);
}

Void CommunicationThread::onSndReq(EThreadMessage &msg)
{
   static EString __method__ = __METHOD_NAME__;

   ReqOutPtr ro = static_cast<ReqOutPtr>(msg.getVoidPtr());
   ro->localNode()->sndInitialReq(ro);
}

Void CommunicationThread::onSndRsp(EThreadMessage &msg)
{
   static EString __method__ = __METHOD_NAME__;

   RspOutPtr ro = static_cast<RspOutPtr>(msg.getVoidPtr());
   ro->localNode()->sndRsp(ro);
}

Void CommunicationThread::onHeartbeatReq(EThreadMessage &msg)
{
   static EString __method__ = __METHOD_NAME__;
   RcvdHeartbeatReqDataPtr hbrq = static_cast<RcvdHeartbeatReqDataPtr>(msg.getVoidPtr());

   Configuration::logger().debug("{} - RcvdHeartbeatReqData seqNbr={} ",
      __method__, hbrq->req()->seqNbr());
   
   // if remote has restarted, snd notification to application thread
   if (hbrq->req()->remoteNode()->startTime() != hbrq->startTime())
      hbrq->req()->remoteNode()->restarted(hbrq->req()->remoteNode(), hbrq->startTime());

   // snd the rsp
   SndHeartbeatRspDataPtr hbrs = new SndHeartbeatRspData(hbrq->req());
   SEND_TO_TRANSLATION(SndHeartbeatRsp, hbrs);

   // delete the heartbeat req object
   delete hbrq;
}

Void CommunicationThread::onHeartbeatRsp(EThreadMessage &msg)
{
   static EString __method__ = __METHOD_NAME__;
   RcvdHeartbeatRspDataPtr hbrs = static_cast<RcvdHeartbeatRspDataPtr>(msg.getVoidPtr());

   Configuration::logger().debug("{} - RcvdHeartbeatRspData seqNbr={} ",
      __method__, hbrs->req().seqNbr());

   // if remote has restarted, snd notification to application thread
   if (hbrs->req().remoteNode()->startTime() != hbrs->startTime())
      hbrs->req().remoteNode()->restarted(
         hbrs->req().remoteNode(), hbrs->startTime());

   // delete the heartbeat rsp object
   delete hbrs;   
}

Void CommunicationThread::onSndHeartbeatReqError(EThreadMessage &msg)
{
   static EString __method__ = __METHOD_NAME__;
   SndHeartbeatReqExceptionDataPtr hbrq = static_cast<SndHeartbeatReqExceptionDataPtr>(msg.getVoidPtr());

   if (hbrq != nullptr)
   {
      Configuration::logger().major(
         "{} - unable to construct heartbeat request message - {}",
         __method__, hbrq->err.what());

      if (hbrq->req)
         delete hbrq->req;
      delete hbrq;
   }
   else
   {
      Configuration::logger().major(
         "{} - SndHeartbeatReqExceptionDataPtr is null", __method__
      );
   }
}

Void CommunicationThread::onSndHeartbeatRspError(EThreadMessage &msg)
{
   static EString __method__ = __METHOD_NAME__;
   SndHeartbeatRspExceptionDataPtr hbrs = static_cast<SndHeartbeatRspExceptionDataPtr>(msg.getVoidPtr());

   if (hbrs != nullptr)
   {
      Configuration::logger().major(
         "{} - unable to construct heartbeat response message - {}",
         __method__, hbrs->err.what());
      if (hbrs->rsp != nullptr)
         delete hbrs->rsp;
      delete hbrs;
   }
   else
   {
      Configuration::logger().major(
         "{} - SndHeartbeatRspExceptionDataPtr is null", __method__
      );
   }
}

Void CommunicationThread::onRcvdReqError(EThreadMessage &msg)
{
   static EString __method__ = __METHOD_NAME__;
   RcvdReqExceptionDataPtr data = static_cast<RcvdReqExceptionDataPtr>(msg.getVoidPtr());

   if (data != nullptr)
   {
      if (data->req != nullptr)
      {
         // remove the RcvdReq from the collection
         data->req->remoteNode()->delRcvdReq(data->req->seqNbr());

         // log the error
         if (data->req->msgClass() == MsgClass::Session)
         {
            Configuration::logger().major(
               "{} - unable to decode request message - {} - "
               " discarding req local={} remote={} seid={} msgType={} msgClass=SESSION seqNbr={} version={} msgLen={}",
               __method__, data->err.what(), data->req->localNode()->ipAddress().address(),
               data->req->remoteNode()->ipAddress().address(), data->req->session()->localSeid(),
               data->req->msgType(), data->req->seqNbr(), data->req->version(), data->req->len());
         }
         else
         {
            Configuration::logger().major(
               "{} - unable to decode request message - {} - "
               " discarding req local={} remote={} msgType={} msgClass={} seqNbr={} version={} msgLen={}",
               __method__, data->err.what(), data->req->localNode()->ipAddress().address(),
               data->req->remoteNode()->ipAddress().address(), data->req->msgType(),
               data->req->msgClass()==MsgClass::Node?"NODE":"UNKNOWN", data->req->seqNbr(),
               data->req->version(), data->req->len());
         }

         // delete ReqInPtr
         delete data->req;
      }
      else
      {
         Configuration::logger().major(
            "{}} - RcvdReqExceptionDataPtr ReqInPtr is null - {}",
            __method__, data->err.what());
      }
      delete data;
   }
   else
   {
      Configuration::logger().major(
         "{} - RcvdReqExceptionDataPtr is null", __method__
      );
   }
}

Void CommunicationThread::onRcvdRspError(EThreadMessage &msg)
{
   static EString __method__ = __METHOD_NAME__;
   RcvdRspExceptionDataPtr data = static_cast<RcvdRspExceptionDataPtr>(msg.getVoidPtr());

   if (data != nullptr)
   {
      if (data->rsp != nullptr)
      {
         // clear the response window for the request that this response is associated with
         data->rsp->localNode()->setRqstOutRespWnd(data->rsp->seqNbr(), 0);

         // log the error
         if (data->rsp->msgClass() == MsgClass::Session)
         {
            Configuration::logger().major(
               "{} - unable to decode response message - {} - "
               " discarding rsp src={} dst={} seid={} msgType={} msgClass=SESSION seqNbr={} version={} msgLen={}",
               __method__, data->err.what(),
               data->rsp->localNode()->ipAddress().address(), data->rsp->remoteNode()->ipAddress().address(),
               static_cast<AppMsgSessionReqPtr>(data->rsp->req())->session()->localSeid(), data->rsp->msgType(),
               data->rsp->seqNbr(), data->rsp->version(), data->rsp->len());
         }
         else
         {
            Configuration::logger().major(
               "{} - unable to decode response message - {} - "
               " discarding rsp src={} dst={} msgType={} msgClass={} seqNbr={} version={} msgLen={}",
               __method__, data->err.what(), data->rsp->localNode()->ipAddress().address(),
               data->rsp->remoteNode()->ipAddress().address(), data->rsp->msgType(),
               data->rsp->msgClass()==MsgClass::Node?"NODE":"UNKNOWN", data->rsp->seqNbr(),
               data->rsp->version(), data->rsp->len());
         }

         // delete RspInPtr
         delete data->rsp;
      }
      else
      {
         Configuration::logger().major(
            "{} - RcvdRspExceptionDataPtr RspInPtr is null - {}",
            __method__, data->err.what());
      }
      delete data;
   }
   else
   {
      Configuration::logger().major(
         "{} - RcvdRspExceptionDataPtr is null", __method__
      );
   }
}

Void CommunicationThread::onReqTimeout(EThreadMessage &msg)
{
   static EString __method__ = __METHOD_NAME__;
   ReqOutPtr ro = static_cast<ReqOutPtr>(msg.getVoidPtr());
   if (ro && !ro->localNode()->onReqOutTimeout(ro))
      delete ro;
}

Void CommunicationThread::onAddSession(EThreadMessage &msg)
{
   static EString __method__ = __METHOD_NAME__;
   SessionBaseSPtr *s = reinterpret_cast<SessionBaseSPtr*>(msg.getVoidPtr());

   if (s == nullptr || !*s)
      Configuration::logger().minor("{} - SessionBaseSPtr is not valid", __method__);
   else
      addSession(*s);

   delete s;
}

Void CommunicationThread::onDelSession(EThreadMessage &msg)
{
   static EString __method__ = __METHOD_NAME__;
   SessionBaseSPtr *s = reinterpret_cast<SessionBaseSPtr*>(msg.getVoidPtr());
   if (s == nullptr || !*s)
      Configuration::logger().minor("{} - SessionBaseSPtr is not valid", __method__);
   else
      delSession(*s);

   delete s;
}

Void CommunicationThread::onDelNxtRmtSession(EThreadMessage &msg)
{
   static EString __method__ = __METHOD_NAME__;
   RemoteNodeSPtr *rn = reinterpret_cast<RemoteNodeSPtr*>(msg.getVoidPtr());
   if (rn)
   {
      SessionBaseSPtr s = (*rn)->getFirstSession();
      if (s)
      {
         delSession(s);
         SEND_TO_COMMUNICATION(DelNxtRmtSession, rn);
      }
      else
      {
         if ((*rn)->state() == RemoteNode::State::Stopping)
            (*rn)->changeState(*rn, RemoteNode::State::Stopped);
         // the remote node should be deleted after the final statistics have been reported.
         delete rn;
      }
   }
}

Void CommunicationThread::addSession(SessionBaseSPtr &s)
{
   static EString __method__ = __METHOD_NAME__;

   if (s->localSeid() != 0)
   {
      if (s->localNode())
         s->localNode()->addSession(s);
      else
         Configuration::logger().minor("{} - the LocalNode associated with the session is invalid", __method__);
   }

   if (s->remoteSeid() != 0)
   {
      if (s->remoteNode())
         s->remoteNode()->addSession(s);
      else
         Configuration::logger().minor("{} - the RemoteNode associated with the session is invalid", __method__);
   }
}

Void CommunicationThread::delSession(SessionBaseSPtr &s)
{
   static EString __method__ = __METHOD_NAME__;
   if (s->localNode())
      s->localNode()->delSession(s);
   if (s->remoteNode())
      s->remoteNode()->delSession(s);
}

/// @endcond

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

} // namespace PFCP