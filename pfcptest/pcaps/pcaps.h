#ifndef __pfcptest_pcaps_pcaps_h_included
#define __pfcptest_pcaps_pcaps_h_included

#include "test.h"

namespace PFCPTest::pcaps
{
    void InitTests();
    bool RunPcapTest(Test *test);

    class PcapTest : public Test
    {
    public:
        PcapTest(const EString &name) : Test(RunPcapTest), m_name(name) {}

        const EString &name() { return m_name; }
    private:
        EString m_name;
    };
}

#endif // #define __pfcptest_pcaps_pcaps_h_included