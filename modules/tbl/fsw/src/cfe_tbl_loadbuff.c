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

/**
 * @file
 *
 * Implementation of table services load buffer methods
 *
 *  References:
 *     Flight Software Branch C Coding Standard Version 1.0a
 *     cFE Flight Software Application Developers Guide
 */

/*
** Includes
*/

#include "cfe_tbl_loadbuff.h"
#include "cfe_tbl_internal.h"
#include "cfe_core_resourceid_basevalues.h"

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_LoadBuffId_ToIndex(CFE_TBL_LoadBuffId_t BuffId, uint32 *Idx)
{
    int32 IdAsInt;

    IdAsInt = CFE_TBL_LOADBUFFID_INT(BuffId);
    if (IdAsInt < 0 || IdAsInt >= CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS)
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

    return CFE_TBL_LOADBUFFID_C(Offset);
}
