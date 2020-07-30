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

#ifndef __pfcptest_test_h_included
#define __pfcptest_test_h_included

#include <functional>
#include <map>

#include "estring.h"
#include "eerror.h"

#define TEST(name)                                   \
   bool name(Test &);                                \
   class name##_TEST                                 \
   {                                                 \
   public:                                           \
      name##_TEST()                                  \
      {                                              \
         std::unique_ptr<Test> test(new Test(name)); \
         TestSuite::add(#name, std::move(test));     \
      }                                              \
   };                                                \
   static name##_TEST name##_TEST_STATIC;            \
   bool name(Test &test)

#define CAST_TEST(name) *dynamic_cast<name *>(&test);

namespace PFCPTest
{
   DECLARE_ERROR_ADVANCED4(TestSuite_UnrecognizedTestName);
   DECLARE_ERROR_ADVANCED4(TestSuite_TestException);

   class Test
   {
   public:
      using Func = std::function<bool(Test &)>;

      Test() = default;
      Test(Func func) : m_func(func) {}

      virtual ~Test() {}

   private:
      friend class TestSuite;

      Func m_func = nullptr;
   };

   class TestSuite
   {
   public:
      using TestLookup = std::map<EString, std::unique_ptr<Test>>;

      static inline TestLookup &tests() { return s_tests; };
      static bool run(const EString &name);
      static void add(const EString &name, std::unique_ptr<Test> test);

   private:
      static TestLookup s_tests;
   };
} // namespace PFCPTest

#endif // #define __pfcptest_test_h_included