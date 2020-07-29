#include "test.h"

#include <fstream>
#include <sstream>

namespace PFCPTest
{
    TestSuite::TestLookup TestSuite::s_tests;

    bool TestSuite::run(const EString &name)
    {
        auto test = s_tests.find(name);

        if(test == s_tests.end())
            throw TestSuite_UnrecognizedTestName(name.c_str());

        try
        {
            return test->second.m_func(test->second.m_args);
        }
        catch(const std::exception& e)
        {
            throw TestSuite_TestException(e.what());
        }
    }

    void TestSuite::add(const EString &name, Test::Func func, Test::Args args)
    {
        Test test(func, args);
        s_tests[name] = test;
    }
}