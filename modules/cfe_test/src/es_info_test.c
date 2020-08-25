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
** File: es_test.c
**
** Purpose:
**   Functional test of basic ES APIs
**
**   Demonstration of how to register and use the UT assert functions.
**
*************************************************************************/

/*
 * Includes
 */

#include "cfe_test.h"

void ES_Test_InfoApi(void)
{
    uint32 AppId;
    uint32 AppId2;
    char   AppNameBuf[OS_MAX_API_NAME + 4];
    uint32 resetSubType;
    CFE_ES_AppInfo_t appInfo;
    CFE_ES_TaskInfo_t taskInfo;

    UtAssert_INT32_EQ(CFE_ES_GetAppID(&AppId), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_GetAppName(AppNameBuf, AppId, sizeof(AppNameBuf)), CFE_SUCCESS);
    UtAssert_StrCmp(AppNameBuf, "ASSERT_APP", "CFE_ES_GetAppName() returned ASSERT_APP");
    UtAssert_INT32_EQ(CFE_ES_GetAppIDByName(&AppId2, AppNameBuf), CFE_SUCCESS);
    UtAssert_INT32_EQ(AppId, AppId2);

    UtAssert_INT32_EQ(CFE_ES_GetResetType(&resetSubType),CFE_PSP_RST_TYPE_POWERON);
    UtAssert_INT32_EQ(resetSubType,CFE_PSP_RST_SUBTYPE_POWER_CYCLE);

    UtAssert_INT32_EQ(CFE_ES_GetAppInfo(&appInfo, AppId), CFE_SUCCESS);
    UtPrintf ("App Info -> Type = %d\n",appInfo.Type);
    UtPrintf ("App Info -> Name = %s\n",appInfo.Name);
    UtPrintf ("App Info -> EntryPt  = %s\n",appInfo.EntryPoint);
    UtPrintf ("App Info -> FileName = %s\n",appInfo.FileName);
    UtPrintf ("App Info -> StackSz  = %d\n",appInfo.StackSize);
    UtPrintf ("App Info -> ModuleId = %d\n",appInfo.ModuleId);
    UtPrintf ("App Info -> AddrsValid? = %d\n",appInfo.AddressesAreValid);
    UtPrintf ("App Info -> CodeAddress = %d\n",appInfo.CodeAddress);
    UtPrintf ("App Info -> CodeSize  = %d\n",appInfo.CodeSize);
    UtPrintf ("App Info -> DataAddress = %d\n",appInfo.DataAddress);
    UtPrintf ("App Info -> DataSize  = %d\n",appInfo.DataSize);
    UtPrintf ("App Info -> BSSAddress = 0x%4x\n",appInfo.BSSAddress);
    UtPrintf ("App Info -> BSSSize  = %d\n",appInfo.BSSSize);
    UtPrintf ("App Info -> StartAddress = 0x%4x\n",appInfo.StartAddress);
    UtPrintf ("App Info -> ExceptionAction = %d\n",appInfo.ExceptionAction);
    UtPrintf ("App Info -> Priority = %d\n",appInfo.Priority);
    UtPrintf ("App Info -> MainTaskID = %d\n",appInfo.MainTaskId);
    UtPrintf ("App Info -> ExecutionCtr = %d\n",appInfo.ExecutionCounter);
    UtPrintf ("App Info -> Task Name = %s\n",appInfo.MainTaskName);
    UtPrintf ("App Info -> Child Tasks = %d\n",appInfo.NumOfChildTasks);

    UtAssert_INT32_EQ(CFE_ES_GetTaskInfo(&taskInfo, appInfo.MainTaskId), CFE_SUCCESS);
    UtPrintf ("Task Info -> TaskId = %d\n",taskInfo.TaskId);
    UtPrintf ("Task Info -> ExeCtr = %d\n",taskInfo.ExecutionCounter);
    UtPrintf ("Task Info -> Task Name = %s\n",taskInfo.TaskName);
    UtPrintf ("Task Info -> AppId = %d\n",taskInfo.AppId);
    UtPrintf ("Task Info -> App Name  = %s\n",taskInfo.AppName);
}
