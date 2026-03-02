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
** File:
**    sb_UT.c
**
** Purpose:
**    Software Bus Services unit test
**
** References:
**    1. cFE Application Developers Guide
**    2. unit test standard 092503
**    3. C Coding Standard 102904
**
** Notes:
**    1. This is unit test code only, not for use in flight
**
*/

/*
** Includes
*/
#include "sb_ut_helpers.h"

/*
 * MSG ID constants for unit testing:
 * Unit test cases should not directly use integer MsgId values
 *
 * The following constants are of the CFE_SB_MsgId_t type
 */

const CFE_SB_MsgId_t SB_UT_CMD_MID = CFE_SB_MSGID_WRAP_VALUE(SB_UT_CMD_MID_VALUE_BASE);
const CFE_SB_MsgId_t SB_UT_TLM_MID = CFE_SB_MSGID_WRAP_VALUE(SB_UT_TLM_MID_VALUE_BASE);

const CFE_SB_MsgId_t SB_UT_CMD_MID1 = CFE_SB_MSGID_WRAP_VALUE(SB_UT_CMD_MID_VALUE_BASE + 1);
const CFE_SB_MsgId_t SB_UT_CMD_MID2 = CFE_SB_MSGID_WRAP_VALUE(SB_UT_CMD_MID_VALUE_BASE + 2);
const CFE_SB_MsgId_t SB_UT_CMD_MID3 = CFE_SB_MSGID_WRAP_VALUE(SB_UT_CMD_MID_VALUE_BASE + 3);
const CFE_SB_MsgId_t SB_UT_CMD_MID4 = CFE_SB_MSGID_WRAP_VALUE(SB_UT_CMD_MID_VALUE_BASE + 4);
const CFE_SB_MsgId_t SB_UT_CMD_MID5 = CFE_SB_MSGID_WRAP_VALUE(SB_UT_CMD_MID_VALUE_BASE + 5);
const CFE_SB_MsgId_t SB_UT_CMD_MID6 = CFE_SB_MSGID_WRAP_VALUE(SB_UT_CMD_MID_VALUE_BASE + 6);

const CFE_SB_MsgId_t SB_UT_TLM_MID1 = CFE_SB_MSGID_WRAP_VALUE(SB_UT_TLM_MID_VALUE_BASE + 1);
const CFE_SB_MsgId_t SB_UT_TLM_MID2 = CFE_SB_MSGID_WRAP_VALUE(SB_UT_TLM_MID_VALUE_BASE + 2);
const CFE_SB_MsgId_t SB_UT_TLM_MID3 = CFE_SB_MSGID_WRAP_VALUE(SB_UT_TLM_MID_VALUE_BASE + 3);
const CFE_SB_MsgId_t SB_UT_TLM_MID4 = CFE_SB_MSGID_WRAP_VALUE(SB_UT_TLM_MID_VALUE_BASE + 4);
const CFE_SB_MsgId_t SB_UT_TLM_MID5 = CFE_SB_MSGID_WRAP_VALUE(SB_UT_TLM_MID_VALUE_BASE + 5);
const CFE_SB_MsgId_t SB_UT_TLM_MID6 = CFE_SB_MSGID_WRAP_VALUE(SB_UT_TLM_MID_VALUE_BASE + 6);

/*
 * A MsgId value which still qualifies as valid
 *
 * This is a  "borderline" value to test the limits of the validity checking
 * The specific value depends on how MsgId is actually defined internally
 */
const CFE_SB_MsgId_t SB_UT_LAST_VALID_MID = CFE_SB_MSGID_WRAP_VALUE(CFE_PLATFORM_SB_HIGHEST_VALID_MSGID);

/*
 * A MsgId value which still qualifies as valid
 *
 * This is a  "borderline" value to test the limits of the validity checking
 * The specific value depends on how MsgId is actually defined internally
 */
const CFE_SB_MsgId_t SB_UT_FIRST_VALID_MID = CFE_SB_MSGID_WRAP_VALUE(1);

/*
 * A MsgId value which is in the middle of the valid range
 *
 * The specific value depends on how MsgId is actually defined internally
 */
const CFE_SB_MsgId_t SB_UT_INTERMEDIATE_VALID_MID =
    CFE_SB_MSGID_WRAP_VALUE(CFE_PLATFORM_SB_HIGHEST_VALID_MSGID / 2 + 1);

/*
 * A MsgId value which is not valid but also not equal to CFE_SB_INVALID_MSG_ID
 * Like CFE_SB_INVALID_MSG_ID, this should also _not_ pass the validity check.
 */
const CFE_SB_MsgId_t SB_UT_ALTERNATE_INVALID_MID = CFE_SB_MSGID_WRAP_VALUE(CFE_PLATFORM_SB_HIGHEST_VALID_MSGID + 1);

/*
 * A MsgId value which is valid per CCSDS but does not have the secondary header bit set
 */
const CFE_SB_MsgId_t SB_UT_BARE_CMD_MID3 = CFE_SB_MSGID_WRAP_VALUE(0x1003);
const CFE_SB_MsgId_t SB_UT_BARE_TLM_MID3 = CFE_SB_MSGID_WRAP_VALUE(0x0003);

/*
 * Helper function to manufacture a fake pipe ID value that will validate
 */
CFE_ResourceId_t UT_SB_MakePipeIdForIndex(uint32 ArrayIdx)
{
    return CFE_ResourceId_FromInteger(CFE_SB_PIPEID_BASE + ArrayIdx);
}

/*
 * Helper function to "corrupt" a resource ID value in a consistent/predictable way,
 * which can also be un-done easily.
 */
CFE_ES_AppId_t UT_SB_AppID_Modify(CFE_ES_AppId_t InitialID, int32 Modifier)
{
    CFE_ES_AppId_t OutValue;
    uint32         InValue;

    InValue = CFE_RESOURCEID_TO_ULONG(InitialID);
    InValue += Modifier;

    /* Underneath the wrapper(s) the IDs are 32-bit integer values, so it can be copied */
    memcpy(&OutValue, &InValue, sizeof(OutValue));

    return OutValue;
}

CFE_Status_t SB_UT_OriginationActionHook(void *UserObj, int32 StubRetcode, uint32 CallCount,
                                         const UT_StubContext_t *Context)
{
    return CFE_SB_BAD_ARGUMENT;
}

void SB_UT_OriginationActionHandler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    bool *IsAcceptable = UT_Hook_GetArgValueByName(Context, "IsAcceptable", bool *);

    /* by default just always return true -- a UT case that needs something else can override this handler */
    if (IsAcceptable != NULL)
    {
        *IsAcceptable = false;
    }
}

CFE_Status_t SB_UT_RecieveBuffer_FalseEndpoint(CFE_SB_Buffer_t **BufPtr, CFE_SB_PipeId_t PipeId, int32 TimeOut)
{
    CFE_SB_ReceiveTxn_State_t  TxnBuf;
    CFE_SB_MessageTxn_State_t *Txn;

    Txn = CFE_SB_ReceiveTxn_Init(&TxnBuf, BufPtr);

    if (CFE_SB_MessageTxn_IsOK(Txn))
    {
        CFE_SB_MessageTxn_SetTimeout(Txn, TimeOut);
    }

    if (CFE_SB_MessageTxn_IsOK(Txn))
    {
        CFE_SB_ReceiveTxn_SetPipeId(Txn, PipeId);

        /*
         * This is the key difference in this Handler as opposed to the actual
         * function. We set Txn->IsEndpoint to false in order to trigger the
         * false branch of the condition "if (Txn->IsEndpoint)" in
         * CFE_SB_ReceieveTxn_Execute().
         */
        CFE_SB_MessageTxn_SetEndpoint(Txn, false);
    }

    if (BufPtr != NULL)
    {
        /*
         * Note - the API should qualify the parameter as "const", but this is
         * kept non-const for backward compatibility.  Callers should never write to
         * the returned buffer, it is const in practice.
         */
        *BufPtr = (CFE_SB_Buffer_t *)CFE_SB_ReceiveTxn_Execute(Txn);
    }

    CFE_SB_MessageTxn_ReportEvents(Txn);

    return CFE_SB_MessageTxn_GetStatus(Txn);
}

/*
** Reset variable values and sockets prior to a test
*/
void SB_ResetUnitTest(void)
{
    UT_InitData();
    CFE_SB_EarlyInit();

    SB_UT_BindDispatchHandlers();
}
