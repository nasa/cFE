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
 * Implementation of table services validation result buffer methods
 *
 *  References:
 *     Flight Software Branch C Coding Standard Version 1.0a
 *     cFE Flight Software Application Developers Guide
 */

/*
** Includes
*/
#include "cfe_tbl_valresult.h"
#include "cfe_tbl_internal.h"
#include "cfe_core_resourceid_basevalues.h"

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
