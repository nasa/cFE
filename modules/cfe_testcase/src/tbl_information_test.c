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
 *   Functional test of Table Information APIs
 *
 *   Demonstration of how to register and use the UT assert functions.
 */

/*
 * Includes
 */

#include "cfe_test.h"
#include "cfe_test_table.h"
#include "cfe_msgids.h"
#include "cfe_test_msgids.h"

void TestGetStatus(void)
{
    UtPrintf("Testing: CFE_TBL_GetStatus");
    /*
     * This assert assumes there are no pending actions for this table
     * Since manage has never been called, I think this is a safe assumption
     */
    UtAssert_INT32_EQ(CFE_TBL_GetStatus(CFE_FT_Global.TblHandle), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_TBL_GetStatus(CFE_TBL_BAD_TABLE_HANDLE), CFE_TBL_ERR_INVALID_HANDLE);
}

void TestGetInfo(void)
{
    CFE_TBL_Info_t TblInfo;
    const char *   BadTblName = "BadTable";

    memset(&TblInfo, 0, sizeof(TblInfo));

    UtPrintf("Testing: CFE_TBL_GetInfo");

    UtAssert_INT32_EQ(CFE_TBL_GetInfo(&TblInfo, CFE_FT_Global.RegisteredTblName), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_TBL_GetInfo(NULL, CFE_FT_Global.TblName), CFE_TBL_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_TBL_GetInfo(&TblInfo, BadTblName), CFE_TBL_ERR_INVALID_NAME);
    UtAssert_INT32_EQ(CFE_TBL_GetInfo(&TblInfo, NULL), CFE_TBL_BAD_ARGUMENT);

    /* This is only checking some parts of the TblInfo struct */
    size_t expectedSize        = sizeof(CFE_TEST_TestTable_t);
    uint32 expectedNumUsers    = 1;
    bool   expectedTableLoaded = false;
    bool   expectedDumpOnly    = false;
    bool   expectedDoubleBuf   = false;
    bool   expectedUserDefAddr = false;
    bool   expectedCritical    = false;
    UtAssert_UINT32_EQ(TblInfo.Size, expectedSize);
    UtAssert_UINT32_EQ(TblInfo.NumUsers, expectedNumUsers);
    UtAssert_INT32_EQ(TblInfo.TableLoadedOnce, expectedTableLoaded);
    UtAssert_INT32_EQ(TblInfo.DumpOnly, expectedDumpOnly);
    UtAssert_INT32_EQ(TblInfo.DoubleBuffered, expectedDoubleBuf);
    UtAssert_INT32_EQ(TblInfo.UserDefAddr, expectedUserDefAddr);
    UtAssert_INT32_EQ(TblInfo.Critical, expectedCritical);
}

void TestNotifyByMessage(void)
{
    CFE_TBL_Handle_t  SharedTblHandle = CFE_TBL_BAD_TABLE_HANDLE;
    const char *      SharedTblName   = CFE_ASSERT_SHARED_TBL_NAME;
    CFE_SB_MsgId_t    TestMsgId       = CFE_SB_ValueToMsgId(CFE_TEST_CMD_MID);
    CFE_MSG_FcnCode_t TestCmdCode     = 0;
    uint32            TestParameter   = 0;

    UtPrintf("Testing: CFE_TBL_NotifyByMessage");

    UtAssert_INT32_EQ(CFE_TBL_NotifyByMessage(CFE_TBL_BAD_TABLE_HANDLE, TestMsgId, TestCmdCode, TestParameter),
                      CFE_TBL_ERR_INVALID_HANDLE);

    UtAssert_INT32_EQ(CFE_TBL_NotifyByMessage(CFE_FT_Global.TblHandle, TestMsgId, TestCmdCode, TestParameter),
                      CFE_SUCCESS);

    /* Attempt on table not owned by this app */
    UtAssert_INT32_EQ(CFE_TBL_Share(&SharedTblHandle, SharedTblName), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_TBL_NotifyByMessage(SharedTblHandle, TestMsgId, TestCmdCode, TestParameter),
                      CFE_TBL_ERR_NO_ACCESS);
}

void TBLInformationTestSetup(void)
{
    UtTest_Add(TestGetStatus, RegisterTestTable, UnregisterTestTable, "Test Table Get Status");
    UtTest_Add(TestGetInfo, RegisterTestTable, UnregisterTestTable, "Test Table Get Info");
    UtTest_Add(TestNotifyByMessage, RegisterTestTable, UnregisterTestTable, "Test Table Notify by Message");
}
