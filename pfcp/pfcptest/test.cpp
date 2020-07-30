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

#include "test.h"

namespace PFCPTest
{
   TestSuite_UnrecognizedTestName::TestSuite_UnrecognizedTestName(cpStr msg)
   {
      setTextf("Unrecognized test name (%s)", msg);
   }

   TestSuite_TestException::TestSuite_TestException(cpStr msg)
   {
      setTextf("Test threw an exception: (%s)", msg);
   }

   TestSuite::TestLookup TestSuite::s_tests;

   bool TestSuite::run(const EString &name)
   {
      auto test = s_tests.find(name);

      if (test == s_tests.end())
         throw TestSuite_UnrecognizedTestName(name.c_str());

      try
      {
         return test->second.get()->m_func(*test->second.get());
      }
      catch (const std::exception &e)
      {
         throw TestSuite_TestException(e.what());
      }
   }

   void TestSuite::add(const EString &name, std::unique_ptr<Test> test)
   {
      s_tests[name] = std::move(test);
   }
} // namespace PFCPTest