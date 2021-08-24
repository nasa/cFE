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
**   Functional test of ES Child Tasks APIs
**
**   Demonstration of how to register and use the UT assert functions.
**
*************************************************************************/

/*
 * Includes
 */

#include "cfe_test.h"

void TaskFunction(void)
{
    while (CFE_FT_Global.Count < 200)
    {
        CFE_FT_Global.Count += 1;
        OS_TaskDelay(100);
    }
    return;
}

void TaskExitFunction(void)
{
    while (CFE_FT_Global.Count < 200)
    {
        CFE_FT_Global.Count += 1;
        CFE_ES_ExitChildTask();
    }
    return;
}

void TestCreateChild(void)
{
    UtPrintf("Testing: CFE_ES_CreateChildTask");

    CFE_ES_TaskId_t            TaskId;
    CFE_ES_TaskId_t            TaskId2;
    const char *               TaskName      = "CHILD_TASK_1";
    CFE_ES_StackPointer_t      StackPointer  = CFE_ES_TASK_STACK_ALLOCATE;
    size_t                     StackSize     = CFE_PLATFORM_ES_PERF_CHILD_STACK_SIZE;
    CFE_ES_TaskPriority_Atom_t Priority      = CFE_PLATFORM_ES_PERF_CHILD_PRIORITY;
    uint32                     Flags         = 0;
    int                        ExpectedCount = 5;

    CFE_FT_Global.Count = 0;

    UtAssert_INT32_EQ(CFE_ES_CreateChildTask(&TaskId, TaskName, TaskFunction, StackPointer, StackSize, Priority, Flags),
                      CFE_SUCCESS);
    OS_TaskDelay(500);

    UtAssert_True(ExpectedCount >= CFE_FT_Global.Count - 1 && ExpectedCount <= CFE_FT_Global.Count + 1,
                  "countCopy (%d) == count (%d)", (int)ExpectedCount, (int)CFE_FT_Global.Count);

    UtAssert_INT32_EQ(
        CFE_ES_CreateChildTask(&TaskId2, TaskName, TaskFunction, StackPointer, StackSize, Priority, Flags),
        CFE_STATUS_EXTERNAL_RESOURCE_FAIL);
    UtAssert_INT32_EQ(CFE_ES_DeleteChildTask(TaskId), CFE_SUCCESS);

    UtAssert_INT32_EQ(CFE_ES_CreateChildTask(NULL, TaskName, TaskFunction, StackPointer, StackSize, Priority, Flags),
                      CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_CreateChildTask(&TaskId, NULL, TaskFunction, StackPointer, StackSize, Priority, Flags),
                      CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_CreateChildTask(&TaskId, TaskName, NULL, StackPointer, StackSize, Priority, Flags),
                      CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_CreateChildTask(&TaskId, TaskName, TaskFunction, StackPointer, 0, Priority, Flags),
                      CFE_STATUS_EXTERNAL_RESOURCE_FAIL);
}

void TestChildTaskName(void)
{
    UtPrintf("Testing: CFE_ES_GetTaskIDByName, CFE_ES_GetTaskName");

    CFE_ES_TaskId_t            TaskId;
    const char *               TaskName            = "CHILD_TASK_1";
    const char                 INVALID_TASK_NAME[] = "INVALID_NAME";
    CFE_ES_TaskId_t            TaskIdByName;
    char                       TaskNameBuf[OS_MAX_API_NAME + 4];
    CFE_ES_StackPointer_t      StackPointer = CFE_ES_TASK_STACK_ALLOCATE;
    size_t                     StackSize    = CFE_PLATFORM_ES_PERF_CHILD_STACK_SIZE;
    CFE_ES_TaskPriority_Atom_t Priority     = CFE_PLATFORM_ES_PERF_CHILD_PRIORITY;
    uint32                     Flags        = 0;

    UtAssert_INT32_EQ(CFE_ES_CreateChildTask(&TaskId, TaskName, TaskFunction, StackPointer, StackSize, Priority, Flags),
                      CFE_SUCCESS);

    UtAssert_INT32_EQ(CFE_ES_GetTaskIDByName(&TaskIdByName, TaskName), CFE_SUCCESS);
    CFE_UtAssert_RESOURCEID_EQ(TaskIdByName, TaskId);

    UtAssert_INT32_EQ(CFE_ES_GetTaskName(TaskNameBuf, TaskId, sizeof(TaskNameBuf)), CFE_SUCCESS);
    UtAssert_StrCmp(TaskNameBuf, TaskName, "CFE_ES_GetTaskName() = %s", TaskNameBuf);

    UtAssert_INT32_EQ(CFE_ES_GetTaskIDByName(NULL, TaskName), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_GetTaskIDByName(&TaskIdByName, INVALID_TASK_NAME), CFE_ES_ERR_NAME_NOT_FOUND);
    CFE_UtAssert_RESOURCEID_UNDEFINED(TaskIdByName);

    UtAssert_INT32_EQ(CFE_ES_GetTaskName(NULL, TaskId, sizeof(TaskNameBuf)), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_GetTaskName(TaskNameBuf, CFE_ES_TASKID_UNDEFINED, sizeof(TaskNameBuf)),
                      CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_ES_GetTaskName(TaskNameBuf, TaskId, sizeof(TaskName) - 4), CFE_ES_ERR_RESOURCEID_NOT_VALID);

    UtAssert_INT32_EQ(CFE_ES_DeleteChildTask(TaskId), CFE_SUCCESS);
}

void TestChildTaskDelete(void)
{
    UtPrintf("Testing: CFE_ES_DeleteChildTask");

    CFE_ES_TaskId_t            TaskId;
    const char *               TaskName      = "CHILD_TASK_1";
    CFE_ES_StackPointer_t      StackPointer  = CFE_ES_TASK_STACK_ALLOCATE;
    size_t                     StackSize     = CFE_PLATFORM_ES_PERF_CHILD_STACK_SIZE;
    CFE_ES_TaskPriority_Atom_t Priority      = CFE_PLATFORM_ES_PERF_CHILD_PRIORITY;
    uint32                     Flags         = 0;
    int                        ExpectedCount = 5;

    CFE_FT_Global.Count = 0;

    UtAssert_INT32_EQ(CFE_ES_CreateChildTask(&TaskId, TaskName, TaskFunction, StackPointer, StackSize, Priority, Flags),
                      CFE_SUCCESS);
    OS_TaskDelay(500);

    UtAssert_True(ExpectedCount >= CFE_FT_Global.Count - 1 && ExpectedCount <= CFE_FT_Global.Count + 1,
                  "countCopy (%d) == count (%d)", (int)ExpectedCount, (int)CFE_FT_Global.Count);

    ExpectedCount = CFE_FT_Global.Count;

    UtAssert_INT32_EQ(CFE_ES_DeleteChildTask(TaskId), CFE_SUCCESS);

    OS_TaskDelay(500);

    UtAssert_True(ExpectedCount == CFE_FT_Global.Count || ExpectedCount == CFE_FT_Global.Count + 1,
                  "ExpectedCount (%d) == count (%d)", (int)ExpectedCount, (int)CFE_FT_Global.Count);

    UtAssert_INT32_EQ(CFE_ES_DeleteChildTask(CFE_ES_TASKID_UNDEFINED), CFE_ES_ERR_RESOURCEID_NOT_VALID);
}

void TestExitChild(void)
{
    UtPrintf("Testing: CFE_ES_ExitChildTask");

    CFE_ES_TaskId_t            TaskId;
    const char *               TaskName      = "CHILD_TASK_1";
    CFE_ES_StackPointer_t      StackPointer  = CFE_ES_TASK_STACK_ALLOCATE;
    size_t                     StackSize     = CFE_PLATFORM_ES_PERF_CHILD_STACK_SIZE;
    CFE_ES_TaskPriority_Atom_t Priority      = CFE_PLATFORM_ES_PERF_CHILD_PRIORITY;
    uint32                     Flags         = 0;
    int                        ExpectedCount = 1;

    UtAssert_INT32_EQ(
        CFE_ES_CreateChildTask(&TaskId, TaskName, TaskExitFunction, StackPointer, StackSize, Priority, Flags),
        CFE_SUCCESS);
    OS_TaskDelay(500);
    UtAssert_INT32_EQ(ExpectedCount, 1);

    /*
     * Invoking CFE_ES_ExitChildTask() from the context of a main task should _not_ actually exit.
     * as this is a void function there is no return code to check here.  The fact that the test
     * continues after this call is evidence that the test passed (i.e. it did not actually end the task).
     */
    UtAssert_VOIDCALL(CFE_ES_ExitChildTask());

    /* If this message is printed, then the test passed.  If the test fails this will not be reached. */
    UtAssert_True(true, "CFE_ES_ExitChildTask() called from main task (ignored; main task did not exit)");
}

void ESTaskTestSetup(void)
{
    UtTest_Add(TestCreateChild, NULL, NULL, "Test Create Child");
    UtTest_Add(TestChildTaskName, NULL, NULL, "Test Child Task Name");
    UtTest_Add(TestChildTaskDelete, NULL, NULL, "Test Child Tasks Delete");
    UtTest_Add(TestExitChild, NULL, NULL, "Test Exit Child");
}
