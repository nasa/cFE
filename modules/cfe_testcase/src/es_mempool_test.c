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
 *   Functional test of basic ES Mempool APIs
 *
 *   Demonstration of how to register and use the UT assert functions.
 */

/*
 * Includes
 */

#include "cfe_test.h"

typedef struct
{
    uint32 Mem[128];
} CFE_FT_PoolMemBlock_t;

static CFE_FT_PoolMemBlock_t CFE_FT_PoolMemBlock[CFE_PLATFORM_ES_MAX_MEMORY_POOLS + 1];

void TestMemPoolCreate(void)
{
    CFE_ES_MemHandle_t PoolID = CFE_ES_MEMHANDLE_UNDEFINED;

    UtPrintf("Testing: CFE_ES_PoolCreateNoSem, CFE_ES_PoolCreate, CFE_ES_PoolCreateEx");

    UtAssert_INT32_EQ(CFE_ES_PoolCreateNoSem(&PoolID, CFE_FT_PoolMemBlock, sizeof(CFE_FT_PoolMemBlock)), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_PoolCreateNoSem(NULL, CFE_FT_PoolMemBlock, sizeof(CFE_FT_PoolMemBlock)),
                      CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_PoolCreateNoSem(&PoolID, NULL, sizeof(CFE_FT_PoolMemBlock)), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_PoolCreateNoSem(&PoolID, CFE_FT_PoolMemBlock, 0), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_PoolDelete(PoolID), CFE_SUCCESS);

    UtAssert_INT32_EQ(CFE_ES_PoolCreate(&PoolID, CFE_FT_PoolMemBlock, sizeof(CFE_FT_PoolMemBlock)), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_PoolCreate(NULL, CFE_FT_PoolMemBlock, sizeof(CFE_FT_PoolMemBlock)), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_PoolCreate(&PoolID, NULL, sizeof(CFE_FT_PoolMemBlock)), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_PoolCreate(&PoolID, CFE_FT_PoolMemBlock, 0), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_PoolDelete(PoolID), CFE_SUCCESS);

    UtAssert_INT32_EQ(
        CFE_ES_PoolCreateEx(&PoolID, CFE_FT_PoolMemBlock, sizeof(CFE_FT_PoolMemBlock), 0, NULL, CFE_ES_NO_MUTEX),
        CFE_SUCCESS);
    UtAssert_INT32_EQ(
        CFE_ES_PoolCreateEx(NULL, CFE_FT_PoolMemBlock, sizeof(CFE_FT_PoolMemBlock), 0, NULL, CFE_ES_NO_MUTEX),
        CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_PoolCreateEx(&PoolID, NULL, sizeof(CFE_FT_PoolMemBlock), 0, NULL, CFE_ES_NO_MUTEX),
                      CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_PoolCreateEx(&PoolID, CFE_FT_PoolMemBlock, 0, 0, NULL, CFE_ES_NO_MUTEX),
                      CFE_ES_BAD_ARGUMENT);

    UtAssert_INT32_EQ(CFE_ES_PoolDelete(PoolID), CFE_SUCCESS);
}

void TestMemPoolCreateMax(void)
{
    CFE_ES_MemHandle_t PoolID[CFE_PLATFORM_ES_MAX_MEMORY_POOLS + 1];
    uint32             NumPools;

    UtPrintf("Testing: CFE_ES_PoolCreate Max Limit");

    NumPools = 0;
    while (NumPools <= CFE_PLATFORM_ES_MAX_MEMORY_POOLS)
    {
        CFE_Assert_STATUS_STORE(CFE_ES_PoolCreateEx(&PoolID[NumPools], &CFE_FT_PoolMemBlock[NumPools],
                                                    sizeof(CFE_FT_PoolMemBlock_t), 0, NULL, CFE_ES_NO_MUTEX));
        if (CFE_Assert_STATUS_MAY_BE(CFE_ES_NO_RESOURCE_IDS_AVAILABLE))
        {
            /* limit reached */
            break;
        }
        CFE_Assert_STATUS_MUST_BE(CFE_SUCCESS);
        ++NumPools;
    }

    UtAssert_UINT32_LTEQ(NumPools, CFE_PLATFORM_ES_MAX_MEMORY_POOLS);

    /* Clean up */
    while (NumPools > 0)
    {
        --NumPools;
        UtAssert_INT32_EQ(CFE_ES_PoolDelete(PoolID[NumPools]), CFE_SUCCESS);
    }
}

void TestMemPoolGetBuf(void)
{
    CFE_ES_MemHandle_t  PoolID = CFE_ES_MEMHANDLE_UNDEFINED;
    int8                Pool[1024];
    size_t              BufferSize = 512;
    size_t              BufferBig  = 2048;
    CFE_ES_MemPoolBuf_t addressp   = CFE_ES_MEMPOOLBUF_C(0);

    UtPrintf("Testing: TestMemPoolGetBuf");

    UtAssert_INT32_EQ(CFE_ES_PoolCreate(&PoolID, Pool, sizeof(Pool)), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_GetPoolBuf(&addressp, PoolID, BufferSize), BufferSize);

    UtAssert_INT32_EQ(CFE_ES_GetPoolBuf(NULL, PoolID, BufferSize), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_GetPoolBuf(&addressp, CFE_ES_MEMHANDLE_UNDEFINED, BufferSize),
                      CFE_ES_ERR_RESOURCEID_NOT_VALID);

    UtAssert_INT32_EQ(CFE_ES_GetPoolBuf(&addressp, PoolID, BufferSize), CFE_ES_ERR_MEM_BLOCK_SIZE);
    UtAssert_INT32_EQ(CFE_ES_PutPoolBuf(PoolID, addressp), BufferSize);

    UtAssert_INT32_EQ(CFE_ES_GetPoolBuf(&addressp, PoolID, BufferBig), CFE_ES_ERR_MEM_BLOCK_SIZE);

    UtAssert_INT32_EQ(CFE_ES_PoolDelete(PoolID), CFE_SUCCESS);
}

void TestMemPoolBufInfo(void)
{
    CFE_ES_MemHandle_t  PoolID = CFE_ES_MEMHANDLE_UNDEFINED;
    int8                Pool[1024];
    size_t              Buffer   = 512;
    CFE_ES_MemPoolBuf_t addressp = CFE_ES_MEMPOOLBUF_C(0);

    UtPrintf("Testing: CFE_ES_GetPoolBufInfo");

    UtAssert_INT32_EQ(CFE_ES_PoolCreate(&PoolID, Pool, sizeof(Pool)), CFE_SUCCESS);

    UtAssert_INT32_EQ(CFE_ES_GetPoolBufInfo(PoolID, addressp), CFE_ES_BAD_ARGUMENT);

    UtAssert_INT32_EQ(CFE_ES_GetPoolBuf(&addressp, PoolID, Buffer), Buffer);
    UtAssert_INT32_EQ(CFE_ES_GetPoolBufInfo(PoolID, addressp), Buffer);

    UtAssert_INT32_EQ(CFE_ES_GetPoolBufInfo(CFE_ES_MEMHANDLE_UNDEFINED, addressp), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_ES_GetPoolBufInfo(PoolID, NULL), CFE_ES_BAD_ARGUMENT);

    /* Pass an address from some other memory which is not part of the pool */
    UtAssert_INT32_EQ(CFE_ES_GetPoolBufInfo(PoolID, &Buffer), CFE_ES_BUFFER_NOT_IN_POOL);

    UtAssert_INT32_EQ(CFE_ES_PoolDelete(PoolID), CFE_SUCCESS);
}

void TestMemPoolPutBuf(void)
{
    CFE_ES_MemHandle_t  PoolID = CFE_ES_MEMHANDLE_UNDEFINED;
    int8                Pool[1024];
    size_t              Buffer   = 512;
    CFE_ES_MemPoolBuf_t addressp = CFE_ES_MEMPOOLBUF_C(0);

    UtPrintf("Testing: CFE_ES_PutPoolBuf");

    UtAssert_INT32_EQ(CFE_ES_PoolCreate(&PoolID, Pool, sizeof(Pool)), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_GetPoolBuf(&addressp, PoolID, Buffer), Buffer);
    UtAssert_INT32_EQ(CFE_ES_PutPoolBuf(PoolID, addressp), Buffer);

    UtAssert_INT32_EQ(CFE_ES_PutPoolBuf(PoolID, addressp), CFE_ES_POOL_BLOCK_INVALID);
    UtAssert_INT32_EQ(CFE_ES_PutPoolBuf(CFE_ES_MEMHANDLE_UNDEFINED, addressp), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_ES_PutPoolBuf(PoolID, NULL), CFE_ES_BAD_ARGUMENT);

    UtAssert_INT32_EQ(CFE_ES_GetPoolBuf(&addressp, PoolID, Buffer), Buffer);

    UtAssert_INT32_EQ(CFE_ES_PoolDelete(PoolID), CFE_SUCCESS);
}

void TestMemPoolDelete(void)
{
    CFE_ES_MemHandle_t    PoolID = CFE_ES_MEMHANDLE_UNDEFINED; /* Poo1 1 handle, no mutex */
    uint8                 Buffer[1024];
    CFE_ES_MemPoolStats_t Stats;

    memset(&Stats, 0, sizeof(Stats));

    UtPrintf("Testing: CFE_ES_PoolDelete, CFE_ES_GetMemPoolStats, CFE_ES_PoolCreateEx");

    UtAssert_INT32_EQ(CFE_ES_PoolCreateEx(&PoolID, Buffer, sizeof(Buffer), 0, NULL, CFE_ES_NO_MUTEX), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_GetMemPoolStats(&Stats, PoolID), CFE_SUCCESS);

    UtAssert_EQ(size_t, CFE_ES_MEMOFFSET_TO_SIZET(Stats.PoolSize), sizeof(Buffer));
    UtAssert_UINT32_EQ(Stats.NumBlocksRequested, 0);
    UtAssert_UINT32_EQ(Stats.CheckErrCtr, 0);
    UtAssert_EQ(size_t, CFE_ES_MEMOFFSET_TO_SIZET(Stats.NumFreeBytes), sizeof(Buffer));

    UtAssert_INT32_EQ(CFE_ES_GetMemPoolStats(NULL, PoolID), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_GetMemPoolStats(&Stats, CFE_ES_MEMHANDLE_UNDEFINED), CFE_ES_ERR_RESOURCEID_NOT_VALID);

    UtAssert_INT32_EQ(CFE_ES_PoolDelete(PoolID), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_GetMemPoolStats(&Stats, PoolID), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_ES_PoolDelete(PoolID), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_ES_PoolDelete(CFE_ES_MEMHANDLE_UNDEFINED), CFE_ES_ERR_RESOURCEID_NOT_VALID);
}

void ESMemPoolTestSetup(void)
{
    UtTest_Add(TestMemPoolCreate, NULL, NULL, "Test Mem Pool Create");
    UtTest_Add(TestMemPoolCreateMax, NULL, NULL, "Test Mem Pool Create Maximum");
    UtTest_Add(TestMemPoolGetBuf, NULL, NULL, "Test Mem Pool Get Buf");
    UtTest_Add(TestMemPoolBufInfo, NULL, NULL, "Test Mem Pool Buf Info");
    UtTest_Add(TestMemPoolPutBuf, NULL, NULL, "Test Mem Pool Put Buf");
    UtTest_Add(TestMemPoolDelete, NULL, NULL, "Test Mem Pool Delete");
}
