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
 * Test message init
 */

/*
 * Includes
 */
#include "utassert.h"
#include "ut_support.h"
#include "test_msg_not.h"
#include "test_msg_utils.h"
#include "cfe_msg.h"
#include "test_cfe_msg_init.h"
#include "cfe_error.h"
#include "cfe_msg_defaults.h"
#include <string.h>

#define TEST_DEFAULT_APID_MASK 0x780 /* Bits that can be set by default apid if msgid V2 */

/*
 * Test MSG Init
 */
void Test_MSG_Init(void)
{
    CFE_MSG_CommandHeader_t    cmd;
    CFE_MSG_Size_t             size;
    CFE_SB_MsgId_Atom_t        msgidval_exp;
    CFE_SB_MsgId_t             msgid_act;
    CFE_MSG_HeaderVersion_t    hdrver;
    CFE_MSG_ApId_t             apid;
    CFE_MSG_SegmentationFlag_t segflag;
    bool                       hassec;
    bool                       is_v1;

    msgidval_exp = 1;
    msgid_act    = CFE_SB_ValueToMsgId(msgidval_exp);

    UtPrintf("Bad parameter tests, Null pointer, invalid size, invalid msgid");
    UT_SetDefaultReturnValue(UT_KEY(CFE_SB_IsValidMsgId), true);
    UtAssert_INT32_EQ(CFE_MSG_Init(NULL, msgid_act, sizeof(cmd)), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_Init(CFE_MSG_PTR(cmd), msgid_act, 0), CFE_MSG_BAD_ARGUMENT);
    UT_SetDefaultReturnValue(UT_KEY(CFE_SB_IsValidMsgId), false);
    UtAssert_INT32_EQ(CFE_MSG_Init(CFE_MSG_PTR(cmd), msgid_act, sizeof(cmd)), CFE_MSG_BAD_ARGUMENT);

    UtPrintf("Set to all F's, msgid value = 0");
    UT_SetDefaultReturnValue(UT_KEY(CFE_SB_IsValidMsgId), true);
    memset(&cmd, 0xFF, sizeof(cmd));
    msgidval_exp = 0;

    CFE_UtAssert_SUCCESS(CFE_MSG_Init(CFE_MSG_PTR(cmd), CFE_SB_ValueToMsgId(msgidval_exp), sizeof(cmd)));
    UT_DisplayPkt(CFE_MSG_PTR(cmd), 0);
    CFE_UtAssert_SUCCESS(CFE_MSG_GetMsgId(CFE_MSG_PTR(cmd), &msgid_act));
    UtAssert_INT32_EQ(CFE_SB_MsgIdToValue(msgid_act), msgidval_exp);
    CFE_UtAssert_SUCCESS(CFE_MSG_GetSize(CFE_MSG_PTR(cmd), &size));
    UtAssert_EQ(size_t, size, sizeof(cmd));
    CFE_UtAssert_SUCCESS(CFE_MSG_GetSegmentationFlag(CFE_MSG_PTR(cmd), &segflag));
    UtAssert_INT32_EQ(segflag, CFE_MSG_SegFlag_Unsegmented);

    CFE_UtAssert_SUCCESS(CFE_MSG_GetApId(CFE_MSG_PTR(cmd), &apid));
    CFE_UtAssert_SUCCESS(CFE_MSG_GetHeaderVersion(CFE_MSG_PTR(cmd), &hdrver));
    CFE_UtAssert_SUCCESS(CFE_MSG_GetHasSecondaryHeader(CFE_MSG_PTR(cmd), &hassec));

    /* A zero msgid will set hassec to false for v1 */
    is_v1 = !hassec;

    if (!is_v1)
    {
        UtAssert_INT32_EQ(apid, CFE_PLATFORM_DEFAULT_APID & TEST_DEFAULT_APID_MASK);
        UtAssert_INT32_EQ(hdrver, CFE_MISSION_CCSDSVER);
    }
    else
    {
        UtAssert_INT32_EQ(apid, 0);
        UtAssert_INT32_EQ(hdrver, 0);
    }

    /* Confirm the rest of the fields not already explicitly checked */
    UtAssert_UINT32_EQ(Test_MSG_Pri_NotZero(CFE_MSG_PTR(cmd)) & ~(MSG_APID_FLAG | MSG_HDRVER_FLAG | MSG_HASSEC_FLAG),
                       MSG_LENGTH_FLAG | MSG_SEGMENT_FLAG);

    UtPrintf("Set to all 0, max msgid value");
    memset(&cmd, 0, sizeof(cmd));
    msgidval_exp = CFE_PLATFORM_SB_HIGHEST_VALID_MSGID;

    CFE_UtAssert_SUCCESS(CFE_MSG_Init(CFE_MSG_PTR(cmd), CFE_SB_ValueToMsgId(msgidval_exp), sizeof(cmd)));
    UT_DisplayPkt(CFE_MSG_PTR(cmd), 0);
    CFE_UtAssert_SUCCESS(CFE_MSG_GetMsgId(CFE_MSG_PTR(cmd), &msgid_act));
    UtAssert_INT32_EQ(CFE_SB_MsgIdToValue(msgid_act), msgidval_exp);
    CFE_UtAssert_SUCCESS(CFE_MSG_GetSize(CFE_MSG_PTR(cmd), &size));
    UtAssert_INT32_EQ(size, sizeof(cmd));
    CFE_UtAssert_SUCCESS(CFE_MSG_GetSegmentationFlag(CFE_MSG_PTR(cmd), &segflag));
    UtAssert_INT32_EQ(segflag, CFE_MSG_SegFlag_Unsegmented);

    CFE_UtAssert_SUCCESS(CFE_MSG_GetApId(CFE_MSG_PTR(cmd), &apid));
    CFE_UtAssert_SUCCESS(CFE_MSG_GetHeaderVersion(CFE_MSG_PTR(cmd), &hdrver));
    CFE_UtAssert_SUCCESS(CFE_MSG_GetHasSecondaryHeader(CFE_MSG_PTR(cmd), &hassec));
    UtAssert_BOOL_TRUE(hassec);
    if (!is_v1)
    {
        UtAssert_INT32_EQ(apid & TEST_DEFAULT_APID_MASK, CFE_PLATFORM_DEFAULT_APID & TEST_DEFAULT_APID_MASK);
        UtAssert_INT32_EQ(hdrver, CFE_MISSION_CCSDSVER);
    }
    else
    {
        UtAssert_INT32_EQ(apid, 0x7FF);
        UtAssert_INT32_EQ(hdrver, 0);
    }

    UtAssert_UINT32_EQ(Test_MSG_Pri_NotZero(CFE_MSG_PTR(cmd)) & ~MSG_HDRVER_FLAG,
                       MSG_APID_FLAG | MSG_HASSEC_FLAG | MSG_TYPE_FLAG | MSG_LENGTH_FLAG | MSG_SEGMENT_FLAG);
}
