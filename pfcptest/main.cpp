#include <stdlib.h>
#include <vector>
#include <signal.h>

#include "pfcpr15.h"
#include "epctools.h"
#include "epfcp.h"

#include "test.h"

#define LOG_SYSTEM 1
#define LOG_PFCP 2
#define LOG_TEST 3

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

      optfile.format("%s.json", argv[0]);
      if (EUtility::file_exists(optfile))
         opt.loadFile(optfile);

      optfile = opt.getCmdLine("-f,--file", "__unknown__");
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

         unsigned int failure_count = 0;

         for(auto itest : TestSuite::tests())
         {
            ELogger::log(LOG_TEST).info("Running test: {}", itest.first);
            bool result = TestSuite::run(itest.first);
            if(result)
            {
               ELogger::log(LOG_TEST).info("Test: {}....{}", itest.first, "PASSED");
            }
            else
            {
               ELogger::log(LOG_TEST).major("Test: {}....{}", itest.first, "FAILED");
               ++failure_count;
            }
         }

         if(failure_count > 0)
         {
            ELogger::log(LOG_TEST).major("{} tests failed.", failure_count);
            return 1;
         }
      }
      catch (const std::exception &e)
      {
         ELogger::log(LOG_SYSTEM).major(e.what());
      }

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