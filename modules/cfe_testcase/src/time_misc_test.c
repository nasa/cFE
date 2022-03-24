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
