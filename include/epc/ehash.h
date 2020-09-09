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

#ifndef __ehash_h_included
#define __ehash_h_included

#include <limits>
#include <bits/hash_bytes.h>

#include "ebase.h"
#include "estring.h"
#include "eerror.h"

/// @file
/// @brief Hash calculation functions for strings and byte arrays.

/// @brief Calcuates a 32-bit hash value for the specified string or array of characters.
class EHash
{
public:
   /// @brief Returns the 32-bit hash value for the specified string.
   /// @param str The string to calculate the hash for.
   /// @return 32-bit hash value
   static ULong getHash(EString &str);
   /// @brief Returns the 32-bit has value for the array of characters.
   /// @param val The array of characters to calculate the hash for.
   /// @param len The length of the array of characters.
   /// @return 32-bit hash value
   static ULong getHash(cpChar val, ULong len);
   /// @brief Returns the 32-bit has value for the array of unsigned characters.
   /// @param val The array of unsigned characters to calculate the hash for.
   /// @param len The length of the array of unsigned characters.
   /// @return 32-bit hash value
   static ULong getHash(cpUChar val, ULong len);

private:
   static ULong m_crcTable[256];
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//
// ESipHash24 is adapted from the C reference implementation located at
//    https://github.com/veorq/SipHash which is licensed to public domain by
//    the [CC0 1.0 Universal] license.
//

DECLARE_ERROR(ESipHash24Error_InvalidKeyLength);

/// @brief Calculates a 32-bit, 64-bit or 128-bit hash value using the SipHash algorithm.
class ESipHash24
{
public:
   /// @brief Represents a key to be used when calculating a hash value.
   class Key
   {
   public:
      /// @brief Class constructor.
      Key() : data_{0} {}
      /// @brief Copy constructor.
      /// @param k the Key to copy.
      Key(const Key &k) { assign(k.data_,sizeof(k.data_)); }
      /// @brief Class constructor.
      /// @param k a buffer containing the key value.
      /// @param len the length of the key buffer.
      Key(cpUChar k, const size_t len) { assign(k,len); }

      /// @brief The const unsigned char* conversion operator.
      /// @return The data value of this Key object.
      operator cpUChar() const { return data_; }
      /// @brief Data extractor.
      /// @return The data value of this Key object.
      cpUChar data() const { return data_; }

      /// @brief Assigns a new key value.
      /// @param k the data buffer to copy.
      /// @param len the length of the data buffer to copy.
      /// @return A reference to this object.
      Key& assign(cpUChar k, const size_t len)
      {
         size_t i = 0;
         for (; i<min(len,sizeof(data_)); i++)
            data_[i] = k[i];
         for( ; i < sizeof(data_); i++)
            data_[i] = 0;
         return *this;
      }
   private:
      UChar data_[16];
   };

   /// @brief Assigns a new default key value.
   /// @param k the new key value.
   /// @return A reference to the new key value.
   static Key& setKey(const Key& k) { return key_ = k; }
   /// @brief Assigns a new default key value.
   /// @param val the key data for the new key value.
   /// @param len the length of the key data. 
   /// @return A reference to the new key value.
   static Key& setKey(cpUChar val, const size_t len) { return key_.assign(val, len); }
   /// @brief Retrieves the default key.
   /// @return Reference to the default key.
   static const Key& key() { return key_; }

   /// @brief Returns the 32-bit hash associated with the string.
   /// @param str The string to calculate the hash for.
   /// @param key The key to be used to calculate the hash.
   /// @return The 32-bit hash value.
   static ULong getHash32(EString &str, const Key &key=key_)
   {
      return getHash32(reinterpret_cast<cpUChar>(str.c_str()), str.length(), key);
   }
   /// @brief Returns the 32-bit hash associated with the character buffer.
   /// @param val The character buffer to calculate the hash for.
   /// @param len The length of the character buffer.
   /// @param key The key to be used to calculate the hash.
   /// @return The 32-bit hash value.
   static ULong getHash32(cpChar val, const size_t len, const Key &key=key_)
   {
      return getHash32(reinterpret_cast<cpUChar>(val), len, key);
   }
   /// @brief Returns the 32-bit hash associated with the unsigned character buffer.
   /// @param in The unsigned character buffer to calculate the hash for.
   /// @param inlen The length of the unsigned character buffer.
   /// @param key The key to be used to calculate the hash.
   /// @return The 32-bit hash value.
   static ULong getHash32(cpUChar in, const size_t inlen, const Key &key=key_);
   /// @brief Calculates the 32-bit hash associated with the unsigned character buffer.
   /// @param in The unsigned character buffer to calculate the hash for.
   /// @param inlen The length of the unsigned character buffer.
   /// @param out The output buffer for the resulting hash.
   /// @param outlen The length of the output buffer.
   /// @param key The key to be used to calculate the hash.
   /// @return True
   static Bool getHash32(cpUChar in, const size_t inlen, pUChar out, const size_t outlen, const Key &key=key_);
   /// @brief Returns the 32-bit hash associated with the unsigned character buffer.
   /// @param val The value to calculate the hash for.
   /// @param key The key to be used to calculate the hash.
   /// @return The 32-bit hash value.
   /// @{
   static ULong getHash32(cShort val, const Key &key=key_)     { return getHash32(reinterpret_cast<cpUChar>(&val), sizeof(val), key); }
   static ULong getHash32(cUShort val, const Key &key=key_)    { return getHash32(reinterpret_cast<cpUChar>(&val), sizeof(val), key); }
   static ULong getHash32(cLong val, const Key &key=key_)      { return getHash32(reinterpret_cast<cpUChar>(&val), sizeof(val), key); }
   static ULong getHash32(cULong val, const Key &key=key_)     { return getHash32(reinterpret_cast<cpUChar>(&val), sizeof(val), key); }
   static ULong getHash32(cLongLong val, const Key &key=key_)  { return getHash32(reinterpret_cast<cpUChar>(&val), sizeof(val), key); }
   static ULong getHash32(cULongLong val, const Key &key=key_) { return getHash32(reinterpret_cast<cpUChar>(&val), sizeof(val), key); }
   static ULong getHash32(cFloat val, const Key &key=key_)     { return getHash32(reinterpret_cast<cpUChar>(&val), sizeof(val), key); }
   static ULong getHash32(cDouble val, const Key &key=key_)    { return getHash32(reinterpret_cast<cpUChar>(&val), sizeof(val), key); }
   /// @}

   /// @brief Returns the 64-bit hash associated with the string.
   /// @param str The string to calculate the hash for.
   /// @param key The key to be used to calculate the hash.
   /// @return The 64-bit hash value.
   static ULongLong getHash64(EString &str, const Key &key=key_)
   {
      return getHash64(reinterpret_cast<cpUChar>(str.c_str()), str.length(), key);
   }
   /// @brief Returns the 64-bit hash associated with the character buffer.
   /// @param val The character buffer to calculate the hash for.
   /// @param len The length of the character buffer.
   /// @param key The key to be used to calculate the hash.
   /// @return The 64-bit hash value.
   static ULongLong getHash64(cpChar val, const size_t len, const Key &key=key_)
   {
      return getHash64(reinterpret_cast<cpUChar>(val), len, key);
   }
   /// @brief Returns the 64-bit hash associated with the unsigned character buffer.
   /// @param in The unsigned character buffer to calculate the hash for.
   /// @param inlen The length of the unsigned character buffer.
   /// @param key The key to be used to calculate the hash.
   /// @return The 64-bit hash value.
   static ULongLong getHash64(cpUChar in, const size_t inlen, const Key &key=key_);
   /// @brief Calculates the 64-bit hash associated with the unsigned character buffer.
   /// @param in The unsigned character buffer to calculate the hash for.
   /// @param inlen The length of the unsigned character buffer.
   /// @param out The output buffer for the resulting hash.
   /// @param outlen The length of the output buffer.
   /// @param key The key to be used to calculate the hash.
   /// @return True
   static Bool getHash64(cpUChar in, size_t inlen, pUChar out, const size_t outlen, const Key &key=key_);
   /// @brief Returns the 64-bit hash associated with the unsigned character buffer.
   /// @param val The value to calculate the hash for.
   /// @param key The key to be used to calculate the hash.
   /// @return The 64-bit hash value.
   static ULongLong getHash64(cShort val, const Key &key=key_)       { return getHash64(reinterpret_cast<cpUChar>(&val), sizeof(val), key); }
   static ULongLong getHash64(cUShort val, const Key &key=key_)      { return getHash64(reinterpret_cast<cpUChar>(&val), sizeof(val), key); }
   static ULongLong getHash64(cLong val, const Key &key=key_)        { return getHash64(reinterpret_cast<cpUChar>(&val), sizeof(val), key); }
   static ULongLong getHash64(cULong val, const Key &key=key_)       { return getHash64(reinterpret_cast<cpUChar>(&val), sizeof(val), key); }
   static ULongLong getHash64(cLongLong val, const Key &key=key_)    { return getHash64(reinterpret_cast<cpUChar>(&val), sizeof(val), key); }
   static ULongLong getHash64(cULongLong val, const Key &key=key_)   { return getHash64(reinterpret_cast<cpUChar>(&val), sizeof(val), key); }
   static ULongLong getHash64(cFloat val, const Key &key=key_)       { return getHash64(reinterpret_cast<cpUChar>(&val), sizeof(val), key); }
   static ULongLong getHash64(cDouble val, const Key &key=key_)      { return getHash64(reinterpret_cast<cpUChar>(&val), sizeof(val), key); }
   /// @}

   /// @brief Calculates the 128-bit hash associated with the string.
   /// @param str The string to calculate the hash for.
   /// @param out The output buffer for the resulting hash.
   /// @param outlen The length of the output buffer.
   /// @param key The key to be used to calculate the hash.
   /// @return True
   static Bool getHash128(EString &str, pUChar out, const size_t outlen, const Key &key=key_)
   {
      return getHash128(reinterpret_cast<cpUChar>(str.c_str()), str.length(), out, outlen, key);
   }
   /// @brief Calculates the 128-bit hash associated with the character buffer.
   /// @param val The character buffer to calculate the hash for.
   /// @param len The length of the unsigned character buffer.
   /// @param out The output buffer for the resulting hash.
   /// @param outlen The length of the output buffer.
   /// @param key The key to be used to calculate the hash.
   /// @return True
   static Bool getHash128(cpChar val, size_t len, pUChar out, const size_t outlen, const Key &key=key_)
   {
      return getHash128(reinterpret_cast<cpUChar>(val), len, out, outlen, key);
   }
   /// @brief Calculates the 128-bit hash associated with the unsigned character buffer.
   /// @param in The unsigned character buffer to calculate the hash for.
   /// @param inlen The length of the unsigned character buffer.
   /// @param out The output buffer for the resulting hash.
   /// @param outlen The length of the output buffer.
   /// @param key The key to be used to calculate the hash.
   /// @return True
   static Bool getHash128(cpUChar in, size_t inlen, pUChar out, const size_t outlen, const Key &key=key_);
   /// @brief Calculates the 128-bit hash associated with the unsigned character buffer.
   /// @param val The value to calculate the hash for.
   /// @param out The output buffer for the resulting hash.
   /// @param outlen The length of the output buffer.
   /// @param key The key to be used to calculate the hash.
   /// @return True
   /// @{
   static Bool getHash128(cShort val, pUChar out, const size_t outlen, const Key &key=key_)     { return getHash128(reinterpret_cast<cpUChar>(&val), sizeof(val), out, outlen, key); }
   static Bool getHash128(cUShort val, pUChar out, const size_t outlen, const Key &key=key_)    { return getHash128(reinterpret_cast<cpUChar>(&val), sizeof(val), out, outlen, key); }
   static Bool getHash128(cLong val, pUChar out, const size_t outlen, const Key &key=key_)      { return getHash128(reinterpret_cast<cpUChar>(&val), sizeof(val), out, outlen, key); }
   static Bool getHash128(cULong val, pUChar out, const size_t outlen, const Key &key=key_)     { return getHash128(reinterpret_cast<cpUChar>(&val), sizeof(val), out, outlen, key); }
   static Bool getHash128(cLongLong val, pUChar out, const size_t outlen, const Key &key=key_)  { return getHash128(reinterpret_cast<cpUChar>(&val), sizeof(val), out, outlen, key); }
   static Bool getHash128(cULongLong val, pUChar out, const size_t outlen, const Key &key=key_) { return getHash128(reinterpret_cast<cpUChar>(&val), sizeof(val), out, outlen, key); }
   static Bool getHash128(cFloat val, pUChar out, const size_t outlen, const Key &key=key_)     { return getHash128(reinterpret_cast<cpUChar>(&val), sizeof(val), out, outlen, key); }
   static Bool getHash128(cDouble val, pUChar out, const size_t outlen, const Key &key=key_)    { return getHash128(reinterpret_cast<cpUChar>(&val), sizeof(val), out, outlen, key); }
   /// @}

private:
   static Bool getFullHash(cpUChar in, size_t inlen, cpUChar k, pUChar out, const size_t outlen);
   static Bool getHalfHash(cpUChar in, size_t inlen, cpUChar k, pUChar out, const size_t outlen);

   static Key key_;
};

/// @brief Calcuates a 64-bit murmur hash for the specified value.
class EMurmurHash64
{
public:
   /// @brief Calculates a 64-bit murmur hash for the value.
   /// @param val The value to calculate the hash for.
   /// @param seed The seed to be used for the hash calculation.
   /// @return The 64-but murmur hash value.
   /// @{
   static size_t getHash(cChar val, size_t seed=0xc70f6907UL) { return _Hash_bytes(&val,sizeof(val),seed); }
   static size_t getHash(cUChar val, size_t seed=0xc70f6907UL) { return _Hash_bytes(&val,sizeof(val),seed); }
   static size_t getHash(cShort val, size_t seed=0xc70f6907UL) { return _Hash_bytes(&val,sizeof(val),seed); }
   static size_t getHash(cUShort val, size_t seed=0xc70f6907UL) { return _Hash_bytes(&val,sizeof(val),seed); }
   static size_t getHash(cLong val, size_t seed=0xc70f6907UL) { return _Hash_bytes(&val,sizeof(val),seed); }
   static size_t getHash(cULong val, size_t seed=0xc70f6907UL) { return _Hash_bytes(&val,sizeof(val),seed); }
   static size_t getHash(cLongLong val, size_t seed=0xc70f6907UL) { return _Hash_bytes(&val,sizeof(val),seed); }
   static size_t getHash(cULongLong val, size_t seed=0xc70f6907UL) { return _Hash_bytes(&val,sizeof(val),seed); }
   static size_t getHash(cFloat val, size_t seed=0xc70f6907UL) { return _Hash_bytes(&val,sizeof(val),seed); }
   static size_t getHash(cDouble val, size_t seed=0xc70f6907UL) { return _Hash_bytes(&val,sizeof(val),seed); }
   static size_t getHash(const EString &val, size_t seed=0xc70f6907UL) { return _Hash_bytes(val.c_str(),val.size(),seed); }
   /// @}

   /// @brief Calculates a 64-bit murmur hash for the value.
   /// @param val The value to calculate the hash for.
   /// @param len The length of the value in bytes.
   /// @param seed The seed to be used for the hash calculation.
   /// @return The 64-bit murmur hash value.
   /// @{
   static size_t getHash(cpChar val, size_t len, size_t seed=0xc70f6907UL) { return _Hash_bytes(val,len,seed); }
   static size_t getHash(cpUChar val, size_t len, size_t seed=0xc70f6907UL) { return _Hash_bytes(val,len,seed); }
   /// @}

   /// @brief Combines 2 64-bit hash values.
   /// @param h1 The first 64-bit hash value.
   /// @param h2 The second 64-bit hash value.
   /// @return The rsulting 64-bit combined hash value.
   static size_t combine(size_t h1, size_t h2)
   {
      return rotateLeft(circularAdd(h1,h2), 19) ^ circularSubtract(h1,h2);
   }

private:
   static size_t rotateLeft(const size_t val, const size_t bits)
   {
      return (size_t)((val << bits) | (val >> (64 - bits)));
   }

   static size_t circularAdd(size_t val1, size_t val2)
   {
      size_t result = val1 + val2;
      return result + (result < val1);
   }

   static size_t circularSubtract(size_t val1, size_t val2)
   {
      size_t result = val1 - val2;
      return result - (result > val1);
   }
};

#endif // #define __ehash_h_included