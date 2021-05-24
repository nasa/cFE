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
** File: es_task_test.c
**
** Purpose:
**   Functional test of basic ES Child Tasks APIs
**
**   Demonstration of how to register and use the UT assert functions.
**
*************************************************************************/

/*
 * Includes
 */

#include "cfe_test.h"

uint32 count = 0;

void TaskFunction(void)
{
    while (count < 200)
    {
        count += 1;
        OS_TaskDelay(100);
    }
    return;
}

void TaskExitFunction(void)
{
    while (count < 200)
    {
        count += 1;
        CFE_ES_ExitChildTask();
    }
    return;
}

void TestCreateChild(void)
{
    UtPrintf("Testing: CFE_ES_CreateChildTask, CFE_ES_GetTaskIDByName, CFE_ES_GetTaskName, CFE_ES_DeleteChildTask");

    CFE_ES_TaskId_t            TaskId;
    const char *               TaskName = "CHILD_TASK_1";
    CFE_ES_TaskId_t            TaskIdByName;
    char                       TaskNameBuf[OS_MAX_API_NAME + 4];
    CFE_ES_StackPointer_t      StackPointer = CFE_ES_TASK_STACK_ALLOCATE;
    size_t                     StackSize    = CFE_PLATFORM_ES_PERF_CHILD_STACK_SIZE;
    CFE_ES_TaskPriority_Atom_t Priority     = CFE_PLATFORM_ES_PERF_CHILD_PRIORITY;
    uint32                     Flags        = 0;
    int                        countCopy;

    UtAssert_INT32_EQ(CFE_ES_CreateChildTask(&TaskId, TaskName, TaskFunction, StackPointer, StackSize, Priority, Flags),
                      CFE_SUCCESS);

    UtAssert_INT32_EQ(CFE_ES_GetTaskIDByName(&TaskIdByName, TaskName), CFE_SUCCESS);
    UtAssert_ResourceID_EQ(TaskIdByName, TaskId);

    UtAssert_INT32_EQ(CFE_ES_GetTaskName(TaskNameBuf, TaskId, sizeof(TaskNameBuf)), CFE_SUCCESS);
    UtAssert_StrCmp(TaskNameBuf, TaskName, "CFE_ES_GetTaskName() = %s", TaskNameBuf);

    OS_TaskDelay(500);

    countCopy = count;

    UtAssert_INT32_EQ(CFE_ES_DeleteChildTask(TaskId), CFE_SUCCESS);

    OS_TaskDelay(500);

    UtAssert_True(countCopy == count || countCopy == count + 1, "countCopy (%d) == count (%d)", (int)countCopy,
                  (int)count);
}

void TestExitChild(void)
{
    UtPrintf("Testing: CFE_ES_ExitChildTask");

    CFE_ES_TaskId_t            TaskId;
    const char *               TaskName     = "CHILD_TASK_1";
    CFE_ES_StackPointer_t      StackPointer = CFE_ES_TASK_STACK_ALLOCATE;
    size_t                     StackSize    = CFE_PLATFORM_ES_PERF_CHILD_STACK_SIZE;
    CFE_ES_TaskPriority_Atom_t Priority     = CFE_PLATFORM_ES_PERF_CHILD_PRIORITY;
    uint32                     Flags        = 0;
    count                                   = 0;

    UtAssert_INT32_EQ(
        CFE_ES_CreateChildTask(&TaskId, TaskName, TaskExitFunction, StackPointer, StackSize, Priority, Flags),
        CFE_SUCCESS);
    OS_TaskDelay(500);
    UtAssert_INT32_EQ(count, 1);
}

void ESTaskTestSetup(void)
{
    UtTest_Add(TestCreateChild, NULL, NULL, "Test Create Child");
    UtTest_Add(TestExitChild, NULL, NULL, "Test Exit Child");
}
