/*
**  GSC-18128-1, "Core Flight Executive Version 6.7"
**
**  Copyright (c) 2006-2019 United States Government as represented by
**  the Administrator of the National Aeronautics and Space Administration.
**  All Rights Reserved.
**
**  Licensed under the Apache License, Version 2.0 (the "License");
**  you may not use this file except in compliance with the License.
**  You may obtain a copy of the License at
**
**    http://www.apache.org/licenses/LICENSE-2.0
**
**  Unless required by applicable law or agreed to in writing, software
**  distributed under the License is distributed on an "AS IS" BASIS,
**  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**  See the License for the specific language governing permissions and
**  limitations under the License.
*/

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

#include "cfe_sb_priv.h"
#include "cfe_sb.h"
#include "osapi.h"
#include "cfe_msgids.h"
#include "cfe_es.h"
#include "cfe_psp.h"
#include "cfe_error.h"
#include "cfe_sb_events.h"

#include <string.h>

/*
**  External Declarations
*/

const size_t CFE_SB_MemPoolDefSize[CFE_PLATFORM_ES_POOL_MAX_BUCKETS] =
{
    CFE_PLATFORM_SB_MAX_BLOCK_SIZE,
    CFE_PLATFORM_SB_MEM_BLOCK_SIZE_16,
    CFE_PLATFORM_SB_MEM_BLOCK_SIZE_15,
    CFE_PLATFORM_SB_MEM_BLOCK_SIZE_14,
    CFE_PLATFORM_SB_MEM_BLOCK_SIZE_13,
    CFE_PLATFORM_SB_MEM_BLOCK_SIZE_12,
    CFE_PLATFORM_SB_MEM_BLOCK_SIZE_11,
    CFE_PLATFORM_SB_MEM_BLOCK_SIZE_10,
    CFE_PLATFORM_SB_MEM_BLOCK_SIZE_09,
    CFE_PLATFORM_SB_MEM_BLOCK_SIZE_08,
    CFE_PLATFORM_SB_MEM_BLOCK_SIZE_07,
    CFE_PLATFORM_SB_MEM_BLOCK_SIZE_06,
    CFE_PLATFORM_SB_MEM_BLOCK_SIZE_05,
    CFE_PLATFORM_SB_MEM_BLOCK_SIZE_04,
    CFE_PLATFORM_SB_MEM_BLOCK_SIZE_03,
    CFE_PLATFORM_SB_MEM_BLOCK_SIZE_02,
    CFE_PLATFORM_SB_MEM_BLOCK_SIZE_01
};


/******************************************************************************
**  Function:  CFE_SB_EarlyInit()
**
**  Purpose:
**    Initialize the Software Bus routing tables.
**
**  Arguments:
**
**  Notes:
**    This function MUST be called before any SB API's are called.
**
**  Return:
**    CFE_SUCCESS
*/
int32 CFE_SB_EarlyInit (void) {

    int32 Stat;

    CFE_SB_Default_Qos.Priority    = CFE_SB_QOS_LOW_PRIORITY;
    CFE_SB_Default_Qos.Reliability = CFE_SB_QOS_LOW_RELIABILITY;

    Stat = OS_MutSemCreate(&CFE_SB.SharedDataMutexId, "CFE_SB_DataMutex", 0);
    if(Stat != OS_SUCCESS){
      CFE_ES_WriteToSysLog("SB shared data mutex creation failed! RC=0x%08x\n",(unsigned int)Stat);
      return Stat;
    }/* end if */
    
    /* Initialize the state of susbcription reporting */
    CFE_SB.SubscriptionReporting = CFE_SB_DISABLE;

    /* Initialize the state of sender reporting */
    CFE_SB.SenderReporting = CFE_PLATFORM_SB_DEFAULT_REPORT_SENDER;

     /* Initialize memory partition. */
    Stat = CFE_SB_InitBuffers();
    if(Stat != CFE_SUCCESS){
      /* error reported in CFE_SB_InitBuffers */
      return Stat;
    }/* end if */

    /* Initialize the pipe table. */
    CFE_SB_InitPipeTbl();

    /* Initialize the routing module */
    CFE_SBR_Init();

    /* Initialize the SB Statistics Pkt */
    CFE_MSG_Init(&CFE_SB.StatTlmMsg.Hdr.BaseMsg,
                 CFE_SB_ValueToMsgId(CFE_SB_STATS_TLM_MID),
                 sizeof(CFE_SB.StatTlmMsg));

    CFE_SB.ZeroCopyTail = NULL;

    return Stat;

}/* end CFE_SB_EarlyInit */



/******************************************************************************
**  Function:  CFE_SB_InitBuffers()
**
**  Purpose:
**    Initialize the Software Bus Buffer Pool.
**
**  Arguments:
**
**  Notes:
**    This function MUST be called before any SB API's are called.
**
**  Return:
**    none
*/
int32  CFE_SB_InitBuffers(void) {

    int32 Stat = 0;

    Stat = CFE_ES_PoolCreateEx(&CFE_SB.Mem.PoolHdl, 
                                CFE_SB.Mem.Partition.Data,
                                CFE_PLATFORM_SB_BUF_MEMORY_BYTES, 
                                CFE_PLATFORM_ES_POOL_MAX_BUCKETS,
                                &CFE_SB_MemPoolDefSize[0],
                                CFE_ES_NO_MUTEX);
    
    if(Stat != CFE_SUCCESS){
        CFE_ES_WriteToSysLog("PoolCreate failed for SB Buffers, gave adr 0x%lx,size %d,stat=0x%x\n",
              (unsigned long)CFE_SB.Mem.Partition.Data,CFE_PLATFORM_SB_BUF_MEMORY_BYTES,(unsigned int)Stat);
        return Stat;
    }
    
    return CFE_SUCCESS;
    
}/* end CFE_SB_InitBuffers */


/******************************************************************************
**  Function:  CFE_SB_InitPipeTbl()
**
**  Purpose:
**    Initialize the Software Bus Pipe Table.
**
**  Arguments:
**
**  Notes:
**    This function MUST be called before any SB API's are called.
**
**  Return:
**    none
*/
void CFE_SB_InitPipeTbl(void){

    uint8  i;

    for(i=0;i<CFE_PLATFORM_SB_MAX_PIPES;i++){
        CFE_SB.PipeTbl[i].InUse         = CFE_SB_NOT_IN_USE;
        CFE_SB.PipeTbl[i].SysQueueId    = CFE_SB_UNUSED_QUEUE;
        CFE_SB.PipeTbl[i].PipeId        = CFE_SB_INVALID_PIPE;
        CFE_SB.PipeTbl[i].CurrentBuff   = NULL;
    }/* end for */

}/* end CFE_SB_InitPipeTbl */

/*****************************************************************************/
