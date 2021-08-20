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
** File: tbl_content_mang_test.c
**
** Purpose:
**   Functional test of Table Manage Content APIs
**
**   Demonstration of how to register and use the UT assert functions.
**
*************************************************************************/

/*
 * Includes
 */

#include "cfe_test.h"
#include "cfe_test_table.h"

/* Does not test partial loads */
void TestLoad(void)
{
    UtPrintf("Testing: CFE_TBL_Load");
    CFE_TBL_Handle_t BadTblHandle;
    const char *     BadTblName = "BadTableName";
    UtAssert_INT32_EQ(
        CFE_TBL_Register(&BadTblHandle, BadTblName, sizeof(TBL_TEST_Table_t), CFE_TBL_OPT_DBL_BUFFER, NULL),
        CFE_SUCCESS);

    /* Load from file */
    UtAssert_INT32_EQ(CFE_TBL_Load(CFE_FT_Global.TblHandle, CFE_TBL_SRC_FILE, "/cf/cfe_test_tbl.tbl"), CFE_SUCCESS);
    /* Load again */
    UtAssert_INT32_EQ(CFE_TBL_Load(CFE_FT_Global.TblHandle, CFE_TBL_SRC_FILE, "/cf/cfe_test_tbl.tbl"), CFE_SUCCESS);
    /* Table name mismatches */
    UtAssert_INT32_EQ(CFE_TBL_Load(BadTblHandle, CFE_TBL_SRC_FILE, "/cf/cfe_test_tbl.tbl"),
                      CFE_TBL_ERR_FILE_FOR_WRONG_TABLE);
    UtAssert_INT32_EQ(CFE_TBL_Load(CFE_FT_Global.TblHandle, CFE_TBL_SRC_FILE, "/cf/sample_app_tbl.tbl"),
                      CFE_TBL_ERR_FILE_FOR_WRONG_TABLE);
    UtAssert_INT32_EQ(CFE_TBL_Load(CFE_FT_Global.TblHandle, CFE_TBL_SRC_FILE, "/cf/not_cfe_test_tbl.tbl"),
                      CFE_TBL_ERR_ACCESS);

    UtAssert_INT32_EQ(CFE_TBL_Load(CFE_TBL_BAD_TABLE_HANDLE, CFE_TBL_SRC_FILE, "/cf/cfe_test_tbl.tbl"),
                      CFE_TBL_ERR_INVALID_HANDLE);

    /* Load from memory */
    TBL_TEST_Table_t TestTable = {1, 2};
    UtAssert_INT32_EQ(CFE_TBL_Load(CFE_FT_Global.TblHandle, CFE_TBL_SRC_ADDRESS, &TestTable), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_TBL_Load(CFE_FT_Global.TblHandle, CFE_TBL_SRC_ADDRESS, NULL), CFE_TBL_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_TBL_Load(CFE_TBL_BAD_TABLE_HANDLE, CFE_TBL_SRC_ADDRESS, &TestTable),
                      CFE_TBL_ERR_INVALID_HANDLE);

    /* Attempt to load a dump only table */
    CFE_TBL_Handle_t DumpTblHandle;
    const char *     DumpTblName = "DumpOnlyTable";
    UtAssert_INT32_EQ(
        CFE_TBL_Register(&DumpTblHandle, DumpTblName, sizeof(TBL_TEST_Table_t), CFE_TBL_OPT_DUMP_ONLY, NULL),
        CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_TBL_Load(DumpTblHandle, CFE_TBL_SRC_FILE, "/cf/cfe_test_tbl.tbl"), CFE_TBL_ERR_DUMP_ONLY);

    /* Load a shared table */
    CFE_TBL_Handle_t SharedTblHandle;
    const char *     SharedTblName = "SAMPLE_APP.SampleAppTable";
    UtAssert_INT32_EQ(CFE_TBL_Share(&SharedTblHandle, SharedTblName), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_TBL_Load(SharedTblHandle, CFE_TBL_SRC_FILE, "/cf/sample_app_tbl.tbl"), CFE_SUCCESS);
}

void TestUpdate(void)
{
    UtPrintf("Testing: CFE_TBL_Update");
    /* Haven't figured out how to get an update pending */
    UtAssert_INT32_EQ(CFE_TBL_Update(CFE_FT_Global.TblHandle), CFE_TBL_INFO_NO_UPDATE_PENDING);
    UtAssert_INT32_EQ(CFE_TBL_Update(CFE_TBL_BAD_TABLE_HANDLE), CFE_TBL_ERR_INVALID_HANDLE);
}

void TestValidate(void)
{
    UtPrintf("Testing: CFE_TBL_Validate");
    /* Haven't figured out how to get a validation pending */
    UtAssert_INT32_EQ(CFE_TBL_Validate(CFE_FT_Global.TblHandle), CFE_TBL_INFO_NO_VALIDATION_PENDING);
    UtAssert_INT32_EQ(CFE_TBL_Validate(CFE_TBL_BAD_TABLE_HANDLE), CFE_TBL_ERR_INVALID_HANDLE);
}

void TestManage(void)
{
    UtPrintf("Testing: CFE_TBL_Manage");
    UtAssert_INT32_EQ(CFE_TBL_Manage(CFE_FT_Global.TblHandle), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_TBL_Manage(CFE_TBL_BAD_TABLE_HANDLE), CFE_TBL_ERR_INVALID_HANDLE);
}

void TestDumpToBuffer(void)
{
    UtPrintf("Testing: CFE_TBL_DumpToBuffer");
    /* This should at least return an info code such as CFE_TBL_INFO_NO_UPDATE_PENDING when CFE_TBL_Update is called
     * with no pending update instead of returning CFE_SUCCESS whether or not it actually dumped*/
    UtAssert_INT32_EQ(CFE_TBL_DumpToBuffer(CFE_FT_Global.TblHandle), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_TBL_DumpToBuffer(CFE_TBL_BAD_TABLE_HANDLE), CFE_TBL_ERR_INVALID_HANDLE);
}

void TestModified(void)
{
    UtPrintf("Testing: CFE_TBL_Modified");
    UtAssert_INT32_EQ(CFE_TBL_Modified(CFE_FT_Global.TblHandle), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_TBL_Modified(CFE_TBL_BAD_TABLE_HANDLE), CFE_TBL_ERR_INVALID_HANDLE);
}

void TBLContentMangTestSetup(void)
{
    UtTest_Add(TestLoad, RegisterTestTable, UnregisterTestTable, "Test Table Load");
    UtTest_Add(TestUpdate, RegisterTestTable, UnregisterTestTable, "Test Table Update");
    UtTest_Add(TestValidate, RegisterTestTable, UnregisterTestTable, "Test Table Validate");
    UtTest_Add(TestManage, RegisterTestTable, UnregisterTestTable, "Test Table Manage");
    UtTest_Add(TestDumpToBuffer, RegisterTestTable, UnregisterTestTable, "Test Table Dump to Buffer");
    UtTest_Add(TestModified, RegisterTestTable, UnregisterTestTable, "Test Table Modified");
}
