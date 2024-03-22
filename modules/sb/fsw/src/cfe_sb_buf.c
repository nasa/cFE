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

#include "cfe_sb_module_all.h"

/*
 * The actual message content of a SB Buffer Descriptor is the
 * offset of the content member.  This will be auto-aligned by
 * the compiler according to the requirements of the machine.
 */
#define CFE_SB_BUFFERD_CONTENT_OFFSET (offsetof(CFE_SB_BufferD_t, Content))

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_SB_TrackingListReset(CFE_SB_BufferLink_t *Link)
{
    /* A singleton node/empty list points to itself */
    Link->Prev = Link;
    Link->Next = Link;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_SB_TrackingListRemove(CFE_SB_BufferLink_t *Node)
{
    /* Remove from list */
    Node->Prev->Next = Node->Next;
    Node->Next->Prev = Node->Prev;

    /* The node is now a singleton */
    CFE_SB_TrackingListReset(Node);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_SB_TrackingListAdd(CFE_SB_BufferLink_t *List, CFE_SB_BufferLink_t *Node)
{
    /* Connect this node to the list at "prev" position (tail) */
    Node->Prev = List->Prev;
    Node->Next = List;

    /* Connect list nodes to this node */
    Node->Prev->Next = Node;
    Node->Next->Prev = Node;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_SB_BufferD_t *CFE_SB_GetBufferFromPool(size_t MaxMsgSize)
{
    int32               stat1;
    size_t              AllocSize;
    CFE_ES_MemPoolBuf_t addr = NULL;
    CFE_SB_BufferD_t *  bd;

    /* The allocation needs to include enough space for the descriptor object */
    AllocSize = MaxMsgSize + CFE_SB_BUFFERD_CONTENT_OFFSET;

    /* Allocate a new buffer descriptor from the SB memory pool.*/
    stat1 = CFE_ES_GetPoolBuf(&addr, CFE_SB_Global.Mem.PoolHdl, AllocSize);
    if (stat1 < 0)
    {
        return NULL;
    }

    /* increment the number of buffers in use and adjust the high water mark if needed */
    CFE_SB_Global.StatTlmMsg.Payload.SBBuffersInUse++;
    if (CFE_SB_Global.StatTlmMsg.Payload.SBBuffersInUse > CFE_SB_Global.StatTlmMsg.Payload.PeakSBBuffersInUse)
    {
        CFE_SB_Global.StatTlmMsg.Payload.PeakSBBuffersInUse = CFE_SB_Global.StatTlmMsg.Payload.SBBuffersInUse;
    }

    /* Add the size of the actual buffer to the memory-in-use ctr and */
    /* adjust the high water mark if needed */
    CFE_SB_Global.StatTlmMsg.Payload.MemInUse += AllocSize;
    if (CFE_SB_Global.StatTlmMsg.Payload.MemInUse > CFE_SB_Global.StatTlmMsg.Payload.PeakMemInUse)
    {
        CFE_SB_Global.StatTlmMsg.Payload.PeakMemInUse = CFE_SB_Global.StatTlmMsg.Payload.MemInUse;
    }

    /* Initialize the buffer descriptor structure. */
    bd = (CFE_SB_BufferD_t *)addr;
    memset(bd, 0, CFE_SB_BUFFERD_CONTENT_OFFSET);

    bd->UseCount      = 1;
    bd->AllocatedSize = AllocSize;

    CFE_SB_TrackingListReset(&bd->Link);

    return bd;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_SB_ReturnBufferToPool(CFE_SB_BufferD_t *bd)
{
    /* Remove from any tracking list (no effect if not in a list) */
    CFE_SB_TrackingListRemove(&bd->Link);

    --CFE_SB_Global.StatTlmMsg.Payload.SBBuffersInUse;
    CFE_SB_Global.StatTlmMsg.Payload.MemInUse -= bd->AllocatedSize;

    /* finally give the buf descriptor back to the buf descriptor pool */
    CFE_ES_PutPoolBuf(CFE_SB_Global.Mem.PoolHdl, bd);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_SB_IncrBufUseCnt(CFE_SB_BufferD_t *bd)
{
    /* range check the UseCount variable */
    if (bd->UseCount < 0x7FFF)
    {
        ++bd->UseCount;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_SB_DecrBufUseCnt(CFE_SB_BufferD_t *bd)
{
    /* range check the UseCount variable */
    if (bd->UseCount > 0)
    {
        --bd->UseCount;

        if (bd->UseCount == 0)
        {
            CFE_SB_ReturnBufferToPool(bd);
        }
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_SB_DestinationD_t *CFE_SB_GetDestinationBlk(void)
{
    int32               Stat;
    CFE_ES_MemPoolBuf_t addr = NULL;

    /* Allocate a new destination descriptor from the SB memory pool.*/
    Stat = CFE_ES_GetPoolBuf(&addr, CFE_SB_Global.Mem.PoolHdl, sizeof(CFE_SB_DestinationD_t));
    if (Stat < 0)
    {
        return NULL;
    }

    /* Add the size of a destination descriptor to the memory-in-use ctr and */
    /* adjust the high water mark if needed */
    CFE_SB_Global.StatTlmMsg.Payload.MemInUse += Stat;
    if (CFE_SB_Global.StatTlmMsg.Payload.MemInUse > CFE_SB_Global.StatTlmMsg.Payload.PeakMemInUse)
    {
        CFE_SB_Global.StatTlmMsg.Payload.PeakMemInUse = CFE_SB_Global.StatTlmMsg.Payload.MemInUse;
    }

    return (CFE_SB_DestinationD_t *)addr;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_SB_PutDestinationBlk(CFE_SB_DestinationD_t *Dest)
{
    int32 Stat;

    if (Dest == NULL)
    {
        return CFE_SB_BAD_ARGUMENT;
    }

    /* give the destination block back to the SB memory pool */
    Stat = CFE_ES_PutPoolBuf(CFE_SB_Global.Mem.PoolHdl, Dest);
    if (Stat > 0)
    {
        /* Subtract the size of the destination block from the Memory in use ctr */
        CFE_SB_Global.StatTlmMsg.Payload.MemInUse -= Stat;
    }

    return CFE_SUCCESS;
}
