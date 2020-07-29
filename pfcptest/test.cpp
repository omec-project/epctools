#include "test.h"

#include <fstream>
#include <sstream>

#include <openssl/sha.h>

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

    // Adapted from https://stackoverflow.com/a/55197320
    EString TestSuite::calculateSHA1Hash(const EString &filename)
    {
        std::ifstream file(filename, std::ifstream::binary);
        if(!file)
            return EString();
        
        SHA_CTX sha_context;
        if(!SHA1_Init(&sha_context))
            return EString();

        char file_buffer[1024 * 16];
        while(file.good())
        {
            file.read(file_buffer, sizeof(file_buffer));
            if(!SHA1_Update(&sha_context, file_buffer, file.gcount()))
                return EString();
        }

        file.close();

        unsigned char hash[SHA_DIGEST_LENGTH];
        if(!SHA1_Final(hash, &sha_context))
            return EString();

        std::ostringstream oss;
        oss << std::hex << std::setfill('0');
        for(const auto &byte : hash)
            oss << std::setw(2) << (int) byte;
        
        return oss.str();
    }
}