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
**   Functional test of basic ES Mempool APIs
**
**   Demonstration of how to register and use the UT assert functions.
**
*************************************************************************/

/*
 * Includes
 */

#include "cfe_test.h"

void TestMemPool(void)
{
    CFE_ES_MemHandle_t  PoolID1; /* Poo1 1 handle, no mutex */
    CFE_ES_MemHandle_t  PoolID2; /* Poo1 2 handle, with mutex */
    size_t              Buffer1 = 256;
    size_t              Buffer2 = 512;
    int8                Pool1[512];
    int8                Pool2[1024];
    CFE_ES_MemPoolBuf_t addressp1 = CFE_ES_MEMPOOLBUF_C(0); /* Pool 1 buffer address */
    CFE_ES_MemPoolBuf_t addressp2 = CFE_ES_MEMPOOLBUF_C(0); /* Pool 2 buffer address */

    UtPrintf("Testing: CFE_ES_PoolCreateNoSem, CFE_ES_PoolCreate, CFE_ES_GetPoolBuf, CFE_ES_GetPoolBufInfo, "
             "CFE_ES_PutPoolBuf");

    UtAssert_INT32_EQ(CFE_ES_PoolCreateNoSem(&PoolID1, Pool1, sizeof(Pool1)), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_PoolCreate(&PoolID2, Pool2, sizeof(Pool2)), CFE_SUCCESS);

    UtAssert_UINT32_EQ(CFE_ES_GetPoolBuf(&addressp1, PoolID1, Buffer1), Buffer1);
    UtAssert_UINT32_EQ(CFE_ES_GetPoolBuf(&addressp2, PoolID2, Buffer2), Buffer2);

    UtAssert_UINT32_EQ(CFE_ES_GetPoolBufInfo(PoolID1, addressp1), Buffer1);
    UtAssert_UINT32_EQ(CFE_ES_GetPoolBufInfo(PoolID2, addressp2), Buffer2);

    UtAssert_UINT32_EQ(CFE_ES_PutPoolBuf(PoolID1, addressp1), Buffer1);
    UtAssert_UINT32_EQ(CFE_ES_PutPoolBuf(PoolID2, addressp2), Buffer2);

    UtAssert_UINT32_EQ(CFE_ES_GetPoolBuf(&addressp1, PoolID1, Buffer1), Buffer1);
    UtAssert_UINT32_EQ(CFE_ES_GetPoolBuf(&addressp2, PoolID2, Buffer2), Buffer2);

    UtAssert_INT32_EQ(CFE_ES_PoolDelete(PoolID1), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_PoolDelete(PoolID2), CFE_SUCCESS);
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
    UtTest_Add(TestMemPool, NULL, NULL, "Test Mem Pool");
    UtTest_Add(TestMemPoolDelete, NULL, NULL, "Test Mem Pool Delete");
}
