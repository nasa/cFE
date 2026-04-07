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
    return CFE_ResourceId_ToIndex(CFE_RESOURCEID_UNWRAP(ValResultId),
                                  CFE_TBL_VALRESULTID_BASE,
                                  CFE_PLATFORM_TBL_MAX_NUM_VALIDATIONS,
                                  Idx);
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
    return CFE_ResourceId_FindNext(CFE_TBL_Global.LastValidationResultId,
                                   CFE_PLATFORM_TBL_MAX_NUM_VALIDATIONS,
                                   CFE_TBL_CheckValidationResultSlotUsed);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_TBL_ValidationResult_t *CFE_TBL_TxnCheckValidationRequest(CFE_TBL_TxnState_t *Txn)
{
    CFE_TBL_ValidationResultId_t PreviousValId;
    CFE_TBL_ValidationResult_t  *ValResultPtr;
    CFE_TBL_RegistryRec_t       *RegRecPtr;

    RegRecPtr = CFE_TBL_TxnRegRec(Txn);

    /* check if validation is already pending */
    PreviousValId = RegRecPtr->PendingValId;
    ValResultPtr  = CFE_TBL_LocateValidationResultByID(PreviousValId);
    if (CFE_TBL_ValidationResultIsMatch(ValResultPtr, PreviousValId))
    {
        /* A validation is already pending and has not been completed yet */
        CFE_TBL_TxnAddEvent(Txn, CFE_TBL_VALIDATION_IN_PROGRESS_ERR_EID, 0, CFE_TBL_VALRESULTID_INT(PreviousValId));
    }
    else
    {
        /* No validation is already pending */
        ValResultPtr = NULL;
    }

    return ValResultPtr;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_TBL_ValidationResult_t *CFE_TBL_TxnSetupValidationRequest(CFE_TBL_TxnState_t         *Txn,
                                                              CFE_TBL_BufferSelect_Enum_t BufferSelect)
{
    CFE_ResourceId_t            NextValId;
    CFE_TBL_ValidationResult_t *ValResultPtr;
    CFE_TBL_RegistryRec_t      *RegRecPtr;
    CFE_TBL_LoadBuff_t         *SelectedBufferPtr;

    RegRecPtr         = CFE_TBL_TxnRegRec(Txn);
    ValResultPtr      = NULL;
    NextValId         = CFE_RESOURCEID_UNDEFINED;
    SelectedBufferPtr = CFE_TBL_GetSelectedBuffer(RegRecPtr, BufferSelect);

    if (SelectedBufferPtr == NULL)
    {
        CFE_TBL_TxnAddEvent(Txn, CFE_TBL_ILLEGAL_BUFF_PARAM_ERR_EID, 0, BufferSelect);
    }
    else
    {
        /*
         * Get a result block
         *
         * NOTE: this is always allocating a result block even if there is no validation function.
         * This is because it was a specific request/command to validate this, and by initializing
         * the result block (even if its a "noop") then the result will be sent in the next HK TLM
         */
        NextValId    = CFE_TBL_GetNextValResultBlock();
        ValResultPtr = CFE_TBL_LocateValidationResultByID(CFE_TBL_VALRESULTID_C(NextValId));

        if (ValResultPtr == NULL)
        {
            CFE_TBL_TxnAddEvent(Txn, CFE_TBL_TOO_MANY_VALIDATIONS_ERR_EID, 0, 0);
        }
        else
        {
            /* If owner has a validation function, then notify the  */
            /* table owner that there is data to be validated       */
            if (CFE_TBL_RegRecGetValidationFunc(RegRecPtr) != NULL)
            {
                /* Set this Validation Response Block for deferred processing by the app */
                ValResultPtr->State  = CFE_TBL_VALIDATION_PENDING;
                ValResultPtr->Result = 0;
                strncpy(ValResultPtr->TableName, CFE_TBL_RegRecGetName(RegRecPtr), sizeof(ValResultPtr->TableName));

                ValResultPtr->CrcOfTable   = SelectedBufferPtr->Crc;
                ValResultPtr->BufferSelect = BufferSelect;

                CFE_TBL_ValidationResultSetUsed(ValResultPtr, NextValId);

                CFE_TBL_Global.LastValidationResultId = NextValId;

                RegRecPtr->PendingValId = CFE_TBL_ValidationResultGetId(ValResultPtr);

                /* Maintain statistic on number of validation requests given to applications */
                CFE_TBL_Global.NumValRequests++;
            }
            else
            {
                /* If there isn't a validation function pointer, then the process is complete  */
                /* By setting this value, we are letting the Housekeeping process recognize it */
                /* as data to be sent to the ground in telemetry.                              */
                ValResultPtr->State        = CFE_TBL_VALIDATION_PERFORMED;
                SelectedBufferPtr->IsValid = true;

                CFE_TBL_TxnAddEvent(Txn, CFE_TBL_ASSUMED_VALID_INF_EID, 0, 0);
            }
        }
    }

    return ValResultPtr;
}

/*----------------------------------------------------------------
 *
 * Local helper function
 *
 *-----------------------------------------------------------------*/
bool CFE_TBL_SendValidationEventHelper(const CFE_TBL_TxnEvent_t *Event, CFE_TBL_TxnEventContext_t *Ctxt)
{
    uint16 EventType;
    char   EventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    /* The majority of the events are errors, but this can be reset later to demote to info/debug */
    EventType      = CFE_EVS_EventType_ERROR;
    EventString[0] = 0;

    switch (Event->EventId)
    {
        case CFE_TBL_TOO_MANY_VALIDATIONS_ERR_EID:
            snprintf(EventString, sizeof(EventString), "Too many Table Validations have been requested");
            break;

        case CFE_TBL_ILLEGAL_BUFF_PARAM_ERR_EID:
            snprintf(EventString,
                     sizeof(EventString),
                     "Request had illegal buffer parameter (0x%08X)",
                     (unsigned int)Event->EventData2);
            break;

        case CFE_TBL_ASSUMED_VALID_INF_EID:
            snprintf(EventString, sizeof(EventString), "No Validation Function has been registered, assumed valid");
            EventType = CFE_EVS_EventType_INFORMATION;
            break;

        case CFE_TBL_VALIDATION_IN_PROGRESS_ERR_EID:
            snprintf(EventString,
                     sizeof(EventString),
                     "Validation already in progress, id=%x",
                     (unsigned int)Event->EventData2);
            break;
    }

    if (EventString[0] == 0)
    {
        return false;
    }

    /* Finally send the actual event by appending all the info we have */
    CFE_EVS_SendEventWithAppID(Event->EventId,
                               EventType,
                               CFE_TBL_Global.TableTaskAppId,
                               "%s,app=%s,tbl=%s:%s",
                               Ctxt->Operation,
                               Ctxt->CallerName,
                               Ctxt->TableName,
                               EventString);

    return true;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_SendValidationEvents(CFE_TBL_TxnState_t *Txn)
{
    CFE_TBL_SendTransactionEvents(Txn, "Validate", CFE_TBL_SendValidationEventHelper, NULL);
}
