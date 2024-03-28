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

/*----------------------------------------------------------------
 *
 * Implemented per public API
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
