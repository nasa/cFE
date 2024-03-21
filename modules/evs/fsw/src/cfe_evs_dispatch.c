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
 * Event services message dispatcher
 */

/* Include Files */
#include "cfe_evs_module_all.h" /* All EVS internal definitions and API */

#include <string.h>

/*
** Local function prototypes.
*/
void CFE_EVS_ProcessGroundCommand(const CFE_SB_Buffer_t *SBBufPtr, CFE_SB_MsgId_t MsgId);
bool CFE_EVS_VerifyCmdLength(const CFE_MSG_Message_t *MsgPtr, size_t ExpectedLength);

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_EVS_ProcessCommandPacket(const CFE_SB_Buffer_t *SBBufPtr)
{
    CFE_SB_MsgId_t MessageID = CFE_SB_INVALID_MSG_ID;

    CFE_MSG_GetMsgId(&SBBufPtr->Msg, &MessageID);

    /* Process all SB messages */
    switch (CFE_SB_MsgIdToValue(MessageID))
    {
        case CFE_EVS_CMD_MID:
            /* EVS task specific command */
            CFE_EVS_ProcessGroundCommand(SBBufPtr, MessageID);
            break;

        case CFE_EVS_SEND_HK_MID:
            /* Housekeeping request */
            CFE_EVS_SendHkCmd((const CFE_EVS_SendHkCmd_t *)SBBufPtr);
            break;

        default:
            /* Unknown command -- should never occur */
            CFE_EVS_Global.EVS_TlmPkt.Payload.CommandErrorCounter++;
            EVS_SendEvent(CFE_EVS_ERR_MSGID_EID, CFE_EVS_EventType_ERROR, "Invalid command packet, Message ID = 0x%08X",
                          (unsigned int)CFE_SB_MsgIdToValue(MessageID));
            break;
    }
}

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 * This function processes a command, verifying that it is valid and of
 *  proper length.
 *
 *-----------------------------------------------------------------*/
void CFE_EVS_ProcessGroundCommand(const CFE_SB_Buffer_t *SBBufPtr, CFE_SB_MsgId_t MsgId)
{
    /* status will get reset if it passes length check */
    int32             Status  = CFE_STATUS_WRONG_MSG_LENGTH;
    CFE_MSG_FcnCode_t FcnCode = 0;

    CFE_MSG_GetFcnCode(&SBBufPtr->Msg, &FcnCode);

    /* Process "known" EVS task ground commands */
    switch (FcnCode)
    {
        case CFE_EVS_NOOP_CC:

            if (CFE_EVS_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_EVS_NoopCmd_t)))
            {
                Status = CFE_EVS_NoopCmd((const CFE_EVS_NoopCmd_t *)SBBufPtr);
            }
            break;

        case CFE_EVS_RESET_COUNTERS_CC:

            if (CFE_EVS_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_EVS_ResetCountersCmd_t)))
            {
                Status = CFE_EVS_ResetCountersCmd((const CFE_EVS_ResetCountersCmd_t *)SBBufPtr);
            }
            break;

        case CFE_EVS_ENABLE_EVENT_TYPE_CC:

            if (CFE_EVS_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_EVS_EnableEventTypeCmd_t)))
            {
                Status = CFE_EVS_EnableEventTypeCmd((const CFE_EVS_EnableEventTypeCmd_t *)SBBufPtr);
            }
            break;

        case CFE_EVS_DISABLE_EVENT_TYPE_CC:

            if (CFE_EVS_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_EVS_DisableEventTypeCmd_t)))
            {
                Status = CFE_EVS_DisableEventTypeCmd((const CFE_EVS_DisableEventTypeCmd_t *)SBBufPtr);
            }
            break;

        case CFE_EVS_SET_EVENT_FORMAT_MODE_CC:

            if (CFE_EVS_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_EVS_SetEventFormatModeCmd_t)))
            {
                Status = CFE_EVS_SetEventFormatModeCmd((const CFE_EVS_SetEventFormatModeCmd_t *)SBBufPtr);
            }
            break;

        case CFE_EVS_ENABLE_APP_EVENT_TYPE_CC:

            if (CFE_EVS_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_EVS_EnableAppEventTypeCmd_t)))
            {
                Status = CFE_EVS_EnableAppEventTypeCmd((const CFE_EVS_EnableAppEventTypeCmd_t *)SBBufPtr);
            }
            break;

        case CFE_EVS_DISABLE_APP_EVENT_TYPE_CC:

            if (CFE_EVS_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_EVS_DisableAppEventTypeCmd_t)))
            {
                Status = CFE_EVS_DisableAppEventTypeCmd((const CFE_EVS_DisableAppEventTypeCmd_t *)SBBufPtr);
            }
            break;

        case CFE_EVS_ENABLE_APP_EVENTS_CC:

            if (CFE_EVS_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_EVS_EnableAppEventsCmd_t)))
            {
                Status = CFE_EVS_EnableAppEventsCmd((const CFE_EVS_EnableAppEventsCmd_t *)SBBufPtr);
            }
            break;

        case CFE_EVS_DISABLE_APP_EVENTS_CC:

            if (CFE_EVS_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_EVS_DisableAppEventsCmd_t)))
            {
                Status = CFE_EVS_DisableAppEventsCmd((const CFE_EVS_DisableAppEventsCmd_t *)SBBufPtr);
            }
            break;

        case CFE_EVS_RESET_APP_COUNTER_CC:

            if (CFE_EVS_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_EVS_ResetAppCounterCmd_t)))
            {
                Status = CFE_EVS_ResetAppCounterCmd((const CFE_EVS_ResetAppCounterCmd_t *)SBBufPtr);
            }
            break;

        case CFE_EVS_SET_FILTER_CC:

            if (CFE_EVS_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_EVS_SetFilterCmd_t)))
            {
                Status = CFE_EVS_SetFilterCmd((const CFE_EVS_SetFilterCmd_t *)SBBufPtr);
            }
            break;

        case CFE_EVS_ENABLE_PORTS_CC:

            if (CFE_EVS_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_EVS_EnablePortsCmd_t)))
            {
                Status = CFE_EVS_EnablePortsCmd((const CFE_EVS_EnablePortsCmd_t *)SBBufPtr);
            }
            break;

        case CFE_EVS_DISABLE_PORTS_CC:

            if (CFE_EVS_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_EVS_DisablePortsCmd_t)))
            {
                Status = CFE_EVS_DisablePortsCmd((const CFE_EVS_DisablePortsCmd_t *)SBBufPtr);
            }
            break;

        case CFE_EVS_RESET_FILTER_CC:

            if (CFE_EVS_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_EVS_ResetFilterCmd_t)))
            {
                Status = CFE_EVS_ResetFilterCmd((const CFE_EVS_ResetFilterCmd_t *)SBBufPtr);
            }
            break;

        case CFE_EVS_RESET_ALL_FILTERS_CC:

            if (CFE_EVS_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_EVS_ResetAllFiltersCmd_t)))
            {
                Status = CFE_EVS_ResetAllFiltersCmd((const CFE_EVS_ResetAllFiltersCmd_t *)SBBufPtr);
            }
            break;

        case CFE_EVS_ADD_EVENT_FILTER_CC:

            if (CFE_EVS_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_EVS_AddEventFilterCmd_t)))
            {
                Status = CFE_EVS_AddEventFilterCmd((const CFE_EVS_AddEventFilterCmd_t *)SBBufPtr);
            }
            break;

        case CFE_EVS_DELETE_EVENT_FILTER_CC:

            if (CFE_EVS_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_EVS_DeleteEventFilterCmd_t)))
            {
                Status = CFE_EVS_DeleteEventFilterCmd((const CFE_EVS_DeleteEventFilterCmd_t *)SBBufPtr);
            }
            break;

        case CFE_EVS_WRITE_APP_DATA_FILE_CC:

            if (CFE_EVS_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_EVS_WriteAppDataFileCmd_t)))
            {
                Status = CFE_EVS_WriteAppDataFileCmd((const CFE_EVS_WriteAppDataFileCmd_t *)SBBufPtr);
            }
            break;

        case CFE_EVS_SET_LOG_MODE_CC:

            if (CFE_EVS_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_EVS_SetLogModeCmd_t)))
            {
                Status = CFE_EVS_SetLogModeCmd((const CFE_EVS_SetLogModeCmd_t *)SBBufPtr);
            }
            break;

        case CFE_EVS_CLEAR_LOG_CC:

            if (CFE_EVS_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_EVS_ClearLogCmd_t)))
            {
                Status = CFE_EVS_ClearLogCmd((const CFE_EVS_ClearLogCmd_t *)SBBufPtr);
            }
            break;

        case CFE_EVS_WRITE_LOG_DATA_FILE_CC:

            if (CFE_EVS_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_EVS_WriteLogDataFileCmd_t)))
            {
                Status = CFE_EVS_WriteLogDataFileCmd((const CFE_EVS_WriteLogDataFileCmd_t *)SBBufPtr);
            }
            break;

        /* default is a bad command code as it was not found above */
        default:

            EVS_SendEvent(CFE_EVS_ERR_CC_EID, CFE_EVS_EventType_ERROR, "Invalid command code -- ID = 0x%08x, CC = %u",
                          (unsigned int)CFE_SB_MsgIdToValue(MsgId), (unsigned int)FcnCode);
            Status = CFE_STATUS_BAD_COMMAND_CODE;

            break;
    }

    if (Status == CFE_SUCCESS)
    {
        CFE_EVS_Global.EVS_TlmPkt.Payload.CommandCounter++;
    }
    else if (Status < 0) /* Negative values indicate errors */
    {
        CFE_EVS_Global.EVS_TlmPkt.Payload.CommandErrorCounter++;
    }
}

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 * This function validates the length of a command structure, and
 * generates an error event if is not the expected length.
 *
 *-----------------------------------------------------------------*/
bool CFE_EVS_VerifyCmdLength(const CFE_MSG_Message_t *MsgPtr, size_t ExpectedLength)
{
    bool              result       = true;
    CFE_MSG_Size_t    ActualLength = 0;
    CFE_MSG_FcnCode_t FcnCode      = 0;
    CFE_SB_MsgId_t    MsgId        = CFE_SB_INVALID_MSG_ID;

    CFE_MSG_GetSize(MsgPtr, &ActualLength);

    /*
    ** Verify the command packet length
    */
    if (ExpectedLength != ActualLength)
    {
        CFE_MSG_GetMsgId(MsgPtr, &MsgId);
        CFE_MSG_GetFcnCode(MsgPtr, &FcnCode);

        EVS_SendEvent(CFE_EVS_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
                      "Invalid msg length: ID = 0x%X,  CC = %u, Len = %u, Expected = %u",
                      (unsigned int)CFE_SB_MsgIdToValue(MsgId), (unsigned int)FcnCode, (unsigned int)ActualLength,
                      (unsigned int)ExpectedLength);
        result = false;
    }

    return result;
}
