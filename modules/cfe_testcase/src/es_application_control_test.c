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
 *   Functional test of ES Application Control APIs
 *
 *   Tests only invalid calls to the application control functions.
 *
 *   Demonstration of how to register and use the UT assert functions.
 */

/*
 * Includes
 */

#include "cfe_test.h"

void TestApplicationControl(void)
{
    UtPrintf("Testing: CFE_ES_RestartApp, CFE_ES_ReloadApp, CFE_ES_DeleteApp");
    CFE_ES_AppId_t TestAppId;
    UtAssert_INT32_EQ(CFE_ES_GetAppID(&TestAppId), CFE_SUCCESS);

    UtAssert_UINT32_EQ(CFE_ES_RestartApp(CFE_ES_APPID_UNDEFINED), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    /*
     * This seems a bit strange that it throws a file io error
     * CFE_ES_ReloadApp calls OS_stat with the null filename
     * OS_stat should return OS_INVALID_POINTER, but the exact
     * error is ignored in CFE_ES_ReloadApp and file io error is returned
     * most other functions return a CFE_ES_BAD_ARGUMENT in this situation
     */
    UtAssert_UINT32_EQ(CFE_ES_ReloadApp(TestAppId, NULL), CFE_ES_FILE_IO_ERR);
    UtAssert_UINT32_EQ(CFE_ES_ReloadApp(TestAppId, "/cf/NOT_cfe_testcase.so"), CFE_ES_FILE_IO_ERR);
    UtAssert_UINT32_EQ(CFE_ES_ReloadApp(CFE_ES_APPID_UNDEFINED, "/cf/cfe_testcase.so"),
                       CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_UINT32_EQ(CFE_ES_DeleteApp(CFE_ES_APPID_UNDEFINED), CFE_ES_ERR_RESOURCEID_NOT_VALID);
}

void ESApplicationControlTestSetup(void)
{
    UtTest_Add(TestApplicationControl, NULL, NULL, "Test Application Control API");
}
