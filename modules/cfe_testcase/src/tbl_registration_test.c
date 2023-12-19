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
 *   Functional test of Table Registration APIs
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

int32 CallbackFunc(void *TblPtr)
{
    return 1;
}

void TestTableRegistration(void)
{
    char             BadTblName[CFE_TBL_MAX_FULL_NAME_LEN + 2];
    CFE_TBL_Handle_t OtherHandle = CFE_TBL_BAD_TABLE_HANDLE;

    UtPrintf("Testing: CFE_TBL_Register, CFE_TBL_Unregister");

    BadTblName[CFE_TBL_MAX_FULL_NAME_LEN + 1] = '\0';
    memset(BadTblName, 'a', sizeof(BadTblName) - 1);

    /* invalid table handle arg */
    UtAssert_INT32_EQ(
        CFE_TBL_Register(NULL, CFE_FT_Global.TblName, sizeof(CFE_TEST_TestTable_t), CFE_TBL_OPT_DEFAULT, &CallbackFunc),
        CFE_TBL_BAD_ARGUMENT);

    /* Successfully create table */
    UtAssert_INT32_EQ(CFE_TBL_Register(&CFE_FT_Global.TblHandle, CFE_FT_Global.TblName, sizeof(CFE_TEST_TestTable_t),
                                       CFE_TBL_OPT_DEFAULT, &CallbackFunc),
                      CFE_SUCCESS);

    /* Duplicate table (should return the same handle) */
    UtAssert_INT32_EQ(CFE_TBL_Register(&OtherHandle, CFE_FT_Global.TblName, sizeof(CFE_TEST_TestTable_t),
                                       CFE_TBL_OPT_DEFAULT, &CallbackFunc),
                      CFE_TBL_WARN_DUPLICATE);

    UtAssert_INT32_EQ(OtherHandle, CFE_FT_Global.TblHandle);

    /* Duplicate table with different size */
    UtAssert_INT32_EQ(CFE_TBL_Register(&OtherHandle, CFE_FT_Global.TblName, sizeof(CFE_TEST_TestTable_t) / 2,
                                       CFE_TBL_OPT_DEFAULT, &CallbackFunc),
                      CFE_TBL_ERR_DUPLICATE_DIFF_SIZE);

    /* Unregister the table */
    UtAssert_INT32_EQ(CFE_TBL_Unregister(CFE_TBL_BAD_TABLE_HANDLE), CFE_TBL_ERR_INVALID_HANDLE);
    UtAssert_INT32_EQ(CFE_TBL_Unregister(CFE_FT_Global.TblHandle), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_TBL_Unregister(CFE_FT_Global.TblHandle), CFE_TBL_ERR_INVALID_HANDLE);

    /* Invalid Name */
    UtAssert_INT32_EQ(
        CFE_TBL_Register(&CFE_FT_Global.TblHandle, BadTblName, sizeof(CFE_TEST_TestTable_t), CFE_TBL_OPT_DEFAULT, NULL),
        CFE_TBL_ERR_INVALID_NAME);
    UtAssert_INT32_EQ(
        CFE_TBL_Register(&CFE_FT_Global.TblHandle, "", sizeof(CFE_TEST_TestTable_t), CFE_TBL_OPT_DEFAULT, NULL),
        CFE_TBL_ERR_INVALID_NAME);

    /* Invalid Table Size */
    UtAssert_INT32_EQ(CFE_TBL_Register(&CFE_FT_Global.TblHandle, CFE_FT_Global.TblName, 0, CFE_TBL_OPT_DEFAULT, NULL),
                      CFE_TBL_ERR_INVALID_SIZE);
    UtAssert_INT32_EQ(CFE_TBL_Register(&CFE_FT_Global.TblHandle, CFE_FT_Global.TblName,
                                       CFE_PLATFORM_TBL_MAX_SNGL_TABLE_SIZE + 1, CFE_TBL_OPT_SNGL_BUFFER, NULL),
                      CFE_TBL_ERR_INVALID_SIZE);
    UtAssert_INT32_EQ(CFE_TBL_Register(&CFE_FT_Global.TblHandle, CFE_FT_Global.TblName,
                                       CFE_PLATFORM_TBL_MAX_DBL_TABLE_SIZE + 1, CFE_TBL_OPT_DBL_BUFFER, NULL),
                      CFE_TBL_ERR_INVALID_SIZE);

    /* Invalid Table Options */
    UtAssert_INT32_EQ(CFE_TBL_Register(&CFE_FT_Global.TblHandle, CFE_FT_Global.TblName, sizeof(CFE_TEST_TestTable_t),
                                       CFE_TBL_OPT_DBL_BUFFER | CFE_TBL_OPT_USR_DEF_ADDR, NULL),
                      CFE_TBL_ERR_INVALID_OPTIONS);
    UtAssert_INT32_EQ(CFE_TBL_Register(&CFE_FT_Global.TblHandle, CFE_FT_Global.TblName, sizeof(CFE_TEST_TestTable_t),
                                       CFE_TBL_OPT_CRITICAL | CFE_TBL_OPT_DUMP_ONLY, NULL),
                      CFE_TBL_ERR_INVALID_OPTIONS);
    UtAssert_INT32_EQ(CFE_TBL_Register(&CFE_FT_Global.TblHandle, CFE_FT_Global.TblName, sizeof(CFE_TEST_TestTable_t),
                                       CFE_TBL_OPT_CRITICAL | CFE_TBL_OPT_USR_DEF_ADDR, NULL),
                      CFE_TBL_ERR_INVALID_OPTIONS);
}

void TestTableMaxLimits(void)
{
    CFE_TBL_Handle_t Handles[CFE_PLATFORM_TBL_MAX_NUM_HANDLES + 2];
    char             TblName[CFE_TBL_MAX_FULL_NAME_LEN];
    uint32           numTblsCreated = 0; /* Track num created to unregister them all */

    /*
     * Create the maximum number of tables
     * There are already some tables in this system, so this will
     * stop succeeding before it reaches the end of the loop
     * Check that after the loop no more tables can be created
     */
    while (numTblsCreated <= CFE_PLATFORM_TBL_MAX_NUM_HANDLES)
    {
        snprintf(TblName, sizeof(TblName), "Tbl%u", (unsigned int)numTblsCreated);
        CFE_Assert_STATUS_STORE(CFE_TBL_Register(&Handles[numTblsCreated], TblName, sizeof(CFE_TEST_TestTable_t),
                                                 CFE_TBL_OPT_DEFAULT, NULL));
        if (CFE_Assert_STATUS_MAY_BE(CFE_TBL_ERR_REGISTRY_FULL))
        {
            break;
        }
        if (!CFE_Assert_STATUS_MUST_BE(CFE_SUCCESS))
        {
            break;
        }
        ++numTblsCreated;
    }

    if (!UtAssert_NONZERO(numTblsCreated))
    {
        UtAssert_WARN("Table test cannot create any tables");
        return;
    }

    UtAssert_UINT32_LT(numTblsCreated, CFE_PLATFORM_TBL_MAX_NUM_TABLES);
    UtAssert_UINT32_LT(numTblsCreated, CFE_PLATFORM_TBL_MAX_NUM_HANDLES);

    /* Delete one table so the registry isn't full anymore */
    --numTblsCreated;
    UtAssert_INT32_EQ(CFE_TBL_Unregister(Handles[numTblsCreated]), CFE_SUCCESS);

    if (!UtAssert_NONZERO(numTblsCreated))
    {
        UtAssert_WARN("Table test cannot run CFE_TBL_Share max without at least one table");
        return;
    }

    /*
     * A shared table has a unique handle but not a unique entry in the registry.
     * By calling CFE_TBL_Share it should consume handles but not registry entries
     */
    snprintf(TblName, sizeof(TblName), "CFE_TEST_APP.Tbl%u", (unsigned int)0);
    while (numTblsCreated <= CFE_PLATFORM_TBL_MAX_NUM_HANDLES)
    {
        CFE_Assert_STATUS_STORE(CFE_TBL_Share(&Handles[numTblsCreated], TblName));
        if (CFE_Assert_STATUS_MAY_BE(CFE_TBL_ERR_HANDLES_FULL))
        {
            break;
        }
        if (!CFE_Assert_STATUS_MUST_BE(CFE_SUCCESS))
        {
            break;
        }
        ++numTblsCreated;
    }

    UtAssert_UINT32_LT(numTblsCreated, CFE_PLATFORM_TBL_MAX_NUM_HANDLES);

    /* also confirm not able to register a new table, either */
    snprintf(TblName, sizeof(TblName), "Tbl%u", (unsigned int)numTblsCreated);
    UtAssert_INT32_EQ(
        CFE_TBL_Register(&Handles[numTblsCreated], TblName, sizeof(CFE_TEST_TestTable_t), CFE_TBL_OPT_DEFAULT, NULL),
        CFE_TBL_ERR_HANDLES_FULL);

    /* Unregister all table handles */
    while (numTblsCreated > 0)
    {
        --numTblsCreated;
        UtAssert_INT32_EQ(CFE_TBL_Unregister(Handles[numTblsCreated]), CFE_SUCCESS);
    }
}

void TestTableShare(void)
{
    UtPrintf("Testing: CFE_TBL_Share");
    CFE_TBL_Handle_t SharedTblHandle;
    const char *     SharedTblName = CFE_ASSERT_SHARED_TBL_NAME;
    const char *     BadTblName    = "ExampleTable";

    UtAssert_INT32_EQ(CFE_TBL_Share(NULL, SharedTblName), CFE_TBL_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_TBL_Share(&SharedTblHandle, SharedTblName), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_TBL_Share(&SharedTblHandle, NULL), CFE_TBL_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_TBL_Share(&SharedTblHandle, BadTblName), CFE_TBL_ERR_INVALID_NAME);
}

void TestTblNonAppContext(void)
{
    CFE_TBL_Handle_t Handle;
    void *           TblPtr;

    /* Attempt to register another table */
    UtAssert_INT32_EQ(
        CFE_TBL_Register(&Handle, "OtherTable", sizeof(CFE_TEST_TestTable_t), CFE_TBL_OPT_DEFAULT, &CallbackFunc),
        CFE_ES_ERR_RESOURCEID_NOT_VALID);

    /* Calling any other API (with a valid handle) should be rejected from this context */
    UtAssert_INT32_EQ(CFE_TBL_DumpToBuffer(CFE_FT_Global.TblHandle), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_TBL_GetAddress(&TblPtr, CFE_FT_Global.TblHandle), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_TBL_GetStatus(CFE_FT_Global.TblHandle), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_TBL_Load(CFE_FT_Global.TblHandle, CFE_TBL_SRC_FILE, "/cf/cfe_test_tbl.tbl"),
                      CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_TBL_Manage(CFE_FT_Global.TblHandle), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_TBL_Modified(CFE_FT_Global.TblHandle), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_TBL_NotifyByMessage(CFE_FT_Global.TblHandle, CFE_SB_ValueToMsgId(CFE_TEST_CMD_MID), 0, 0),
                      CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_TBL_ReleaseAddress(CFE_FT_Global.TblHandle), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_TBL_Share(&Handle, CFE_FT_Global.TblName), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_TBL_Update(CFE_FT_Global.TblHandle), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_TBL_Validate(CFE_FT_Global.TblHandle), CFE_ES_ERR_RESOURCEID_NOT_VALID);

    /* Attempt to unregister a table */
    UtAssert_INT32_EQ(CFE_TBL_Unregister(CFE_FT_Global.TblHandle), CFE_ES_ERR_RESOURCEID_NOT_VALID);
}

void TestTableBadContext(void)
{
    uint32         RetryCount;
    osal_id_t      OtherTaskId = OS_OBJECT_ID_UNDEFINED;
    OS_task_prop_t TaskProp;

    /* Create one (good) handle first from this task */
    UtAssert_INT32_EQ(CFE_TBL_Register(&CFE_FT_Global.TblHandle, CFE_FT_Global.TblName, sizeof(CFE_TEST_TestTable_t),
                                       CFE_TBL_OPT_DEFAULT, &CallbackFunc),
                      CFE_SUCCESS);

    /* Create a separate task to run the tests, to confirm TBL context checks */
    UtAssert_INT32_EQ(OS_TaskCreate(&OtherTaskId, "NonCfe", TestTblNonAppContext, OSAL_TASK_STACK_ALLOCATE, 16384,
                                    OSAL_PRIORITY_C(200), 0),
                      OS_SUCCESS);

    /* wait for task to exit itself */
    RetryCount = 0;
    while (RetryCount < 20)
    {
        /*
         * poll until OS_TaskGetInfo() returns an error, then the task has exited
         */
        if (OS_TaskGetInfo(OtherTaskId, &TaskProp) != OS_SUCCESS)
        {
            break;
        }

        OS_TaskDelay(100);
        ++RetryCount;
    }

    UtAssert_UINT32_LT(RetryCount, 20);
    UtAssert_INT32_EQ(CFE_TBL_Unregister(CFE_FT_Global.TblHandle), CFE_SUCCESS);
}

void TBLRegistrationTestSetup(void)
{
    UtTest_Add(TestTableRegistration, NULL, NULL, "Test Table Registration");
    UtTest_Add(TestTableMaxLimits, NULL, NULL, "Table Max Limits");
    UtTest_Add(TestTableShare, NULL, NULL, "Test Table Sharing");
    UtTest_Add(TestTableBadContext, NULL, NULL, "Test Table Bad Context");
}
