/*************************************************************************
**
**      GSC-18128-1, "Core Flight Executive Version 6.7"
**
**      Copyright (c) 2006-2019 United States Government as represented by
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
** File: tbl_registration_test.c
**
** Purpose:
**   Functional test of Table Registration APIs
**
**   Demonstration of how to register and use the UT assert functions.
**
*************************************************************************/

/*
 * Includes
 */

#include "cfe_test.h"

int32 CallbackFunc(void *TblPtr)
{
    return 1;
}

void TestTableRegistration(void)
{
    UtPrintf("Testing: CFE_TBL_Register, CFE_TBL_Unregister");
    CFE_TBL_Handle_t TblHandle;
    const char *     TblName = "TestTable";
    char             BadTblName[CFE_TBL_MAX_FULL_NAME_LEN + 2];
    BadTblName[CFE_TBL_MAX_FULL_NAME_LEN + 1] = '\0';
    memset(BadTblName, 'a', sizeof(BadTblName) - 1);

    /* Successfully create table */
    UtAssert_INT32_EQ(
        CFE_TBL_Register(&TblHandle, TblName, sizeof(TBL_TEST_Table_t), CFE_TBL_OPT_DEFAULT, &CallbackFunc),
        CFE_SUCCESS);

    /* Duplicate table */
    UtAssert_INT32_EQ(
        CFE_TBL_Register(&TblHandle, TblName, sizeof(TBL_TEST_Table_t), CFE_TBL_OPT_DEFAULT, &CallbackFunc),
        CFE_TBL_WARN_DUPLICATE);

    /* Unregister the table */
    UtAssert_INT32_EQ(CFE_TBL_Unregister(TblHandle), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_TBL_Unregister(TblHandle), CFE_TBL_ERR_INVALID_HANDLE);

    /* Invalid Name */
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle, BadTblName, sizeof(TBL_TEST_Table_t), CFE_TBL_OPT_DEFAULT, NULL),
                      CFE_TBL_ERR_INVALID_NAME);
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle, "", sizeof(TBL_TEST_Table_t), CFE_TBL_OPT_DEFAULT, NULL),
                      CFE_TBL_ERR_INVALID_NAME);

    /* Invalid Table Size */
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle, TblName, 0, CFE_TBL_OPT_DEFAULT, NULL), CFE_TBL_ERR_INVALID_SIZE);
    UtAssert_INT32_EQ(
        CFE_TBL_Register(&TblHandle, TblName, CFE_PLATFORM_TBL_MAX_SNGL_TABLE_SIZE + 1, CFE_TBL_OPT_SNGL_BUFFER, NULL),
        CFE_TBL_ERR_INVALID_SIZE);
    UtAssert_INT32_EQ(
        CFE_TBL_Register(&TblHandle, TblName, CFE_PLATFORM_TBL_MAX_DBL_TABLE_SIZE + 1, CFE_TBL_OPT_DBL_BUFFER, NULL),
        CFE_TBL_ERR_INVALID_SIZE);

    /* Invalid Table Options */
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle, TblName, sizeof(TBL_TEST_Table_t),
                                       CFE_TBL_OPT_DBL_BUFFER | CFE_TBL_OPT_USR_DEF_ADDR, NULL),
                      CFE_TBL_ERR_INVALID_OPTIONS);
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle, TblName, sizeof(TBL_TEST_Table_t),
                                       CFE_TBL_OPT_CRITICAL | CFE_TBL_OPT_DUMP_ONLY, NULL),
                      CFE_TBL_ERR_INVALID_OPTIONS);
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle, TblName, sizeof(TBL_TEST_Table_t),
                                       CFE_TBL_OPT_CRITICAL | CFE_TBL_OPT_USR_DEF_ADDR, NULL),
                      CFE_TBL_ERR_INVALID_OPTIONS);

    /*
     * Create the maximum number of tables
     * There are already some tables in this system, so this will
     * stop succeeding before it reaches the end of the loop
     * Check that after the loop no more tables can be created
     */
    CFE_TBL_Handle_t TblHandles[CFE_PLATFORM_TBL_MAX_NUM_TABLES];
    char             TblName2[10];
    int              numTblsCreated = 0; /* Track num created to unregister them all */
    for (int i = 0; i < CFE_PLATFORM_TBL_MAX_NUM_TABLES; i++)
    {
        sprintf(TblName2, "%d", i);
        if (CFE_TBL_Register(&TblHandles[i], TblName2, sizeof(TBL_TEST_Table_t), CFE_TBL_OPT_DEFAULT, NULL) ==
            CFE_SUCCESS)
        {
            numTblsCreated++;
        }
    }
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandles[numTblsCreated], TblName, sizeof(TBL_TEST_Table_t),
                                       CFE_TBL_OPT_DEFAULT, &CallbackFunc),
                      CFE_TBL_ERR_REGISTRY_FULL);
    /* Unregister the tables */
    for (int i = 0; i < numTblsCreated; i++)
    {
        if (CFE_TBL_Unregister(TblHandles[i]) != CFE_SUCCESS)
        {
            UtAssert_Failed("Failed to unregister table number %d", i);
        }
    }
}

void TestTableShare(void)
{
    UtPrintf("Testing: CFE_TBL_Share");
    CFE_TBL_Handle_t SharedTblHandle;
    const char *     SharedTblName = "SAMPLE_APP.SampleAppTable";
    const char *     BadTblName    = "SampleAppTable";
    UtAssert_INT32_EQ(CFE_TBL_Share(&SharedTblHandle, SharedTblName), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_TBL_Share(&SharedTblHandle, NULL), CFE_TBL_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_TBL_Share(&SharedTblHandle, BadTblName), CFE_TBL_ERR_INVALID_NAME);
}

void TBLRegistrationTestSetup(void)
{
    UtTest_Add(TestTableRegistration, NULL, NULL, "Test Table Registration");
    UtTest_Add(TestTableShare, NULL, NULL, "Test Table Sharing");
}
