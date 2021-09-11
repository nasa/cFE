/*************************************************************************
**
**      GSC-18128-1, "Core Flight Executive Version 6.7"
**
**      Copyright (c) 2006-2021 United States Government as represented by
**      the Administrator of the National Aeronautics and Space Administration.
**      All Rights Reserved.
**
**      Licensed under the Apache License, Version 2.0 (the "License");
**      you may not use this file except in compliance with the License.
**      You may obtain a copy of the License at
**
**        http://www.apache.org/licenses/LICENSE-2.0
**
**      Unless required by applicable law or agreed to in writing, software
**      distributed under the License is distributed on an "AS IS" BASIS,
**      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**      See the License for the specific language governing permissions and
**      limitations under the License.
**
** File: msg_api_test.c
**
** Purpose:
**   Functional test of cFE Message header APIs.
**
*************************************************************************/

/*
 * Includes
 */

#include "cfe_test.h"
#include <string.h>

void TestMsgApiBasic(void)
{
    UtPrintf("Testing: CFE_MSG_Init, CFE_MSG_GetSize, CFE_MSG_SetSize, CFE_MSG_GetType, "
             "CFE_MSG_SetType, CFE_MSG_GetHeaderVersion, CFE_MSG_SetHeaderVersion, "
             "CFE_MSG_GetHasSecondaryHeader, CFE_MSG_SetHasSecondaryHeader, "
             "CFE_MSG_GetApId, CFE_MSG_SetApId");

    /* declare local vars */
    CFE_MSG_CommandHeader_t cmd;
    CFE_MSG_Size_t          size;
    CFE_MSG_Type_t          type;
    CFE_SB_MsgId_t          msgId;
    CFE_MSG_HeaderVersion_t hdrVer;
    CFE_MSG_ApId_t          appId;
    bool                    _expected = true;
    bool                    _returned = false;

    memset(&cmd, 0xFF, sizeof(cmd));
    msgId = CFE_SB_ValueToMsgId(0);

    /* test msg-init */
    UtAssert_INT32_EQ(CFE_MSG_Init(NULL, CFE_SB_ValueToMsgId(0), sizeof(cmd)), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_Init(&cmd.Msg, msgId, 0), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_Init(&cmd.Msg, CFE_PLATFORM_SB_HIGHEST_VALID_MSGID + 1, sizeof(cmd)),
                      CFE_MSG_BAD_ARGUMENT);

    UtAssert_INT32_EQ(CFE_MSG_Init(&cmd.Msg, msgId, sizeof(cmd)), CFE_SUCCESS);

    /* test set-msg-size */
    UtAssert_INT32_EQ(CFE_MSG_SetSize(NULL, 12), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_SetSize(&cmd.Msg, 0), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_SetSize(&cmd.Msg, UINT32_MAX), CFE_MSG_BAD_ARGUMENT);

    UtAssert_INT32_EQ(CFE_MSG_SetSize(&cmd.Msg, 12), CFE_SUCCESS);

    /* test get-msg-size */
    UtAssert_INT32_EQ(CFE_MSG_GetSize(NULL, &size), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_GetSize(&cmd.Msg, NULL), CFE_MSG_BAD_ARGUMENT);

    UtAssert_INT32_EQ(CFE_MSG_GetSize(&cmd.Msg, &size), CFE_SUCCESS);
    UtAssert_UINT32_EQ(size, 12);

    /* test get-type */
    UtAssert_INT32_EQ(CFE_MSG_SetType(NULL, CFE_MSG_Type_Cmd), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_SetType(&cmd.Msg, CFE_MSG_Type_Invalid), CFE_MSG_BAD_ARGUMENT);

    UtAssert_INT32_EQ(CFE_MSG_SetType(&cmd.Msg, CFE_MSG_Type_Cmd), CFE_SUCCESS);

    UtAssert_INT32_EQ(CFE_MSG_GetType(NULL, &type), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_GetType(&cmd.Msg, NULL), CFE_MSG_BAD_ARGUMENT);

    UtAssert_INT32_EQ(CFE_MSG_GetType(&cmd.Msg, &type), CFE_SUCCESS);
    UtAssert_INT32_EQ(type, CFE_MSG_Type_Cmd);

    /* test msg set-type */
    UtAssert_INT32_EQ(CFE_MSG_SetType(&cmd.Msg, CFE_MSG_Type_Tlm), CFE_SUCCESS);
    /* check if set-type works like expected */
    UtAssert_INT32_EQ(CFE_MSG_GetType(&cmd.Msg, &type), CFE_SUCCESS);
    UtAssert_INT32_EQ(type, CFE_MSG_Type_Tlm);

    /* test get header-version */
    UtAssert_INT32_EQ(CFE_MSG_GetHeaderVersion(NULL, &hdrVer), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_GetHeaderVersion(&cmd.Msg, &hdrVer), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_GetHeaderVersion(&cmd.Msg, NULL), CFE_MSG_BAD_ARGUMENT);

    /* test set header-version */
    UtAssert_INT32_EQ(CFE_MSG_SetHeaderVersion(NULL, hdrVer), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_SetHeaderVersion(&cmd.Msg, UINT16_MAX), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_SetHeaderVersion(&cmd.Msg, 0), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_GetHeaderVersion(&cmd.Msg, &hdrVer), CFE_SUCCESS);
    UtAssert_True(hdrVer == 0, "hdrVer = 0");

    /* test get-has-secondary-header and set-has-secondary-header*/
    UtAssert_INT32_EQ(CFE_MSG_GetHasSecondaryHeader(NULL, &_expected), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_GetHasSecondaryHeader(&cmd.Msg, NULL), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_SetHasSecondaryHeader(NULL, _expected), CFE_MSG_BAD_ARGUMENT);

    UtAssert_INT32_EQ(CFE_MSG_SetHasSecondaryHeader(&cmd.Msg, _expected), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_GetHasSecondaryHeader(&cmd.Msg, &_returned), CFE_SUCCESS);
    UtAssert_UINT32_EQ(_expected, _returned);

    /* test get-apid */
    UtAssert_INT32_EQ(CFE_MSG_GetApId(NULL, &appId), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_GetApId(&cmd.Msg, NULL), CFE_MSG_BAD_ARGUMENT);

    /* test set-apid */
    UtAssert_INT32_EQ(CFE_MSG_SetApId(&cmd.Msg, 0xFFFF), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_SetApId(NULL, 0), CFE_MSG_BAD_ARGUMENT);

    UtAssert_INT32_EQ(CFE_MSG_SetApId(&cmd.Msg, 5), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_GetApId(&cmd.Msg, &appId), CFE_SUCCESS);
    UtAssert_INT32_EQ(appId, 5);
}

void TestMsgApiAdvanced(void)
{
    UtPrintf("Testing: CFE_MSG_GetSegmentationFlag, CFE_MSG_SetSegmentationFlag, "
             "CFE_MSG_GetSequenceCount, CFE_MSG_SetSequenceCount, CFE_MSG_GetNextSequenceCount");

    /* declare local vars */
    CFE_MSG_CommandHeader_t    cmd;
    CFE_SB_MsgId_t             msgId;
    CFE_MSG_SegmentationFlag_t segFlag;
    CFE_MSG_SequenceCount_t    seqCnt;

    memset(&cmd, 0xFF, sizeof(cmd));
    msgId = CFE_SB_ValueToMsgId(0);

    UtAssert_INT32_EQ(CFE_MSG_Init(&cmd.Msg, msgId, sizeof(cmd)), CFE_SUCCESS);

    /* test get/set-segmentation-flag */
    UtAssert_INT32_EQ(CFE_MSG_GetSegmentationFlag(NULL, &segFlag), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_GetSegmentationFlag(&cmd.Msg, NULL), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_SetSegmentationFlag(NULL, CFE_MSG_SegFlag_Continue), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_SetSegmentationFlag(&cmd.Msg, CFE_MSG_SegFlag_Invalid), CFE_MSG_BAD_ARGUMENT);

    UtAssert_INT32_EQ(CFE_MSG_SetSegmentationFlag(&cmd.Msg, CFE_MSG_SegFlag_Continue), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_SetSegmentationFlag(&cmd.Msg, CFE_MSG_SegFlag_First), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_SetSegmentationFlag(&cmd.Msg, CFE_MSG_SegFlag_Last), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_SetSegmentationFlag(&cmd.Msg, CFE_MSG_SegFlag_Unsegmented), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_GetSegmentationFlag(&cmd.Msg, &segFlag), CFE_SUCCESS);
    UtAssert_UINT32_EQ(segFlag, CFE_MSG_SegFlag_Unsegmented);

    /* test set/get-sequence-count */
    UtAssert_INT32_EQ(CFE_MSG_SetSequenceCount(NULL, 2), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_SetSequenceCount(&cmd.Msg, UINT16_MAX), CFE_MSG_BAD_ARGUMENT);

    UtAssert_INT32_EQ(CFE_MSG_GetSequenceCount(NULL, &seqCnt), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_GetSequenceCount(&cmd.Msg, NULL), CFE_MSG_BAD_ARGUMENT);

    UtAssert_INT32_EQ(CFE_MSG_SetSequenceCount(&cmd.Msg, 2), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_GetSequenceCount(&cmd.Msg, &seqCnt), CFE_SUCCESS);
    UtAssert_INT32_EQ(seqCnt, 2);

    /* test get-next-sequence-count */
    UtAssert_INT32_EQ(CFE_MSG_GetNextSequenceCount(seqCnt), 3);
    UtAssert_INT32_EQ(CFE_MSG_GetNextSequenceCount(UINT16_MAX), 0);
}

void TestMsgHeaderSecondaryApi(void)
{
    UtPrintf("Testing: CFE_MSG_GenerateChecksum, CFE_MSG_ValidateChecksum, CFE_MSG_SetFcnCode, "
             "CFE_MSG_GetFcnCode, CFE_MSG_GetMsgTime, CFE_MSG_SetMsgTime ");

    /* declare local vars */
    CFE_MSG_CommandHeader_t cmd;
    CFE_MSG_CommandHeader_t cmdTlm;
    CFE_MSG_CommandHeader_t cmd2;
    CFE_MSG_FcnCode_t       fcnCode;
    CFE_TIME_SysTime_t      msgTime;
    bool                    isValid     = true;
    CFE_TIME_SysTime_t      currentTime = {1000, 0xFFFF0000};
    CFE_Status_t            status;

    memset(&cmd, 0, sizeof(cmd));
    memset(&cmdTlm, 0xFF, sizeof(cmdTlm));
    memset(&cmd2, 0xFF, sizeof(cmd2));

    /* msg-init */
    UtAssert_INT32_EQ(CFE_MSG_Init(&cmd.Msg, CFE_SB_ValueToMsgId(0), sizeof(cmd)), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_SetHasSecondaryHeader(&cmd.Msg, true), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_SetType(&cmd.Msg, CFE_MSG_Type_Cmd), CFE_SUCCESS);

    UtAssert_INT32_EQ(CFE_MSG_Init(&cmdTlm.Msg, CFE_SB_ValueToMsgId(1), sizeof(cmdTlm)), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_SetHasSecondaryHeader(&cmdTlm.Msg, true), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_SetType(&cmdTlm.Msg, CFE_MSG_Type_Tlm), CFE_SUCCESS);

    UtAssert_INT32_EQ(CFE_MSG_Init(&cmd2.Msg, CFE_SB_ValueToMsgId(2), sizeof(cmd2)), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_SetHasSecondaryHeader(&cmd2.Msg, true), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_SetType(&cmd2.Msg, CFE_MSG_Type_Cmd), CFE_SUCCESS);

    /* test generate-checksum */
    status = CFE_MSG_GenerateChecksum(NULL);
    if (status == CFE_MSG_NOT_IMPLEMENTED)
    {
        UtAssert_NA("CFE_MSG_GenerateChecksum not implemented");
    }
    else
    {
        UtAssert_INT32_EQ(CFE_MSG_GenerateChecksum(NULL), CFE_MSG_BAD_ARGUMENT);
        UtAssert_INT32_EQ(CFE_MSG_GenerateChecksum(&cmdTlm.Msg), CFE_MSG_WRONG_MSG_TYPE);
        UtAssert_INT32_EQ(CFE_MSG_ValidateChecksum(&cmdTlm.Msg, &isValid), CFE_MSG_WRONG_MSG_TYPE);
        UtAssert_INT32_EQ(CFE_MSG_ValidateChecksum(NULL, &isValid), CFE_MSG_BAD_ARGUMENT);
        UtAssert_INT32_EQ(CFE_MSG_ValidateChecksum(&cmdTlm.Msg, NULL), CFE_MSG_BAD_ARGUMENT);

        UtAssert_INT32_EQ(CFE_MSG_ValidateChecksum(&cmd.Msg, &isValid), CFE_SUCCESS);
        UtAssert_True(!isValid, "Checksum isValid (%d) = false", isValid);
        UtAssert_INT32_EQ(CFE_MSG_GenerateChecksum(&cmd.Msg), CFE_SUCCESS);
        UtAssert_INT32_EQ(CFE_MSG_ValidateChecksum(&cmd.Msg, &isValid), CFE_SUCCESS);
        UtAssert_True(isValid, "Checksum isValid (%d) = true", isValid);
    }

    /* test get/set-fcn-code */
    UtAssert_INT32_EQ(CFE_MSG_SetFcnCode(NULL, 4), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_SetFcnCode(&cmdTlm.Msg, 4), CFE_MSG_WRONG_MSG_TYPE);

    UtAssert_INT32_EQ(CFE_MSG_GetFcnCode(&cmd.Msg, NULL), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_GetFcnCode(NULL, &fcnCode), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_GetFcnCode(&cmdTlm.Msg, &fcnCode), CFE_MSG_WRONG_MSG_TYPE);

    UtAssert_INT32_EQ(CFE_MSG_SetFcnCode(&cmd.Msg, 4), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_GetFcnCode(&cmd.Msg, &fcnCode), CFE_SUCCESS);
    UtAssert_INT32_EQ(fcnCode, 4);

    /* test get/set-msg-time */
    UtAssert_INT32_EQ(CFE_MSG_SetType(&cmd.Msg, CFE_MSG_Type_Tlm), CFE_SUCCESS);

    UtAssert_INT32_EQ(CFE_MSG_GetMsgTime(NULL, &msgTime), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_GetMsgTime(&cmd.Msg, NULL), CFE_MSG_BAD_ARGUMENT);

    CFE_Assert_STATUS_STORE(CFE_MSG_GetMsgTime(&cmd2.Msg, &msgTime));
    if (!CFE_Assert_STATUS_MAY_BE(CFE_SUCCESS))
    {
        CFE_Assert_STATUS_MUST_BE(CFE_MSG_WRONG_MSG_TYPE);
    }

    UtAssert_INT32_EQ(CFE_MSG_SetMsgTime(NULL, currentTime), CFE_MSG_BAD_ARGUMENT);

    CFE_Assert_STATUS_STORE(CFE_MSG_SetMsgTime(&cmd2.Msg, currentTime));
    if (!CFE_Assert_STATUS_MAY_BE(CFE_SUCCESS))
    {
        CFE_Assert_STATUS_MUST_BE(CFE_MSG_WRONG_MSG_TYPE);
    }

    UtAssert_INT32_EQ(CFE_MSG_SetMsgTime(&cmd.Msg, currentTime), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_GetMsgTime(&cmd.Msg, &msgTime), CFE_SUCCESS);
    UtAssert_UINT32_EQ(CFE_TIME_Compare(msgTime, currentTime), CFE_TIME_EQUAL);
}

void MsgApiTestSetup(void)
{
    UtTest_Add(TestMsgApiBasic, NULL, NULL, "Test basic message header apis");
    UtTest_Add(TestMsgApiAdvanced, NULL, NULL, "Test advanced message header apis");
    UtTest_Add(TestMsgHeaderSecondaryApi, NULL, NULL, "Test message secondary header apis");
}
