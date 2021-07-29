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
** File: es_mempool_test.c
**
** Purpose:
**   Functional test of basic ES Mempool APIs
**
**   Demonstration of how to register and use the UT assert functions.
**
*************************************************************************/

/*
 * Includes
 */

#include "cfe_test.h"

void TestMemPoolCreate(void)
{
    CFE_ES_MemHandle_t PoolID;
    int8               Pool[1024];

    UtPrintf("Testing: CFE_ES_PoolCreateNoSem, CFE_ES_PoolCreate, CFE_ES_PoolCreateEx");

    UtAssert_INT32_EQ(CFE_ES_PoolCreateNoSem(&PoolID, Pool, sizeof(Pool)), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_PoolCreateNoSem(NULL, Pool, sizeof(Pool)), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_PoolCreateNoSem(&PoolID, NULL, sizeof(Pool)), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_PoolCreateNoSem(&PoolID, Pool, 0), CFE_ES_BAD_ARGUMENT);

    UtAssert_INT32_EQ(CFE_ES_PoolCreate(&PoolID, Pool, sizeof(Pool)), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_PoolCreate(NULL, Pool, sizeof(Pool)), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_PoolCreate(&PoolID, NULL, sizeof(Pool)), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_PoolCreate(&PoolID, Pool, 0), CFE_ES_BAD_ARGUMENT);

    UtAssert_INT32_EQ(CFE_ES_PoolCreateEx(&PoolID, Pool, sizeof(Pool), 0, NULL, CFE_ES_NO_MUTEX), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_PoolCreateEx(NULL, Pool, sizeof(Pool), 0, NULL, CFE_ES_NO_MUTEX), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_PoolCreateEx(&PoolID, NULL, sizeof(Pool), 0, NULL, CFE_ES_NO_MUTEX), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_PoolCreateEx(&PoolID, Pool, 0, 0, NULL, CFE_ES_NO_MUTEX), CFE_ES_BAD_ARGUMENT);

    UtAssert_INT32_EQ(CFE_ES_PoolDelete(PoolID), CFE_SUCCESS);
}

void TestMemPoolGetBuf(void)
{
    CFE_ES_MemHandle_t  PoolID;
    int8                Pool[1024];
    size_t              Buffer    = 512;
    size_t              BufferBig = 2048;
    CFE_ES_MemPoolBuf_t addressp  = CFE_ES_MEMPOOLBUF_C(0);

    UtPrintf("Testing: TestMemPoolGetBuf");

    UtAssert_INT32_EQ(CFE_ES_PoolCreate(&PoolID, Pool, sizeof(Pool)), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_GetPoolBuf(&addressp, PoolID, Buffer), Buffer);

    UtAssert_INT32_EQ(CFE_ES_GetPoolBuf(NULL, PoolID, Buffer), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_GetPoolBuf(&addressp, CFE_ES_MEMHANDLE_UNDEFINED, Buffer),
                      CFE_ES_ERR_RESOURCEID_NOT_VALID);

    UtAssert_INT32_EQ(CFE_ES_GetPoolBuf(&addressp, PoolID, Buffer), CFE_ES_ERR_MEM_BLOCK_SIZE);
    UtAssert_INT32_EQ(CFE_ES_PutPoolBuf(PoolID, addressp), Buffer);

    UtAssert_INT32_EQ(CFE_ES_GetPoolBuf(&addressp, PoolID, BufferBig), CFE_ES_ERR_MEM_BLOCK_SIZE);

    UtAssert_INT32_EQ(CFE_ES_PoolDelete(PoolID), CFE_SUCCESS);
}

void TestMemPoolBufInfo(void)
{
    CFE_ES_MemHandle_t  PoolID;
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

    UtAssert_INT32_EQ(CFE_ES_PoolDelete(PoolID), CFE_SUCCESS);
}

void TestMemPoolPutBuf(void)
{
    CFE_ES_MemHandle_t  PoolID;
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
    CFE_ES_MemHandle_t    PoolID; /* Poo1 1 handle, no mutex */
    uint8                 Buffer[1024];
    CFE_ES_MemPoolStats_t Stats;

    UtPrintf("Testing: CFE_ES_PoolDelete, CFE_ES_GetMemPoolStats, CFE_ES_PoolCreateEx");

    UtAssert_INT32_EQ(CFE_ES_PoolCreateEx(&PoolID, Buffer, sizeof(Buffer), 0, NULL, CFE_ES_NO_MUTEX), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_GetMemPoolStats(&Stats, PoolID), CFE_SUCCESS);

    UtAssert_UINT32_EQ(Stats.PoolSize, sizeof(Buffer));
    UtAssert_UINT32_EQ(Stats.NumBlocksRequested, 0);
    UtAssert_UINT32_EQ(Stats.CheckErrCtr, 0);
    UtAssert_UINT32_EQ(Stats.NumFreeBytes, sizeof(Buffer));

    UtAssert_INT32_EQ(CFE_ES_PoolDelete(PoolID), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_GetMemPoolStats(&Stats, PoolID), CFE_ES_ERR_RESOURCEID_NOT_VALID);
}

void ESMemPoolTestSetup(void)
{
    UtTest_Add(TestMemPoolCreate, NULL, NULL, "Test Mem Pool Create");
    UtTest_Add(TestMemPoolGetBuf, NULL, NULL, "Test Mem Pool Get Buf");
    UtTest_Add(TestMemPoolBufInfo, NULL, NULL, "Test Mem Pool Buf Info");
    UtTest_Add(TestMemPoolPutBuf, NULL, NULL, "Test Mem Pool Put Buf");
    UtTest_Add(TestMemPoolDelete, NULL, NULL, "Test Mem Pool Delete");
}
