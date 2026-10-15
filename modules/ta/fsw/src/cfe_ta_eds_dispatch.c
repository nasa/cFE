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

/*
**  File:
**    cfe_ta_eds_dispatch.c
**
** Task Affinity EDS message dispatcher
*/

/* Include Files */
#include "cfe_ta_module_all.h"
#include "cfe_ta_eds_dictionary.h"
#include "cfe_ta_eds_dispatcher.h"

#include <string.h>

/*
 * Define a lookup table for TA command codes mapping the EDS definitions
 * to the internal CFE_TA implementation functions.
 */
/* clang-format off */
static const EdsDispatchTable_EdsComponent_CFE_TA_Application_CFE_SB_Telecommand_t CFE_TA_TC_DISPATCH_TABLE =
{
    .CMD =
    {
        .NoopCmd_indication            = (int32_t (*)(const EdsDataType_CFE_TA_NoopCmd_t *)) CFE_TA_NoopCmd,
        .ResetCountersCmd_indication   = (int32_t (*)(const EdsDataType_CFE_TA_ResetCountersCmd_t *)) CFE_TA_ResetCountersCmd,
        .QueryAllTasksCmd_indication   = (int32_t (*)(const EdsDataType_CFE_TA_QueryAllTasksCmd_t *)) CFE_TA_QueryAllTasksCmd,
        .SetTaskAffinityCmd_indication = (int32_t (*)(const EdsDataType_CFE_TA_SetTaskAffinityCmd_t *)) CFE_TA_SetTaskAffinityCmd,
        .GetTaskAffinityCmd_indication = (int32_t (*)(const EdsDataType_CFE_TA_GetTaskAffinityCmd_t *)) CFE_TA_GetTaskAffinityCmd
    },
    .SEND_HK =
    {
        .indication = (int32_t (*)(const EdsDataType_CFE_TA_SendHkCmd_t *)) CFE_TA_ReportHousekeepingCmd
    }
};
/* clang-format on */

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TA_TaskPipe(const CFE_SB_Buffer_t *SBBufPtr)
{
    CFE_Status_t      Status;
    CFE_SB_MsgId_t    MsgId;
    CFE_MSG_Size_t    MsgSize;
    CFE_MSG_FcnCode_t MsgFc;

    /* Call the auto-generated EDS dispatcher */
    Status = EdsDispatch_EdsComponent_CFE_TA_Application_Telecommand(SBBufPtr, &CFE_TA_TC_DISPATCH_TABLE);

    if (Status == CFE_STATUS_BAD_COMMAND_CODE || Status == CFE_STATUS_WRONG_MSG_LENGTH
        || Status == CFE_STATUS_UNKNOWN_MSG_ID)
    {
        CFE_MSG_GetMsgId(&SBBufPtr->Msg, &MsgId);
        CFE_MSG_GetFcnCode(&SBBufPtr->Msg, &MsgFc);
        CFE_MSG_GetSize(&SBBufPtr->Msg, &MsgSize);

        /* Increment error counter for routing failures */
        CFE_TA_Global.CommandErrorCounter++;

        if (Status == CFE_STATUS_BAD_COMMAND_CODE)
        {
            CFE_EVS_SendEvent(CFE_TA_ERR_CC_EID,
                              CFE_EVS_EventType_ERROR,
                              "Invalid command code -- ID = 0x%08X, CC = %u",
                              (unsigned int)CFE_SB_MsgIdToValue(MsgId),
                              (unsigned int)MsgFc);
        }
        else if (Status == CFE_STATUS_WRONG_MSG_LENGTH)
        {
            CFE_EVS_SendEvent(CFE_TA_LEN_ERR_EID,
                              CFE_EVS_EventType_ERROR,
                              "Invalid msg length: ID = 0x%08X, CC = %u, Len = %u",
                              (unsigned int)CFE_SB_MsgIdToValue(MsgId),
                              (unsigned int)MsgFc,
                              (unsigned int)MsgSize);
        }
        else if (Status == CFE_STATUS_UNKNOWN_MSG_ID)
        {
            CFE_EVS_SendEvent(CFE_TA_ERR_MSGID_EID,
                              CFE_EVS_EventType_ERROR,
                              "Invalid command packet, Message ID = 0x%08X",
                              (unsigned int)CFE_SB_MsgIdToValue(MsgId));
        }
    }
}
