/*
* Copyright (c) 2009-2019 Brian Waters
* Copyright (c) 2019 Sprint
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

#include "stdio.h"
#include <iostream>
#include <locale>
#include <array>
#include <memory.h>
#include <signal.h>

#include "epc/epctools.h"
//#include "epc/ethread.h"
#include "epc/etevent.h"
#include "epc/esocket.h"
#include "epc/einternal.h"

#include "epc/emgmt.h"
#include "epc/etimerpool.h"

#include "epc/epcdns.h"

#include "epc/eip.h"
#include "epc/eostring.h"
#include "epc/eteid.h"

std::locale defaultLocale;
std::locale mylocale;

Void ECircularBuffer_test()
{
   pUChar pData = (pUChar) "[1234567890123456789012345678]";
   UChar buf[256];

   memset(buf, 0, sizeof(buf));

   cout << "ECircularBuffer_test() Start" << endl;

   ECircularBuffer cb(50);

   for (int i = 0; i < 20; i++)
   {
      cout << "Pass " << i << " writing 30 bytes...";
      cb.writeData(pData, 0, 30);
      cout << " reading 30 bytes...";
      cb.readData(buf, 0, 30);
      cout << buf << endl;
   }

   cout << "ECircularBuffer_test() Complete" << endl;
}

Void ETimer_test()
{
   cout << "ETimer_test() Start" << endl;

   ETimer t;

   t.Start();

   for (int i = 0; i < 1000000; i++)
      ;

   //t.Stop();

   epctime_t a = t.MicroSeconds();
   epctime_t b = t.MilliSeconds();

   cout << "MicroSeconds() = " << a << endl;
   cout << "MilliSeconds() = " << b << endl;

   cout << "ETimer_test() Complete" << endl;
}

Void EMutexPrivate_test()
{
   cout << "EMutexPrivate_test() Start" << endl;
   cout << "Creating private mutex" << endl;
   EMutexPrivate m;
   {
      EMutexLock l(m);
      cout << "Mutex locked.  Press [Enter] to continue ...";
      getc(stdin);
      cout << endl
           << "Destroying private mutex" << endl;
   }

   cout << "EMutex_test() Complete" << endl;
}

Void EMutexPublic_test()
{
   cout << "EMutexPublic_test() Start" << endl;
   {
      cout << "Creating 1st public mutex" << endl;
      EMutexPublic m;
      {
         cout << "1st Mutex ID = " << m.mutexId() << endl;
         EMutexLock l(m);
         cout << "1st Mutex locked.  Press [Enter] to continue ...";
         getc(stdin);
         cout << endl
              << "Destroying 1st public mutex" << endl;
      }
   }

   {
      cout << "Creating 2nd public mutex" << endl;
      EMutexPublic m;
      {
         cout << "2nd Mutex ID = " << m.mutexId() << endl;
         EMutexLock l(m);
         cout << "2nd Mutex locked.  Press [Enter] to continue ...";
         getc(stdin);
         cout << endl
              << "Destroying 2nd public mutex" << endl;
      }
   }

   cout << "EMutex_test() Complete" << endl;
}

Void ESemaphorePrivate_test()
{
   cout << "ESemaphorePrivate_test() Start" << endl;

   {
      cout << "Creating semaphore initial/max count of 5" << endl;
      ESemaphorePrivate s1(5);
      cout << "Decrementing";
      for (int i = 1; i <= 5; i++)
      {
         if (!s1.Decrement())
            cout << "Error decrementing semaphore on pass " << i << endl;
         cout << ".";
      }
      cout << "Checking for decrement action at zero...";
      if (s1.Decrement(False))
         cout << " failed - Decrement returned true" << endl;
      else
         cout << "success" << endl;
   }

   cout << "ESemaphorePrivate_test() Complete" << endl;
}

Void ESemaphorePublic_test()
{
   cout << "ESemaphorePublic_test() Start" << endl;

   {
      cout << "Creating semaphore initial/max count of 5" << endl;
      ESemaphorePublic s1(5);
      cout << "Decrementing";
      for (int i = 1; i <= 5; i++)
      {
         if (!s1.Decrement())
            cout << "Error decrementing semaphore on pass " << i << endl;
         cout << ".";
      }
      cout << "Checking for decrement action at zero...";
      if (s1.Decrement(False))
         cout << " failed - Decrement returned true" << endl;
      else
         cout << "success" << endl;
   }

   cout << "ESemaphorePublic_test() Complete" << endl;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define EM_MUTEXTEST (EM_USER + 1)

class EThreadMutexTest2 : public EThreadPrivate
{
public:
   EThreadMutexTest2(EMutexData &m1, int loop)
       : m_mutex1(m1),
         m_loop(loop)
   {
   }

   virtual Void onInit()
   {
      m_timer.Start();

      for (int i = 0; i < m_loop; i++)
      {
         EMutexLock l1(m_mutex1);
      }

      m_timer.Stop();

      quit();
   }

   virtual Void onQuit()
   {
      EThreadPrivate::onQuit();
      cout << "EThreadMutexTest2 elapsed time " << m_timer.MicroSeconds() - 1000000 << " microseconds" << endl;
   }

   DECLARE_MESSAGE_MAP()

private:
   EThreadMutexTest2();

   ETimer m_timer;
   EMutexData &m_mutex1;
   int m_loop;
};

BEGIN_MESSAGE_MAP(EThreadMutexTest2, EThreadPrivate)
END_MESSAGE_MAP()

Void EMutex_test2()
{
   cout << "EMutex_test2() Start" << endl;

   int loop = 1000000;
   EMutexPrivate m1;

   EThreadMutexTest2 t1(m1, loop);
   EThreadMutexTest2 t2(m1, loop);

   {
      EMutexLock l1(m1);

      t1.init(1, 1, NULL, 200000);
      t2.init(1, 2, NULL, 200000);

      cout << "Sleeping for 1 sercond ... ";
      EThreadBasic::sleep(1000);
   }

   cout << "test started" << endl;

   t1.join();
   t2.join();

   cout << "EMutex_test2() Complete" << endl;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class EThreadBasicCancelWaitTest : public EThreadBasic
{
public:
   Bool decrementResult;
   ESemaphorePrivate runningSemaphore;

   EThreadBasicCancelWaitTest()
   {
      decrementResult = False;
   }

   Dword threadProc(Void *arg)
   {
      ESemaphorePrivate s;
      runningSemaphore.Increment();
      try
      {
         decrementResult = s.Decrement();
         cout << "after the decrement in the test thread" << endl;
      }
      catch (EError &e)
      {
         cout << e << endl;
      }
      catch (std::exception &ex)
      {
         cout << "an exception occurred - " << ex.what() << endl;
         throw;
      }
      return 0;
   }
};

Void EThread_cancel_wait()
{
   cout << "EThread_cancel_wait() Start" << endl;

   EThreadBasicCancelWaitTest t;

   t.init((Void *)"this is the thread argument\n", true);
   //cout << "before resume" << endl;
   //t.resume();
   cout << "waiting for test thread to start" << endl;
   t.runningSemaphore.Decrement();
   Int x = 10000;
   cout << "sleeping for " << x / 1000 << " seconds" << endl;
   t.sleep(x);
   cout << "before cancelWait()" << endl;
   cout << "cancelWait() returned " << t.cancelWait() << endl;
   cout << "before join" << endl;
   t.join();

   if (t.decrementResult)
      cout << "The cancel wait test semaphore decrement returned True (error)" << endl;
   else
      cout << "The cancel wait test semaphore decrement returned False (success)" << endl;

   cout << "EThread_cancel_wait() Complete" << endl;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Void ESemaphore_test_cancel_wait()
{
   cout << "ESemaphore_test() Start" << endl;

   {
      cout << "Creating semaphore initial/max count of 5" << endl;
      ESemaphorePrivate s1(5);
      cout << "Decrementing";
      for (int i = 1; i <= 5; i++)
      {
         if (s1.Decrement() < 0)
            cout << "Error decrementing semaphore on pass " << i << endl;
         cout << ".";
      }
      cout << "Checking for decrement action at zero...";
      if (s1.Decrement(False))
         cout << " failed - Decrement returned true" << endl;
      else
         cout << "success" << endl;
   }

   cout << "ESemaphore_test() Complete" << endl;
}

Void EError_test()
{
   cout << "EError_test() Start" << endl;

   cout << "Creating error object" << endl;
   EError e1;
   e1.appendTextf("error object #%d", 1);
   cout << e1 << endl;

   cout << "EError_test() Complete" << endl;
}

class EThreadBasicTest : public EThreadBasic
{
public:
   EThreadBasicTest() : m_timetoquit(false) {}

   Dword threadProc(Void *arg)
   {
      while (!m_timetoquit)
      {
         cout << "Inside the thread [" << (cpStr)arg << "]" << endl;
         sleep(1000);
      }
      cout << "Exiting EThreadBasicTest::threadProc()" << endl;
      return 0;
   }

   Void setTimeToQuit()
   {
      m_timetoquit = true;
   }

private:
   bool m_timetoquit;
};

Void EThreadBasic_test()
{
   cout << "EThread_test() Start" << endl;

   EThreadBasicTest t;

   cout << "initialize and start the thread" << endl;
   t.init((Void *)"this is the thread argument");
   cout << "sleep for 5 seconds" << endl;
   t.sleep(5000);
   cout << "call setTimeToQuit()" << endl;
   t.setTimeToQuit();
   cout << "wait for thread to exit (join)" << endl;
   t.join();

   cout << "EThread_test() Complete" << endl;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define EM_USER1 (EM_USER + 1)
#define EM_USER2 (EM_USER + 2)
#define EM_USER3 (EM_USER + 3)
#define EM_USER4 (EM_USER + 4)

class EThreadTest4 : public EThreadPrivate
{
public:
   EThreadTest4(int mm)
   {
      msg1snt = 0;
      msg1rcv = 0;
      msgmax = mm;
   }

   Void userFunc1(EThreadMessage &msg)
   {
      msg1rcv++;
      if (msg1rcv == msgmax)
         quit();
      else if (msg1snt < msgmax)
         sendMessage(EThreadMessage(EM_USER1, 0, (Long)msg1snt++));
   }

   Void onInit()
   {
      printf("Inside onInit()\n");

      te.Start();

      //sendMessage(EM_USER1, 0, (Long)msg1snt++);
      sendMessage(EThreadMessage(EM_USER1, 0, (Long)msg1snt++));
   }

   virtual Void onQuit()
   {
      te.Stop();

      EThreadPrivate::onQuit();
      cout << "EThreadTest4 sent     " << msg1snt << " EM_USER1 messages" << endl;
      cout << "EThreadTest4 received " << msg1rcv << " EM_USER1 messages" << endl;
   }

   Void onSuspend()
   {
      cout << "Inside onSuspend()" << endl;
      EThreadPrivate::onSuspend();
   }

   ETimer &getTimer() { return te; }

   DECLARE_MESSAGE_MAP()

private:
   EThreadTest4();

   int msg1snt;
   int msg1rcv;
   int msgmax;
   ETimer te;
};

class EThreadTest : public EThreadPublic
{
public:
   EThreadTest(EMutexPublic &mutex)
       : mtx(mutex)
   {
      msg1cnt = 0;
      msg2cnt = 0;
   }

   Void userFunc1(EThreadMessage &msg)
   {
      msg1cnt++;
   }

   Void userFunc2(EThreadMessage &msg)
   {
      msg2cnt++;
   }

   //	Void onInit()
   //	{
   //		printf("Inside onInit()\n");
   //	}

   virtual Void onQuit()
   {
      EThreadPublic::onQuit();
      EMutexLock l(mtx);
      cout << "EThreadTest received " << msg1cnt << " EM_USER1 messages" << endl;
      cout << "EThreadTest received " << msg2cnt << " EM_USER2 messages" << endl;
   }

   Void onSuspend()
   {
      cout << "Inside onSuspend()" << endl;
      EThreadPublic::onSuspend();
   }

   DECLARE_MESSAGE_MAP()

private:
   EMutexPublic &mtx;
   int msg1cnt;
   int msg2cnt;

   EThreadTest();
};

class EThreadTest2 : public EThreadTest
{
public:
   EThreadTest2(EMutexPublic &mutex)
       : EThreadTest(mutex), mtx(mutex)
   {
      msg3cnt = 0;
      msg4cnt = 0;
   }

   Void userFunc3(EThreadMessage &msg)
   {
      msg3cnt++;
   }

   Void userFunc4(EThreadMessage &msg)
   {
      msg4cnt++;
   }

   virtual Void onQuit()
   {
      EThreadTest::onQuit();
      EMutexLock l(mtx);
      cout << "EThreadTest2 received " << msg3cnt << " EM_USER3 messages" << endl;
      cout << "EThreadTest2 received " << msg4cnt << " EM_USER4 messages" << endl;
   }

   DECLARE_MESSAGE_MAP()

private:
   EMutexPublic &mtx;
   int msg3cnt;
   int msg4cnt;

   EThreadTest2();
};

class EThreadTest3 : public EThreadPrivate
{
public:
   EThreadTest3()
   {
      msg1cnt = 0;
      msg2cnt = 0;
      msg3cnt = 0;
      msg4cnt = 0;
   }

   Void userFunc1(EThreadMessage &msg)
   {
      msg1cnt++;
   }

   Void userFunc2(EThreadMessage &msg)
   {
      msg2cnt++;
   }

   Void userFunc3(EThreadMessage &msg)
   {
      msg3cnt++;
   }

   Void userFunc4(EThreadMessage &msg)
   {
      msg4cnt++;
   }

   virtual Void onQuit()
   {
      EThreadPrivate::onQuit();
      cout << "EThreadTest3 received " << msg1cnt << " EM_USER1 messages" << endl;
      cout << "EThreadTest3 received " << msg2cnt << " EM_USER2 messages" << endl;
      cout << "EThreadTest3 received " << msg3cnt << " EM_USER3 messages" << endl;
      cout << "EThreadTest3 received " << msg4cnt << " EM_USER4 messages" << endl;
   }

   DECLARE_MESSAGE_MAP()

private:
   int msg1cnt;
   int msg2cnt;
   int msg3cnt;
   int msg4cnt;
};

BEGIN_MESSAGE_MAP(EThreadTest, EThreadPublic)
   ON_MESSAGE(EM_USER1, EThreadTest::userFunc1)
   ON_MESSAGE(EM_USER2, EThreadTest::userFunc2)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(EThreadTest2, EThreadTest)
   ON_MESSAGE(EM_USER3, EThreadTest2::userFunc3)
   ON_MESSAGE(EM_USER4, EThreadTest2::userFunc4)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(EThreadTest3, EThreadPrivate)
   ON_MESSAGE(EM_USER1, EThreadTest3::userFunc1)
   ON_MESSAGE(EM_USER2, EThreadTest3::userFunc2)
   ON_MESSAGE(EM_USER3, EThreadTest3::userFunc3)
   ON_MESSAGE(EM_USER4, EThreadTest3::userFunc4)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(EThreadTest4, EThreadPrivate)
   ON_MESSAGE(EM_USER1, EThreadTest4::userFunc1)
END_MESSAGE_MAP()

template <class T>
std::string numberFormatWithCommas(T value, int precision = 6)
{
   struct Numpunct : public std::numpunct<char>
   {
   protected:
      virtual char do_thousands_sep() const { return ','; }
      virtual std::string do_grouping() const { return "\03"; }
   };
   std::stringstream ss;
   ss.imbue({std::locale(), new Numpunct});
   ss << std::setprecision(precision) << std::fixed << value;
   return ss.str();
}

Void EThread_test4()
{
   int maxmsg = 100000000;

   EThreadTest4 t1(maxmsg);

   t1.init(1, 1, NULL, 200000);

   EThreadBasic::yield();

   t1.join();

   double persec = ((double)maxmsg) / (((double)t1.getTimer().MicroSeconds()) / 1000000);
   std::string s = numberFormatWithCommas<double>(persec);
   cout << "Processed " << maxmsg << " messages in " << ((double)t1.getTimer().MicroSeconds()) / 1000000 << " seconds (" << s << " per second)" << endl;
   //" seconds (" << (((double)maxmsg) / (((double)te.MicroSeconds())/1000000)) << " per second)" << endl;
}

class EThreadTest5rcv : public EThreadPrivate
{
public:
   EThreadTest5rcv(int senders)
   {
      msg1rcv = 0;
      msg2rcv = 0;
      msg3rcv = 0;
      msgsenders = senders;
      maxpending = 0;
   }

   Void rcv1(EThreadMessage &msg)
   {
      msg1rcv++;

      if (msg1rcv == 1)
         te.Start();
   }

   Void rcv2(EThreadMessage &msg)
   {
      Long pending = getMsgSemaphore().currCount();
      if (pending > maxpending)
         maxpending = pending;

      msg2rcv++;

      if (msg2rcv % 1000000 == 0)
         cout << "\rEThreadTest5 received " << msg2rcv << " max pending " << maxpending << flush;
   }

   Void rcv3(EThreadMessage &msg)
   {
      msg3rcv++;

      if (msg3rcv == msgsenders)
      {
         te.Stop();
         quit();
      }
   }

   virtual Void onQuit()
   {
      EThreadPrivate::onQuit();
      cout << "\rEThreadTest5rcv received " << msg2rcv << " EM_USER2 messages with max pending of " << maxpending << endl
           << flush;
   }

   ETimer &getTimer() { return te; }

   DECLARE_MESSAGE_MAP()

private:
   EThreadTest5rcv();

   int msg1rcv;
   int msg2rcv;
   int msg3rcv;
   int msgsenders;
   Long maxpending;
   ETimer te;
};

BEGIN_MESSAGE_MAP(EThreadTest5rcv, EThreadPrivate)
ON_MESSAGE(EM_USER1, EThreadTest5rcv::rcv1)
ON_MESSAGE(EM_USER2, EThreadTest5rcv::rcv2)
ON_MESSAGE(EM_USER3, EThreadTest5rcv::rcv3)
END_MESSAGE_MAP()

class EThreadTest5snd : public EThreadBasic
{
public:
   EThreadTest5snd(EThreadTest5rcv *receiver, int cnt)
   {
      msgreceiver = receiver;
      msgcnt = cnt;
      msgsnt = 0;
   }

   Dword threadProc(Void *arg)
   {
      msgreceiver->sendMessage(EM_USER1);

      for (msgsnt = 0; msgsnt < msgcnt; msgsnt++)
      {
         if (!msgreceiver->sendMessage(EM_USER2))
            cout << endl
                 << "sendMessage returned FALSE for message # " << msgsnt << endl
                 << flush;
      }

      msgreceiver->sendMessage(EM_USER3);

      return 0;
   }

   int getMessagesSent() { return msgsnt; }

private:
   EThreadTest5rcv *msgreceiver;
   int msgcnt;
   int msgsnt;
};

Void EThread_test5()
{
   static Int nSenders = 1;
   static Int nMessages = 1000000;
   Char buffer[128];
   EThreadTest5snd **sendthrds = NULL;

   cout << "Enter number of sending threads [" << nSenders << "]: ";
   cin.getline(buffer, sizeof(buffer));
   nSenders = buffer[0] ? atoi(buffer) : nSenders;

   cout << "Enter message count [" << nMessages << "]: ";
   cin.getline(buffer, sizeof(buffer));
   nMessages = buffer[0] ? atoi(buffer) : nMessages;

   EThreadTest5rcv t1(nSenders);

   t1.init(1, 1, NULL, 200000);

   sendthrds = new EThreadTest5snd *[nSenders];

   for (int i = 0; i < nSenders; i++)
   {
      sendthrds[i] = new EThreadTest5snd(&t1, nMessages);
      sendthrds[i]->init(NULL);
   }

   EThreadBasic::yield();

   t1.join();
   for (int i = 0; i < nSenders; i++)
   {
      sendthrds[i]->join();
      delete sendthrds[i];
   }
   delete[] sendthrds;

   double persec = ((double)(nSenders * nMessages)) / (((double)t1.getTimer().MicroSeconds()) / 1000000);
   std::string s = numberFormatWithCommas<double>(persec);
   cout << "Processed " << (nSenders * nMessages) << " messages in " << ((double)t1.getTimer().MicroSeconds()) / 1000000 << " seconds (" << s << " per second)" << endl;
}

Void EThread_test()
{
   EMutexPublic mtx;
   EThreadTest2 t1(mtx);

   t1.init(1, 1, NULL, 200000);
   EThreadBasic::yield();

   ETimer te;

   te.Start();
   int i;

   for (i = 0; i < 4000000; i++)
   {
      t1.sendMessage(EThreadMessage(EM_USER1, 0, (Long)i));
      t1.sendMessage(EThreadMessage(EM_USER2, 0, (Long)i));
      t1.sendMessage(EThreadMessage(EM_USER3, 0, (Long)i));
      t1.sendMessage(EThreadMessage(EM_USER4, 0, (Long)i));
      //        EThreadBasic::yield();
   }

   t1.quit();
   t1.join();

   i *= 4;
   cout << "Processed " << i << " messages in " << ((double)te.MicroSeconds()) / 1000000 << " seconds (" << ((double)i) / (((double)te.MicroSeconds()) / 1000000) << " per second)" << endl;
}

Void EThread_test2()
{
   EMutexPublic mtx;
   EThreadTest2 t1(mtx);
   EThreadTest2 t2(mtx);
   EThreadTest2 t3(mtx);
   EThreadTest2 t4(mtx);

   t1.init(1, 1, NULL, 200000);
   t2.init(1, 2, NULL, 200000);
   t3.init(1, 3, NULL, 200000);
   t4.init(1, 4, NULL, 200000);
   t1.yield();

   ETimer te;

   te.Start();
   int i;

   for (i = 0; i < 1000000; i++)
   {
      for (int i = 0; i < 4; i++)
      {
         try
         {
            switch (i)
            {
            case 0:
               t1.sendMessage(EThreadMessage(EM_USER1, 0, (Long)i));
               t2.sendMessage(EThreadMessage(EM_USER1, 0, (Long)i));
               t3.sendMessage(EThreadMessage(EM_USER1, 0, (Long)i));
               t4.sendMessage(EThreadMessage(EM_USER1, 0, (Long)i));
               break;
            case 1:
               t1.sendMessage(EThreadMessage(EM_USER2, 0, (Long)i));
               t2.sendMessage(EThreadMessage(EM_USER2, 0, (Long)i));
               t3.sendMessage(EThreadMessage(EM_USER2, 0, (Long)i));
               t4.sendMessage(EThreadMessage(EM_USER2, 0, (Long)i));
               break;
            case 2:
               t1.sendMessage(EThreadMessage(EM_USER3, 0, (Long)i));
               t2.sendMessage(EThreadMessage(EM_USER3, 0, (Long)i));
               t3.sendMessage(EThreadMessage(EM_USER3, 0, (Long)i));
               t4.sendMessage(EThreadMessage(EM_USER3, 0, (Long)i));
               break;
            case 3:
               t1.sendMessage(EThreadMessage(EM_USER4, 0, (Long)i));
               t2.sendMessage(EThreadMessage(EM_USER4, 0, (Long)i));
               t3.sendMessage(EThreadMessage(EM_USER4, 0, (Long)i));
               t4.sendMessage(EThreadMessage(EM_USER4, 0, (Long)i));
               break;
            }
         }
         catch (EError &e)
         {
            cout << "Processing exception " << e.Name() << " - " << e << endl;
            throw;
         }
      }
      //        EThreadBasic::yield();
   }

   t1.quit();
   t2.quit();
   t3.quit();
   t4.quit();
   t1.join();
   t2.join();
   t3.join();
   t4.join();

   i *= 16;
   cout << "Processed " << i << " messages in " << ((double)te.MicroSeconds()) / 1000000 << " seconds (" << ((double)i) / (((double)te.MicroSeconds()) / 1000000) << " per second)" << endl;
}

Void EThread_test3()
{
   EThreadTest3 t1;
   EThreadTest3 t2;
   EThreadTest3 t3;
   EThreadTest3 t4;

   t1.init(1, 1, NULL, 200000);
   t2.init(1, 2, NULL, 200000);
   t3.init(1, 3, NULL, 200000);
   t4.init(1, 4, NULL, 200000);
   t1.yield();

   ETimer te;

   te.Start();
   int i;

   for (i = 0; i < 1000000; i++)
   {
      for (int i = 0; i < 4; i++)
      {
         try
         {
            switch (i)
            {
            case 0:
               t1.sendMessage(EThreadMessage(EM_USER1, 0, (Long)i));
               t2.sendMessage(EThreadMessage(EM_USER1, 0, (Long)i));
               t3.sendMessage(EThreadMessage(EM_USER1, 0, (Long)i));
               t4.sendMessage(EThreadMessage(EM_USER1, 0, (Long)i));
               break;
            case 1:
               t1.sendMessage(EThreadMessage(EM_USER2, 0, (Long)i));
               t2.sendMessage(EThreadMessage(EM_USER2, 0, (Long)i));
               t3.sendMessage(EThreadMessage(EM_USER2, 0, (Long)i));
               t4.sendMessage(EThreadMessage(EM_USER2, 0, (Long)i));
               break;
            case 2:
               t1.sendMessage(EThreadMessage(EM_USER3, 0, (Long)i));
               t2.sendMessage(EThreadMessage(EM_USER3, 0, (Long)i));
               t3.sendMessage(EThreadMessage(EM_USER3, 0, (Long)i));
               t4.sendMessage(EThreadMessage(EM_USER3, 0, (Long)i));
               break;
            case 3:
               t1.sendMessage(EThreadMessage(EM_USER4, 0, (Long)i));
               t2.sendMessage(EThreadMessage(EM_USER4, 0, (Long)i));
               t3.sendMessage(EThreadMessage(EM_USER4, 0, (Long)i));
               t4.sendMessage(EThreadMessage(EM_USER4, 0, (Long)i));
               break;
            }
         }
         catch (EError &e)
         {
            cout << "Processing exception " << e.Name() << " - " << e << endl;
            throw;
         }
      }
      //        EThreadBasic::yield();
   }

   t1.quit();
   t2.quit();
   t3.quit();
   t4.quit();
   t1.join();
   t2.join();
   t3.join();
   t4.join();

   i *= 16;
   cout << "Processed " << i << " messages in " << ((double)te.MicroSeconds()) / 1000000 << " seconds (" << ((double)i) / (((double)te.MicroSeconds()) / 1000000) << " per second)" << endl;
}

void ESharedMemory_test()
{
   ESharedMemory m("test", 1, 1024 * 1024);
}

class testmessage : public EQueueMessage
{
public:
   testmessage()
   {
      epc_strcpy_s(m_data, sizeof(m_data),
                   "This is a shared queue test. Four score and 7 years ago, our fathers");
   }

   ~testmessage() {}

   virtual Void getLength(ULong &length)
   {
      EQueueMessage::getLength(length);
      elementLength(m_data, length);
   }
   virtual Void serialize(pVoid pBuffer, ULong &nOffset)
   {
      EQueueMessage::serialize(pBuffer, nOffset);
      pack(m_data, pBuffer, nOffset);
   }
   virtual Void unserialize(pVoid pBuffer, ULong &nOffset)
   {
      EQueueMessage::unserialize(pBuffer, nOffset);
      unpack(m_data, pBuffer, nOffset);
   }

   Char m_data[128];
};

class TestPublicQueue : public EQueuePublic
{
public:
   EQueueMessage *allocMessage(Long msgType)
   {
      return &msg;
   }

   testmessage &getMessage()
   {
      return msg;
   }

private:
   testmessage msg;
};

void EQueuePublic_test(Bool bWriter)
{
   static Int nQueueId = 1;
   static Int nMsgCnt = 100000;
   Char buffer[128];
   TestPublicQueue q;

   cout << "Enter Queue Id [" << nQueueId << "]: ";
   cin.getline(buffer, sizeof(buffer));
   nQueueId = buffer[0] ? atoi(buffer) : nQueueId;

   cout << "Enter message count [" << nMsgCnt << "]: ";
   cin.getline(buffer, sizeof(buffer));
   nMsgCnt = buffer[0] ? atoi(buffer) : nMsgCnt;

   try
   {
      q.init(nQueueId, bWriter ? EQueueBase::WriteOnly : EQueueBase::ReadOnly);
   }
   catch (EError &e)
   {
      cout << e.Name() << " - " << e << endl;
      return;
   }

   testmessage msg;
   ETimer te;

   Int cnt = 0;

   if (bWriter)
   {
      // writer
      for (cnt = 0; cnt < nMsgCnt; cnt++)
         q.push(msg);
   }
   else
   {
      // reader
      while (cnt < nMsgCnt)
      {
         cnt++;
         q.pop();
      }
   }

   cout << "Processed " << cnt << " messages in " << ((double)te.MicroSeconds()) / 1000000
        << " seconds (" << ((double)cnt) / (((double)te.MicroSeconds()) / 1000000) << " per second)" << endl;
}

#if 0
#define LOG_MASK_1 0x0000000000000001
#define LOG_MASK_2 0x0000000000000002
#define LOG_MASK_3 0x0000000000000004
#define LOG_MASK_4 0x0000000000000008

Void ELogger_test()
{
    cout << "ELogger_test start" << endl;

    ELogger::setGroupMask(1, 0);
    ELogger::setGroupMask(2, 0);

    ELogger::logError(1, LOG_MASK_1, "ELogger_test", "This line should not be logged, because it is not enabled.");
    ELogger::enableGroupMask(1, LOG_MASK_1);
    ELogger::logError(1, LOG_MASK_1, "ELogger_test", "This line should be logged.");

    cout << "Testing single file log rollover." << endl;
    EString s;
    Int i;
    for (i = 0; i<19; i++)
        ELogger::logInfo(1, LOG_MASK_1, "ELogger_test", "LOG_MASK_1 Line %d", i + 1);
    cout << "  Review log file 1.  There should be 10 lines in the log file, numbers 10-19." << endl;

    ELogger::disableGroupMask(1, LOG_MASK_1);
    ELogger::logInfo(1, LOG_MASK_1, "ELogger_test", "This line should not be logged because it has been disabled.");

    cout << "Testing log rollover with 2 files." << endl;
    ELogger::enableGroupMask(2, LOG_MASK_2);
    for (i = 0; i<20; i++)
        ELogger::logInfo(2, LOG_MASK_2, "ELogger_test", "LOG_MASK_2 Line %d", i + 1);
    cout << "  Review log file 2.  There should be 2 segments with 10 lines each." << endl;

    cout << "Testing syslog." << endl;
    ELogger::enableGroupMask(3, LOG_MASK_2);
    ELogger::logInfo(3, LOG_MASK_2, "ELogger_test", "Info message");
    ELogger::logWarning(3, LOG_MASK_2, "ELogger_test", "Warning message");
    ELogger::logError(3, LOG_MASK_2, "ELogger_test", "Error message");
    cout << "  Review log file 3.  There should be 2 segments with 10 lines each." << endl;

    cout << "ELogger_test complete" << endl;
}
#endif

Void EDateTime_test()
{
   ETime t;

   t.Now();

   EString s;
   t.Format(s, "%Y-%m-%d %H:%M:%S.%0", False);
   cout << s << endl;
   t.Format(s, "%Y-%m-%d %H:%M:%S.%0", True);
   cout << s << endl;

   cpStr pszDate;

   pszDate = "2009-12-09 09:12:56.373";
   t.ParseDateTime(pszDate, True);
   t.Format(s, "%Y-%m-%d %H:%M:%S.%0", True);
   cout << "Is [" << pszDate << "] equal to [" << s << "]" << endl;

   pszDate = "09 Dec 2009 11:42:13:767";
   t.ParseDateTime(pszDate, False);
   t.Format(s, "%Y-%m-%d %H:%M:%S.%0", False);
   cout << " Is [" << pszDate << "] equal to [" << s << "]" << endl;
}

class EThreadTimerTest : public EThreadPrivate
{
public:
   EThreadTimerTest()
   {
   }

   Void onInit()
   {
      std::cout << "EThreadTimerTest::onInit()" << std::endl << std::flush;
      if (m_oneshot)
      {
         m_cnt = 0;
         m_timer1.setInterval(m_timerLength);
         m_timer1.setOneShot(True);
         m_timer2.setInterval(m_timerLength * 2);
         m_timer2.setOneShot(True);
         m_timer3.setInterval(m_timerLength * 3);
         m_timer3.setOneShot(True);

         initTimer(m_timer1);
         initTimer(m_timer2);
         initTimer(m_timer3);

         m_elapsed.Start();

         m_timer1.start();
         m_timer2.start();
         m_timer3.start();
      }
      else
      {
         m_cnt = 0;
         m_timer1.setInterval(m_timerLength);
         m_timer1.setOneShot(m_oneshot);
         initTimer(m_timer1);
         m_elapsed.Start();
         m_timer1.start();
      }
   }

   Void onTimer(EThreadEventTimer *pTimer)
   {
      cout << m_elapsed.MilliSeconds(m_oneshot?False:True) << " milliseconds has elapsed for timer " << pTimer->getId() << endl << std::flush;
      if (pTimer->getId() == m_timer1.getId())
      {
         m_cnt++;
         if (m_cnt == 5)
            quit();
      }
      else if (pTimer->getId() == m_timer2.getId())
      {
      }
      else if (pTimer->getId() == m_timer3.getId())
      {
         quit();
      }
   }

   Void onQuit()
   {
      std::cout << "EThreadTimerTest::onQuit()" << std::endl << std::flush;
   }

   Void setOneShot(Bool oneshot) { m_oneshot = oneshot; }

   Void setTimerLength(Long tl) { m_timerLength = tl; }

   DECLARE_MESSAGE_MAP()

private:
   Long m_timerLength;
   Int m_cnt;
   Bool m_oneshot;
   EThreadEventTimer m_timer1;
   EThreadEventTimer m_timer2;
   EThreadEventTimer m_timer3;
   ETimer m_elapsed;
};

BEGIN_MESSAGE_MAP(EThreadTimerTest, EThreadPrivate)
END_MESSAGE_MAP()

Void EThreadTimerPeriodic_test()
{
   static Long timerLength = 5000;
   Char buffer[256];

   cout << "Enter the timer length in milliseconds [" << timerLength << "]: ";
   cin.getline(buffer, sizeof(buffer));
   if (*buffer)
      timerLength = std::atol(buffer);
   cout.imbue(defaultLocale);

   EThreadTimerTest t;
   t.setTimerLength(timerLength);
   t.setOneShot(False);
   t.init(1, 1, NULL, 2000);
   t.join();
}

Void EThreadTimerOneShot_test()
{
   static Long timerLength = 5000;
   Char buffer[256];

   cout << "Enter the timer length in milliseconds [" << timerLength << "]: ";
   cin.getline(buffer, sizeof(buffer));
   if (*buffer)
      timerLength = std::atol(buffer);
   cout.imbue(defaultLocale);

   EThreadTimerTest t;
   t.setTimerLength(timerLength);
   t.setOneShot(True);
   t.init(1, 1, NULL, 2000);
   t.join();
}

class EThreadTestSuspendResume : public EThreadPrivate
{
public:
   EThreadTestSuspendResume()
   {
      m_timer.Start();
   }

   Void userFunc1(EThreadMessage &msg)
   {
      cout << "received EM_USER1 - elapsed time " << m_timer.MicroSeconds() << "us" << endl;
      m_timer.Start();
   }

   Void onInit()
   {
      cout << "received EM_INIT - elapsed time " << m_timer.MicroSeconds() << "us" << endl;
      m_timer.Start();
   }

   Void onSuspend()
   {
      cout << "received EM_SUSPEND - elapsed time " << m_timer.MicroSeconds() << "us" << endl;
      m_timer.Start();
   }

   Void onQuit()
   {
      EThreadPrivate::onQuit();
      cout << "received EM_QUIT - elapsed time " << m_timer.MicroSeconds() << "us" << endl;
      m_timer.Start();
   }

   DECLARE_MESSAGE_MAP()

private:
   ETimer m_timer;
};

BEGIN_MESSAGE_MAP(EThreadTestSuspendResume, EThreadPrivate)
ON_MESSAGE(EM_USER1, EThreadTestSuspendResume::userFunc1)
END_MESSAGE_MAP()

Void EThreadSuspendResume_test()
{
   cout << "EThreadSuspendResume_test - start" << endl;

   EThreadTestSuspendResume t;

   t.init(1, 1, NULL, 2000);
   EThreadBasic::sleep(1000);

   cout << "start sending EM_USER1 messages" << endl;
   for (Int i = 0; i < 5; i++)
   {
      t.sendMessage(EM_USER1);
      EThreadBasic::sleep(1000);
   }

   for (Int i = 0; i < 5; i++)
   {
      Int ms = 3000;
      if (!i)
      {
         cout << "suspending the thread for " << ms / 1000 << "seconds" << endl;
         t.suspend();
         cout << "sending EM_USER1 while thread is suspended " << endl;
      }

      t.sendMessage(EM_USER1);

      if (!i)
      {
         EThreadBasic::sleep(ms);
         cout << "resuming the thread" << endl;
         t.resume();
      }

      EThreadBasic::sleep(1000);
   }

   t.quit();
   t.join();
   cout << "EThreadSuspendResume_test - complete" << endl;
}

Void EHash_test()
{
   cout << "Ehash_test start" << endl;

   ULong hash;
   EString s;

   // EString short
   {
      EString s;
      s = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456";
      hash = EHash::getHash(s);
      cout << std::hex << std::uppercase << hash << std::nouppercase << std::dec << " for EString [" << s << "]" << endl;
   }

   {
      EString s;
      s = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123457";
      hash = EHash::getHash(s);
      cout << std::hex << std::uppercase << hash << std::nouppercase << std::dec << " for EString [" << s << "]" << endl;
   }

   // EString long
   {
      EString s;
      s = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
      hash = EHash::getHash(s);
      cout << std::hex << std::uppercase << hash << std::nouppercase << std::dec << " for EString [" << s << "]" << endl;
   }

   {
      EString s;
      s = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456780";
      hash = EHash::getHash(s);
      cout << std::hex << std::uppercase << hash << std::nouppercase << std::dec << " for EString [" << s << "]" << endl;
   }

   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////

   {
      cpChar s = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456";
      hash = EHash::getHash(s, strlen(s));
      cout << std::hex << std::uppercase << hash << std::nouppercase << std::dec << " for cpChar [" << s << "]" << endl;
   }

   {
      cpChar s = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123457";
      hash = EHash::getHash(s, strlen(s));
      cout << std::hex << std::uppercase << hash << std::nouppercase << std::dec << " for cpChar [" << s << "]" << endl;
   }

   {
      cpChar s = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
      hash = EHash::getHash(s, strlen(s));
      cout << std::hex << std::uppercase << hash << std::nouppercase << std::dec << " for cpChar [" << s << "]" << endl;
   }

   {
      cpChar s = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456780";
      hash = EHash::getHash(s, strlen(s));
      cout << std::hex << std::uppercase << hash << std::nouppercase << std::dec << " for cpChar [" << s << "]" << endl;
   }

   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////

   {
      cpUChar s = (cpUChar) "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456";
      hash = EHash::getHash(s, strlen((cpChar)s));
      cout << std::hex << std::uppercase << hash << std::nouppercase << std::dec << " for cpUChar [" << s << "]" << endl;
   }

   {
      cpUChar s = (cpUChar) "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123457";
      hash = EHash::getHash(s, strlen((cpChar)s));
      cout << std::hex << std::uppercase << hash << std::nouppercase << std::dec << " for cpUChar [" << s << "]" << endl;
   }

   {
      cpUChar s = (cpUChar) "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
      hash = EHash::getHash(s, strlen((cpChar)s));
      cout << std::hex << std::uppercase << hash << std::nouppercase << std::dec << " for cpUChar [" << s << "]" << endl;
   }

   {
      cpUChar s = (cpUChar) "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456780";
      hash = EHash::getHash(s, strlen((cpChar)s));
      cout << std::hex << std::uppercase << hash << std::nouppercase << std::dec << " for cpUChar [" << s << "]" << endl;
   }

   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////

   const uint8_t vectors_hsip32[64][4] = {
      { 0xa9, 0x35, 0x9f, 0x5b },
      { 0x27, 0x47, 0x5a, 0xb8 },
      { 0xfa, 0x62, 0xa6, 0x03 },
      { 0x8a, 0xfe, 0xe7, 0x04 },
      { 0x2a, 0x6e, 0x46, 0x89 },
      { 0xc5, 0xfa, 0xb6, 0x69 },
      { 0x58, 0x63, 0xfc, 0x23 },
      { 0x8b, 0xcf, 0x63, 0xc5 },
      { 0xd0, 0xb8, 0x84, 0x8f },
      { 0xf8, 0x06, 0xe7, 0x79 },
      { 0x94, 0xb0, 0x79, 0x34 },
      { 0x08, 0x08, 0x30, 0x50 },
      { 0x57, 0xf0, 0x87, 0x2f },
      { 0x77, 0xe6, 0x63, 0xff },
      { 0xd6, 0xff, 0xf8, 0x7c },
      { 0x74, 0xfe, 0x2b, 0x97 },
      { 0xd9, 0xb5, 0xac, 0x84 },
      { 0xc4, 0x74, 0x64, 0x5b },
      { 0x46, 0x5b, 0x8d, 0x9b },
      { 0x7b, 0xef, 0xe3, 0x87 },
      { 0xe3, 0x4d, 0x10, 0x45 },
      { 0x61, 0x3f, 0x62, 0xb3 },
      { 0x70, 0xf3, 0x67, 0xfe },
      { 0xe6, 0xad, 0xb8, 0xbd },
      { 0x27, 0x40, 0x0c, 0x63 },
      { 0x26, 0x78, 0x78, 0x75 },
      { 0x4f, 0x56, 0x7b, 0x5f },
      { 0x3a, 0xb0, 0xe6, 0x69 },
      { 0xb0, 0x64, 0x40, 0x00 },
      { 0xff, 0x67, 0x0f, 0xb4 },
      { 0x50, 0x9e, 0x33, 0x8b },
      { 0x5d, 0x58, 0x9f, 0x1a },
      { 0xfe, 0xe7, 0x21, 0x12 },
      { 0x33, 0x75, 0x32, 0x59 },
      { 0x6a, 0x43, 0x4f, 0x8c },
      { 0xfe, 0x28, 0xb7, 0x29 },
      { 0xe7, 0x5c, 0xc6, 0xec },
      { 0x69, 0x7e, 0x8d, 0x54 },
      { 0x63, 0x68, 0x8b, 0x0f },
      { 0x65, 0x0b, 0x62, 0xb4 },
      { 0xb6, 0xbc, 0x18, 0x40 },
      { 0x5d, 0x07, 0x45, 0x05 },
      { 0x24, 0x42, 0xfd, 0x2e },
      { 0x7b, 0xb7, 0x86, 0x3a },
      { 0x77, 0x05, 0xd5, 0x48 },
      { 0xd7, 0x52, 0x08, 0xb1 },
      { 0xb6, 0xd4, 0x99, 0xc8 },
      { 0x08, 0x92, 0x20, 0x2e },
      { 0x69, 0xe1, 0x2c, 0xe3 },
      { 0x8d, 0xb5, 0x80, 0xe5 },
      { 0x36, 0x97, 0x64, 0xc6 },
      { 0x01, 0x6e, 0x02, 0x04 },
      { 0x3b, 0x85, 0xf3, 0xd4 },
      { 0xfe, 0xdb, 0x66, 0xbe },
      { 0x1e, 0x69, 0x2a, 0x3a },
      { 0xc6, 0x89, 0x84, 0xc0 },
      { 0xa5, 0xc5, 0xb9, 0x40 },
      { 0x9b, 0xe9, 0xe8, 0x8c },
      { 0x7d, 0xbc, 0x81, 0x40 },
      { 0x7c, 0x07, 0x8e, 0xc5 },
      { 0xd4, 0xe7, 0x6c, 0x73 },
      { 0x42, 0x8f, 0xcb, 0xb9 },
      { 0xbd, 0x83, 0x99, 0x7a },
      { 0x59, 0xea, 0x4a, 0x74 }
   };

   const uint8_t vectors_hsip64[64][8] = {
      { 0x21, 0x8d, 0x1f, 0x59, 0xb9, 0xb8, 0x3c, 0xc8 },
      { 0xbe, 0x55, 0x24, 0x12, 0xf8, 0x38, 0x73, 0x15 },
      { 0x06, 0x4f, 0x39, 0xef, 0x7c, 0x50, 0xeb, 0x57 },
      { 0xce, 0x0f, 0x1a, 0x45, 0xf7, 0x06, 0x06, 0x79 },
      { 0xd5, 0xe7, 0x8a, 0x17, 0x5b, 0xe5, 0x2e, 0xa1 },
      { 0xcb, 0x9d, 0x7c, 0x3f, 0x2f, 0x3d, 0xb5, 0x80 },
      { 0xce, 0x3e, 0x91, 0x35, 0x8a, 0xa2, 0xbc, 0x25 },
      { 0xff, 0x20, 0x27, 0x28, 0xb0, 0x7b, 0xc6, 0x84 },
      { 0xed, 0xfe, 0xe8, 0x20, 0xbc, 0xe4, 0x85, 0x8c },
      { 0x5b, 0x51, 0xcc, 0xcc, 0x13, 0x88, 0x83, 0x07 },
      { 0x95, 0xb0, 0x46, 0x9f, 0x06, 0xa6, 0xf2, 0xee },
      { 0xae, 0x26, 0x33, 0x39, 0x94, 0xdd, 0xcd, 0x48 },
      { 0x7b, 0xc7, 0x1f, 0x9f, 0xae, 0xf5, 0xc7, 0x99 },
      { 0x5a, 0x23, 0x52, 0xd7, 0x5a, 0x0c, 0x37, 0x44 },
      { 0x3b, 0xb1, 0xa8, 0x70, 0xea, 0xe8, 0xe6, 0x58 },
      { 0x21, 0x7d, 0x0b, 0xcb, 0x4e, 0x81, 0xc9, 0x02 },
      { 0x73, 0x36, 0xaa, 0xd2, 0x5f, 0x7b, 0xf3, 0xb5 },
      { 0x37, 0xad, 0xc0, 0x64, 0x1c, 0x4c, 0x4f, 0x6a },
      { 0xc9, 0xb2, 0xdb, 0x2b, 0x9a, 0x3e, 0x42, 0xf9 },
      { 0xf9, 0x10, 0xe4, 0x80, 0x20, 0xab, 0x36, 0x3c },
      { 0x1b, 0xf5, 0x2b, 0x0a, 0x6f, 0xee, 0xa7, 0xdb },
      { 0x00, 0x74, 0x1d, 0xc2, 0x69, 0xe8, 0xb3, 0xef },
      { 0xe2, 0x01, 0x03, 0xfa, 0x1b, 0xa7, 0x76, 0xef },
      { 0x4c, 0x22, 0x10, 0xe5, 0x4b, 0x68, 0x1d, 0x73 },
      { 0x70, 0x74, 0x10, 0x45, 0xae, 0x3f, 0xa6, 0xf1 },
      { 0x0c, 0x86, 0x40, 0x37, 0x39, 0x71, 0x40, 0x38 },
      { 0x0d, 0x89, 0x9e, 0xd8, 0x11, 0x29, 0x23, 0xf0 },
      { 0x22, 0x6b, 0xf5, 0xfa, 0xb8, 0x1e, 0xe1, 0xb8 },
      { 0x2d, 0x92, 0x5f, 0xfb, 0x1e, 0x00, 0x16, 0xb5 },
      { 0x36, 0x19, 0x58, 0xd5, 0x2c, 0xee, 0x10, 0xf1 },
      { 0x29, 0x1a, 0xaf, 0x86, 0x48, 0x98, 0x17, 0x9d },
      { 0x86, 0x3c, 0x7f, 0x15, 0x5c, 0x34, 0x11, 0x7c },
      { 0x28, 0x70, 0x9d, 0x46, 0xd8, 0x11, 0x62, 0x6c },
      { 0x24, 0x84, 0x77, 0x68, 0x1d, 0x28, 0xf8, 0x9c },
      { 0x83, 0x24, 0xe4, 0xd7, 0x52, 0x8f, 0x98, 0x30 },
      { 0xf9, 0xef, 0xd4, 0xe1, 0x3a, 0xea, 0x6b, 0xd8 },
      { 0x86, 0xd6, 0x7a, 0x40, 0xec, 0x42, 0x76, 0xdc },
      { 0x3f, 0x62, 0x92, 0xec, 0xcc, 0xa9, 0x7e, 0x35 },
      { 0xcb, 0xd9, 0x2e, 0xe7, 0x24, 0xd4, 0x21, 0x09 },
      { 0x36, 0x8d, 0xf6, 0x80, 0x8d, 0x40, 0x3d, 0x79 },
      { 0x5b, 0x38, 0xc8, 0x1c, 0x67, 0xc8, 0xae, 0x4c },
      { 0x95, 0xab, 0x71, 0x89, 0xd4, 0x39, 0xac, 0xb3 },
      { 0xa9, 0x1a, 0x52, 0xc0, 0x25, 0x32, 0x70, 0x24 },
      { 0x5b, 0x00, 0x87, 0xc6, 0x95, 0x28, 0xac, 0xea },
      { 0x1e, 0x30, 0xf3, 0xad, 0x27, 0xdc, 0xb1, 0x5a },
      { 0x69, 0x7f, 0x5c, 0x9a, 0x90, 0x32, 0x4e, 0xd4 },
      { 0x49, 0x5c, 0x0f, 0x99, 0x55, 0x57, 0xdc, 0x38 },
      { 0x94, 0x27, 0x20, 0x2a, 0x3c, 0x29, 0xf9, 0x4d },
      { 0xa9, 0xea, 0xa8, 0xc0, 0x4b, 0xa9, 0x3e, 0x3e },
      { 0xee, 0xa4, 0xc1, 0x73, 0x7d, 0x01, 0x12, 0x18 },
      { 0x91, 0x2d, 0x56, 0x8f, 0xd8, 0xf6, 0x5a, 0x49 },
      { 0x56, 0x91, 0x95, 0x96, 0xb0, 0xff, 0x5c, 0x97 },
      { 0x02, 0x44, 0x5a, 0x79, 0x98, 0xf5, 0x50, 0xe1 },
      { 0x86, 0xec, 0x46, 0x6c, 0xe7, 0x1d, 0x1f, 0xb2 },
      { 0x35, 0x95, 0x69, 0xe7, 0xd2, 0x89, 0xe3, 0xbc },
      { 0x87, 0x1b, 0x05, 0xca, 0x62, 0xbb, 0x7c, 0x96 },
      { 0xa1, 0xa4, 0x92, 0xf9, 0x42, 0xf1, 0x5f, 0x1d },
      { 0x12, 0xec, 0x26, 0x7f, 0xf6, 0x09, 0x5b, 0x6e },
      { 0x5d, 0x1b, 0x5e, 0xa1, 0xb2, 0x31, 0xd8, 0x9d },
      { 0xd8, 0xcf, 0xb4, 0x45, 0x3f, 0x92, 0xee, 0x54 },
      { 0xd6, 0x76, 0x28, 0x90, 0xbf, 0x26, 0xe4, 0x60 },
      { 0x31, 0x35, 0x63, 0xa4, 0xb7, 0xed, 0x5c, 0xf3 },
      { 0xf9, 0x0b, 0x3a, 0xb5, 0x72, 0xd4, 0x66, 0x93 },
      { 0x2e, 0xa6, 0x3c, 0x71, 0xbf, 0x32, 0x60, 0x87 }
   };

   const uint8_t vectors_sip128[64][16] = {
      { 0xa3, 0x81, 0x7f, 0x04, 0xba, 0x25, 0xa8, 0xe6, 0x6d, 0xf6, 0x72, 0x14, 0xc7, 0x55, 0x02, 0x93 },
      { 0xda, 0x87, 0xc1, 0xd8, 0x6b, 0x99, 0xaf, 0x44, 0x34, 0x76, 0x59, 0x11, 0x9b, 0x22, 0xfc, 0x45 },
      { 0x81, 0x77, 0x22, 0x8d, 0xa4, 0xa4, 0x5d, 0xc7, 0xfc, 0xa3, 0x8b, 0xde, 0xf6, 0x0a, 0xff, 0xe4 },
      { 0x9c, 0x70, 0xb6, 0x0c, 0x52, 0x67, 0xa9, 0x4e, 0x5f, 0x33, 0xb6, 0xb0, 0x29, 0x85, 0xed, 0x51 },
      { 0xf8, 0x81, 0x64, 0xc1, 0x2d, 0x9c, 0x8f, 0xaf, 0x7d, 0x0f, 0x6e, 0x7c, 0x7b, 0xcd, 0x55, 0x79 },
      { 0x13, 0x68, 0x87, 0x59, 0x80, 0x77, 0x6f, 0x88, 0x54, 0x52, 0x7a, 0x07, 0x69, 0x0e, 0x96, 0x27 },
      { 0x14, 0xee, 0xca, 0x33, 0x8b, 0x20, 0x86, 0x13, 0x48, 0x5e, 0xa0, 0x30, 0x8f, 0xd7, 0xa1, 0x5e },
      { 0xa1, 0xf1, 0xeb, 0xbe, 0xd8, 0xdb, 0xc1, 0x53, 0xc0, 0xb8, 0x4a, 0xa6, 0x1f, 0xf0, 0x82, 0x39 },
      { 0x3b, 0x62, 0xa9, 0xba, 0x62, 0x58, 0xf5, 0x61, 0x0f, 0x83, 0xe2, 0x64, 0xf3, 0x14, 0x97, 0xb4 },
      { 0x26, 0x44, 0x99, 0x06, 0x0a, 0xd9, 0xba, 0xab, 0xc4, 0x7f, 0x8b, 0x02, 0xbb, 0x6d, 0x71, 0xed },
      { 0x00, 0x11, 0x0d, 0xc3, 0x78, 0x14, 0x69, 0x56, 0xc9, 0x54, 0x47, 0xd3, 0xf3, 0xd0, 0xfb, 0xba },
      { 0x01, 0x51, 0xc5, 0x68, 0x38, 0x6b, 0x66, 0x77, 0xa2, 0xb4, 0xdc, 0x6f, 0x81, 0xe5, 0xdc, 0x18 },
      { 0xd6, 0x26, 0xb2, 0x66, 0x90, 0x5e, 0xf3, 0x58, 0x82, 0x63, 0x4d, 0xf6, 0x85, 0x32, 0xc1, 0x25 },
      { 0x98, 0x69, 0xe2, 0x47, 0xe9, 0xc0, 0x8b, 0x10, 0xd0, 0x29, 0x93, 0x4f, 0xc4, 0xb9, 0x52, 0xf7 },
      { 0x31, 0xfc, 0xef, 0xac, 0x66, 0xd7, 0xde, 0x9c, 0x7e, 0xc7, 0x48, 0x5f, 0xe4, 0x49, 0x49, 0x02 },
      { 0x54, 0x93, 0xe9, 0x99, 0x33, 0xb0, 0xa8, 0x11, 0x7e, 0x08, 0xec, 0x0f, 0x97, 0xcf, 0xc3, 0xd9 },
      { 0x6e, 0xe2, 0xa4, 0xca, 0x67, 0xb0, 0x54, 0xbb, 0xfd, 0x33, 0x15, 0xbf, 0x85, 0x23, 0x05, 0x77 },
      { 0x47, 0x3d, 0x06, 0xe8, 0x73, 0x8d, 0xb8, 0x98, 0x54, 0xc0, 0x66, 0xc4, 0x7a, 0xe4, 0x77, 0x40 },
      { 0xa4, 0x26, 0xe5, 0xe4, 0x23, 0xbf, 0x48, 0x85, 0x29, 0x4d, 0xa4, 0x81, 0xfe, 0xae, 0xf7, 0x23 },
      { 0x78, 0x01, 0x77, 0x31, 0xcf, 0x65, 0xfa, 0xb0, 0x74, 0xd5, 0x20, 0x89, 0x52, 0x51, 0x2e, 0xb1 },
      { 0x9e, 0x25, 0xfc, 0x83, 0x3f, 0x22, 0x90, 0x73, 0x3e, 0x93, 0x44, 0xa5, 0xe8, 0x38, 0x39, 0xeb },
      { 0x56, 0x8e, 0x49, 0x5a, 0xbe, 0x52, 0x5a, 0x21, 0x8a, 0x22, 0x14, 0xcd, 0x3e, 0x07, 0x1d, 0x12 },
      { 0x4a, 0x29, 0xb5, 0x45, 0x52, 0xd1, 0x6b, 0x9a, 0x46, 0x9c, 0x10, 0x52, 0x8e, 0xff, 0x0a, 0xae },
      { 0xc9, 0xd1, 0x84, 0xdd, 0xd5, 0xa9, 0xf5, 0xe0, 0xcf, 0x8c, 0xe2, 0x9a, 0x9a, 0xbf, 0x69, 0x1c },
      { 0x2d, 0xb4, 0x79, 0xae, 0x78, 0xbd, 0x50, 0xd8, 0x88, 0x2a, 0x8a, 0x17, 0x8a, 0x61, 0x32, 0xad },
      { 0x8e, 0xce, 0x5f, 0x04, 0x2d, 0x5e, 0x44, 0x7b, 0x50, 0x51, 0xb9, 0xea, 0xcb, 0x8d, 0x8f, 0x6f },
      { 0x9c, 0x0b, 0x53, 0xb4, 0xb3, 0xc3, 0x07, 0xe8, 0x7e, 0xae, 0xe0, 0x86, 0x78, 0x14, 0x1f, 0x66 },
      { 0xab, 0xf2, 0x48, 0xaf, 0x69, 0xa6, 0xea, 0xe4, 0xbf, 0xd3, 0xeb, 0x2f, 0x12, 0x9e, 0xeb, 0x94 },
      { 0x06, 0x64, 0xda, 0x16, 0x68, 0x57, 0x4b, 0x88, 0xb9, 0x35, 0xf3, 0x02, 0x73, 0x58, 0xae, 0xf4 },
      { 0xaa, 0x4b, 0x9d, 0xc4, 0xbf, 0x33, 0x7d, 0xe9, 0x0c, 0xd4, 0xfd, 0x3c, 0x46, 0x7c, 0x6a, 0xb7 },
      { 0xea, 0x5c, 0x7f, 0x47, 0x1f, 0xaf, 0x6b, 0xde, 0x2b, 0x1a, 0xd7, 0xd4, 0x68, 0x6d, 0x22, 0x87 },
      { 0x29, 0x39, 0xb0, 0x18, 0x32, 0x23, 0xfa, 0xfc, 0x17, 0x23, 0xde, 0x4f, 0x52, 0xc4, 0x3d, 0x35 },
      { 0x7c, 0x39, 0x56, 0xca, 0x5e, 0xea, 0xfc, 0x3e, 0x36, 0x3e, 0x9d, 0x55, 0x65, 0x46, 0xeb, 0x68 },
      { 0x77, 0xc6, 0x07, 0x71, 0x46, 0xf0, 0x1c, 0x32, 0xb6, 0xb6, 0x9d, 0x5f, 0x4e, 0xa9, 0xff, 0xcf },
      { 0x37, 0xa6, 0x98, 0x6c, 0xb8, 0x84, 0x7e, 0xdf, 0x09, 0x25, 0xf0, 0xf1, 0x30, 0x9b, 0x54, 0xde },
      { 0xa7, 0x05, 0xf0, 0xe6, 0x9d, 0xa9, 0xa8, 0xf9, 0x07, 0x24, 0x1a, 0x2e, 0x92, 0x3c, 0x8c, 0xc8 },
      { 0x3d, 0xc4, 0x7d, 0x1f, 0x29, 0xc4, 0x48, 0x46, 0x1e, 0x9e, 0x76, 0xed, 0x90, 0x4f, 0x67, 0x11 },
      { 0x0d, 0x62, 0xbf, 0x01, 0xe6, 0xfc, 0x0e, 0x1a, 0x0d, 0x3c, 0x47, 0x51, 0xc5, 0xd3, 0x69, 0x2b },
      { 0x8c, 0x03, 0x46, 0x8b, 0xca, 0x7c, 0x66, 0x9e, 0xe4, 0xfd, 0x5e, 0x08, 0x4b, 0xbe, 0xe7, 0xb5 },
      { 0x52, 0x8a, 0x5b, 0xb9, 0x3b, 0xaf, 0x2c, 0x9c, 0x44, 0x73, 0xcc, 0xe5, 0xd0, 0xd2, 0x2b, 0xd9 },
      { 0xdf, 0x6a, 0x30, 0x1e, 0x95, 0xc9, 0x5d, 0xad, 0x97, 0xae, 0x0c, 0xc8, 0xc6, 0x91, 0x3b, 0xd8 },
      { 0x80, 0x11, 0x89, 0x90, 0x2c, 0x85, 0x7f, 0x39, 0xe7, 0x35, 0x91, 0x28, 0x5e, 0x70, 0xb6, 0xdb },
      { 0xe6, 0x17, 0x34, 0x6a, 0xc9, 0xc2, 0x31, 0xbb, 0x36, 0x50, 0xae, 0x34, 0xcc, 0xca, 0x0c, 0x5b },
      { 0x27, 0xd9, 0x34, 0x37, 0xef, 0xb7, 0x21, 0xaa, 0x40, 0x18, 0x21, 0xdc, 0xec, 0x5a, 0xdf, 0x89 },
      { 0x89, 0x23, 0x7d, 0x9d, 0xed, 0x9c, 0x5e, 0x78, 0xd8, 0xb1, 0xc9, 0xb1, 0x66, 0xcc, 0x73, 0x42 },
      { 0x4a, 0x6d, 0x80, 0x91, 0xbf, 0x5e, 0x7d, 0x65, 0x11, 0x89, 0xfa, 0x94, 0xa2, 0x50, 0xb1, 0x4c },
      { 0x0e, 0x33, 0xf9, 0x60, 0x55, 0xe7, 0xae, 0x89, 0x3f, 0xfc, 0x0e, 0x3d, 0xcf, 0x49, 0x29, 0x02 },
      { 0xe6, 0x1c, 0x43, 0x2b, 0x72, 0x0b, 0x19, 0xd1, 0x8e, 0xc8, 0xd8, 0x4b, 0xdc, 0x63, 0x15, 0x1b },
      { 0xf7, 0xe5, 0xae, 0xf5, 0x49, 0xf7, 0x82, 0xcf, 0x37, 0x90, 0x55, 0xa6, 0x08, 0x26, 0x9b, 0x16 },
      { 0x43, 0x8d, 0x03, 0x0f, 0xd0, 0xb7, 0xa5, 0x4f, 0xa8, 0x37, 0xf2, 0xad, 0x20, 0x1a, 0x64, 0x03 },
      { 0xa5, 0x90, 0xd3, 0xee, 0x4f, 0xbf, 0x04, 0xe3, 0x24, 0x7e, 0x0d, 0x27, 0xf2, 0x86, 0x42, 0x3f },
      { 0x5f, 0xe2, 0xc1, 0xa1, 0x72, 0xfe, 0x93, 0xc4, 0xb1, 0x5c, 0xd3, 0x7c, 0xae, 0xf9, 0xf5, 0x38 },
      { 0x2c, 0x97, 0x32, 0x5c, 0xbd, 0x06, 0xb3, 0x6e, 0xb2, 0x13, 0x3d, 0xd0, 0x8b, 0x3a, 0x01, 0x7c },
      { 0x92, 0xc8, 0x14, 0x22, 0x7a, 0x6b, 0xca, 0x94, 0x9f, 0xf0, 0x65, 0x9f, 0x00, 0x2a, 0xd3, 0x9e },
      { 0xdc, 0xe8, 0x50, 0x11, 0x0b, 0xd8, 0x32, 0x8c, 0xfb, 0xd5, 0x08, 0x41, 0xd6, 0x91, 0x1d, 0x87 },
      { 0x67, 0xf1, 0x49, 0x84, 0xc7, 0xda, 0x79, 0x12, 0x48, 0xe3, 0x2b, 0xb5, 0x92, 0x25, 0x83, 0xda },
      { 0x19, 0x38, 0xf2, 0xcf, 0x72, 0xd5, 0x4e, 0xe9, 0x7e, 0x94, 0x16, 0x6f, 0xa9, 0x1d, 0x2a, 0x36 },
      { 0x74, 0x48, 0x1e, 0x96, 0x46, 0xed, 0x49, 0xfe, 0x0f, 0x62, 0x24, 0x30, 0x16, 0x04, 0x69, 0x8e },
      { 0x57, 0xfc, 0xa5, 0xde, 0x98, 0xa9, 0xd6, 0xd8, 0x00, 0x64, 0x38, 0xd0, 0x58, 0x3d, 0x8a, 0x1d },
      { 0x9f, 0xec, 0xde, 0x1c, 0xef, 0xdc, 0x1c, 0xbe, 0xd4, 0x76, 0x36, 0x74, 0xd9, 0x57, 0x53, 0x59 },
      { 0xe3, 0x04, 0x0c, 0x00, 0xeb, 0x28, 0xf1, 0x53, 0x66, 0xca, 0x73, 0xcb, 0xd8, 0x72, 0xe7, 0x40 },
      { 0x76, 0x97, 0x00, 0x9a, 0x6a, 0x83, 0x1d, 0xfe, 0xcc, 0xa9, 0x1c, 0x59, 0x93, 0x67, 0x0f, 0x7a },
      { 0x58, 0x53, 0x54, 0x23, 0x21, 0xf5, 0x67, 0xa0, 0x05, 0xd5, 0x47, 0xa4, 0xf0, 0x47, 0x59, 0xbd },
      { 0x51, 0x50, 0xd1, 0x77, 0x2f, 0x50, 0x83, 0x4a, 0x50, 0x3e, 0x06, 0x9a, 0x97, 0x3f, 0xbd, 0x7c }
   };

   UChar in[64], out[16];
   size_t lengths[3] = { 4, 8, 16 };

   for (int ver=0; ver<3; ver++)
   {
      int fails = 0;
      switch (ver)
      {
         case 0:  std::cout << "********** 32-Bit **********" << std::endl; break;
         case 1:  std::cout << "********** 64-Bit **********" << std::endl; break;
         case 2:  std::cout << "********** 128-Bit **********" << std::endl; break;
      }
      for (int i=0; i<64; i++)
      {
         in[i] = i;
         switch(ver)
         {
            case 0:
            {
               union
               {
                  ULong val;
                  UChar bytes[sizeof(val)];
               } dbg;
               ESipHash24::getHash32(in, i, out, lengths[ver]);
               dbg.val = ESipHash24::getHash32(in, i);
               if (memcmp(out,vectors_hsip32[i],lengths[ver]) || memcmp(dbg.bytes,vectors_hsip32[i],lengths[ver]))
               {
                  std::cout << "Fail for " << i << " bytes" << std::endl;
                  fails++;
               }
               break;
            }
            case 1:
            {
               union
               {
                  ULongLong val;
                  UChar bytes[sizeof(val)];
               } dbg;
               ESipHash24::getHash64(in, i, out, lengths[ver]);
               dbg.val = ESipHash24::getHash64(in, i);
               if (memcmp(out,vectors_hsip64[i],lengths[ver]) || memcmp(dbg.bytes,vectors_hsip64[i],lengths[ver]))
               {
                  std::cout << "Fail for " << i << " bytes" << std::endl;
                  fails++;
               }
               break;
            }
            case 2:
            {
               ESipHash24::getHash128(in, i, out, lengths[ver]);
               if (memcmp(out,vectors_sip128[i],lengths[ver]))
               {
                  std::cout << "Fail for " << i << " bytes" << std::endl;
                  fails++;
               }
               break;
            }
         }
      }

      if (!fails)
         std::cout << "OK" << std::endl;
   }

   //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////

   EString str = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPARSTUVWXYZ0123456789"
                  "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPARSTUVWXYZ0123456789";
   std::cout << "EMurmurHash64::getHash((cChar)1) = " << EMurmurHash64::getHash((cChar)1) << std::endl;
   std::cout << "EMurmurHash64::getHash((cUChar)1) = " << EMurmurHash64::getHash((cUChar)1) << std::endl;
   std::cout << "EMurmurHash64::getHash((cShort)1) = " << EMurmurHash64::getHash((cShort)1) << std::endl;
   std::cout << "EMurmurHash64::getHash((cUShort)1) = " << EMurmurHash64::getHash((cUShort)1) << std::endl;
   std::cout << "EMurmurHash64::getHash((cLong)1) = " << EMurmurHash64::getHash((cLong)1) << std::endl;
   std::cout << "EMurmurHash64::getHash((cULong)1) = " << EMurmurHash64::getHash((cULong)1) << std::endl;
   std::cout << "EMurmurHash64::getHash((cLongLong)1) = " << EMurmurHash64::getHash((cLongLong)1) << std::endl;
   std::cout << "EMurmurHash64::getHash((cULongLong)1) = " << EMurmurHash64::getHash((cULongLong)1) << std::endl;
   std::cout << "EMurmurHash64::getHash((cFloat)1) = " << EMurmurHash64::getHash((cFloat)1) << std::endl;
   std::cout << "EMurmurHash64::getHash((cDouble)1) = " << EMurmurHash64::getHash((cDouble)1) << std::endl;
   std::cout << "EMurmurHash64::getHash(str.c_str()) = " << EMurmurHash64::getHash(str.c_str()) << std::endl;
   std::cout << "EMurmurHash64::getHash(str.c_str(),strlen(str.c_str())) = " << EMurmurHash64::getHash(str.c_str(),strlen(str.c_str())) << std::endl;
   std::cout << "EMurmurHash64::getHash((cpUChar)str.c_str(),strlen(str.c_str())) = " << EMurmurHash64::getHash((cpUChar)str.c_str(),strlen(str.c_str())) << std::endl;
   std::cout << "EMurmurHash64::getHash(str) = " << EMurmurHash64::getHash(str) << std::endl;

   ETimer tmr;

   tmr.Start();
   for (int i=0; i<1000000; i++)
      EMurmurHash64::getHash((cULongLong)1);
   tmr.Stop();
   std::cout << "EMurmurHash64::getHash((cULongLong)1) 1,000,000 iterations took " << tmr.MicroSeconds() << "us" << std::endl;

   tmr.Start();
   for (int i=0; i<1000000; i++)
      EMurmurHash64::getHash(str);
   tmr.Stop();
   std::cout << "EMurmurHash64::getHash(str) 1,000,000 iterations took " << tmr.MicroSeconds() << "us" << std::endl;

   cout << "Ehash_test complete" << endl;
}

Void EDirectory_test()
{
   Char path[512];
   Char mask[512];

   cout << "Path to Search : ";
   cin.getline(path, sizeof(path));
   cout << endl;

   cout << "Search Mask : ";
   cin.getline(mask, sizeof(mask));
   cout << endl;

   if (path[0] == 'q' || path[0] == 'Q')
      return;

   try
   {
      EDirectory d;
      cpStr fn = d.getFirstEntry(path, mask);
      while (fn)
      {
         cout << fn << endl;
         fn = d.getNextEntry();
      }
   }
   catch (EError &e)
   {
      cout << e.getText() << endl;
   }
}

class EThreadDeadlock : public EThreadBasic
{
public:
   EThreadDeadlock(const char *name, EMutexData &m1, const char *m1name, EMutexData &m2, const char *m2name)
       : m_m1(m1),
         m_m2(m2)
   {
      m_name = name;
      m_m1_name = m1name;
      m_m2_name = m2name;
   }

   Dword threadProc(Void *arg)
   {
      cout << "Thread " << m_name << " is attempting to lock " << m_m1_name << endl;
      EMutexLock l1(m_m1, false);
      if (l1.acquire(false))
      {
         cout << "Thread " << m_name << " has locked " << m_m1_name << ", sleeping for 1 second" << endl;
      }
      else
      {
         cout << "Thread " << m_name << " was unable to lock " << m_m1_name << ", sleeping for 1 second" << endl;
         return 0;
      }

      sleep(1000);

      cout << "Thread " << m_name << " is attempting to lock " << m_m2_name << endl;
      EMutexLock l2(m_m2, false);
      if (l2.acquire(false))
      {
         cout << "Thread " << m_name << " has locked " << m_m2_name << endl;
      }
      else
      {
         cout << "Thread " << m_name << " was unable to lock " << m_m2_name << endl;
         return 0;
      }

      return 0;
   }

private:
   EString m_name;
   EString m_m1_name;
   EString m_m2_name;
   EMutexData &m_m1;
   EMutexData &m_m2;
};

Void deadlock()
{
   EMutexPrivate m1;
   EMutexPrivate m2;
   EThreadDeadlock t1("THREAD_1", m1, "MUTEX_1", m2, "MUTEX_2");
   EThreadDeadlock t2("THREAD_2", m2, "MUTEX_2", m1, "MUTEX_1");

   t1.init(NULL);
   t2.init(NULL);

   t1.join();
   t2.join();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class Listener;
class Talker;

class TcpWorker : public ESocket::ThreadPrivate
{
public:
   TcpWorker()
   {
      m_repeat = 1;
      m_listen = False;
      m_port = 0;
      m_attempt = 0;
      m_cnt = 0;
      m_talker = NULL;
   }

   Void onInit();
   Void onQuit();
   Void onClose();
   Void onSocketClosed(ESocket::BasePrivate *psocket);
   Void onSocketError(ESocket::BasePrivate *psocket);

   Void errorHandler(EError &err, ESocket::BasePrivate *psocket);

   Talker *createTalker();

   Void setListen(Bool v) { m_listen = v; }
   Bool getListen() { return m_listen; }

   Void setCount(Int cnt) { m_cnt = cnt; }
   Int getCnt() { return m_cnt; }

   Void setIpAddress(cpStr ipaddr) { m_ipaddress = ipaddr; }
   cpStr getIpAddress() { return m_ipaddress; }

   Void setPort(UShort port) { m_port = port; }
   UShort getPort() { return m_port; }

   Void setRepeat(ULongLong repeat) { m_repeat = repeat; }
   ULongLong getRepeat() { return m_repeat; }

private:
   Void connect();

   ULongLong m_repeat;
   Bool m_listen;
   EString m_ipaddress;
   UShort m_port;
   ULongLong m_attempt;
   Int m_cnt;
   Listener *m_listener;
   Talker *m_talker;
};

class Listener : public ESocket::TCP::ListenerPrivate
{
public:
   Listener(TcpWorker &thread) : ESocket::TCP::ListenerPrivate(thread) {}
   virtual ~Listener() {}

   ESocket::TCP::TalkerPrivate *createSocket(ESocket::ThreadPrivate &thread);

   Void onClose();
   Void onError();

private:
   Listener();
};

class Talker : public ESocket::TCP::TalkerPrivate
{
public:
   Talker(TcpWorker &thread) : ESocket::TCP::TalkerPrivate(thread) {}
   ~Talker() {}

   Void onConnect();
   Void onReceive();
   Void onClose();

private:
   Talker();
};

///////////////////////////////////////////////////////////////////////////////////

ESocket::TCP::TalkerPrivate *Listener::createSocket(ESocket::ThreadPrivate &thread)
{

   return ((TcpWorker &)thread).createTalker();
}

Void Listener::onClose()
{
   std::cout << "listening socket closed" << std::endl
             << std::flush;
}

Void Listener::onError()
{
   std::cout << "socket error " << getError() << " occurred on listening socket during select" << std::endl
             << std::flush;
}

///////////////////////////////////////////////////////////////////////////////////

Void Talker::onConnect()
{
   ESocket::TCP::TalkerPrivate::onConnect();

   if (((TcpWorker &)getThread()).getListen())
   {
      std::cout << "Talker::onConnect() - server connected" << std::endl
                << std::flush;

      EString localIpAddr = getLocalAddress();
      UShort localPort = getLocalPort();
      EString remoteIpAddr = getRemoteAddress();
      UShort remotePort = getRemotePort();

      std::cout.imbue(defaultLocale);
      std::cout << "socket connected"
                << " localIp=" << localIpAddr << " localPort=" << localPort
                << " remoteIp=" << remoteIpAddr << " remotePort=" << remotePort
                << std::endl
                << std::flush;
      std::cout.imbue(mylocale);
   }
   else
   {
      std::cout << "Talker::onConnect() - client connected" << std::endl
                << std::flush;

      EString localIpAddr = getLocalAddress();
      UShort localPort = getLocalPort();
      EString remoteIpAddr = getRemoteAddress();
      UShort remotePort = getRemotePort();

      std::cout.imbue(defaultLocale);
      std::cout << "socket connected"
                << " localIp=" << localIpAddr << " localPort=" << localPort
                << " remoteIp=" << remoteIpAddr << " remotePort=" << remotePort
                << std::endl
                << std::flush;
      std::cout.imbue(mylocale);

      try
      {
         Int val = 1;
         write((pUChar)&val, sizeof(val));
      }
      catch (const ESocket::TcpTalkerError_SendingPacket &e)
      {
         std::cerr << e.what() << '\n'
                   << std::flush;
         getThread().quit();
      }
   }
}

Void Talker::onReceive()
{
   UChar buffer[1024];
   Int *pval = (Int *)buffer;

   try
   {
      while (true)
      {
         if (bytesPending() < 4 || read(buffer, 4) != 4)
            break;

         if (((TcpWorker &)getThread()).getListen())
         {
            if ((*pval) % 10000 == 1)
               std::cout << "\r" << *pval - 1 << std::flush;
         }
         else
         {
            if ((*pval) % 10000 == 0)
               std::cout << "\r" << *pval << std::flush;
         }
         
         if (*pval != -1)
         {
            *pval = (((TcpWorker &)getThread()).getCnt() > 0 && *pval >= ((TcpWorker &)getThread()).getCnt()) ? -1 : (*pval + 1);
            write(buffer, 4);
         }

         if (*pval == -1)
         {
            if (((TcpWorker &)getThread()).getListen())
               disconnect();
            break;
         }
      }
   }
   catch (const ESocket::TcpTalkerError_SendingPacket &e)
   {
      std::cerr << e.what() << '\n'
                << std::flush;
      getThread().quit();
   }
   catch (const std::exception &e)
   {
      std::cerr << e.what() << '\n'
                << std::flush;
      getThread().quit();
   }
}

Void Talker::onClose()
{
   std::cout << std::endl
             << "socket closed" << std::endl
             << std::flush;
   // ((TcpWorker &)getThread()).onClose();
}

///////////////////////////////////////////////////////////////////////////////////

Talker *TcpWorker::createTalker()
{
   m_attempt++;
   return m_talker = new Talker(*this);
}

Void TcpWorker::onInit()
{
   if (getListen())
   {
      m_listener = new Listener(*this);
      m_listener->getLocalAddress().setAddress(getIpAddress(), getPort());
      m_listener->setBacklog(10);
      m_listener->listen();
      std::cout.imbue(defaultLocale);
      std::cout << "waiting for client to attach on port " << m_port << std::endl
                << std::flush;
      std::cout.imbue(mylocale);
   }
   else
   {
      connect();
   }

   std::cout << std::endl
             << std::flush;
}

Void TcpWorker::connect()
{
   std::cout.imbue(defaultLocale);
   std::cout << "connecting to server on port " << m_port << " attempt " << numberFormatWithCommas<ULongLong>(m_attempt + 1) << std::endl
               << std::flush;
   std::cout.imbue(mylocale);
   createTalker()->connect(getIpAddress(), m_port);
}

Void TcpWorker::onQuit()
{
}

Void TcpWorker::onClose()
{
   if (m_talker)
   {
      Talker *t = m_talker;
      m_talker = NULL;

      //t->close();
      delete t;
      quit();
   }
}

Void TcpWorker::onSocketClosed(ESocket::BasePrivate *psocket)
{
   if (psocket == reinterpret_cast<ESocket::BasePrivate*>(m_talker))
   {
      delete m_talker;
      m_talker = NULL;
      if (m_attempt >= m_repeat)
         quit();
      else if (!getListen())
         connect();
   }
   else if (psocket == reinterpret_cast<ESocket::BasePrivate*>(m_listener))
   {
      delete m_listener;
      m_listener = NULL;
      quit();
   }
}

Void TcpWorker::onSocketError(ESocket::BasePrivate *psocket)
{
   onSocketClosed(psocket);
}

Void TcpWorker::errorHandler(EError &err, ESocket::BasePrivate *psocket)
{
   //std::cout << "Socket exception - " << err << std::endl << std::flush;
}

///////////////////////////////////////////////////////////////////////////////////

Void tcpsockettest(Bool server)
{
   static Int messages = 100000;
   static Char ipaddress[128] = "127.0.0.1";
   static UShort port = 12345;
   static Int repeat = 1;
   TcpWorker *pWorker = new TcpWorker();
   Char buffer[128];

   cout.imbue(defaultLocale);
   cout << "Enter the number of times to repeat the test(-1 indicates forever) [" << repeat << "]: ";
   cout.imbue(mylocale);
   cin.getline(buffer, sizeof(buffer));
   repeat = buffer[0] ? std::stoi(buffer) : repeat;
   pWorker->setRepeat(repeat < 0 ? ULLONG_MAX : repeat);

   cout.imbue(defaultLocale);
   cout << "Enter the IP address for the connection [" << ipaddress << "]: ";
   cout.imbue(mylocale);
   cin.getline(buffer, sizeof(buffer));
   if (buffer[0])
      strcpy(ipaddress, buffer);
   pWorker->setIpAddress(ipaddress);

   cout.imbue(defaultLocale);
   cout << "Enter the port number for the connection [" << port << "]: ";
   cout.imbue(mylocale);
   cin.getline(buffer, sizeof(buffer));
   port = buffer[0] ? (UShort)std::stoi(buffer) : port;
   pWorker->setPort(port);

   if (server)
   {
      cout << "Enter number of messages to exchange with the client [" << messages << "]: ";
      cin.getline(buffer, sizeof(buffer));
      messages = buffer[0] ? std::stoi(buffer) : messages;
      pWorker->setCount(messages);
   }

   pWorker->setListen(server);

   pWorker->init(1, 1, NULL);
   pWorker->join();
   delete pWorker;
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

class UdpSocket;

class UdpWorker : public ESocket::ThreadPrivate
{
public:
   UdpWorker()
   {
      m_localport = 0;
      m_remoteport = 0;
      m_cnt = 0;
      m_socket = NULL;
   }

   Void onInit();
   Void onQuit();
   Void onClose();

   Void errorHandler(EError &err, ESocket::BasePrivate *psocket);

   Void setCount(Int cnt) { m_cnt = cnt; }
   Int getCount() { return m_cnt; }

   Void setLocalIp(cpStr ip) { m_localip = ip; }
   EString &getLocalIp() { return m_localip; }
   Void setLocalPort(UShort port) { m_localport = port; }
   UShort getLocalPort() { return m_localport; }

   Void setRemoteIp(cpStr ip) { m_remoteip = ip; }
   EString &getRemoteIp() { return m_remoteip; }
   Void setRemotePort(UShort port) { m_remoteport = port; }
   UShort getRemotePort() { return m_remoteport; }

   Void onTimer(EThreadEventTimer *pTimer);

   Void myHandler(EThreadMessage &msg)
   {
      std::cout << "UdpWorker::myHandler() - example event handler in socket thread" << std::endl << std::endl << std::flush;
   }

   DECLARE_MESSAGE_MAP()

private:
   EString m_localip;
   UShort m_localport;
   EString m_remoteip;
   UShort m_remoteport;
   Int m_cnt;
   UdpSocket *m_socket;
   EThreadEventTimer m_timer;
};

#define EM_UDPWORKER_1 (EM_USER + 1)
BEGIN_MESSAGE_MAP(UdpWorker, ESocket::ThreadPrivate)
   ON_MESSAGE(EM_UDPWORKER_1, UdpWorker::myHandler)
END_MESSAGE_MAP()

class UdpSocket : public ESocket::UdpPrivate
{
public:
   UdpSocket(UdpWorker &thread) : ESocket::UdpPrivate(thread)
   {
      m_cnt = 0;
      m_sentcnt = 0;
   }

   virtual ~UdpSocket() {}

   Void onReceive(const ESocket::Address &from, const ESocket::Address &to, cpUChar msg, Int len);
   Void onError();

   Void sendpacket();

   Void setCount(Int cnt) { m_cnt = cnt; }
   Int getSentCount() { return m_sentcnt; }

   Void setRemote(const ESocket::Address addr) { m_remote = addr; }

private:
   Int m_cnt;
   Int m_sentcnt;
   ESocket::Address m_remote;
};

///////////////////////////////////////////////////////////////////////////////////

Void UdpSocket::sendpacket()
{
   if (m_sentcnt != -1)
   {
      if (m_sentcnt < m_cnt)
         m_sentcnt++;
      else
         m_sentcnt = -1;
   }
   std::cout << "sending packet number " << m_sentcnt << std::endl;
   write( m_remote, reinterpret_cast<cpUChar>(&m_sentcnt), sizeof(m_sentcnt) );
}

Void UdpSocket::onReceive(const ESocket::Address &from, const ESocket::Address &to, cpUChar pData, Int length)
{
   std::cout.imbue(defaultLocale);
   std::cout << ETime::Now().Format("%Y-%m-%dT%H:%M:%S.%0",True)
             << " Received ["
             << *(Int*)pData
             << "] length ["
             << length
             << "] from ["
             << from.getAddress()
             << ":"
             << from.getPort()
             << "] to ["
             << to.getAddress()
             << ":"
             << to.getPort()
             << "]"
             << std::endl << std::flush;
   std::cout.imbue(mylocale);

   if (*(Int*)pData == -1)
   {
      if (m_sentcnt != -1)
      {
         m_sentcnt = -1;
         sendpacket();
      }
      getThread().quit();
   }
   else
   {
      //sendpacket();
   }
}

Void UdpSocket::onError()
{
   std::cout << "socket error " << getError() << " occurred on UDP socket during select" << std::endl
             << std::flush;
}

///////////////////////////////////////////////////////////////////////////////////

Void UdpWorker::onInit()
{
   std::cout << "creating local UDP socket" << std::endl << std::flush;
   m_socket = new UdpSocket(*this);

   std::cout.imbue(defaultLocale);
   std::cout << "binding to IP [" << getLocalIp() << "] port [" << getLocalPort() << "]" << std::endl << std::flush;
   std::cout.imbue(mylocale);
   m_socket->bind( getLocalIp(), getLocalPort() );

   ESocket::Address remote( getRemoteIp(), getRemotePort() );
   m_socket->setRemote( remote );
   m_socket->setCount( m_cnt );

   //std::cout << "sending first packet" << std::endl << std::endl << std::flush;
   //m_socket->sendpacket();

   std::cout << "starting the periodic timer" << std::endl << std::endl << std::flush;
   m_timer.setInterval(1000);
   m_timer.setOneShot(False);
   initTimer(m_timer);
   m_timer.start();

   sendMessage(EThreadMessage(EM_UDPWORKER_1));
}

Void UdpWorker::onQuit()
{
   delete m_socket;
}

Void UdpWorker::errorHandler(EError &err, ESocket::BasePrivate *psocket)
{
   //std::cout << "Socket exception - " << err << std::endl << std::flush;
}

Void UdpWorker::onTimer(EThreadEventTimer *pTimer)
{
   if (pTimer->getId() == m_timer.getId())
   {
      m_socket->sendpacket();
      if (m_socket->getSentCount() == -1)
         m_timer.stop();
   }
}

///////////////////////////////////////////////////////////////////////////////////

Void udpsockettest()
{
   static Int messages = 20;
   static UShort localport = 11111;
   static UShort remoteport = 22222;
   // static EString localip = "127.0.0.1";
   // static EString remoteip = "127.0.0.1";
   static EString localip = "::1";
   static EString remoteip = "::1";

   UdpWorker *pWorker = new UdpWorker();
   Char buffer[128];

   cout << "Enter the local IP address for the connection [" << localip << "]: ";
   cin.getline(buffer, sizeof(buffer));
   if (*buffer)
      localip = buffer;
   cout.imbue(defaultLocale);
   cout << "Enter the local port number for the connection [" << localport << "]: ";
   cout.imbue(mylocale);
   cin.getline(buffer, sizeof(buffer));
   localport = buffer[0] ? (UShort)std::stoi(buffer) : localport;

   cout << "Enter the remote IP address for the connection [" << remoteip << "]: ";
   cin.getline(buffer, sizeof(buffer));
   if (*buffer)
      remoteip = buffer;
   cout.imbue(defaultLocale);
   cout << "Enter the remote port number for the connection [" << remoteport << "]: ";
   cout.imbue(mylocale);
   cin.getline(buffer, sizeof(buffer));
   remoteport = buffer[0] ? (UShort)std::stoi(buffer) : remoteport;

   pWorker->setLocalIp(localip);
   pWorker->setLocalPort(localport);
   pWorker->setRemoteIp(remoteip);
   pWorker->setRemotePort(remoteport);

   cout << "Enter number of messages to exchange with the peer [" << messages << "]: ";
   cin.getline(buffer, sizeof(buffer));
   messages = buffer[0] ? std::stoi(buffer) : messages;
   pWorker->setCount(messages);

   pWorker->init(1, 1, NULL);
   pWorker->join();
   delete pWorker;
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

#define EM_RWLOCKTEST (EM_USER + 1)

class ERWLockTestThread : public EThreadPrivate
{
public:
   ERWLockTestThread(ERWLock &rwl, Bool reader, cpStr name)
       : m_rwlock(rwl),
         m_reader(reader),
         m_name(name)
   {
   }

   Void handleRequest(EThreadMessage &msg)
   {
      Int delay = (Int)msg.data().data().int32[0];
      Int hold = (Int)msg.data().data().int32[1];
      ETimer tmr;

      EThreadBasic::sleep(delay);
      cout << "thread [" << m_name << "] starting after " << delay << "ms (" << tmr.MilliSeconds() << ")" << endl
           << flush;

      if (m_reader)
      {
         {
            cout << "thread [" << m_name << "] waiting for read lock" << endl
                 << flush;
            ERDLock rdlck(m_rwlock);
            epctime_t elapsed = tmr.MilliSeconds();
            cout << "thread [" << m_name << "] read lock obtained after " << elapsed << "ms - holding lock for " << hold << "ms" << endl
                 << flush;
            EThreadBasic::sleep(hold);
         }
         cout << "thread [" << m_name << "] read lock released" << endl
              << flush;
      }
      else
      {
         {
            cout << "thread [" << m_name << "] waiting for write lock" << endl
                 << flush;
            EWRLock wrlck(m_rwlock);
            epctime_t elapsed = tmr.MilliSeconds();
            cout << "thread [" << m_name << "] write lock obtained after " << elapsed << "ms - holding lock for " << hold << "ms" << endl
                 << flush;
            EThreadBasic::sleep(hold);
         }
         cout << "thread [" << m_name << "] write lock released" << endl
              << flush;
      }
   }

   DECLARE_MESSAGE_MAP()

private:
   ERWLockTestThread();

   ERWLock &m_rwlock;
   Bool m_reader;
   cpStr m_name;
};

BEGIN_MESSAGE_MAP(ERWLockTestThread, EThreadPrivate)
   ON_MESSAGE(EM_RWLOCKTEST, ERWLockTestThread::handleRequest)
END_MESSAGE_MAP()

Void ERWLock_test()
{
   cout << "ERWLock_test() Start" << endl;

   ERWLock rwl;

   ERWLockTestThread read1(rwl, True, "READ1");
   ERWLockTestThread read2(rwl, True, "READ2");
   ERWLockTestThread write1(rwl, False, "WRITE1");

   cout << "ERWLock_test - initializing threads" << endl
        << flush;
   read1.init(1, 1, NULL, 20000);
   read2.init(1, 2, NULL, 20000);
   write1.init(1, 3, NULL, 20000);

   cout << "ERWLock_test - starting 1st test" << endl
        << flush;
   read1.sendMessage(EThreadMessage(EM_RWLOCKTEST, 0, 4000));
   read2.sendMessage(EThreadMessage(EM_RWLOCKTEST, 50, 4000));
   write1.sendMessage(EThreadMessage(EM_RWLOCKTEST, 1000, 4000));
   EThreadBasic::sleep(10000);
   cout << "ERWLock_test - 1st test complete" << endl
        << flush;

   cout << "ERWLock_test - starting 2nd test" << endl
        << flush;
   read1.sendMessage(EThreadMessage(EM_RWLOCKTEST, 1000, 4000));
   read2.sendMessage(EThreadMessage(EM_RWLOCKTEST, 1050, 4000));
   write1.sendMessage(EThreadMessage(EM_RWLOCKTEST, 0, 4000));
   EThreadBasic::sleep(10000);
   cout << "ERWLock_test - 2nd test complete" << endl
        << flush;

   read1.quit();
   read2.quit();
   write1.quit();

   read1.join();
   read2.join();
   write1.join();

   cout << "ERWLock_test() Complete" << endl;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Void EGetOpt_test(EGetOpt &opt)
{
   opt.print();

   {
      Long qs1 = opt.get("/Logger/QueueSize", -1);
      Long qs2 = opt.get("/Logger/QueueSize/", -1);
      Long qs3 = opt.get("Logger/QueueSize", -1);
      Long qs4 = opt.get("Logger/QueueSize/", -1);
      std::cout << "No prefix qs1=" << qs1 << " qs2=" << qs2 << " qs3=" << qs3 << " qs4=" << qs4 << std::endl;
   }

   {
      opt.setPrefix("/EpcTools");
      Long qs1 = opt.get("/Logger/QueueSize", -1);
      Long qs2 = opt.get("/Logger/QueueSize/", -1);
      Long qs3 = opt.get("Logger/QueueSize", -1);
      Long qs4 = opt.get("Logger/QueueSize/", -1);
      std::cout << "prefix=\"/EpcTools\" qs1=" << qs1 << " qs2=" << qs2 << " qs3=" << qs3 << " qs4=" << qs4 << std::endl;
   }

   {
      opt.setPrefix("/EpcTools/");
      Long qs1 = opt.get("/Logger/QueueSize", -1);
      Long qs2 = opt.get("/Logger/QueueSize/", -1);
      Long qs3 = opt.get("Logger/QueueSize", -1);
      Long qs4 = opt.get("Logger/QueueSize/", -1);
      std::cout << "prefix=\"/EpcTools/\" qs1=" << qs1 << " qs2=" << qs2 << " qs3=" << qs3 << " qs4=" << qs4 << std::endl;
   }

   {
      opt.setPrefix("EpcTools");
      Long qs1 = opt.get("/Logger/QueueSize", -1);
      Long qs2 = opt.get("/Logger/QueueSize/", -1);
      Long qs3 = opt.get("Logger/QueueSize", -1);
      Long qs4 = opt.get("Logger/QueueSize/", -1);
      std::cout << "prefix=\"EpcTools\" qs1=" << qs1 << " qs2=" << qs2 << " qs3=" << qs3 << " qs4=" << qs4 << std::endl;
   }

   {
      opt.setPrefix("EpcTools/");
      Long qs1 = opt.get("/Logger/QueueSize", -1);
      Long qs2 = opt.get("/Logger/QueueSize/", -1);
      Long qs3 = opt.get("Logger/QueueSize", -1);
      Long qs4 = opt.get("Logger/QueueSize/", -1);
      std::cout << "prefix=\"EpcTools/\" qs1=" << qs1 << " qs2=" << qs2 << " qs3=" << qs3 << " qs4=" << qs4 << std::endl;
   }

   opt.setPrefix("");
   std::cout << "/EpcTools/EnablePublicObjects=" << (opt.get("/EpcTools/EnablePublicObjects", false) ? "true" : "false") << std::endl;
   std::cout << "/EpcTools/Logger/ApplicationName=" << opt.get("/EpcTools/Logger/ApplicationName", "undefined") << std::endl;
   std::cout << "/EpcTools/Logger/QueueSize=" << opt.get("/EpcTools/Logger/QueueSize", -1) << std::endl;
   std::cout << "/EpcTools/Logger/SinkSets/0/Sinks/4/MaxNumberFiles=" << opt.get("/EpcTools/Logger/SinkSets/0/Sinks/4/MaxNumberFiles", -1) << std::endl;

   {
      UInt cnt = opt.getCount("EpcTools/Logger/SinkSets");
      std::cout << "EpcTools/Logger/SinkSets count=" << cnt << std::endl;
      for (UInt i = 0; i < cnt; i++)
      {
         Long sinkid = opt.get(i, "EpcTools/Logger/SinkSets", "SinkID", -1);
         std::cout << "EpcTools/Logger/SinkSets/" << i << "/SinkID=" << sinkid << std::endl;

         EString path;
         path.format("/EpcTools/Logger/SinkSets/%d/Sinks", i);
         UInt cnt2 = opt.getCount(path.c_str());
         std::cout << path << " count=" << cnt2 << std::endl;
         for (UInt j = 0; j < cnt2; j++)
            std::cout << path << "/" << j << "/SinkType = " << opt.get(j, path, "SinkType", "unknown") << std::endl;
      }
   }
}

#define LOG_SYSTEM 1
#define LOG_TEST1 2
#define LOG_TEST2 3
#define LOG_TEST3 4

#define LOG_TEST3_SINKSET 3

Void ELogger_test()
{
   // spdlog::init_thread_pool(8192, 1);
   // ELogger::applicationName("testapp");

   // spdlog::sink_ptr s1 = std::make_shared<spdlog::sinks::syslog_sink_mt>("",LOG_USER,0,true);
   // spdlog::sink_ptr s2 = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
   // spdlog::sink_ptr s3 = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("mylog",10*1024*1024,2);

   // ELogger::createSinkSet( STANDARD_SINKSET );

   // ELogger::sinkSet(STANDARD_SINKSET).addSink( s1, ELogger::eWarn, "[__APPNAME__] [%n] [%l] %v" );
   // ELogger::sinkSet(STANDARD_SINKSET).addSink( s2, ELogger::eTrace );
   // ELogger::sinkSet(STANDARD_SINKSET).addSink( s3, ELogger::eTrace );

   // ELogger::createLog( LOG_SYSTEM, "system", STANDARD_SINKSET );
   // ELogger::createLog( LOG_TEST1, "test1", STANDARD_SINKSET );
   // ELogger::createLog( LOG_TEST2, "test2", STANDARD_SINKSET );

   // ELogger::log(LOG_SYSTEM).set_level( ELogger::eTrace );
   // ELogger::log(LOG_TEST1).set_level( ELogger::eTrace );
   // ELogger::log(LOG_TEST2).set_level( ELogger::eTrace );

   ELogger::log(LOG_SYSTEM).debug("Hello Wonderful World from the system log!!");

   ELogger::log(LOG_SYSTEM).debug("Hello {} from the system log!!", "World");
   ELogger::log(LOG_SYSTEM).info("Hello {} from the system log!!", "World");
   ELogger::log(LOG_SYSTEM).startup("Hello {} from the system log!!", "World");
   ELogger::log(LOG_SYSTEM).minor("Hello {} from the system log!!", "World");
   ELogger::log(LOG_SYSTEM).major("Hello {} from the system log!!", "World");
   ELogger::log(LOG_SYSTEM).critical("Hello {} from the system log!!", "World");

   ELogger::log(LOG_TEST1).debug("Hello {} from the test1 log!!", "World");
   ELogger::log(LOG_TEST1).info("Hello {} from the test1 log!!", "World");
   ELogger::log(LOG_TEST1).startup("Hello {} from the test1 log!!", "World");
   ELogger::log(LOG_TEST1).minor("Hello {} from the test1 log!!", "World");
   ELogger::log(LOG_TEST1).major("Hello {} from the test1 log!!", "World");
   ELogger::log(LOG_TEST1).critical("Hello {} from the test1 log!!", "World");

   ELogger::log(LOG_TEST2).debug("Hello {} from the test2 log!!", "World");
   ELogger::log(LOG_TEST2).info("Hello {} from the test2 log!!", "World");
   ELogger::log(LOG_TEST2).startup("Hello {} from the test2 log!!", "World");
   ELogger::log(LOG_TEST2).minor("Hello {} from the test2 log!!", "World");
   ELogger::log(LOG_TEST2).major("Hello {} from the test2 log!!", "World");
   ELogger::log(LOG_TEST2).critical("Hello {} from the test2 log!!", "World");

   ELogger::log(LOG_SYSTEM).flush();
   ELogger::log(LOG_TEST1).flush();
   ELogger::log(LOG_TEST2).flush();

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   ELogger::createSinkSet(LOG_TEST3_SINKSET);
   std::shared_ptr<ELoggerSink> sp = std::make_shared<ELoggerSinkBasicFile>(
       ELogger::eDebug, ELoggerSink::getDefaultPattern(), "mylog", true);
   ELogger::sinkSet(LOG_TEST3_SINKSET).addSink(sp);

   ELogger::createLog(LOG_TEST3, "test3", LOG_TEST3_SINKSET);

   ELogger::log(LOG_TEST3).setLogLevel(ELogger::eInfo);

   ELogger::log(LOG_TEST3).debug("Hello {} from the test3 log!!", "World");
   ELogger::log(LOG_TEST3).info("Hello {} from the test3 log!!", "World");
   ELogger::log(LOG_TEST3).startup("Hello {} from the test3 log!!", "World");
   ELogger::log(LOG_TEST3).minor("Hello {} from the test3 log!!", "World");
   ELogger::log(LOG_TEST3).major("Hello {} from the test3 log!!", "World");
   ELogger::log(LOG_TEST3).critical("Hello {} from the test3 log!!", "World");
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

#define EM_TIMERPOOLTEST1 (EM_USER + 1)
#define EM_TIMERPOOLTEST2 (EM_USER + 2)
#define EM_TIMERPOOLTEST3 (EM_USER + 3)
#define EM_TIMERPOOLTEST4 (EM_USER + 4)

class TimerPoolTestThread : public EThreadPrivate
{
public:

   Void userFunc1(EThreadMessage &msg)
   {
      std::cout << ETime::Now().Format("%Y-%m-%d %H:%M:%S.%0", True) << " - userFunc1 - msg data " << msg.data().data().int64 << std::endl << std::flush;
   }

   Void userFunc2(EThreadMessage &msg)
   {
      std::cout << ETime::Now().Format("%Y-%m-%d %H:%M:%S.%0", True) << " - userFunc2 - msg data " << msg.data().data().int64 << std::endl << std::flush;
   }

   Void userFunc3(EThreadMessage &msg)
   {
      std::cout << ETime::Now().Format("%Y-%m-%d %H:%M:%S.%0", True) << " - userFunc3 - msg data " << msg.data().data().int64 << std::endl << std::flush;
   }

   Void userFunc4(EThreadMessage &msg)
   {
      std::cout << ETime::Now().Format("%Y-%m-%d %H:%M:%S.%0", True) << " - userFunc4 - msg data " << msg.data().data().int64 << std::endl << std::flush;
   }

   DECLARE_MESSAGE_MAP()
};

Void timerpoolcallback(ULong id, pVoid data)
{
   std::cout << ETime::Now().Format("%Y-%m-%d %H:%M:%S.%0", True) << " - timerpoolcallback for " << id << " - data " << (int64_t)data << std::endl << std::flush;
}

BEGIN_MESSAGE_MAP(TimerPoolTestThread, EThreadPrivate)
   ON_MESSAGE(EM_TIMERPOOLTEST1, TimerPoolTestThread::userFunc1)
   ON_MESSAGE(EM_TIMERPOOLTEST2, TimerPoolTestThread::userFunc2)
   ON_MESSAGE(EM_TIMERPOOLTEST3, TimerPoolTestThread::userFunc3)
   ON_MESSAGE(EM_TIMERPOOLTEST4, TimerPoolTestThread::userFunc4)
END_MESSAGE_MAP()

Void timerpooltest()
{
   ETimerPool::Instance().setResolution( 50 );
   ETimerPool::Instance().setRounding( ETimerPool::Rounding::up );

   std::cout << "ETimerPool Settings" << std::endl
      << "\tresolution = " << ETimerPool::Instance().getResolution() << std::endl;
   std::cout
      << "\trounding = " << (ETimerPool::Instance().getRounding()==ETimerPool::Rounding::up?"UP":"DOWN") << std::endl;
   std::cout
      << "\ttimer signal = " << strsignal(ETimerPool::Instance().getTimerSignal()) << std::endl;
   std::cout
      << "\tquit signal = " << strsignal(ETimerPool::Instance().getQuitSignal()) << std::endl;
   
   std::cout << "Initializing ETimerPool..." << std::flush;
   ETimerPool::Instance().init();
   std::cout << "complete" << std::endl << std::flush;

   std::cout << "Starting TimerPoolTestThread..." << std::flush;
   TimerPoolTestThread t;
   t.init(1, 1, NULL, 200000);
   std::cout << "complete" << std::endl << std::endl << std::flush;

   ETime now;
   std::cout << "Starting registrations at tv_sec=" << now.getTimeVal().tv_sec << " tv_usec=" << now.getTimeVal().tv_usec << std::endl << std::flush;
   ULong id1 = ETimerPool::Instance().registerTimer(1000, new EThreadMessage(EM_TIMERPOOLTEST1,11111111111111), t);
   std::cout << ETime::Now().Format("%Y-%m-%d %H:%M:%S.%0", True) << " registered timer " << id1 << " for 1000ms" << std::endl << std::flush;
   ETimerPool::Instance().dump();
   ULong id2 = ETimerPool::Instance().registerTimer(2500, new EThreadMessage(EM_TIMERPOOLTEST2,22222222222222), t);
   std::cout << ETime::Now().Format("%Y-%m-%d %H:%M:%S.%0", True) << " registered timer " << id2 << " for 2500ms" << std::endl << std::flush;
   ETimerPool::Instance().dump();
   // ULong id3 = ETimerPool::Instance().registerTimer(4123, new EThreadMessage(EM_TIMERPOOLTEST3,33333333333333), t);
   ULong id3 = ETimerPool::Instance().registerTimer(4123, timerpoolcallback, (pVoid)33333333333333);
   std::cout << ETime::Now().Format("%Y-%m-%d %H:%M:%S.%0", True) << " registered timer " << id3 << " for 4123ms" << std::endl << std::flush;
   ETimerPool::Instance().dump();
   // ULong id4 = ETimerPool::Instance().registerTimer(1000, new EThreadMessage(EM_TIMERPOOLTEST4,44444444444444), t);
   ULong id4 = ETimerPool::Instance().registerTimer(1000, timerpoolcallback, (pVoid)44444444444444);
   std::cout << ETime::Now().Format("%Y-%m-%d %H:%M:%S.%0", True) << " registered timer " << id4 << " for 1000ms" << std::endl << std::flush;
   ETimerPool::Instance().dump();
   now = ETime::Now();
   std::cout << "Completed registrations at tv_sec=" << now.getTimeVal().tv_sec << " tv_usec=" << now.getTimeVal().tv_usec << std::endl << std::flush;

   std::cout << "Sleeping for 10 seconds" << std::endl << std::flush;
   EThreadBasic::sleep(10000);
   ETimerPool::Instance().dump();

   now = ETime::Now();
   std::cout << "Starting registrations at tv_sec=" << now.getTimeVal().tv_sec << " tv_usec=" << now.getTimeVal().tv_usec << std::endl;
   id1 = ETimerPool::Instance().registerTimer(1000, new EThreadMessage(EM_TIMERPOOLTEST1,11111111111111), t);
   std::cout << ETime::Now().Format("%Y-%m-%d %H:%M:%S.%0", True) << " registered timer " << id1 << " for 1000ms" << std::endl << std::flush;
   ETimerPool::Instance().dump();
   id2 = ETimerPool::Instance().registerTimer(2500, new EThreadMessage(EM_TIMERPOOLTEST2,22222222222222), t);
   std::cout << ETime::Now().Format("%Y-%m-%d %H:%M:%S.%0", True) << " registered timer " << id2 << " for 2500ms" << std::endl << std::flush;
   ETimerPool::Instance().dump();
   // id3 = ETimerPool::Instance().registerTimer(4123, new EThreadMessage(EM_TIMERPOOLTEST3,33333333333333), t);
   id3 = ETimerPool::Instance().registerTimer(4123, timerpoolcallback, (pVoid)33333333333333);
   std::cout << ETime::Now().Format("%Y-%m-%d %H:%M:%S.%0", True) << " registered timer " << id3 << " for 4123ms" << std::endl << std::flush;
   ETimerPool::Instance().dump();
   // id4 = ETimerPool::Instance().registerTimer(1000, new EThreadMessage(EM_TIMERPOOLTEST4,44444444444444), t);
   id4 = ETimerPool::Instance().registerTimer(1000, timerpoolcallback, (pVoid)44444444444444);
   std::cout << ETime::Now().Format("%Y-%m-%d %H:%M:%S.%0", True) << " registered timer " << id4 << " for 1000ms" << std::endl << std::flush;
   ETimerPool::Instance().dump();
   now = ETime::Now();
   std::cout << "Completed registrations at tv_sec=" << now.getTimeVal().tv_sec << " tv_usec=" << now.getTimeVal().tv_usec << std::endl << std::flush;

   now = ETime::Now();
   std::cout << "Starting unregistrations at tv_sec=" << now.getTimeVal().tv_sec << " tv_usec=" << now.getTimeVal().tv_usec << std::endl << std::flush;
   std::cout << "Unregistering " << id1 << std::endl << std::flush;
   ETimerPool::Instance().unregisterTimer( id1 );
   ETimerPool::Instance().dump();
   std::cout << "Unregistering " << id3 << std::endl << std::flush;
   ETimerPool::Instance().unregisterTimer( id3 );
   ETimerPool::Instance().dump();
   now = ETime::Now();
   std::cout << "Completed unregistrations at tv_sec=" << now.getTimeVal().tv_sec << " tv_usec=" << now.getTimeVal().tv_usec << std::endl << std::flush;

   now = ETime::Now();
   std::cout << "Calling ETimerPool::Instance().uninit()" << std::endl << std::flush;
   ETimerPool::Instance().uninit(True);
   now = ETime::Now();
   std::cout << "Completed ETimerPool::Instance().uninit() at tv_sec=" << now.getTimeVal().tv_sec << " tv_usec=" << now.getTimeVal().tv_usec << std::endl << std::flush;
   
   t.quit();
   t.join();
   std::cout << "ETimerPool test complete" << std::endl << std::flush;
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

//#define ROTATELEFT(x, b) (uint64_t)(((x) << (b)) | ((x) >> (64 - (b))))
#define ROTATELEFT(val,len) (ULongLong)(((val) << (len)) | ((val) >> (64 - (len))))

template <class T>
std::string hexFormatWithoutCommas(T value, int width = sizeof(T)*2)
{
   struct Numpunct : public std::numpunct<char>
   {
   protected:
      virtual char do_thousands_sep() const { return ' '; }
      virtual std::string do_grouping() const { return ""; }
   };
   std::stringstream ss;
   ss.imbue({std::locale(), new Numpunct});
   ss << std::setfill('0') << std::setw(width) << std::hex << value;
   return ss.str();
}

Void printObjectSizes()
{
   {
      ULongLong a = 0x100000000ull;
   
      for (int i=0; i<64; i++)
      {
         std::cout << "ROTATELEFT(0x" << hexFormatWithoutCommas(a) << "," << i+1 << ") = 0x" << hexFormatWithoutCommas(ROTATELEFT(a,i+1)) << std::endl;
      }
   }

   ULongLong a = 0x1234567890123456ull;
   std::cout << "ULongLong a=" << a << " (0x" << std::hex << a << ")" << std::endl;
   std::cout << "sizeof(size_t) = " << sizeof(size_t) << std::endl;
   std::cout << "sizeof(EThreadMessage) = " << sizeof(EThreadMessage) << std::endl;
   std::cout << "sizeof(_EThreadMessage) = " << sizeof(_EThreadMessage) << std::endl;
   std::cout << "sizeof(_EThreadEventMessageBase) = " << sizeof(_EThreadEventMessageBase) << std::endl;
   std::cout << "sizeof(EThreadEventMessageData) = " << sizeof(EThreadEventMessageData) << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

class MyCustomEventData : public EThreadEventMessageDataBase
{
public:
   MyCustomEventData() : EThreadEventMessageDataBase(), m_voidptr(), m_print(False), m_val() {}
   MyCustomEventData(UInt msgid) : EThreadEventMessageDataBase(msgid), m_voidptr(), m_val() {}

   pVoid getVoidPtr() { return m_voidptr; }
   Void setVoidPtr(pVoid p) { m_voidptr = p; }

   Void setValue(Int idx, Int val) { if (idx>=0 && idx<4) m_val[idx] = val; }
   Int getValue(Int idx) { if (idx>=0 && idx<4) return m_val[idx]; return -1; }

   Void setPrint(Bool print) { m_print = print; }
   Bool getPrint() { return m_print; }

private:
   pVoid m_voidptr;
   Bool m_print;
   Int m_val[4];
};

typedef EThreadEventMessageBase<MyCustomEventData> MyCustomEvent;

#define MYAPPID      9
#define MYTHREADID   99
#define MYEVENT_1    (EM_USER + 1)

typedef EThreadQueuePublic<MyCustomEvent> MyCustomEventPublicQueue;
typedef EThreadEvent<MyCustomEventPublicQueue, MyCustomEvent> MyCustomThreadEventPublic;

class MyCustomThread : public MyCustomThreadEventPublic
{
public:
   MyCustomThread()
   {
      m_count = 0;
   }

   Void onInit()
   {
      std::cout << "MyCustomThread::onInit()" << std::endl << std::flush;
   }

   Void onQuit()
   {
      m_timer.Stop();

      std::cout << "MyCustomThread::onQuit()" << std::endl << std::flush;

      double persec = ((double)m_count) / (((double)m_timer.MicroSeconds()) / 1000000);
      std::string s = numberFormatWithCommas<double>(persec);
      cout << "Processed " << m_count << " messages in " << ((double)m_timer.MicroSeconds()) / 1000000 << " seconds (" << s << " per second)" << std::endl;
   }

   Void myfunc1(MyCustomEvent &msg)
   {
      if (m_count == 0)
         m_timer.Start();

      m_count++;

      if (msg.data().getPrint())
      {
         std::cout << "MyCustomThread::myfunc1() -"
            << " event=" << msg.data().getMessageId()
            << " m_voidptr=" << msg.data().getVoidPtr()
            << " m_print=" << (msg.data().getPrint()?"TRUE":"FALSE")
            << " m_val[0]=" << msg.data().getValue(0)
            << " m_val[1]=" << msg.data().getValue(1)
            << " m_val[2]=" << msg.data().getValue(2)
            << " m_val[3]=" << msg.data().getValue(3)
            << std::endl;
      }
   }

   DECLARE_MESSAGE_MAP()
private:
   Int m_maxprintindex;
   Int m_count;
   ETimer m_timer;
};

BEGIN_MESSAGE_MAP(MyCustomThread, MyCustomThreadEventPublic)
   ON_MESSAGE(MYEVENT_1, MyCustomThread::myfunc1)
END_MESSAGE_MAP()

Void customThreadTest(Bool isHost)
{
   static Int queueSize = 100000;
   Char buffer[128];

   cout << "Enter the public event queue size [" << queueSize << "]: ";
   cin.getline(buffer, sizeof(buffer));
   queueSize = *buffer ? std::stoi(buffer) : queueSize;

   if (isHost)
   {
      MyCustomThread t;
      std::cout << "customThreadTest() starting custom thread" << std::endl << std::flush;
      t.init(MYAPPID, MYTHREADID, NULL, queueSize);
      t.join();
      std::cout << "customThreadTest() custom thread test complete" << std::endl << std::flush;
   }
   else
   {
      static Int msgcnt = 10000;
      static Int maxPrintIndex = 1;

      cout << "Enter the number of messages to send [" << msgcnt << "]: ";
      cin.getline(buffer, sizeof(buffer));
      msgcnt = *buffer ? std::stoi(buffer) : msgcnt;

      cout << "Enter the number of messages to print [" << maxPrintIndex << "]: ";
      cin.getline(buffer, sizeof(buffer));
      maxPrintIndex = *buffer ? std::stoi(buffer) : maxPrintIndex;

      MyCustomEventPublicQueue q;
      // thread id (t) 0-9999
      // thread type (x) thread 1, worker 2
      // application id (a) 0 - ...
      // aaaaaxtttt
      Long id = MYAPPID * 100000 + 10000 + MYTHREADID;
      q.init(queueSize, id, True, EThreadQueueMode::WriteOnly);

      MyCustomEvent event;
      event.data().setMessageId(MYEVENT_1);
      event.data().setPrint(True);
      Int idx;
      for (idx=0; idx<msgcnt; idx++)
      {
         if (idx == maxPrintIndex)
            event.data().setPrint(False);

         event.data().setValue(0,100000000+idx);
         event.data().setValue(1,200000000+idx);
         event.data().setValue(2,300000000+idx);
         event.data().setValue(3,400000000+idx);

         q.push(event);
      }
      q.push(MyCustomEvent(EM_QUIT));
      cout << std::endl << "Posted " << idx << " MyCustomEvent event messages to public queue " << id << std::endl;
   }
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

#define MYEVENT (EM_USER + 1)

class MyThread : public EThreadPrivate
{
public:
   MyThread(Long periodic_ms, Long overall_ms)
   {
      m_periodic_ms = periodic_ms;
      m_overall_ms = overall_ms;
      m_count = 0;
   }

   Void onInit()
   {
      std::cout << ETime::Now().Format("%i",True) << " " << __PRETTY_FUNCTION__ << " invoked" << std::endl << std::flush;

      std::cout << ETime::Now().Format("%i",True) << " " << __PRETTY_FUNCTION__ << " initializing periodic timer(" << m_periodic.getId() << ") to " << m_periodic_ms << "ms" << std::endl << std::flush;
      m_periodic.setInterval(m_periodic_ms);
      m_periodic.setOneShot(False);
      initTimer(m_periodic);

      std::cout << ETime::Now().Format("%i",True) << " " << __PRETTY_FUNCTION__ << " initializing overall timer(" << m_overall.getId() << ") to " << m_overall_ms << "ms" << std::endl << std::flush;
      m_overall.setInterval(m_overall_ms);
      m_overall.setOneShot(False);
      initTimer(m_overall);

      std::cout << ETime::Now().Format("%i",True) << " " << __PRETTY_FUNCTION__ << " starting periodic timer" << "ms" << std::endl << std::flush;
      m_periodic.start();
      std::cout << ETime::Now().Format("%i",True) << " " << __PRETTY_FUNCTION__ << " starting overall timer" << "ms" << std::endl << std::flush;
      m_overall.start();

      m_count = 0;
   }

   Void onQuit()
   {
      std::cout << ETime::Now().Format("%i",True) << "  " << __PRETTY_FUNCTION__ << " invoked - count=" << m_count << std::endl << std::flush;
   }

   Void onTimer(EThreadEventTimer *ptimer)
   {
      if (ptimer->getId() == m_periodic.getId())
      {
         sendMessage(MYEVENT);
      }
      else if (ptimer->getId() == m_overall.getId())
      {
         quit();
      }
   }  

   Void myhandler(EThreadMessage &msg)
   {
      std::cout << ETime::Now().Format("%i",True) << "  " << __PRETTY_FUNCTION__ << " invoked" << std::endl;
      m_count++;
   }

   DECLARE_MESSAGE_MAP()

private:
   MyThread();
   Long m_periodic_ms;
   Long m_overall_ms;
   Int m_count;
   EThreadEventTimer m_periodic;
   EThreadEventTimer m_overall;
};

BEGIN_MESSAGE_MAP(MyThread, EThreadPrivate)
   ON_MESSAGE(MYEVENT, MyThread::myhandler)
END_MESSAGE_MAP()

Void threadExample()
{
   static Long periodic_ms = 1000;
   static Long overall_ms = 10000;
   Char buffer[128];

   cout << "Enter the periodic timer duration in milliseconds [" << periodic_ms << "]: ";
   cin.getline(buffer, sizeof(buffer));
   periodic_ms = *buffer ? std::stol(buffer) : periodic_ms;

   cout << "Enter the overall timer duration in milliseconds [" << overall_ms << "]: ";
   cin.getline(buffer, sizeof(buffer));
   overall_ms = *buffer ? std::stol(buffer) : overall_ms;

   MyThread t(periodic_ms, overall_ms);

   std::cout << ETime::Now().Format("%i",True) << "  Starting thread example" << std::endl;
   t.init(1,1,NULL);
   t.join();
   std::cout << ETime::Now().Format("%i",True) << "  Thread example complete" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

#define MYPUBLICAPPID      10
#define MYPUBLICTHREADID   1
#define MYPUBLICEVENT      (EM_USER + 1)

class MyPublicThread : public EThreadPublic
{
public:
   MyPublicThread()
   {
      m_count = 0;
   }

   Void onInit()
   {
      std::cout << "MyPublicThread::onInit()" << std::endl << std::flush;
   }

   Void onQuit()
   {
      m_timer.Stop();

      std::cout << "MyPublicThread::onQuit()" << std::endl << std::flush;

      double persec = ((double)m_count) / (((double)m_timer.MicroSeconds()) / 1000000);
      std::string s = numberFormatWithCommas<double>(persec);
      cout << "Processed " << m_count << " messages in " << ((double)m_timer.MicroSeconds()) / 1000000 << " seconds (" << s << " per second)" << std::endl;
   }

   Void myfunc1(EThreadMessage &msg)
   {
      if (m_count == 0)
         m_timer.Start();
      m_count++;
   }

   DECLARE_MESSAGE_MAP()
private:
   Int m_count;
   ETimer m_timer;
};

BEGIN_MESSAGE_MAP(MyPublicThread, EThreadPublic)
   ON_MESSAGE(MYPUBLICEVENT, MyPublicThread::myfunc1)
END_MESSAGE_MAP()

Void publicThreadExample(Bool isHost)
{
   static Int queueSize = 100000;
   Char buffer[128];

   cout << "Enter the public event queue size [" << queueSize << "]: ";
   cin.getline(buffer, sizeof(buffer));
   queueSize = *buffer ? std::stoi(buffer) : queueSize;

   if (isHost)
   {
      MyPublicThread t;
      std::cout << "publicThreadExample() starting public thread" << std::endl << std::flush;
      t.init(MYPUBLICAPPID, MYPUBLICTHREADID, NULL, queueSize);
      t.join();
      std::cout << "publicThreadExample() public thread test complete" << std::endl << std::flush;
   }
   else
   {
      static Int msgcnt = 10000;

      cout << "Enter the number of messages to send [" << msgcnt << "]: ";
      cin.getline(buffer, sizeof(buffer));
      msgcnt = *buffer ? std::stoi(buffer) : msgcnt;

      EThreadQueuePublic<EThreadMessage> q;
      Long id = MYPUBLICAPPID * 100000 + 10000 + MYPUBLICTHREADID;
      q.init(queueSize, id, True, EThreadQueueMode::WriteOnly);

      EThreadMessage event;
      event.data().setMessageId(MYPUBLICEVENT);
      Int idx;
      for (idx=0; idx<msgcnt; idx++)
      {
         event.data().data().int64 = idx;
         q.push(event);
      }
      q.push(EThreadMessage(EM_QUIT));
      cout << std::endl << "Posted " << idx << " event messages to public queue " << id << std::endl;
   }
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

extern "C" Void NodeSelector_test_callback(EPCDNS::NodeSelector &ns, cpVoid data)
{
   //EPCDNS::PGWUPFNodeSelector *s = (EPCDNS::PGWUPFNodeSelector*)data;
   std::cout << "*********** Asynchronous Node Selector ***********" << std::endl;
   std::cout << "NodeSelector_test_callback() - data = 0x" << hexFormatWithoutCommas((ULongLong)data) << std::endl;
   ns.dump();
   std::cout << "*************************************************" << std::endl;
}

Void NodeSelector_test()
{
   DNS::Cache::setRefreshConcurrent( 2 );
   DNS::Cache::setRefreshPercent( 70 );
   DNS::Cache::setRefreshInterval( 4000 );
   DNS::Cache::getInstance().addNamedServer("192.168.3.156");
   DNS::Cache::getInstance().applyNamedServers();

   EPCDNS::PGWUPFNodeSelector s1( "apn1", "120", "310" );
   s1.setNamedServerID(DNS::NS_DEFAULT);
   s1.addDesiredProtocol( EPCDNS::UPFAppProtocolEnum::upf_x_sxb );
   s1.addDesiredNetworkCapability( "lbo" );
   s1.process();
   std::cout << "*********** Synchronous Node Selector ***********" << std::endl;
   s1.dump();
   std::cout << "*************************************************" << std::endl;

   EPCDNS::PGWUPFNodeSelector s2( "apn1", "120", "310" );
   s2.setNamedServerID(DNS::NS_DEFAULT);
   s2.addDesiredProtocol( EPCDNS::UPFAppProtocolEnum::upf_x_sxb );
   s2.addDesiredNetworkCapability( "lbo" );
   s2.process(&s2, NodeSelector_test_callback);

   std::cout << "sleeping for 2 seconds" << std::endl;
   EThreadBasic::sleep(2000);

   std::cout << std::endl << "NodeSelector_test() complete" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

Void eipfilterruleTest()
{
   EIpFilterRule r;
   
   r = "permit in ip from any to any";       r.dump();   std::cout << std::endl;
   r = "deny out ip from any to any";        r.dump();   std::cout << std::endl;
   r = "permit in 0 from 1.2.3.4 to 5.6.7.8"; r.dump();  std::cout << std::endl;
   r = "permit in 0 from 1.2.3.4/24 to 5.6.7.8/17"; r.dump();  std::cout << std::endl;
   r = "permit in 0 from 1.2.3.4/24 111 to 5.6.7.8/17 111,222,333-444,555-666"; r.dump();  std::cout << std::endl;
   r = "permit in ip from any to any frag ipoptions aaaaa tcpoptions bbbbb established setup tcpflags fin,syn,rst,psh,ack,urg icmptypes 0,12,3,4,5";       r.dump();   std::cout << std::endl;

   EIpAddress a("1.2.3.4/25"), b("5.6.7.8/17"), c("5.6.7.8/18");
   std::cout << "std::hash(" << a.address() << ") = " << std::hash<EIpAddress>{}(a) << std::endl;
   std::cout << "std::hash(" << b.address() << ") = " << std::hash<EIpAddress>{}(b) << std::endl;
   std::cout << "std::hash(" << c.address() << ") = " << std::hash<EIpAddress>{}(c) << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

Void octetStringTest()
{
   cpChar str = "A character string";
   UChar buf[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 };
   {
      EOctetString os;
      std::cout << "EOctetString() length=" << os.length() << " capacity=" << os.capacity() << " { " << os << " }" << std::endl;
   }
   {
      EOctetString os(50);
      std::cout << "EOctetString(50) length=" << os.length() << " capacity=" << os.capacity() << " { " << os << " }" << std::endl;
   }
   {
      EOctetString os(str);
      std::cout << "EOctetString(\"" << str << "\") length=" << os.length() << " capacity=" << os.capacity() << " { " << os << " }" << std::endl;
   }
   {
      EOctetString os(buf, sizeof(buf));
      std::cout << "EOctetString({1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20}) length=" << os.length() << " capacity=" << os.capacity() << " { " << os << " }" << std::endl;
   }
   {
      EOctetString os1(buf, sizeof(buf));
      EOctetString os2(os1);
      std::cout << "EOctetString os2(os1) length=" << os2.length() << " capacity=" << os2.capacity() << " { " << os2 << " }" << std::endl;
   }
   {
      EOctetString os1(buf, sizeof(buf));
      EOctetString os2(os1, 5);
      std::cout << "EOctetString os2(os1,5) length=" << os2.length() << " capacity=" << os2.capacity() << " { " << os2 << " }" << std::endl;
   }
   {
      EOctetString os1(buf, sizeof(buf));
      EOctetString os2(os1, 5, 10);
      std::cout << "EOctetString os2(os1,5,10) length=" << os2.length() << " capacity=" << os2.capacity() << " { " << os2 << " }" << std::endl;
   }
   {
      EOctetString os1;
      EOctetString os2(buf, sizeof(buf));
      os1 = os2;
      std::cout << "os1=os2 length=" << os1.length() << " capacity=" << os1.capacity() << " { " << os1 << " }" << std::endl;

      os1=str;
      std::cout << "os1=os2 length=" << os1.length() << " capacity=" << os1.capacity() << " { " << os1 << " }" << std::endl;

      os1='A';
      std::cout << "os1='x' length=" << os1.length() << " capacity=" << os1.capacity() << " { " << os1 << " }" << std::endl;

      os1=buf[0];
      std::cout << "os1=255 length=" << os1.length() << " capacity=" << os1.capacity() << " { " << os1 << " }" << std::endl;

      os1 = std::move(os2);
      std::cout << "os1=std::move(os2) os1 length=" << os1.length() << " capacity=" << os1.capacity() << " { " << os1 << " }" << std::endl;
      std::cout << "                   os2 length=" << os2.length() << " capacity=" << os2.capacity() << " { " << os2 << " }" << std::endl;
   }
   {
      EOctetString os1;
      EOctetString os2(buf, 8);

      std::cout << "os1 length=" << os1.length() << " capacity=" << os1.capacity() << " { " << os1 << " }" << std::endl;
      os1 += os2;
      std::cout << "os1+=os2 length=" << os1.length() << " capacity=" << os1.capacity() << " { " << os1 << " }" << std::endl;
      os1 += os2;
      std::cout << "os1+=os2 length=" << os1.length() << " capacity=" << os1.capacity() << " { " << os1 << " }" << std::endl;
      os1.assign(buf,8);
      std::cout << "os1 length=" << os1.length() << " capacity=" << os1.capacity() << " { " << os1 << " }" << std::endl;
      os1 += str;
      std::cout << "os1+=str length=" << os1.length() << " capacity=" << os1.capacity() << " { " << os1 << " }" << std::endl;
      os1+='A';
      std::cout << "os1+='A' length=" << os1.length() << " capacity=" << os1.capacity() << " { " << os1 << " }" << std::endl;
      os1+=buf[0];
      std::cout << "os1+=0x01 length=" << os1.length() << " capacity=" << os1.capacity() << " { " << os1 << " }" << std::endl;
   }
   {
      EOctetString os1(buf,sizeof(buf));
      std::cout << "os1 length=" << os1.length() << " capacity=" << os1.capacity() << " { " << os1 << " }" << std::endl;
      os1[0] += 32;
      std::cout << "os1[0]+=32 length=" << os1.length() << " capacity=" << os1.capacity() << " { " << os1 << " }" << std::endl;
   }
   {
      EOctetString os1(buf,sizeof(buf)), os2(buf,sizeof(buf));
      std::cout << "os1 length=" << os1.length() << " capacity=" << os1.capacity() << " { " << os1 << " }" << std::endl;
      std::cout << "os2 length=" << os2.length() << " capacity=" << os2.capacity() << " { " << os2 << " }" << std::endl;
      os1.append(os2);
      std::cout << "os1.append(os2) length=" << os1.length() << " capacity=" << os1.capacity() << " { " << os1 << " }" << std::endl;
      os1.assign(buf,sizeof(buf));
      os1.append(os2,9,10);
      std::cout << "os1.append(os2,9,10) length=" << os1.length() << " capacity=" << os1.capacity() << " { " << os1 << " }" << std::endl;
      os1.assign(buf,5);
      os1.append(str);
      std::cout << "os1.append(str) length=" << os1.length() << " capacity=" << os1.capacity() << " { " << os1 << " }" << std::endl;
      os1.assign(buf,5);
      os1.append(str,10);
      std::cout << "os1.append(str,10) length=" << os1.length() << " capacity=" << os1.capacity() << " { " << os1 << " }" << std::endl;
      os1.assign(buf,5);
      os1.append(buf,10);
      std::cout << "os1.append(buf,10) length=" << os1.length() << " capacity=" << os1.capacity() << " { " << os1 << " }" << std::endl;
      os1.assign(buf,5);
      os1.append(20,buf[0]);
      std::cout << "os1.append(20,buf[0]) length=" << os1.length() << " capacity=" << os1.capacity() << " { " << os1 << " }" << std::endl;
   }
   {
      EOctetString os1, os2(buf,sizeof(buf));
      std::cout << "os1 length=" << os1.length() << " capacity=" << os1.capacity() << " { " << os1 << " }" << std::endl;
      os1.assign(os2);
      std::cout << "os1.assign(os2) length=" << os1.length() << " capacity=" << os1.capacity() << " { " << os1 << " }" << std::endl;
      os1.assign(os2,9,10);
      std::cout << "os1.assign(os2,9,10) length=" << os1.length() << " capacity=" << os1.capacity() << " { " << os1 << " }" << std::endl;
      os1.assign(str);
      std::cout << "os1.assign(str) length=" << os1.length() << " capacity=" << os1.capacity() << " { " << os1 << " }" << std::endl;
      os1.assign(str,10);
      std::cout << "os1.assign(str,10) length=" << os1.length() << " capacity=" << os1.capacity() << " { " << os1 << " }" << std::endl;
      os1.assign(buf,5);
      std::cout << "os1.assign(buf,5) length=" << os1.length() << " capacity=" << os1.capacity() << " { " << os1 << " }" << std::endl;
      os1.assign(30,'0');
      std::cout << "os1.assign(30,'0') length=" << os1.length() << " capacity=" << os1.capacity() << " { " << os1 << " }" << std::endl;
      os1.assign(std::move(os2));
      std::cout << "os2.assign(std::move(os2)) os1 length=" << os1.length() << " capacity=" << os1.capacity() << " { " << os1 << " }" << std::endl
                << "                           os2 length=" << os2.length() << " capacity=" << os2.capacity() << " { " << os2 << " }" << std::endl;
   }
   {
      EOctetString os1(buf,10);
      std::cout << "os1 length=" << os1.length() << " capacity=" << os1.capacity() << " { " << os1 << " }" << std::endl;
      std::cout << "os1.at(4)=" << static_cast<UInt>(os1.at(4)) << std::endl;
      std::cout << "os1.back()=" << static_cast<UInt>(os1.back()) << std::endl;
   }
   {
      EOctetString os1(buf,10), os2(&buf[10],10), os3(buf,sizeof(buf)), os4(buf,sizeof(buf));
      std::cout << "os1 length=" << os1.length() << " capacity=" << os1.capacity() << " { " << os1 << " }" << std::endl;
      std::cout << "os2 length=" << os2.length() << " capacity=" << os2.capacity() << " { " << os2 << " }" << std::endl;
      std::cout << "os3 length=" << os3.length() << " capacity=" << os3.capacity() << " { " << os3 << " }" << std::endl;
      std::cout << "os4 length=" << os4.length() << " capacity=" << os4.capacity() << " { " << os4 << " }" << std::endl;
      std::cout << "os1==os1 " << (os1==os1?"TRUE":"FALSE") << std::endl;
      std::cout << "os1==os2 " << (os1==os2?"TRUE":"FALSE") << std::endl;
      std::cout << "os1!=os1 " << (os1!=os1?"TRUE":"FALSE") << std::endl;
      std::cout << "os1!=os2 " << (os1!=os2?"TRUE":"FALSE") << std::endl;
      std::cout << "os1<os1 " << (os1<os1?"TRUE":"FALSE") << std::endl;
      std::cout << "os1<os2 " << (os1<os2?"TRUE":"FALSE") << std::endl;
      std::cout << "os2<os1 " << (os2<os1?"TRUE":"FALSE") << std::endl;
      std::cout << "os1>os1 " << (os1>os1?"TRUE":"FALSE") << std::endl;
      std::cout << "os1>os2 " << (os1>os2?"TRUE":"FALSE") << std::endl;
      std::cout << "os2>os1 " << (os2>os1?"TRUE":"FALSE") << std::endl;
      std::cout << "os1<=os1 " << (os1<=os1?"TRUE":"FALSE") << std::endl;
      std::cout << "os1<=os2 " << (os1<=os2?"TRUE":"FALSE") << std::endl;
      std::cout << "os2<=os1 " << (os2<=os1?"TRUE":"FALSE") << std::endl;
      std::cout << "os1>=os1 " << (os1>=os1?"TRUE":"FALSE") << std::endl;
      std::cout << "os1>=os2 " << (os1>=os2?"TRUE":"FALSE") << std::endl;
      std::cout << "os2>=os1 " << (os2>=os1?"TRUE":"FALSE") << std::endl;
   }
   {
      EOctetString os1, os2(buf,10);
      std::cout << "os1 length=" << os1.length() << " capacity=" << os1.capacity() << " { " << os1 << " }" << std::endl;
      std::cout << "os2 length=" << os2.length() << " capacity=" << os2.capacity() << " { " << os2 << " }" << std::endl;
      std::cout << "os1.empty()" << (os1.empty()?"TRUE":"FALSE") << std::endl;
      std::cout << "os2.empty()" << (os2.empty()?"TRUE":"FALSE") << std::endl;
      std::cout << "os1.erase() length=" << os1.length() << " capacity=" << os1.capacity() << " { " << os1 << " }" << std::endl;
      std::cout << "os2.erase() length=" << os1.length() << " capacity=" << os1.capacity() << " { " << os1 << " }" << std::endl;
   }
   {
      EOctetString os1(buf,10);
      std::cout << "os1 length=" << os1.length() << " capacity=" << os1.capacity() << " { " << os1 << " }" << std::endl;
      os1.resize(5);
      std::cout << "os1.resize(5) length=" << os1.length() << " capacity=" << os1.capacity() << " { " << os1 << " }" << std::endl;
      os1.resize(50,'0');
      std::cout << "os1.resize(50,'0') length=" << os1.length() << " capacity=" << os1.capacity() << " { " << os1 << " }" << std::endl;
      os1.shrink_to_fit();
      std::cout << "os1.shrink_to_fit() length=" << os1.length() << " capacity=" << os1.capacity() << " { " << os1 << " }" << std::endl;
      std::cout << "std::hash<EOctetString>{}(os1) = " << std::hash<EOctetString>{}(os1) << std::endl;
   }
}

Void teidTest()
{
   ETeidManager mgr1, mgr2(1,0), mgr3(2,0), mgr4(3,0), mgr5(4,0), mgr6(5,0), mgr7(6,0), mgr8(7,0), mgr9(7,127);
   std::cout << "ETeidManager() rangeBits=" << mgr1.rangeBits() << " rangeValue=" << mgr1.rangeValue() << " min=" << hexFormatWithoutCommas<ULong>(mgr1.min()) << " max=" << hexFormatWithoutCommas<ULong>(mgr1.max()) << std::endl;
   std::cout << "ETeidManager() rangeBits=" << mgr2.rangeBits() << " rangeValue=" << mgr2.rangeValue() << " min=" << hexFormatWithoutCommas<ULong>(mgr2.min()) << " max=" << hexFormatWithoutCommas<ULong>(mgr2.max()) << std::endl;
   std::cout << "ETeidManager() rangeBits=" << mgr3.rangeBits() << " rangeValue=" << mgr3.rangeValue() << " min=" << hexFormatWithoutCommas<ULong>(mgr3.min()) << " max=" << hexFormatWithoutCommas<ULong>(mgr3.max()) << std::endl;
   std::cout << "ETeidManager() rangeBits=" << mgr4.rangeBits() << " rangeValue=" << mgr4.rangeValue() << " min=" << hexFormatWithoutCommas<ULong>(mgr4.min()) << " max=" << hexFormatWithoutCommas<ULong>(mgr4.max()) << std::endl;
   std::cout << "ETeidManager() rangeBits=" << mgr5.rangeBits() << " rangeValue=" << mgr5.rangeValue() << " min=" << hexFormatWithoutCommas<ULong>(mgr5.min()) << " max=" << hexFormatWithoutCommas<ULong>(mgr5.max()) << std::endl;
   std::cout << "ETeidManager() rangeBits=" << mgr6.rangeBits() << " rangeValue=" << mgr6.rangeValue() << " min=" << hexFormatWithoutCommas<ULong>(mgr6.min()) << " max=" << hexFormatWithoutCommas<ULong>(mgr6.max()) << std::endl;
   std::cout << "ETeidManager() rangeBits=" << mgr7.rangeBits() << " rangeValue=" << mgr7.rangeValue() << " min=" << hexFormatWithoutCommas<ULong>(mgr7.min()) << " max=" << hexFormatWithoutCommas<ULong>(mgr7.max()) << std::endl;
   std::cout << "ETeidManager() rangeBits=" << mgr8.rangeBits() << " rangeValue=" << mgr8.rangeValue() << " min=" << hexFormatWithoutCommas<ULong>(mgr8.min()) << " max=" << hexFormatWithoutCommas<ULong>(mgr8.max()) << std::endl;
   std::cout << "ETeidManager() rangeBits=" << mgr9.rangeBits() << " rangeValue=" << mgr9.rangeValue() << " min=" << hexFormatWithoutCommas<ULong>(mgr9.min()) << " max=" << hexFormatWithoutCommas<ULong>(mgr9.max()) << std::endl;

   ULong v1, v2, v3;

   std::cout << "starting teid assignment min=" << mgr8.min() << " max=" << mgr8.max() << std::endl;
   v1 = mgr8.alloc();
   v2 = 0;
   v3 = 0;
   while (1)
   {
      v2 = mgr8.alloc();
      if (v2 < v3)
         break;
      v3 = v2;
   }
   std::cout << "ETeidManager() v1=" << hexFormatWithoutCommas<ULong>(v1) << " v2=" << hexFormatWithoutCommas<ULong>(v2) << " v3=" << hexFormatWithoutCommas<ULong>(v3) << std::endl;

   std::cout << "starting teid assignment min=" << mgr9.min() << " max=" << mgr9.max() << std::endl;
   v1 = mgr9.alloc();
   v2 = 0;
   v3 = 0;
   while (1)
   {
      v2 = mgr9.alloc();
      if (v2 < v3)
         break;
      v3 = v2;
   }
   std::cout << "ETeidManager() v1=" << hexFormatWithoutCommas<ULong>(v1) << " v2=" << hexFormatWithoutCommas<ULong>(v2) << " v3=" << hexFormatWithoutCommas<ULong>(v3) << std::endl;

   std::cout << "starting teid assignment min=" << mgr1.min() << " max=" << mgr1.max() << std::endl;
   v1 = mgr1.alloc();
   v2 = 0;
   v3 = 0;
   while (1)
   {
      v2 = mgr1.alloc();
      if (v2 < v3)
         break;
      v3 = v2;
   }
   std::cout << "ETeidManager() v1=" << hexFormatWithoutCommas<ULong>(v1) << " v2=" << hexFormatWithoutCommas<ULong>(v2) << " v3=" << hexFormatWithoutCommas<ULong>(v3) << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

class TestWorkGroup;

class TestWorker : public EThreadWorkerPrivate
{
public:
   TestWorker()
      : m_twg(nullptr),
        m_val(0)
   {
   }

   Void onInit()
   {
      std::cout << "TestWorker::onInit() workerId=" << this->workerId() << std::endl;
   }

   Void onQuit()
   {
      std::cout << "TestWorker::onQuit() workerId=" << this->workerId() << std::endl;
   }

   Void onTimer(EThreadEventTimer *pTimer)
   {
      myOnTimer(pTimer);
   }

   Void onMessageQueued(const EThreadMessage &msg)
   {
   }

   Void handler(EThreadMessage &msg);

   TestWorkGroup &group()
   {
      return *m_twg;
   }

   TestWorker &group(TestWorkGroup &twg)
   {
      m_twg = &twg;
      return *this;
   }

   BEGIN_MESSAGE_MAP2(TestWorker, EThreadWorkerPrivate)
      ON_MESSAGE2(EM_USER1, TestWorker::handler)
   END_MESSAGE_MAP2()

private:
   Void myOnTimer(EThreadEventTimer *pTimer);

   TestWorkGroup *m_twg;
   Int m_val;
};

class TestWorkGroup : public EThreadWorkGroupPrivate<TestWorker>
{
public:
   TestWorkGroup()
   {
      m_cnt = 0;
   }

   Bool keepGoing()
   {
      m_cnt++;
      return m_cnt < 200;
   }

protected:
   Void onCreateWorker(TestWorker &worker)
   {
      worker.group(*this);
   }

   Void onMessageQueued(const EThreadMessage &msg)
   {
   }

private:
   Int m_cnt;
};

Void TestWorker::myOnTimer(EThreadEventTimer *pTimer)
{
   std::cout << "TestWorker::onTimer() workerId=" << workerId() << " timerId=" << pTimer->getId() << std::endl;
   if (!group().keepGoing())
      group().quit();
}

Void TestWorker::handler(EThreadMessage &msg)
{
   std::cout << "TestWorker::handler() workerId=" << this->workerId() << " data=" << msg.data().data().int32[0] << std::endl;
   m_val++;

   EThreadMessage m(EM_USER1, this->workerId() * 1000 + m_val);
   group().sendMessage(m);
   sleep(100);
}

Void workGroup_test()
{
   static int numWorkers = 1;
   char buffer[128];
   TestWorkGroup wg;

   cout << "Enter the number of workers threads for this work group [" << numWorkers << "]: ";
   cout.imbue(mylocale);
   cin.getline(buffer, sizeof(buffer));
   if (buffer[0])
      numWorkers = std::stoi(buffer);

   std::cout << "workGroup_test() initializing the work group " << std::endl;
   wg.init(1, 1, numWorkers);

   std::cout << "workGroup_test() posting messages" << std::endl;
   for (int i=0; i<numWorkers; i++)
   {
      EThreadMessage msg(EM_USER1,i+1);
      wg.sendMessage(msg);
   }

   EThreadEventTimer tmr;
   tmr.setOneShot(False);
   tmr.setInterval(10);
   wg.initTimer(tmr);
   tmr.start();
   
   std::cout << "workGroup_test() joining" << std::endl;
   wg.join();
   std::cout << "workGroup_test() complete" << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


Void usage()
{
   const char *msg =
       "USAGE:  epctest [--help] [--file optionfile]\n";
   cout << msg;
}

Void printMenu()
{
   printf(
       "\n"
       "                       Enhanced Packet Core Tools Test Menu                     \n"
       "                         Public features are %senabled                          \n"
       "\n"
       "1.  Semaphore/thread cancellation              23. Thread Test (4 writers)      \n"
       "2.  DateTime object tests                      24. Mutex performance test       \n"
       "3.  Public thread test (1 writer, 1 reader)    25. Socket server                \n"
       "4.  Public thread test (1 writer, 4 readers)   26. Socket client                \n"
       "5.  Private thread test (1 writer, 4 readers)  27. Read/Write Lock test         \n"
       "6.  Public queue test (reader)                 28. Options test                 \n"
       "7.  Public queue test (writer)                 29. Logger test                  \n"
       "8.  Elapsed timer                              30. UDP socket test              \n"
       "9.  Error handling                             31. Timer Pool test              \n"
       "10. Private Mutex test                         32. Object Sizes                 \n"
       "11. Public Mutex test                          33. Custom Public Event Host     \n"
       "12. Private Semaphore test                     34. Custom Public Event Client   \n"
       "13. Public Semaphore test                      35. Private Thread Example       \n"
       "14. Basic thread test                          36. Public Thread Example Host   \n"
       "15. Thread suspend/resume                      37. Public Thread Example Client \n"
       "16. Thread periodic timer test                 38. EIpFilterRule                \n"
       "17. Thread one shot timer test                 39. Octet string                 \n"
       "18. Circular buffer test                       40. Teid                         \n"
       "19. Directory test                             41. Work Group                   \n"
       "20. Hash test                                  \n"
       "21. Thread test (1 reader/writer)              \n"
       "22. Deadlock                                   \n"
       "\n",
       EpcTools::isPublicEnabled() ? "" : "NOT ");
}

Void run(EGetOpt &options)
{
   Int opt;
   Char selection[128];

   while (1)
   {
      printMenu();
      cout << "Selection : ";
      while ( cin.getline(selection, sizeof(selection)).fail() )
      {
         cin.clear();
         continue;
      }
      cout << endl;

      if (selection[0] == 'q' || selection[0] == 'Q')
         break;

      opt = atoi(selection);

      try
      {
         switch (opt)
         {
            case 1:  EThread_cancel_wait();        break;
            case 2:  EDateTime_test();             break;
            case 3:  EThread_test();               break;
            case 4:  EThread_test2();              break;
            case 5:  EThread_test3();              break;
            case 6:  EQueuePublic_test(False);     break;
            case 7:  EQueuePublic_test(True);      break;
            case 8:  ETimer_test();                break;
            case 9:  EError_test();                break;
            case 10: EMutexPrivate_test();         break;
            case 11: EMutexPublic_test();          break;
            case 12: ESemaphorePrivate_test();     break;
            case 13: ESemaphorePublic_test();      break;
            case 14: EThreadBasic_test();          break;
            case 15: EThreadSuspendResume_test();  break;
            case 16: EThreadTimerPeriodic_test();  break;
            case 17: EThreadTimerOneShot_test();   break;
            case 18: ECircularBuffer_test();       break;
            case 19: EDirectory_test();            break;
            case 20: EHash_test();                 break;
            case 21: EThread_test4();              break;
            case 22: deadlock();                   break;
            case 23: EThread_test5();              break;
            case 24: EMutex_test2();               break;
            case 25: tcpsockettest(True);          break;
            case 26: tcpsockettest(False);         break;
            case 27: ERWLock_test();               break;
            case 28: EGetOpt_test(options);        break;
            case 29: ELogger_test();               break;
            case 30: udpsockettest();              break;
            case 31: timerpooltest();              break;
            case 32: printObjectSizes();           break;
            case 33: customThreadTest(True);       break;
            case 34: customThreadTest(False);      break;
            case 35: threadExample();              break;
            case 36: publicThreadExample(True);    break;
            case 37: publicThreadExample(False);   break;
            case 38: eipfilterruleTest();          break;
            case 39: octetStringTest();            break;
            case 40: teidTest();                   break;
            case 41: workGroup_test();             break;
            default: cout << "Invalid Selection" << endl << endl;    break;
         }
      }
      catch (EError &e)
      {
         cout << e.Name() << " - " << e << endl;
      }
   }
}

#define BUFFER_SIZE 262144
int main(int argc, char *argv[])
{
   typedef uint8_t ThreeBytes[3];
   typedef std::array<uint8_t,3> AnotherThreeBytes;
   typedef struct
   {
      UChar f1_ : 1;
      UChar f2_ : 1;
      UChar f3_ : 1;
      UChar f4_ : 1;
      UChar f5_ : 1;
   } Flags;

   class Brian
   {
   public:
      ThreeBytes &threeBytes() { return threebytes_; }
      AnotherThreeBytes &anotherThreeBytes() { return anotherthreebytes_; }
      Flags &flags() { return flags_; }
      ThreeBytes threebytes_;
      AnotherThreeBytes anotherthreebytes_;
      Flags flags_;
   };

   Brian b;

   std::cout << "sizeof Brian::ThreeBytes() = " << sizeof(b.threeBytes()) << std::endl;
   std::cout << "sizeof Brian::AnotherThreeBytes() = " << sizeof(b.anotherThreeBytes()) << std::endl;
   std::cout << "sizeof Brian::flags() = " << sizeof(b.flags()) << std::endl;

   //EString s;
   //ETime t1, t2;
   //t1.Format(s, "%i", True);	cout << s << endl;
   //t2 = t1.add(1,0,0,0,0);
   //t1.Format(s, "%i", True);	cout << s << endl;
   //t2.Format(s, "%i", True);	cout << s << endl;
   //LongLong chk;
   //chk = t1.year() * 10000000000LL + t1.month() * 100000000LL + t1.day() * 1000000LL + t1.hour() * 10000LL + t1.minute() * 100LL + t1.second();
   //cout << chk << endl;

   //try
   //{
   //	EBzip2 bz;

   //	Int block = 0;
   //	ULongLong tamt = 0;
   //	pChar buf = new Char[BUFFER_SIZE];
   //	memset(buf, 0x7f, BUFFER_SIZE);
   //	//bz.readOpen("C:\\Users\\bwaters\\Downloads\\cdr1.txt.20140730110101.bz2");
   //	//for (;;)
   //	//{
   //	//	Int amt = bz.read((pUChar)buf, BUFFER_SIZE);
   //	//	cout << "\r" << block++ << ":" << amt;
   //	//	tamt += (ULong)amt;
   //	//	if (amt < sizeof(buf))
   //	//		break;
   //	//}
   //	//cout << "\r" << block << ":" << tamt << endl;
   //	//bz.close();

   //	Int amt=1;
   //	block = 0;
   //	bz.readOpen("C:\\Users\\bwaters\\Downloads\\cdr1.txt.20140730110101.bz2");
   //	while (amt)
   //	{
   //		amt = bz.readLine(buf, BUFFER_SIZE);
   //		if (block % 1000 == 0)
   //			cout << "\r" << block << ":" << amt;
   //		block++;
   //	}
   //	cout << "\r" << block++ << ":" << amt << endl;
   //	bz.close();
   //}
   //catch (EError  e)
   //{
   //	cout << e->getText() << endl;
   //}

   //{
   //   EMutexData d;
   //   EMutexDataPublic dp;
   //
   //   cout << "sizeof EMutexPrivate = " << sizeof(EMutexPrivate) << endl;
   //   cout << "sizeof EMutexPublic = " << sizeof(EMutexPublic) << endl;
   //   cout << "sizeof EMutexData = " << sizeof(EMutexData) << endl;
   //   cout << "sizeof EMutexData.m_initialized = " << sizeof(d.initialized()) << endl;
   //   cout << "sizeof EMutexData.m_mutex = " << sizeof(d.mutex()) << endl;
   //   cout << "sizeof EMutexDataPublic = " << sizeof(dp) << endl;
   //   cout << "sizeof EMutexDataPublic.m_initialized = " << sizeof(dp.initialized()) << endl;
   //   cout << "sizeof EMutexDataPublic.m_mutex = " << sizeof(dp.mutex()) << endl;
   //   cout << "sizeof EMutexDataPublic.m_nextIndex = " << sizeof(dp.nextIndex()) << endl;
   //   cout << "sizeof EMutexDataPublic.m_mutexId = " << sizeof(dp.mutexId()) << endl;
   //}

   EGetOpt::Option options[] = {
       {"-h", "--help", EGetOpt::no_argument, EGetOpt::dtNone},
       {"-f", "--file", EGetOpt::required_argument, EGetOpt::dtString},
       {"", "", EGetOpt::no_argument, EGetOpt::dtNone},
   };

   EGetOpt opt;
   EString optFile;

   try
   {
      opt.loadCmdLine(argc, argv, options);
      if (opt.getCmdLine("-h,--help", false))
      {
         usage();
         exit(0);
      }

      optFile.format("%s.json", argv[0]);
      opt.loadFile(optFile.c_str());

      optFile = opt.getCmdLine("-f,--file", "");
      if (!optFile.empty())
         opt.loadFile(optFile.c_str());
   }
   catch (const EGetOptError_FileParsing &e)
   {
      std::cerr << e.Name() << " - " << e.what() << '\n';
      exit(0);
   }
   catch (const std::exception &e)
   {
      std::cerr << e.what() << '\n';
      exit(0);
   }

   defaultLocale = std::cout.getloc();
   mylocale = std::locale("");

   std::cout.imbue(mylocale);

// ETime now = ETime::Now();
// std::cout
//    << "timeval.tv_sec="
//    << now.getTimeVal().tv_sec
//    << " sizeof(timeval.tv_sec)="
//    << sizeof(now.getTimeVal().tv_sec)
//    << " timeval.tv_usec="
//    << now.getTimeVal().tv_usec
//    << " sizeof(timeval.tv_usec)="
//    << sizeof(now.getTimeVal().tv_usec)
//    << " (timeval.tv_sec * 1000000 + timeval.tv_usec % 1000000)="
//    << now.getTimeVal().tv_sec * 1000000 + now.getTimeVal().tv_usec % 1000000
//    << std::endl << std::flush;

   try
   {
      {
         sigset_t sigset;

         /* mask SIGALRM in all threads by default */
         sigemptyset(&sigset);
         sigaddset(&sigset, SIGRTMIN + 2);
         sigaddset(&sigset, SIGRTMIN + 3);
         sigaddset(&sigset, SIGUSR1);
         sigprocmask(SIG_BLOCK, &sigset, NULL);
      }

      EpcTools::Initialize(opt);

      // NodeSelector_test();

      run(opt);

      EpcTools::UnInitialize();
   }
   catch (EError &e)
   {
      cout << (cpStr)e << endl;
   }

   return 0;
}

#if 0
typedef enum {
   itS11,
   itS5S8,
   itSxa,
   itSxb,
   itSxaSxb,
   itGx
} EInterfaceType;

typedef enum {
   dIn,
   dOut,
   dBoth,
   dNone
} EDirection;

typedef struct {
   int msgtype;
   const char *msgname;
   EDirection dir;
} MessageType;

typedef struct {
   int cnt;
   time_t ts;
} Statistic;

#define SENT 0
#define RCVD 1

typedef struct {
   struct in_addr ipaddr;
   EInterfaceType intfctype;
   int hcsent[2];
   int hcrcvd[2];
   union {
      Statistic s11[51][2];
      Statistic s5s8[37];
      Statistic sxa[21];
      Statistic sxb[21];
      Statistic sxasxb[23];
   } stats;
} SPeer;

MessageType s11MessageDefs[] = {
   { 3, "Version Not Supported Indication", dBoth },
   { 1, NULL, dNone }
};

int s11MessageTypes [] = {
   -1, -1, -1, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 3, 4, 7, 8, 5, 6, 11, 12, 9, 10, 
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
   -1, -1, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, -1, -1, -1, -1, -1, -1, -1, -1, 
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 25, 26, 27, 28, 29, 30, 31, 
   32, 33, 34, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 37, 38, 
   35, 36, 13, 14, 39, 40, 41, 42, 43, 44, -1, -1, -1, -1, 45, 46, -1, 47, 48, -1, 
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
   -1, -1, -1, -1, -1, -1, -1, -1, -1, 49, 50, };

loop through peers
{
   csAddPeer();
   csAddPeerStats( SPeer*, Statistic*, MessageType*, msgcnt );
}
csAddPeer()
#endif
