/*
* Copyright (c) 2009-2019 Brian Waters
* Copyright (c) 2019 Sprint
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

#ifndef __eqbase_h_included
#define __eqbase_h_included

#include "ebase.h"
#include "eerror.h"
#include "esynch.h"
#include "etime.h"
#include "etimer.h"
//#include "etq.h"
#include "emsg.h"

/// @file
/// @brief Provides base class support for sending and receiving messages via a
///        message queue.
/// @details This custom FIFO message queue implementation stores the data in a buffer
///    either allocated from the heap or from shared memory.  The private queue,
///    allocated from the heap, can be accessed by any thread in the same
///    process, while the public queue, allocated from shared memory, can be
///    accessed from any process running on the same machine.  The EMessage class
///    serializes/deserializes the data to/from the queue.

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/// @cond DOXYGEN_EXCLUDE
DECLARE_ERROR(EQueueBaseError_UnInitialized);
DECLARE_ERROR(EQueueBaseError_NotOpenForWriting);
DECLARE_ERROR(EQueueBaseError_NotOpenForReading);
DECLARE_ERROR(EQueueBaseError_MultipleReadersNotAllowed);
/// @endcond

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class EQueueBase;

/// @brief Represents a message to be written to/read from a message queue.
class EQueueMessage : public EMessage
{
   friend class EQueueBase;

public:
   /// @brief Default constructor.
   EQueueMessage()
   {
   }

   /// @brief Class destructor.
   ~EQueueMessage()
   {
   }

   /// @brief Assignment operator.
   /// @param a the EQueueMessage to copy.
   /// @return a reference to this object.
   EQueueMessage &operator=(const EQueueMessage &a)
   {
      m_timer = a.m_timer;
      m_msgType = a.m_msgType;
      return *this;
   }

   /// @brief Calculates the length of the message.
   /// @param length the variable to update with the length.
   virtual Void getLength(ULong &length);
   /// @brief Serializes the message.
   /// @param pBuffer a pointer to the serialized message data.
   /// @param nOffset a reference that will update the offset where to next
   ///    write data.
   virtual Void serialize(pVoid pBuffer, ULong &nOffset);
   /// @brief Deserializes the message.
   /// @param pBuffer a pointer to the serialized message data.
   /// @param nOffset a reference that will update the offset where to next
   ///    read data.
   virtual Void unserialize(pVoid pBuffer, ULong &nOffset);

   /// @brief Retrieves the message lifetime timer.
   /// @return a reference to the message lifetime timer.
   ETime &getTimer()
   {
      return m_timer;
   }

   /// @brief Assigns the message type value.
   /// @param msgType the message type value.
   Void setMsgType(Long msgType) { m_msgType = msgType; }
   /// @brief Retrieves the message type value.
   /// @return the message type value.
   Long getMsgType() { return m_msgType; }

private:
   ETime m_timer;
   Long m_msgType;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class EQueuePublic;
class EQueuePrivate;

/// @brief The message queue base class.
class EQueueBase
{
   friend class EQueuePublic;
   friend class EQueuePrivate;

public:
   /// @brief The queue access modes.
   enum Mode
   {
      /// provivdes read only access
      ReadOnly,
      /// provides write only access
      WriteOnly,
      /// provides read/write access
      ReadWrite
   };

   /// @brief Writes a message to the queue.
   /// @param msg the message to be written.
   /// @param wait indicated whether to wait for space to become available
   ///    in the queue before writing the data.
   /// @return True indicates that the message was successfully written, otherwise
   ///    there was insufficient space to write the message.
   Bool push(EQueueMessage &msg, Bool wait = True);
   /// @brief Retrieves the next message from the queue.
   /// @param wait indicates whether to wait for a message or to return immediately.
   /// @return a pointer to the successfully retrieved message otherwise NULL.
   EQueueMessage *pop(Bool wait = True);

   /// @brief Destroys the message queue.
   Void destroy();

protected:
   /// @cond DOXYGEN_EXCLUDE
   virtual Bool isPublic() = 0;
   virtual ULong &msgSize() = 0;
   virtual Int &msgCnt() = 0;
   virtual Long &msgHead() = 0;
   virtual Long &msgTail() = 0;
   virtual Bool &multipleReaders() = 0;
   virtual Bool &multipleWriters() = 0;
   virtual Int &numReaders() = 0;
   virtual Int &numWriters() = 0;
   virtual Int &refCnt() = 0;
   virtual pChar data() = 0;
   virtual Void allocDataSpace(cpStr sFile, Char cId, Int nSize) = 0;
   virtual Void initReadMutex() = 0;
   virtual Void initWriteMutex() = 0;
   virtual Void initSemFree(UInt initialCount) = 0;
   virtual Void initSemMsgs(UInt initialCount) = 0;

   virtual EMutexData &readMutex() = 0;
   virtual EMutexData &writeMutex() = 0;
   virtual ESemaphoreData &semMsgs() = 0;
   virtual ESemaphoreData &semFree() = 0;

   virtual EQueueMessage *allocMessage(Long msgType) = 0;

   Mode mode() { return m_mode; }

   EQueueBase();
   ~EQueueBase();

   Void init(ULong nMsgSize, Int nMsgCnt, Int queueId, Bool bMultipleReaders,
             Bool bMultipleWriters, EQueueBase::Mode eMode);
   /// @endcond

private:
   Bool m_initialized;
   Mode m_mode;
   Char m_rBuffer[USHRT_MAX];
   Char m_wBuffer[USHRT_MAX];
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#if 0
class PointerMessage
{
   friend class PointerQueue;
public:
   virtual PointerMessage &operator=(const PointerMessage &msg) { tmr_=msg.tmr_; ptr_=msg.ptr_; return *this; }
   epctime_t queueMicroSeconds() { return tmr_.MicroSeconds(); }
   epctime_t queueMilliSeconds() { return tmr_.MilliSeconds(); }
   pVoid pointer() { return ptr_; }
   PointerMessage &pointer(pVoid p) { ptr_=p; return *this; }
   Void start() { tmr_.Start(); }
   Void stop() { tmr_.Stop(); }
private:
   ETimer tmr_;
   pVoid ptr_;
};

class PointerQueue
{
public:
   PointerQueue();
   ~PointerQueue();

   size_t maxPages() const    { return mp_; }
   size_t messageSize() const { return ms_; }
   UShort pageSize() const    { return ps_; }

   PointerQueue &init(size_t ms, UShort ps, size_t mp_);

   Bool push(pVoid ptr, Bool wait=True);
   pVoid pop(Bool wait=True);

private:
   struct Page
   {
      Page *prevPage;
      Page *nextPage;
      pUChar data;
   };

   class PageAddress
   {
   public:
      PageAddress() : pg_(nullptr), ofs_(0) {}
      PageAddress(Page &page, UShort offset) : pg_(&page), ofs_(offset) {}
      PageAddress(const PageAddress &addr) : pg_(addr.pg_), ofs_(addr.ofs_) {}

      PageAddress &operator=(const PageAddress &addr) { pg_=addr.pg_; ofs_=addr.ofs_; return *this; }
      Bool operator==(const PageAddress &addr) { return pg_==addr.pg_ && ofs_==addr.ofs_; }

      Page &page()                     { return *pg_; }
      PageAddress &page(Page &p)       { pg_=&p; return *this; }
      PageAddress &page(Page *p)       { pg_=p; return *this; }
      UShort offset() const            { return ofs_; }
      PageAddress &offset(UShort ofs)  { ofs_=ofs; return *this; }

      Page &nextPage() { return *pg_->nextPage; }
      Page &prevPage() { return *pg_->prevPage; }

   private:
      Page *pg_;
      UShort ofs_;
   };

   std::atomic_int rc_; // reference count
   std::atomic_int nr_; // number of readers
   std::atomic_int nw_; // number of writers

   size_t mp_; // max pages
   size_t ms_; // message size
   UShort ps_; // page size (entries per page)
   Page *head_; // write to the head
   Page *tail_; // read from the tail

   EMutexPrivate rmtx_;
   EMutexPrivate wmtx_;
   ESemaphorePrivate free_;
   ESemaphorePrivate used_;

   // write to the head, read from the tail
   // if no more space in the current page and the next page contains the tail, create a new page
};
#endif

#endif // #define __eqbase_h_included
