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

/*
**  File:
**  cfe_tbl_resource.c
**
**  Purpose:
**      Function definitions related to CFE resource management
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**     cFE Flight Software Application Developers Guide
*/

/*
** Includes
*/
#include "cfe_tbl_module_all.h"
#include "cfe_core_resourceid_basevalues.h"

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_Handle_ToIndex(CFE_TBL_Handle_t TblHandle, uint32 *Idx)
{
    if (TblHandle < 0 || TblHandle >= CFE_PLATFORM_TBL_MAX_NUM_HANDLES)
    {
        return CFE_TBL_ERR_INVALID_HANDLE;
    }

    *Idx = TblHandle;

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_LoadBuffId_ToIndex(CFE_TBL_LoadBuffId_t BuffId, uint32 *Idx)
{
    int32 IdAsInt;

    IdAsInt = BuffId;
    if (IdAsInt < 0 || IdAsInt >= CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS)
    {
        return CFE_TBL_ERR_INVALID_HANDLE;
    }

    *Idx = IdAsInt;

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_DumpCtrlId_ToIndex(CFE_TBL_DumpCtrlId_t DumpCtrlId, uint32 *Idx)
{
    return CFE_ResourceId_ToIndex(CFE_RESOURCEID_UNWRAP(DumpCtrlId), CFE_TBL_DUMPCTRLID_BASE,
                                  CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS, Idx);
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_ValidationResultId_ToIndex(CFE_TBL_ValidationResultId_t ValResultId, uint32 *Idx)
{
    return CFE_ResourceId_ToIndex(CFE_RESOURCEID_UNWRAP(ValResultId), CFE_TBL_VALRESULTID_BASE,
                                  CFE_PLATFORM_TBL_MAX_NUM_VALIDATIONS, Idx);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_RegId_ToIndex(CFE_TBL_RegId_t RegId, uint32 *Idx)
{
    int32 IdAsInt;

    IdAsInt = RegId;
    if (IdAsInt < 0 || IdAsInt >= CFE_PLATFORM_TBL_MAX_NUM_TABLES)
    {
        return CFE_TBL_ERR_INVALID_HANDLE;
    }

    *Idx = IdAsInt;

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_TBL_RegistryRec_t *CFE_TBL_LocateRegRecByName(const char *Name)
{
    CFE_TBL_RegistryRec_t *RegRecPtr;
    uint32                 i;

    /*
    ** Search the Registry table for an app with a matching name.
    */
    for (i = 0; i < CFE_PLATFORM_TBL_MAX_NUM_TABLES; ++i)
    {
        RegRecPtr = &CFE_TBL_Global.Registry[i];

        if (CFE_TBL_RegRecIsUsed(RegRecPtr) && strcmp(Name, CFE_TBL_RegRecGetName(RegRecPtr)) == 0)
        {
            break;
        }
    }

    if (i >= CFE_PLATFORM_TBL_MAX_NUM_TABLES)
    {
        RegRecPtr = NULL;
    }

    return RegRecPtr;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_TBL_RegistryRec_t *CFE_TBL_LocateRegRecByID(CFE_TBL_RegId_t RegId)
{
    CFE_TBL_RegistryRec_t *RegRecPtr;
    uint32                 Idx;

    if (CFE_TBL_RegId_ToIndex(RegId, &Idx) == CFE_SUCCESS)
    {
        RegRecPtr = &CFE_TBL_Global.Registry[Idx];
    }
    else
    {
        RegRecPtr = NULL;
    }

    return RegRecPtr;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_TBL_RegId_t CFE_TBL_RegRecGetID(const CFE_TBL_RegistryRec_t *RegRecPtr)
{
    /* The pointer should be to an entry within the Registry array */
    int32 IdAsInt;

    IdAsInt = (RegRecPtr - CFE_TBL_Global.Registry);

    return IdAsInt;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_TBL_AccessDescriptor_t *CFE_TBL_LocateAccDescByHandle(CFE_TBL_Handle_t TblHandle)
{
    CFE_TBL_AccessDescriptor_t *AccDescPtr;
    uint32                      Idx;

    if (CFE_TBL_Handle_ToIndex(TblHandle, &Idx) == CFE_SUCCESS)
    {
        AccDescPtr = &CFE_TBL_Global.Handles[Idx];
    }
    else
    {
        AccDescPtr = NULL;
    }

    return AccDescPtr;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_TBL_Handle_t CFE_TBL_AccDescGetHandle(const CFE_TBL_AccessDescriptor_t *AccDescPtr)
{
    /* The pointer should be to an entry within the Handles array */
    return (AccDescPtr - CFE_TBL_Global.Handles);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_TBL_LoadBuff_t *CFE_TBL_LocateLoadBufferByID(CFE_TBL_RegistryRec_t *ContextPtr, CFE_TBL_LoadBuffId_t BufferId)
{
    CFE_TBL_LoadBuff_t *BuffPtr;
    uint32              Idx;

    if (CFE_TBL_LoadBuffId_ToIndex(BufferId, &Idx) == CFE_SUCCESS)
    {
        /* Check if it refers to a shared load buffer or a dedicated table buffer */
        if (ContextPtr == NULL)
        {
            /* It refers to one of the shared/global load buffs */
            BuffPtr = &CFE_TBL_Global.LoadBuffs[Idx % CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS];
        }
        else
        {
            /* It refers to one of the table buffs in the registry */
            /* There are (possibly) two buffers per registry entry */
            BuffPtr = &ContextPtr->Buffers[Idx % 2];
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
CFE_TBL_LoadBuffId_t CFE_TBL_LoadBufferGetID(const CFE_TBL_LoadBuff_t *LoadBuffPtr)
{
    size_t  Offset;
    cpuaddr Addr;
    cpuaddr Ref;

    /* The intent is to mimic prior code behavior.  The buffer could point
     * to an entry in the shared working buffs array, or it could point to
     * a dedicated buffer for a table (inside the RegRec).  Both of which are
     * zero-indexed and thus "0" could mean either the first shared working
     * buffer or the first dedicated buffer. This should be cleaned up and
     * made non-ambiguous, which will also make this logic much cleaner. */

    Addr = (cpuaddr)LoadBuffPtr;
    Ref  = (cpuaddr)CFE_TBL_Global.Registry;
    if (Addr < Ref)
    {
        /* It must be one of the shared load buffs */
        Ref = (cpuaddr)CFE_TBL_Global.LoadBuffs;
    }
    else
    {
        /* It is inside the registry record for a specific table, first get the registry id */
        Offset = (Addr - Ref) / sizeof(CFE_TBL_RegistryRec_t);

        Ref = (cpuaddr)&CFE_TBL_Global.Registry[Offset].Buffers;
    }

    Offset = (Addr - Ref) / sizeof(CFE_TBL_LoadBuff_t);

    return Offset;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_TBL_DumpControl_t *CFE_TBL_LocateDumpCtrlByID(CFE_TBL_DumpCtrlId_t BlockId)
{
    CFE_TBL_DumpControl_t *BlockPtr;
    uint32                 Idx;

    if (CFE_TBL_DumpCtrlId_ToIndex(BlockId, &Idx) == CFE_SUCCESS)
    {
        BlockPtr = &CFE_TBL_Global.DumpControlBlocks[Idx];
    }
    else
    {
        BlockPtr = NULL;
    }

    return BlockPtr;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
bool CFE_TBL_CheckDumpCtrlSlotUsed(CFE_ResourceId_t CheckId)
{
    CFE_TBL_DumpControl_t *BlockPtr;

    /*
     * Note - The pointer here should never be NULL because the ID should always be
     * within the expected range, but if it ever is NULL, this should return true
     * such that the caller will _not_ attempt to use the record.
     */
    BlockPtr = CFE_TBL_LocateDumpCtrlByID(CFE_TBL_DUMPCTRLID_C(CheckId));
    return (BlockPtr == NULL || CFE_TBL_DumpCtrlBlockIsUsed(BlockPtr));
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_ResourceId_t CFE_TBL_GetNextDumpCtrlBlock(void)
{
    return CFE_ResourceId_FindNext(CFE_TBL_Global.LastDumpCtrlBlockId, CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS,
                                   CFE_TBL_CheckDumpCtrlSlotUsed);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_TBL_ValidationResult_t *CFE_TBL_LocateValidationResultByID(CFE_TBL_ValidationResultId_t ValResultId)
{
    CFE_TBL_ValidationResult_t *ResultPtr;
    uint32                      Idx;

    if (CFE_TBL_ValidationResultId_ToIndex(ValResultId, &Idx) == CFE_SUCCESS)
    {
        ResultPtr = &CFE_TBL_Global.ValidationResults[Idx];
    }
    else
    {
        ResultPtr = NULL;
    }

    return ResultPtr;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
bool CFE_TBL_CheckValidationResultSlotUsed(CFE_ResourceId_t CheckId)
{
    CFE_TBL_ValidationResult_t *BuffPtr;

    /*
     * Note - The pointer here should never be NULL because the ID should always be
     * within the expected range, but if it ever is NULL, this should return true
     * such that the caller will _not_ attempt to use the record.
     */
    BuffPtr = CFE_TBL_LocateValidationResultByID(CFE_TBL_VALRESULTID_C(CheckId));
    return (BuffPtr == NULL || CFE_TBL_ValidationResultIsUsed(BuffPtr));
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_ResourceId_t CFE_TBL_GetNextValResultBlock(void)
{
    return CFE_ResourceId_FindNext(CFE_TBL_Global.LastValidationResultId, CFE_PLATFORM_TBL_MAX_NUM_VALIDATIONS,
                                   CFE_TBL_CheckValidationResultSlotUsed);
}
