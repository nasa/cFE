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
 *   Functional test of basic ES Critical Data Store APIs
 *
 *   Demonstration of how to register and use the UT assert functions.
 */

/*
 * Includes
 */

#include "cfe_test.h"

void TestRegisterCDS(void)
{
    CFE_ES_CDSHandle_t CDSHandlePtr;
    CFE_ES_CDSHandle_t CDSHandlePtr2;

    size_t      BlockSize  = 10;
    size_t      BlockSize2 = 15;
    const char *Name       = "CDS_Test";
    const char *LongName   = "VERY_LONG_NAME_CDS_Test";

    UtPrintf("Testing: CFE_ES_RegisterCDS");

    /*
     * Since this test app may be executed multiple times, or the system may have
     * been booted from a processor reset rather than a power-on reset, it is possible
     * that the CDS already exists at the time this test is executed.  In this case
     * the new CDS allocation path cannot be checked in functional test, but other CDS
     * functions can still be called.
     */

    CFE_Assert_STATUS_STORE(CFE_ES_RegisterCDS(&CDSHandlePtr, BlockSize2, Name));

    if (CFE_Assert_STATUS_MAY_BE(CFE_ES_CDS_ALREADY_EXISTS))
    {
        /*
         * add an informational message that the functional test is incomplete here, due
         * to preconditions beyond the control of this test app.  Need to clear the CDS
         * memory and/or do a power-on reset to get full test.
         */
        UtAssert_WARN("CDS already exists. CFE_ES_RegisterCDS new allocation could not be properly tested");
    }
    else
    {
        /*
         * If not CFE_ES_CDS_ALREADY_EXISTS, then the only other acceptable status is CFE_SUCCESS,
         * which indicates that the CDS was created and initialized from a clean slate.
         */
        CFE_Assert_STATUS_MUST_BE(CFE_SUCCESS);

        /* In this case, calling CFE_ES_RegisterCDS() again should return the CFE_ES_CDS_ALREADY_EXISTS */
        UtAssert_INT32_EQ(CFE_ES_RegisterCDS(&CDSHandlePtr2, BlockSize2, Name), CFE_ES_CDS_ALREADY_EXISTS);
    }

    UtAssert_INT32_EQ(CFE_ES_RegisterCDS(&CDSHandlePtr2, BlockSize, Name), CFE_SUCCESS);

    UtAssert_INT32_EQ(CFE_ES_RegisterCDS(NULL, BlockSize, Name), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_RegisterCDS(&CDSHandlePtr, 0, Name), CFE_ES_CDS_INVALID_SIZE);
    UtAssert_INT32_EQ(CFE_ES_RegisterCDS(&CDSHandlePtr, BlockSize, NULL), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_RegisterCDS(&CDSHandlePtr, BlockSize, LongName), CFE_ES_CDS_INVALID_NAME);
}

void TestCDSName(void)
{
    CFE_ES_CDSHandle_t CDSHandlePtr;
    size_t             BlockSize    = 10;
    const char *       Name         = "CDS_Test";
    const char *       CDSName      = "CFE_TEST_APP.CDS_Test";
    const char *       INVALID_NAME = "INVALID_NAME";

    CFE_ES_CDSHandle_t IdByName;
    char               CDSNameBuf[CFE_MISSION_ES_CDS_MAX_FULL_NAME_LEN];

    memset(CDSNameBuf, 0, sizeof(CDSNameBuf));

    UtPrintf("Testing: CFE_ES_GetCDSBlockIDByName, CFE_ES_GetCDSBlockName");

    UtAssert_INT32_EQ(CFE_ES_RegisterCDS(&CDSHandlePtr, BlockSize, Name), CFE_ES_CDS_ALREADY_EXISTS);

    UtAssert_INT32_EQ(CFE_ES_GetCDSBlockName(CDSNameBuf, CDSHandlePtr, sizeof(CDSNameBuf)), CFE_SUCCESS);
    UtAssert_StrCmp(CDSNameBuf, CDSName, "CFE_ES_GetCDSBlockName() = %s", CDSNameBuf);
    UtAssert_INT32_EQ(CFE_ES_GetCDSBlockIDByName(&IdByName, CDSNameBuf), CFE_SUCCESS);
    CFE_Assert_RESOURCEID_EQ(CDSHandlePtr, IdByName);

    UtAssert_INT32_EQ(CFE_ES_GetCDSBlockName(NULL, CDSHandlePtr, sizeof(CDSNameBuf)), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_GetCDSBlockName(CDSNameBuf, CFE_ES_CDS_BAD_HANDLE, sizeof(CDSNameBuf)),
                      CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_ES_GetCDSBlockName(CDSNameBuf, CDSHandlePtr, 0), CFE_ES_BAD_ARGUMENT);

    UtAssert_INT32_EQ(CFE_ES_GetCDSBlockIDByName(NULL, CDSNameBuf), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_GetCDSBlockIDByName(&IdByName, NULL), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_GetCDSBlockIDByName(&IdByName, INVALID_NAME), CFE_ES_ERR_NAME_NOT_FOUND);
}

void TestCopyRestoreCDS(void)
{
    CFE_ES_CDSHandle_t CDSHandlePtr;
    size_t             BlockSize = 10;
    const char *       Name      = "CDS_Copy_Test";
    CFE_Status_t       status;
    char               Data[BlockSize];
    char               DataBuff[BlockSize];

    memset(DataBuff, 0, sizeof(DataBuff));

    UtPrintf("Testing: CFE_ES_CopyToCDS, CFE_ES_RestoreFromCDS");

    snprintf(Data, BlockSize, "Test Data");

    status = CFE_ES_RegisterCDS(&CDSHandlePtr, BlockSize, Name);
    UtAssert_True(status == CFE_SUCCESS || status == CFE_ES_CDS_ALREADY_EXISTS, "Register CDS status = %d",
                  (int)status);

    UtAssert_INT32_EQ(CFE_ES_CopyToCDS(CDSHandlePtr, Data), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_RestoreFromCDS(DataBuff, CDSHandlePtr), CFE_SUCCESS);
    UtAssert_StrCmp(Data, DataBuff, "RestoreFromCDS = %s", DataBuff);

    UtAssert_INT32_EQ(CFE_ES_CopyToCDS(CFE_ES_CDS_BAD_HANDLE, Data), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_ES_CopyToCDS(CDSHandlePtr, NULL), CFE_ES_BAD_ARGUMENT);

    UtAssert_INT32_EQ(CFE_ES_RestoreFromCDS(DataBuff, CFE_ES_CDS_BAD_HANDLE), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_ES_RestoreFromCDS(NULL, CDSHandlePtr), CFE_ES_BAD_ARGUMENT);
}

void ESCDSTestSetup(void)
{
    UtTest_Add(TestRegisterCDS, NULL, NULL, "Test Register CDS");
    UtTest_Add(TestCDSName, NULL, NULL, "Test CDS Name");
    UtTest_Add(TestCopyRestoreCDS, NULL, NULL, "Test Copy Restore CDS");
}
