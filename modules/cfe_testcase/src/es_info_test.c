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
** File: es_info_test.c
**
** Purpose:
**   Functional test of basic ES Information APIs
**
**   Demonstration of how to register and use the UT assert functions.
**
*************************************************************************/

/*
 * Includes
 */

#include "cfe_test.h"

void TestAppInfo(void)
{
    CFE_ES_AppId_t   TestAppId;
    CFE_ES_AppId_t   ESAppId;
    CFE_ES_AppId_t   AppIdByName;
    const char *     TestAppName = "TESTRUN_APP";
    const char *     ESAppName   = "CFE_ES";
    const char *     InvalidName = "INVALID_NAME";
    char             AppNameBuf[OS_MAX_API_NAME + 4];
    CFE_ES_AppInfo_t TestAppInfo;
    CFE_ES_AppInfo_t ESAppInfo;

    UtPrintf("Testing: CFE_ES_GetAppID, CFE_ES_GetAppIDByName, CFE_ES_GetAppName, CFE_ES_GetAppInfo");

    UtAssert_INT32_EQ(CFE_ES_GetAppIDByName(&AppIdByName, TestAppName), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_GetAppID(&TestAppId), CFE_SUCCESS);
    UtAssert_ResourceID_EQ(TestAppId, AppIdByName);
    UtAssert_INT32_EQ(CFE_ES_GetAppName(AppNameBuf, TestAppId, sizeof(AppNameBuf)), CFE_SUCCESS);
    UtAssert_StrCmp(AppNameBuf, TestAppName, "CFE_ES_GetAppName() = %s", AppNameBuf);

    UtAssert_INT32_EQ(CFE_ES_GetAppInfo(&TestAppInfo, TestAppId), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_GetAppIDByName(&ESAppId, ESAppName), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_GetAppInfo(&ESAppInfo, ESAppId), CFE_SUCCESS);

    UtAssert_True(TestAppInfo.Type == CFE_ES_AppType_EXTERNAL, "Test App Info -> Type = %d", (int)TestAppInfo.Type);
    UtAssert_True(ESAppInfo.Type == CFE_ES_AppType_CORE, "ES App Info -> Type = %d", (int)ESAppInfo.Type);

    UtAssert_StrCmp(TestAppInfo.Name, TestAppName, "Test App Info -> Name = %s", TestAppInfo.Name);
    UtAssert_StrCmp(ESAppInfo.Name, ESAppName, "ES App Info -> Name = %s", ESAppInfo.Name);

    UtAssert_StrCmp(TestAppInfo.EntryPoint, "CFE_TR_AppMain", "Test App Info -> EntryPt  = %s", TestAppInfo.EntryPoint);
    UtAssert_True(strlen(ESAppInfo.EntryPoint) == 0, "ES App Info -> EntryPt  = %s", ESAppInfo.EntryPoint);

    UtAssert_StrCmp(TestAppInfo.FileName, "/cf/cfe_testrunner.so", "Test App Info -> FileName = %s",
                    TestAppInfo.FileName);
    UtAssert_True(strlen(ESAppInfo.FileName) == 0, "ES App Info -> FileName  = %s", ESAppInfo.FileName);

    UtAssert_True(TestAppInfo.StackSize > 0, "Test App Info -> StackSz  = %d", (int)TestAppInfo.StackSize);
    UtAssert_True(ESAppInfo.StackSize > 0, "ES App Info -> StackSz  = %d", (int)ESAppInfo.StackSize);

    if (TestAppInfo.AddressesAreValid)
    {
        UtAssert_True(TestAppInfo.AddressesAreValid > 0, "Test App Info -> AddrsValid?  = %d",
                      (int)TestAppInfo.AddressesAreValid);
        UtAssert_True(TestAppInfo.CodeAddress > 0, "Test App Info -> CodeAddress  = %ld",
                      (unsigned long)TestAppInfo.CodeAddress);
        UtAssert_True(TestAppInfo.CodeSize > 0, "Test App Info -> CodeSize  = %ld",
                      (unsigned long)TestAppInfo.CodeSize);
        UtAssert_True(TestAppInfo.DataAddress > 0, "Test App Info -> DataAddress  = %ld",
                      (unsigned long)TestAppInfo.DataAddress);
        UtAssert_True(TestAppInfo.DataSize > 0, "Test App Info -> DataSize  = %ld",
                      (unsigned long)TestAppInfo.DataSize);
        UtAssert_True(TestAppInfo.BSSAddress > 0, "Test App Info -> BSSAddress  = %ld",
                      (unsigned long)TestAppInfo.BSSAddress);
        UtAssert_True(TestAppInfo.BSSSize > 0, "Test App Info -> BSSSize  = %ld", (unsigned long)TestAppInfo.BSSSize);
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

    UtAssert_True(TestAppInfo.StartAddress > 0, "Test App Info -> StartAddress  = 0x%8lx",
                  (unsigned long)TestAppInfo.StartAddress);
    UtAssert_True(ESAppInfo.StartAddress == 0, "ES App Info -> StartAddress  = 0x%8lx",
                  (unsigned long)ESAppInfo.StartAddress);

    UtAssert_INT32_EQ(TestAppInfo.ExceptionAction, 0);
    UtAssert_INT32_EQ(ESAppInfo.ExceptionAction, 1);

    UtAssert_True(TestAppInfo.Priority > 0, "Test App Info -> Priority  = %d", (int)TestAppInfo.Priority);
    UtAssert_True(ESAppInfo.Priority > 0, "ES App Info -> Priority  = %d", (int)ESAppInfo.Priority);

    UtAssert_True(TestAppInfo.NumOfChildTasks == 0, "Test App Info -> Child Tasks  = %d",
                  (int)TestAppInfo.NumOfChildTasks);
    UtAssert_True(ESAppInfo.NumOfChildTasks > 0, "ES App Info -> Child Tasks  = %d", (int)ESAppInfo.NumOfChildTasks);

    UtAssert_INT32_EQ(CFE_ES_GetAppIDByName(&AppIdByName, InvalidName), CFE_ES_ERR_NAME_NOT_FOUND);
    UtAssert_ResourceID_Undifeined(AppIdByName);
    UtAssert_INT32_EQ(CFE_ES_GetAppID(NULL), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_GetAppIDByName(NULL, TestAppName), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_GetAppName(AppNameBuf, CFE_ES_APPID_UNDEFINED, sizeof(AppNameBuf)),
                      CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_ES_GetAppName(NULL, TestAppId, sizeof(AppNameBuf)), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_GetAppInfo(&TestAppInfo, CFE_ES_APPID_UNDEFINED), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_ES_GetAppInfo(NULL, CFE_ES_APPID_UNDEFINED), CFE_ES_BAD_ARGUMENT);
}

void TestTaskInfo(void)
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
    UtAssert_ResourceID_EQ(TaskId, AppInfo.MainTaskId);

    UtAssert_StrCmp(TaskInfo.AppName, AppInfo.Name, "TaskInfo.AppName (%s) = AppInfo.name (%s)", TaskInfo.AppName,
                    AppInfo.Name);
    UtAssert_StrCmp(TaskInfo.TaskName, AppInfo.MainTaskName, "TaskInfo.TaskName (%s) = AppInfo.MainTaskName (%s)",
                    TaskInfo.TaskName, AppInfo.MainTaskName);

    UtAssert_ResourceID_EQ(TaskInfo.TaskId, AppInfo.MainTaskId);
    UtAssert_ResourceID_EQ(TaskInfo.AppId, AppId);
    UtAssert_INT32_EQ(TaskInfo.ExecutionCounter, AppInfo.ExecutionCounter);

    UtAssert_INT32_EQ(CFE_ES_GetTaskInfo(&TaskInfo, CFE_ES_TASKID_UNDEFINED), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_ES_GetTaskInfo(NULL, CFE_ES_TASKID_UNDEFINED), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_GetTaskID(NULL), CFE_ES_BAD_ARGUMENT);
}

void TestLibInfo(void)
{
    CFE_ES_LibId_t   LibIdByName;
    CFE_ES_AppInfo_t LibInfo;
    const char *     LibName     = "ASSERT_LIB";
    const char *     InvalidName = "INVALID_NAME";
    char             LibNameBuf[OS_MAX_API_NAME + 4];

    UtPrintf("Testing: CFE_ES_GetLibIDByName, CFE_ES_GetLibName, CFE_ES_GetLibInfo");

    UtAssert_INT32_EQ(CFE_ES_GetLibIDByName(&LibIdByName, LibName), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_GetLibInfo(&LibInfo, LibIdByName), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_GetLibName(LibNameBuf, LibIdByName, sizeof(LibNameBuf)), CFE_SUCCESS);
    UtAssert_StrCmp(LibNameBuf, LibName, "CFE_ES_GetLibName() = %s", LibNameBuf);
    UtAssert_True(LibInfo.Type == CFE_ES_AppType_LIBRARY, "Lib Info -> Type = %d", (int)LibInfo.Type);
    UtAssert_StrCmp(LibInfo.Name, LibName, "Lib Info -> Name = %s", LibInfo.Name);
    UtAssert_StrCmp(LibInfo.EntryPoint, "CFE_Assert_LibInit", "Lib Info -> EntryPt  = %s", LibInfo.EntryPoint);
    UtAssert_StrCmp(LibInfo.FileName, "/cf/cfe_assert.so", "Lib Info -> FileName = %s", LibInfo.FileName);
    UtAssert_True(LibInfo.StackSize == 0, "Lib Info -> StackSz  = %d", (int)LibInfo.StackSize);

    if (LibInfo.AddressesAreValid)
    {
        UtAssert_True(LibInfo.AddressesAreValid > 0, "Lib Info -> AddrsValid?  = %ld",
                      (unsigned long)LibInfo.AddressesAreValid);
        UtAssert_True(LibInfo.CodeAddress > 0, "Lib Info -> CodeAddress  = %ld", (unsigned long)LibInfo.CodeAddress);
        UtAssert_True(LibInfo.CodeSize > 0, "Lib Info -> CodeSize  = %ld", (unsigned long)LibInfo.CodeSize);
        UtAssert_True(LibInfo.DataAddress > 0, "Lib Info -> DataAddress  = %ld", (unsigned long)LibInfo.DataAddress);
        UtAssert_True(LibInfo.DataSize > 0, "Lib Info -> DataSize  = %ld", (unsigned long)LibInfo.DataSize);
        UtAssert_True(LibInfo.BSSAddress > 0, "Lib Info -> BSSAddress  = %ld", (unsigned long)LibInfo.BSSAddress);
        UtAssert_True(LibInfo.BSSSize > 0, "Lib Info -> BSSSize  = %ld", (unsigned long)LibInfo.BSSSize);
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
    UtAssert_ResourceID_Undifeined(LibInfo.MainTaskId);
    UtAssert_True(LibInfo.ExecutionCounter == 0, "Lib Info -> ExecutionCounter  = %d", (int)LibInfo.ExecutionCounter);
    UtAssert_True(strlen(LibInfo.MainTaskName) == 0, "Lib Info -> Task Name  = %s", LibInfo.MainTaskName);
    UtAssert_True(LibInfo.NumOfChildTasks == 0, "Lib Info -> Child Tasks  = %d", (int)LibInfo.NumOfChildTasks);

    UtAssert_INT32_EQ(CFE_ES_GetLibIDByName(&LibIdByName, InvalidName), CFE_ES_ERR_NAME_NOT_FOUND);
    UtAssert_INT32_EQ(CFE_ES_GetLibInfo(&LibInfo, LibIdByName), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_ES_GetLibIDByName(NULL, LibName), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_GetLibInfo(NULL, LibIdByName), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_GetLibName(LibNameBuf, CFE_ES_LIBID_UNDEFINED, sizeof(LibNameBuf)),
                      CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_ES_GetLibName(NULL, LibIdByName, sizeof(LibNameBuf)), CFE_ES_BAD_ARGUMENT);
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

void TestModuleInfo(void)
{
    CFE_ES_AppInfo_t ModuleInfo;
    CFE_ES_LibId_t   LibIdByName;
    CFE_ES_AppId_t   TestAppId;
    CFE_ES_AppInfo_t LibInfo;
    CFE_ES_AppInfo_t TestAppInfo;
    const char *     LibName = "ASSERT_LIB";

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

int32 ESInfoTestSetup(int32 LibId)
{
    UtTest_Add(TestAppInfo, NULL, NULL, "Test App Info");
    UtTest_Add(TestTaskInfo, NULL, NULL, "Test Task Info");
    UtTest_Add(TestLibInfo, NULL, NULL, "Test Lib Info");
    UtTest_Add(TestResetType, NULL, NULL, "Test Reset Type");
    UtTest_Add(TestModuleInfo, NULL, NULL, "Test Module Info");

    return CFE_SUCCESS;
}
