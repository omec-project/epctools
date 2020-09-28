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

/// @brief A class used to build JSON strings. It maintains a stack of JSON
///   objects which allows you to build a hierarchy of items. New objects are
///   added to the scope of the current object (i.e. the top of the stack). It 
///   provides methods to push and pop objects from the stack and helper classes 
///   to automatically manage the stack.
class EJsonBuilder
{
public:

    /// @brief Constructor. Initializes the document object used to contain 
    ///   all appended JSON objects.
    EJsonBuilder();

    /// @brief Destructor
    ~EJsonBuilder();

    /// @brief Enumeration for JSON containers types
    enum class ContainerType
    {
        /// @brief an array type []
        Array,
        /// @brief an object type with name/values {}
        Object
    };

    /// @brief A helper class which pushes/pops items on the builder's 
    ///   object stack based on its lifetime.
    template<typename T>
    class StackValue
    {
    public:
        /// @brief Constructs the helper class with the provided values. This
        ///   pushes and pops the value object onto/off the builder's stack.
        /// @param builder the builder to append to
        /// @param value the value to add
        /// @param name see EJsonBuilder::pop() for details
        /// @throw see EJsonBuilder::pop() for details
        StackValue(EJsonBuilder &builder, const T &value, const EString &name = "");

    private:
        EJsonBuilder &m_builder;
        EString m_name;
    };

    using StackString = StackValue<EString>;
    using StackUInt = StackValue<UInt>;

    /// @brief A helper class which pushes/pops items on the builder's 
    ///   object stack based on its lifetime.
    template<ContainerType T>
    class StackContainer
    {
    public:
        /// @brief Constructs the helper class with the provided container type. This
        ///   pushes the container object onto the builder's stack.
        /// @param builder the builder to append to
        /// @param name see EJsonBuilder::pop() for details
        StackContainer(EJsonBuilder &builder, const EString &name = "");

        /// @brief Destructor. Pops the container object off the builder's stack.
        /// @throw see EJsonBuilder::pop() for details
        ~StackContainer();

    private:
        EJsonBuilder &m_builder;
        EString m_name;
    };

    using StackArray = StackContainer<ContainerType::Array>;
    using StackObject = StackContainer<ContainerType::Object>;

    /// @brief Pushes a container type object onto the stack
    /// @param type the type of container object to push
    /// @throw EJsonBuilder_UnrecognizedType
    Void push(ContainerType type);

    /// @brief Pushes a string object onto the stack
    /// @param value the value of the string object
    Void push(const EString &value);

    /// @brief Pushes an unsigned integer value onto the stack
    /// @param value the value of the unsigned integer object
    Void push(const UInt value);

    /// @brief Pops the top object off the stack.
    /// @param name the name to use when adding this object to a container object 
    ///   on the stack. If the current container object is an array, then the name
    ///   shall be empty, otherwise, it is required to be non-empty.
    /// @throw EJsonBuilder_EmptyStack
    /// @throw EJsonBuilder_NonEmptyName
    /// @throw EJsonBuilder_EmptyName
    Void pop(const EString &name = "");

    /// @brief Returns a string representation of the json objects
    /// @return a constant pointer to the character buffer held by this object
    /// @throw EJsonBuilder_NonEmptyStack 
    cpStr toString();

private:
    class Impl;
    Impl &impl();
    std::unique_ptr<Impl> m_impl;
};

/// @cond DOXYGEN_EXCLUDE
DECLARE_ERROR(EJsonBuilder_UnrecognizedType);
DECLARE_ERROR(EJsonBuilder_EmptyStack);
DECLARE_ERROR(EJsonBuilder_NonEmptyStack);
DECLARE_ERROR(EJsonBuilder_EmptyName);
DECLARE_ERROR(EJsonBuilder_NonEmptyName);
/// @endcond

#endif // #ifndef __ejsonbuilder_h_included