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
 *   Functional test of basic ES Miscellaneous APIs
 *
 *   Demonstration of how to register and use the UT assert functions.
 */

/*
 * Includes
 */

#include "cfe_test.h"

void TestCalculateCRC(void)
{
    static const char CRC_CHECK_INPUT[] = "123456789";
    static const char CRC_OTHER_INPUT[] = "Random Stuff";

    uint32 inputCrc = 345353;

    UtPrintf("Testing: CFE_ES_CalculateCRC");

    /* The CFE_ES_CrcType_NONE algorithm always returns 0. */
    UtAssert_UINT32_EQ(CFE_ES_CalculateCRC(CRC_CHECK_INPUT, sizeof(CRC_CHECK_INPUT) - 1, 1, CFE_ES_CrcType_NONE), 0);

    /* Calling with an invalid type value should get aliased to NONE. */
    UtAssert_UINT32_EQ(CFE_ES_CalculateCRC(CRC_CHECK_INPUT, sizeof(CRC_CHECK_INPUT) - 1, 1, CFE_ES_CrcType_MAX + 1), 0);

    /* The CFE_ES_CrcType_16_ARC algorithm uses an input value of 0 and the check value result is 0xbb3d */
    UtAssert_UINT32_EQ(CFE_ES_CalculateCRC(CRC_CHECK_INPUT, sizeof(CRC_CHECK_INPUT) - 1, 0, CFE_ES_CrcType_16_ARC),
                       0xBB3D);

    /*
     * In this version of CFE, only CRC-16/ARC is defined, so the default must be the same.  In a future version of CFE,
     * if more than one option is available, this can go back to an "MIR" case, and the user must compare the result
     * to the check value for what they have actully configured as the default CRC.
     */
    UtAssert_UINT32_EQ(CFE_ES_CalculateCRC(CRC_CHECK_INPUT, sizeof(CRC_CHECK_INPUT) - 1, 0, CFE_MISSION_ES_DEFAULT_CRC),
                       0xBB3D);

    /* Compute a secondary string - the reference value obtained using 3rd party tool implementing same algorithm */
    UtAssert_UINT32_EQ(CFE_ES_CalculateCRC(CRC_OTHER_INPUT, sizeof(CRC_OTHER_INPUT) - 1, 0, CFE_ES_CrcType_16_ARC),
                       0x11E3);

    /* Test of nonzero initial value, this is used for checking CRC in across non-contiguous chunks or across multiple
     * cycles */
    UtAssert_UINT32_EQ(CFE_ES_CalculateCRC(CRC_CHECK_INPUT, sizeof(CRC_CHECK_INPUT) - 1, 345353, CFE_ES_CrcType_16_ARC),
                       0xE493);

    /* NULL input or 0 size returns input crc */
    UtAssert_UINT32_EQ(CFE_ES_CalculateCRC(NULL, sizeof(CRC_OTHER_INPUT), inputCrc, CFE_ES_CrcType_16_ARC), inputCrc);
    UtAssert_UINT32_EQ(CFE_ES_CalculateCRC(CRC_OTHER_INPUT, 0, inputCrc, CFE_ES_CrcType_16_ARC), inputCrc);
}

void TestWriteToSysLog(void)
{
    const char *TestString = "Test String for CFE_ES_WriteToSysLog Functional Test";
    uint32      Iterations = CFE_PLATFORM_ES_SYSTEM_LOG_SIZE / 50;

    UtPrintf("Testing: CFE_ES_WriteToSysLog");

    UtAssert_INT32_EQ(CFE_ES_WriteToSysLog(NULL), CFE_ES_BAD_ARGUMENT);

    CFE_Assert_STATUS_STORE(CFE_ES_WriteToSysLog("MIR (Manual Inspection Required) for CFE_ES_WriteToSysLog"));
    if (!CFE_Assert_STATUS_MAY_BE(CFE_ES_ERR_SYS_LOG_FULL))
    {
        CFE_Assert_STATUS_MUST_BE(CFE_SUCCESS);
    }

    /* The test string is a little over 50 chars in length, so writing it repeatedly should fill it up. */
    /* This does depend on whether the system is set to OVERWRITE or DISCARD mode, though -
     * in OVERWRITE mode, the system log will never fill, and therefore CFE_ES_ERR_SYS_LOG_FULL cannot be tested */
    Iterations = 1 + (CFE_PLATFORM_ES_SYSTEM_LOG_SIZE / strlen(TestString));

    while (Iterations > 0)
    {
        --Iterations;
        CFE_Assert_STATUS_STORE(CFE_ES_WriteToSysLog("%s", TestString));
        if (CFE_Assert_STATUS_MAY_BE(CFE_ES_ERR_SYS_LOG_FULL))
        {
            break;
        }
        if (!CFE_Assert_STATUS_MAY_BE(CFE_ES_ERR_SYS_LOG_TRUNCATED))
        {
            CFE_Assert_STATUS_MUST_BE(CFE_SUCCESS);
        }
    }

    UtAssert_MIR("MIR (Manual Inspection Required) for CFE_ES_WriteToSysLog");
}

void TestProcessAsyncEvent(void)
{
    UtPrintf("Testing: CFE_ES_ProcessAsyncEvent");
    UtAssert_VOIDCALL(CFE_ES_ProcessAsyncEvent());
}

void TestBackgroundWakeup(void)
{
    UtPrintf("Testing: CFE_ES_BackgroundWakeup");
    UtAssert_VOIDCALL(CFE_ES_BackgroundWakeup());
}

void ESMiscTestSetup(void)
{
    UtTest_Add(TestCalculateCRC, NULL, NULL, "Test Calculate CRC");
    UtTest_Add(TestWriteToSysLog, NULL, NULL, "Test Write To Sys Log");
    UtTest_Add(TestProcessAsyncEvent, NULL, NULL, "Test Process Async Event");
    UtTest_Add(TestBackgroundWakeup, NULL, NULL, "Test Background Wakeup");
}
