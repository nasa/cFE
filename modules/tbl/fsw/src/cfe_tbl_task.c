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
** File: cfe_tbl_task.c
**
** Subsystem: cFE TBL Task
**
** Author: David Kobe (the Hammers Company, Inc.)
**
** Notes:
**
*/

/*
** Required header files
*/
#include "cfe_tbl_module_all.h"
#include "cfe_version.h"
#include "cfe_config.h" /* For version string construction */
#include "cfe_tbl_verify.h"

#include <string.h>

/*
** Table task global data
*/
CFE_TBL_Global_t CFE_TBL_Global;

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_TaskMain(void)
{
    int32            Status;
    CFE_SB_Buffer_t *SBBufPtr;

    CFE_ES_PerfLogEntry(CFE_MISSION_TBL_MAIN_PERF_ID);

    Status = CFE_TBL_TaskInit();

    if (Status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: Application Init Failed,RC=0x%08X\n", __func__, (unsigned int)Status);
        CFE_ES_PerfLogExit(CFE_MISSION_TBL_MAIN_PERF_ID);
        /* Note: CFE_ES_ExitApp will not return */
        CFE_ES_ExitApp(CFE_ES_RunStatus_CORE_APP_INIT_ERROR);
    }

    /*
     * Wait for other apps to start.
     * It is important that the core apps are present before this starts receiving
     * messages from the command pipe, as some of those handlers might depend on
     * the other core apps.
     */
    CFE_ES_WaitForSystemState(CFE_ES_SystemState_CORE_READY, CFE_PLATFORM_CORE_MAX_STARTUP_MSEC);

    /* Main loop */
    while (Status == CFE_SUCCESS)
    {
        /* Increment the Main task Execution Counter */
        CFE_ES_IncrementTaskCounter();

        CFE_ES_PerfLogExit(CFE_MISSION_TBL_MAIN_PERF_ID);

        /* Pend on receipt of packet */
        Status = CFE_SB_ReceiveBuffer(&SBBufPtr, CFE_TBL_Global.CmdPipe, CFE_SB_PEND_FOREVER);

        CFE_ES_PerfLogEntry(CFE_MISSION_TBL_MAIN_PERF_ID);

        if (Status == CFE_SUCCESS)
        {
            /* Process cmd pipe msg */
            CFE_TBL_TaskPipe(SBBufPtr);
        }
        else
        {
            CFE_ES_WriteToSysLog("%s: Error reading cmd pipe,RC=0x%08X\n", __func__, (unsigned int)Status);
        }

    } /* end while */

    /* while loop exits only if CFE_SB_ReceiveBuffer returns error */
    CFE_ES_ExitApp(CFE_ES_RunStatus_CORE_APP_RUNTIME_ERROR);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_TBL_TaskInit(void)
{
    int32 Status;
    char  VersionString[CFE_CFG_MAX_VERSION_STR_LEN];

    /*
    ** Initialize global Table Services data
    */
    CFE_TBL_InitData();

    /*
    ** Register event filter table
    */
    Status = CFE_EVS_Register(NULL, 0, 0);

    if (Status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: Call to CFE_EVS_Register Failed:RC=0x%08X\n", __func__, (unsigned int)Status);
        return Status;
    }

    /*
    ** Create Software Bus message pipe
    */
    Status = CFE_SB_CreatePipe(&CFE_TBL_Global.CmdPipe, CFE_TBL_TASK_PIPE_DEPTH, CFE_TBL_TASK_PIPE_NAME);
    if (Status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: Error creating cmd pipe:RC=0x%08X\n", __func__, (unsigned int)Status);
        return Status;
    }

    /*
    ** Subscribe to Housekeeping request commands
    */
    Status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(CFE_TBL_SEND_HK_MID), CFE_TBL_Global.CmdPipe);

    if (Status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: Error subscribing to HK Request:RC=0x%08X\n", __func__, (unsigned int)Status);
        return Status;
    }

    /*
    ** Subscribe to Table task ground command packets
    */
    Status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(CFE_TBL_CMD_MID), CFE_TBL_Global.CmdPipe);

    if (Status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: Error subscribing to gnd cmds:RC=0x%08X\n", __func__, (unsigned int)Status);
        return Status;
    }

    /*
    ** Task startup event message
    */
    CFE_Config_GetVersionString(VersionString, CFE_CFG_MAX_VERSION_STR_LEN, "cFE", CFE_SRC_VERSION, CFE_BUILD_CODENAME,
                                CFE_LAST_OFFICIAL);
    Status = CFE_EVS_SendEvent(CFE_TBL_INIT_INF_EID, CFE_EVS_EventType_INFORMATION, "cFE TBL Initialized: %s",
                               VersionString);

    if (Status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: Error sending init event:RC=0x%08X\n", __func__, (unsigned int)Status);
        return Status;
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_InitData(void)
{
    /* Get the assigned Application ID for the Table Services Task */
    CFE_ES_GetAppID(&CFE_TBL_Global.TableTaskAppId);

    /* Initialize Packet Headers */
    CFE_MSG_Init(CFE_MSG_PTR(CFE_TBL_Global.HkPacket.TelemetryHeader), CFE_SB_ValueToMsgId(CFE_TBL_HK_TLM_MID),
                 sizeof(CFE_TBL_Global.HkPacket));

    CFE_MSG_Init(CFE_MSG_PTR(CFE_TBL_Global.TblRegPacket.TelemetryHeader), CFE_SB_ValueToMsgId(CFE_TBL_REG_TLM_MID),
                 sizeof(CFE_TBL_Global.TblRegPacket));

    /* Message ID is set when sent, so OK as 0 here */
    CFE_MSG_Init(CFE_MSG_PTR(CFE_TBL_Global.NotifyMsg.CommandHeader), CFE_SB_INVALID_MSG_ID,
                 sizeof(CFE_TBL_Global.NotifyMsg));

    CFE_TBL_Global.LastValidationResultId = CFE_ResourceId_FromInteger(CFE_TBL_VALRESULTID_BASE);
    CFE_TBL_Global.LastDumpCtrlBlockId    = CFE_ResourceId_FromInteger(CFE_TBL_DUMPCTRLID_BASE);
}
