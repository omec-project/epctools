#include <functional>
#include <map>
#include <string>

#include "eerror.h"

#define TEST(name)                     \
bool name();                           \
class name##_TEST                      \
{                                      \
public:                                \
    name##_TEST()                      \
    {                                  \
        TestSuite::add(#name, name);   \
    }                                  \
};                                     \
static name##_TEST name##_TEST_STATIC; \
bool name()

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
        using Test = std::function<bool()>;
        using TestLookup = std::map<std::string, Test>;
        
        static inline TestLookup &tests() { return s_tests; };
        static bool run(const std::string &name);
        static void add(const std::string &name, Test test);
        static std::string calculateSHA1Hash(const std::string &filename);

    private:
        static TestLookup s_tests;
    };
}