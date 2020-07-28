#include "test.h"

#include <fstream>
#include <sstream>

#include <openssl/sha.h>

namespace PFCPTest
{
    TestSuite::TestLookup TestSuite::s_tests;

    bool TestSuite::run(const std::string &name)
    {
        auto test = s_tests.find(name);

        if(test == s_tests.end())
            throw TestSuite_UnrecognizedTestName(name.c_str());

        try
        {
            return test->second();
        }
        catch(const std::exception& e)
        {
            throw TestSuite_TestException(e.what());
        }
    }

    void TestSuite::add(const std::string &name, Test test)
    {
        s_tests[name] = test;
    }

    // Adapted from https://stackoverflow.com/a/55197320
    std::string TestSuite::calculateSHA1Hash(const std::string &filename)
    {
        std::ifstream file(filename, std::ifstream::binary);
        if(!file)
            return std::string();
        
        SHA_CTX sha_context;
        if(!SHA1_Init(&sha_context))
            return std::string();

        char file_buffer[1024 * 16];
        while(file.good())
        {
            file.read(file_buffer, sizeof(file_buffer));
            if(!SHA1_Update(&sha_context, file_buffer, file.gcount()))
                return std::string();
        }

        file.close();

        unsigned char hash[SHA_DIGEST_LENGTH];
        if(!SHA1_Final(hash, &sha_context))
            return std::string();

        std::ostringstream oss;
        oss << std::hex << std::setfill('0');
        for(const auto &byte : hash)
            oss << std::setw(2) << (int) byte;
        
        return oss.str();
    }
}