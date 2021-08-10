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

#include <unistd.h>

#include "cfe.h"
#include "cfe_test.h"

typedef struct
{
    CFE_MSG_TelemetryHeader_t Hdr;
    uint32                    Tlm32Param1;
    uint16                    Tlm16Param1;
    uint16                    Tlm16Param2;
    uint8                     Tlm8Param1;
    uint8                     Tlm8Param2;
    uint8                     Tlm8Param3;
    uint8                     Tlm8Param4;
} UT_Test_Tlm_t;

typedef struct ReceiveBuffer_PendForeverPipeDestruction_Task_Data_s
{
    CFE_SB_Buffer_t *SBBufPtr;
    CFE_SB_PipeId_t  PipeId;
    CFE_Status_t     status;
    uint8_t          entered;
    uint8_t          exited;
} ReceiveBuffer_PendForeverPipeDestruction_Task_Data_t;

static ReceiveBuffer_PendForeverPipeDestruction_Task_Data_t ReceiveBuffer_PendForeverPipeDestruction_Task_Data = {
    NULL, 0, CFE_SUCCESS, 0, 0};

void ReceiveBuffer_PendForeverPipeDestruction_Task(void)
{
    ReceiveBuffer_PendForeverPipeDestruction_Task_Data_t *D = &ReceiveBuffer_PendForeverPipeDestruction_Task_Data;

    D->entered = 1;

    // Receive messages while receive is successful
    while (D->status == CFE_SUCCESS)
    {
        D->status = CFE_SB_ReceiveBuffer(&(D->SBBufPtr), D->PipeId, CFE_SB_PEND_FOREVER);
    }

    D->exited = 1;

    CFE_ES_ExitChildTask();
}

/*
** Test destroying a pipe during a "pend forever" timeout
*/
void Test_ReceiveBuffer_PendForeverPipeDestruction(void)
{
    ReceiveBuffer_PendForeverPipeDestruction_Task_Data_t *D     = &ReceiveBuffer_PendForeverPipeDestruction_Task_Data;
    CFE_SB_MsgId_t                                        MsgId = 0x08D5;
    UT_Test_Tlm_t                                         TlmPkt;
    uint32                                                PipeDepth = 10;
    CFE_ES_TaskId_t                                       TaskID;

    /* Initialize message header */
    CFE_MSG_Init(&TlmPkt.Hdr.Msg, MsgId, sizeof(TlmPkt));

    /* Create pipe */
    UtAssert_INT32_EQ(CFE_SUCCESS, CFE_SB_CreatePipe(&(D->PipeId), PipeDepth, "RcvTestPipe"));

    /* Subscribe to the pipe */
    UtAssert_UINT32_EQ(CFE_SUCCESS, CFE_SB_Subscribe(MsgId, D->PipeId));

    /* Send one message */
    UtAssert_UINT32_EQ(CFE_SUCCESS, CFE_SB_TransmitMsg(&TlmPkt.Hdr.Msg, true));

    /* Create a task to service the pipe */
    UtAssert_INT32_EQ(CFE_SUCCESS, CFE_ES_CreateChildTask(
                                       &(TaskID), "PFPD_Task",
                                       (CFE_ES_ChildTaskMainFuncPtr_t)&ReceiveBuffer_PendForeverPipeDestruction_Task,
                                       NULL, CFE_PLATFORM_ES_DEFAULT_STACK_SIZE, 0, 0));

    /* Wait for one second after creating the child task */
    OS_TaskDelay(1000 /* us */);

    /* Delete the pipe (this should cause the child task to break out if its loop) */
    UtAssert_INT32_EQ(CFE_SUCCESS, CFE_SB_DeletePipe(D->PipeId));

    /* Wait for the task to fail */
    OS_TaskDelay(1000 /* us */);

    /* Assert that the child task has broken out of its loop and terminated */
    UtAssert_INT32_EQ(D->entered, 1);
    UtAssert_INT32_EQ(D->exited, 1);
    UtAssert_INT32_NEQ(D->status, CFE_SUCCESS);
}

void SBTaskTestSetup(void)
{
    UtTest_Add(Test_ReceiveBuffer_PendForeverPipeDestruction, NULL, NULL, "Test destroying a pipe while waiting.");
}
