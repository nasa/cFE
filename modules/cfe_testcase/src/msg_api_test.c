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

    memset(&cmd, 0xFF, sizeof(cmd));
    msgId = CFE_SB_ValueToMsgId(0);

    /* test msg-init */
    UtAssert_INT32_EQ(CFE_MSG_Init(NULL, CFE_SB_ValueToMsgId(0), sizeof(cmd)), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_Init(&cmd.Msg, msgId, 0), CFE_MSG_BAD_ARGUMENT);

    cFE_FTAssert_SUCCESS(CFE_MSG_Init(&cmd.Msg, msgId, sizeof(cmd)));

    /* test set-msg-size */
    UtAssert_INT32_EQ(CFE_MSG_SetSize(NULL, 12), CFE_MSG_BAD_ARGUMENT);

    cFE_FTAssert_SUCCESS(CFE_MSG_SetSize(&cmd.Msg, 12));

    /* test get-msg-size */
    UtAssert_INT32_EQ(CFE_MSG_GetSize(NULL, &size), CFE_MSG_BAD_ARGUMENT);

    cFE_FTAssert_SUCCESS(CFE_MSG_GetSize(&cmd.Msg, &size));
    UtAssert_UINT32_EQ(size, 12);

    /* test get-type */
    UtAssert_INT32_EQ(CFE_MSG_SetType(NULL, CFE_MSG_Type_Cmd), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_GetType(NULL, &type), CFE_MSG_BAD_ARGUMENT);
    cFE_FTAssert_SUCCESS(CFE_MSG_SetType(&cmd.Msg, CFE_MSG_Type_Cmd));

    cFE_FTAssert_SUCCESS(CFE_MSG_GetType(&cmd.Msg, &type));
    UtAssert_INT32_EQ(type, CFE_MSG_Type_Cmd);

    /* test msg set-type */
    cFE_FTAssert_SUCCESS(CFE_MSG_SetType(&cmd.Msg, CFE_MSG_Type_Tlm));
    /* check if set-type works like expected */
    cFE_FTAssert_SUCCESS(CFE_MSG_GetType(&cmd.Msg, &type));
    UtAssert_INT32_EQ(type, CFE_MSG_Type_Tlm);

    /* test get header-version */
    UtAssert_INT32_EQ(CFE_MSG_GetHeaderVersion(NULL, &hdrVer), CFE_MSG_BAD_ARGUMENT);
    cFE_FTAssert_SUCCESS(CFE_MSG_GetHeaderVersion(&cmd.Msg, &hdrVer));
    UtAssert_INT32_EQ(CFE_MSG_GetHeaderVersion(&cmd.Msg, NULL), CFE_MSG_BAD_ARGUMENT);

    /* test set header-version */
    UtAssert_INT32_EQ(CFE_MSG_SetHeaderVersion(NULL, hdrVer), CFE_MSG_BAD_ARGUMENT);
    cFE_FTAssert_SUCCESS(CFE_MSG_SetHeaderVersion(&cmd.Msg, 0));
    cFE_FTAssert_SUCCESS(CFE_MSG_GetHeaderVersion(&cmd.Msg, &hdrVer));
    UtAssert_True(hdrVer == 0, "hdrVer = 0");

    /* test get-has-secondary-header and set-has-scondary-header*/
    bool _expected = true;
    bool _returned = false;

    UtAssert_INT32_EQ(CFE_MSG_GetHasSecondaryHeader(NULL, &_expected), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_GetHasSecondaryHeader(&cmd.Msg, NULL), CFE_MSG_BAD_ARGUMENT);

    cFE_FTAssert_SUCCESS(CFE_MSG_SetHasSecondaryHeader(&cmd.Msg, _expected));
    cFE_FTAssert_SUCCESS(CFE_MSG_GetHasSecondaryHeader(&cmd.Msg, &_returned));
    UtAssert_UINT32_EQ(_expected, _returned);

    /* test get-apid */
    UtAssert_INT32_EQ(CFE_MSG_GetApId(NULL, &appId), CFE_MSG_BAD_ARGUMENT);
    cFE_FTAssert_SUCCESS(CFE_MSG_GetApId(&cmd.Msg, &appId));
    UtAssert_INT32_EQ(CFE_MSG_GetApId(&cmd.Msg, NULL), CFE_MSG_BAD_ARGUMENT);

    /* test set-apid */
    UtAssert_INT32_EQ(CFE_MSG_GetApId(NULL, 0), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_SetApId(&cmd.Msg, 0xFFFF), CFE_MSG_BAD_ARGUMENT);

    cFE_FTAssert_SUCCESS(CFE_MSG_SetApId(&cmd.Msg, 0));
    cFE_FTAssert_SUCCESS(CFE_MSG_GetApId(&cmd.Msg, &appId));
    UtAssert_True(appId == 0, "CFE_MSG_SetApId => apid = 0");
}

void TestMsgApiAdvanced1(void)
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

    /* test msg-init */
    UtAssert_INT32_EQ(CFE_MSG_Init(NULL, CFE_SB_ValueToMsgId(0), sizeof(cmd)), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_Init(&cmd.Msg, msgId, 0), CFE_MSG_BAD_ARGUMENT);

    cFE_FTAssert_SUCCESS(CFE_MSG_Init(&cmd.Msg, msgId, sizeof(cmd)));

    /* test get/set-segmentation-flag */
    UtAssert_INT32_EQ(CFE_MSG_GetSegmentationFlag(NULL, &segFlag), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_GetSegmentationFlag(&cmd.Msg, NULL), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_SetSegmentationFlag(NULL, CFE_MSG_SegFlag_Continue), CFE_MSG_BAD_ARGUMENT);

    cFE_FTAssert_SUCCESS(CFE_MSG_SetSegmentationFlag(&cmd.Msg, CFE_MSG_SegFlag_Continue));
    cFE_FTAssert_SUCCESS(CFE_MSG_GetSegmentationFlag(&cmd.Msg, &segFlag));
    UtAssert_True(segFlag == CFE_MSG_SegFlag_Continue,
                  "CFE_MSG_SetSegmentationFlag => segFlag = CFE_MSG_SegFlag_Continue");

    /* test set/get-sequence-count */
    UtAssert_INT32_EQ(CFE_MSG_SetSequenceCount(NULL, 2), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_GetSequenceCount(NULL, &seqCnt), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_GetSequenceCount(&cmd.Msg, NULL), CFE_MSG_BAD_ARGUMENT);

    cFE_FTAssert_SUCCESS(CFE_MSG_SetSequenceCount(&cmd.Msg, 2));
    cFE_FTAssert_SUCCESS(CFE_MSG_GetSequenceCount(&cmd.Msg, &seqCnt));

    UtAssert_True(seqCnt == 2, "CFE_MSG_SetSequenceCount => seqCnt = 2");

    /* test get-next-sequence-count */

    /* 0x3FFF <- bad because defined CFE_MSG_SEQCNT_MASK value but not public */
    /* UtAssert_True(CFE_MSG_GetNextSequenceCount(0x3FFF) == 0, "CFE_MSG_GetNextSequenceCount(0x3FFF) = 0"); */
    UtAssert_True(CFE_MSG_GetNextSequenceCount(2) == 3, "CFE_MSG_GetNextSequenceCount(2) = 3");
}

void TestMsgApiAdvanced2(void)
{
    UtPrintf("Testing: CFE_MSG_GenerateChecksum, CFE_MSG_ValidateChecksum, CFE_MSG_SetFcnCode, "
             "CFE_MSG_GetFcnCode, CFE_MSG_GetMsgTime, CFE_MSG_SetMsgTime ");

    /* declare local vars */
    CFE_MSG_CommandHeader_t cmd;
    CFE_SB_MsgId_t          msgId;
    CFE_MSG_FcnCode_t       fcnCode;
    CFE_TIME_SysTime_t      msgTime;

    memset(&cmd, 0xFF, sizeof(cmd));
    msgId = CFE_SB_ValueToMsgId(0);

    /* msg-init */
    UtAssert_INT32_EQ(CFE_MSG_Init(NULL, CFE_SB_ValueToMsgId(0), sizeof(cmd)), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_Init(&cmd.Msg, msgId, 0), CFE_MSG_BAD_ARGUMENT);

    cFE_FTAssert_SUCCESS(CFE_MSG_Init(&cmd.Msg, msgId, sizeof(cmd)));
    cFE_FTAssert_SUCCESS(CFE_MSG_SetHasSecondaryHeader(&cmd.Msg, true));

    /* test generate-checksum */
    UtAssert_INT32_EQ(CFE_MSG_GenerateChecksum(NULL), CFE_MSG_BAD_ARGUMENT);
    cFE_FTAssert_SUCCESS(CFE_MSG_SetType(&cmd.Msg, CFE_MSG_Type_Tlm));
    UtAssert_INT32_EQ(CFE_MSG_GenerateChecksum(&cmd.Msg), CFE_MSG_WRONG_MSG_TYPE);
    cFE_FTAssert_SUCCESS(CFE_MSG_SetType(&cmd.Msg, CFE_MSG_Type_Cmd));

    /* create new cmd message */
    CFE_MSG_CommandHeader_t cmd2;
    bool                    isValid = true;

    memset(&cmd2, 0, sizeof(cmd2));
    cFE_FTAssert_SUCCESS(CFE_MSG_Init(&cmd2.Msg, CFE_SB_ValueToMsgId(1), sizeof(cmd2)));
    cFE_FTAssert_SUCCESS(CFE_MSG_SetHasSecondaryHeader(&cmd2.Msg, true));
    cFE_FTAssert_SUCCESS(CFE_MSG_SetType(&cmd2.Msg, CFE_MSG_Type_Cmd));

    cFE_FTAssert_SUCCESS(CFE_MSG_ValidateChecksum(&cmd2.Msg, &isValid));
    UtAssert_True(!isValid, "Is checksum valid = false");
    cFE_FTAssert_SUCCESS(CFE_MSG_GenerateChecksum(&cmd2.Msg));
    cFE_FTAssert_SUCCESS(CFE_MSG_ValidateChecksum(&cmd2.Msg, &isValid));
    UtAssert_True(isValid, "Checksum is valid");

    /* test get/set-fcn-code */
    UtAssert_INT32_EQ(CFE_MSG_SetFcnCode(NULL, 4), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_GetFcnCode(&cmd.Msg, NULL), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_GetFcnCode(NULL, &fcnCode), CFE_MSG_BAD_ARGUMENT);

    cFE_FTAssert_SUCCESS(CFE_MSG_SetFcnCode(&cmd.Msg, 4));
    cFE_FTAssert_SUCCESS(CFE_MSG_GetFcnCode(&cmd.Msg, &fcnCode));

    UtAssert_True(fcnCode == 4, "CFE_MSG_SetFcnCode => fcnCode = 4");

    /* test get/set-msg-time */
    CFE_TIME_SysTime_t currentTime = CFE_TIME_GetTime();
    cFE_FTAssert_SUCCESS(CFE_MSG_SetType(&cmd.Msg, CFE_MSG_Type_Tlm));

    UtAssert_INT32_EQ(CFE_MSG_GetMsgTime(NULL, &msgTime), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_GetMsgTime(&cmd.Msg, NULL), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_SetMsgTime(NULL, currentTime), CFE_MSG_BAD_ARGUMENT);

    cFE_FTAssert_SUCCESS(CFE_MSG_SetMsgTime(&cmd.Msg, currentTime));
    cFE_FTAssert_SUCCESS(CFE_MSG_GetMsgTime(&cmd.Msg, &msgTime));

    UtAssert_UINT32_EQ(CFE_TIME_Compare(msgTime, currentTime), CFE_TIME_A_LT_B);
}

void MsgApiTestSetup(void)
{
    UtTest_Add(TestMsgApiBasic, NULL, NULL, "Test basic message header apis");
    UtTest_Add(TestMsgApiAdvanced1, NULL, NULL, "Test advanced message header apis part 1");
    UtTest_Add(TestMsgApiAdvanced2, NULL, NULL, "Test advanced message header apis part 2");
}
