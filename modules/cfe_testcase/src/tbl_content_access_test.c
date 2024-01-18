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
 *   Functional test of Table Access Content APIs
 *
 *   Demonstration of how to register and use the UT assert functions.
 */

/*
 * Includes
 */

#include "cfe_test.h"
#include "cfe_test_table.h"

/*
 * Helper function to attempt to load the test table with data and assert with the provided CFE_Status_t
 */
void LoadTable(CFE_TEST_TestTable_t *TestTable, CFE_Status_t ExpectedStatus)
{
    UtAssert_INT32_EQ(CFE_TBL_Load(CFE_FT_Global.TblHandle, CFE_TBL_SRC_ADDRESS, TestTable), ExpectedStatus);
}

void TestGetAddress(void)
{
    void *                TblPtr;
    CFE_TEST_TestTable_t *TestTblPtr;
    CFE_TEST_TestTable_t  TestTable = {1, 2};

    CFE_TBL_Handle_t SharedTblHandle = CFE_TBL_BAD_TABLE_HANDLE;
    const char *     SharedTblName   = CFE_ASSERT_SHARED_TBL_NAME;

    UtPrintf("Testing: CFE_TBL_GetAddress");

    /* Never loaded */
    UtAssert_INT32_EQ(CFE_TBL_GetAddress(&TblPtr, CFE_FT_Global.TblHandle), CFE_TBL_ERR_NEVER_LOADED);
    UtAssert_INT32_EQ(CFE_TBL_GetAddress(&TblPtr, CFE_TBL_BAD_TABLE_HANDLE), CFE_TBL_ERR_INVALID_HANDLE);
    UtAssert_INT32_EQ(CFE_TBL_GetAddress(NULL, CFE_FT_Global.TblHandle), CFE_TBL_BAD_ARGUMENT);

    /* Returns CFE_TBL_INFO_UPDATED since it was just loaded */
    LoadTable(&TestTable, CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_TBL_GetAddress(&TblPtr, CFE_FT_Global.TblHandle), CFE_TBL_INFO_UPDATED);
    UtAssert_INT32_EQ(CFE_TBL_GetAddress(&TblPtr, CFE_FT_Global.TblHandle), CFE_SUCCESS);

    /* Check table contents */
    TestTblPtr = (CFE_TEST_TestTable_t *)TblPtr;
    UtAssert_INT32_EQ(TestTblPtr->Int1, TestTable.Int1);
    UtAssert_INT32_EQ(TestTblPtr->Int2, TestTable.Int2);

    /* Unregistered table */
    UnregisterTestTable();
    UtAssert_INT32_EQ(CFE_TBL_GetAddress(&TblPtr, CFE_FT_Global.TblHandle), CFE_TBL_ERR_INVALID_HANDLE);

    /* Access a shared table */
    UtAssert_INT32_EQ(CFE_TBL_Share(&SharedTblHandle, SharedTblName), CFE_SUCCESS);
    /* Returns CFE_TBL_INFO_UPDATED since it hasn't been touched since it was loaded */
    UtAssert_INT32_EQ(CFE_TBL_GetAddress(&TblPtr, SharedTblHandle), CFE_TBL_INFO_UPDATED);
    UtAssert_INT32_EQ(CFE_TBL_Unregister(SharedTblHandle), CFE_SUCCESS);
}

void TestReleaseAddress(void)
{
    UtPrintf("Testing: CFE_TBL_GetAddress");
    void *               TblPtr;
    CFE_TEST_TestTable_t TestTable = {1, 2};
    /* Never loaded */
    UtAssert_INT32_EQ(CFE_TBL_ReleaseAddress(CFE_FT_Global.TblHandle), CFE_TBL_ERR_NEVER_LOADED);
    UtAssert_INT32_EQ(CFE_TBL_ReleaseAddress(CFE_TBL_BAD_TABLE_HANDLE), CFE_TBL_ERR_INVALID_HANDLE);
    /* Successful load and release */
    LoadTable(&TestTable, CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_TBL_ReleaseAddress(CFE_FT_Global.TblHandle), CFE_TBL_INFO_UPDATED);

    /* Attempt to load while address is locked */
    LoadTable(&TestTable, CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_TBL_GetAddress(&TblPtr, CFE_FT_Global.TblHandle), CFE_TBL_INFO_UPDATED);
    LoadTable(&TestTable, CFE_TBL_INFO_TABLE_LOCKED);

    /* Release and try again */
    UtAssert_INT32_EQ(CFE_TBL_ReleaseAddress(CFE_FT_Global.TblHandle), CFE_SUCCESS);
    /* It is necessary to call CFE_TBL_Manage because the table still thinks there is a load in progress from the failed
    load while the table was locked. This call shouldn't be necessary. */
    UtAssert_INT32_EQ(CFE_TBL_Manage(CFE_FT_Global.TblHandle), CFE_TBL_INFO_UPDATED);
    LoadTable(&TestTable, CFE_SUCCESS);

    /* Attempt to release an unregistered table */
    UnregisterTestTable();
    UtAssert_INT32_EQ(CFE_TBL_ReleaseAddress(CFE_FT_Global.TblHandle), CFE_TBL_ERR_INVALID_HANDLE);
}

void TestGetReleaseAddresses(void)
{
    int                  numValidTbls = 5;
    char                 TblName[10];
    CFE_TBL_Handle_t     TblHandles[numValidTbls + 1];
    void *               TblPtrs[numValidTbls + 1];
    CFE_TEST_TestTable_t TblPtrsList[numValidTbls + 1];
    CFE_TEST_TestTable_t TestTable = {1, 2};

    /* Put an invalid handle at the start*/
    TblHandles[0] = CFE_TBL_BAD_TABLE_HANDLE;
    TblPtrs[0]    = TblPtrsList;
    for (int i = 1; i < numValidTbls + 1; i++)
    {
        sprintf(TblName, "%d", i);
        UtAssert_INT32_EQ(
            CFE_TBL_Register(&TblHandles[i], TblName, sizeof(CFE_TEST_TestTable_t), CFE_TBL_OPT_DEFAULT, NULL),
            CFE_SUCCESS);
        TblPtrs[i] = TblPtrsList + i;
    }

    UtAssert_INT32_EQ(CFE_TBL_GetAddresses(NULL, numValidTbls, TblHandles), CFE_TBL_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_TBL_GetAddresses((void ***)&TblPtrs, numValidTbls, NULL), CFE_TBL_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_TBL_GetAddresses((void ***)&TblPtrs, numValidTbls, TblHandles), CFE_TBL_ERR_INVALID_HANDLE);
    /* Skip the first table handle to only consider valid handles */
    UtAssert_INT32_EQ(CFE_TBL_GetAddresses((void ***)&TblPtrs, numValidTbls, TblHandles + 1), CFE_TBL_ERR_NEVER_LOADED);

    /* Load data and then get addresses */
    for (int i = 1; i < numValidTbls + 1; i++)
    {
        if (CFE_TBL_Load(TblHandles[i], CFE_TBL_SRC_ADDRESS, &TestTable) != CFE_SUCCESS)
        {
            UtAssert_Failed("Failed to load data for table number %d", i);
        }
    }
    /* First time returns status message of CFE_TBL_INFO_UPDATED */
    UtAssert_INT32_EQ(CFE_TBL_GetAddresses((void ***)&TblPtrs, numValidTbls, TblHandles + 1), CFE_TBL_INFO_UPDATED);
    UtAssert_INT32_EQ(CFE_TBL_GetAddresses((void ***)&TblPtrs, numValidTbls, TblHandles + 1), CFE_SUCCESS);
    /* Attempt to release the invalid handle at the start of the array */
    UtAssert_INT32_EQ(CFE_TBL_ReleaseAddresses(numValidTbls, TblHandles), CFE_TBL_ERR_INVALID_HANDLE);
    UtAssert_INT32_EQ(CFE_TBL_ReleaseAddresses(numValidTbls, NULL), CFE_TBL_BAD_ARGUMENT);
    /* Skip the invalid handle */
    UtAssert_INT32_EQ(CFE_TBL_ReleaseAddresses(numValidTbls, TblHandles + 1), CFE_SUCCESS);

    /* Unregister all tables */
    for (int i = 1; i < numValidTbls + 1; i++)
    {
        if (CFE_TBL_Unregister(TblHandles[i]) != CFE_SUCCESS)
        {
            UtAssert_Failed("Failed to unregister table number %d", i);
        }
    }
}

void TBLContentAccessTestSetup(void)
{
    UtTest_Add(TestGetAddress, RegisterTestTable, NULL, "Test Table Get Address");
    UtTest_Add(TestReleaseAddress, RegisterTestTable, NULL, "Test Table Release Address");
    UtTest_Add(TestGetReleaseAddresses, NULL, NULL, "Test Table Get and Release Addresses");
}
