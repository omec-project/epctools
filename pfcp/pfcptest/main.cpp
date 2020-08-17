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

#include <stdlib.h>
#include <vector>
#include <signal.h>

#include "pfcpr15.h"
#include "epctools.h"
#include "epfcp.h"
#include "edir.h"

#include "test.h"
#include "pcaps/pcaps.h"

using namespace PFCPTest;

Void usage()
{
   std::cout << "USAGE:  pfcptest [--help] [--file optionfile]" << std::endl;
}

int main(int argc, char *argv[])
{
   EGetOpt::Option options[] = {
       {"-h", "--help", EGetOpt::no_argument, EGetOpt::dtNone},
       {"-f", "--file", EGetOpt::required_argument, EGetOpt::dtString},
       {"", "", EGetOpt::no_argument, EGetOpt::dtNone},
   };

   EGetOpt opt;
   EString optfile;

   try
   {
      opt.loadCmdLine(argc, argv, options);
      if (opt.getCmdLine("-h,--help", false))
      {
         usage();
         return 0;
      }

      optfile = opt.getCmdLine("-f,--file", "__unknown__");
      if (optfile.compare("__unknown__") == 0)
         optfile.format("%s.json", argv[0]);

      if (EUtility::file_exists(optfile))
         opt.loadFile(optfile);
   }
   catch (const std::exception &e)
   {
      std::cerr << e.what() << std::endl;
      return 1;
   }

   try
   {
      EpcTools::Initialize(opt);
      ELogger::log(LOG_SYSTEM).startup("EpcTools initialization complete");

      try
      {
         PFCP::Configuration::setLogger(ELogger::log(LOG_PFCP));

         unsigned int failureCount = 0;

         pcaps::InitTests();

         // Fill out the tests to run. If any tests are specified on the command line, use
         // them, otherwise, add all the tests from the test suite.
         std::vector<EString> tests;
         if (!opt.getCmdLineArgs().empty())
         {
            for (const auto &test : opt.getCmdLineArgs())
            {
               if (!TestSuite::contains(test))
                  ELogger::log(LOG_TEST).major("Unable to find test: {}", test);
               else
                  tests.push_back(test);
            }
         }
         else
         {
            for (const auto &test : TestSuite::tests())
               tests.push_back(test.first);
         }

         for (const auto &test : tests)
         {
            ELogger::log(LOG_TEST).info("Running test: {}", test);
            bool result = TestSuite::run(test);
            if (result)
            {
               ELogger::log(LOG_TEST).info("Test: {}....{}", test, "PASSED");
            }
            else
            {
               ELogger::log(LOG_TEST).major("Test: {}....{}", test, "FAILED");
               ++failureCount;
            }
         }

         ELogger::log(LOG_TEST).info("Ran {} tests", tests.size());
         if (failureCount > 0)
         {
            ELogger::log(LOG_TEST).major("{} tests failed", failureCount);
            return 1;
         }
      }
      catch (const std::exception &e)
      {
         ELogger::log(LOG_SYSTEM).major(e.what());
      }

      ELogger::log(LOG_PFCP).startup("Session counts created={} deleted={}",
         PFCP::SessionBase::sessionsCreated(), PFCP::SessionBase::sessionsDeleted());
      ELogger::log(LOG_PFCP).startup("Node counts created={} deleted={}",
         PFCP::Node::nodesCreated(), PFCP::Node::nodesDeleted());
      ELogger::log(LOG_SYSTEM).startup("Shutting down EpcTools");
      EpcTools::UnInitialize();
   }
   catch (const std::exception &e)
   {
      std::cerr << e.what() << std::endl;
      return 2;
   }

   return 0;
}