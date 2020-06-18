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

#ifndef __EOSTRING_H
#define __EOSTRING_H

#include <string>
#include <iostream>
#include <vector>

#include "eerror.h"
#include "ehash.h"

DECLARE_ERROR(EOctetString_OutOfRange);

class EOctetString
{
public:
   typedef size_t size_type;
   static const size_type npos { std::string::npos };

   EOctetString()
       : capacity_(0),
         length_(0),
         data_(NULL)
   {
      reserve(minCapacity_);
   }
   EOctetString(size_type n)
      : capacity_(0),
        length_(0),
        data_(NULL)
   {
      reserve(n);
   }
   EOctetString(cpStr s)
      : capacity_(0),
        length_(0),
        data_(NULL)
   {
      size_type n = strlen(s);
      reserve(n);
      assign(s);
   }
   EOctetString(cpUChar data, size_type n)
      : capacity_(0),
        length_(0),
        data_(NULL)
   {
      reserve(n);
      assign(data, n);
   }
   EOctetString(const EOctetString &ostr)
       : capacity_(0),
         length_(0),
         data_(NULL)
   {
      *this = ostr;
   }
   EOctetString(const EOctetString &ostr, size_type pos, size_type len = npos)
       : capacity_(0),
         length_(0),
         data_(NULL)
   {
      assign(ostr, pos, len);
   }

   ~EOctetString()
   {
      dealloc();
   }

   EOctetString &operator=(const EOctetString &ostr)  { return assign(ostr); }
   EOctetString &operator=(cpStr s)                   { return assign(s);    }
   EOctetString &operator=(Char c)                    { return assign(1, c); }
   EOctetString &operator=(UChar c)                   { return assign(1, c); }
   EOctetString &operator=(EOctetString &&ostr)       { return assign(std::move(ostr)); }

   EOctetString &operator+=(const EOctetString &ostr) { return append(ostr); }
   EOctetString &operator+=(cpStr s)                  { return append(s);    }
   EOctetString &operator+=(Char c)                   { return append((UChar)c); }
   EOctetString &operator+=(UChar c)                  { return append(c); }

   UChar &operator[](size_type pos)                   { if (pos >= length_) throw EOctetString_OutOfRange(); return data_[pos]; }
   cUChar &operator[](size_type pos) const            { if (pos >= length_) throw EOctetString_OutOfRange(); return data_[pos]; }

   bool operator==(const EOctetString &ostr) const { return compare(ostr) == 0; }
   bool operator!=(const EOctetString &ostr) const { return compare(ostr) != 0; }
   bool operator<(const EOctetString &ostr)  const { return compare(ostr) < 0; }
   bool operator>(const EOctetString &ostr)  const { return compare(ostr) > 0; }
   bool operator<=(const EOctetString &ostr) const { return !(compare(ostr) > 0); }
   bool operator>=(const EOctetString &ostr) const { return !(compare(ostr) < 0); }

   bool operator==(cpStr str) const { return compare(str) == 0; }
   bool operator!=(cpStr str) const { return compare(str) != 0; }
   bool operator<(cpStr str)  const { return compare(str) < 0; }
   bool operator>(cpStr str)  const { return compare(str) > 0; }
   bool operator<=(cpStr str) const { return !(compare(str) > 0); }
   bool operator>=(cpStr str) const { return !(compare(str) < 0); }

   EOctetString &append(const EOctetString &ostr)
   {
      return append(ostr.data_, ostr.length_);
   }

   EOctetString &append(const EOctetString &ostr, size_type subpos, size_type sublen)
   {
      if (subpos >= ostr.length_)
         throw EOctetString_OutOfRange();

      return append(&ostr.data_[subpos], sublen);
   }

   EOctetString &append(cpStr s)
   {
      return append(reinterpret_cast<cpUChar>(s), strlen(s));
   }

   EOctetString &append(cpStr s, size_type n)
   {
      return append(reinterpret_cast<cpUChar>(s), n);
   }

   EOctetString &append(cUChar c)
   {
      if (length_ + 1 > capacity_)
      {
         size_type newCapacity = capacity_;
         while (newCapacity < length_ + 1)
            newCapacity *= 2;
         reserve(newCapacity);
      }
      data_[length_++] = c;
      return *this;
   }

   EOctetString &append(cpUChar data, size_type n)
   {
      if (length_ + n > capacity_)
      {
         size_type newCapacity = capacity_;
         while (newCapacity < length_ + n)
            newCapacity *= 2;
         reserve(newCapacity);
      }
      memcpy(&data_[length_], data, n);
      length_ += n;
      return *this;
   }

   EOctetString &append(size_type n, UChar c)
   {
      if (length_ + n > capacity_)
      {
         size_type newCapacity = capacity_;
         while (newCapacity < length_ + n)
            newCapacity *= 2;
         reserve(newCapacity);
      }
      memset(&data_[length_], c, n);
      length_ += n;
      return *this;
   }

   EOctetString &assign(const EOctetString &ostr)
   {
      if (capacity_ < ostr.capacity_)
         alloc(ostr.capacity_);
      length_ = ostr.length_;
      memcpy(data_, ostr.data_, length_);
      return *this;
   }
   EOctetString &assign(const EOctetString &ostr, size_type subpos, size_type sublen)
   {
      if (subpos >= ostr.length_)
         throw EOctetString_OutOfRange();

      size_type len = std::min(ostr.length_ - subpos, sublen);
      if (capacity_ < len)
         alloc(len);
      length_ = len;
      memcpy(data_, &ostr.data_[subpos], length_);
      return *this;
   }
   EOctetString &assign(cpStr s)
   {
      size_type len = strlen(s);
      if (capacity_ < len)
         alloc(len);
      length_ = len;
      memcpy(data_, s, length_);
      return *this;
   }
   EOctetString &assign(cpChar s, size_type n)
   {
      return assign(reinterpret_cast<cpUChar>(s), n);
   }
   EOctetString &assign(cpUChar data, size_type n)
   {
      if (capacity_ < n)
         alloc(n);
      length_ = n;
      memcpy(data_, data, length_);
      return *this;
   }
   EOctetString &assign(size_type n, UChar c)
   {
      if (capacity_ < n)
         alloc(n);
      length_ = n;
      memset(data_, c, length_);
      return *this;
   }
   EOctetString &assign(EOctetString &&ostr)
   {
      dealloc();

      capacity_ = ostr.capacity_;
      length_ = ostr.length_;
      data_ = ostr.data_;

      ostr.capacity_ = 0;
      ostr.length_ = 0;
      ostr.data_ = nullptr;

      return *this;
   }

   UChar &at(size_type pos)
   {
      if (pos > length_)
         throw EOctetString_OutOfRange();
      return data_[pos];
   }

   cUChar &at(size_type pos) const
   {
      if (pos > length_)
         throw EOctetString_OutOfRange();
      return data_[pos];
   }

   UChar &back()
   {
      return data_[length_ - 1];
   }

   cUChar &back() const
   {
      return data_[length_ - 1];
   }

   size_type capacity() const { return capacity_; }

   Void clear()
   {
      memset(data_, 0, length_);
      length_ = 0;
   }

   Int compare(const EOctetString &ostr) const
   {
      return compare(0, length_, ostr.data_, ostr.length_);
   }

   Int compare(size_type pos, size_type len, const EOctetString &ostr) const
   {
      return compare(pos, len, ostr.data_, ostr.length_);
   }

   Int compare(size_type pos, size_type len, const EOctetString &ostr, size_type subpos, size_type sublen) const
   {
      if (subpos >= ostr.length_)
         throw EOctetString_OutOfRange();
      return compare(pos, len, &ostr.data_[subpos], std::min(ostr.length_ - subpos, sublen));
   }

   Int compare(cpStr s) const
   {
      return compare(0, length_, reinterpret_cast<cpUChar>(s), strlen(s));
   }

   Int compare(cpUChar data, size_type n) const
   {
      return compare(0, length_, data, n);
   }

   Int compare(size_type pos, size_type len, cpStr s) const
   {
      return compare(pos, len, reinterpret_cast<cpUChar>(s), strlen(s));
   }

   Int compare(size_type pos, size_type len, cpStr s, size_type n) const
   {
      return compare(pos, len, reinterpret_cast<cpUChar>(s), n);
   }

   Int compare(size_type pos, size_type len, cpUChar data, size_type n) const
   {
      if (pos >= length_)
         throw EOctetString_OutOfRange();

      len = std::min(len, length_ - pos);

      Int result = memcmp(&data_[pos], data, len);

      if (!result)
         return len < length_ ? -1 : len > length_ ? 1 : 0;
      return result;
   }

   size_type copy(pUChar dst, size_type len, size_type pos = 0) const
   {
      if (pos >= length_)
         throw EOctetString_OutOfRange();
      len = std::min(length_ - pos, len);
      memcpy(dst, &data_[pos], len);
      return len;
   }

   cpUChar data() const { return data_; }
   Bool empty() const { return length_ == 0; }

   EOctetString &erase(size_type pos = 0, size_type len = npos)
   {
      if (pos >= length_)
         throw EOctetString_OutOfRange();
      len = std::min(length_ - pos, len);
      memset(&data_[pos], 0, len);
      length_ -= len;
      return *this;
   }

   size_type length() const { return length_; }

   Void pop_back()
   {
      if (length_ > 0)
      {
         length_--;
         data_[length_] = 0;
      }
   }

   Void push_back(Char c)  { append(1, static_cast<UChar>(c)); }
   Void push_back(UChar c) { append(1, c); }

   Void reserve(size_type n = 0)
   {
      n = std::max(n, length_);

      if (capacity_ == n)
         return;

      pUChar oldData = data_;
      data_ = 0;

      alloc(n);

      if (oldData)
      {
         memcpy(data_, oldData, length_);
         delete [] oldData;
      }
   }

   Void resize(size_type n, Char c)
   {
      resize(n, static_cast<UChar>(c));
   }

   Void resize(size_type n, UChar c = 0)
   {
      if (n > length_)
         append(n - length_, c);
      else if (n < length_)
      {
         memset(&data_[n], c, length_ - n);
         length_ = n;
      }
   }

   void shrink_to_fit()
   {
      reserve();
   }

   size_type size() const { return length_; }

   friend std::ostream &operator<<(std::ostream &output, const EOctetString &ostr);

private:
   static const size_type minCapacity_ { 15 };

   Void dealloc()
   {
      capacity_ = 0;
      length_ = 0;
      if (data_)
      {
         delete[] data_;
         data_ = NULL;
      }
   }
   Void alloc(size_type n)
   {
      capacity_ = n;
      if (data_)
         delete[] data_;
      if (capacity_ > 0)
         data_ = new UChar[capacity_];
   }

   size_type capacity_;
   size_type length_;
   pUChar data_;
};

typedef std::vector<EOctetString> EOctetStringVec;

inline std::ostream &operator<<(std::ostream &output, const EOctetString &ostr)
{
   std::ios_base::fmtflags f(std::cout.flags());
   for (EOctetString::size_type idx=0; idx<ostr.length(); idx++)
       output << (idx?", ":"") << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<UInt>(ostr.at(idx));
   std::cout.flags(f);
   return output;
}

namespace std
{
   template <>
   struct hash<EOctetString>
   {
      std::size_t operator()(const EOctetString &ostr) const noexcept
      {
         return EMurmurHash64::getHash(ostr.data(), ostr.length());
      }
   };
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#ifndef LOW_NIBBLE
#define LOW_NIBBLE(a) (((unsigned char)a) & 0x0f)
#endif
#ifndef HIGH_NIBBLE
#define HIGH_NIBBLE(a) ((((unsigned char)a) & 0xf0) >> 4)
#endif
#define CHAR2TBCD(c)                \
(                                   \
   c >= '0' && c <= '9' ? c - '0' : \
   c == '*' ? 10 :                  \
   c == '#' ? 11 :                  \
   (c == 'a' || c == 'A') ? 12 :    \
   (c == 'b' || c == 'B') ? 13 :    \
   (c == 'c' || c == 'C') ? 14 : 15 \
)

class ETbcdString : public EOctetString
{
public:
   //
   // Excerpt from ITU-T Recommendation Q.825
   //
   // The following octets representing digits of an address encoded as a TBCD-STRING.
   // TBCD-STRING ::= OCTETSTRING
   // This type (Telephony Binary Coded Decimal String) is used to represent dig its from 0
   // through 9, *, #, a, b, c, two digits per octet, each digit encoded 0000 to 1001 (0 to 9),
   // 1010 (*) 1011(#), 1100 (a), 1101 (b) or 1110 (c); 1111 (end of pulsing signal-ST); 0000 is
   // used as a filler when there is an odd number of digits.
   // The most significant address signal is sent first. Subsequent address signals are sent in
   // successive 4-bit fields.
   //

   ETbcdString()
      : EOctetString(),
        len_(0)
   {
   }
   ETbcdString(size_t len)
      : EOctetString((len / 2) + (len % 2)),
        len_(len)
   {
   }
   ETbcdString(const ETbcdString &tbcd)
      : EOctetString(tbcd),
        len_(tbcd.len_)
   {
   }

   ETbcdString &operator=(const ETbcdString &tbcd)
   {
      EOctetString::operator=(tbcd);
      len_ = tbcd.len_;
      return *this;
   }


   EString toString() const
   {
      static cpChar tbcdChars = (cpChar)"0123456789*#abc";

      EString str;

      for (size_t idx=0; idx<EOctetString::length(); idx++)
      {
         UChar c = (*this)[idx];

         if (tbcdChars[LOW_NIBBLE(c)])
         {
            str += tbcdChars[LOW_NIBBLE(c)];
            if (tbcdChars[HIGH_NIBBLE(c)])
               str += tbcdChars[HIGH_NIBBLE(c)];
         }
      }

      return str;
   }

   ETbcdString &fromString(const std::string &str)
   {
      return fromString(str.c_str());
   }

   ETbcdString &fromString(cpStr str)
   {
      EOctetString::clear();

      if (!str)
         return *this;

      size_t idx = 0;

      while (True)
      {
         if (!str[idx])
            break;

         UChar c = CHAR2TBCD(str[idx++]);
         len_++;

         if (str[idx])
         {
            c |= CHAR2TBCD(str[idx++]);
            len_++;
         }
         else
         {
            c |= 0xf0;
         }
         append(c);
      }

      return *this;
   }

   size_t length() const { return len_; }

   ETbcdString &clear() { EOctetString::clear(); len_ = 0; return *this; }

private:
   size_t len_;
};

#endif // __EOSTRING_H