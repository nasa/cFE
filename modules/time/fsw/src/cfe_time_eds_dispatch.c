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
 * @brief
 *
 * CFE TIME dispatch implementation
 */

/*
** Required header files...
*/
#include "cfe_time_module_all.h"
#include "cfe_time_eds_dictionary.h"
#include "cfe_time_eds_dispatcher.h"

/*
 * Define a lookup table for ES command codes
 */
/* clang-format off */
static const EdsDispatchTable_EdsComponent_CFE_TIME_Application_CFE_SB_Telecommand_t CFE_TIME_TC_DISPATCH_TABLE =
{
    .CMD =
    {
        .AddOneHzAdjustmentCmd_indication = CFE_TIME_AddOneHzAdjustmentCmd,
        .AddAdjustCmd_indication          = CFE_TIME_AddAdjustCmd,
        .AddDelayCmd_indication           = CFE_TIME_AddDelayCmd,
        .SendDiagnosticCmd_indication     = CFE_TIME_SendDiagnosticTlm,
        .NoopCmd_indication               = CFE_TIME_NoopCmd,
        .ResetCountersCmd_indication      = CFE_TIME_ResetCountersCmd,
        .SetLeapSecondsCmd_indication     = CFE_TIME_SetLeapSecondsCmd,
        .SetMETCmd_indication             = CFE_TIME_SetMETCmd,
        .SetSignalCmd_indication          = CFE_TIME_SetSignalCmd,
        .SetSourceCmd_indication          = CFE_TIME_SetSourceCmd,
        .SetStateCmd_indication           = CFE_TIME_SetStateCmd,
        .SetSTCFCmd_indication            = CFE_TIME_SetSTCFCmd,
        .SetTimeCmd_indication            = CFE_TIME_SetTimeCmd,
        .SubOneHzAdjustmentCmd_indication = CFE_TIME_SubOneHzAdjustmentCmd,
        .SubAdjustCmd_indication          = CFE_TIME_SubAdjustCmd,
        .SubDelayCmd_indication           = CFE_TIME_SubDelayCmd
    },
    .SEND_HK =
    {
        .indication = CFE_TIME_SendHkCmd
    },
    .ONEHZ_CMD =
    {
        .indication = CFE_TIME_OneHzCmd
    },
#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
    .SEND_CMD =
    {
        .indication = CFE_TIME_ToneSendCmd
    },
#endif
    .TONE_CMD =
    {
        .indication = CFE_TIME_ToneSignalCmd
    },
    .DATA_CMD =
    {
        .indication = CFE_TIME_ToneDataCmd
    }
};
/* clang-format on */

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TIME_TaskPipe(const CFE_SB_Buffer_t *SBBufPtr)
{
    int32             Status;
    CFE_SB_MsgId_t    MsgId;
    CFE_MSG_FcnCode_t MsgFc;

    Status = EdsDispatch_EdsComponent_CFE_TIME_Application_Telecommand(SBBufPtr, &CFE_TIME_TC_DISPATCH_TABLE);

    /* These specific status codes require sending an event with the details */
    if (Status == CFE_STATUS_BAD_COMMAND_CODE || Status == CFE_STATUS_WRONG_MSG_LENGTH ||
        Status == CFE_STATUS_UNKNOWN_MSG_ID)
    {
        CFE_MSG_GetMsgId(&SBBufPtr->Msg, &MsgId);
        CFE_MSG_GetFcnCode(&SBBufPtr->Msg, &MsgFc);

        if (Status == CFE_STATUS_BAD_COMMAND_CODE)
        {
            CFE_TIME_Global.CommandErrorCounter++;
            CFE_EVS_SendEvent(CFE_TIME_CC_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Invalid command code -- ID = 0x%X, CC = %u", (unsigned int)CFE_SB_MsgIdToValue(MsgId),
                              (unsigned int)MsgFc);
        }
        else if (Status == CFE_STATUS_WRONG_MSG_LENGTH)
        {
            ++CFE_TIME_Global.CommandErrorCounter;
            CFE_EVS_SendEvent(CFE_TIME_LEN_ERR_EID, CFE_EVS_EventType_ERROR, "Invalid cmd length: ID = 0x%X, CC = %u",
                              (unsigned int)CFE_SB_MsgIdToValue(MsgId), (unsigned int)MsgFc);
        }
        else
        {
            CFE_EVS_SendEvent(CFE_TIME_ID_ERR_EID, CFE_EVS_EventType_ERROR, "Invalid message ID");
        }
    }
}
