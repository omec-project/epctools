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

#include <iostream>

#include "epctools.h"
#include "egetopt.h"

#define RAPIDJSON_NAMESPACE epctoolsrapidjson
#include "rapidjson/filereadstream.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/pointer.h"

#define CMDLINE "cmdline"
#define PROGRAM "program"
#define RAW "raw"
#define ARGS "args"
#define CMDLINEARGS CMDLINE "/" ARGS
#define CMDLINERAW CMDLINE "/" RAW

#define FILEBUFFER 512

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/// @cond DOXYGEN_EXCLUDE

EGetOptError_MissingRequiredArgument::EGetOptError_MissingRequiredArgument(cpStr arg)
{
   setSevere();
   setTextf("Missing required argument for %s", arg);
}

EGetOptError_UnsupportedArgType::EGetOptError_UnsupportedArgType(EGetOpt::ArgType argType)
{
   EString at;

   setSevere();
   switch (argType)
   {
   case EGetOpt::no_argument:
   {
      at = "no_argument";
      break;
   }
   case EGetOpt::required_argument:
   {
      at = "required_argument";
      break;
   }
   case EGetOpt::optional_argument:
   {
      at = "optional_argument";
      break;
   }
   default:
   {
      at.format("%d", argType);
      break;
   }
   }
   setTextf("Unsupported option argument type [%s]", at.c_str());
}

EGetOptError_UnsupportedDataType::EGetOptError_UnsupportedDataType(EGetOpt::DataType dataType)
{
   EString at;

   setSevere();
   switch (dataType)
   {
   case EGetOpt::dtString:
   {
      at = "dtString";
      break;
   }
   case EGetOpt::dtInt32:
   {
      at = "dtInt32";
      break;
   }
   case EGetOpt::dtInt64:
   {
      at = "dtInt64";
      break;
   }
   case EGetOpt::dtUInt32:
   {
      at = "dtUInt32";
      break;
   }
   case EGetOpt::dtUInt64:
   {
      at = "dtUInt64";
      break;
   }
   case EGetOpt::dtDouble:
   {
      at = "dtDouble";
      break;
   }
   case EGetOpt::dtBool:
   {
      at = "dtBool";
      break;
   }
   default:
   {
      at.format("%d", dataType);
      break;
   }
   }
   setTextf("Unsupported option data type [%s]", at.c_str());
}

EGetOptError_UnsupportedBooleanValue::EGetOptError_UnsupportedBooleanValue(cpStr val)
{
   setSevere();
   setTextf("Unsupported boolean argument value [%s]", val);
}

EGetOptError_FileParsing::EGetOptError_FileParsing(cpStr val)
{
   setSevere();
   setTextf("Unable to open [%s] for parsing.", val);
}

EGetOptError_UnsupportedArrayType::EGetOptError_UnsupportedArrayType(cpStr val)
{
   setSevere();
   setTextf("Unsupported vector type [%s] for array.", val);
}

EGetOptError_UnexpectedArrayElementType::EGetOptError_UnexpectedArrayElementType(cpStr rcvd, cpStr expctd)
{
   setSevere();
   setTextf("Unsupported array element type.  Received [%s] expected [%s].", rcvd, expctd);
}

EGetOptError_NotArray::EGetOptError_NotArray()
{
   setSevere();
   setText("Element is not an array");
}

/// @endcond

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

static Void _print(RAPIDJSON_NAMESPACE::Value &node)
{
   RAPIDJSON_NAMESPACE::StringBuffer sb;
   RAPIDJSON_NAMESPACE::PrettyWriter<RAPIDJSON_NAMESPACE::StringBuffer> writer(sb);

   node.Accept(writer);
   std::cout << sb.GetString() << std::endl;
}

static EString _combinePath(cpStr root, cpStr path)
{
   EString result = "/";

   if (root && *root)
      result += (*root == '/' ? &root[1] : root);

   if (result[result.size()-1] == '/')
      result = result.substr(0, result.size()-1);

   if (path && *path)
   {
      result += "/";
      result += (*path == '/' ? &path[1] : path);

      if (result[result.size()-1] == '/')
         result = result.substr(0, result.size()-1);
   }

   return result;
}

static Bool _findOneOf(RAPIDJSON_NAMESPACE::Document &doc, const char *root, const char *path, RAPIDJSON_NAMESPACE::Value* &value)
{
   auto paths = EUtility::split(path, ",");

   for (EString &pth : paths)
   {
      EString s = _combinePath(root, pth);
      RAPIDJSON_NAMESPACE::Pointer ptr(s.c_str());
      value = RAPIDJSON_NAMESPACE::GetValueByPointer(doc, ptr);
      if (value != nullptr)
         return true;
   }

   return false;
}

static Bool _merge(RAPIDJSON_NAMESPACE::Value &dst, RAPIDJSON_NAMESPACE::Value &src, RAPIDJSON_NAMESPACE::Document::AllocatorType &allocator)
{
   for (auto srcIt = src.MemberBegin(); srcIt != src.MemberEnd(); ++srcIt)
   {
      auto dstIt = dst.FindMember(srcIt->name);
      if (dstIt == dst.MemberEnd())
      {
         RAPIDJSON_NAMESPACE::Value dstName;
         dstName.CopyFrom(srcIt->name, allocator);
         RAPIDJSON_NAMESPACE::Value dstVal;
         dstVal.CopyFrom(srcIt->value, allocator);

         dst.AddMember(dstName, dstVal, allocator);

         dstName.CopyFrom(srcIt->name, allocator);
         dstIt = dst.FindMember(dstName);
         if (dstIt == dst.MemberEnd())
            return false;
      }
      else
      {
         auto srcT = srcIt->value.GetType();
         auto dstT = dstIt->value.GetType();
         if (srcT != dstT)
            return false;

         if (srcIt->value.IsArray())
         {
            for (auto arrayIt = srcIt->value.Begin(); arrayIt != srcIt->value.End(); ++arrayIt)
            {
               RAPIDJSON_NAMESPACE::Value dstVal;
               dstVal.CopyFrom(*arrayIt, allocator);
               dstIt->value.PushBack(dstVal, allocator);
            }
         }
         else if (srcIt->value.IsObject())
         {
            if (!_merge(dstIt->value, srcIt->value, allocator))
               return false;
         }
         else
         {
            dstIt->value.CopyFrom(srcIt->value, allocator);
         }
      }
   }

   return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/// @cond DOXYGEN_EXCLUDE
struct _EGetOpt
{
   RAPIDJSON_NAMESPACE::Document s_json;
};
/// @endcond

#define ADDRAW(__node__, __arg__, __allocator__)                                                 \
   {                                                                                             \
      __node__[RAW].PushBack(RAPIDJSON_NAMESPACE::Value(__arg__, __allocator__), __allocator__); \
   }

EGetOpt::EGetOpt()
{
   _EGetOpt *o = new _EGetOpt();
   o->s_json.SetObject();

   RAPIDJSON_NAMESPACE::Value val;

   val.SetObject();
   o->s_json.AddMember(CMDLINE, val, o->s_json.GetAllocator());

   RAPIDJSON_NAMESPACE::Value &cl = o->s_json[CMDLINE];

   val.SetString("");
   cl.AddMember(PROGRAM, val, o->s_json.GetAllocator());

   val.SetArray();
   cl.AddMember(RAW, val, o->s_json.GetAllocator());

   val.SetArray();
   cl.AddMember(ARGS, val, o->s_json.GetAllocator());

   m_json = o;
}

EGetOpt::~EGetOpt()
{
   if (m_json)
   {
      delete ((_EGetOpt *)m_json);
   }
}

EString EGetOpt::combinePath(cpStr path1, cpStr path2) const
{
   return _combinePath(path1, path2);
}

Void EGetOpt::print() const
{
   _print(((_EGetOpt *)m_json)->s_json);
}

Long EGetOpt::getCmdLine(cpStr path, Long def) const
{
   RAPIDJSON_NAMESPACE::Document &doc = ((_EGetOpt *)m_json)->s_json;
   RAPIDJSON_NAMESPACE::Value *value = nullptr;

   if (_findOneOf(doc, CMDLINE, path, value))
   {
      if (value->IsString())
         return std::stol(value->GetString());

      if (value->IsInt())
         return value->GetInt();
   }

   return def;
}

LongLong EGetOpt::getCmdLine(cpStr path, LongLong def) const
{
   RAPIDJSON_NAMESPACE::Document &doc = ((_EGetOpt *)m_json)->s_json;
   RAPIDJSON_NAMESPACE::Value *value = nullptr;

   if (_findOneOf(doc, CMDLINE, path, value))
   {
      if (value->IsString())
         return std::stoll(value->GetString());

      if (value->IsInt64())
         return value->GetInt64();
   }

   return def;
}

ULong EGetOpt::getCmdLine(cpStr path, ULong def) const
{
   RAPIDJSON_NAMESPACE::Document &doc = ((_EGetOpt *)m_json)->s_json;
   RAPIDJSON_NAMESPACE::Value *value = nullptr;

   if (_findOneOf(doc, CMDLINE, path, value))
   {
      if (value->IsString())
         return std::stoul(value->GetString());

      if (value->IsInt())
         return value->GetUint();
   }

   return def;
}

ULongLong EGetOpt::getCmdLine(cpStr path, ULongLong def) const
{
   RAPIDJSON_NAMESPACE::Document &doc = ((_EGetOpt *)m_json)->s_json;
   RAPIDJSON_NAMESPACE::Value *value = nullptr;

   if (_findOneOf(doc, CMDLINE, path, value))
   {
      if (value->IsString())
         return std::stoull(value->GetString());

      if (value->IsUint64())
         return value->GetUint64();
   }

   return def;
}

Double EGetOpt::getCmdLine(cpStr path, Double def) const
{
   RAPIDJSON_NAMESPACE::Document &doc = ((_EGetOpt *)m_json)->s_json;
   RAPIDJSON_NAMESPACE::Value *value = nullptr;

   if (_findOneOf(doc, CMDLINE, path, value))
   {
      if (value->IsString())
         return std::stod(value->GetString());

      if (value->IsDouble())
         return value->GetDouble();
   }

   return def;
}

cpStr EGetOpt::getCmdLine(cpStr path, cpStr def) const
{
   RAPIDJSON_NAMESPACE::Document &doc = ((_EGetOpt *)m_json)->s_json;
   RAPIDJSON_NAMESPACE::Value *value = nullptr;

   if (_findOneOf(doc, CMDLINE, path, value))
   {
      if (value->IsString())
         return value->GetString();
   }

   return def;
}

Bool EGetOpt::getCmdLine(cpStr path, Bool def) const
{
   RAPIDJSON_NAMESPACE::Document &doc = ((_EGetOpt *)m_json)->s_json;
   RAPIDJSON_NAMESPACE::Value *value = nullptr;

   if (_findOneOf(doc, CMDLINE, path, value))
   {
      if (value->IsBool())
         return value->GetBool();
   }

   return def;
}

std::vector<EString> EGetOpt::getCmdLineArgs() const
{
   RAPIDJSON_NAMESPACE::Document &doc = ((_EGetOpt *)m_json)->s_json;
   RAPIDJSON_NAMESPACE::Pointer ptr("/" CMDLINEARGS);
   RAPIDJSON_NAMESPACE::Value *value = RAPIDJSON_NAMESPACE::GetValueByPointer(doc, ptr);
   std::vector<EString> v;
   EString s;

   if (value != nullptr && value->IsArray())
   {
      for (auto it = value->Begin(); it != value->End(); it++)
      {
         s = it->GetString();
         v.push_back(s);
      }
   }

   return v;
}

std::vector<EString> EGetOpt::getCmdLineRaw() const
{
   RAPIDJSON_NAMESPACE::Document &doc = ((_EGetOpt *)m_json)->s_json;
   RAPIDJSON_NAMESPACE::Pointer ptr("/" CMDLINERAW);
   RAPIDJSON_NAMESPACE::Value *value = RAPIDJSON_NAMESPACE::GetValueByPointer(doc, ptr);
   std::vector<EString> v;
   EString s;

   if (value != nullptr && value->IsArray())
   {
      for (auto it = value->Begin(); it != value->End(); it++)
      {
         s = it->GetString();
         v.push_back(s);
      }
   }

   return v;
}

std::vector<EString> EGetOpt::getMembers(cpStr path) const
{
   EString pth = _combinePath(m_prefix.c_str(), path);
   RAPIDJSON_NAMESPACE::Document &root = ((_EGetOpt *)m_json)->s_json;
   RAPIDJSON_NAMESPACE::Pointer ptr(pth.c_str());
   RAPIDJSON_NAMESPACE::Value *value = RAPIDJSON_NAMESPACE::GetValueByPointer(root, ptr);
   std::vector<EString> v;
   EString s;

   for (RAPIDJSON_NAMESPACE::Value::ConstMemberIterator itr = value->MemberBegin();
        itr != value->MemberEnd(); ++itr)
   {
      s = itr->name.GetString();
      v.push_back(s);
   }

   return v;
}

std::vector<EString> EGetOpt::getMembers(UInt idx, cpStr path, cpStr member) const
{
   EString mbr;
   mbr.format("%u/%s", idx, member);
   EString pth = _combinePath(m_prefix.c_str(), path);
   pth = _combinePath(pth, mbr.c_str());
   RAPIDJSON_NAMESPACE::Document &root = ((_EGetOpt *)m_json)->s_json;
   RAPIDJSON_NAMESPACE::Pointer ptr(pth.c_str());
   RAPIDJSON_NAMESPACE::Value *value = RAPIDJSON_NAMESPACE::GetValueByPointer(root, ptr);
   std::vector<EString> v;
   EString s;

   for (RAPIDJSON_NAMESPACE::Value::ConstMemberIterator itr = value->MemberBegin();
        itr != value->MemberEnd(); ++itr)
   {
      s = itr->name.GetString();
      v.push_back(s);
   }

   return v;
}

Long EGetOpt::get(cpStr path, Long def) const
{
   EString pth = _combinePath(m_prefix.c_str(), path);
   RAPIDJSON_NAMESPACE::Document &root = ((_EGetOpt *)m_json)->s_json;
   RAPIDJSON_NAMESPACE::Pointer ptr(pth.c_str());
   RAPIDJSON_NAMESPACE::Value *value = RAPIDJSON_NAMESPACE::GetValueByPointer(root, ptr);

   if (value != nullptr)
   {
      if (value->IsString())
         return std::stol(value->GetString());
      if (value->IsInt())
         return value->GetInt();
   }

   return def;
}

LongLong EGetOpt::get(cpStr path, LongLong def) const
{
   EString pth = _combinePath(m_prefix.c_str(), path);
   RAPIDJSON_NAMESPACE::Document &root = ((_EGetOpt *)m_json)->s_json;
   RAPIDJSON_NAMESPACE::Pointer ptr(pth.c_str());
   RAPIDJSON_NAMESPACE::Value *value = RAPIDJSON_NAMESPACE::GetValueByPointer(root, ptr);

   if (value != nullptr)
   {
      if (value->IsString())
         return std::stoll(value->GetString());
      if (value->IsInt64())
         return value->GetInt64();
   }

   return def;
}

ULong EGetOpt::get(cpStr path, ULong def) const
{
   EString pth = _combinePath(m_prefix.c_str(), path);
   RAPIDJSON_NAMESPACE::Document &root = ((_EGetOpt *)m_json)->s_json;
   RAPIDJSON_NAMESPACE::Pointer ptr(pth.c_str());
   RAPIDJSON_NAMESPACE::Value *value = RAPIDJSON_NAMESPACE::GetValueByPointer(root, ptr);

   if (value != nullptr)
   {
      if (value->IsString())
         return std::stoul(value->GetString());
      if (value->IsUint())
         return value->GetUint();
   }

   return def;
}

ULongLong EGetOpt::get(cpStr path, ULongLong def) const
{
   EString pth = _combinePath(m_prefix.c_str(), path);
   RAPIDJSON_NAMESPACE::Document &root = ((_EGetOpt *)m_json)->s_json;
   RAPIDJSON_NAMESPACE::Pointer ptr(pth.c_str());
   RAPIDJSON_NAMESPACE::Value *value = RAPIDJSON_NAMESPACE::GetValueByPointer(root, ptr);

   if (value != nullptr)
   {
      if (value->IsString())
         return std::stoull(value->GetString());
      if (value->IsUint64())
         return value->GetUint64();
   }

   return def;
}

Double EGetOpt::get(cpStr path, Double def) const
{
   EString pth = _combinePath(m_prefix.c_str(), path);
   RAPIDJSON_NAMESPACE::Document &root = ((_EGetOpt *)m_json)->s_json;
   RAPIDJSON_NAMESPACE::Pointer ptr(pth.c_str());
   RAPIDJSON_NAMESPACE::Value *value = RAPIDJSON_NAMESPACE::GetValueByPointer(root, ptr);

   if (value != nullptr)
   {
      if (value->IsString())
         return std::stod(value->GetString());
      if (value->IsDouble())
         return value->GetDouble();
   }

   return def;
}

cpStr EGetOpt::get(cpStr path, cpStr def) const
{
   EString pth = _combinePath(m_prefix.c_str(), path);
   RAPIDJSON_NAMESPACE::Document &root = ((_EGetOpt *)m_json)->s_json;
   RAPIDJSON_NAMESPACE::Pointer ptr(pth.c_str());
   RAPIDJSON_NAMESPACE::Value *value = RAPIDJSON_NAMESPACE::GetValueByPointer(root, ptr);

   if (value != nullptr)
   {
      if (value->IsString())
         return value->GetString();
   }

   return def;
}

Bool EGetOpt::get(cpStr path, Bool def) const
{
   EString pth = _combinePath(m_prefix.c_str(), path);
   RAPIDJSON_NAMESPACE::Document &root = ((_EGetOpt *)m_json)->s_json;
   RAPIDJSON_NAMESPACE::Pointer ptr(pth.c_str());
   RAPIDJSON_NAMESPACE::Value *value = RAPIDJSON_NAMESPACE::GetValueByPointer(root, ptr);

   if (value != nullptr)
   {
      if (value->IsBool())
         return value->GetBool();
   }

   return def;
}

UInt EGetOpt::getCount(cpStr path) const
{
   UInt cnt = 0;
   EString pth = _combinePath(m_prefix.c_str(), path);
   RAPIDJSON_NAMESPACE::Document &root = ((_EGetOpt *)m_json)->s_json;
   RAPIDJSON_NAMESPACE::Pointer ptr(pth.c_str());
   RAPIDJSON_NAMESPACE::Value *value = RAPIDJSON_NAMESPACE::GetValueByPointer(root, ptr);

   if (value != nullptr)
      if (value->IsArray())
         cnt = value->Capacity();

   return cnt;
}

static const char* kTypeNames[] = 
   { "Null", "False", "True", "Object", "Array", "String", "Number" };

template<typename T>
std::vector<T> _getArrayBool(RAPIDJSON_NAMESPACE::Value *value)
{
   std::vector<T> vec;
   for (auto &v : value->GetArray())
   {
      if (!v.IsBool())
         throw EGetOptError_UnexpectedArrayElementType(typeid(T).name(), kTypeNames[v.GetType()]);
      vec.push_back(v.GetBool());
   }

   return vec;
}

template<typename T>
std::vector<T> _getArrayInt(RAPIDJSON_NAMESPACE::Value *value)
{
   std::vector<T> vec;
   for (auto &v : value->GetArray())
   {
      if (!v.IsInt())
         throw EGetOptError_UnexpectedArrayElementType(typeid(T).name(), kTypeNames[v.GetType()]);
      vec.push_back(static_cast<T>(v.GetInt()));
   }

   return vec;
}

template<typename T>
std::vector<T> _getArrayUInt(RAPIDJSON_NAMESPACE::Value *value)
{
   std::vector<T> vec;
   for (auto &v : value->GetArray())
   {
      if (!v.IsUint())
         throw EGetOptError_UnexpectedArrayElementType(typeid(T).name(), kTypeNames[v.GetType()]);
      vec.push_back(static_cast<T>(v.GetUint()));
   }

   return vec;
}

template<typename T>
std::vector<T> _getArrayInt64(RAPIDJSON_NAMESPACE::Value *value)
{
   std::vector<T> vec;
   for (auto &v : value->GetArray())
   {
      if (!v.IsInt64())
         throw EGetOptError_UnexpectedArrayElementType(typeid(T).name(), kTypeNames[v.GetType()]);
      vec.push_back(static_cast<T>(v.GetInt64()));
   }

   return vec;
}

template<typename T>
std::vector<T> _getArrayUInt64(RAPIDJSON_NAMESPACE::Value *value)
{
   std::vector<T> vec;
   for (auto &v : value->GetArray())
   {
      if (!v.IsUint64())
         throw EGetOptError_UnexpectedArrayElementType(typeid(T).name(), kTypeNames[v.GetType()]);
      vec.push_back(static_cast<T>(v.GetUint64()));
   }

   return vec;
}

template<typename T>
std::vector<T> _getArrayFloat(RAPIDJSON_NAMESPACE::Value *value)
{
   std::vector<T> vec;
   for (auto &v : value->GetArray())
   {
      if (!v.IsFloat())
         throw EGetOptError_UnexpectedArrayElementType(typeid(T).name(), kTypeNames[v.GetType()]);
      vec.push_back(static_cast<T>(v.GetFloat()));
   }

   return vec;
}

template<typename T>
std::vector<T> _getArrayDouble(RAPIDJSON_NAMESPACE::Value *value)
{
   std::vector<T> vec;
   for (auto &v : value->GetArray())
   {
      if (!v.IsDouble())
         throw EGetOptError_UnexpectedArrayElementType(typeid(T).name(), kTypeNames[v.GetType()]);
      vec.push_back(static_cast<T>(v.GetDouble()));
   }

   return vec;
}

template<typename T>
std::vector<T> _getArrayString(RAPIDJSON_NAMESPACE::Value *value)
{
   std::vector<T> vec;
   T val;
   for (auto &v : value->GetArray())
   {
      if (!v.IsString())
         throw EGetOptError_UnexpectedArrayElementType(typeid(T).name(), kTypeNames[v.GetType()]);
      val = v.GetString();
      vec.push_back(val);
   }

   return vec;
}

static const auto &bool_ti = typeid(bool);
static const auto &double_ti = typeid(double);
static const auto &float_ti = typeid(float);
static const auto &char_ti = typeid(char);
static const auto &uchar_ti = typeid(unsigned char);
static const auto &int_ti = typeid(int);
static const auto &short_ti = typeid(short);
static const auto &long_ti = typeid(long);
static const auto &long_long_ti = typeid(long long);
static const auto &uint_ti = typeid(unsigned int);
static const auto &ushort_ti = typeid(unsigned short);
static const auto &ulong_ti = typeid(unsigned long);
static const auto &ulong_long_ti = typeid(unsigned long long);
static const auto &int8_ti = typeid(int8_t);
static const auto &int16_ti = typeid(int16_t);
static const auto &int32_ti = typeid(int32_t);
static const auto &int64_ti = typeid(int64_t);
static const auto &uint8_ti = typeid(uint8_t);
static const auto &uint16_ti = typeid(uint16_t);
static const auto &uint32_ti = typeid(uint32_t);
static const auto &uint64_ti = typeid(uint64_t);
static const auto &string_ti = typeid(std::string);
static const auto &estring_ti = typeid(EString);

template<typename T>
std::vector<T> EGetOpt::getArray(cpStr path) const
{
   EString pth = _combinePath(m_prefix.c_str(), path);
   RAPIDJSON_NAMESPACE::Document &root = ((_EGetOpt *)m_json)->s_json;
   RAPIDJSON_NAMESPACE::Pointer ptr(pth.c_str());
   RAPIDJSON_NAMESPACE::Value *value = RAPIDJSON_NAMESPACE::GetValueByPointer(root, ptr);

   if (value == nullptr || !value->IsArray())
      throw EGetOptError_NotArray();

   const auto t_hc = typeid(T).hash_code();
   const auto t_name = typeid(T).name();

   if (t_hc == bool_ti.hash_code())
   {
      return _getArrayBool<T>(value);
   }
   else if (t_hc == char_ti.hash_code() || t_hc == short_ti.hash_code() || t_hc == int_ti.hash_code() || 
            t_hc == int8_ti.hash_code() || t_hc == int16_ti.hash_code() || t_hc == int32_ti.hash_code())
   {
      return _getArrayInt<T>(value);
   }
   else if (t_hc == uchar_ti.hash_code() || t_hc == ushort_ti.hash_code() || t_hc == uint_ti.hash_code() || 
            t_hc == uint8_ti.hash_code() || t_hc == uint16_ti.hash_code() || t_hc == uint32_ti.hash_code())
   {
      return _getArrayUInt<T>(value);
   }
   else if (t_hc == long_ti.hash_code() || t_hc == long_long_ti.hash_code() || t_hc == int64_ti.hash_code())
   {
      return _getArrayInt64<T>(value);
   }
   else if (t_hc == ulong_ti.hash_code() || t_hc == ulong_long_ti.hash_code() || t_hc == uint64_ti.hash_code())
   {
      return _getArrayUInt64<T>(value);
   }
   else if (t_hc == float_ti.hash_code())
   {
      return _getArrayFloat<T>(value);
   }
   else if (t_hc == double_ti.hash_code())
   {
      return _getArrayDouble<T>(value);
   }
   else
   {
      throw EGetOptError_UnsupportedArrayType(t_name);
   }
}

template<>
std::vector<std::string> EGetOpt::getArray(cpStr path) const
{
   EString pth = _combinePath(m_prefix.c_str(), path);
   RAPIDJSON_NAMESPACE::Document &root = ((_EGetOpt *)m_json)->s_json;
   RAPIDJSON_NAMESPACE::Pointer ptr(pth.c_str());
   RAPIDJSON_NAMESPACE::Value *value = RAPIDJSON_NAMESPACE::GetValueByPointer(root, ptr);

   if (value == nullptr || !value->IsArray())
      throw EGetOptError_NotArray();

   return _getArrayString<std::string>(value);
}

template<>
std::vector<EString> EGetOpt::getArray(cpStr path) const
{
   EString pth = _combinePath(m_prefix.c_str(), path);
   RAPIDJSON_NAMESPACE::Document &root = ((_EGetOpt *)m_json)->s_json;
   RAPIDJSON_NAMESPACE::Pointer ptr(pth.c_str());
   RAPIDJSON_NAMESPACE::Value *value = RAPIDJSON_NAMESPACE::GetValueByPointer(root, ptr);

   if (value == nullptr || !value->IsArray())
      throw EGetOptError_NotArray();

   return _getArrayString<EString>(value);
}

template std::vector<bool> EGetOpt::getArray<bool>(cpStr path) const;
template std::vector<char> EGetOpt::getArray<char>(cpStr path) const;
template std::vector<int8_t> EGetOpt::getArray<int8_t>(cpStr path) const;
template std::vector<int16_t> EGetOpt::getArray<int16_t>(cpStr path) const;
template std::vector<int32_t> EGetOpt::getArray<int32_t>(cpStr path) const;
template std::vector<int64_t> EGetOpt::getArray<int64_t>(cpStr path) const;
template std::vector<unsigned char> EGetOpt::getArray<unsigned char>(cpStr path) const;
// template std::vector<uint8_t> EGetOpt::getArray<uint8_t>(cpStr path) const;
template std::vector<uint16_t> EGetOpt::getArray<uint16_t>(cpStr path) const;
template std::vector<uint32_t> EGetOpt::getArray<uint32_t>(cpStr path) const;
template std::vector<uint64_t> EGetOpt::getArray<uint64_t>(cpStr path) const;
template std::vector<long long> EGetOpt::getArray<long long>(cpStr path) const;
template std::vector<unsigned long long> EGetOpt::getArray<unsigned long long>(cpStr path) const;
template std::vector<std::string> EGetOpt::getArray<std::string>(cpStr path) const;
template std::vector<EString> EGetOpt::getArray<EString>(cpStr path) const;

const EGetOpt::Option *EGetOpt::findOption(cpStr name, const EGetOpt::Option *options)
{
   if (!name)
      return NULL;

   Int len = strlen(name);
   Bool isShort = len == 2 && name[0] == '-';
   Bool isLong = len >= 3 && name[0] == '-' && name[1] == '-';

   while (!options->shortName.empty() || !options->longName.empty())
   {
      if (isShort && options->shortName == name)
         return options;
      else if (isLong && options->longName == name)
         return options;
      options++;
   }

   return NULL;
}

Void EGetOpt::loadCmdLine(Int argc, pStr *argv, const EGetOpt::Option *options)
{
   RAPIDJSON_NAMESPACE::Document &root = ((_EGetOpt *)m_json)->s_json;
   RAPIDJSON_NAMESPACE::Document::AllocatorType &allocator = root.GetAllocator();

   RAPIDJSON_NAMESPACE::Value &cl = root[CMDLINE];

   const EGetOpt::Option *opt = NULL;
   RAPIDJSON_NAMESPACE::Value key;
   RAPIDJSON_NAMESPACE::Value val;

   for (Int i = 0; i < argc; i++)
   {
      ADDRAW(cl, argv[i], allocator);

      if (i == 0)
      {
         cl[PROGRAM].SetString(argv[i], allocator);
      }
      else
      {
         if (opt)
         {
            switch (opt->argType)
            {
            case required_argument:
            {
               switch (opt->dataType)
               {
               case EGetOpt::dtString:
               {
                  val.SetString(argv[i], allocator);
                  break;
               }
               case EGetOpt::dtInt32:
               {
                  val.SetInt(std::stol(argv[i]));
                  break;
               }
               case EGetOpt::dtInt64:
               {
                  val.SetInt64(std::stoll(argv[i]));
                  break;
               }
               case EGetOpt::dtUInt32:
               {
                  val.SetUint(std::stoul(argv[i]));
                  break;
               }
               case EGetOpt::dtUInt64:
               {
                  val.SetUint64(std::stoull(argv[i]));
                  break;
               }
               case EGetOpt::dtDouble:
               {
                  val.SetDouble(std::stod(argv[i]));
                  break;
               }
               case EGetOpt::dtBool:
               {
                  EString arg(argv[i]);
                  arg.tolower();
                  if (arg == "true" || arg == "yes")
                     val.SetBool(true);
                  else if (arg == "false" || arg == "no")
                     val.SetBool(false);
                  else
                     throw EGetOptError_UnsupportedBooleanValue(argv[i]);
                  break;
               }
               default:
               {
                  throw EGetOptError_UnsupportedDataType(opt->dataType);
                  break;
               }
               }

               if (cl.HasMember(key))
                  cl[key] = val;
               else
                  cl.AddMember(key, val, allocator);

               opt = NULL;

               break;
            }
            default:
            {
               throw EGetOptError_UnsupportedArgType(opt->argType);
               break;
            }
            }
         }
         else
         {
            opt = findOption(argv[i], options);
            key.SetString(argv[i], allocator);

            if (opt)
            {
               switch (opt->argType)
               {
               case no_argument:
               {
                  RAPIDJSON_NAMESPACE::Value val(true);
                  if (cl.HasMember(key))
                     cl[key] = val;
                  else
                     cl.AddMember(key, val, allocator);
                  opt = NULL;
                  break;
               }
               case required_argument:
               {
                  break;
               }
               default:
               {
                  throw EGetOptError_UnsupportedArgType(opt->argType);
                  break;
               }
               }
            }
            else
            {
               RAPIDJSON_NAMESPACE::Value &a = cl[ARGS];
               val.SetString(argv[i], allocator);
               a.PushBack(val, allocator);
            }
         }
      }
   }
}

Void EGetOpt::loadFile(cpStr filename)
{
   RAPIDJSON_NAMESPACE::Document &dst = ((_EGetOpt *)m_json)->s_json;
   RAPIDJSON_NAMESPACE::Document src;
   FILE *fp;
   Char buf[FILEBUFFER];

   fp = fopen(filename, "r");
   if (!fp)
      throw EGetOptError_FileParsing(filename);

   RAPIDJSON_NAMESPACE::FileReadStream is(fp, buf, sizeof(buf));
   src.ParseStream(is);
   fclose(fp);

   _merge(dst, src, dst.GetAllocator());
}