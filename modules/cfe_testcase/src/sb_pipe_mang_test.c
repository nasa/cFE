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
** File: sb_pipe_mang_test.c
**
** Purpose:
**   Functional test of Sb Pipe Management APIs
**
**   Demonstration of how to register and use the UT assert functions.
**
*************************************************************************/

/*
 * Includes
 */

#include "cfe_test.h"

void TestPipeCreate(void)
{
    CFE_SB_PipeId_t PipeId1;
    uint16          PipeDepth  = 10;
    const char      PipeName[] = "Test Pipe";

    UtPrintf("Testing: CFE_SB_CreatePipe, CFE_SB_DeletePipe");

    UtAssert_INT32_EQ(CFE_SB_CreatePipe(&PipeId1, PipeDepth, PipeName), CFE_SUCCESS);

    UtAssert_INT32_EQ(CFE_SB_CreatePipe(NULL, PipeDepth, PipeName), CFE_SB_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_SB_CreatePipe(&PipeId1, OS_QUEUE_MAX_DEPTH + 5, PipeName), CFE_SB_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_SB_CreatePipe(&PipeId1, 0, PipeName), CFE_SB_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_SB_CreatePipe(&PipeId1, PipeDepth, NULL), CFE_SB_PIPE_CR_ERR);

    UtAssert_INT32_EQ(CFE_SB_DeletePipe(PipeId1), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_DeletePipe(PipeId1), CFE_SB_BAD_ARGUMENT);
}

void TestPipeIndex(void)
{
    CFE_SB_PipeId_t PipeId;
    uint16          PipeDepth  = 10;
    const char      PipeName[] = "Test Pipe";
    uint32          Idx;

    UtPrintf("Testing: CFE_SB_PipeId_ToIndex");

    UtAssert_INT32_EQ(CFE_SB_CreatePipe(&PipeId, PipeDepth, PipeName), CFE_SUCCESS);

    UtAssert_INT32_EQ(CFE_SB_PipeId_ToIndex(PipeId, &Idx), CFE_SUCCESS);

    UtAssert_INT32_EQ(CFE_SB_PipeId_ToIndex(CFE_SB_INVALID_PIPE, &Idx), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_SB_PipeId_ToIndex(PipeId, NULL), CFE_ES_BAD_ARGUMENT);

    UtAssert_INT32_EQ(CFE_SB_DeletePipe(PipeId), CFE_SUCCESS);
}

void TestPipeOptions(void)
{
    CFE_SB_PipeId_t PipeId;
    uint16          PipeDepth  = 10;
    const char      PipeName[] = "Test Pipe";
    uint8           Opts       = 2;
    uint8           OptsBuff;

    UtPrintf("Testing: CFE_SB_SetPipeOpts, CFE_SB_GetPipeOpts");

    UtAssert_INT32_EQ(CFE_SB_CreatePipe(&PipeId, PipeDepth, PipeName), CFE_SUCCESS);

    UtAssert_INT32_EQ(CFE_SB_SetPipeOpts(PipeId, Opts), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_GetPipeOpts(PipeId, &OptsBuff), CFE_SUCCESS);
    UtAssert_UINT32_EQ(Opts, OptsBuff);

    UtAssert_INT32_EQ(CFE_SB_SetPipeOpts(CFE_SB_INVALID_PIPE, Opts), CFE_SB_BAD_ARGUMENT);

    UtAssert_INT32_EQ(CFE_SB_GetPipeOpts(CFE_SB_INVALID_PIPE, &OptsBuff), CFE_SB_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_SB_GetPipeOpts(PipeId, NULL), CFE_SB_BAD_ARGUMENT);

    UtAssert_INT32_EQ(CFE_SB_DeletePipe(PipeId), CFE_SUCCESS);
}

void TestPipeName(void)
{
    CFE_SB_PipeId_t PipeId;
    uint16          PipeDepth  = 10;
    const char      PipeName[] = "Test Pipe";
    char            PipeNameBuf[OS_MAX_API_NAME];
    CFE_SB_PipeId_t PipeIdBuff;
    const char      InvalidPipeName[] = "Invalid Pipe";

    UtPrintf("Testing: CFE_SB_GetPipeName, CFE_SB_GetPipeIdByName");

    UtAssert_INT32_EQ(CFE_SB_CreatePipe(&PipeId, PipeDepth, PipeName), CFE_SUCCESS);

    UtAssert_INT32_EQ(CFE_SB_GetPipeName(PipeNameBuf, sizeof(PipeNameBuf), PipeId), CFE_SUCCESS);
    UtAssert_StrCmp(PipeNameBuf, PipeName, "CFE_SB_GetPipeName() = %s", PipeNameBuf);

    UtAssert_INT32_EQ(CFE_SB_GetPipeIdByName(&PipeIdBuff, PipeName), CFE_SUCCESS);
    CFE_UtAssert_RESOURCEID_EQ(PipeId, PipeIdBuff);

    UtAssert_INT32_EQ(CFE_SB_GetPipeName(NULL, sizeof(PipeNameBuf), PipeId), CFE_SB_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_SB_GetPipeName(PipeNameBuf, 0, PipeId), CFE_SB_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_SB_GetPipeName(PipeNameBuf, sizeof(PipeNameBuf), CFE_SB_INVALID_PIPE), CFE_SB_BAD_ARGUMENT);

    UtAssert_INT32_EQ(CFE_SB_GetPipeIdByName(NULL, PipeName), CFE_SB_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_SB_GetPipeIdByName(&PipeIdBuff, InvalidPipeName), CFE_SB_BAD_ARGUMENT);

    UtAssert_INT32_EQ(CFE_SB_DeletePipe(PipeId), CFE_SUCCESS);
}

void SBPipeMangSetup(void)
{
    UtTest_Add(TestPipeCreate, NULL, NULL, "Test Pipe Create");
    UtTest_Add(TestPipeIndex, NULL, NULL, "Test Pipe Index");
    UtTest_Add(TestPipeOptions, NULL, NULL, "Test Pipe Options");
    UtTest_Add(TestPipeName, NULL, NULL, "Test Pipe Name");
}
