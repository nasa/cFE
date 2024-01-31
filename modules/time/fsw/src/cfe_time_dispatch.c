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
 * @brief
 *
 * CFE TIME dispatch implementation
 */

/*
** Required header files...
*/
#include "cfe_time_module_all.h"

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 * Function to verify the length of incoming TIME command packets
 *
 *-----------------------------------------------------------------*/
bool CFE_TIME_VerifyCmdLength(const CFE_MSG_Message_t *MsgPtr, size_t ExpectedLength)
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

        CFE_EVS_SendEvent(CFE_TIME_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Invalid msg length: ID = 0x%X,  CC = %u, Len = %u, Expected = %u",
                          (unsigned int)CFE_SB_MsgIdToValue(MsgId), (unsigned int)FcnCode, (unsigned int)ActualLength,
                          (unsigned int)ExpectedLength);
        result = false;
        ++CFE_TIME_Global.CommandErrorCounter;
    }

    return result;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TIME_TaskPipe(const CFE_SB_Buffer_t *SBBufPtr)
{
    CFE_SB_MsgId_t    MessageID   = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t CommandCode = 0;

    CFE_MSG_GetMsgId(&SBBufPtr->Msg, &MessageID);

    switch (CFE_SB_MsgIdToValue(MessageID))
    {
        /*
        ** Housekeeping telemetry request...
        */
        case CFE_TIME_SEND_HK_MID:
            CFE_TIME_SendHkCmd((const CFE_TIME_SendHkCmd_t *)SBBufPtr);
            break;

        /*
        ** Time at the tone "signal"...
        */
        case CFE_TIME_TONE_CMD_MID:
            CFE_TIME_ToneSignalCmd((const CFE_TIME_ToneSignalCmd_t *)SBBufPtr);
            break;

        /*
        ** Time at the tone "data"...
        */
        case CFE_TIME_DATA_CMD_MID:
            CFE_TIME_ToneDataCmd((const CFE_TIME_ToneDataCmd_t *)SBBufPtr);
            break;

        /*
        ** Run time state machine at 1Hz...
        */
        case CFE_TIME_ONEHZ_CMD_MID:
            CFE_TIME_OneHzCmd((const CFE_TIME_OneHzCmd_t *)SBBufPtr);
            break;

/*
** Request for time at the tone "data"...
*/
#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
        case CFE_TIME_SEND_CMD_MID:
            CFE_TIME_ToneSendCmd((const CFE_TIME_FakeToneCmd_t *)SBBufPtr);
            break;
#endif

        /*
        ** Time task ground commands...
        */
        case CFE_TIME_CMD_MID:

            CFE_MSG_GetFcnCode(&SBBufPtr->Msg, &CommandCode);
            switch (CommandCode)
            {
                case CFE_TIME_NOOP_CC:
                    if (CFE_TIME_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_TIME_NoopCmd_t)))
                    {
                        CFE_TIME_NoopCmd((const CFE_TIME_NoopCmd_t *)SBBufPtr);
                    }
                    break;

                case CFE_TIME_RESET_COUNTERS_CC:
                    if (CFE_TIME_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_TIME_ResetCountersCmd_t)))
                    {
                        CFE_TIME_ResetCountersCmd((const CFE_TIME_ResetCountersCmd_t *)SBBufPtr);
                    }
                    break;

                case CFE_TIME_SEND_DIAGNOSTIC_CC:
                    if (CFE_TIME_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_TIME_SendDiagnosticCmd_t)))
                    {
                        CFE_TIME_SendDiagnosticTlm((const CFE_TIME_SendDiagnosticCmd_t *)SBBufPtr);
                    }
                    break;

                case CFE_TIME_SET_STATE_CC:
                    if (CFE_TIME_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_TIME_SetStateCmd_t)))
                    {
                        CFE_TIME_SetStateCmd((const CFE_TIME_SetStateCmd_t *)SBBufPtr);
                    }
                    break;

                case CFE_TIME_SET_SOURCE_CC:
                    if (CFE_TIME_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_TIME_SetSourceCmd_t)))
                    {
                        CFE_TIME_SetSourceCmd((const CFE_TIME_SetSourceCmd_t *)SBBufPtr);
                    }
                    break;

                case CFE_TIME_SET_SIGNAL_CC:
                    if (CFE_TIME_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_TIME_SetSignalCmd_t)))
                    {
                        CFE_TIME_SetSignalCmd((const CFE_TIME_SetSignalCmd_t *)SBBufPtr);
                    }
                    break;

                /*
                ** Time Clients process "tone delay" commands...
                */
                case CFE_TIME_ADD_DELAY_CC:
                    if (CFE_TIME_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_TIME_AddDelayCmd_t)))
                    {
                        CFE_TIME_AddDelayCmd((const CFE_TIME_AddDelayCmd_t *)SBBufPtr);
                    }
                    break;

                case CFE_TIME_SUB_DELAY_CC:
                    if (CFE_TIME_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_TIME_SubDelayCmd_t)))
                    {
                        CFE_TIME_SubDelayCmd((const CFE_TIME_SubDelayCmd_t *)SBBufPtr);
                    }
                    break;

                /*
                ** Time Servers process "set time" commands...
                */
                case CFE_TIME_SET_TIME_CC:
                    if (CFE_TIME_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_TIME_SetTimeCmd_t)))
                    {
                        CFE_TIME_SetTimeCmd((const CFE_TIME_SetTimeCmd_t *)SBBufPtr);
                    }
                    break;

                case CFE_TIME_SET_MET_CC:
                    if (CFE_TIME_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_TIME_SetMETCmd_t)))
                    {
                        CFE_TIME_SetMETCmd((const CFE_TIME_SetMETCmd_t *)SBBufPtr);
                    }
                    break;

                case CFE_TIME_SET_STCF_CC:
                    if (CFE_TIME_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_TIME_SetSTCFCmd_t)))
                    {
                        CFE_TIME_SetSTCFCmd((const CFE_TIME_SetSTCFCmd_t *)SBBufPtr);
                    }
                    break;

                case CFE_TIME_SET_LEAP_SECONDS_CC:
                    if (CFE_TIME_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_TIME_SetLeapSecondsCmd_t)))
                    {
                        CFE_TIME_SetLeapSecondsCmd((const CFE_TIME_SetLeapSecondsCmd_t *)SBBufPtr);
                    }
                    break;

                case CFE_TIME_ADD_ADJUST_CC:
                    if (CFE_TIME_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_TIME_AddAdjustCmd_t)))
                    {
                        CFE_TIME_AddAdjustCmd((const CFE_TIME_AddAdjustCmd_t *)SBBufPtr);
                    }
                    break;

                case CFE_TIME_SUB_ADJUST_CC:
                    if (CFE_TIME_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_TIME_SubAdjustCmd_t)))
                    {
                        CFE_TIME_SubAdjustCmd((const CFE_TIME_SubAdjustCmd_t *)SBBufPtr);
                    }
                    break;

                case CFE_TIME_ADD_ONE_HZ_ADJUSTMENT_CC:
                    if (CFE_TIME_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_TIME_AddOneHzAdjustmentCmd_t)))
                    {
                        CFE_TIME_AddOneHzAdjustmentCmd((const CFE_TIME_AddOneHzAdjustmentCmd_t *)SBBufPtr);
                    }
                    break;

                case CFE_TIME_SUB_ONE_HZ_ADJUSTMENT_CC:
                    if (CFE_TIME_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_TIME_SubOneHzAdjustmentCmd_t)))
                    {
                        CFE_TIME_SubOneHzAdjustmentCmd((const CFE_TIME_SubOneHzAdjustmentCmd_t *)SBBufPtr);
                    }
                    break;

                default:

                    CFE_TIME_Global.CommandErrorCounter++;
                    CFE_EVS_SendEvent(CFE_TIME_CC_ERR_EID, CFE_EVS_EventType_ERROR,
                                      "Invalid command code -- ID = 0x%X, CC = %d",
                                      (unsigned int)CFE_SB_MsgIdToValue(MessageID), (int)CommandCode);
                    break;
            } /* switch (CFE_TIME_CMD_MID -- command code)*/
            break;

        default:

            /*
            ** Note: we only increment the command error counter when
            **    processing CFE_TIME_CMD_MID commands...
            */
            CFE_EVS_SendEvent(CFE_TIME_ID_ERR_EID, CFE_EVS_EventType_ERROR, "Invalid message ID -- ID = 0x%X",
                              (unsigned int)CFE_SB_MsgIdToValue(MessageID));
            break;

    } /* switch (message ID) */
}
