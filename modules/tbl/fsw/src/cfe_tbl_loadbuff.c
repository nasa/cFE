/************************************************************************
 * NASA Docket No. GSC-19,200-1, and identified as "cFS Draco"
 *
 * Copyright (c) 2023 United States Government as represented by the
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
 * @file
 *
 * Implementation of table services load buffer methods
 */

/*
** Includes
*/

#include "cfe_tbl_loadbuff.h"
#include "cfe_tbl_internal.h"
#include "cfe_core_resourceid_basevalues.h"

/*----------------------------------------------------------------
 *
 * Local definitions
 *
 *-----------------------------------------------------------------*/

#define CFE_TBL_LOADBUFF_SHARED_CATEGORY      0
#define CFE_TBL_LOADBUFF_LOCAL_CATEGORY_START 1

#define CFE_TBL_MAX_BUFFS_PER_REG 2 /* double buffering is the current max */
#define CFE_TBL_TOTAL_REG_BUFFS   (CFE_PLATFORM_TBL_MAX_NUM_TABLES * CFE_TBL_MAX_BUFFS_PER_REG)
#define CFE_TBL_TOTAL_LOAD_BUFFS  (CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS + CFE_TBL_TOTAL_REG_BUFFS)

/*----------------------------------------------------------------
 *
 * Local helper function - not used outside of this unit
 * Decomposes a table buffer serial number into its index and category value
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_LoadBuffSerialDecompose(uint32 Serial, uint32 *CategoryOut, uint32 *IdxOut)
{
    uint32 Idx;

    Idx = Serial % CFE_TBL_TOTAL_LOAD_BUFFS;

    if (Idx < CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS)
    {
        /* It is one of the global/shared load buffs */
        if (CategoryOut != NULL)
        {
            *CategoryOut = CFE_TBL_LOADBUFF_SHARED_CATEGORY;
        }
        if (IdxOut != NULL)
        {
            *IdxOut = Idx;
        }
    }
    else
    {
        /* It is one of the local per-registry buffers */
        Idx -= CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS;
        if (CategoryOut != NULL)
        {
            *CategoryOut = CFE_TBL_LOADBUFF_LOCAL_CATEGORY_START + (Idx / CFE_TBL_MAX_BUFFS_PER_REG);
        }
        if (IdxOut != NULL)
        {
            *IdxOut = Idx % CFE_TBL_MAX_BUFFS_PER_REG;
        }
    }
}

/*----------------------------------------------------------------
 *
 * Local helper function - not used outside of this unit
 * Composes a serial serial number from an index and category value
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_LoadBuffSerialCompose(uint32 *SerialInOut, uint32 ReqCategory, uint32 ReqIdx)
{
    uint32 Serial;

    Serial = *SerialInOut;

    Serial += CFE_TBL_TOTAL_LOAD_BUFFS - 1;
    Serial -= Serial % CFE_TBL_TOTAL_LOAD_BUFFS;

    if (ReqCategory >= CFE_TBL_LOADBUFF_LOCAL_CATEGORY_START)
    {
        /* It is one of the local per-registry buffers */
        Serial += CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS;
        Serial += (ReqCategory - CFE_TBL_LOADBUFF_LOCAL_CATEGORY_START) * CFE_TBL_MAX_BUFFS_PER_REG;
    }

    Serial += ReqIdx;

    *SerialInOut = Serial;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_LoadBuffId_ToIndex(CFE_TBL_LoadBuffId_t BuffId, uint32 *Category, uint32 *Idx)
{
    uint32       Serial;
    CFE_Status_t Status;

    Serial = CFE_ResourceId_ToInteger(CFE_RESOURCEID_UNWRAP(BuffId));
    Serial -= CFE_TBL_LOADBUFFID_BASE;

    if (Serial <= CFE_RESOURCEID_MAX)
    {
        CFE_TBL_LoadBuffSerialDecompose(Serial, Category, Idx);
        Status = CFE_SUCCESS;
    }
    else
    {
        /* The ID value passed in is not valid */
        Status = CFE_TBL_ERR_INVALID_HANDLE;
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_TBL_LoadBuff_t *CFE_TBL_LocateLoadBufferByID(CFE_TBL_LoadBuffId_t BufferId)
{
    CFE_TBL_LoadBuff_t *   BuffPtr;
    CFE_TBL_RegistryRec_t *ParentRegRecPtr;
    uint32                 Idx;

    uint32 Serial;
    uint32 Category;

    Serial = CFE_ResourceId_ToInteger(CFE_RESOURCEID_UNWRAP(BufferId));
    Serial -= CFE_TBL_LOADBUFFID_BASE;

    if (Serial <= CFE_RESOURCEID_MAX)
    {
        CFE_TBL_LoadBuffSerialDecompose(Serial, &Category, &Idx);

        /* Check if it refers to a shared load buffer or a dedicated table buffer */
        if (Category == CFE_TBL_LOADBUFF_SHARED_CATEGORY)
        {
            /* It refers to one of the shared/global load buffs */
            BuffPtr         = &CFE_TBL_Global.LoadBuffs[Idx];
            ParentRegRecPtr = NULL;
        }
        else
        {
            /* It refers to one of the table buffs in the registry */
            /* There are (possibly) two buffers per registry entry */
            Category -= CFE_TBL_LOADBUFF_LOCAL_CATEGORY_START;
            ParentRegRecPtr = &CFE_TBL_Global.Registry[Category];
            if (!CFE_TBL_RegRecGetConfig(ParentRegRecPtr)->DoubleBuffered)
            {
                /* single-buffered table is always at local index 0 */
                Idx = 0;
            }
            BuffPtr = &ParentRegRecPtr->Buffers[Idx];
        }
    }
    else
    {
        BuffPtr = NULL;
    }

    return BuffPtr;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
bool CFE_TBL_CheckLoadBuffSlotUsed(CFE_ResourceId_t CheckId)
{
    CFE_TBL_LoadBuff_t *BuffPtr;

    /*
     * Note - The pointer here should never be NULL because the ID should always be
     * within the expected range, but if it ever is NULL, this should return true
     * such that the caller will _not_ attempt to use the record.
     */
    BuffPtr = CFE_TBL_LocateLoadBufferByID(CFE_TBL_LOADBUFFID_C(CheckId));
    return (BuffPtr == NULL || CFE_TBL_LoadBuffIsUsed(BuffPtr));
}

uint32 CFE_TBL_LoadBuffIncrementSerial_Impl(uint32 InSerial, uint32 ReqCategory)
{
    uint32 OutSerial;
    uint32 ActualCategory;
    uint32 Idx;

    OutSerial = InSerial + 1;
    CFE_TBL_LoadBuffSerialDecompose(OutSerial, &ActualCategory, &Idx);

    /* This must restrict itself to buffers in the same category */
    if (ActualCategory != ReqCategory)
    {
        CFE_TBL_LoadBuffSerialCompose(&OutSerial, ReqCategory, Idx);
    }

    if (OutSerial >= CFE_RESOURCEID_MAX)
    {
        /* note this does not change the actual index being referred to */
        OutSerial %= CFE_TBL_TOTAL_LOAD_BUFFS;
    }

    return OutSerial;
}

CFE_ResourceId_t CFE_TBL_LoadBuffIncrementSerial(CFE_ResourceId_t Id, void *Arg)
{
    CFE_TBL_LoadBuffFindNextState_t *State = Arg;
    CFE_ResourceId_t                 Result;
    uint32                           NextSerial;

    if (State->RemainingCount == 0)
    {
        Result = CFE_RESOURCEID_UNDEFINED;
    }
    else
    {
        --State->RemainingCount;
        NextSerial = CFE_TBL_LoadBuffIncrementSerial_Impl(CFE_ResourceId_GetSerial(Id), State->BaseCategory);
        Result     = CFE_ResourceId_FromInteger(CFE_TBL_LOADBUFFID_BASE + NextSerial);
    }

    return Result;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_ResourceId_t CFE_TBL_FindNextSharedBufferId(void)
{
    CFE_TBL_LoadBuffFindNextState_t State;
    CFE_ResourceId_t                PendingId;

    memset(&State, 0, sizeof(State));

    State.BaseCategory   = CFE_TBL_LOADBUFF_SHARED_CATEGORY;
    State.RemainingCount = CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS;

    PendingId = CFE_ResourceId_FindNextEx(CFE_TBL_Global.LastLoadBuffId, CFE_TBL_LoadBuffIncrementSerial, &State,
                                          CFE_TBL_CheckLoadBuffSlotUsed);

    return PendingId;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_LoadBuffClearData(CFE_TBL_LoadBuff_t *BufferPtr)
{
    void *DestPtr = CFE_TBL_LoadBuffGetWritePointer(BufferPtr);
    if (DestPtr != NULL)
    {
        memset(DestPtr, 0, CFE_TBL_LoadBuffGetAllocSize(BufferPtr));
    }
    CFE_TBL_LoadBuffSetContentSize(BufferPtr, 0);
    BufferPtr->Crc = 0; /* Just in case the CRC of old data had been calculated */
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_LoadBuffCopyData(CFE_TBL_LoadBuff_t *BufferPtr, const void *SourcePtr, size_t SourceSize)
{
    void *DestPtr = CFE_TBL_LoadBuffGetWritePointer(BufferPtr);
    if (DestPtr != NULL && SourceSize <= CFE_TBL_LoadBuffGetAllocSize(BufferPtr))
    {
        memcpy(DestPtr, SourcePtr, SourceSize);
        CFE_TBL_LoadBuffSetContentSize(BufferPtr, SourceSize);
        BufferPtr->Crc = 0; /* Just in case the CRC of old data had been calculated */
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_LoadBuffRecomputeCRC(CFE_TBL_LoadBuff_t *BufferPtr)
{
    BufferPtr->Crc = CFE_ES_CalculateCRC(CFE_TBL_LoadBuffGetReadPointer(BufferPtr),
                                         CFE_TBL_LoadBuffGetContentSize(BufferPtr), 0, CFE_MISSION_ES_DEFAULT_CRC);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_ResourceId_t CFE_TBL_GetNextLocalBufferId(const CFE_TBL_RegistryRec_t *RegRecPtr)
{
    CFE_ResourceId_t PendingId;
    uint32           Serial;
    uint32           Idx;

    if (CFE_TBL_RegId_ToIndex(CFE_TBL_RegRecGetID(RegRecPtr), &Idx) != CFE_SUCCESS)
    {
        PendingId = CFE_RESOURCEID_UNDEFINED;
    }
    else
    {
        if (CFE_TBL_LOADBUFFID_IS_VALID(RegRecPtr->Status.ActiveBufferId))
        {
            PendingId = CFE_RESOURCEID_UNWRAP(RegRecPtr->Status.ActiveBufferId);
            Serial    = CFE_ResourceId_GetSerial(PendingId);
        }
        else
        {
            Serial = 0;
        }

        Serial    = CFE_TBL_LoadBuffIncrementSerial_Impl(Serial, CFE_TBL_LOADBUFF_LOCAL_CATEGORY_START + Idx);
        PendingId = CFE_ResourceId_FromInteger(CFE_TBL_LOADBUFFID_BASE + Serial);
    }

    return PendingId;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_TBL_RegistryRec_t *CFE_TBL_LoadBuffGetRegRecFromId(CFE_TBL_LoadBuffId_t BuffId)
{
    CFE_TBL_LoadBuff_t *   BuffPtr;
    CFE_TBL_RegistryRec_t *RegRecPtr;

    BuffPtr = CFE_TBL_LocateLoadBufferByID(BuffId);
    if (!CFE_TBL_LoadBuffIsMatch(BuffPtr, BuffId))
    {
        /* Buff ID is not valid */
        RegRecPtr = NULL;
    }
    else
    {
        RegRecPtr = CFE_TBL_LocateRegRecByID(BuffPtr->OwnerRegId);
        if (!CFE_TBL_RegRecIsMatch(RegRecPtr, BuffPtr->OwnerRegId))
        {
            /* owner is gone ... Buffer may be stale? */
            RegRecPtr = NULL;
        }
    }

    return RegRecPtr;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_TBL_LoadBuffIdToTlmIndex(CFE_TBL_LoadBuffId_t BufferId)
{
    int32  ResultIdx;
    uint32 LocalIdx;
    uint32 CategoryIdx;

    /*
     * First try to interpret the ID as a shared load buffer.
     * This relies on the internal range-checking; this will fail if the ID is not actually a shared load buffer
     */
    if (CFE_TBL_LoadBuffId_ToIndex(BufferId, &CategoryIdx, &LocalIdx) != CFE_SUCCESS)
    {
        /* This is for backward compatibility, where -1 meant "none" */
        ResultIdx = -1;
    }
    else
    {
        ResultIdx = LocalIdx;

        /* global load buffs (category 0) can be reported direct, but local bufs need translating */
        if (CategoryIdx != 0)
        {
            /* This is historical: negative numbers meant local table buffers on a double-buffered table */
            /* -3 is the first buffer (idx 0) and -2 is the second buffer (idx 1) */
            ResultIdx -= 3;
        }
    }

    return ResultIdx;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_TBL_LoadBuff_t *CFE_TBL_PrepareNewLoadBuff(CFE_TBL_RegistryRec_t *RegRecPtr)
{
    CFE_TBL_LoadBuff_t *LoadBuffPtr;
    CFE_TBL_LoadBuff_t *ActiveBuffPtr;

    if (CFE_TBL_RegRecGetConfig(RegRecPtr)->DoubleBuffered)
    {
        /* If the table is a double buffered table, then check to make sure the */
        /* inactive buffer has been freed by any Applications that may have been using it */
        LoadBuffPtr = CFE_TBL_GetInactiveBufferExclusive(RegRecPtr);
    }
    else
    {
        /* Single Buffered Table, these use a pool of shared temporary load buffers */
        LoadBuffPtr = CFE_TBL_AcquireGlobalLoadBuff(CFE_TBL_RegRecGetID(RegRecPtr));
    }

    if (LoadBuffPtr != NULL)
    {
        /* In case the file contains a partial table load, get the active buffer contents first */
        ActiveBuffPtr = CFE_TBL_GetActiveBuffer(RegRecPtr);

        /* NOTE: The Active Buffer pointer will be NULL if the table was never loaded */
        if (ActiveBuffPtr != NULL)
        {
            CFE_TBL_LoadBuffCopyData(LoadBuffPtr, CFE_TBL_LoadBuffGetReadPointer(ActiveBuffPtr),
                                     CFE_TBL_LoadBuffGetContentSize(ActiveBuffPtr));
        }

        /* Always consider the copied buffer as unvalidated for now */
        LoadBuffPtr->Validated = false;
    }

    return LoadBuffPtr;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
bool CFE_TBL_LoadBuffIsPrivate(CFE_TBL_LoadBuffId_t BuffId, CFE_TBL_RegId_t RegId)
{
    uint32 Category;
    uint32 BuffIdx;
    uint32 RegIdx;
    bool   Result;

    Result = false;
    if (CFE_TBL_LoadBuffId_ToIndex(BuffId, &Category, &BuffIdx) == CFE_SUCCESS &&
        CFE_TBL_RegId_ToIndex(RegId, &RegIdx) == CFE_SUCCESS)
    {
        /* 0 is the shared category, otherwise it is the reg index w/offset */
        Result = ((RegIdx + CFE_TBL_LOADBUFF_LOCAL_CATEGORY_START) == Category);
    }

    return Result;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
bool CFE_TBL_LoadBuffIsShared(CFE_TBL_LoadBuffId_t BuffId)
{
    uint32 Category;
    uint32 Idx;
    bool   Result;

    Result = false;
    if (CFE_TBL_LoadBuffId_ToIndex(BuffId, &Category, &Idx) == CFE_SUCCESS)
    {
        /* 0 is the shared category, otherwise it is the (1-based) reg index */
        Result = (Category == CFE_TBL_LOADBUFF_SHARED_CATEGORY);
    }

    return Result;
}
