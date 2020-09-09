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

#ifndef __EMEMORY_H
#define __EMEMORY_H

/// @file
/// @brief Contains the class definitions to support the pool based memory allocation.

#include "eerror.h"
#include "esynch.h"
#include "etime.h"

DECLARE_ERROR(EMemory_NodeSizeTooSmall);

class EMemory
{
public:
   class Pool;

   class Block
   {
      friend Pool;
   public:
      Block()
         : next_(nullptr)
#ifdef EMEMORY_STATISTICS
           , cnt_(0)
#endif
      {
      }

      Block *next()     { return next_; }
      pUChar data()     { return data_; }

#ifdef EMEMORY_STATISTICS
      const ETime &lastAllocated() const  { return last_; }
      size_t ccount() const               { return cnt_; }
   protected:
      Void allocate()
      {
         last_ = ETime::Now();
         cnt_++;
      }
#endif

   protected:
      Block &next(Block *nxt)          { next_ = nxt; return *this; }

   private:
      Block *next_;
#ifdef EMEMORY_STATISTICS
      ETime last_; // the last time the block was allocated
      size_t cnt_; // the number of times this block has been allocated
#endif
      UChar data_[0];
   };

   class Node
   {
      friend Pool;
   public:
      Node(Pool &pool) : pool_(pool), next_(nullptr) {}

      Pool &pool()            { return pool_; }
      Node *next()            { return next_; }
      pUChar data()           { return data_; }
   
   protected:
      Node &next(Node *nxt)   { next_ = nxt; return *this; }

   private:
      Node();
      Pool &pool_;
      Node *next_;
      UChar data_[0];
   };

   class Pool
   {
   public:
      Pool(size_t allocSize=0, size_t nodeSize=0, size_t blockCount=0)
         : head_(nullptr),
           free_(nullptr),
           as_(allocSize),
           ns_(nodeSize),
           bs_(0),
           bc_(blockCount)
      {
         setSize();
      }
      ~Pool()
      {
         while (head_)
         {
            Node *nxt = head_->next();
            delete [] reinterpret_cast<pUChar>(head_);
            head_ = nxt;
         }
      }

      size_t allocSize() const            { return as_; }
      size_t blockSize() const            { return bs_; }
      size_t blockCount() const           { return bc_; }
      size_t nodeSize() const             { return ns_; }

      Pool &setSize(size_t as, size_t ns, size_t bc=0)
      {
         as_ = as;
         ns_ = ns;
         bc_ = bc;
         setSize();
         return *this;
      }

      pVoid allocate()
      {
         EMutexLock l(mutex_);
         if (!free_)
         {
            pUChar mem = new UChar[ns_];
            Node *n = new (mem) Node(*this);
            pUChar data = n->data();
            for (size_t i=0; i<bc_; i++)
            {
               Block *blk = new (data) Block();
               blk->next(free_);
               free_ = blk;
               data += bs_;
            }
         }
         Block *blk = free_;
         free_ = blk->next();
#ifdef EMEMORY_STATISTICS
         blk->allocate();
#endif
         return blk->data();
      }

      Void deallocate(pVoid data)
      {
         EMutexLock l(mutex_);
         Block *blk = reinterpret_cast<Block*>(reinterpret_cast<pUChar>(data) - sizeof(Block));
         blk->next(free_);
         free_ = blk;
      }

   private:
      Void setSize()
      {
         if (as_ == 0)
            return;

         if (ns_ == 0)
         {
            bs_ = sizeof(Block) + as_;
            bs_ += bs_ % sizeof(pVoid); 
            if (bc_ == 0)
               bc_ = 1;
            ns_ = sizeof(Node) + bs_ * bc_;
         }
         else
         {
            bs_ = sizeof(Block) + as_;
            bs_ += bs_ % sizeof(pVoid); 
            bc_ = (ns_ - sizeof(Node)) / bs_;
            if (bc_ == 0)
               throw EMemory_NodeSizeTooSmall();
         }
      }

      EMutexPrivate mutex_;
      Node *head_;
      Block *free_;
      size_t as_;
      size_t ns_;
      size_t bs_;
      size_t bc_;
   };
};

#endif // #ifndef __EMEMORY_H
