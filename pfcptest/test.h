#include <functional>
#include <map>

#include "estring.h"
#include "eerror.h"

#define TEST(name)                     \
bool name(void *);                     \
class name##_TEST                      \
{                                      \
public:                                \
    name##_TEST()                      \
    {                                  \
        TestSuite::add(#name, name);   \
    }                                  \
};                                     \
static name##_TEST name##_TEST_STATIC; \
bool name(void *args)

namespace PFCPTest
{
    DECLARE_ERROR_ADVANCED4(TestSuite_UnrecognizedTestName);
    inline TestSuite_UnrecognizedTestName::TestSuite_UnrecognizedTestName(cpStr msg)
    {
        setTextf("Unrecognized test name (%s)", msg);
    }

    DECLARE_ERROR_ADVANCED4(TestSuite_TestException);
    inline TestSuite_TestException::TestSuite_TestException(cpStr msg)
    {
        setTextf("Test threw an exception: (%s)", msg);
    }

    class TestSuite
    {
    public:
        class Test
        {
        public:
            using Func = std::function<bool(void *)>;
            using Args = void *;

            Test() = default;
            Test(Func func, Args args) : m_func(func), m_args(args) {}

        private:
            friend class TestSuite;

            Func m_func = nullptr;
            Args m_args = nullptr;
        };
        
        using TestLookup = std::map<EString, Test>;
        
        static inline TestLookup &tests() { return s_tests; };
        static bool run(const EString &name);
        static void add(const EString &name, Test::Func func, Test::Args args = nullptr);
        static EString calculateSHA1Hash(const EString &filename);

    private:
        static TestLookup s_tests;
    };
}