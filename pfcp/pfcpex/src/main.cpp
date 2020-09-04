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

#include <signal.h>
#include "epctools.h"
#include "epfcp.h"
#include "ejsonbuilder.h"

#include "pfcpex.h"
using namespace PFCP;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void signal_handler(int signal)
{
   ELogger::log(LOG_SYSTEM).startup( "Caught signal ({})", signal );

   switch (signal)
   {
      case SIGINT:
      case SIGTERM:
      {
         ELogger::log(LOG_SYSTEM).startup( "Setting shutdown event" );
         ExamplePfcpApplicationWorkGroup::setShutdownEvent();
         break;
      }
   }
}

Void init_signal_handler()
{
   {
      sigset_t sigset;

      /* mask SIGALRM in all threads by default */
      sigemptyset(&sigset);
      sigaddset(&sigset, SIGRTMIN);
      sigaddset(&sigset, SIGRTMIN + 2);
      sigaddset(&sigset, SIGRTMIN + 3);
      sigaddset(&sigset, SIGUSR1);
      sigprocmask(SIG_BLOCK, &sigset, NULL);
   }

   struct sigaction sa;

   /* Setup the signal handler */
   sa.sa_handler = signal_handler;

   /* Restart the system call, if at all possible */
   sa.sa_flags = SA_RESTART;

   /* Block every signal during the handler */
   sigfillset(&sa.sa_mask);

   if (sigaction(SIGINT, &sa, NULL) == -1)
      throw EError( EError::Warning, errno, "Unable to register SIGINT handler") ;
   ELogger::log(LOG_SYSTEM).startup( "signal handler registered for SIGINT" );

   if (sigaction(SIGTERM, &sa, NULL) == -1)
      throw EError( EError::Warning, errno, "Unable to register SIGTERM handler" );
   ELogger::log(LOG_SYSTEM).startup( "signal handler registered for SIGTERM" );

   if (sigaction(SIGRTMIN+1, &sa, NULL) == -1)
      throw EError( EError::Warning, errno, "Unable to register SIGRTMIN handler" );
   ELogger::log(LOG_SYSTEM).startup( "signal handler registered for SIGRTMIN" );
}

Void usage()
{
   std::cout << "USAGE:  pfcpex [--help] [--file optionfile]" << std::endl;
}

EString collectStats()
{
   EJsonBuilder builder;

   try
   {
      EJsonBuilder::StackString pushReportTime(builder, ETime::Now().Format("%i",True), "reporttime");

      std::vector<LocalNodeSPtr> localNodes;
      {
         ERDLock lck(CommunicationThread::Instance().localNodesLock());
         localNodes.reserve(CommunicationThread::Instance().localNodes().size());
         for (auto &iln : CommunicationThread::Instance().localNodes())
            localNodes.emplace_back(iln.second);
      }

      std::sort(localNodes.begin(), localNodes.end(),
         [](const LocalNodeSPtr &a, const LocalNodeSPtr &b) -> bool
         {
            return a->ipAddress().address().compare(b->ipAddress().address()) == 0;
         }
      );

      EJsonBuilder::StackArray pushLocalNodes(builder, "local_nodes");

      for (auto &localNodeSPtr : localNodes)
      {
         auto &localNode = *localNodeSPtr.get();

         EJsonBuilder::StackObject pushLocalNode(builder);
         EJsonBuilder::StackString pushLocalAddress(builder, localNode.ipAddress().address(), "local_address");

         std::vector<RemoteNodeSPtr> remoteNodes;
         {
            ERDLock lck(localNode.remoteNodesLock());
            remoteNodes.reserve(localNode.remoteNodes().size());
            for (auto &irn : localNode.remoteNodes())
               remoteNodes.emplace_back(irn.second);
         }

         std::sort(remoteNodes.begin(), remoteNodes.end(),
            [](const RemoteNodeSPtr &a, const RemoteNodeSPtr &b) -> bool
            {
               return a->ipAddress().address().compare(b->ipAddress().address()) == 0;
            }
         );

         EJsonBuilder::StackArray pushRemoteNodes(builder, "remote_nodes");

         for (auto &remoteNodeSPtr : remoteNodes)
         {
            auto &remoteNode = *remoteNodeSPtr.get();

            EJsonBuilder::StackObject pushRemoteNode(builder);
            EJsonBuilder::StackString pushRemoteAddress(builder, remoteNode.ipAddress().address(), "remote_address");            
            EJsonBuilder::StackString pushLastActivity(builder, remoteNode.stats().getLastActivity().Format("%i",True), "last_activity");

            std::vector<UInt> messages;
            {
               ERDLock lck(remoteNode.stats().getLock());
               for (auto imsg : remoteNode.stats().messageStats())
                  messages.push_back(imsg.first);
            }

            std::sort(messages.begin(), messages.end());

            EJsonBuilder::StackObject pushMessages(builder, "messages");
            for (auto msg : messages)
            {
               MessageStats *m = nullptr;
               {
                  ERDLock l(remoteNode.stats().getLock());
                  auto found = remoteNode.stats().messageStats().find(msg);
                  m = &found->second;
               }

               if (m == nullptr)
                  continue;

               EJsonBuilder::StackObject pushMsgObj(builder, m->getName());
               EJsonBuilder::StackUInt pushId(builder, m->getId(), "id");
               EJsonBuilder::StackUInt pushReceived(builder, m->getReceived(), "received");
               EJsonBuilder::StackUInt pushTimeout(builder, m->getTimeout(), "timeout");
               EJsonBuilder::StackArray pushSentArray(builder, "sent");
               for (auto sent : m->getSent())
                  EJsonBuilder::StackUInt pushSent(builder, sent);
            }
         }
      }
   }
   catch(std::exception &e)
   {
      ELogger::log(LOG_SYSTEM).major("Unhandled exception in collectStats()");
   }

   return builder.toString();
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
      if (opt.getCmdLine("-h,--help",false))
      {
         usage();
         return 0;
      }

      optfile.format("%s.json", argv[0]);
      if (EUtility::file_exists(optfile))
         opt.loadFile(optfile);

      optfile = opt.getCmdLine( "-f,--file", "__unknown__" );
      if (EUtility::file_exists(optfile))
         opt.loadFile(optfile);

      if (opt.getCmdLine( "-p,--print", false))
         opt.print();
   }
   catch(const std::exception& e)
   {
      std::cerr << e.what() << '\n';
      return 1;
   }

   try
   {
      EpcTools::Initialize(opt);
      ELogger::log(LOG_SYSTEM).startup("EpcTools initialization complete" );

      try
      {
         init_signal_handler();

         ExamplePfcpApplicationWorkGroup wg;

         wg.startup(opt);
         wg.waitForShutdown();
         EString stats = collectStats();
         ELogger::log(LOG_SYSTEM).info("Collected stats: {}", stats);
         wg.shutdown();
      }
      catch(const std::exception& e)
      {
         ELogger::log(LOG_SYSTEM).major( e.what() );
      }

      ELogger::log(LOG_SYSTEM).startup("Shutting down EpcTools" );
      EpcTools::UnInitialize();
   }
   catch(const std::exception& e)
   {
      std::cerr << e.what() << '\n';
      return 2;
   }

   return 0;
}