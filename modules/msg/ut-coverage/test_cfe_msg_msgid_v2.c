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
 * Test MsgId V2 accessors
 */

/*
 * Includes
 */
#include "utassert.h"
#include "ut_support.h"
#include "cfe_msg.h"
#include "test_msg_not.h"
#include "test_msg_utils.h"
#include "test_cfe_msg_msgid.h"
#include "cfe_error.h"
#include <string.h>

void Test_MSG_MsgId(void)
{
    CFE_MSG_Message_t   msg;
    CFE_SB_MsgId_t      msgid = CFE_SB_ValueToMsgId(1);
    CFE_MSG_Type_t      type;
    CFE_MSG_ApId_t      apid;
    CFE_MSG_Subsystem_t subsystem;
    int                 local_subsys_flag = 0;

    /* Check if subsystem accessor functions are implemented */
    if (CFE_MSG_GetSubsystem(&msg, &subsystem) != CFE_MSG_NOT_IMPLEMENTED)
    {
        local_subsys_flag = MSG_SUBSYS_FLAG;
    }

    UtPrintf("Bad parameter tests, Null pointers and invalid msg ID");
    UT_SetDefaultReturnValue(UT_KEY(CFE_SB_IsValidMsgId), true);
    memset(&msg, 0, sizeof(msg));
    UtAssert_INT32_EQ(CFE_MSG_GetMsgId(NULL, &msgid), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_SB_MsgIdToValue(msgid), 1);
    UtAssert_INT32_EQ(CFE_MSG_GetMsgId(&msg, NULL), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(Test_MSG_NotZero(&msg), 0);
    UtAssert_INT32_EQ(CFE_MSG_SetMsgId(NULL, msgid), CFE_MSG_BAD_ARGUMENT);
    UT_SetDefaultReturnValue(UT_KEY(CFE_SB_IsValidMsgId), false);
    UtAssert_INT32_EQ(CFE_MSG_SetMsgId(&msg, msgid), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(Test_MSG_NotZero(&msg), 0);

    UtPrintf("Set msg to all F's, set msgid to 0 and verify");
    UT_SetDefaultReturnValue(UT_KEY(CFE_SB_IsValidMsgId), true);
    memset(&msg, 0xFF, sizeof(msg));
    CFE_UtAssert_SUCCESS(CFE_MSG_GetMsgId(&msg, &msgid));
    UtAssert_INT32_EQ(CFE_SB_MsgIdToValue(msgid), 0xFFFF);
    CFE_UtAssert_SUCCESS(CFE_MSG_SetMsgId(&msg, 0));
    UT_DisplayPkt(&msg, sizeof(msg));
    CFE_UtAssert_SUCCESS(CFE_MSG_GetMsgId(&msg, &msgid));
    UtAssert_INT32_EQ(CFE_SB_MsgIdToValue(msgid), 0);
    UtAssert_INT32_EQ(Test_MSG_NotF(&msg), MSG_APID_FLAG | MSG_TYPE_FLAG | local_subsys_flag);

    UtPrintf("Set msg to all 0, set msgid to max and verify");
    memset(&msg, 0, sizeof(msg));
    CFE_UtAssert_SUCCESS(CFE_MSG_GetMsgId(&msg, &msgid));
    UtAssert_INT32_EQ(CFE_SB_MsgIdToValue(msgid), 0);
    CFE_UtAssert_SUCCESS(CFE_MSG_SetMsgId(&msg, CFE_PLATFORM_SB_HIGHEST_VALID_MSGID));
    UT_DisplayPkt(&msg, sizeof(msg));
    CFE_UtAssert_SUCCESS(CFE_MSG_GetMsgId(&msg, &msgid));
    UtAssert_INT32_EQ(CFE_SB_MsgIdToValue(msgid), CFE_PLATFORM_SB_HIGHEST_VALID_MSGID);
    UtAssert_INT32_EQ(Test_MSG_NotZero(&msg), MSG_APID_FLAG | MSG_TYPE_FLAG | local_subsys_flag);
    CFE_UtAssert_SUCCESS(CFE_MSG_GetApId(&msg, &apid));
    UtAssert_INT32_EQ(apid, 0x7F);
    CFE_UtAssert_SUCCESS(CFE_MSG_GetType(&msg, &type));
    UtAssert_INT32_EQ(type, CFE_MSG_Type_Cmd);
    if (CFE_MSG_GetSubsystem(&msg, &subsystem) != CFE_MSG_NOT_IMPLEMENTED)
    {
        UtAssert_INT32_EQ(subsystem, CFE_PLATFORM_SB_HIGHEST_VALID_MSGID >> 8);
    }

    UtPrintf("Set ApId msgid bits only and verify");
    CFE_UtAssert_SUCCESS(CFE_MSG_SetMsgId(&msg, CFE_SB_ValueToMsgId(0x007F)));
    CFE_UtAssert_SUCCESS(CFE_MSG_GetMsgId(&msg, &msgid));
    UtAssert_INT32_EQ(Test_MSG_NotZero(&msg), MSG_APID_FLAG);
    CFE_UtAssert_SUCCESS(CFE_MSG_GetApId(&msg, &apid));
    UtAssert_INT32_EQ(apid, 0x007F);

    UtPrintf("Set type msgid bit only and verify");
    CFE_UtAssert_SUCCESS(CFE_MSG_SetMsgId(&msg, CFE_SB_ValueToMsgId(0x0080)));
    CFE_UtAssert_SUCCESS(CFE_MSG_GetMsgId(&msg, &msgid));
    UtAssert_INT32_EQ(Test_MSG_NotZero(&msg), MSG_TYPE_FLAG);
    CFE_UtAssert_SUCCESS(CFE_MSG_GetType(&msg, &type));
    UtAssert_INT32_EQ(type, CFE_MSG_Type_Cmd);

    UtPrintf("Set subsystem msgid bits only and verify");
    CFE_UtAssert_SUCCESS(CFE_MSG_SetMsgId(&msg, CFE_SB_ValueToMsgId(0xFF00 & CFE_PLATFORM_SB_HIGHEST_VALID_MSGID)));
    CFE_UtAssert_SUCCESS(CFE_MSG_GetMsgId(&msg, &msgid));
    UtAssert_INT32_EQ(Test_MSG_NotZero(&msg), local_subsys_flag);
    if (CFE_MSG_GetSubsystem(&msg, &subsystem) != CFE_MSG_NOT_IMPLEMENTED)
    {
        UtAssert_INT32_EQ(subsystem, CFE_PLATFORM_SB_HIGHEST_VALID_MSGID >> 8);
    }
}
