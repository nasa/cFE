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
 *  @file
 *
 * Msg pipe dispatcher routines for CFE ES
 */

/*
 * Includes
 */
#include "cfe_es_module_all.h"
#include "cfe_es_eds_dictionary.h"
#include "cfe_es_eds_dispatcher.h"
#include "cfe_mission_eds_interface_parameters.h"

/*
 * Define a lookup table for ES command codes
 */
/* clang-format off */
static const EdsDispatchTable_EdsComponent_CFE_ES_Application_CFE_SB_Telecommand_t CFE_ES_TC_DISPATCH_TABLE =
{
    .CMD =
    {
        .NoopCmd_indication               = CFE_ES_NoopCmd,
        .ResetCountersCmd_indication      = CFE_ES_ResetCountersCmd,
        .RestartCmd_indication            = CFE_ES_RestartCmd,
        .StartAppCmd_indication           = CFE_ES_StartAppCmd,
        .StopAppCmd_indication            = CFE_ES_StopAppCmd,
        .RestartAppCmd_indication         = CFE_ES_RestartAppCmd,
        .ReloadAppCmd_indication          = CFE_ES_ReloadAppCmd,
        .QueryOneCmd_indication           = CFE_ES_QueryOneCmd,
        .QueryAllCmd_indication           = CFE_ES_QueryAllCmd,
        .QueryAllTasksCmd_indication      = CFE_ES_QueryAllTasksCmd,
        .ClearSysLogCmd_indication        = CFE_ES_ClearSysLogCmd,
        .WriteSysLogCmd_indication        = CFE_ES_WriteSysLogCmd,
        .OverWriteSysLogCmd_indication    = CFE_ES_OverWriteSysLogCmd,
        .ClearERLogCmd_indication         = CFE_ES_ClearERLogCmd,
        .WriteERLogCmd_indication         = CFE_ES_WriteERLogCmd,
        .StartPerfDataCmd_indication      = CFE_ES_StartPerfDataCmd,
        .StopPerfDataCmd_indication       = CFE_ES_StopPerfDataCmd,
        .SetPerfFilterMaskCmd_indication  = CFE_ES_SetPerfFilterMaskCmd,
        .SetPerfTriggerMaskCmd_indication = CFE_ES_SetPerfTriggerMaskCmd,
        .ResetPRCountCmd_indication       = CFE_ES_ResetPRCountCmd,
        .SetMaxPRCountCmd_indication      = CFE_ES_SetMaxPRCountCmd,
        .DeleteCDSCmd_indication          = CFE_ES_DeleteCDSCmd,
        .SendMemPoolStatsCmd_indication   = CFE_ES_SendMemPoolStatsCmd,
        .DumpCDSRegistryCmd_indication    = CFE_ES_DumpCDSRegistryCmd,
    },
    .SEND_HK =
    {
        .indication = CFE_ES_SendHkCmd
    }
};
/* clang-format on */

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_ES_TaskPipe(const CFE_SB_Buffer_t *SBBufPtr)
{
    int32             Status;
    CFE_SB_MsgId_t    MsgId;
    CFE_MSG_Size_t    MsgSize;
    CFE_MSG_FcnCode_t MsgFc;

    Status = EdsDispatch_EdsComponent_CFE_ES_Application_Telecommand(SBBufPtr, &CFE_ES_TC_DISPATCH_TABLE);

    /* These specific status codes require sending an event with the details */
    if (Status == CFE_STATUS_BAD_COMMAND_CODE || Status == CFE_STATUS_WRONG_MSG_LENGTH ||
        Status == CFE_STATUS_UNKNOWN_MSG_ID)
    {
        CFE_MSG_GetMsgId(&SBBufPtr->Msg, &MsgId);
        CFE_MSG_GetFcnCode(&SBBufPtr->Msg, &MsgFc);
        CFE_MSG_GetSize(&SBBufPtr->Msg, &MsgSize);

        CFE_ES_Global.TaskData.CommandErrorCounter++;

        if (Status == CFE_STATUS_UNKNOWN_MSG_ID)
        {
            CFE_EVS_SendEvent(CFE_ES_MID_ERR_EID, CFE_EVS_EventType_ERROR, "Invalid command pipe message ID: 0x%X",
                              (unsigned int)CFE_SB_MsgIdToValue(MsgId));
        }
        else if (Status == CFE_STATUS_WRONG_MSG_LENGTH)
        {
            CFE_EVS_SendEvent(CFE_ES_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Invalid length for command: ID = 0x%X, CC = %d, length = %u",
                              (unsigned int)CFE_SB_MsgIdToValue(MsgId), (int)MsgFc, (unsigned int)MsgSize);
        }
        else
        {
            CFE_EVS_SendEvent(CFE_ES_CC1_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Invalid ground command code: ID = 0x%X, CC = %d",
                              (unsigned int)CFE_SB_MsgIdToValue(MsgId), (int)MsgFc);
        }
    }
}
