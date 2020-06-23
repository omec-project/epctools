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

#ifndef __ETEID_H
#define __ETEID_H

#include <atomic>
#include <limits>

#include "eerror.h"

DECLARE_ERROR_ADVANCED(ETeidManager_InvalidNumberOfRangeBits);

inline ETeidManager_InvalidNumberOfRangeBits::ETeidManager_InvalidNumberOfRangeBits(){
   setText("The range bits must be between 0 and 7");
}

class ETeidManager_InvalidRangeValue : public EError {
public:
   ETeidManager_InvalidRangeValue(UChar min, UChar max) {
      setTextf("The TEID range value must be between %u and %u", min, max);
   }
private:
   ETeidManager_InvalidRangeValue();
};

class ETeidManager
{
public:

   ETeidManager(Int rangeBits=0, Int rangeValue=0)
      : bits_(rangeBits),
        range_(rangeValue)
   {
      if (bits_ < 0 || bits_ > 7)
         throw ETeidManager_InvalidNumberOfRangeBits();
      
      Int minRange = 0;
      Int maxRange = (bits_ == 0) ? 0 : ((1 << (bits_)) - 1);

      if (rangeValue > maxRange)
         throw ETeidManager_InvalidRangeValue(minRange,maxRange);

      min_ = static_cast<ULongLong>(rangeValue) << ((sizeof(ULong)*8) - bits_);
      max_ = ((1ull << (32 - bits_)) - 1) | min_;
      if (min_ == 0)
         min_++;
      
      next_ = min_;
   }

   Int rangeBits() const { return bits_; }
   Int rangeValue() const { return range_; }
   ULong min() const { return min_; }
   ULong max() const { return max_; }

   ULong alloc()
   {
      // unsigned long long nxt=0;
      unsigned long long nxt;
      while ((nxt=next_.fetch_add(1)) > max_)
         next_.compare_exchange_strong(++nxt, min_);
      return static_cast<ULong>(nxt);
   }

private:
   std::atomic_ullong next_;
   Int bits_;
   Int range_;
   ULongLong min_;
   ULongLong max_;
};

#endif // __ETEID_H