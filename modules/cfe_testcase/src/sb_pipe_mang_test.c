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
 *   Functional test of Sb Pipe Management APIs
 *
 *   Demonstration of how to register and use the UT assert functions.
 */

/*
 * Includes
 */

#include "cfe_test.h"

void TestPipeCreate(void)
{
    CFE_SB_PipeId_t PipeId1    = CFE_SB_INVALID_PIPE;
    uint16          PipeDepth  = 10;
    const char      PipeName[] = "Test Pipe";

    UtPrintf("Testing: CFE_SB_CreatePipe, CFE_SB_DeletePipe");

    UtAssert_INT32_EQ(CFE_SB_CreatePipe(&PipeId1, PipeDepth, PipeName), CFE_SUCCESS);

    UtAssert_INT32_EQ(CFE_SB_DeletePipe(PipeId1), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_DeletePipe(PipeId1), CFE_SB_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_SB_DeletePipe(CFE_SB_INVALID_PIPE), CFE_SB_BAD_ARGUMENT);

    UtAssert_INT32_EQ(CFE_SB_CreatePipe(NULL, PipeDepth, PipeName), CFE_SB_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_SB_CreatePipe(&PipeId1, OS_QUEUE_MAX_DEPTH + 5, PipeName), CFE_SB_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_SB_CreatePipe(&PipeId1, 0, PipeName), CFE_SB_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_SB_CreatePipe(&PipeId1, PipeDepth, NULL), CFE_SB_PIPE_CR_ERR);
    UtAssert_INT32_EQ(CFE_SB_CreatePipe(&PipeId1, PipeDepth, ""), CFE_SB_BAD_ARGUMENT);
}

void TestPipeCreateMax(void)
{
    CFE_SB_PipeId_t PipeId[CFE_PLATFORM_SB_MAX_PIPES + 1];
    char            PipeName[12];
    uint32          NumPipes;

    UtPrintf("Testing: CFE_SB_CreatePipe, maximum pipe limit");

    /*
     * NOTE: because any other running apps (including core apps) will likely have
     * created some pipes already, it is not known how many more pipes can be created
     * at this point.  So this cannot assert directly on the return code of
     * CFE_SB_CreatePipe because we do not know which iteration will return error,
     * but it will be less than CFE_PLATFORM_SB_MAX_PIPES.
     */
    NumPipes = 0;
    while (NumPipes <= CFE_PLATFORM_SB_MAX_PIPES)
    {
        snprintf(PipeName, sizeof(PipeName), "TestPipe%u", (unsigned int)NumPipes);
        CFE_Assert_STATUS_STORE(CFE_SB_CreatePipe(&PipeId[NumPipes], 10, PipeName));
        /*
         * Normally, this will return CFE_SUCCESS, until the max number of pipes is reached.
         * Confirm that the last creation attempt returned CFE_SB_MAX_PIPES_MET
         *
         * NOTE: this also mimics the same format as UtAssert_INT32_EQ so that any post-procesing
         * test log analysis tools will see this call as well.
         */
        if (CFE_Assert_STATUS_MAY_BE(CFE_SB_MAX_PIPES_MET))
        {
            break;
        }

        /* If not CFE_SB_MAX_PIPES_MET, then the only acceptable response is SUCCESS */
        CFE_Assert_STATUS_MUST_BE(CFE_SUCCESS);

        ++NumPipes;
    }

    /* should have gotten CFE_SB_MAX_PIPES_MET before CFE_PLATFORM_SB_MAX_PIPES reached */
    UtAssert_UINT32_LT(NumPipes, CFE_PLATFORM_SB_MAX_PIPES);

    /* Cleanup: delete all pipes created above */
    while (NumPipes > 0)
    {
        --NumPipes;
        UtAssert_INT32_EQ(CFE_SB_DeletePipe(PipeId[NumPipes]), CFE_SUCCESS);
    }
}

void TestPipeIndex(void)
{
    CFE_SB_PipeId_t PipeId     = CFE_SB_INVALID_PIPE;
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
    CFE_SB_PipeId_t PipeId     = CFE_SB_INVALID_PIPE;
    uint16          PipeDepth  = 10;
    const char      PipeName[] = "Test Pipe";
    uint8           Opts       = 2;
    uint8           OptsBuff   = 0;

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
    CFE_SB_PipeId_t PipeId     = CFE_SB_INVALID_PIPE;
    uint16          PipeDepth  = 10;
    const char      PipeName[] = "Test Pipe";
    char            PipeNameBuf[OS_MAX_API_NAME];
    CFE_SB_PipeId_t PipeIdBuff        = CFE_SB_INVALID_PIPE;
    const char      InvalidPipeName[] = "Invalid Pipe";

    UtPrintf("Testing: CFE_SB_GetPipeName, CFE_SB_GetPipeIdByName");

    UtAssert_INT32_EQ(CFE_SB_CreatePipe(&PipeId, PipeDepth, PipeName), CFE_SUCCESS);

    UtAssert_INT32_EQ(CFE_SB_GetPipeName(PipeNameBuf, sizeof(PipeNameBuf), PipeId), CFE_SUCCESS);
    UtAssert_StrCmp(PipeNameBuf, PipeName, "CFE_SB_GetPipeName() = %s", PipeNameBuf);

    UtAssert_INT32_EQ(CFE_SB_GetPipeIdByName(&PipeIdBuff, PipeName), CFE_SUCCESS);
    CFE_Assert_RESOURCEID_EQ(PipeId, PipeIdBuff);

    UtAssert_INT32_EQ(CFE_SB_GetPipeName(NULL, sizeof(PipeNameBuf), PipeId), CFE_SB_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_SB_GetPipeName(PipeNameBuf, 0, PipeId), CFE_SB_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_SB_GetPipeName(PipeNameBuf, sizeof(PipeNameBuf), CFE_SB_INVALID_PIPE), CFE_SB_BAD_ARGUMENT);

    UtAssert_INT32_EQ(CFE_SB_GetPipeIdByName(NULL, PipeName), CFE_SB_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_SB_GetPipeIdByName(&PipeIdBuff, NULL), CFE_SB_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_SB_GetPipeIdByName(&PipeIdBuff, InvalidPipeName), CFE_SB_BAD_ARGUMENT);

    UtAssert_INT32_EQ(CFE_SB_DeletePipe(PipeId), CFE_SUCCESS);
}

void SBPipeMangSetup(void)
{
    UtTest_Add(TestPipeCreate, NULL, NULL, "Test Pipe Create");
    UtTest_Add(TestPipeCreateMax, NULL, NULL, "Test Pipe Create Max Limit");
    UtTest_Add(TestPipeIndex, NULL, NULL, "Test Pipe Index");
    UtTest_Add(TestPipeOptions, NULL, NULL, "Test Pipe Options");
    UtTest_Add(TestPipeName, NULL, NULL, "Test Pipe Name");
}
