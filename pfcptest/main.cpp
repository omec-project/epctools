#include <stdlib.h>
#include <vector>
#include <set>
#include <signal.h>

#include "pfcpr15.h"
#include "epctools.h"
#include "epfcp.h"
#include "edir.h"

#include "test.h"
#include "pcaps/pcaps.h"

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

         // Load pcap tests
         std::set<EString> pcap_files;
         std::vector<EString> pcap_args;
         {
            EString pcaps_path = "./pcaps/originals";
            EDirectory pcaps_dir;
            cpStr pcap_file = pcaps_dir.getFirstEntry(pcaps_path, "*.pcap");
            while(pcap_file)
            {
               pcap_files.emplace(pcap_file);

               EString name = EPath::getFileNameWithoutExtension(pcap_file);
               pcap_args.push_back(name);
               TestSuite::add(name, PcapTest, &pcap_args.back());

               pcap_file = pcaps_dir.getNextEntry();
            }
         }

         EString results_path = "./pcaps/results/";
         EPath::verify(results_path);
         EDirectory dir;
         cpStr fn = dir.getFirstEntry(results_path, "*");
         while (fn)
         {
            EString file(fn);
            if(pcap_files.count(file) > 0)
            {
               EString file_path;
               EPath::combine(results_path, file, file_path);
               EUtility::delete_file(file_path);
            }
            fn = dir.getNextEntry();
         }

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