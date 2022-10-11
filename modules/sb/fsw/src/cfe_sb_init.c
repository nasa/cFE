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

/******************************************************************************
** File: cfe_sb_init.c
**
** Purpose:
**      This file contains the source code for the SB Initialization.
**
** Author:   R.McGraw/SSI
**
******************************************************************************/

/*
**  Include Files
*/

#include "cfe_sb_module_all.h"

#include <string.h>

/*
**  External Declarations
*/

const size_t CFE_SB_MemPoolDefSize[CFE_PLATFORM_ES_POOL_MAX_BUCKETS] = {
    CFE_PLATFORM_SB_MAX_BLOCK_SIZE,    CFE_PLATFORM_SB_MEM_BLOCK_SIZE_16, CFE_PLATFORM_SB_MEM_BLOCK_SIZE_15,
    CFE_PLATFORM_SB_MEM_BLOCK_SIZE_14, CFE_PLATFORM_SB_MEM_BLOCK_SIZE_13, CFE_PLATFORM_SB_MEM_BLOCK_SIZE_12,
    CFE_PLATFORM_SB_MEM_BLOCK_SIZE_11, CFE_PLATFORM_SB_MEM_BLOCK_SIZE_10, CFE_PLATFORM_SB_MEM_BLOCK_SIZE_09,
    CFE_PLATFORM_SB_MEM_BLOCK_SIZE_08, CFE_PLATFORM_SB_MEM_BLOCK_SIZE_07, CFE_PLATFORM_SB_MEM_BLOCK_SIZE_06,
    CFE_PLATFORM_SB_MEM_BLOCK_SIZE_05, CFE_PLATFORM_SB_MEM_BLOCK_SIZE_04, CFE_PLATFORM_SB_MEM_BLOCK_SIZE_03,
    CFE_PLATFORM_SB_MEM_BLOCK_SIZE_02, CFE_PLATFORM_SB_MEM_BLOCK_SIZE_01};

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_SB_EarlyInit(void)
{
    int32 OsStatus;
    int32 Stat;

    /* Clear task global */
    memset(&CFE_SB_Global, 0, sizeof(CFE_SB_Global));

    OsStatus = OS_MutSemCreate(&CFE_SB_Global.SharedDataMutexId, "CFE_SB_DataMutex", 0);
    if (OsStatus != OS_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: Shared data mutex creation failed! RC=%ld\n", __func__, (long)OsStatus);
        return CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
    }

    /* Initialize the state of subscription reporting */
    CFE_SB_Global.SubscriptionReporting = CFE_SB_DISABLE;

    /* Initialize memory partition. */
    Stat = CFE_SB_InitBuffers();
    if (Stat != CFE_SUCCESS)
    {
        /* error reported in CFE_SB_InitBuffers */
        return Stat;
    }

    /* Initialize the pipe table. */
    CFE_SB_InitPipeTbl();

    /* Initialize the routing module */
    CFE_SBR_Init();

    /* Initialize the SB Statistics Pkt */
    CFE_MSG_Init(CFE_MSG_PTR(CFE_SB_Global.StatTlmMsg.TelemetryHeader), CFE_SB_ValueToMsgId(CFE_SB_STATS_TLM_MID),
                 sizeof(CFE_SB_Global.StatTlmMsg));

    return Stat;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_SB_InitBuffers(void)
{
    int32 Stat = 0;

    Stat = CFE_ES_PoolCreateEx(&CFE_SB_Global.Mem.PoolHdl, CFE_SB_Global.Mem.Partition.Data,
                               CFE_PLATFORM_SB_BUF_MEMORY_BYTES, CFE_PLATFORM_ES_POOL_MAX_BUCKETS,
                               &CFE_SB_MemPoolDefSize[0], CFE_ES_NO_MUTEX);

    if (Stat != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: PoolCreate failed for SB Buffers, gave adr 0x%lx,size %d,stat=0x%x\n", __func__,
                             (unsigned long)CFE_SB_Global.Mem.Partition.Data, CFE_PLATFORM_SB_BUF_MEMORY_BYTES,
                             (unsigned int)Stat);
        return Stat;
    }

    /*
     * Initialize the buffer tracking lists to be empty
     */
    CFE_SB_TrackingListReset(&CFE_SB_Global.InTransitList);
    CFE_SB_TrackingListReset(&CFE_SB_Global.ZeroCopyList);

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_SB_InitPipeTbl(void)
{
    CFE_SB_Global.LastPipeId = CFE_ResourceId_FromInteger(CFE_SB_PIPEID_BASE);
}
