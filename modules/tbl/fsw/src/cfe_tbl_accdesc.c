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
 * Implementation of table services access descriptor methods
 *
 *  References:
 *     Flight Software Branch C Coding Standard Version 1.0a
 *     cFE Flight Software Application Developers Guide
 */

/*
** Includes
*/

#include "cfe_tbl_accdesc.h"
#include "cfe_tbl_regrec.h"
#include "cfe_tbl_internal.h"
#include "cfe_core_resourceid_basevalues.h"
#include "cfe_resourceid_api_typedefs.h"

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_InitAccessDescriptor(CFE_TBL_AccessDescriptor_t *AccessDescPtr)
{
    memset(AccessDescPtr, 0, sizeof(*AccessDescPtr));

    CFE_TBL_HandleLinkInit(&AccessDescPtr->Link);
}

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
