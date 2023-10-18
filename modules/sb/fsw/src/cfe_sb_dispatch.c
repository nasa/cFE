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

/******************************************************************************
** File: cfe_sb_task.c
**
** Purpose:
**      This file contains the source code for the SB task.
**
** Author:   R.McGraw/SSI
**
******************************************************************************/

/* Include Files */

#include "cfe_sb_module_all.h"

#include <string.h>

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 * Verifies the length of incoming SB command packets, returns true if acceptable
 *
 *-----------------------------------------------------------------*/
bool CFE_SB_VerifyCmdLength(const CFE_MSG_Message_t *MsgPtr, size_t ExpectedLength)
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

        CFE_EVS_SendEvent(CFE_SB_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Invalid msg length: ID = 0x%X,  CC = %u, Len = %u, Expected = %u",
                          (unsigned int)CFE_SB_MsgIdToValue(MsgId), (unsigned int)FcnCode, (unsigned int)ActualLength,
                          (unsigned int)ExpectedLength);
        result = false;
        ++CFE_SB_Global.HKTlmMsg.Payload.CommandErrorCounter;
    }

    return result;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_SB_ProcessCmdPipePkt(const CFE_SB_Buffer_t *SBBufPtr)
{
    CFE_SB_MsgId_t    MessageID = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t FcnCode   = 0;

    CFE_MSG_GetMsgId(&SBBufPtr->Msg, &MessageID);

    switch (CFE_SB_MsgIdToValue(MessageID))
    {
        case CFE_SB_SEND_HK_MID:
            /* Note: Command counter not incremented for this command */
            CFE_SB_SendHKTlmCmd((const CFE_SB_SendHkCmd_t *)SBBufPtr);
            break;

        case CFE_SB_SUB_RPT_CTRL_MID:
            /* Note: Command counter not incremented for this command */
            CFE_MSG_GetFcnCode(&SBBufPtr->Msg, &FcnCode);
            switch (FcnCode)
            {
                case CFE_SB_SEND_PREV_SUBS_CC:
                    if (CFE_SB_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_SB_SendPrevSubsCmd_t)))
                    {
                        CFE_SB_SendPrevSubsCmd((const CFE_SB_SendPrevSubsCmd_t *)SBBufPtr);
                    }
                    break;

                case CFE_SB_ENABLE_SUB_REPORTING_CC:
                    if (CFE_SB_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_SB_EnableSubReportingCmd_t)))
                    {
                        CFE_SB_EnableSubReportingCmd((const CFE_SB_EnableSubReportingCmd_t *)SBBufPtr);
                    }
                    break;

                case CFE_SB_DISABLE_SUB_REPORTING_CC:
                    if (CFE_SB_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_SB_DisableSubReportingCmd_t)))
                    {
                        CFE_SB_DisableSubReportingCmd((const CFE_SB_DisableSubReportingCmd_t *)SBBufPtr);
                    }
                    break;

                default:
                    CFE_EVS_SendEvent(CFE_SB_BAD_CMD_CODE_EID, CFE_EVS_EventType_ERROR,
                                      "Invalid Cmd, Unexpected Command Code %u", (unsigned int)FcnCode);
                    CFE_SB_Global.HKTlmMsg.Payload.CommandErrorCounter++;
                    break;
            } /* end switch on cmd code */
            break;

        case CFE_SB_CMD_MID:
            CFE_MSG_GetFcnCode(&SBBufPtr->Msg, &FcnCode);
            switch (FcnCode)
            {
                case CFE_SB_NOOP_CC:
                    if (CFE_SB_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_SB_NoopCmd_t)))
                    {
                        CFE_SB_NoopCmd((const CFE_SB_NoopCmd_t *)SBBufPtr);
                    }
                    break;

                case CFE_SB_RESET_COUNTERS_CC:
                    if (CFE_SB_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_SB_ResetCountersCmd_t)))
                    {
                        /* Note: Command counter not incremented for this command */
                        CFE_SB_ResetCountersCmd((const CFE_SB_ResetCountersCmd_t *)SBBufPtr);
                    }
                    break;

                case CFE_SB_SEND_SB_STATS_CC:
                    if (CFE_SB_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_SB_SendSbStatsCmd_t)))
                    {
                        CFE_SB_SendStatsCmd((const CFE_SB_SendSbStatsCmd_t *)SBBufPtr);
                    }
                    break;

                case CFE_SB_WRITE_ROUTING_INFO_CC:
                    if (CFE_SB_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_SB_WriteRoutingInfoCmd_t)))
                    {
                        CFE_SB_WriteRoutingInfoCmd((const CFE_SB_WriteRoutingInfoCmd_t *)SBBufPtr);
                    }
                    break;

                case CFE_SB_ENABLE_ROUTE_CC:
                    if (CFE_SB_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_SB_EnableRouteCmd_t)))
                    {
                        CFE_SB_EnableRouteCmd((const CFE_SB_EnableRouteCmd_t *)SBBufPtr);
                    }
                    break;

                case CFE_SB_DISABLE_ROUTE_CC:
                    if (CFE_SB_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_SB_DisableRouteCmd_t)))
                    {
                        CFE_SB_DisableRouteCmd((const CFE_SB_DisableRouteCmd_t *)SBBufPtr);
                    }
                    break;

                case CFE_SB_WRITE_PIPE_INFO_CC:
                    if (CFE_SB_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_SB_WritePipeInfoCmd_t)))
                    {
                        CFE_SB_WritePipeInfoCmd((const CFE_SB_WritePipeInfoCmd_t *)SBBufPtr);
                    }
                    break;

                case CFE_SB_WRITE_MAP_INFO_CC:
                    if (CFE_SB_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_SB_WriteMapInfoCmd_t)))
                    {
                        CFE_SB_WriteMapInfoCmd((const CFE_SB_WriteMapInfoCmd_t *)SBBufPtr);
                    }
                    break;

                default:
                    CFE_EVS_SendEvent(CFE_SB_BAD_CMD_CODE_EID, CFE_EVS_EventType_ERROR,
                                      "Invalid Cmd, Unexpected Command Code %u", FcnCode);
                    CFE_SB_Global.HKTlmMsg.Payload.CommandErrorCounter++;
                    break;
            } /* end switch on cmd code */
            break;

        default:
            CFE_EVS_SendEvent(CFE_SB_BAD_MSGID_EID, CFE_EVS_EventType_ERROR, "Invalid Cmd, Unexpected Msg Id: 0x%x",
                              (unsigned int)CFE_SB_MsgIdToValue(MessageID));
            CFE_SB_Global.HKTlmMsg.Payload.CommandErrorCounter++;
            break;

    } /* end switch on MsgId */
}
