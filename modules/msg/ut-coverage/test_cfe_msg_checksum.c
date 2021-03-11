/*
**  GSC-18128-1, "Core Flight Executive Version 6.7"
**
**  Copyright (c) 2006-2019 United States Government as represented by
**  the Administrator of the National Aeronautics and Space Administration.
**  All Rights Reserved.
**
**  Licensed under the Apache License, Version 2.0 (the "License");
**  you may not use this file except in compliance with the License.
**  You may obtain a copy of the License at
**
**    http://www.apache.org/licenses/LICENSE-2.0
**
**  Unless required by applicable law or agreed to in writing, software
**  distributed under the License is distributed on an "AS IS" BASIS,
**  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**  See the License for the specific language governing permissions and
**  limitations under the License.
*/

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
#include <string.h>

void Test_MSG_Checksum(void)
{
    CFE_MSG_CommandHeader_t cmd;
    CFE_MSG_Message_t *     msgptr = &cmd.Msg;
    bool                    actual;

    UtPrintf("Bad parameter tests, Null pointers");
    memset(&cmd, 0, sizeof(cmd));
    actual = true;
    ASSERT_EQ(CFE_MSG_GenerateChecksum(NULL), CFE_MSG_BAD_ARGUMENT);
    ASSERT_EQ(CFE_MSG_ValidateChecksum(NULL, &actual), CFE_MSG_BAD_ARGUMENT);
    ASSERT_EQ(actual, true);
    ASSERT_EQ(CFE_MSG_ValidateChecksum(msgptr, NULL), CFE_MSG_BAD_ARGUMENT);
    ASSERT_EQ(Test_MSG_NotZero(msgptr), 0);

    UtPrintf("Bad message, no secondary header");
    ASSERT_EQ(CFE_MSG_SetType(msgptr, CFE_MSG_Type_Cmd), CFE_SUCCESS);
    ASSERT_EQ(CFE_MSG_ValidateChecksum(msgptr, &actual), CFE_MSG_WRONG_MSG_TYPE);
    ASSERT_EQ(actual, true);
    ASSERT_EQ(CFE_MSG_GenerateChecksum(msgptr), CFE_MSG_WRONG_MSG_TYPE);
    ASSERT_EQ(Test_MSG_NotZero(msgptr), MSG_TYPE_FLAG);

    UtPrintf("Bad message, wrong type (telemetry)");
    ASSERT_EQ(CFE_MSG_SetType(msgptr, CFE_MSG_Type_Tlm), CFE_SUCCESS);
    ASSERT_EQ(CFE_MSG_SetHasSecondaryHeader(msgptr, true), CFE_SUCCESS);
    ASSERT_EQ(CFE_MSG_ValidateChecksum(msgptr, &actual), CFE_MSG_WRONG_MSG_TYPE);
    ASSERT_EQ(actual, true);
    ASSERT_EQ(CFE_MSG_GenerateChecksum(msgptr), CFE_MSG_WRONG_MSG_TYPE);
    ASSERT_EQ(Test_MSG_NotZero(msgptr), MSG_HASSEC_FLAG);

    UtPrintf("Set to all F's, validate/generate/validate");
    memset(&cmd, 0xFF, sizeof(cmd));
    ASSERT_EQ(CFE_MSG_SetSize(msgptr, sizeof(cmd)), CFE_SUCCESS);
    ASSERT_EQ(CFE_MSG_ValidateChecksum(msgptr, &actual), CFE_SUCCESS);
    ASSERT_EQ(actual, false);
    ASSERT_EQ(CFE_MSG_GenerateChecksum(msgptr), CFE_SUCCESS);
    UT_DisplayPkt(msgptr, sizeof(cmd));
    ASSERT_EQ(CFE_MSG_ValidateChecksum(msgptr, &actual), CFE_SUCCESS);
    ASSERT_EQ(actual, true);
    ASSERT_EQ(Test_MSG_NotF(msgptr), MSG_LENGTH_FLAG);

    UtPrintf("Set to all 0 except secheader and type, validate/generate/validate");
    memset(&cmd, 0, sizeof(cmd));
    ASSERT_EQ(CFE_MSG_SetSize(msgptr, sizeof(cmd)), CFE_SUCCESS);
    ASSERT_EQ(CFE_MSG_SetType(msgptr, CFE_MSG_Type_Cmd), CFE_SUCCESS);
    ASSERT_EQ(CFE_MSG_SetHasSecondaryHeader(msgptr, true), CFE_SUCCESS);
    ASSERT_EQ(CFE_MSG_ValidateChecksum(msgptr, &actual), CFE_SUCCESS);
    ASSERT_EQ(actual, false);
    ASSERT_EQ(CFE_MSG_GenerateChecksum(msgptr), CFE_SUCCESS);
    UT_DisplayPkt(msgptr, sizeof(cmd));
    ASSERT_EQ(CFE_MSG_ValidateChecksum(msgptr, &actual), CFE_SUCCESS);
    ASSERT_EQ(actual, true);
    ASSERT_EQ(Test_MSG_NotZero(msgptr), MSG_LENGTH_FLAG | MSG_HASSEC_FLAG | MSG_TYPE_FLAG);
}
