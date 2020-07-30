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