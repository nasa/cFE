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
**  File: cfe_evs_task.c
**
**  Title: Event Service API Management Control Interfaces
**
**  Purpose: This module defines the top level functions of the
**           cFE Event Service task defining the control, command,
**           and telemetry interfaces
**
*/

/* Include Files */
#include "cfe_evs_module_all.h" /* All EVS internal definitions and API */
#include "cfe_version.h"        /* cFE version definitions */
#include "cfe_config.h"         /* For version string construction */
#include "cfe_evs_verify.h"

#include <string.h>

#include "cfe_es_resetdata_typedef.h" /* Definition of CFE_ES_ResetData_t */

/* Global Data */
CFE_EVS_Global_t CFE_EVS_Global;

/* Defines */
#define CFE_EVS_PANIC_DELAY 500 /**< \brief Task delay before PSP panic */

/* Function Definitions */

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_EVS_EarlyInit(void)
{
    int32               OsStatus;
    int32               Status;
    int32               PspStatus;
    uint32              resetAreaSize = 0;
    cpuaddr             resetAreaAddr;
    CFE_ES_ResetData_t *CFE_EVS_ResetDataPtr = (CFE_ES_ResetData_t *)NULL;

    memset(&CFE_EVS_Global, 0, sizeof(CFE_EVS_Global));

    /* Initialize housekeeping packet */
    CFE_MSG_Init(CFE_MSG_PTR(CFE_EVS_Global.EVS_TlmPkt.TelemetryHeader), CFE_SB_ValueToMsgId(CFE_EVS_HK_TLM_MID),
                 sizeof(CFE_EVS_Global.EVS_TlmPkt));

    /* Elements stored in the hk packet that have non-zero default values */
    CFE_EVS_Global.EVS_TlmPkt.Payload.MessageFormatMode = CFE_PLATFORM_EVS_DEFAULT_MSG_FORMAT_MODE;
    CFE_EVS_Global.EVS_TlmPkt.Payload.OutputPort        = CFE_PLATFORM_EVS_PORT_DEFAULT;
    CFE_EVS_Global.EVS_TlmPkt.Payload.LogMode           = CFE_PLATFORM_EVS_DEFAULT_LOG_MODE;

    CFE_EVS_Global.EVS_EventBurstMax = CFE_PLATFORM_EVS_MAX_APP_EVENT_BURST;

    /* Get a pointer to the CFE reset area from the BSP */
    PspStatus = CFE_PSP_GetResetArea(&resetAreaAddr, &resetAreaSize);

    /* Panic on error */
    if (PspStatus != CFE_PSP_SUCCESS)
    {
        /* Can't log evs messages without the reset area */
        Status = CFE_EVS_RESET_AREA_POINTER;
        CFE_ES_WriteToSysLog("%s: Call to CFE_PSP_GetResetArea failed, RC=0x%08x\n", __func__, (unsigned int)PspStatus);

        /* Delay to allow message to be read */
        OS_TaskDelay(CFE_EVS_PANIC_DELAY);

        CFE_PSP_Panic(CFE_PSP_PANIC_MEMORY_ALLOC);
    }
    else if (resetAreaSize < sizeof(CFE_ES_ResetData_t))
    {
        /* Got the pointer but the size is wrong */
        Status = CFE_EVS_RESET_AREA_POINTER;
        CFE_ES_WriteToSysLog("%s: Unexpected size from CFE_PSP_GetResetArea: expected = 0x%08lX, actual = 0x%08lX\n",
                             __func__, (unsigned long)sizeof(CFE_ES_ResetData_t), (unsigned long)resetAreaSize);

        /* Delay to allow message to be read */
        OS_TaskDelay(CFE_EVS_PANIC_DELAY);

        CFE_PSP_Panic(CFE_PSP_PANIC_MEMORY_ALLOC);
    }
    else
    {
        CFE_EVS_ResetDataPtr = (CFE_ES_ResetData_t *)resetAreaAddr;
        /* Save pointer to the EVS portion of the CFE reset area */
        CFE_EVS_Global.EVS_LogPtr = &CFE_EVS_ResetDataPtr->EVS_Log;

        /* Create semaphore to serialize access to event log */
        OsStatus = OS_MutSemCreate(&CFE_EVS_Global.EVS_SharedDataMutexID, "CFE_EVS_DataMutex", 0);

        if (OsStatus != OS_SUCCESS)
        {
            CFE_ES_WriteToSysLog("%s: OS_MutSemCreate failed, RC=%ld\n", __func__, (long)OsStatus);

            /* Delay to allow message to be read */
            OS_TaskDelay(CFE_EVS_PANIC_DELAY);

            CFE_PSP_Panic(CFE_PSP_PANIC_STARTUP_SEM);
            Status = CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
        }
        else
        {
            /* Convert to CFE success type */
            Status = CFE_SUCCESS;
        }

        /* Report log as enabled */
        CFE_EVS_Global.EVS_TlmPkt.Payload.LogEnabled = true;

        /* Clear event log if power-on reset or bad contents */
        if (CFE_ES_GetResetType(NULL) == CFE_PSP_RST_TYPE_POWERON)
        {
            CFE_ES_WriteToSysLog("%s: Event Log cleared following power-on reset\n", __func__);
            EVS_ClearLog();
            CFE_EVS_Global.EVS_LogPtr->LogMode = CFE_PLATFORM_EVS_DEFAULT_LOG_MODE;
        }
        else if (((CFE_EVS_Global.EVS_LogPtr->LogMode != CFE_EVS_LogMode_OVERWRITE) &&
                  (CFE_EVS_Global.EVS_LogPtr->LogMode != CFE_EVS_LogMode_DISCARD)) ||
                 ((CFE_EVS_Global.EVS_LogPtr->LogFullFlag != false) &&
                  (CFE_EVS_Global.EVS_LogPtr->LogFullFlag != true)) ||
                 (CFE_EVS_Global.EVS_LogPtr->Next >= CFE_PLATFORM_EVS_LOG_MAX))
        {
            CFE_ES_WriteToSysLog("%s: Event Log cleared, n=%d, c=%d, f=%d, m=%d, o=%d\n", __func__,
                                 (int)CFE_EVS_Global.EVS_LogPtr->Next, (int)CFE_EVS_Global.EVS_LogPtr->LogCount,
                                 (int)CFE_EVS_Global.EVS_LogPtr->LogFullFlag, (int)CFE_EVS_Global.EVS_LogPtr->LogMode,
                                 (int)CFE_EVS_Global.EVS_LogPtr->LogOverflowCounter);
            EVS_ClearLog();
            CFE_EVS_Global.EVS_LogPtr->LogMode = CFE_PLATFORM_EVS_DEFAULT_LOG_MODE;
        }
        else
        {
            CFE_ES_WriteToSysLog("%s: Event Log restored, n=%d, c=%d, f=%d, m=%d, o=%d\n", __func__,
                                 (int)CFE_EVS_Global.EVS_LogPtr->Next, (int)CFE_EVS_Global.EVS_LogPtr->LogCount,
                                 (int)CFE_EVS_Global.EVS_LogPtr->LogFullFlag, (int)CFE_EVS_Global.EVS_LogPtr->LogMode,
                                 (int)CFE_EVS_Global.EVS_LogPtr->LogOverflowCounter);
        }
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_EVS_CleanUpApp(CFE_ES_AppId_t AppID)
{
    int32          Status = CFE_SUCCESS;
    EVS_AppData_t *AppDataPtr;

    /* Query and verify the caller's AppID */
    AppDataPtr = EVS_GetAppDataByID(AppID);
    if (AppDataPtr == NULL)
    {
        Status = CFE_EVS_APP_ILLEGAL_APP_ID;
    }
    else if (EVS_AppDataIsMatch(AppDataPtr, AppID))
    {
        EVS_AppDataSetFree(AppDataPtr);
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_EVS_TaskMain(void)
{
    int32            Status;
    CFE_SB_Buffer_t *SBBufPtr;

    CFE_ES_PerfLogEntry(CFE_MISSION_EVS_MAIN_PERF_ID);

    Status = CFE_EVS_TaskInit();

    if (Status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: Application Init Failed,RC=0x%08X\n", __func__, (unsigned int)Status);
        CFE_ES_PerfLogExit(CFE_MISSION_EVS_MAIN_PERF_ID);
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

        CFE_ES_PerfLogExit(CFE_MISSION_EVS_MAIN_PERF_ID);

        /* Pend on receipt of packet */
        Status = CFE_SB_ReceiveBuffer(&SBBufPtr, CFE_EVS_Global.EVS_CommandPipe, CFE_SB_PEND_FOREVER);

        CFE_ES_PerfLogEntry(CFE_MISSION_EVS_MAIN_PERF_ID);

        if (Status == CFE_SUCCESS)
        {
            /* Process cmd pipe msg */
            CFE_EVS_ProcessCommandPacket(SBBufPtr);
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
int32 CFE_EVS_TaskInit(void)
{
    int32          Status;
    CFE_ES_AppId_t AppID;
    char           VersionString[CFE_CFG_MAX_VERSION_STR_LEN];

    /* Query and verify the AppID */
    Status = CFE_ES_GetAppID(&AppID);
    if (Status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: Call to CFE_ES_GetAppID Failed:RC=0x%08X\n", __func__, (unsigned int)Status);
        return Status;
    }

    /* Register EVS task for event services */
    Status = CFE_EVS_Register(NULL, 0, CFE_EVS_EventFilter_BINARY);
    if (Status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: Call to CFE_EVS_Register Failed:RC=0x%08X\n", __func__, (unsigned int)Status);
        return Status;
    }

    /* Create software bus command pipe */
    Status = CFE_SB_CreatePipe(&CFE_EVS_Global.EVS_CommandPipe, CFE_EVS_PIPE_DEPTH, CFE_EVS_PIPE_NAME);
    if (Status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: Call to CFE_SB_CreatePipe Failed:RC=0x%08X\n", __func__, (unsigned int)Status);
        return Status;
    }

    /* Subscribe to command and telemetry requests coming in on the command pipe */
    Status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(CFE_EVS_CMD_MID), CFE_EVS_Global.EVS_CommandPipe);
    if (Status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: Subscribing to Cmds Failed:RC=0x%08X\n", __func__, (unsigned int)Status);
        return Status;
    }

    Status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(CFE_EVS_SEND_HK_MID), CFE_EVS_Global.EVS_CommandPipe);
    if (Status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: Subscribing to HK Request Failed:RC=0x%08X\n", __func__, (unsigned int)Status);
        return Status;
    }

    /* Write the AppID to the global location, now that the rest of initialization is done */
    CFE_EVS_Global.EVS_AppID = AppID;
    CFE_Config_GetVersionString(VersionString, CFE_CFG_MAX_VERSION_STR_LEN, "cFE", CFE_SRC_VERSION, CFE_BUILD_CODENAME,
                                CFE_LAST_OFFICIAL);
    EVS_SendEvent(CFE_EVS_STARTUP_EID, CFE_EVS_EventType_INFORMATION, "cFE EVS Initialized: %s", VersionString);

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_EVS_NoopCmd(const CFE_EVS_NoopCmd_t *data)
{
    char VersionString[CFE_CFG_MAX_VERSION_STR_LEN];
    CFE_Config_GetVersionString(VersionString, CFE_CFG_MAX_VERSION_STR_LEN, "cFE", CFE_SRC_VERSION, CFE_BUILD_CODENAME,
                                CFE_LAST_OFFICIAL);
    EVS_SendEvent(CFE_EVS_NOOP_EID, CFE_EVS_EventType_INFORMATION, "No-op Cmd Rcvd: %s", VersionString);
    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_EVS_ClearLogCmd(const CFE_EVS_ClearLogCmd_t *data)
{
    EVS_ClearLog();
    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_EVS_SendHkCmd(const CFE_EVS_SendHkCmd_t *data)
{
    uint32                i, j;
    EVS_AppData_t *       AppDataPtr;
    CFE_EVS_AppTlmData_t *AppTlmDataPtr;

    /* Copy hk variables that are maintained in the event log */
    CFE_EVS_Global.EVS_TlmPkt.Payload.LogFullFlag        = CFE_EVS_Global.EVS_LogPtr->LogFullFlag;
    CFE_EVS_Global.EVS_TlmPkt.Payload.LogMode            = CFE_EVS_Global.EVS_LogPtr->LogMode;
    CFE_EVS_Global.EVS_TlmPkt.Payload.LogOverflowCounter = CFE_EVS_Global.EVS_LogPtr->LogOverflowCounter;

    /* Write event state data for registered apps to telemetry packet */
    AppDataPtr    = CFE_EVS_Global.AppData;
    AppTlmDataPtr = CFE_EVS_Global.EVS_TlmPkt.Payload.AppData;
    for (i = 0, j = 0; j < CFE_MISSION_ES_MAX_APPLICATIONS && i < CFE_PLATFORM_ES_MAX_APPLICATIONS; i++)
    {
        if (EVS_AppDataIsUsed(AppDataPtr))
        {
            AppTlmDataPtr->AppID                      = EVS_AppDataGetID(AppDataPtr);
            AppTlmDataPtr->AppEnableStatus            = AppDataPtr->ActiveFlag;
            AppTlmDataPtr->AppMessageSentCounter      = AppDataPtr->EventCount;
            AppTlmDataPtr->AppMessageSquelchedCounter = AppDataPtr->SquelchedCount;

            ++j;
            ++AppTlmDataPtr;
        }
        ++AppDataPtr;
    }

    /* Clear unused portion of event state data in telemetry packet */
    for (i = j; i < CFE_MISSION_ES_MAX_APPLICATIONS; i++)
    {
        AppTlmDataPtr->AppID                      = CFE_ES_APPID_UNDEFINED;
        AppTlmDataPtr->AppEnableStatus            = false;
        AppTlmDataPtr->AppMessageSentCounter      = 0;
        AppTlmDataPtr->AppMessageSquelchedCounter = 0;
    }

    CFE_SB_TimeStampMsg(CFE_MSG_PTR(CFE_EVS_Global.EVS_TlmPkt.TelemetryHeader));

    CFE_SB_TransmitMsg(CFE_MSG_PTR(CFE_EVS_Global.EVS_TlmPkt.TelemetryHeader), true);

    return CFE_STATUS_NO_COUNTER_INCREMENT;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_EVS_ResetCountersCmd(const CFE_EVS_ResetCountersCmd_t *data)
{
    /* Status of commands processed by EVS task */
    CFE_EVS_Global.EVS_TlmPkt.Payload.CommandCounter      = 0;
    CFE_EVS_Global.EVS_TlmPkt.Payload.CommandErrorCounter = 0;

    /* EVS telemetry counters */
    CFE_EVS_Global.EVS_TlmPkt.Payload.MessageSendCounter     = 0;
    CFE_EVS_Global.EVS_TlmPkt.Payload.MessageTruncCounter    = 0;
    CFE_EVS_Global.EVS_TlmPkt.Payload.UnregisteredAppCounter = 0;

    EVS_SendEvent(CFE_EVS_RSTCNT_EID, CFE_EVS_EventType_DEBUG, "Reset Counters Command Received");

    /* NOTE: Historically the reset counters command does _NOT_ increment the command counter */

    return CFE_STATUS_NO_COUNTER_INCREMENT;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_EVS_SetFilterCmd(const CFE_EVS_SetFilterCmd_t *data)
{
    const CFE_EVS_AppNameEventIDMaskCmd_Payload_t *CmdPtr = &data->Payload;
    EVS_BinFilter_t *                              FilterPtr;
    int32                                          Status;
    EVS_AppData_t *                                AppDataPtr;
    char                                           LocalName[OS_MAX_API_NAME];

    /* Copy appname from command, ensures NULL termination */
    CFE_SB_MessageStringGet(LocalName, (char *)CmdPtr->AppName, NULL, sizeof(LocalName), sizeof(CmdPtr->AppName));

    /* Retrieve application data */
    Status = EVS_GetApplicationInfo(&AppDataPtr, LocalName);

    if (Status == CFE_SUCCESS)
    {
        FilterPtr = EVS_FindEventID(CmdPtr->EventID, AppDataPtr->BinFilters);

        if (FilterPtr != NULL)
        {
            /* Set application filter mask */
            FilterPtr->Mask = CmdPtr->Mask;

            EVS_SendEvent(CFE_EVS_SETFILTERMSK_EID, CFE_EVS_EventType_DEBUG,
                          "Set Filter Mask Command Received with AppName=%s, EventID=0x%08x, Mask=0x%04x", LocalName,
                          (unsigned int)CmdPtr->EventID, (unsigned int)CmdPtr->Mask);
        }
        else
        {
            EVS_SendEvent(CFE_EVS_ERR_EVTIDNOREGS_EID, CFE_EVS_EventType_ERROR,
                          "%s Event ID %d not registered for filtering: CC = %lu ", LocalName, (int)CmdPtr->EventID,
                          (long unsigned int)CFE_EVS_SET_FILTER_CC);

            Status = CFE_EVS_EVT_NOT_REGISTERED;
        }
    }
    else if (Status == CFE_EVS_APP_NOT_REGISTERED)
    {
        EVS_SendEvent(CFE_EVS_ERR_APPNOREGS_EID, CFE_EVS_EventType_ERROR, "%s not registered with EVS: CC = %lu",
                      LocalName, (long unsigned int)CFE_EVS_SET_FILTER_CC);
    }
    else if (Status == CFE_EVS_APP_ILLEGAL_APP_ID)
    {
        EVS_SendEvent(CFE_EVS_ERR_ILLAPPIDRANGE_EID, CFE_EVS_EventType_ERROR,
                      "Illegal application ID retrieved for %s: CC = %lu", LocalName,
                      (long unsigned int)CFE_EVS_SET_FILTER_CC);
    }
    else
    {
        EVS_SendEvent(CFE_EVS_ERR_NOAPPIDFOUND_EID, CFE_EVS_EventType_ERROR,
                      "Unable to retrieve application ID for %s: CC = %lu", LocalName,
                      (long unsigned int)CFE_EVS_SET_FILTER_CC);
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_EVS_EnablePortsCmd(const CFE_EVS_EnablePortsCmd_t *data)
{
    const CFE_EVS_BitMaskCmd_Payload_t *CmdPtr = &data->Payload;
    int32                               ReturnCode;

    /* Need to check for an out of range bitmask, since oue bit masks are only 4 bits */
    if (CmdPtr->BitMask == 0x0 || CmdPtr->BitMask > 0x0F)
    {
        EVS_SendEvent(CFE_EVS_ERR_INVALID_BITMASK_EID, CFE_EVS_EventType_ERROR,
                      "Bit Mask = 0x%08x out of range: CC = %lu", (unsigned int)CmdPtr->BitMask,
                      (long unsigned int)CFE_EVS_ENABLE_PORTS_CC);
        ReturnCode = CFE_EVS_INVALID_PARAMETER;
    }
    else
    {
        /* Process command data */
        CFE_EVS_Global.EVS_TlmPkt.Payload.OutputPort |= CmdPtr->BitMask;

        EVS_SendEvent(CFE_EVS_ENAPORT_EID, CFE_EVS_EventType_DEBUG,
                      "Enable Ports Command Received with Port Bit Mask = 0x%02x", (unsigned int)CmdPtr->BitMask);
        ReturnCode = CFE_SUCCESS;
    }

    return ReturnCode;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_EVS_DisablePortsCmd(const CFE_EVS_DisablePortsCmd_t *data)
{
    const CFE_EVS_BitMaskCmd_Payload_t *CmdPtr = &data->Payload;
    int32                               ReturnCode;

    /* Need to check for an out of range bitmask, since oue bit masks are only 4 bits */
    if (CmdPtr->BitMask == 0x0 || CmdPtr->BitMask > 0x0F)
    {
        EVS_SendEvent(CFE_EVS_ERR_INVALID_BITMASK_EID, CFE_EVS_EventType_ERROR,
                      "Bit Mask = 0x%08x out of range: CC = %lu", (unsigned int)CmdPtr->BitMask,
                      (long unsigned int)CFE_EVS_DISABLE_PORTS_CC);
        ReturnCode = CFE_EVS_INVALID_PARAMETER;
    }
    else
    {
        /* Process command data */
        CFE_EVS_Global.EVS_TlmPkt.Payload.OutputPort &= ~CmdPtr->BitMask;

        EVS_SendEvent(CFE_EVS_DISPORT_EID, CFE_EVS_EventType_DEBUG,
                      "Disable Ports Command Received with Port Bit Mask = 0x%02x", (unsigned int)CmdPtr->BitMask);

        ReturnCode = CFE_SUCCESS;
    }

    return ReturnCode;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_EVS_EnableEventTypeCmd(const CFE_EVS_EnableEventTypeCmd_t *data)
{
    uint32                              i;
    const CFE_EVS_BitMaskCmd_Payload_t *CmdPtr = &data->Payload;
    int32                               ReturnCode;
    EVS_AppData_t *                     AppDataPtr;

    /* Need to check for an out of range bitmask, since our bit masks are only 4 bits */
    if (CmdPtr->BitMask == 0x0 || CmdPtr->BitMask > 0x0F)
    {
        EVS_SendEvent(CFE_EVS_ERR_INVALID_BITMASK_EID, CFE_EVS_EventType_ERROR,
                      "Bit Mask = 0x%08x out of range: CC = %lu", (unsigned int)CmdPtr->BitMask,
                      (long unsigned int)CFE_EVS_ENABLE_EVENT_TYPE_CC);
        ReturnCode = CFE_EVS_INVALID_PARAMETER;
    }
    else
    {
        AppDataPtr = CFE_EVS_Global.AppData;
        for (i = 0; i < CFE_PLATFORM_ES_MAX_APPLICATIONS; i++)
        {
            /* Make sure application is registered for event services */
            if (EVS_AppDataIsUsed(AppDataPtr))
            {
                EVS_EnableTypes(AppDataPtr, CmdPtr->BitMask);
            }
            ++AppDataPtr;
        }

        EVS_SendEvent(CFE_EVS_ENAEVTTYPE_EID, CFE_EVS_EventType_DEBUG,
                      "Enable Event Type Command Received with Event Type Bit Mask = 0x%02x",
                      (unsigned int)CmdPtr->BitMask);

        ReturnCode = CFE_SUCCESS;
    }

    return ReturnCode;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_EVS_DisableEventTypeCmd(const CFE_EVS_DisableEventTypeCmd_t *data)
{
    uint32                              i;
    const CFE_EVS_BitMaskCmd_Payload_t *CmdPtr = &data->Payload;
    int32                               ReturnCode;
    EVS_AppData_t *                     AppDataPtr;

    /* Need to check for an out of range bitmask, since our bit masks are only 4 bits */
    if (CmdPtr->BitMask == 0x0 || CmdPtr->BitMask > 0x0F)
    {
        EVS_SendEvent(CFE_EVS_ERR_INVALID_BITMASK_EID, CFE_EVS_EventType_ERROR,
                      "Bit Mask = 0x%08x out of range: CC = %lu", (unsigned int)CmdPtr->BitMask,
                      (long unsigned int)CFE_EVS_DISABLE_EVENT_TYPE_CC);
        ReturnCode = CFE_EVS_INVALID_PARAMETER;
    }

    else
    {
        AppDataPtr = CFE_EVS_Global.AppData;
        for (i = 0; i < CFE_PLATFORM_ES_MAX_APPLICATIONS; i++)
        {
            /* Make sure application is registered for event services */
            if (EVS_AppDataIsUsed(AppDataPtr))
            {
                EVS_DisableTypes(AppDataPtr, CmdPtr->BitMask);
            }
            ++AppDataPtr;
        }

        EVS_SendEvent(CFE_EVS_DISEVTTYPE_EID, CFE_EVS_EventType_DEBUG,
                      "Disable Event Type Command Received with Event Type Bit Mask = 0x%02x",
                      (unsigned int)CmdPtr->BitMask);

        ReturnCode = CFE_SUCCESS;
    }

    return ReturnCode;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_EVS_SetEventFormatModeCmd(const CFE_EVS_SetEventFormatModeCmd_t *data)
{
    const CFE_EVS_SetEventFormatMode_Payload_t *CmdPtr = &data->Payload;
    int32                                       Status;

    if ((CmdPtr->MsgFormat == CFE_EVS_MsgFormat_SHORT) || (CmdPtr->MsgFormat == CFE_EVS_MsgFormat_LONG))
    {
        CFE_EVS_Global.EVS_TlmPkt.Payload.MessageFormatMode = CmdPtr->MsgFormat;

        EVS_SendEvent(CFE_EVS_SETEVTFMTMOD_EID, CFE_EVS_EventType_DEBUG,
                      "Set Event Format Mode Command Received with Mode = 0x%02x", (unsigned int)CmdPtr->MsgFormat);
        Status = CFE_SUCCESS;
    }
    else
    {
        EVS_SendEvent(CFE_EVS_ERR_ILLEGALFMTMOD_EID, CFE_EVS_EventType_ERROR,
                      "Set Event Format Mode Command: Invalid Event Format Mode = 0x%02x",
                      (unsigned int)CmdPtr->MsgFormat);
        Status = CFE_EVS_INVALID_PARAMETER;
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_EVS_EnableAppEventTypeCmd(const CFE_EVS_EnableAppEventTypeCmd_t *data)
{
    const CFE_EVS_AppNameBitMaskCmd_Payload_t *CmdPtr = &data->Payload;
    EVS_AppData_t *                            AppDataPtr;
    int32                                      Status;
    char                                       LocalName[OS_MAX_API_NAME];

    /* Copy appname from command, ensures NULL termination */
    CFE_SB_MessageStringGet(LocalName, (char *)CmdPtr->AppName, NULL, sizeof(LocalName), sizeof(CmdPtr->AppName));

    /* Retrieve application data */
    Status = EVS_GetApplicationInfo(&AppDataPtr, LocalName);

    if (Status == CFE_SUCCESS)
    {
        /* Need to check for an out of range bitmask, since our bit masks are only 4 bits */
        if (CmdPtr->BitMask == 0x0 || CmdPtr->BitMask > 0x0F)
        {
            EVS_SendEvent(CFE_EVS_ERR_INVALID_BITMASK_EID, CFE_EVS_EventType_ERROR,
                          "Bit Mask = 0x%08x out of range: CC = %lu", (unsigned int)CmdPtr->BitMask,
                          (long unsigned int)CFE_EVS_ENABLE_APP_EVENT_TYPE_CC);
            Status = CFE_EVS_INVALID_PARAMETER;
        }
        else
        {
            EVS_EnableTypes(AppDataPtr, CmdPtr->BitMask);
        }
    }
    else if (Status == CFE_EVS_APP_NOT_REGISTERED)
    {
        EVS_SendEvent(CFE_EVS_ERR_APPNOREGS_EID, CFE_EVS_EventType_ERROR, "%s not registered with EVS: CC = %lu",
                      LocalName, (long unsigned int)CFE_EVS_ENABLE_APP_EVENT_TYPE_CC);
    }
    else if (Status == CFE_EVS_APP_ILLEGAL_APP_ID)
    {
        EVS_SendEvent(CFE_EVS_ERR_ILLAPPIDRANGE_EID, CFE_EVS_EventType_ERROR,
                      "Illegal application ID retrieved for %s: CC = %lu", LocalName,
                      (long unsigned int)CFE_EVS_ENABLE_APP_EVENT_TYPE_CC);
    }
    else
    {
        EVS_SendEvent(CFE_EVS_ERR_NOAPPIDFOUND_EID, CFE_EVS_EventType_ERROR,
                      "Unable to retrieve application ID for %s: CC = %lu", LocalName,
                      (long unsigned int)CFE_EVS_ENABLE_APP_EVENT_TYPE_CC);
    }

    if (Status == CFE_SUCCESS)
    {
        EVS_SendEvent(CFE_EVS_ENAAPPEVTTYPE_EID, CFE_EVS_EventType_DEBUG,
                      "Enable App Event Type Command Received with AppName = %s, EventType Bit Mask = 0x%02x",
                      LocalName, CmdPtr->BitMask);
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_EVS_DisableAppEventTypeCmd(const CFE_EVS_DisableAppEventTypeCmd_t *data)
{
    EVS_AppData_t *                            AppDataPtr;
    const CFE_EVS_AppNameBitMaskCmd_Payload_t *CmdPtr = &data->Payload;
    int32                                      Status;
    char                                       LocalName[OS_MAX_API_NAME];

    /* Copy appname from command, ensures NULL termination */
    CFE_SB_MessageStringGet(LocalName, (char *)CmdPtr->AppName, NULL, sizeof(LocalName), sizeof(CmdPtr->AppName));

    /* Retrieve application data */
    Status = EVS_GetApplicationInfo(&AppDataPtr, LocalName);

    if (Status == CFE_SUCCESS)
    {
        /* Need to check for an out of range bitmask, since our bit masks are only 4 bits */
        if (CmdPtr->BitMask == 0x0 || CmdPtr->BitMask > 0x0F)
        {
            EVS_SendEvent(CFE_EVS_ERR_INVALID_BITMASK_EID, CFE_EVS_EventType_ERROR,
                          "Bit Mask = 0x%08x out of range: CC = %lu", (unsigned int)CmdPtr->BitMask,
                          (long unsigned int)CFE_EVS_DISABLE_APP_EVENT_TYPE_CC);
            Status = CFE_EVS_INVALID_PARAMETER;
        }
        else
        {
            EVS_DisableTypes(AppDataPtr, CmdPtr->BitMask);
        }
    }
    else if (Status == CFE_EVS_APP_NOT_REGISTERED)
    {
        EVS_SendEvent(CFE_EVS_ERR_APPNOREGS_EID, CFE_EVS_EventType_ERROR, "%s not registered with EVS,: CC = %lu",
                      LocalName, (long unsigned int)CFE_EVS_DISABLE_APP_EVENT_TYPE_CC);
    }
    else if (Status == CFE_EVS_APP_ILLEGAL_APP_ID)
    {
        EVS_SendEvent(CFE_EVS_ERR_ILLAPPIDRANGE_EID, CFE_EVS_EventType_ERROR,
                      "Illegal application ID retrieved for %s: CC = %lu", LocalName,
                      (long unsigned int)CFE_EVS_DISABLE_APP_EVENT_TYPE_CC);
    }
    else
    {
        EVS_SendEvent(CFE_EVS_ERR_NOAPPIDFOUND_EID, CFE_EVS_EventType_ERROR,
                      "Unable to retrieve application ID for %s: CC = %lu", LocalName,
                      (long unsigned int)CFE_EVS_DISABLE_APP_EVENT_TYPE_CC);
    }

    if (Status == CFE_SUCCESS)
    {
        EVS_SendEvent(CFE_EVS_DISAPPENTTYPE_EID, CFE_EVS_EventType_DEBUG,
                      "Disable App Event Type Command Received with AppName = %s, EventType Bit Mask = 0x%02x",
                      LocalName, (unsigned int)CmdPtr->BitMask);
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_EVS_EnableAppEventsCmd(const CFE_EVS_EnableAppEventsCmd_t *data)
{
    EVS_AppData_t *                     AppDataPtr;
    const CFE_EVS_AppNameCmd_Payload_t *CmdPtr = &data->Payload;
    int32                               Status;
    char                                LocalName[OS_MAX_API_NAME];

    /* Copy appname from command, ensures NULL termination */
    CFE_SB_MessageStringGet(LocalName, (char *)CmdPtr->AppName, NULL, sizeof(LocalName), sizeof(CmdPtr->AppName));

    /* Retrieve application data */
    Status = EVS_GetApplicationInfo(&AppDataPtr, LocalName);

    if (Status == CFE_SUCCESS)
    {
        AppDataPtr->ActiveFlag = true;

        EVS_SendEvent(CFE_EVS_ENAAPPEVT_EID, CFE_EVS_EventType_DEBUG,
                      "Enable App Events Command Received with AppName = %s", LocalName);
    }
    else if (Status == CFE_EVS_APP_NOT_REGISTERED)
    {
        EVS_SendEvent(CFE_EVS_ERR_APPNOREGS_EID, CFE_EVS_EventType_ERROR, "%s not registered with EVS: CC = %lu",
                      LocalName, (long unsigned int)CFE_EVS_ENABLE_APP_EVENTS_CC);
    }
    else if (Status == CFE_EVS_APP_ILLEGAL_APP_ID)
    {
        EVS_SendEvent(CFE_EVS_ERR_ILLAPPIDRANGE_EID, CFE_EVS_EventType_ERROR,
                      "Illegal application ID retrieved for %s: CC = %lu", LocalName,
                      (long unsigned int)CFE_EVS_ENABLE_APP_EVENTS_CC);
    }
    else
    {
        EVS_SendEvent(CFE_EVS_ERR_NOAPPIDFOUND_EID, CFE_EVS_EventType_ERROR,
                      "Unable to retrieve application ID for %s: CC = %lu", LocalName,
                      (long unsigned int)CFE_EVS_ENABLE_APP_EVENTS_CC);
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_EVS_DisableAppEventsCmd(const CFE_EVS_DisableAppEventsCmd_t *data)
{
    EVS_AppData_t *                     AppDataPtr;
    const CFE_EVS_AppNameCmd_Payload_t *CmdPtr = &data->Payload;
    int32                               Status;
    char                                LocalName[OS_MAX_API_NAME];

    /* Copy appname from command, ensures NULL termination */
    CFE_SB_MessageStringGet(LocalName, (char *)CmdPtr->AppName, NULL, sizeof(LocalName), sizeof(CmdPtr->AppName));

    /* Retrieve application data */
    Status = EVS_GetApplicationInfo(&AppDataPtr, LocalName);

    if (Status == CFE_SUCCESS)
    {
        AppDataPtr->ActiveFlag = false;

        EVS_SendEvent(CFE_EVS_DISAPPEVT_EID, CFE_EVS_EventType_DEBUG,
                      "Disable App Events Command Received with AppName = %s", LocalName);
    }
    else if (Status == CFE_EVS_APP_NOT_REGISTERED)
    {
        EVS_SendEvent(CFE_EVS_ERR_APPNOREGS_EID, CFE_EVS_EventType_ERROR, "%s not registered with EVS: CC = %lu",
                      LocalName, (long unsigned int)CFE_EVS_DISABLE_APP_EVENTS_CC);
    }
    else if (Status == CFE_EVS_APP_ILLEGAL_APP_ID)
    {
        EVS_SendEvent(CFE_EVS_ERR_ILLAPPIDRANGE_EID, CFE_EVS_EventType_ERROR,
                      "Illegal application ID retrieved for %s: CC = %lu", LocalName,
                      (long unsigned int)CFE_EVS_DISABLE_APP_EVENTS_CC);
    }
    else
    {
        EVS_SendEvent(CFE_EVS_ERR_NOAPPIDFOUND_EID, CFE_EVS_EventType_ERROR,
                      "Disable App Events Command: Unable to retrieve application ID for %s: CC = %lu", LocalName,
                      (long unsigned int)CFE_EVS_DISABLE_APP_EVENTS_CC);
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_EVS_ResetAppCounterCmd(const CFE_EVS_ResetAppCounterCmd_t *data)
{
    EVS_AppData_t *                     AppDataPtr;
    const CFE_EVS_AppNameCmd_Payload_t *CmdPtr = &data->Payload;
    int32                               Status;
    char                                LocalName[OS_MAX_API_NAME];

    /* Copy appname from command, ensures NULL termination */
    CFE_SB_MessageStringGet(LocalName, (char *)CmdPtr->AppName, NULL, sizeof(LocalName), sizeof(CmdPtr->AppName));

    /* Retrieve application data */
    Status = EVS_GetApplicationInfo(&AppDataPtr, LocalName);

    if (Status == CFE_SUCCESS)
    {
        AppDataPtr->EventCount     = 0;
        AppDataPtr->SquelchedCount = 0;

        EVS_SendEvent(CFE_EVS_RSTEVTCNT_EID, CFE_EVS_EventType_DEBUG,
                      "Reset Event Counter Command Received with AppName = %s", LocalName);
    }
    else if (Status == CFE_EVS_APP_NOT_REGISTERED)
    {
        EVS_SendEvent(CFE_EVS_ERR_APPNOREGS_EID, CFE_EVS_EventType_ERROR, "%s not registered with EVS: CC = %lu",
                      LocalName, (long unsigned int)CFE_EVS_RESET_APP_COUNTER_CC);
    }
    else if (Status == CFE_EVS_APP_ILLEGAL_APP_ID)
    {
        EVS_SendEvent(CFE_EVS_ERR_ILLAPPIDRANGE_EID, CFE_EVS_EventType_ERROR,
                      "Illegal application ID retrieved for %s: CC = %lu", LocalName,
                      (long unsigned int)CFE_EVS_RESET_APP_COUNTER_CC);
    }
    else
    {
        EVS_SendEvent(CFE_EVS_ERR_NOAPPIDFOUND_EID, CFE_EVS_EventType_ERROR,
                      "Reset Event Counter Command: Unable to retrieve application ID for %s: CC = %lu", LocalName,
                      (long unsigned int)CFE_EVS_RESET_APP_COUNTER_CC);
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_EVS_ResetFilterCmd(const CFE_EVS_ResetFilterCmd_t *data)
{
    const CFE_EVS_AppNameEventIDCmd_Payload_t *CmdPtr = &data->Payload;
    EVS_BinFilter_t *                          FilterPtr;
    int32                                      Status;
    EVS_AppData_t *                            AppDataPtr;
    char                                       LocalName[OS_MAX_API_NAME];

    /* Copy appname from command, ensures NULL termination */
    CFE_SB_MessageStringGet(LocalName, (char *)CmdPtr->AppName, NULL, sizeof(LocalName), sizeof(CmdPtr->AppName));

    /* Retrieve application data */
    Status = EVS_GetApplicationInfo(&AppDataPtr, LocalName);

    if (Status == CFE_SUCCESS)
    {
        FilterPtr = EVS_FindEventID(CmdPtr->EventID, AppDataPtr->BinFilters);

        if (FilterPtr != NULL)
        {
            FilterPtr->Count = 0;

            EVS_SendEvent(CFE_EVS_RSTFILTER_EID, CFE_EVS_EventType_DEBUG,
                          "Reset Filter Command Received with AppName = %s, EventID = 0x%08x", LocalName,
                          (unsigned int)CmdPtr->EventID);
        }
        else
        {
            EVS_SendEvent(CFE_EVS_ERR_EVTIDNOREGS_EID, CFE_EVS_EventType_ERROR,
                          "%s Event ID %d not registered for filtering: CC = %lu", LocalName, (int)CmdPtr->EventID,
                          (long unsigned int)CFE_EVS_RESET_FILTER_CC);

            Status = CFE_EVS_EVT_NOT_REGISTERED;
        }
    }
    else if (Status == CFE_EVS_APP_NOT_REGISTERED)
    {
        EVS_SendEvent(CFE_EVS_ERR_APPNOREGS_EID, CFE_EVS_EventType_ERROR, "%s not registered with EVS: CC = %lu",
                      LocalName, (long unsigned int)CFE_EVS_RESET_FILTER_CC);
    }
    else if (Status == CFE_EVS_APP_ILLEGAL_APP_ID)
    {
        EVS_SendEvent(CFE_EVS_ERR_ILLAPPIDRANGE_EID, CFE_EVS_EventType_ERROR,
                      "Illegal application ID retrieved for %s: CC = %lu", LocalName,
                      (long unsigned int)CFE_EVS_RESET_FILTER_CC);
    }
    else
    {
        EVS_SendEvent(CFE_EVS_ERR_NOAPPIDFOUND_EID, CFE_EVS_EventType_ERROR,
                      "Unable to retrieve application ID for %s: CC = %lu", LocalName,
                      (long unsigned int)CFE_EVS_RESET_FILTER_CC);
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_EVS_ResetAllFiltersCmd(const CFE_EVS_ResetAllFiltersCmd_t *data)
{
    EVS_AppData_t *                     AppDataPtr;
    const CFE_EVS_AppNameCmd_Payload_t *CmdPtr = &data->Payload;
    int32                               Status;
    uint32                              i;
    char                                LocalName[OS_MAX_API_NAME];

    /* Copy appname from command, ensures NULL termination */
    CFE_SB_MessageStringGet(LocalName, (char *)CmdPtr->AppName, NULL, sizeof(LocalName), sizeof(CmdPtr->AppName));

    /* Retrieve application data */
    Status = EVS_GetApplicationInfo(&AppDataPtr, LocalName);

    if (Status == CFE_SUCCESS)
    {
        for (i = 0; i < CFE_PLATFORM_EVS_MAX_EVENT_FILTERS; i++)
        {
            AppDataPtr->BinFilters[i].Count = 0;
        }

        EVS_SendEvent(CFE_EVS_RSTALLFILTER_EID, CFE_EVS_EventType_DEBUG,
                      "Reset All Filters Command Received with AppName = %s", LocalName);
    }
    else if (Status == CFE_EVS_APP_NOT_REGISTERED)
    {
        EVS_SendEvent(CFE_EVS_ERR_APPNOREGS_EID, CFE_EVS_EventType_ERROR, "%s not registered with EVS: CC = %lu",
                      LocalName, (long unsigned int)CFE_EVS_RESET_ALL_FILTERS_CC);
    }
    else if (Status == CFE_EVS_APP_ILLEGAL_APP_ID)
    {
        EVS_SendEvent(CFE_EVS_ERR_ILLAPPIDRANGE_EID, CFE_EVS_EventType_ERROR,
                      "Illegal application ID retrieved for %s: CC = %lu", LocalName,
                      (long unsigned int)CFE_EVS_RESET_ALL_FILTERS_CC);
    }
    else
    {
        EVS_SendEvent(CFE_EVS_ERR_NOAPPIDFOUND_EID, CFE_EVS_EventType_ERROR,
                      "Unable to retrieve application ID for %s: CC = %lu", LocalName,
                      (long unsigned int)CFE_EVS_RESET_ALL_FILTERS_CC);
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_EVS_AddEventFilterCmd(const CFE_EVS_AddEventFilterCmd_t *data)
{
    const CFE_EVS_AppNameEventIDMaskCmd_Payload_t *CmdPtr = &data->Payload;
    EVS_BinFilter_t *                              FilterPtr;
    int32                                          Status;
    EVS_AppData_t *                                AppDataPtr;
    char                                           LocalName[OS_MAX_API_NAME];

    /* Copy appname from command, ensures NULL termination */
    CFE_SB_MessageStringGet(LocalName, (char *)CmdPtr->AppName, NULL, sizeof(LocalName), sizeof(CmdPtr->AppName));

    /* Retrieve application data */
    Status = EVS_GetApplicationInfo(&AppDataPtr, LocalName);

    if (Status == CFE_SUCCESS)
    {
        /* Check to see if this event is already registered for filtering */
        FilterPtr = EVS_FindEventID(CmdPtr->EventID, AppDataPtr->BinFilters);

        /* FilterPtr != NULL means that this Event ID was found as already being registered */
        if (FilterPtr != NULL)
        {
            EVS_SendEvent(CFE_EVS_EVT_FILTERED_EID, CFE_EVS_EventType_ERROR,
                          "Add Filter Command:AppName = %s, EventID = 0x%08x is already registered for filtering",
                          LocalName, (unsigned int)CmdPtr->EventID);

            Status = CFE_EVS_EVT_NOT_REGISTERED;
        }
        else
        {
            /* now check to see if there is a free slot */
            FilterPtr = EVS_FindEventID(CFE_EVS_FREE_SLOT, AppDataPtr->BinFilters);

            if (FilterPtr != NULL)
            {
                /* Add Filter Contents */
                FilterPtr->EventID = CmdPtr->EventID;
                FilterPtr->Mask    = CmdPtr->Mask;
                FilterPtr->Count   = 0;

                EVS_SendEvent(CFE_EVS_ADDFILTER_EID, CFE_EVS_EventType_DEBUG,
                              "Add Filter Command Received with AppName = %s, EventID = 0x%08x, Mask = 0x%04x",
                              LocalName, (unsigned int)CmdPtr->EventID, (unsigned int)CmdPtr->Mask);
            }
            else
            {
                EVS_SendEvent(CFE_EVS_ERR_MAXREGSFILTER_EID, CFE_EVS_EventType_ERROR,
                              "Add Filter Command: number of registered filters has reached max = %d",
                              CFE_PLATFORM_EVS_MAX_EVENT_FILTERS);

                Status = CFE_EVS_APP_FILTER_OVERLOAD;
            }
        } /* end else*/
    }     /* end if (Status == CFE_SUCCESS) */

    else if (Status == CFE_EVS_APP_NOT_REGISTERED)
    {
        EVS_SendEvent(CFE_EVS_ERR_APPNOREGS_EID, CFE_EVS_EventType_ERROR, "%s not registered with EVS: CC = %lu",
                      LocalName, (long unsigned int)CFE_EVS_ADD_EVENT_FILTER_CC);
    }
    else if (Status == CFE_EVS_APP_ILLEGAL_APP_ID)
    {
        EVS_SendEvent(CFE_EVS_ERR_ILLAPPIDRANGE_EID, CFE_EVS_EventType_ERROR,
                      "Illegal application ID retrieved for %s: CC = %lu", LocalName,
                      (long unsigned int)CFE_EVS_ADD_EVENT_FILTER_CC);
    }
    else
    {
        EVS_SendEvent(CFE_EVS_ERR_NOAPPIDFOUND_EID, CFE_EVS_EventType_ERROR,
                      "Unable to retrieve application ID for %s: CC = %lu", LocalName,
                      (long unsigned int)CFE_EVS_ADD_EVENT_FILTER_CC);
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_EVS_DeleteEventFilterCmd(const CFE_EVS_DeleteEventFilterCmd_t *data)
{
    const CFE_EVS_AppNameEventIDCmd_Payload_t *CmdPtr = &data->Payload;
    EVS_BinFilter_t *                          FilterPtr;
    int32                                      Status;
    EVS_AppData_t *                            AppDataPtr;
    char                                       LocalName[OS_MAX_API_NAME];

    /* Copy appname from command, ensures NULL termination */
    CFE_SB_MessageStringGet(LocalName, (char *)CmdPtr->AppName, NULL, sizeof(LocalName), sizeof(CmdPtr->AppName));

    /* Retrieve application data */
    Status = EVS_GetApplicationInfo(&AppDataPtr, LocalName);

    if (Status == CFE_SUCCESS)
    {
        FilterPtr = EVS_FindEventID(CmdPtr->EventID, AppDataPtr->BinFilters);

        if (FilterPtr != NULL)
        {
            /* Clear Filter Contents */
            FilterPtr->EventID = CFE_EVS_FREE_SLOT;
            FilterPtr->Mask    = CFE_EVS_NO_MASK;
            FilterPtr->Count   = 0;

            EVS_SendEvent(CFE_EVS_DELFILTER_EID, CFE_EVS_EventType_DEBUG,
                          "Delete Filter Command Received with AppName = %s, EventID = 0x%08x", LocalName,
                          (unsigned int)CmdPtr->EventID);
        }
        else
        {
            EVS_SendEvent(CFE_EVS_ERR_EVTIDNOREGS_EID, CFE_EVS_EventType_ERROR,
                          "%s Event ID %d not registered for filtering: CC = %lu", LocalName, (int)CmdPtr->EventID,
                          (long unsigned int)CFE_EVS_DELETE_EVENT_FILTER_CC);
            Status = CFE_EVS_EVT_NOT_REGISTERED;
        }
    }
    else if (Status == CFE_EVS_APP_NOT_REGISTERED)
    {
        EVS_SendEvent(CFE_EVS_ERR_APPNOREGS_EID, CFE_EVS_EventType_ERROR, "%s not registered with EVS: CC = %lu",
                      LocalName, (long unsigned int)CFE_EVS_DELETE_EVENT_FILTER_CC);
    }
    else if (Status == CFE_EVS_APP_ILLEGAL_APP_ID)
    {
        EVS_SendEvent(CFE_EVS_ERR_ILLAPPIDRANGE_EID, CFE_EVS_EventType_ERROR,
                      "Illegal application ID retrieved for %s: CC = %lu", LocalName,
                      (long unsigned int)CFE_EVS_DELETE_EVENT_FILTER_CC);
    }
    else
    {
        EVS_SendEvent(CFE_EVS_ERR_NOAPPIDFOUND_EID, CFE_EVS_EventType_ERROR,
                      "Unable to retrieve application ID for %s: CC = %lu", LocalName,
                      (long unsigned int)CFE_EVS_DELETE_EVENT_FILTER_CC);
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_EVS_WriteAppDataFileCmd(const CFE_EVS_WriteAppDataFileCmd_t *data)
{
    int32                               Result;
    osal_id_t                           FileHandle = OS_OBJECT_ID_UNDEFINED;
    int32                               OsStatus;
    int32                               BytesWritten;
    uint32                              EntryCount = 0;
    uint32                              i;
    static CFE_EVS_AppDataFile_t        AppDataFile;
    CFE_FS_Header_t                     FileHdr;
    EVS_AppData_t *                     AppDataPtr;
    const CFE_EVS_AppDataCmd_Payload_t *CmdPtr = &data->Payload;
    char                                LocalName[OS_MAX_PATH_LEN];

    /*
    ** Copy the filename into local buffer with default name/path/extension if not specified
    */
    Result = CFE_FS_ParseInputFileNameEx(LocalName, CmdPtr->AppDataFilename, sizeof(LocalName),
                                         sizeof(CmdPtr->AppDataFilename), CFE_PLATFORM_EVS_DEFAULT_APP_DATA_FILE,
                                         CFE_FS_GetDefaultMountPoint(CFE_FS_FileCategory_BINARY_DATA_DUMP),
                                         CFE_FS_GetDefaultExtension(CFE_FS_FileCategory_BINARY_DATA_DUMP));

    if (Result != CFE_SUCCESS)
    {
        EVS_SendEvent(CFE_EVS_ERR_CRDATFILE_EID, CFE_EVS_EventType_ERROR,
                      "Write App Data Command Error: CFE_FS_ParseInputFileNameEx() = 0x%08X", (unsigned int)Result);
    }
    else
    {
        /* Create Application Data File */
        OsStatus = OS_OpenCreate(&FileHandle, LocalName, OS_FILE_FLAG_CREATE | OS_FILE_FLAG_TRUNCATE, OS_WRITE_ONLY);

        if (OsStatus != OS_SUCCESS)
        {
            EVS_SendEvent(CFE_EVS_ERR_CRDATFILE_EID, CFE_EVS_EventType_ERROR,
                          "Write App Data Command Error: OS_OpenCreate = %ld, filename = %s", (long)OsStatus,
                          LocalName);
            Result = CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
        }
    }

    if (Result == OS_SUCCESS)
    {
        /* Result will be overridden if everything works */
        Result = CFE_EVS_FILE_WRITE_ERROR;

        /* Initialize cFE file header */
        CFE_FS_InitHeader(&FileHdr, "EVS Application Data File", CFE_FS_SubType_EVS_APPDATA);

        /* Write cFE file header to the App File */
        BytesWritten = CFE_FS_WriteHeader(FileHandle, &FileHdr);

        if (BytesWritten == sizeof(CFE_FS_Header_t))
        {
            AppDataPtr = CFE_EVS_Global.AppData;
            for (i = 0; i < CFE_PLATFORM_ES_MAX_APPLICATIONS; i++)
            {
                /* Only have data for apps that are registered */
                if (EVS_AppDataIsUsed(AppDataPtr))
                {
                    /* Clear application file data record */
                    memset(&AppDataFile, 0, sizeof(CFE_EVS_AppDataFile_t));

                    /* Copy application data to application file data record */
                    CFE_ES_GetAppName(AppDataFile.AppName, EVS_AppDataGetID(AppDataPtr), sizeof(AppDataFile.AppName));
                    AppDataFile.ActiveFlag           = AppDataPtr->ActiveFlag;
                    AppDataFile.EventCount           = AppDataPtr->EventCount;
                    AppDataFile.EventTypesActiveFlag = AppDataPtr->EventTypesActiveFlag;
                    AppDataFile.SquelchedCount       = AppDataPtr->SquelchedCount;

                    /* Copy application filter data to application file data record */
                    memcpy(AppDataFile.Filters, AppDataPtr->BinFilters,
                           CFE_PLATFORM_EVS_MAX_EVENT_FILTERS * sizeof(EVS_BinFilter_t));

                    /* Write application data record to file */
                    OsStatus = OS_write(FileHandle, &AppDataFile, sizeof(CFE_EVS_AppDataFile_t));

                    if (OsStatus == sizeof(CFE_EVS_AppDataFile_t))
                    {
                        EntryCount++;
                    }
                    else
                    {
                        EVS_SendEvent(CFE_EVS_ERR_WRDATFILE_EID, CFE_EVS_EventType_ERROR,
                                      "Write App Data Command Error: At entry = %d, OS_write = %ld, filename = %s",
                                      (int)EntryCount, (long)OsStatus, LocalName);
                        break;
                    }
                }
                ++AppDataPtr;
            }

            /* Process command handler success result */
            if (i == CFE_PLATFORM_ES_MAX_APPLICATIONS)
            {
                EVS_SendEvent(CFE_EVS_WRDAT_EID, CFE_EVS_EventType_DEBUG,
                              "Write App Data Command: %d application data entries written to %s", (int)EntryCount,
                              LocalName);
                Result = CFE_SUCCESS;
            }
        }

        OS_close(FileHandle);
    }

    return Result;
}
