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
 *  @file
 *
 * Msg pipe dispatcher routines for CFE ES
 */

/*
 * Includes
 */
#include "cfe_es_module_all.h"

#include "cfe_version.h"
#include "target_config.h"
#include "cfe_es_verify.h"

#include "cfe_config.h"

#include <string.h>

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
bool CFE_ES_VerifyCmdLength(const CFE_MSG_Message_t *MsgPtr, size_t ExpectedLength)
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

        CFE_EVS_SendEvent(CFE_ES_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Invalid msg length: ID = 0x%X,  CC = %u, Len = %u, Expected = %u",
                          (unsigned int)CFE_SB_MsgIdToValue(MsgId), (unsigned int)FcnCode, (unsigned int)ActualLength,
                          (unsigned int)ExpectedLength);
        result = false;
        CFE_ES_Global.TaskData.CommandErrorCounter++;
    }

    return result;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_ES_TaskPipe(const CFE_SB_Buffer_t *SBBufPtr)
{
    CFE_SB_MsgId_t    MessageID   = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t CommandCode = 0;

    CFE_MSG_GetMsgId(&SBBufPtr->Msg, &MessageID);
    switch (CFE_SB_MsgIdToValue(MessageID))
    {
        /*
        ** Housekeeping telemetry request
        */
        case CFE_ES_SEND_HK_MID:
            CFE_ES_SendHkCmd((const CFE_ES_SendHkCmd_t *)SBBufPtr);
            break;

        /*
        ** ES task ground commands
        */
        case CFE_ES_CMD_MID:

            CFE_MSG_GetFcnCode(&SBBufPtr->Msg, &CommandCode);
            switch (CommandCode)
            {
                case CFE_ES_NOOP_CC:
                    if (CFE_ES_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_ES_NoopCmd_t)))
                    {
                        CFE_ES_NoopCmd((const CFE_ES_NoopCmd_t *)SBBufPtr);
                    }
                    break;

                case CFE_ES_RESET_COUNTERS_CC:
                    if (CFE_ES_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_ES_ResetCountersCmd_t)))
                    {
                        CFE_ES_ResetCountersCmd((const CFE_ES_ResetCountersCmd_t *)SBBufPtr);
                    }
                    break;

                case CFE_ES_RESTART_CC:
                    if (CFE_ES_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_ES_RestartCmd_t)))
                    {
                        CFE_ES_RestartCmd((const CFE_ES_RestartCmd_t *)SBBufPtr);
                    }
                    break;

                case CFE_ES_START_APP_CC:
                    if (CFE_ES_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_ES_StartAppCmd_t)))
                    {
                        CFE_ES_StartAppCmd((const CFE_ES_StartAppCmd_t *)SBBufPtr);
                    }
                    break;

                case CFE_ES_STOP_APP_CC:
                    if (CFE_ES_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_ES_StopAppCmd_t)))
                    {
                        CFE_ES_StopAppCmd((const CFE_ES_StopAppCmd_t *)SBBufPtr);
                    }
                    break;

                case CFE_ES_RESTART_APP_CC:
                    if (CFE_ES_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_ES_RestartAppCmd_t)))
                    {
                        CFE_ES_RestartAppCmd((const CFE_ES_RestartAppCmd_t *)SBBufPtr);
                    }
                    break;

                case CFE_ES_RELOAD_APP_CC:
                    if (CFE_ES_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_ES_ReloadAppCmd_t)))
                    {
                        CFE_ES_ReloadAppCmd((const CFE_ES_ReloadAppCmd_t *)SBBufPtr);
                    }
                    break;

                case CFE_ES_QUERY_ONE_CC:
                    if (CFE_ES_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_ES_QueryOneCmd_t)))
                    {
                        CFE_ES_QueryOneCmd((const CFE_ES_QueryOneCmd_t *)SBBufPtr);
                    }
                    break;

                case CFE_ES_QUERY_ALL_CC:
                    if (CFE_ES_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_ES_QueryAllCmd_t)))
                    {
                        CFE_ES_QueryAllCmd((const CFE_ES_QueryAllCmd_t *)SBBufPtr);
                    }
                    break;

                case CFE_ES_QUERY_ALL_TASKS_CC:
                    if (CFE_ES_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_ES_QueryAllTasksCmd_t)))
                    {
                        CFE_ES_QueryAllTasksCmd((const CFE_ES_QueryAllTasksCmd_t *)SBBufPtr);
                    }
                    break;

                case CFE_ES_CLEAR_SYS_LOG_CC:
                    if (CFE_ES_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_ES_ClearSysLogCmd_t)))
                    {
                        CFE_ES_ClearSysLogCmd((const CFE_ES_ClearSysLogCmd_t *)SBBufPtr);
                    }
                    break;

                case CFE_ES_WRITE_SYS_LOG_CC:
                    if (CFE_ES_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_ES_WriteSysLogCmd_t)))
                    {
                        CFE_ES_WriteSysLogCmd((const CFE_ES_WriteSysLogCmd_t *)SBBufPtr);
                    }
                    break;

                case CFE_ES_OVER_WRITE_SYS_LOG_CC:
                    if (CFE_ES_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_ES_OverWriteSysLogCmd_t)))
                    {
                        CFE_ES_OverWriteSysLogCmd((const CFE_ES_OverWriteSysLogCmd_t *)SBBufPtr);
                    }
                    break;

                case CFE_ES_CLEAR_ER_LOG_CC:
                    if (CFE_ES_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_ES_ClearERLogCmd_t)))
                    {
                        CFE_ES_ClearERLogCmd((const CFE_ES_ClearERLogCmd_t *)SBBufPtr);
                    }
                    break;

                case CFE_ES_WRITE_ER_LOG_CC:
                    if (CFE_ES_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_ES_WriteERLogCmd_t)))
                    {
                        CFE_ES_WriteERLogCmd((const CFE_ES_WriteERLogCmd_t *)SBBufPtr);
                    }
                    break;

                case CFE_ES_START_PERF_DATA_CC:
                    if (CFE_ES_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_ES_StartPerfDataCmd_t)))
                    {
                        CFE_ES_StartPerfDataCmd((const CFE_ES_StartPerfDataCmd_t *)SBBufPtr);
                    }
                    break;

                case CFE_ES_STOP_PERF_DATA_CC:
                    if (CFE_ES_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_ES_StopPerfDataCmd_t)))
                    {
                        CFE_ES_StopPerfDataCmd((const CFE_ES_StopPerfDataCmd_t *)SBBufPtr);
                    }
                    break;

                case CFE_ES_SET_PERF_FILTER_MASK_CC:
                    if (CFE_ES_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_ES_SetPerfFilterMaskCmd_t)))
                    {
                        CFE_ES_SetPerfFilterMaskCmd((const CFE_ES_SetPerfFilterMaskCmd_t *)SBBufPtr);
                    }
                    break;

                case CFE_ES_SET_PERF_TRIGGER_MASK_CC:
                    if (CFE_ES_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_ES_SetPerfTriggerMaskCmd_t)))
                    {
                        CFE_ES_SetPerfTriggerMaskCmd((const CFE_ES_SetPerfTriggerMaskCmd_t *)SBBufPtr);
                    }
                    break;

                case CFE_ES_RESET_PR_COUNT_CC:
                    if (CFE_ES_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_ES_ResetPRCountCmd_t)))
                    {
                        CFE_ES_ResetPRCountCmd((const CFE_ES_ResetPRCountCmd_t *)SBBufPtr);
                    }
                    break;

                case CFE_ES_SET_MAX_PR_COUNT_CC:
                    if (CFE_ES_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_ES_SetMaxPRCountCmd_t)))
                    {
                        CFE_ES_SetMaxPRCountCmd((const CFE_ES_SetMaxPRCountCmd_t *)SBBufPtr);
                    }
                    break;

                case CFE_ES_DELETE_CDS_CC:
                    if (CFE_ES_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_ES_DeleteCDSCmd_t)))
                    {
                        CFE_ES_DeleteCDSCmd((const CFE_ES_DeleteCDSCmd_t *)SBBufPtr);
                    }
                    break;

                case CFE_ES_SEND_MEM_POOL_STATS_CC:
                    if (CFE_ES_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_ES_SendMemPoolStatsCmd_t)))
                    {
                        CFE_ES_SendMemPoolStatsCmd((const CFE_ES_SendMemPoolStatsCmd_t *)SBBufPtr);
                    }
                    break;

                case CFE_ES_DUMP_CDS_REGISTRY_CC:
                    if (CFE_ES_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_ES_DumpCDSRegistryCmd_t)))
                    {
                        CFE_ES_DumpCDSRegistryCmd((const CFE_ES_DumpCDSRegistryCmd_t *)SBBufPtr);
                    }
                    break;

                default:
                    CFE_EVS_SendEvent(CFE_ES_CC1_ERR_EID, CFE_EVS_EventType_ERROR,
                                      "Invalid ground command code: ID = 0x%X, CC = %d",
                                      (unsigned int)CFE_SB_MsgIdToValue(MessageID), (int)CommandCode);
                    CFE_ES_Global.TaskData.CommandErrorCounter++;
                    break;
            }
            break;

        default:

            CFE_EVS_SendEvent(CFE_ES_MID_ERR_EID, CFE_EVS_EventType_ERROR, "Invalid command pipe message ID: 0x%X",
                              (unsigned int)CFE_SB_MsgIdToValue(MessageID));
            CFE_ES_Global.TaskData.CommandErrorCounter++;
            break;
    }
}
