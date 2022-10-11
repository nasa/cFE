/************************************************************************
 * NASA Docket No. GSC-18,719-1, and identified as “core Flight System: Bootes”
 *
 * Copyright (c) 2020 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

/**
 * \file
 *   Functional test of basic Time Arithmetic APIs
 *
 *   Demonstration of how to register and use the UT assert functions.
 */

/*
 * Includes
 */

#include "cfe_test.h"

void TestTimeAdd(void)
{
    UtPrintf("Testing: CFE_TIME_Add");
    CFE_TIME_SysTime_t time1        = {1000, 0};
    CFE_TIME_SysTime_t time2        = {0, 1000};
    CFE_TIME_SysTime_t timeAdded    = CFE_TIME_Add(time1, time2);
    CFE_TIME_SysTime_t timeExpected = {1000, 1000};

    UtAssert_UINT32_EQ(timeAdded.Seconds, timeExpected.Seconds);
    UtAssert_UINT32_EQ(timeAdded.Subseconds, timeExpected.Subseconds);

    time1.Seconds           = UINT32_MAX;
    time1.Subseconds        = UINT32_MAX;
    time2.Seconds           = 0;
    time2.Subseconds        = 1;
    timeAdded               = CFE_TIME_Add(time1, time2);
    timeExpected.Seconds    = 0;
    timeExpected.Subseconds = 0;
    UtAssert_UINT32_EQ(timeAdded.Seconds, timeExpected.Seconds);
    UtAssert_UINT32_EQ(timeAdded.Subseconds, timeExpected.Subseconds);

    time1.Seconds           = UINT32_MAX;
    time1.Subseconds        = UINT32_MAX;
    time2.Seconds           = UINT32_MAX;
    time2.Subseconds        = UINT32_MAX;
    timeAdded               = CFE_TIME_Add(time1, time2);
    timeExpected.Seconds    = UINT32_MAX;
    timeExpected.Subseconds = UINT32_MAX - 1;
    UtAssert_UINT32_EQ(timeAdded.Seconds, timeExpected.Seconds);
    UtAssert_UINT32_EQ(timeAdded.Subseconds, timeExpected.Subseconds);
}

void TestTimeSubtract(void)
{
    UtPrintf("Testing: CFE_TIME_Subtract");
    CFE_TIME_SysTime_t time1          = {1000, 1000};
    CFE_TIME_SysTime_t time2          = {999, 999};
    CFE_TIME_SysTime_t timeSubtracted = CFE_TIME_Subtract(time1, time2);
    CFE_TIME_SysTime_t timeExpected   = {1, 1};

    UtAssert_UINT32_EQ(timeSubtracted.Seconds, timeExpected.Seconds);
    UtAssert_UINT32_EQ(timeSubtracted.Subseconds, timeExpected.Subseconds);

    time1.Seconds           = 0;
    time1.Subseconds        = 0;
    time2.Seconds           = UINT32_MAX;
    time2.Subseconds        = UINT32_MAX;
    timeSubtracted          = CFE_TIME_Subtract(time1, time2);
    timeExpected.Seconds    = 0;
    timeExpected.Subseconds = 1;
    UtAssert_UINT32_EQ(timeSubtracted.Seconds, timeExpected.Seconds);
    UtAssert_UINT32_EQ(timeSubtracted.Subseconds, timeExpected.Subseconds);

    time1.Seconds           = 0;
    time1.Subseconds        = 0;
    time2.Seconds           = 0;
    time2.Subseconds        = 1;
    timeSubtracted          = CFE_TIME_Subtract(time1, time2);
    timeExpected.Seconds    = UINT32_MAX;
    timeExpected.Subseconds = UINT32_MAX;
    UtAssert_UINT32_EQ(timeSubtracted.Seconds, timeExpected.Seconds);
    UtAssert_UINT32_EQ(timeSubtracted.Subseconds, timeExpected.Subseconds);
}

void TestTimeCompare(void)
{
    UtPrintf("Testing: CFE_TIME_Compare");
    CFE_TIME_SysTime_t time1 = {1000, 1000};
    CFE_TIME_SysTime_t time2 = {999, 999};
    UtAssert_UINT32_EQ(CFE_TIME_Compare(time1, time2), CFE_TIME_A_GT_B);
    UtAssert_UINT32_EQ(CFE_TIME_Compare(time2, time1), CFE_TIME_A_LT_B);

    time1.Seconds    = 500;
    time1.Subseconds = 1;
    time2.Seconds    = 500;
    time2.Subseconds = 1;
    UtAssert_UINT32_EQ(CFE_TIME_Compare(time1, time2), CFE_TIME_EQUAL);
    UtAssert_UINT32_EQ(CFE_TIME_Compare(time2, time1), CFE_TIME_EQUAL);

    /* time1 > time2 here due to the roll over handling of the comparison */
    time1.Seconds    = 1;
    time1.Subseconds = 1;
    time2.Seconds    = UINT32_MAX;
    time2.Subseconds = UINT32_MAX;
    UtAssert_UINT32_EQ(CFE_TIME_Compare(time1, time2), CFE_TIME_A_GT_B);
    UtAssert_UINT32_EQ(CFE_TIME_Compare(time2, time1), CFE_TIME_A_LT_B);
}

void TimeArithmeticTestSetup(void)
{
    UtTest_Add(TestTimeAdd, NULL, NULL, "Test Time Addition");
    UtTest_Add(TestTimeSubtract, NULL, NULL, "Test Time Subtraction");
    UtTest_Add(TestTimeCompare, NULL, NULL, "Test Time Comparison");
}
