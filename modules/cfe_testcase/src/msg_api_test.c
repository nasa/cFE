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
 *   Functional test of cFE Message header APIs.
 */

/*
 * Includes
 */

#include "cfe_test.h"
#include "cfe_test_msgids.h"
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
    msgId = CFE_SB_ValueToMsgId(CFE_TEST_CMD_MID);

    /* test msg-init */
    UtAssert_INT32_EQ(CFE_MSG_Init(NULL, msgId, sizeof(cmd)), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_Init(CFE_MSG_PTR(cmd), CFE_SB_INVALID_MSG_ID, sizeof(cmd)), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_Init(CFE_MSG_PTR(cmd), msgId, 0), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(
        CFE_MSG_Init(CFE_MSG_PTR(cmd), CFE_SB_ValueToMsgId(CFE_PLATFORM_SB_HIGHEST_VALID_MSGID + 1), sizeof(cmd)),
        CFE_MSG_BAD_ARGUMENT);

    UtAssert_INT32_EQ(CFE_MSG_Init(CFE_MSG_PTR(cmd), msgId, sizeof(cmd)), CFE_SUCCESS);

    /* test set-msg-size */
    UtAssert_INT32_EQ(CFE_MSG_SetSize(NULL, 12), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_SetSize(CFE_MSG_PTR(cmd), 0), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_SetSize(CFE_MSG_PTR(cmd), UINT32_MAX), CFE_MSG_BAD_ARGUMENT);

    UtAssert_INT32_EQ(CFE_MSG_SetSize(CFE_MSG_PTR(cmd), 12), CFE_SUCCESS);

    /* test get-msg-size */
    UtAssert_INT32_EQ(CFE_MSG_GetSize(NULL, &size), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_GetSize(CFE_MSG_PTR(cmd), NULL), CFE_MSG_BAD_ARGUMENT);

    UtAssert_INT32_EQ(CFE_MSG_GetSize(CFE_MSG_PTR(cmd), &size), CFE_SUCCESS);
    UtAssert_UINT32_EQ(size, 12);

    /* test get-type */
    UtAssert_INT32_EQ(CFE_MSG_SetType(NULL, CFE_MSG_Type_Cmd), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_SetType(CFE_MSG_PTR(cmd), CFE_MSG_Type_Invalid), CFE_MSG_BAD_ARGUMENT);

    UtAssert_INT32_EQ(CFE_MSG_SetType(CFE_MSG_PTR(cmd), CFE_MSG_Type_Cmd), CFE_SUCCESS);

    UtAssert_INT32_EQ(CFE_MSG_GetType(NULL, &type), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_GetType(CFE_MSG_PTR(cmd), NULL), CFE_MSG_BAD_ARGUMENT);

    UtAssert_INT32_EQ(CFE_MSG_GetType(CFE_MSG_PTR(cmd), &type), CFE_SUCCESS);
    UtAssert_INT32_EQ(type, CFE_MSG_Type_Cmd);

    /* test msg set-type */
    UtAssert_INT32_EQ(CFE_MSG_SetType(CFE_MSG_PTR(cmd), CFE_MSG_Type_Tlm), CFE_SUCCESS);
    /* check if set-type works like expected */
    UtAssert_INT32_EQ(CFE_MSG_GetType(CFE_MSG_PTR(cmd), &type), CFE_SUCCESS);
    UtAssert_INT32_EQ(type, CFE_MSG_Type_Tlm);

    /* test get header-version */
    UtAssert_INT32_EQ(CFE_MSG_GetHeaderVersion(NULL, &hdrVer), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_GetHeaderVersion(CFE_MSG_PTR(cmd), &hdrVer), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_GetHeaderVersion(CFE_MSG_PTR(cmd), NULL), CFE_MSG_BAD_ARGUMENT);

    /* test set header-version */
    UtAssert_INT32_EQ(CFE_MSG_SetHeaderVersion(NULL, hdrVer), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_SetHeaderVersion(CFE_MSG_PTR(cmd), UINT16_MAX), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_SetHeaderVersion(CFE_MSG_PTR(cmd), 0), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_GetHeaderVersion(CFE_MSG_PTR(cmd), &hdrVer), CFE_SUCCESS);
    UtAssert_True(hdrVer == 0, "hdrVer = 0");

    /* test get-has-secondary-header and set-has-secondary-header*/
    UtAssert_INT32_EQ(CFE_MSG_GetHasSecondaryHeader(NULL, &_expected), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_GetHasSecondaryHeader(CFE_MSG_PTR(cmd), NULL), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_SetHasSecondaryHeader(NULL, _expected), CFE_MSG_BAD_ARGUMENT);

    UtAssert_INT32_EQ(CFE_MSG_SetHasSecondaryHeader(CFE_MSG_PTR(cmd), _expected), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_GetHasSecondaryHeader(CFE_MSG_PTR(cmd), &_returned), CFE_SUCCESS);
    UtAssert_UINT32_EQ(_expected, _returned);

    /* test get-apid */
    UtAssert_INT32_EQ(CFE_MSG_GetApId(NULL, &appId), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_GetApId(CFE_MSG_PTR(cmd), NULL), CFE_MSG_BAD_ARGUMENT);

    /* test set-apid */
    UtAssert_INT32_EQ(CFE_MSG_SetApId(CFE_MSG_PTR(cmd), 0xFFFF), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_SetApId(NULL, 0), CFE_MSG_BAD_ARGUMENT);

    UtAssert_INT32_EQ(CFE_MSG_SetApId(CFE_MSG_PTR(cmd), 5), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_GetApId(CFE_MSG_PTR(cmd), &appId), CFE_SUCCESS);
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
    msgId = CFE_SB_ValueToMsgId(CFE_TEST_CMD_MID);

    UtAssert_INT32_EQ(CFE_MSG_Init(CFE_MSG_PTR(cmd), msgId, sizeof(cmd)), CFE_SUCCESS);

    /* test get/set-segmentation-flag */
    UtAssert_INT32_EQ(CFE_MSG_GetSegmentationFlag(NULL, &segFlag), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_GetSegmentationFlag(CFE_MSG_PTR(cmd), NULL), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_SetSegmentationFlag(NULL, CFE_MSG_SegFlag_Continue), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_SetSegmentationFlag(CFE_MSG_PTR(cmd), CFE_MSG_SegFlag_Invalid), CFE_MSG_BAD_ARGUMENT);

    UtAssert_INT32_EQ(CFE_MSG_SetSegmentationFlag(CFE_MSG_PTR(cmd), CFE_MSG_SegFlag_Continue), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_SetSegmentationFlag(CFE_MSG_PTR(cmd), CFE_MSG_SegFlag_First), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_SetSegmentationFlag(CFE_MSG_PTR(cmd), CFE_MSG_SegFlag_Last), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_SetSegmentationFlag(CFE_MSG_PTR(cmd), CFE_MSG_SegFlag_Unsegmented), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_GetSegmentationFlag(CFE_MSG_PTR(cmd), &segFlag), CFE_SUCCESS);
    UtAssert_UINT32_EQ(segFlag, CFE_MSG_SegFlag_Unsegmented);

    /* test set/get-sequence-count */
    UtAssert_INT32_EQ(CFE_MSG_SetSequenceCount(NULL, 2), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_SetSequenceCount(CFE_MSG_PTR(cmd), UINT16_MAX), CFE_MSG_BAD_ARGUMENT);

    UtAssert_INT32_EQ(CFE_MSG_GetSequenceCount(NULL, &seqCnt), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_GetSequenceCount(CFE_MSG_PTR(cmd), NULL), CFE_MSG_BAD_ARGUMENT);

    UtAssert_INT32_EQ(CFE_MSG_SetSequenceCount(CFE_MSG_PTR(cmd), 2), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_GetSequenceCount(CFE_MSG_PTR(cmd), &seqCnt), CFE_SUCCESS);
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
    CFE_MSG_CommandHeader_t   cmd;
    CFE_MSG_TelemetryHeader_t tlm;
    CFE_MSG_FcnCode_t         fcnCode;
    CFE_TIME_SysTime_t        msgTime;
    bool                      isValid     = true;
    CFE_TIME_SysTime_t        currentTime = {1000, 0xFFFF0000};

    memset(&cmd, 0, sizeof(cmd));
    memset(&tlm, 0, sizeof(tlm));

    /* msg-init */
    UtAssert_INT32_EQ(CFE_MSG_Init(CFE_MSG_PTR(cmd), CFE_SB_ValueToMsgId(1), sizeof(cmd)), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_SetHasSecondaryHeader(CFE_MSG_PTR(cmd), true), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_SetType(CFE_MSG_PTR(cmd), CFE_MSG_Type_Cmd), CFE_SUCCESS);

    UtAssert_INT32_EQ(CFE_MSG_Init(CFE_MSG_PTR(tlm), CFE_SB_ValueToMsgId(2), sizeof(tlm)), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_SetHasSecondaryHeader(CFE_MSG_PTR(tlm), true), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_SetType(CFE_MSG_PTR(tlm), CFE_MSG_Type_Tlm), CFE_SUCCESS);

    /* test generate-checksum on commands */
    CFE_Assert_STATUS_STORE(CFE_MSG_ValidateChecksum(CFE_MSG_PTR(cmd), &isValid));
    if (CFE_Assert_STATUS_MAY_BE(CFE_SUCCESS))
    {
        UtAssert_BOOL_FALSE(isValid); /* assuming a zero'ed out struct will have an invalid checksum */

        UtAssert_INT32_EQ(CFE_MSG_GenerateChecksum(CFE_MSG_PTR(cmd)), CFE_SUCCESS);
        UtAssert_INT32_EQ(CFE_MSG_ValidateChecksum(CFE_MSG_PTR(cmd), &isValid), CFE_SUCCESS);
        UtAssert_BOOL_TRUE(isValid);
    }
    else if (CFE_Assert_STATUS_MAY_BE(CFE_MSG_WRONG_MSG_TYPE))
    {
        UtAssert_NA("CFE_MSG_GenerateChecksum does not operate on commands");
    }
    else
    {
        UtAssert_NA("CFE_MSG_GenerateChecksum on commands not implemented");
        CFE_Assert_STATUS_MUST_BE(CFE_MSG_NOT_IMPLEMENTED);
    }

    /* test generate-checksum on telemetry */
    CFE_Assert_STATUS_STORE(CFE_MSG_ValidateChecksum(CFE_MSG_PTR(tlm), &isValid));
    if (CFE_Assert_STATUS_MAY_BE(CFE_SUCCESS))
    {
        UtAssert_BOOL_FALSE(isValid); /* assuming a zero'ed out struct will have an invalid checksum */

        UtAssert_INT32_EQ(CFE_MSG_GenerateChecksum(CFE_MSG_PTR(tlm)), CFE_SUCCESS);
        UtAssert_INT32_EQ(CFE_MSG_ValidateChecksum(CFE_MSG_PTR(tlm), &isValid), CFE_SUCCESS);
        UtAssert_BOOL_TRUE(isValid);
    }
    else if (CFE_Assert_STATUS_MAY_BE(CFE_MSG_WRONG_MSG_TYPE))
    {
        UtAssert_NA("CFE_MSG_GenerateChecksum does not operate on telemetry");
    }
    else
    {
        UtAssert_NA("CFE_MSG_GenerateChecksum on telemetry not implemented");
        CFE_Assert_STATUS_MUST_BE(CFE_MSG_NOT_IMPLEMENTED);
    }

    /* test get/set-fcn-code */
    UtAssert_INT32_EQ(CFE_MSG_SetFcnCode(NULL, 4), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_SetFcnCode(CFE_MSG_PTR(tlm), 4), CFE_MSG_WRONG_MSG_TYPE);

    UtAssert_INT32_EQ(CFE_MSG_GetFcnCode(CFE_MSG_PTR(cmd), NULL), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_GetFcnCode(NULL, &fcnCode), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_GetFcnCode(CFE_MSG_PTR(tlm), &fcnCode), CFE_MSG_WRONG_MSG_TYPE);

    UtAssert_INT32_EQ(CFE_MSG_SetFcnCode(CFE_MSG_PTR(cmd), 4), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_GetFcnCode(CFE_MSG_PTR(cmd), &fcnCode), CFE_SUCCESS);
    UtAssert_INT32_EQ(fcnCode, 4);

    /* test get/set-msg-time */
    UtAssert_INT32_EQ(CFE_MSG_GetMsgTime(NULL, &msgTime), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_GetMsgTime(CFE_MSG_PTR(tlm), NULL), CFE_MSG_BAD_ARGUMENT);

    /* Check if GetMsgTime is implemented on commands */
    CFE_Assert_STATUS_STORE(CFE_MSG_GetMsgTime(CFE_MSG_PTR(cmd), &msgTime));
    if (CFE_Assert_STATUS_MAY_BE(CFE_SUCCESS))
    {
        memset(&msgTime, 0xff, sizeof(msgTime));
        UtAssert_INT32_EQ(CFE_MSG_SetMsgTime(CFE_MSG_PTR(cmd), currentTime), CFE_SUCCESS);
        UtAssert_INT32_EQ(CFE_MSG_GetMsgTime(CFE_MSG_PTR(cmd), &msgTime), CFE_SUCCESS);
        UtAssert_UINT32_EQ(CFE_TIME_Compare(msgTime, currentTime), CFE_TIME_EQUAL);
    }
    else if (CFE_Assert_STATUS_MAY_BE(CFE_MSG_WRONG_MSG_TYPE))
    {
        UtAssert_NA("CFE_MSG_GetMsgTime does not operate on commands");
    }
    else
    {
        UtAssert_NA("CFE_MSG_GetMsgTime on commands not implemented");
        CFE_Assert_STATUS_MUST_BE(CFE_MSG_NOT_IMPLEMENTED);
    }

    /* Check if GetMsgTime is implemented on telemetry */
    CFE_Assert_STATUS_STORE(CFE_MSG_GetMsgTime(CFE_MSG_PTR(tlm), &msgTime));
    if (CFE_Assert_STATUS_MAY_BE(CFE_SUCCESS))
    {
        memset(&msgTime, 0xff, sizeof(msgTime));
        UtAssert_INT32_EQ(CFE_MSG_SetMsgTime(CFE_MSG_PTR(tlm), currentTime), CFE_SUCCESS);
        UtAssert_INT32_EQ(CFE_MSG_GetMsgTime(CFE_MSG_PTR(tlm), &msgTime), CFE_SUCCESS);
        UtAssert_UINT32_EQ(CFE_TIME_Compare(msgTime, currentTime), CFE_TIME_EQUAL);
    }
    else if (CFE_Assert_STATUS_MAY_BE(CFE_MSG_WRONG_MSG_TYPE))
    {
        UtAssert_NA("CFE_MSG_GetMsgTime does not operate on telemetry");
    }
    else
    {
        UtAssert_NA("CFE_MSG_GetMsgTime on telemetry not implemented");
        CFE_Assert_STATUS_MUST_BE(CFE_MSG_NOT_IMPLEMENTED);
    }
}

void MsgApiTestSetup(void)
{
    UtTest_Add(TestMsgApiBasic, NULL, NULL, "Test basic message header apis");
    UtTest_Add(TestMsgApiAdvanced, NULL, NULL, "Test advanced message header apis");
    UtTest_Add(TestMsgHeaderSecondaryApi, NULL, NULL, "Test message secondary header apis");
}
