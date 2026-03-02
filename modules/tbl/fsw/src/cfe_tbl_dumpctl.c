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
 * Implementation of table services dump control block methods
 */

/*
** Includes
*/

#include "cfe_tbl_dumpctl.h"
#include "cfe_tbl_internal.h"
#include "cfe_core_resourceid_basevalues.h"

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
