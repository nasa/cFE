/*
**  GSC-18128-1, "Core Flight Executive Version 6.6"
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

uint32 CFE_SB_MemPoolDefSize[CFE_ES_MAX_MEMPOOL_BLOCK_SIZES] = 
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

    int32 Stat = CFE_SUCCESS;

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

    /* Initialize the routing index look up table */
    CFE_SB_InitMsgMap();

    /* Initialize the routing table. */
    CFE_SB_InitRoutingTbl();

    /* Initialize the APID to routing index map table */
    CFE_SB_InitIdxStack();
    
    /* Initialize the SB Statistics Pkt */
    CFE_SB_InitMsg(&CFE_SB.StatTlmMsg,
                   CFE_SB_STATS_TLM_MID,
                   sizeof(CFE_SB.StatTlmMsg),
                   true);    

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
                                CFE_ES_MAX_MEMPOOL_BLOCK_SIZES, 
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
        memset(&CFE_SB.PipeTbl[i].PipeName[0],0,OS_MAX_API_NAME);
    }/* end for */

}/* end CFE_SB_InitPipeTbl */



/******************************************************************************
**  Function:  CFE_SB_InitMsgMap()
**
**  Purpose:
**    Initialize the Software Bus Message Map.
**
**  Arguments:
**
**  Notes:
**    This function MUST be called before any SB API's are called.
**
**  Return:
**    none
*/
void CFE_SB_InitMsgMap(void){

    CFE_SB_MsgKey_Atom_t   KeyVal;

#ifdef MESSAGE_FORMAT_IS_CCSDS

    for (KeyVal=0; KeyVal < CFE_SB_MAX_NUMBER_OF_MSG_KEYS; KeyVal++)
    {
        CFE_SB.MsgMap[KeyVal] = CFE_SB_INVALID_ROUTE_IDX;
    }

#ifndef MESSAGE_FORMAT_IS_CCSDS_VER_2  /* Then use the default, version 1 */
    CFE_ES_WriteToSysLog("SB internal message format: CCSDS Space Packet Protocol version 1\n");
#else   /* VER_2 is the same now but will change for larger and/or distributed systems */
    CFE_ES_WriteToSysLog("SB internal message format: Space Packet Protocol version 2 (extended hdr)\n");
#endif
#endif

}/* end CFE_SB_InitMsgMap */



/******************************************************************************
**  Function:  CFE_SB_InitRoutingTbl()
**
**  Purpose:
**    Initialize the Software Bus Routing Table.
**
**  Arguments:
**
**  Notes:
**    This function MUST be called before any SB API's are called.
**
**  Return:
**    none
*/
void CFE_SB_InitRoutingTbl(void){

    uint32  i;

    /* Initialize routing table */
    for(i=0;i<CFE_PLATFORM_SB_MAX_MSG_IDS;i++){

        CFE_SB.RoutingTbl[i].MsgId = CFE_SB_INVALID_MSG_ID;
        CFE_SB.RoutingTbl[i].SeqCnt = 0;
        CFE_SB.RoutingTbl[i].Destinations = 0;
        CFE_SB.RoutingTbl[i].ListHeadPtr = NULL;

    }/* end for */

}/* end CFE_SB_InitRoutingTbl */

/*****************************************************************************/
