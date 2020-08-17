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

#ifndef __pfcptest_pcaps_pcaps_h_included
#define __pfcptest_pcaps_pcaps_h_included

#include "test.h"

namespace PFCPTest
{
   namespace pcaps
   {
      void InitTests();
      bool RunPcapTest(Test &test);

      class PcapTest : public Test
      {
      public:
         PcapTest(const EString &name) : Test(RunPcapTest, name) {}
      };
   } // namespace pcaps
} // namespace PFCPTest

#endif // #define __pfcptest_pcaps_pcaps_h_included