/************************************************************************
 * NASA Docket No. GSC-19,200-1, and identified as "cFS Draco"
 *
 * Copyright (c) 2023 United States Government as represented by the
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
 * Test checksum accessors
 */

/*
 * Includes
 */
#include "utassert.h"
#include "ut_support.h"
#include "test_msg_not.h"
#include "test_msg_utils.h"
#include "cfe_msg.h"
#include "test_cfe_msg_checksum.h"
#include "cfe_error.h"
#include "cfe_msg_defaults.h"
#include <string.h>

void Test_MSG_Checksum(void)
{
    CFE_MSG_CommandHeader_t cmd;
    CFE_MSG_Message_t *     msgptr = CFE_MSG_PTR(cmd);
    bool                    actual;

    UtPrintf("Bad parameter tests, Null pointers");
    memset(&cmd, 0, sizeof(cmd));
    actual = true;
    UtAssert_INT32_EQ(CFE_MSG_GenerateChecksum(NULL), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_ValidateChecksum(NULL, &actual), CFE_MSG_BAD_ARGUMENT);
    UtAssert_BOOL_TRUE(actual);
    UtAssert_INT32_EQ(CFE_MSG_ValidateChecksum(msgptr, NULL), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(Test_MSG_NotZero(msgptr), 0);

    UtPrintf("Bad message, no secondary header");
    CFE_UtAssert_SUCCESS(CFE_MSG_SetType(msgptr, CFE_MSG_Type_Cmd));
    UtAssert_INT32_EQ(CFE_MSG_ValidateChecksum(msgptr, &actual), CFE_MSG_WRONG_MSG_TYPE);
    UtAssert_BOOL_TRUE(actual);
    UtAssert_INT32_EQ(CFE_MSG_GenerateChecksum(msgptr), CFE_MSG_WRONG_MSG_TYPE);
    UtAssert_INT32_EQ(Test_MSG_NotZero(msgptr), MSG_TYPE_FLAG);

    UtPrintf("Bad message, wrong type (telemetry)");
    CFE_UtAssert_SUCCESS(CFE_MSG_SetType(msgptr, CFE_MSG_Type_Tlm));
    CFE_UtAssert_SUCCESS(CFE_MSG_SetHasSecondaryHeader(msgptr, true));
    UtAssert_INT32_EQ(CFE_MSG_ValidateChecksum(msgptr, &actual), CFE_MSG_WRONG_MSG_TYPE);
    UtAssert_BOOL_TRUE(actual);
    UtAssert_INT32_EQ(CFE_MSG_GenerateChecksum(msgptr), CFE_MSG_WRONG_MSG_TYPE);
    UtAssert_INT32_EQ(Test_MSG_NotZero(msgptr), MSG_HASSEC_FLAG);

    UtPrintf("Set to all F's, validate/generate/validate");
    memset(&cmd, 0xFF, sizeof(cmd));
    CFE_UtAssert_SUCCESS(CFE_MSG_SetSize(msgptr, sizeof(cmd)));
    CFE_UtAssert_SUCCESS(CFE_MSG_SetHeaderVersion(msgptr, CFE_MISSION_CCSDSVER));
    CFE_UtAssert_SUCCESS(CFE_MSG_ValidateChecksum(msgptr, &actual));
    UtAssert_BOOL_FALSE(actual);
    CFE_UtAssert_SUCCESS(CFE_MSG_GenerateChecksum(msgptr));
    UT_DisplayPkt(msgptr, sizeof(cmd));
    CFE_UtAssert_SUCCESS(CFE_MSG_ValidateChecksum(msgptr, &actual));
    UtAssert_BOOL_TRUE(actual);
    UtAssert_INT32_EQ(Test_MSG_NotF(msgptr), MSG_LENGTH_FLAG | MSG_HDRVER_FLAG);

    UtPrintf("Set to all 0 except secheader and type, validate/generate/validate");
    memset(&cmd, 0, sizeof(cmd));
    CFE_UtAssert_SUCCESS(CFE_MSG_SetSize(msgptr, sizeof(cmd)));
    CFE_UtAssert_SUCCESS(CFE_MSG_SetType(msgptr, CFE_MSG_Type_Cmd));
    CFE_UtAssert_SUCCESS(CFE_MSG_SetHeaderVersion(msgptr, CFE_MISSION_CCSDSVER));
    CFE_UtAssert_SUCCESS(CFE_MSG_SetHasSecondaryHeader(msgptr, true));
    CFE_UtAssert_SUCCESS(CFE_MSG_ValidateChecksum(msgptr, &actual));
    UtAssert_BOOL_FALSE(actual);
    CFE_UtAssert_SUCCESS(CFE_MSG_GenerateChecksum(msgptr));
    UT_DisplayPkt(msgptr, sizeof(cmd));
    CFE_UtAssert_SUCCESS(CFE_MSG_ValidateChecksum(msgptr, &actual));
    UtAssert_BOOL_TRUE(actual);
    UtAssert_INT32_EQ(Test_MSG_NotZero(msgptr), MSG_LENGTH_FLAG | MSG_HASSEC_FLAG | MSG_TYPE_FLAG);

    UtPrintf("Input wrong CCSDS version number, generate/validate");
    memset(&cmd, 0, sizeof(cmd));
    CFE_UtAssert_SUCCESS(CFE_MSG_SetSize(msgptr, sizeof(cmd)));
    CFE_UtAssert_SUCCESS(CFE_MSG_SetType(msgptr, CFE_MSG_Type_Cmd));
    CFE_UtAssert_SUCCESS(CFE_MSG_SetHasSecondaryHeader(msgptr, true));
    /* Insert wrong version number (typically should be CFE_MISSION_CCSDSVER which is 0) */
    CFE_UtAssert_SUCCESS(CFE_MSG_SetHeaderVersion(msgptr, CFE_MISSION_CCSDSVER + 1));
    UtAssert_BOOL_TRUE(CFE_MSG_GenerateChecksum(msgptr) == CFE_MSG_WRONG_MSG_TYPE);
    UtAssert_BOOL_TRUE(CFE_MSG_ValidateChecksum(msgptr, &actual) == CFE_MSG_WRONG_MSG_TYPE);
    /* call to ValidateChecksum terminated early so actual wasn't changed */
    UtAssert_BOOL_TRUE(actual);
    /* Still complete packet even though version is incorrect + version in header */
    UtAssert_INT32_EQ(Test_MSG_NotZero(msgptr), MSG_LENGTH_FLAG | MSG_HASSEC_FLAG | MSG_TYPE_FLAG | MSG_HDRVER_FLAG);
}
