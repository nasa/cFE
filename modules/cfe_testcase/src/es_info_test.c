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
 *   Functional test of basic ES Information APIs
 *
 *   Demonstration of how to register and use the UT assert functions.
 */

/*
 * Includes
 */

#include "cfe_test.h"

const char TEST_EXPECTED_ENTRYPOINT[] = "CFE_TestMain";
const char TEST_EXPECTED_APP_NAME[]   = "CFE_TEST_APP";
const char TEST_EXPECTED_FILE_NAME[]  = "cfe_testcase";
const char ES_APP_EXPECTED_NAME[]     = "CFE_ES";
const char INVALID_APP_NAME[]         = "INVALID_NAME";

void TestGetAppInfo(void)
{
    CFE_ES_AppId_t   TestAppId;
    CFE_ES_AppId_t   ESAppId;
    CFE_ES_AppId_t   AppIdByName;
    char             AppNameBuf[OS_MAX_API_NAME + 4];
    CFE_ES_AppInfo_t TestAppInfo;
    CFE_ES_AppInfo_t ESAppInfo;

    UtPrintf("Testing: CFE_ES_GetAppID, CFE_ES_GetAppIDByName, CFE_ES_GetAppName, CFE_ES_GetAppInfo");

    UtAssert_INT32_EQ(CFE_ES_GetAppIDByName(&AppIdByName, TEST_EXPECTED_APP_NAME), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_GetAppID(&TestAppId), CFE_SUCCESS);
    CFE_Assert_RESOURCEID_EQ(TestAppId, AppIdByName);
    UtAssert_INT32_EQ(CFE_ES_GetAppName(AppNameBuf, TestAppId, sizeof(AppNameBuf)), CFE_SUCCESS);
    UtAssert_StrCmp(AppNameBuf, TEST_EXPECTED_APP_NAME, "CFE_ES_GetAppName() = %s", AppNameBuf);

    UtAssert_INT32_EQ(CFE_ES_GetAppInfo(&TestAppInfo, TestAppId), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_GetAppIDByName(&ESAppId, ES_APP_EXPECTED_NAME), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_GetAppInfo(&ESAppInfo, ESAppId), CFE_SUCCESS);

    UtAssert_True(TestAppInfo.Type == CFE_ES_AppType_EXTERNAL, "Test App Info -> Type = %d", (int)TestAppInfo.Type);
    UtAssert_True(ESAppInfo.Type == CFE_ES_AppType_CORE, "ES App Info -> Type = %d", (int)ESAppInfo.Type);

    UtAssert_StrCmp(TestAppInfo.Name, TEST_EXPECTED_APP_NAME, "Test App Info -> Name = %s", TestAppInfo.Name);
    UtAssert_StrCmp(ESAppInfo.Name, ES_APP_EXPECTED_NAME, "ES App Info -> Name = %s", ESAppInfo.Name);

    UtAssert_StrCmp(TestAppInfo.EntryPoint, TEST_EXPECTED_ENTRYPOINT, "Test App Info -> EntryPt  = %s",
                    TestAppInfo.EntryPoint);
    UtAssert_True(strlen(ESAppInfo.EntryPoint) == 0, "ES App Info -> EntryPt  = %s", ESAppInfo.EntryPoint);

    UtAssert_True(strstr(TestAppInfo.FileName, TEST_EXPECTED_FILE_NAME) != NULL, "Test App Info -> FileName = %s",
                  TestAppInfo.FileName);
    UtAssert_True(strlen(ESAppInfo.FileName) == 0, "ES App Info -> FileName  = %s", ESAppInfo.FileName);

    UtAssert_NONZERO(CFE_ES_MEMOFFSET_TO_SIZET(TestAppInfo.StackSize));
    UtAssert_NONZERO(CFE_ES_MEMOFFSET_TO_SIZET(ESAppInfo.StackSize));

    if (TestAppInfo.AddressesAreValid)
    {
        UtAssert_NOT_NULL(CFE_ES_MEMADDRESS_TO_PTR(TestAppInfo.CodeAddress));
        UtAssert_NONZERO(CFE_ES_MEMOFFSET_TO_SIZET(TestAppInfo.CodeSize));
        UtAssert_NOT_NULL(CFE_ES_MEMADDRESS_TO_PTR(TestAppInfo.DataAddress));
        UtAssert_NONZERO(CFE_ES_MEMOFFSET_TO_SIZET(TestAppInfo.DataSize));
        UtAssert_NOT_NULL(CFE_ES_MEMADDRESS_TO_PTR(TestAppInfo.BSSAddress));
        UtAssert_NONZERO(CFE_ES_MEMOFFSET_TO_SIZET(TestAppInfo.BSSSize));
    }
    else
    {
        UtAssert_NA("AddressesAreValid is false: Code Address test skipped");
        UtAssert_NA("AddressesAreValid is false: Code Size test skipped");
        UtAssert_NA("AddressesAreValid is false: Data Address test skipped");
        UtAssert_NA("AddressesAreValid is false: Data Size test skipped");
        UtAssert_NA("AddressesAreValid is false: BSS  Address test skipped");
        UtAssert_NA("AddressesAreValid is false: BSS Size test skipped");
    }

    UtAssert_True(ESAppInfo.AddressesAreValid == 0, "ES App Info -> AddrsValid?  = %d",
                  (int)ESAppInfo.AddressesAreValid);

    UtAssert_NOT_NULL(CFE_ES_MEMADDRESS_TO_PTR(TestAppInfo.StartAddress));
    UtAssert_NULL(CFE_ES_MEMADDRESS_TO_PTR(ESAppInfo.StartAddress));

    UtAssert_INT32_EQ(TestAppInfo.ExceptionAction, 0);
    UtAssert_INT32_EQ(ESAppInfo.ExceptionAction, 1);

    UtAssert_True(TestAppInfo.Priority > 0, "Test App Info -> Priority  = %d", (int)TestAppInfo.Priority);
    UtAssert_True(ESAppInfo.Priority > 0, "ES App Info -> Priority  = %d", (int)ESAppInfo.Priority);

    UtAssert_True(TestAppInfo.NumOfChildTasks == 0, "Test App Info -> Child Tasks  = %d",
                  (int)TestAppInfo.NumOfChildTasks);
    UtAssert_True(ESAppInfo.NumOfChildTasks > 0, "ES App Info -> Child Tasks  = %d", (int)ESAppInfo.NumOfChildTasks);

    UtAssert_INT32_EQ(CFE_ES_GetAppIDByName(&AppIdByName, INVALID_APP_NAME), CFE_ES_ERR_NAME_NOT_FOUND);
    CFE_Assert_RESOURCEID_UNDEFINED(AppIdByName);
    UtAssert_INT32_EQ(CFE_ES_GetAppID(NULL), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_GetAppIDByName(NULL, TEST_EXPECTED_APP_NAME), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_GetAppIDByName(&AppIdByName, NULL), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_GetAppName(AppNameBuf, CFE_ES_APPID_UNDEFINED, sizeof(AppNameBuf)),
                      CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_ES_GetAppName(NULL, TestAppId, sizeof(AppNameBuf)), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_GetAppInfo(&TestAppInfo, CFE_ES_APPID_UNDEFINED), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_ES_GetAppInfo(NULL, TestAppId), CFE_ES_BAD_ARGUMENT);
}

void TestGetTaskInfo(void)
{
    CFE_ES_AppId_t    AppId;
    CFE_ES_AppInfo_t  AppInfo;
    CFE_ES_TaskInfo_t TaskInfo;
    CFE_ES_TaskId_t   TaskId;

    UtPrintf("Testing: CFE_ES_GetTaskID, CFE_ES_GetTaskInfo");

    UtAssert_INT32_EQ(CFE_ES_GetAppID(&AppId), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_GetAppInfo(&AppInfo, AppId), CFE_SUCCESS);

    UtAssert_INT32_EQ(CFE_ES_GetTaskInfo(&TaskInfo, AppInfo.MainTaskId), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_GetTaskID(&TaskId), CFE_SUCCESS);
    CFE_Assert_RESOURCEID_EQ(TaskId, AppInfo.MainTaskId);

    UtAssert_StrCmp(TaskInfo.AppName, AppInfo.Name, "TaskInfo.AppName (%s) = AppInfo.name (%s)", TaskInfo.AppName,
                    AppInfo.Name);
    UtAssert_StrCmp(TaskInfo.TaskName, AppInfo.MainTaskName, "TaskInfo.TaskName (%s) = AppInfo.MainTaskName (%s)",
                    TaskInfo.TaskName, AppInfo.MainTaskName);

    CFE_Assert_RESOURCEID_EQ(TaskInfo.TaskId, AppInfo.MainTaskId);
    CFE_Assert_RESOURCEID_EQ(TaskInfo.AppId, AppId);
    UtAssert_INT32_EQ(TaskInfo.ExecutionCounter, AppInfo.ExecutionCounter);

    UtAssert_INT32_EQ(CFE_ES_GetTaskInfo(&TaskInfo, CFE_ES_TASKID_UNDEFINED), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_ES_GetTaskInfo(NULL, TaskId), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_GetTaskID(NULL), CFE_ES_BAD_ARGUMENT);
}

void TestGetLibInfo(void)
{
    CFE_ES_LibId_t   LibId;
    CFE_ES_LibId_t   CheckId;
    CFE_ES_AppInfo_t LibInfo;
    const char *     LibName     = "ASSERT_LIB";
    const char *     FileName    = "cfe_assert";
    const char *     InvalidName = "INVALID_NAME";
    char             LibNameBuf[OS_MAX_API_NAME + 4];

    UtPrintf("Testing: CFE_ES_GetLibIDByName, CFE_ES_GetLibName, CFE_ES_GetLibInfo");

    UtAssert_INT32_EQ(CFE_ES_GetLibIDByName(&LibId, LibName), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_GetLibInfo(&LibInfo, LibId), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_GetLibName(LibNameBuf, LibId, sizeof(LibNameBuf)), CFE_SUCCESS);
    UtAssert_StrCmp(LibNameBuf, LibName, "CFE_ES_GetLibName() = %s", LibNameBuf);
    UtAssert_True(LibInfo.Type == CFE_ES_AppType_LIBRARY, "Lib Info -> Type = %d", (int)LibInfo.Type);
    UtAssert_StrCmp(LibInfo.Name, LibName, "Lib Info -> Name = %s", LibInfo.Name);
    UtAssert_StrCmp(LibInfo.EntryPoint, "CFE_Assert_LibInit", "Lib Info -> EntryPt  = %s", LibInfo.EntryPoint);
    UtAssert_True(strstr(LibInfo.FileName, FileName) != NULL, "Lib Info -> FileName = %s contains %s", LibInfo.FileName,
                  FileName);

    UtAssert_ZERO(CFE_ES_MEMOFFSET_TO_SIZET(LibInfo.StackSize));

    if (LibInfo.AddressesAreValid)
    {
        UtAssert_NOT_NULL(CFE_ES_MEMADDRESS_TO_PTR(LibInfo.CodeAddress));
        UtAssert_NONZERO(CFE_ES_MEMOFFSET_TO_SIZET(LibInfo.CodeSize));
        UtAssert_NOT_NULL(CFE_ES_MEMADDRESS_TO_PTR(LibInfo.DataAddress));
        UtAssert_NONZERO(CFE_ES_MEMOFFSET_TO_SIZET(LibInfo.DataSize));
        UtAssert_NOT_NULL(CFE_ES_MEMADDRESS_TO_PTR(LibInfo.BSSAddress));
        UtAssert_NONZERO(CFE_ES_MEMOFFSET_TO_SIZET(LibInfo.BSSSize));
    }
    else
    {
        UtAssert_NA("AddressesAreValid is false: Code Address test skipped");
        UtAssert_NA("AddressesAreValid is false: Code Size test skipped");
        UtAssert_NA("AddressesAreValid is false: Data Address test skipped");
        UtAssert_NA("AddressesAreValid is false: Data Size test skipped");
        UtAssert_NA("AddressesAreValid is false: BSS  Address test skipped");
        UtAssert_NA("AddressesAreValid is false: BSS Size test skipped");
    }

    UtAssert_INT32_EQ(LibInfo.ExceptionAction, 0);
    UtAssert_True(LibInfo.Priority == 0, "Lib Info -> Priority  = %d", (int)LibInfo.Priority);
    CFE_Assert_RESOURCEID_UNDEFINED(LibInfo.MainTaskId);
    UtAssert_True(LibInfo.ExecutionCounter == 0, "Lib Info -> ExecutionCounter  = %d", (int)LibInfo.ExecutionCounter);
    UtAssert_True(strlen(LibInfo.MainTaskName) == 0, "Lib Info -> Task Name  = %s", LibInfo.MainTaskName);
    UtAssert_True(LibInfo.NumOfChildTasks == 0, "Lib Info -> Child Tasks  = %d", (int)LibInfo.NumOfChildTasks);

    UtAssert_INT32_EQ(CFE_ES_GetLibIDByName(&CheckId, InvalidName), CFE_ES_ERR_NAME_NOT_FOUND);
    CFE_Assert_RESOURCEID_UNDEFINED(CheckId);
    UtAssert_INT32_EQ(CFE_ES_GetLibInfo(&LibInfo, CFE_ES_LIBID_UNDEFINED), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_ES_GetLibInfo(NULL, LibId), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_GetLibIDByName(NULL, LibName), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_GetLibIDByName(&CheckId, NULL), CFE_ES_BAD_ARGUMENT);
    CFE_Assert_RESOURCEID_UNDEFINED(CheckId);
    UtAssert_INT32_EQ(CFE_ES_GetLibName(LibNameBuf, CFE_ES_LIBID_UNDEFINED, sizeof(LibNameBuf)),
                      CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_ES_GetLibName(LibNameBuf, LibId, 0), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_GetLibName(NULL, LibId, sizeof(LibNameBuf)), CFE_ES_BAD_ARGUMENT);
}

void TestResetType(void)
{
    int32  rType;
    uint32 rSubType;

    UtPrintf("Testing: CFE_ES_GetResetType");

    rType = CFE_ES_GetResetType(NULL);
    UtAssert_True((rType == CFE_PSP_RST_TYPE_POWERON) || (rType == CFE_PSP_RST_TYPE_PROCESSOR), "Reset Type = %d",
                  (int)rType);

    CFE_ES_GetResetType(&rSubType);
    UtAssert_True((rSubType > 0) && (rSubType < 10), "Reset Sub-Type = %d", (int)rSubType);
}

void TestGetModuleInfo(void)
{
    CFE_ES_AppInfo_t ModuleInfo;
    CFE_ES_LibId_t   LibIdByName;
    CFE_ES_AppId_t   TestAppId;
    CFE_ES_AppInfo_t LibInfo;
    CFE_ES_AppInfo_t TestAppInfo;
    const char *     LibName = "ASSERT_LIB";

    memset(&ModuleInfo, 0, sizeof(ModuleInfo));
    memset(&LibInfo, 0, sizeof(LibInfo));
    memset(&TestAppInfo, 0, sizeof(TestAppInfo));

    UtPrintf("Testing: CFE_ES_GetModuleInfo");

    UtAssert_INT32_EQ(CFE_ES_GetAppID(&TestAppId), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_GetAppInfo(&TestAppInfo, TestAppId), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_GetModuleInfo(&ModuleInfo, CFE_RESOURCEID_UNWRAP(TestAppId)), CFE_SUCCESS);
    UtAssert_StrCmp(TestAppInfo.Name, ModuleInfo.Name, "App Info Name (%s) = Module Info Name (%s) ", TestAppInfo.Name,
                    ModuleInfo.Name);

    UtAssert_INT32_EQ(CFE_ES_GetLibIDByName(&LibIdByName, LibName), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_GetLibInfo(&LibInfo, LibIdByName), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_GetModuleInfo(&ModuleInfo, CFE_RESOURCEID_UNWRAP(LibIdByName)), CFE_SUCCESS);
    UtAssert_StrCmp(LibInfo.Name, ModuleInfo.Name, "Lib Info Name (%s) = Module Info Name (%s) ", LibInfo.Name,
                    ModuleInfo.Name);

    UtAssert_INT32_EQ(CFE_ES_GetModuleInfo(&ModuleInfo, CFE_RESOURCEID_UNDEFINED), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_ES_GetModuleInfo(NULL, CFE_RESOURCEID_UNWRAP(TestAppId)), CFE_ES_BAD_ARGUMENT);
}

void ESInfoTestSetup(void)
{
    UtTest_Add(TestGetAppInfo, NULL, NULL, "Test App Info");
    UtTest_Add(TestGetTaskInfo, NULL, NULL, "Test Task Info");
    UtTest_Add(TestGetLibInfo, NULL, NULL, "Test Lib Info");
    UtTest_Add(TestResetType, NULL, NULL, "Test Reset Type");
    UtTest_Add(TestGetModuleInfo, NULL, NULL, "Test Module Info");
}
