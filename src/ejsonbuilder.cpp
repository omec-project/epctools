/*
* Copyright (c) 2020 T-Mobile
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

#include "ejsonbuilder.h"

#define RAPIDJSON_NAMESPACE epctoolsrapidjson
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

/// @cond DOXYGEN_EXCLUDE
class EJsonBuilder::Impl
{
public:
   Impl();

   Void init();
   Void push(ContainerType type);
   Void push(const EString &value);
   Void push(UInt value);
   Void pop(const EString &name = "");
   cpStr toString();

private:
   Void updateCurrValue();

   RAPIDJSON_NAMESPACE::Document m_doc;
   RAPIDJSON_NAMESPACE::StringBuffer m_buf;
   RAPIDJSON_NAMESPACE::Value *m_curr_value;
   std::vector<RAPIDJSON_NAMESPACE::Value> m_value_stack;
};

EJsonBuilder::Impl::Impl()
   : m_curr_value(&m_doc)
{
   init();
}

Void EJsonBuilder::Impl::init()
{
   m_doc.SetObject();
}

Void EJsonBuilder::Impl::push(ContainerType type)
{
   RAPIDJSON_NAMESPACE::Type valueType;
   switch (type)
   {
      case ContainerType::Array: valueType = RAPIDJSON_NAMESPACE::kArrayType; break;
      case ContainerType::Object: valueType = RAPIDJSON_NAMESPACE::kObjectType; break;
      default: throw EJsonBuilder_UnrecognizedType();
   }

   m_value_stack.emplace_back(valueType);
   updateCurrValue();
}

Void EJsonBuilder::Impl::push(const EString &value)
{
   m_value_stack.emplace_back();
   m_value_stack.back().SetString(value, m_doc.GetAllocator());
   updateCurrValue();
}

Void EJsonBuilder::Impl::push(UInt value)
{
   m_value_stack.emplace_back(value);
   updateCurrValue();
}

Void EJsonBuilder::Impl::pop(const EString &name)
{
   if (m_value_stack.empty())
      throw EJsonBuilder_EmptyStack();

   RAPIDJSON_NAMESPACE::Value &value = m_value_stack.back();
   RAPIDJSON_NAMESPACE::Value &container = (m_value_stack.size() > 1 ? *(m_value_stack.end() - 2) : m_doc);
   
   if (container.IsArray())
   {
      if (!name.empty())
         throw EJsonBuilder_NonEmptyName();

      container.PushBack(value, m_doc.GetAllocator());
   }
   else
   {
      if (name.empty())
         throw EJsonBuilder_EmptyName();
      
      container.AddMember(RAPIDJSON_NAMESPACE::Value(name, m_doc.GetAllocator()).Move(), value, m_doc.GetAllocator());
   }

   m_value_stack.pop_back();
   updateCurrValue();
}

Void EJsonBuilder::Impl::updateCurrValue()
{
   if (!m_value_stack.empty())
      m_curr_value = &m_value_stack.back();
   else
      m_curr_value = &m_doc;
}

cpStr EJsonBuilder::Impl::toString()
{
   if (!m_value_stack.empty())
      throw EJsonBuilder_NonEmptyStack();

   m_buf.Clear();
   RAPIDJSON_NAMESPACE::Writer<RAPIDJSON_NAMESPACE::StringBuffer> writer(m_buf);
   m_doc.Accept( writer );
   return m_buf.GetString();
}
/// @endcond

EJsonBuilder::EJsonBuilder()
   : m_impl(new EJsonBuilder::Impl())
{
}

EJsonBuilder::~EJsonBuilder() = default;

template<typename T>
EJsonBuilder::StackValue<T>::StackValue(EJsonBuilder &builder, const T &value, const EString &name)
   : m_builder(builder),
     m_name(name)
{
   m_builder.push(value);
   builder.pop(m_name);
}

// Explicit Instantiations
template class EJsonBuilder::StackValue<UInt>;
template class EJsonBuilder::StackValue<EString>;

template<EJsonBuilder::ContainerType T>
EJsonBuilder::StackContainer<T>::StackContainer(EJsonBuilder &builder, const EString &name)
   : m_builder(builder),
     m_name(name)
{
   m_builder.push(T);
}

template<EJsonBuilder::ContainerType T>
EJsonBuilder::StackContainer<T>::~StackContainer()
{
   m_builder.pop(m_name);
}

// Explicit Instantiations
template class EJsonBuilder::StackContainer<EJsonBuilder::ContainerType::Array>;
template class EJsonBuilder::StackContainer<EJsonBuilder::ContainerType::Object>;

Void EJsonBuilder::push(ContainerType type)
{
   impl().push(type);
}

Void EJsonBuilder::push(const EString &value)
{
   impl().push(value);
}

Void EJsonBuilder::push(UInt value)
{
   impl().push(value);
}

Void EJsonBuilder::pop(const EString &name)
{
   impl().pop(name);
}

cpStr EJsonBuilder::toString()
{
   return impl().toString();
}

EJsonBuilder::Impl &EJsonBuilder::impl() { return *m_impl.get(); }