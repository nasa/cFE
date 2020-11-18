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
** File: cfe_sb_buf.c
**
** Purpose:
**      This file contains the source code for the SB memory management
**      functions.
**
** Author:   R.McGraw/SSI
**
******************************************************************************/

/*
**  Include Files
*/

#include "cfe_sb_priv.h"
#include "cfe_sb_events.h"
#include "osapi.h"
#include "cfe_es.h"
#include "cfe_error.h"

/******************************************************************************
**  Function:   CFE_SB_GetBufferFromPool()
**
**  Purpose:
**    Request a buffer from the SB buffer pool. The SB buffer pool is a
**    pre-allocated block of memory of size CFE_PLATFORM_SB_BUF_MEMORY_BYTES. It is used
**    by the SB to dynamically allocate memory to hold the message and a buffer
**    descriptor associated with the message during the sending of a message.
**
**  Arguments:
**    msgId        : Message ID
**    size         : Size of the buffer in bytes.
**
**  Return:
**    Pointer to the buffer descriptor for the new buffer, or NULL if the buffer
**    could not be allocated.
*/

CFE_SB_BufferD_t * CFE_SB_GetBufferFromPool(CFE_SB_MsgId_t MsgId, size_t Size) {
   int32                stat1;
   uint8               *address = NULL;
   CFE_SB_BufferD_t    *bd = NULL;

    /* Allocate a new buffer descriptor from the SB memory pool.*/
    stat1 = CFE_ES_GetPoolBuf((CFE_ES_MemPoolBuf_t*)&bd, CFE_SB.Mem.PoolHdl,  Size + sizeof(CFE_SB_BufferD_t));
    if(stat1 < 0){
        return NULL;
    }

    /* increment the number of buffers in use and adjust the high water mark if needed */
    CFE_SB.StatTlmMsg.Payload.SBBuffersInUse++;
    if(CFE_SB.StatTlmMsg.Payload.SBBuffersInUse > CFE_SB.StatTlmMsg.Payload.PeakSBBuffersInUse){
        CFE_SB.StatTlmMsg.Payload.PeakSBBuffersInUse = CFE_SB.StatTlmMsg.Payload.SBBuffersInUse;
    }/* end if */

    /* Add the size of the actual buffer to the memory-in-use ctr and */
    /* adjust the high water mark if needed */
    CFE_SB.StatTlmMsg.Payload.MemInUse+=stat1;
    if(CFE_SB.StatTlmMsg.Payload.MemInUse > CFE_SB.StatTlmMsg.Payload.PeakMemInUse){
        CFE_SB.StatTlmMsg.Payload.PeakMemInUse = CFE_SB.StatTlmMsg.Payload.MemInUse;
    }/* end if */

    /* first set ptr to actual msg buffer the same as ptr to descriptor */
    address = (uint8 *)bd;

    /* increment actual msg buffer ptr beyond the descriptor */
    address += sizeof(CFE_SB_BufferD_t);

    /* Initialize the buffer descriptor structure. */
    bd->MsgId     = MsgId;
    bd->UseCount  = 1;
    bd->Size      = Size;
    bd->Buffer    = (void *)address;

    return bd;

}/* CFE_SB_GetBufferFromPool */


/******************************************************************************
**  Function:   CFE_SB_GetBufferFromCaller()
**
**  Purpose:
**    Request a buffer from the SB buffer pool to use as a buffer descriptor
**    for and already created SB buffer.
**
**  Arguments:
**    msgId        : Message ID
**    Address      : Address of the buffer
**
**  Return:
**    Pointer to the buffer descriptor for the suplied buffer, or NULL if the
**    descriptor could not be allocated.
*/

CFE_SB_BufferD_t * CFE_SB_GetBufferFromCaller(CFE_SB_MsgId_t MsgId,
                                              void *Address) {
   CFE_SB_BufferD_t    *bd = (CFE_SB_BufferD_t *)((cpuaddr)Address - sizeof(CFE_SB_BufferD_t));

    /* Initialize the MsgId in the buffer descriptor (the rest has already been initialized in this case). */
    bd->MsgId     = MsgId;

    return bd;

}/* CFE_SB_GetBufferFromCaller */


/******************************************************************************
**  Function:   CFE_SB_ReturnBufferToPool()
**
**  Purpose:
**    This function will return two blocks of memory back to the memory pool.
**    One block is the memory used to store the actual message, the other block
**    was used to store the buffer descriptor for the message.
**
**  Arguments:
**    bd     : Pointer to the buffer descriptor.
**
**  Return:
**    SB status
*/
int32 CFE_SB_ReturnBufferToPool(CFE_SB_BufferD_t *bd){
    int32    Stat;

    /* give the buf descriptor back to the buf descriptor pool */
    Stat = CFE_ES_PutPoolBuf(CFE_SB.Mem.PoolHdl, bd);
    if(Stat > 0){
        CFE_SB.StatTlmMsg.Payload.SBBuffersInUse--;
        /* Substract the size of a buffer descriptor from the Memory in use ctr */
        CFE_SB.StatTlmMsg.Payload.MemInUse-=Stat;
    }/* end if */

    return CFE_SUCCESS;

}/* end CFE_SB_ReturnBufferToPool */



/******************************************************************************
**  Function:   CFE_SB_DecrBufUseCnt()
**
**  Purpose:
**    This function will decrement the UseCount of a particular buffer. If the
**    the UseCount is decremented to zero, it will return the buffer to the
**    memory pool.
**
**  Note:
**    UseCount is a variable in the CFE_SB_BufferD_t and is used only to
**    determine when a buffer may be returned to the memory pool.
**
**  Arguments:
**    bd : Pointer to the buffer descriptor.
**
**  Return:
**    CFE_SUCCESS for normal operation.
*/
int32 CFE_SB_DecrBufUseCnt(CFE_SB_BufferD_t *bd){

    /* range check the UseCount variable */
    if(bd->UseCount > 0){

        bd->UseCount--;

        if (bd->UseCount == 0) {
           CFE_SB_ReturnBufferToPool(bd);
        }/* end if */

    }

    return CFE_SUCCESS;

}/* end CFE_SB_DecrBufUseCnt */



/******************************************************************************
**  Function:   CFE_SB_GetDestinationBlk()
**
**  Purpose:
**    This function gets a destination descriptor from the SB memory pool.
**
**  Arguments:
**    None
**
**  Return:
**    Pointer to the destination descriptor
*/
CFE_SB_DestinationD_t *CFE_SB_GetDestinationBlk(void)
{
    int32 Stat;
    CFE_SB_DestinationD_t *Dest = NULL;

    /* Allocate a new destination descriptor from the SB memory pool.*/
    Stat = CFE_ES_GetPoolBuf((CFE_ES_MemPoolBuf_t*)&Dest, CFE_SB.Mem.PoolHdl,  sizeof(CFE_SB_DestinationD_t));
    if(Stat < 0){
        return NULL;
    }

    /* Add the size of a destination descriptor to the memory-in-use ctr and */
    /* adjust the high water mark if needed */
    CFE_SB.StatTlmMsg.Payload.MemInUse+=Stat;
    if(CFE_SB.StatTlmMsg.Payload.MemInUse > CFE_SB.StatTlmMsg.Payload.PeakMemInUse){
       CFE_SB.StatTlmMsg.Payload.PeakMemInUse = CFE_SB.StatTlmMsg.Payload.MemInUse;
    }/* end if */

    return Dest;

}/* end CFE_SB_GetDestinationBlk */


/******************************************************************************
**  Function:   CFE_SB_PutDestinationBlk()
**
**  Purpose:
**    This function returns a destination descriptor to the SB memory pool.
**
**  Arguments:
**    None
**
**  Return:
**    Pointer to the destination descriptor
*/
int32 CFE_SB_PutDestinationBlk(CFE_SB_DestinationD_t *Dest)
{
    int32 Stat;

    if(Dest==NULL){
        return CFE_SB_BAD_ARGUMENT;
    }/* end if */

    /* give the destination block back to the SB memory pool */
    Stat = CFE_ES_PutPoolBuf(CFE_SB.Mem.PoolHdl, Dest);
    if(Stat > 0){
        /* Substract the size of the destination block from the Memory in use ctr */
        CFE_SB.StatTlmMsg.Payload.MemInUse-=Stat;
    }/* end if */

    return CFE_SUCCESS;

}/* end CFE_SB_PutDestinationBlk */


/*****************************************************************************/
