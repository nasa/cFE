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
 *   Functional test of basic External Time Source APIs
 *
 *   Demonstration of how to register and use the UT assert functions.
 */

/*
 * Includes
 */

#include "cfe_test.h"

int32 TestCallbackFunction(void)
{
    CFE_FT_Global.Count += 1;
    return CFE_SUCCESS;
}

int32 TestCallbackFunction2(void)
{
    CFE_FT_Global.Count = 0;
    return CFE_SUCCESS;
}

void TestCallback(void)
{
    CFE_FT_Global.Count = 1;

    UtPrintf("Testing: CFE_TIME_RegisterSynchCallback, CFE_TIME_UnregisterSynchCallback");

    UtAssert_INT32_EQ(CFE_TIME_RegisterSynchCallback(&TestCallbackFunction), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_TIME_RegisterSynchCallback(&TestCallbackFunction2), CFE_TIME_TOO_MANY_SYNCH_CALLBACKS);

    OS_TaskDelay(2500);
    if (CFE_FT_Global.Count < 2)
    {
        UtAssert_MIR("CFE_TIME_RegisterSynchCallback requires manual inspection to determine if failure is with the "
                     "API or due to an insufficient timing performance of this machine");
    }

    CFE_FT_Global.Count = 1;
    UtAssert_INT32_EQ(CFE_TIME_UnregisterSynchCallback(&TestCallbackFunction), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_TIME_UnregisterSynchCallback(&TestCallbackFunction), CFE_TIME_CALLBACK_NOT_REGISTERED);

    OS_TaskDelay(2500);
    UtAssert_INT32_LTEQ(CFE_FT_Global.Count, 2);

    UtAssert_INT32_EQ(CFE_TIME_UnregisterSynchCallback(NULL), CFE_TIME_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_TIME_RegisterSynchCallback(NULL), CFE_TIME_BAD_ARGUMENT);
}

void TestExternal(void)
{
#if ((CFE_PLATFORM_TIME_CFG_SRC_MET == true) || (CFE_PLATFORM_TIME_CFG_SRC_GPS == true) || \
     (CFE_PLATFORM_TIME_CFG_SRC_TIME == true))
    CFE_TIME_SysTime_t time = {1000, 0};
#endif

    UtPrintf("Testing: CFE_TIME_ExternalTone, CFE_TIME_ExternalMET, CFE_TIME_ExternalGPS, CFE_TIME_ExternalTime");
    /* These time calls could impact the system timekeeping. Likely impact is incorrect time for one update cycle, a
     * rejected external time update, multiple tone's or external updates detected, or similar. */
    UtAssert_VOIDCALL(CFE_TIME_ExternalTone());

#if (CFE_PLATFORM_TIME_CFG_SRC_MET == true)
    UtAssert_VOIDCALL(CFE_TIME_ExternalMET(time));
#endif

#if (CFE_PLATFORM_TIME_CFG_SRC_GPS == true)
    UtAssert_VOIDCALL(CFE_TIME_ExternalGPS(time, 5));
#endif

#if (CFE_PLATFORM_TIME_CFG_SRC_TIME == true)
    UtAssert_VOIDCALL(CFE_TIME_ExternalTime(time));
#endif
}

void TimeExternalTestSetup(void)
{
    UtTest_Add(TestCallback, NULL, NULL, "Test Time Synch Callbacks");
    UtTest_Add(TestExternal, NULL, NULL, "Test External Sources");
}
