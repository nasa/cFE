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
 * Test function code accessors
 */

/*
 * Includes
 */
#include "utassert.h"
#include "ut_support.h"
#include "test_msg_not.h"
#include "test_msg_utils.h"
#include "cfe_msg.h"
#include "test_cfe_msg_fc.h"
#include "cfe_error.h"
#include <string.h>

/*
 * Defines
 */
#define TEST_FCNCODE_MAX 0x7F /* Maximum value for fc field */

void Test_MSG_FcnCode(void)
{
    CFE_MSG_CommandHeader_t cmd;
    CFE_MSG_Message_t *     msgptr  = CFE_MSG_PTR(cmd);
    CFE_MSG_FcnCode_t       input[] = {0, TEST_FCNCODE_MAX / 2, TEST_FCNCODE_MAX};
    CFE_MSG_FcnCode_t       actual  = TEST_FCNCODE_MAX;
    int                     i;

    UtPrintf("Bad parameter tests, Null pointers, invalid (max valid + 1, max)");
    memset(&cmd, 0, sizeof(cmd));
    UtAssert_INT32_EQ(CFE_MSG_GetFcnCode(NULL, &actual), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(actual, TEST_FCNCODE_MAX);
    UtAssert_INT32_EQ(CFE_MSG_GetFcnCode(msgptr, NULL), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(Test_MSG_NotZero(msgptr), 0);
    UtAssert_INT32_EQ(CFE_MSG_SetFcnCode(NULL, input[0]), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_SetFcnCode(msgptr, TEST_FCNCODE_MAX + 1), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(Test_MSG_NotZero(msgptr), 0);
    UtAssert_INT32_EQ(CFE_MSG_SetFcnCode(msgptr, 0xFFFF), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(Test_MSG_NotZero(msgptr), 0);

    UtPrintf("Bad message, no secondary header");
    CFE_UtAssert_SUCCESS(CFE_MSG_SetType(msgptr, CFE_MSG_Type_Cmd));
    UtAssert_INT32_EQ(CFE_MSG_GetFcnCode(msgptr, &actual), CFE_MSG_WRONG_MSG_TYPE);
    UtAssert_INT32_EQ(actual, 0);
    UtAssert_INT32_EQ(CFE_MSG_SetFcnCode(msgptr, 0), CFE_MSG_WRONG_MSG_TYPE);
    UtAssert_INT32_EQ(Test_MSG_NotZero(msgptr), MSG_TYPE_FLAG);

    UtPrintf("Bad message, wrong type (telemetry)");
    memset(&cmd, 0, sizeof(cmd));
    actual = TEST_FCNCODE_MAX;
    CFE_UtAssert_SUCCESS(CFE_MSG_SetHasSecondaryHeader(msgptr, true));
    UtAssert_INT32_EQ(CFE_MSG_GetFcnCode(msgptr, &actual), CFE_MSG_WRONG_MSG_TYPE);
    UtAssert_INT32_EQ(actual, 0);
    UtAssert_INT32_EQ(CFE_MSG_SetFcnCode(msgptr, 0), CFE_MSG_WRONG_MSG_TYPE);
    UtAssert_INT32_EQ(Test_MSG_NotZero(msgptr), MSG_HASSEC_FLAG);

    UtPrintf("Set to all F's, various valid inputs");
    for (i = 0; i < sizeof(input) / sizeof(input[0]); i++)
    {
        memset(&cmd, 0xFF, sizeof(cmd));
        CFE_UtAssert_SUCCESS(CFE_MSG_GetFcnCode(msgptr, &actual));
        UtAssert_INT32_EQ(actual, TEST_FCNCODE_MAX);
        CFE_UtAssert_SUCCESS(CFE_MSG_SetFcnCode(msgptr, input[i]));
        UT_DisplayPkt(msgptr, sizeof(cmd));
        CFE_UtAssert_SUCCESS(CFE_MSG_GetFcnCode(msgptr, &actual));
        UtAssert_INT32_EQ(actual, input[i]);
        UtAssert_INT32_EQ(Test_MSG_NotF(msgptr), 0);
    }

    UtPrintf("Set to all 0, various valid inputs");
    for (i = 0; i < sizeof(input) / sizeof(input[0]); i++)
    {
        memset(&cmd, 0, sizeof(cmd));
        CFE_UtAssert_SUCCESS(CFE_MSG_SetType(msgptr, CFE_MSG_Type_Cmd));
        CFE_UtAssert_SUCCESS(CFE_MSG_SetHasSecondaryHeader(msgptr, true));
        CFE_UtAssert_SUCCESS(CFE_MSG_GetFcnCode(msgptr, &actual));
        UtAssert_INT32_EQ(actual, 0);
        CFE_UtAssert_SUCCESS(CFE_MSG_SetFcnCode(msgptr, input[i]));
        UT_DisplayPkt(msgptr, sizeof(cmd));
        CFE_UtAssert_SUCCESS(CFE_MSG_GetFcnCode(msgptr, &actual));
        UtAssert_INT32_EQ(actual, input[i]);
        UtAssert_INT32_EQ(Test_MSG_NotZero(msgptr), MSG_HASSEC_FLAG | MSG_TYPE_FLAG);
    }
}
