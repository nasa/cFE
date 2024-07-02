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
    if (RegId < 0 || RegId >= CFE_PLATFORM_TBL_MAX_NUM_TABLES)
    {
        return CFE_TBL_ERR_INVALID_HANDLE;
    }

    *Idx = RegId;

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_TBL_RegistryRec_t *CFE_TBL_LocateRegistryRecordByID(CFE_TBL_RegId_t RegId)
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
CFE_TBL_RegId_t CFE_TBL_RegistryRecordGetID(const CFE_TBL_RegistryRec_t *RegRecPtr)
{
    /* The pointer should be to an entry within the Registry array */
    return (RegRecPtr - CFE_TBL_Global.Registry);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_TBL_AccessDescriptor_t *CFE_TBL_LocateAccessDescriptorByHandle(CFE_TBL_Handle_t TblHandle)
{
    CFE_TBL_AccessDescriptor_t *AccessDescPtr;
    uint32                      Idx;

    if (CFE_TBL_Handle_ToIndex(TblHandle, &Idx) == CFE_SUCCESS)
    {
        AccessDescPtr = &CFE_TBL_Global.Handles[Idx];
    }
    else
    {
        AccessDescPtr = NULL;
    }

    return AccessDescPtr;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_TBL_Handle_t CFE_TBL_AccessDescriptorGetHandle(const CFE_TBL_AccessDescriptor_t *AccessDescPtr)
{
    /* The pointer should be to an entry within the Handles array */
    return (AccessDescPtr - CFE_TBL_Global.Handles);
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
