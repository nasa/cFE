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

/*
 * Test time accessors
 */

/*
 * Includes
 */
#include "utassert.h"
#include "ut_support.h"
#include "test_msg_not.h"
#include "test_msg_utils.h"
#include "cfe_msg.h"
#include "test_cfe_msg_time.h"
#include "cfe_error.h"
#include <string.h>

void Test_MSG_Time(void)
{
    CFE_MSG_TelemetryHeader_t tlm;
    CFE_MSG_Message_t *       msgptr  = CFE_MSG_PTR(tlm);
    CFE_TIME_SysTime_t        input[] = {{0, 0}, {0x12345678, 0xABCDEF12}, {0xFFFFFFFF, 0xFFFFFFFF}};
    CFE_TIME_SysTime_t        actual  = {0xFFFFFFFF, 0xFFFFFFFF};
    int                       i;

    UtPrintf("Bad parameter tests, Null pointers, no secondary header");
    memset(&tlm, 0, sizeof(tlm));
    UtAssert_INT32_EQ(CFE_MSG_GetMsgTime(NULL, &actual), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(actual.Seconds, 0xFFFFFFFF);
    UtAssert_INT32_EQ(actual.Subseconds, 0xFFFFFFFF);
    UtAssert_INT32_EQ(CFE_MSG_GetMsgTime(msgptr, NULL), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(Test_MSG_NotZero(msgptr), 0);
    UtAssert_INT32_EQ(CFE_MSG_SetMsgTime(NULL, input[0]), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_SetMsgTime(msgptr, actual), CFE_MSG_WRONG_MSG_TYPE);
    UtAssert_INT32_EQ(Test_MSG_NotZero(msgptr), 0);
    UtAssert_INT32_EQ(CFE_MSG_GetMsgTime(msgptr, &actual), CFE_MSG_WRONG_MSG_TYPE);
    UtAssert_INT32_EQ(actual.Seconds, 0);
    UtAssert_INT32_EQ(actual.Subseconds, 0);

    UtPrintf("Bad <F2>message, wrong type (command)");
    CFE_UtAssert_SUCCESS(CFE_MSG_SetType(msgptr, CFE_MSG_Type_Cmd));
    UtAssert_INT32_EQ(CFE_MSG_SetMsgTime(msgptr, actual), CFE_MSG_WRONG_MSG_TYPE);
    UtAssert_INT32_EQ(Test_MSG_NotZero(msgptr), MSG_TYPE_FLAG);
    UtAssert_INT32_EQ(CFE_MSG_GetMsgTime(msgptr, &actual), CFE_MSG_WRONG_MSG_TYPE);
    UtAssert_INT32_EQ(actual.Seconds, 0);
    UtAssert_INT32_EQ(actual.Subseconds, 0);

    UtPrintf("Set to all F's, various valid inputs");
    for (i = 0; i < sizeof(input) / sizeof(input[0]); i++)
    {
        memset(&tlm, 0xFF, sizeof(tlm));
        CFE_UtAssert_SUCCESS(CFE_MSG_SetType(msgptr, CFE_MSG_Type_Tlm));
        CFE_UtAssert_SUCCESS(CFE_MSG_GetMsgTime(msgptr, &actual));
        UtAssert_INT32_EQ(actual.Seconds, 0xFFFFFFFF);
        UtAssert_INT32_EQ(actual.Subseconds, 0xFFFF0000);
        CFE_UtAssert_SUCCESS(CFE_MSG_SetMsgTime(msgptr, input[i]));
        UT_DisplayPkt(msgptr, sizeof(tlm));
        CFE_UtAssert_SUCCESS(CFE_MSG_GetMsgTime(msgptr, &actual));
        UtAssert_INT32_EQ(actual.Seconds, input[i].Seconds);
        UtAssert_INT32_EQ(actual.Subseconds, input[i].Subseconds & 0xFFFF0000);
        UtAssert_INT32_EQ(Test_MSG_NotF(msgptr), MSG_TYPE_FLAG);
    }

    UtPrintf("Set to all 0, various valid inputs");
    for (i = 0; i < sizeof(input) / sizeof(input[0]); i++)
    {
        memset(&tlm, 0, sizeof(tlm));
        CFE_UtAssert_SUCCESS(CFE_MSG_SetHasSecondaryHeader(msgptr, true));
        CFE_UtAssert_SUCCESS(CFE_MSG_GetMsgTime(msgptr, &actual));
        UtAssert_INT32_EQ(actual.Seconds, 0);
        UtAssert_INT32_EQ(actual.Subseconds, 0);
        CFE_UtAssert_SUCCESS(CFE_MSG_SetMsgTime(msgptr, input[i]));
        UT_DisplayPkt(msgptr, sizeof(tlm));
        CFE_UtAssert_SUCCESS(CFE_MSG_GetMsgTime(msgptr, &actual));
        UtAssert_INT32_EQ(actual.Seconds, input[i].Seconds);
        UtAssert_INT32_EQ(actual.Subseconds, input[i].Subseconds & 0xFFFF0000);
        UtAssert_INT32_EQ(Test_MSG_NotZero(msgptr), MSG_HASSEC_FLAG);
    }
}
