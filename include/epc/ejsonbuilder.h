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

#ifndef __ejsonbuilder_h_included
#define __ejsonbuilder_h_included

#include "estring.h"
#include "eerror.h"

#include <memory>

class EJsonBuilder
{
public:
    EJsonBuilder();
    ~EJsonBuilder();

    enum class ContainerType
    {
        Array,
        Object
    };

    template<typename T>
    class StackValue
    {
    public:
        StackValue(EJsonBuilder &builder, const T &value, const EString &name = "");
        ~StackValue();

    private:
        EJsonBuilder &m_builder;
        EString m_name;
    };

    using StackString = StackValue<EString>;
    using StackUInt = StackValue<UInt>;

    template<ContainerType T>
    class StackContainer
    {
    public:
        StackContainer(EJsonBuilder &builder, const EString &name = "");
        ~StackContainer();

    private:
        EJsonBuilder &m_builder;
        EString m_name;
    };

    using StackArray = StackContainer<ContainerType::Array>;
    using StackObject = StackContainer<ContainerType::Object>;

    Void push(ContainerType type);
    Void push(const EString &value);
    Void push(const UInt value);
    Void pop(const EString &name = "");

    cpStr toString();

private:
    class Impl;
    Impl &impl();
    std::unique_ptr<Impl> m_impl;
};

DECLARE_ERROR(EJsonBuilder_UnrecognizedType);
DECLARE_ERROR(EJsonBuilder_EmptyStack);
DECLARE_ERROR(EJsonBuilder_NonEmptyStack);
DECLARE_ERROR(EJsonBuilder_EmptyName);
DECLARE_ERROR(EJsonBuilder_NonEmptyName);

#endif // #ifndef __ejsonbuilder_h_included