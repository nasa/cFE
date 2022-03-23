/**
 * \file
 *   Functional test of miscellaneous Time APIs
 *
 *   Demonstration of how to register and use the UT assert functions.
 */

/*
 * Includes
 */

#include "cfe_test.h"

void TestTimePrint(void)
{
    UtPrintf("Testing: CFE_TIME_Print");
    char               timeBuf1[sizeof("yyyy-ddd-hh:mm:ss.xxxxx_")];
    CFE_TIME_SysTime_t time1 = {0, 0};
    /* 365 days */
    CFE_TIME_SysTime_t time2 = {31536000, 0};
    /* 366 days */
    CFE_TIME_SysTime_t time3 = {31622400, 0};

    UtAssert_VOIDCALL(CFE_TIME_Print(NULL, time1));
    UtAssert_VOIDCALL(CFE_TIME_Print(timeBuf1, time1));
    UtPrintf("%s", timeBuf1);
    UtAssert_VOIDCALL(CFE_TIME_Print(timeBuf1, time2));
    UtPrintf("%s", timeBuf1);
    UtAssert_VOIDCALL(CFE_TIME_Print(timeBuf1, time3));
    UtPrintf("%s", timeBuf1);
}

void TimeMiscTestSetup(void)
{
    UtTest_Add(TestTimePrint, NULL, NULL, "Test Time Print");
}
