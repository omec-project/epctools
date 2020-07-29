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

        if(test == s_tests.end())
            throw TestSuite_UnrecognizedTestName(name.c_str());

        try
        {
            return test->second.get()->m_func(*test->second.get());
        }
        catch(const std::exception& e)
        {
            throw TestSuite_TestException(e.what());
        }
    }

    void TestSuite::add(const EString &name, std::unique_ptr<Test> test)
    {
        s_tests[name] = std::move(test);
    }
}