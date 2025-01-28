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
 *   Functional test of the Software Bus Message ID APIs
 */

/*
 * Includes
 */
#include "cfe_test.h"

void TestCheckIfValidMsgId(void)
{
    UtPrintf("Testing: CFE_SB_IsValidMsgId");
    bool           Result;
    CFE_SB_MsgId_t ValidMsgId        = CFE_SB_ValueToMsgId(1);
    CFE_SB_MsgId_t highestValidMsgId = CFE_SB_ValueToMsgId(CFE_PLATFORM_SB_HIGHEST_VALID_MSGID);
    CFE_SB_MsgId_t TooHighMsgId      = CFE_SB_ValueToMsgId(CFE_PLATFORM_SB_HIGHEST_VALID_MSGID + 1);

    /* Test for true result with a valid MsgId */
    Result = CFE_SB_IsValidMsgId(ValidMsgId);
    UtAssert_True(Result == true, "Result == true");

    /* Test for false result with an invalid MsgId (utilizing CFE_SB_INVALID_MSG_ID) */
    Result = CFE_SB_IsValidMsgId(CFE_SB_INVALID_MSG_ID);
    UtAssert_True(Result == false, "Result == false");

    /* Test for true result with the highest valid MsgId (as defined by CFE_PLATFORM_SB_HIGHEST_VALID_MSGID) */
    Result = CFE_SB_IsValidMsgId(highestValidMsgId);
    UtAssert_True(Result == true, "Result == true");

    /*
     * Test for false result with a MsgId that is too high (i.e. above the value
     * set for CFE_PLATFORM_SB_HIGHEST_VALID_MSGID in the platform config).
     */
    Result = CFE_SB_IsValidMsgId(TooHighMsgId);
    UtAssert_True(Result == false, "Result == false");
}

void TestCheckIfMsgIdEqual(void)
{
    UtPrintf("Testing: CFE_SB_MsgId_Equal");
    bool           Result;
    CFE_SB_MsgId_t MsgId1 = CFE_SB_ValueToMsgId(1);
    CFE_SB_MsgId_t MsgId2 = CFE_SB_ValueToMsgId(1);
    CFE_SB_MsgId_t MsgId3 = CFE_SB_ValueToMsgId(2);

    /* Test for true result with equal MsgId's */
    Result = CFE_SB_MsgId_Equal(MsgId1, MsgId2);
    UtAssert_True(Result == true, "Result == true");

    /* Test for false result with unequal MsgId's */
    Result = CFE_SB_MsgId_Equal(MsgId2, MsgId3);
    UtAssert_True(Result == false, "Result == false");
}

void TestConvertValueToMsgAndBack(void)
{
    UtPrintf("Testing: CFE_SB_ValueToMsgId, CFE_SB_MsgIdToValue");
    uint32              Value = 10;
    CFE_SB_MsgId_t      MsgIdResult;
    CFE_SB_MsgId_Atom_t ValueResult;

    /* Test integer value -> MsgId -> integer value */
    MsgIdResult = CFE_SB_ValueToMsgId(Value);
    ValueResult = CFE_SB_MsgIdToValue(MsgIdResult);
    UtAssert_INT32_EQ(ValueResult, Value);
}

void SBMsgIdTestSetup(void)
{
    UtTest_Add(TestCheckIfValidMsgId, NULL, NULL, "Test Check if a MsgId is valid");
    UtTest_Add(TestCheckIfMsgIdEqual, NULL, NULL, "Test Check if two MsgId's are equal");
    UtTest_Add(TestConvertValueToMsgAndBack, NULL, NULL, "Test Convert a value to MsgId and back");
}
