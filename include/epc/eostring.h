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

/// @file
/// @brief Defines the EOctetString and ETbcdString classes.

DECLARE_ERROR(EOctetString_OutOfRange);

/// @brief Represents an OctetString as defined in RFC 6733.
class EOctetString
{
public:
   typedef size_t size_type;
   static const size_type npos { std::string::npos };

   /// @brief Default constructor.
   EOctetString()
       : capacity_(0),
         length_(0),
         data_(NULL)
   {
      reserve(minCapacity_);
   }
   /// @brief Class constructor.
   /// @param n initial internal buffer size.
   EOctetString(size_type n)
      : capacity_(0),
        length_(0),
        data_(NULL)
   {
      reserve(n);
   }
   /// @brief Class constructor.
   /// @param s string used to initialize this EOctetString object.
   EOctetString(cpStr s)
      : capacity_(0),
        length_(0),
        data_(NULL)
   {
      size_type n = strlen(s);
      reserve(n);
      assign(s);
   }
   /// @brief Class constructor.
   /// @param data pointer to buffer used to initialize this EOctetString object.
   /// @param n length of the buffer.
   EOctetString(cpUChar data, size_type n)
      : capacity_(0),
        length_(0),
        data_(NULL)
   {
      reserve(n);
      assign(data, n);
   }
   /// @brief Copy constructor.
   /// @param ostr EOctetString object to copy.
   EOctetString(const EOctetString &ostr)
       : capacity_(0),
         length_(0),
         data_(NULL)
   {
      *this = ostr;
   }
   /// @brief Class constructor. Initializes this EOctetString from a subset of
   ///   another EOctetString object.
   /// @param ostr EOctetString object to copy from.
   /// @param pos starting position of data to copy.
   /// @param len length of data to copy.  A value of "npos" causes data to be
   ///   from the starting position to the end of the EOctetString.
   EOctetString(const EOctetString &ostr, size_type pos, size_type len = npos)
       : capacity_(0),
         length_(0),
         data_(NULL)
   {
      assign(ostr, pos, len);
   }
   /// @brief Class destructor.
   ~EOctetString()
   {
      dealloc();
   }

   /// @brief Assignment operator
   /// @param ostr a reference to the EOctetString object to copy.
   EOctetString &operator=(const EOctetString &ostr)  { return assign(ostr); }
   /// @brief Assignment operator
   /// @param s copies the NULL terminated string.
   EOctetString &operator=(cpStr s)                   { return assign(s);    }
   /// @brief Assignment operator
   /// @param c a single character to assign.
   EOctetString &operator=(Char c)                    { return assign(1, c); }
   /// @brief Assignment operator
   /// @param c a single unsigned character to assign.
   EOctetString &operator=(UChar c)                   { return assign(1, c); }
   /// @brief Assignment operator.  Performs a move assignment.
   /// @param ostr the EOctetString to assign to this object.
   EOctetString &operator=(EOctetString &&ostr)       { return assign(std::move(ostr)); }

   /// @brief Append operator.
   /// @param ostr a reference to the EOctetString to append to this object.
   EOctetString &operator+=(const EOctetString &ostr) { return append(ostr); }
   /// @brief Append operator.
   /// @param s the NULL terminated string to append to this object.
   EOctetString &operator+=(cpStr s)                  { return append(s);    }
   /// @brief Append operator.
   /// @param c the character to append to this object.
   EOctetString &operator+=(Char c)                   { return append((UChar)c); }
   /// @brief Append operator.
   /// @param c the unsigned character to append to this object.
   EOctetString &operator+=(UChar c)                  { return append(c); }

   /// @brief Array index operator.  Returns a reference to the specified array member.
   /// @param pos the array index.
   /// @{
   UChar &operator[](size_type pos)                   { if (pos >= length_) throw EOctetString_OutOfRange(); return data_[pos]; }
   cUChar &operator[](size_type pos) const            { if (pos >= length_) throw EOctetString_OutOfRange(); return data_[pos]; }
   /// @}

   /// @brief Equality operator.
   /// @param ostr the EOctetString object to compare to.
   bool operator==(const EOctetString &ostr) const { return compare(ostr) == 0; }
   /// @brief Not equal operator.
   /// @param ostr the EOctetString object to compare to.
   bool operator!=(const EOctetString &ostr) const { return compare(ostr) != 0; }
   /// @brief Less than operator.
   /// @param ostr the EOctetString object to compare to.
   bool operator<(const EOctetString &ostr)  const { return compare(ostr) < 0; }
   /// @brief Greater than operator.
   /// @param ostr the EOctetString object to compare to.
   bool operator>(const EOctetString &ostr)  const { return compare(ostr) > 0; }
   /// @brief Less than or equal to operator.
   /// @param ostr the EOctetString object to compare to.
   bool operator<=(const EOctetString &ostr) const { return !(compare(ostr) > 0); }
   /// @brief Greater than or equal to operator.
   /// @param ostr the EOctetString object to compare to.
   bool operator>=(const EOctetString &ostr) const { return !(compare(ostr) < 0); }

   /// @brief Equality operator.
   /// @param str the NULL terminated string to compare against.
   bool operator==(cpStr str) const { return compare(str) == 0; }
   /// @brief Not equal operator.
   /// @param str the NULL terminated string to compare against.
   bool operator!=(cpStr str) const { return compare(str) != 0; }
   /// @brief Less than operator.
   /// @param str the NULL terminated string to compare against.
   bool operator<(cpStr str)  const { return compare(str) < 0; }
   /// @param str the NULL terminated string to compare against.
   /// @brief Greater than operator.
   bool operator>(cpStr str)  const { return compare(str) > 0; }
   /// @param str the NULL terminated string to compare against.
   /// @brief Less than or equal to operator.
   bool operator<=(cpStr str) const { return !(compare(str) > 0); }
   /// @param str the NULL terminated string to compare against.
   /// @brief Greater than or equal to operator.
   bool operator>=(cpStr str) const { return !(compare(str) < 0); }

   /// @brief Appends to this object.
   /// @param ostr a reference to the EOctetString object copy.
   EOctetString &append(const EOctetString &ostr)
   {
      return append(ostr.data_, ostr.length_);
   }
   /// @brief Appends to this object.
   /// @param ostr a reference to the EOctetString object to copy from.
   /// @param subpos the position to start copying from.
   /// @param sublen the number of bytes to copy.
   EOctetString &append(const EOctetString &ostr, size_type subpos, size_type sublen)
   {
      if (subpos >= ostr.length_)
         throw EOctetString_OutOfRange();

      return append(&ostr.data_[subpos], sublen);
   }
   /// @brief Appends to this object.
   /// @param s the NULL terminated string to append.
   EOctetString &append(cpStr s)
   {
      return append(reinterpret_cast<cpUChar>(s), strlen(s));
   }
   /// @brief Appends to this object.
   /// @param s the NULL terminated string to append.
   /// @param n the number of bytes to copy from the string.
   EOctetString &append(cpStr s, size_type n)
   {
      return append(reinterpret_cast<cpUChar>(s), n);
   }
   /// @brief Appends to this object.
   /// @param c the unsigned character to append.
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
   /// @brief Appends to this object.
   /// @param data a pointer to where to start appending data from.
   /// @param n the number of bytes to append.
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
   /// @brief Appends to this object.
   /// @param n the number of times to append the specified value.
   /// @param c the value to repeat.
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

   /// @brief Assigns the specified value to this object.
   /// @param ostr a reference to the EOctetString to assign to this object.
   EOctetString &assign(const EOctetString &ostr)
   {
      if (capacity_ < ostr.capacity_)
         alloc(ostr.capacity_);
      length_ = ostr.length_;
      memcpy(data_, ostr.data_, length_);
      return *this;
   }
   /// @brief Assigns the specified value to this object.
   /// @param ostr a reference to the EOctetString to assign from.
   /// @param subpos the position in the EOctetString to start assigning from.
   /// @param sublen the number of bytes to assign.
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
   /// @brief Assigns the specified value to this object.
   /// @param s a pointer to a NULL terminated string to assign to this object.
   EOctetString &assign(cpStr s)
   {
      size_type len = strlen(s);
      if (capacity_ < len)
         alloc(len);
      length_ = len;
      memcpy(data_, s, length_);
      return *this;
   }
   /// @brief Assigns the specified value to this object.
   /// @param s a pointer to the character buffer to assign to this object.
   /// @param n the number of bytes to assign.
   EOctetString &assign(cpChar s, size_type n)
   {
      return assign(reinterpret_cast<cpUChar>(s), n);
   }
   /// @brief Assigns the specified value to this object.
   /// @param data pointer to the unsigned character buffer to assign to this object.
   /// @param n teh number of bytes to assign.
   EOctetString &assign(cpUChar data, size_type n)
   {
      if (capacity_ < n)
         alloc(n);
      length_ = n;
      memcpy(data_, data, length_);
      return *this;
   }
   /// @brief Assigns the specified value to this object.
   /// @param n the number of times to repeat the specified value.
   /// @param c the value to repeat.
   EOctetString &assign(size_type n, UChar c)
   {
      if (capacity_ < n)
         alloc(n);
      length_ = n;
      memset(data_, c, length_);
      return *this;
   }
   /// @brief Assign the specified value to this object using move semantics.
   /// @param ostr the EOctetString object to move to this object.
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

   /// @brief Returns a reference to the data at the specified offset.
   /// @param pos the offset of the data to return.
   UChar &at(size_type pos)
   {
      if (pos > length_)
         throw EOctetString_OutOfRange();
      return data_[pos];
   }
   /// @brief Returns a constant reference to the data at the specified offset.
   /// @param pos the offset of the data to return.
   cUChar &at(size_type pos) const
   {
      if (pos > length_)
         throw EOctetString_OutOfRange();
      return data_[pos];
   }

   /// @brief Returns a reference to the element at the end of the buffer.
   UChar &back()
   {
      return data_[length_ - 1];
   }
   /// @brief Returns a constant reference to the element at the end of the buffer.
   cUChar &back() const
   {
      return data_[length_ - 1];
   }

   /// @brief Returns the size of the currently allocated internal buffer.
   size_type capacity() const { return capacity_; }

   /// @brief Sets the internal buffer to all NULL's and sets the length to zero.
   Void clear()
   {
      memset(data_, 0, length_);
      length_ = 0;
   }

   /// @brief Compares two EOctetString objects.
   /// @param ostr the EOctetString to compare this object to.
   /// @return an integral value indicating the relation ship between the octet strings.
   ///   0 they are equal, <0 either the value of the first byte that does not match
   ///   is lower in the compared string or all compared bytes match but the compared
   ///   octet string is shorter, >0 either the value of the first byte that does not
   ///   match is greater in the compared object or all compared bytes match but the
   ///   compared string is longer.
   Int compare(const EOctetString &ostr) const
   {
      return compare(0, length_, ostr.data_, ostr.length_);
   }
   /// @brief Compares a subset of the supplied EOctetString to this object.
   /// @param pos the offset in this object to start comparing.
   /// @param len the number of bytes to compare.
   /// @param ostr the EOctetString to compare this object to.
   /// @return an integral value indicating the relation ship between the octet strings.
   ///   0 they are equal, <0 either the value of the first byte that does not match
   ///   is lower in the compared string or all compared bytes match but the compared
   ///   octet string is shorter, >0 either the value of the first byte that does not
   ///   match is greater in the compared object or all compared bytes match but the
   ///   compared string is longer.
   Int compare(size_type pos, size_type len, const EOctetString &ostr) const
   {
      return compare(pos, len, ostr.data_, ostr.length_);
   }
   /// @brief Compares a subset of the supplied EOctetString to this object.
   /// @param pos the offset in this object to start comparing.
   /// @param len the number of bytes to compare.
   /// @param ostr the EOctetString to compare this object to.
   /// @param subpos the offset in the supplied object to start comparing.
   /// @param sublen the number of bytes in the supplied object to compare.
   /// @return an integral value indicating the relation ship between the octet strings.
   ///   0 they are equal, <0 either the value of the first byte that does not match
   ///   is lower in the compared string or all compared bytes match but the compared
   ///   octet string is shorter, >0 either the value of the first byte that does not
   ///   match is greater in the compared object or all compared bytes match but the
   ///   compared string is longer.
   Int compare(size_type pos, size_type len, const EOctetString &ostr, size_type subpos, size_type sublen) const
   {
      if (subpos >= ostr.length_)
         throw EOctetString_OutOfRange();
      return compare(pos, len, &ostr.data_[subpos], std::min(ostr.length_ - subpos, sublen));
   }
   /// @brief Compares the supplied NULL terminated string to this object.
   /// @param s a pointer to the NULL terminated to string to compare.
   /// @return an integral value indicating the relation ship between the octet strings.
   ///   0 they are equal, <0 either the value of the first byte that does not match
   ///   is lower in the compared string or all compared bytes match but the compared
   ///   octet string is shorter, >0 either the value of the first byte that does not
   ///   match is greater in the compared object or all compared bytes match but the
   ///   compared string is longer.
   Int compare(cpStr s) const
   {
      return compare(0, length_, reinterpret_cast<cpUChar>(s), strlen(s));
   }
   /// @brief Compares the supplied buffer to this object.
   /// @param data a pointer to the buffer to compare.
   /// @param n the number of bytes to compare.
   /// @return an integral value indicating the relation ship between the octet strings.
   ///   0 they are equal, <0 either the value of the first byte that does not match
   ///   is lower in the compared string or all compared bytes match but the compared
   ///   octet string is shorter, >0 either the value of the first byte that does not
   ///   match is greater in the compared object or all compared bytes match but the
   ///   compared string is longer.
   Int compare(cpUChar data, size_type n) const
   {
      return compare(0, length_, data, n);
   }
   /// @brief Compares a NULL terminated string to this object starting at the supplied offset.
   /// @param pos the offset in this object to start the comparison.
   /// @param len the number of bytes to compare.
   /// @param s a pointer to the NULL terminated string to compare against.
   /// @return an integral value indicating the relation ship between the octet strings.
   ///   0 they are equal, <0 either the value of the first byte that does not match
   ///   is lower in the compared string or all compared bytes match but the compared
   ///   octet string is shorter, >0 either the value of the first byte that does not
   ///   match is greater in the compared object or all compared bytes match but the
   ///   compared string is longer.
   Int compare(size_type pos, size_type len, cpStr s) const
   {
      return compare(pos, len, reinterpret_cast<cpUChar>(s), strlen(s));
   }
   /// @brief Compares a NULL terminated string to this object starting at the supplied offset.
   /// @param pos the offset in this object to start the comparison.
   /// @param len the number of bytes to compare.
   /// @param s a pointer to the NULL terminated string to compare against.
   /// @param n the maximum number of bytes in the supplied NULL terminated string to compare.
   /// @return an integral value indicating the relation ship between the octet strings.
   ///   0 they are equal, <0 either the value of the first byte that does not match
   ///   is lower in the compared string or all compared bytes match but the compared
   ///   octet string is shorter, >0 either the value of the first byte that does not
   ///   match is greater in the compared object or all compared bytes match but the
   ///   compared string is longer.
   Int compare(size_type pos, size_type len, cpStr s, size_type n) const
   {
      return compare(pos, len, reinterpret_cast<cpUChar>(s), n);
   }
   /// @brief Compares buffer to this object starting at the supplied offset.
   /// @param pos the offset in this object to start the comparison.
   /// @param len the number of bytes to compare.
   /// @param data a pointer to the buffer to compare against.
   /// @param n the maximum number of bytes in the supplied buffer to compare.
   /// @return an integral value indicating the relation ship between the octet strings.
   ///   0 they are equal, <0 either the value of the first byte that does not match
   ///   is lower in the compared string or all compared bytes match but the compared
   ///   octet string is shorter, >0 either the value of the first byte that does not
   ///   match is greater in the compared object or all compared bytes match but the
   ///   compared string is longer.
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

   /// @brief Copies data from this object to the supplied buffer.
   /// @param dst a pointer to the destination buffer.
   /// @param len the maximum number of bytes to copy.
   /// @param pos the array offset in the internal buffer to start copying from.
   size_type copy(pUChar dst, size_type len, size_type pos = 0) const
   {
      if (pos >= length_)
         throw EOctetString_OutOfRange();
      len = std::min(length_ - pos, len);
      memcpy(dst, &data_[pos], len);
      return len;
   }

   /// @brief Returns a pointer to the internal data buffer.
   /// @return a pointer to the internal data buffer.
   cpUChar data() const { return data_; }
   /// @brief Returns whether this object is empty.
   /// @return True if length is zero, False otherwise.
   Bool empty() const { return length_ == 0; }

   /// @brief Erases the specified number of bytes.  Supports erasing
   ///   bytes from the middle of the octet string.
   /// @param pos the offset where to start erasing.
   /// @param len the number of bytes to erase.
   /// @return a referenc to this object.
   EOctetString &erase(size_type pos = 0, size_type len = npos)
   {
      // make sure that "pos" is in bounds
      if (pos >= length_)
         throw EOctetString_OutOfRange();

      // adjust length to not delete more bytes than are present
      len = std::min(length_ - pos, len);

      // calculate the offet to start erasing and the length
      size_type erase_start = length_ - len;

      // check for erasing x bytes from the middle
      if (erase_start > pos)
      {
         size_type copy_dst = pos;
         size_type copy_src = pos + len;
         for (; copy_src<length_; copy_dst++, copy_src++)
            data_[copy_dst] = data_[copy_src];
      }
      
      // clear the "erased" bytes
      std::memset(&data_[erase_start], 0, len);

      // reduce the length
      length_ -= len;
      return *this;
   }

   /// @brief Returns the length of the assigned value of this object.
   /// @return the length of the assigned value of this object.
   /// @{
   size_type length() const { return length_; }
   size_type size() const { return length_; }
   /// @}

   /// @brief Removes the last byte.
   Void pop_back()
   {
      if (length_ > 0)
      {
         length_--;
         data_[length_] = 0;
      }
   }

   /// @brief Appends the specified value to the end of the octet string.
   /// @param c the value to append.
   /// @{
   Void push_back(Char c)  { append(1, static_cast<UChar>(c)); }
   Void push_back(UChar c) { append(1, c); }
   /// @}

   /// @brief Sets the internal buffer size to the specified value and
   ///   reallocates the buffer if necessary.
   /// @param n the number of bytes to reserve.
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
   /// @brief Resizes the buffer to the specified and fills the "new"
   ///   space with the specified value.
   /// @param n the new buffer size.
   /// @param c the fill value.
   /// @{
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
   /// @}

   /// @brief reduces the capacity of the buffer to match it's current length.
   void shrink_to_fit()
   {
      reserve();
   }

   /// @brief Insertion oeprator for serialization.
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

/// @cond DOXYGEN_EXCLUDE
typedef std::vector<EOctetString> EOctetStringVec;

inline std::ostream &operator<<(std::ostream &output, const EOctetString &ostr)
{
   std::ios_base::fmtflags f(std::cout.flags());
   for (EOctetString::size_type idx=0; idx<ostr.length(); idx++)
       output << (idx?", ":"") << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<UInt>(ostr.at(idx));
   std::cout.flags(f);
   return output;
}
/// @endcond

namespace std
{
   /// @brief Calcuates a hash value for the specified EOctetString object.
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

/// @cond DOXYGEN_EXCLUDE
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
/// @endcond

/// @brief Represents an TBCD String (Telephony Binary Coded Decimal String) as
///   defined by ITU-T Recommendation Q.825.
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

   /// @brief Default constructor.
   ETbcdString()
      : EOctetString(),
        len_(0)
   {
   }
   /// @brief Class constructor.
   /// @param len the initial length of the TBCD string.
   ETbcdString(size_t len)
      : EOctetString((len / 2) + (len % 2)),
        len_(len)
   {
   }
   /// @brief Copy constructor.
   /// @param tbcd a reference to the ETbcdString object to copy.
   ETbcdString(const ETbcdString &tbcd)
      : EOctetString(tbcd),
        len_(tbcd.len_)
   {
   }
   /// @brief Assignment operator.
   /// @param tbcd a reference to the ETbcdString object to copy.
   ETbcdString &operator=(const ETbcdString &tbcd)
   {
      EOctetString::operator=(tbcd);
      len_ = tbcd.len_;
      return *this;
   }

   /// @brief Converts a TBCD string to a printable string.
   /// @return the printable string.
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
   /// @brief Converts the printable string to a TBCD string.
   /// @param str the string to convert.
   /// @return a reference to this object.
   /// @{
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
   /// @}

   /// @brief Returns the length of the TBCD string.
   /// @return the length of the TBCD string.
   size_t length() const { return len_; }

   /// @brief Clears the TBCD string setting it's length to zero.
   /// @return a reference to this object.
   ETbcdString &clear() { EOctetString::clear(); len_ = 0; return *this; }

private:
   size_t len_;
};

#endif // __EOSTRING_H