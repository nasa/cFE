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
**  File: cfe_es_task.c
**
**  Purpose:
**  cFE Executive Services (ES) task
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**     cFE Flight Software Application Developers Guide
**
**  Notes:
**
*/

/*
** Includes
*/
#include "cfe_es_module_all.h"

#include "cfe_version.h"
#include "target_config.h"
#include "cfe_es_verify.h"

#include "cfe_config.h"

#include <string.h>

/*
** Defines
*/
#define CFE_ES_PERF_MASK_ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

#define CFE_ES_PERF_TRIGGERMASK_INT_SIZE \
    CFE_ES_PERF_MASK_ARRAY_SIZE(CFE_ES_Global.ResetDataPtr->Perf.MetaData.TriggerMask)
#define CFE_ES_PERF_TRIGGERMASK_EXT_SIZE \
    CFE_ES_PERF_MASK_ARRAY_SIZE(CFE_ES_Global.TaskData.HkPacket.Payload.PerfTriggerMask)
#define CFE_ES_PERF_FILTERMASK_INT_SIZE \
    CFE_ES_PERF_MASK_ARRAY_SIZE(CFE_ES_Global.ResetDataPtr->Perf.MetaData.FilterMask)
#define CFE_ES_PERF_FILTERMASK_EXT_SIZE \
    CFE_ES_PERF_MASK_ARRAY_SIZE(CFE_ES_Global.TaskData.HkPacket.Payload.PerfFilterMask)

/*
** This define should be put in the OS API headers -- Right now it matches what the OS API uses
*/
#define OS_MAX_PRIORITY 255

/*
** Executive Services (ES) task global data.
*/
CFE_ES_TaskData_t CFE_ES_TaskData;

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_ES_TaskMain(void)
{
    int32            Status;
    uint32           AppRunStatus = CFE_ES_RunStatus_APP_RUN;
    CFE_SB_Buffer_t *SBBufPtr;

    /*
    ** Performance Time Stamp Entry
    */
    CFE_ES_PerfLogEntry(CFE_MISSION_ES_MAIN_PERF_ID);

    /*
    ** Perform task specific initialization.
    */
    Status = CFE_ES_TaskInit();
    if (Status != CFE_SUCCESS)
    {
        /*
        ** Create a syslog entry
        */
        CFE_ES_WriteToSysLog("%s: Application Init Failed,RC=0x%08X\n", __func__, (unsigned int)Status);

        /*
        ** Allow Core App to Exit
        */
        AppRunStatus = CFE_ES_RunStatus_CORE_APP_INIT_ERROR;
    }

    /*
     * Wait for other apps to start.
     * It is important that the core apps are present before this starts receiving
     * messages from the command pipe, as some of those handlers might depend on
     * the other core apps.
     */
    CFE_ES_WaitForSystemState(CFE_ES_SystemState_CORE_READY, CFE_PLATFORM_CORE_MAX_STARTUP_MSEC);

    /*
    ** Main process loop
    */
    while (AppRunStatus == CFE_ES_RunStatus_APP_RUN)
    {
        /*
        ** Increment the main task execution counter
        **  This is normally done in the CFE_ES_RunLoop call, but
        **  currently CFE Child tasks and the cFE core tasks do not
        **  use the RunLoop call.
        */
        CFE_ES_IncrementTaskCounter();

        /*
        ** Performance Time Stamp Exit
        */
        CFE_ES_PerfLogExit(CFE_MISSION_ES_MAIN_PERF_ID);

        /*
        ** Wait for the next Software Bus message.
        */
        Status = CFE_SB_ReceiveBuffer(&SBBufPtr, CFE_ES_Global.TaskData.CmdPipe, CFE_SB_PEND_FOREVER);

        /*
        ** Performance Time Stamp Entry
        */
        CFE_ES_PerfLogEntry(CFE_MISSION_ES_MAIN_PERF_ID);

        if (Status == CFE_SUCCESS)
        {
            /*
            ** Process message.
            */
            CFE_ES_TaskPipe(SBBufPtr);

            /*
             * Wake up the background task, which includes the
             * scanning of the ES app table for entries that may need cleanup
             */
            CFE_ES_BackgroundWakeup();
        }
        else
        {
            /*
            ** SB Error: Write a SysLog Message
            */
            CFE_ES_WriteToSysLog("%s: Error reading cmd pipe,RC=0x%08X\n", __func__, (unsigned int)Status);

            /*
            ** Allow Core App to Exit
            */
            AppRunStatus = CFE_ES_RunStatus_CORE_APP_RUNTIME_ERROR;
        }

    } /* end while */

    /*
    ** Performance Time Stamp Exit
    */
    CFE_ES_PerfLogExit(CFE_MISSION_ES_MAIN_PERF_ID);

    /*
    ** Exit the application, CFE_ES_ExitApp will not return.
    */
    CFE_ES_ExitApp(AppRunStatus);
}

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 * Send a single CFE_ES_VERSION_INF_EID event for a component/module
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_GenerateSingleVersionEvent(const char *ModuleType, const char *ModuleName, CFE_ConfigId_t Id)
{
    int32 Status;

    /*
     * Advertise the mission version information
     * NOTE: CFE_Config_GetString() does not return NULL, so its OK to use inside an arg list
     */
    Status = CFE_EVS_SendEvent(CFE_ES_VERSION_INF_EID, CFE_EVS_EventType_INFORMATION, "Version Info: %s %s, version %s",
                               ModuleType, ModuleName, CFE_Config_GetString(Id));

    return Status;
}

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 * Callback for iterating all configuration keys
 *
 *-----------------------------------------------------------------*/
void CFE_ES_ModSrcVerCallback(void *Arg, CFE_ConfigId_t Id, const char *Name)
{
    static const char IDNAME_PREFIX[] = "MOD_SRCVER_";

    if (strncmp(Name, IDNAME_PREFIX, sizeof(IDNAME_PREFIX) - 1) == 0)
    {
        CFE_ES_GenerateSingleVersionEvent("Module", &Name[sizeof(IDNAME_PREFIX) - 1], Id);
    }
}

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 * Send CFE_ES_VERSION_INF_EID events for all components/modules
 *
 *-----------------------------------------------------------------*/
void CFE_ES_GenerateVersionEvents(void)
{
    int32 Status;

    /*
     * Advertise the mission version information
     */
    Status = CFE_ES_GenerateSingleVersionEvent("Mission", GLOBAL_CONFIGDATA.MissionName, CFE_CONFIGID_MISSION_SRCVER);
    if (Status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: Error sending mission version event:RC=0x%08X\n", __func__, (unsigned int)Status);
    }

    CFE_Config_IterateAll(NULL, CFE_ES_ModSrcVerCallback);
}

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 * Sends the CFE_ES_BUILD_INF_EID event with build information
 *
 *-----------------------------------------------------------------*/
void CFE_ES_GenerateBuildInfoEvents(void)
{
    int32       Status;
    const char *BuildDate;
    const char *BuildUser;
    const char *BuildHost;

    /* NOTE: The config APIs using "GetString" will not return NULL */
    BuildDate = CFE_Config_GetString(CFE_CONFIGID_CORE_BUILDINFO_DATE);
    BuildUser = CFE_Config_GetString(CFE_CONFIGID_CORE_BUILDINFO_USER);
    BuildHost = CFE_Config_GetString(CFE_CONFIGID_CORE_BUILDINFO_HOST);

    Status = CFE_EVS_SendEvent(CFE_ES_BUILD_INF_EID, CFE_EVS_EventType_INFORMATION, "Build %s by %s@%s, config %s",
                               BuildDate, BuildUser, BuildHost, GLOBAL_CONFIGDATA.Config);
    if (Status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: Error sending build info event:RC=0x%08X\n", __func__, (unsigned int)Status);
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_TaskInit(void)
{
    int32   Status;
    int32   PspStatus;
    uint32  SizeofCfeSegment;
    cpuaddr CfeSegmentAddr;
    uint8   VersionNumber[4];
    char    VersionString[CFE_CFG_MAX_VERSION_STR_LEN];

    /*
    ** Initialize task command execution counters
    */
    CFE_ES_Global.TaskData.CommandCounter      = 0;
    CFE_ES_Global.TaskData.CommandErrorCounter = 0;

    /*
    ** Initialize systemlog to default Power On or Processor Reset mode
    */
    if (CFE_ES_GetResetType(NULL) == CFE_PSP_RST_TYPE_POWERON)
    {
        CFE_ES_Global.ResetDataPtr->SystemLogMode = CFE_PLATFORM_ES_DEFAULT_POR_SYSLOG_MODE;
    }
    else
    {
        CFE_ES_Global.ResetDataPtr->SystemLogMode = CFE_PLATFORM_ES_DEFAULT_PR_SYSLOG_MODE;
    }

    /*
    ** Register event filter table.
    */
    Status = CFE_EVS_Register(NULL, 0, CFE_EVS_EventFilter_BINARY);
    if (Status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: Call to CFE_EVS_Register Failed, RC = 0x%08X\n", __func__, (unsigned int)Status);
        return Status;
    }

    /*
    ** Initialize housekeeping packet (clear user data area)
    */
    CFE_MSG_Init(CFE_MSG_PTR(CFE_ES_Global.TaskData.HkPacket.TelemetryHeader), CFE_SB_ValueToMsgId(CFE_ES_HK_TLM_MID),
                 sizeof(CFE_ES_Global.TaskData.HkPacket));

    /*
    ** Initialize single application telemetry packet
    */
    CFE_MSG_Init(CFE_MSG_PTR(CFE_ES_Global.TaskData.OneAppPacket.TelemetryHeader),
                 CFE_SB_ValueToMsgId(CFE_ES_APP_TLM_MID), sizeof(CFE_ES_Global.TaskData.OneAppPacket));

    /*
    ** Initialize memory pool statistics telemetry packet
    */
    CFE_MSG_Init(CFE_MSG_PTR(CFE_ES_Global.TaskData.MemStatsPacket.TelemetryHeader),
                 CFE_SB_ValueToMsgId(CFE_ES_MEMSTATS_TLM_MID), sizeof(CFE_ES_Global.TaskData.MemStatsPacket));

    /*
    ** Create Software Bus message pipe
    */
    Status = CFE_SB_CreatePipe(&CFE_ES_Global.TaskData.CmdPipe, CFE_ES_PIPE_DEPTH, CFE_ES_PIPE_NAME);
    if (Status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: Cannot Create SB Pipe, RC = 0x%08X\n", __func__, (unsigned int)Status);
        return Status;
    }

    /*
    ** Subscribe to Housekeeping request commands
    */
    Status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(CFE_ES_SEND_HK_MID), CFE_ES_Global.TaskData.CmdPipe);
    if (Status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: Cannot Subscribe to HK packet, RC = 0x%08X\n", __func__, (unsigned int)Status);
        return Status;
    }

    /*
    ** Subscribe to ES task ground command packets
    */
    Status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(CFE_ES_CMD_MID), CFE_ES_Global.TaskData.CmdPipe);
    if (Status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: Cannot Subscribe to ES ground commands, RC = 0x%08X\n", __func__,
                             (unsigned int)Status);
        return Status;
    }

    /*
    ** Compute the CRC for the cfe core code segment and place
    ** in ES Housekeeping pkt.
    */
    PspStatus = CFE_PSP_GetCFETextSegmentInfo(&CfeSegmentAddr, &SizeofCfeSegment);

    if (PspStatus == CFE_PSP_SUCCESS)
    {
        CFE_ES_Global.TaskData.HkPacket.Payload.CFECoreChecksum =
            CFE_ES_CalculateCRC((void *)(CfeSegmentAddr), SizeofCfeSegment, 0, CFE_MISSION_ES_DEFAULT_CRC);
    }
    else
    {
        CFE_ES_Global.TaskData.HkPacket.Payload.CFECoreChecksum = 0xFFFF;
    }

    /*
    ** Initialize the version numbers in the ES Housekeeping pkt
    */
    CFE_ES_Global.TaskData.HkPacket.Payload.CFEMajorVersion    = CFE_MAJOR_VERSION;
    CFE_ES_Global.TaskData.HkPacket.Payload.CFEMinorVersion    = CFE_MINOR_VERSION;
    CFE_ES_Global.TaskData.HkPacket.Payload.CFERevision        = CFE_REVISION;
    CFE_ES_Global.TaskData.HkPacket.Payload.CFEMissionRevision = CFE_MISSION_REV;

    OS_GetVersionNumber(VersionNumber);
    CFE_ES_Global.TaskData.HkPacket.Payload.OSALMajorVersion    = VersionNumber[0];
    CFE_ES_Global.TaskData.HkPacket.Payload.OSALMinorVersion    = VersionNumber[1];
    CFE_ES_Global.TaskData.HkPacket.Payload.OSALRevision        = VersionNumber[2];
    CFE_ES_Global.TaskData.HkPacket.Payload.OSALMissionRevision = VersionNumber[3];

    CFE_PSP_GetVersionNumber(VersionNumber);
    CFE_ES_Global.TaskData.HkPacket.Payload.PSPMajorVersion    = VersionNumber[0];
    CFE_ES_Global.TaskData.HkPacket.Payload.PSPMinorVersion    = VersionNumber[1];
    CFE_ES_Global.TaskData.HkPacket.Payload.PSPRevision        = VersionNumber[2];
    CFE_ES_Global.TaskData.HkPacket.Payload.PSPMissionRevision = VersionNumber[3];

    /*
    ** Task startup event message.
    */
    CFE_Config_GetVersionString(VersionString, CFE_CFG_MAX_VERSION_STR_LEN, "cFE", CFE_SRC_VERSION, CFE_BUILD_CODENAME,
                                CFE_LAST_OFFICIAL);
    Status =
        CFE_EVS_SendEvent(CFE_ES_INIT_INF_EID, CFE_EVS_EventType_INFORMATION, "cFE ES Initialized: %s", VersionString);
    if (Status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: Error sending init event:RC=0x%08X\n", __func__, (unsigned int)Status);
        return Status;
    }

    Status =
        CFE_EVS_SendEvent(CFE_ES_INITSTATS_INF_EID, CFE_EVS_EventType_INFORMATION,
                          "cFS Versions: cfe %s, osal %s, psp %s. cFE chksm %d", CFE_SRC_VERSION, OS_GetVersionString(),
                          CFE_PSP_GetVersionString(), (int)CFE_ES_Global.TaskData.HkPacket.Payload.CFECoreChecksum);

    if (Status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: Error sending init stats event:RC=0x%08X\n", __func__, (unsigned int)Status);
        return Status;
    }

    /*
     * Generate all module version and build info events.
     */
    CFE_ES_GenerateVersionEvents();
    CFE_ES_GenerateBuildInfoEvents();

    /*
     * Initialize the "background task" which is a low priority child task
     * devoted to maintenance duties that do not need to execute on a
     * strict/precise schedule.
     */
    Status = CFE_ES_BackgroundInit();
    if (Status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: Error initializing background task:RC=0x%08X\n", __func__, (unsigned int)Status);
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
int32 CFE_ES_SendHkCmd(const CFE_ES_SendHkCmd_t *data)
{
    OS_heap_prop_t HeapProp;
    int32          OsStatus;
    uint32         PerfIdx;

    memset(&HeapProp, 0, sizeof(HeapProp));

    /*
    ** Get command execution counters, system log entry count & bytes used.
    */
    CFE_ES_Global.TaskData.HkPacket.Payload.CommandCounter      = CFE_ES_Global.TaskData.CommandCounter;
    CFE_ES_Global.TaskData.HkPacket.Payload.CommandErrorCounter = CFE_ES_Global.TaskData.CommandErrorCounter;

    CFE_ES_Global.TaskData.HkPacket.Payload.SysLogBytesUsed =
        CFE_ES_MEMOFFSET_C(CFE_ES_Global.ResetDataPtr->SystemLogEndIdx);
    CFE_ES_Global.TaskData.HkPacket.Payload.SysLogSize    = CFE_ES_MEMOFFSET_C(CFE_PLATFORM_ES_SYSTEM_LOG_SIZE);
    CFE_ES_Global.TaskData.HkPacket.Payload.SysLogEntries = CFE_ES_Global.ResetDataPtr->SystemLogEntryNum;
    CFE_ES_Global.TaskData.HkPacket.Payload.SysLogMode    = CFE_ES_Global.ResetDataPtr->SystemLogMode;

    CFE_ES_Global.TaskData.HkPacket.Payload.ERLogIndex   = CFE_ES_Global.ResetDataPtr->ERLogIndex;
    CFE_ES_Global.TaskData.HkPacket.Payload.ERLogEntries = CFE_ES_Global.ResetDataPtr->ERLogEntries;

    CFE_ES_Global.TaskData.HkPacket.Payload.RegisteredCoreApps     = CFE_ES_Global.RegisteredCoreApps;
    CFE_ES_Global.TaskData.HkPacket.Payload.RegisteredExternalApps = CFE_ES_Global.RegisteredExternalApps;
    CFE_ES_Global.TaskData.HkPacket.Payload.RegisteredTasks        = CFE_ES_Global.RegisteredTasks;
    CFE_ES_Global.TaskData.HkPacket.Payload.RegisteredLibs         = CFE_ES_Global.RegisteredLibs;

    CFE_ES_Global.TaskData.HkPacket.Payload.ResetType       = CFE_ES_Global.ResetDataPtr->ResetVars.ResetType;
    CFE_ES_Global.TaskData.HkPacket.Payload.ResetSubtype    = CFE_ES_Global.ResetDataPtr->ResetVars.ResetSubtype;
    CFE_ES_Global.TaskData.HkPacket.Payload.ProcessorResets = CFE_ES_Global.ResetDataPtr->ResetVars.ProcessorResetCount;
    CFE_ES_Global.TaskData.HkPacket.Payload.MaxProcessorResets =
        CFE_ES_Global.ResetDataPtr->ResetVars.MaxProcessorResetCount;
    CFE_ES_Global.TaskData.HkPacket.Payload.BootSource = CFE_ES_Global.ResetDataPtr->ResetVars.BootSource;

    CFE_ES_Global.TaskData.HkPacket.Payload.PerfState        = CFE_ES_Global.ResetDataPtr->Perf.MetaData.State;
    CFE_ES_Global.TaskData.HkPacket.Payload.PerfMode         = CFE_ES_Global.ResetDataPtr->Perf.MetaData.Mode;
    CFE_ES_Global.TaskData.HkPacket.Payload.PerfTriggerCount = CFE_ES_Global.ResetDataPtr->Perf.MetaData.TriggerCount;
    CFE_ES_Global.TaskData.HkPacket.Payload.PerfDataStart    = CFE_ES_Global.ResetDataPtr->Perf.MetaData.DataStart;
    CFE_ES_Global.TaskData.HkPacket.Payload.PerfDataEnd      = CFE_ES_Global.ResetDataPtr->Perf.MetaData.DataEnd;
    CFE_ES_Global.TaskData.HkPacket.Payload.PerfDataCount    = CFE_ES_Global.ResetDataPtr->Perf.MetaData.DataCount;
    CFE_ES_Global.TaskData.HkPacket.Payload.PerfDataToWrite  = CFE_ES_GetPerfLogDumpRemaining();

    /*
     * Fill out the perf trigger/filter mask objects
     * The entire array in the HK payload object (external size) must be filled,
     * to avoid sending garbage data.
     *
     * If it is larger than what the platform supports (internal size), it will
     * be padded with 0's
     *
     * If it is smaller than what the platform supports, then truncate.
     */
    for (PerfIdx = 0; PerfIdx < CFE_ES_PERF_TRIGGERMASK_EXT_SIZE; ++PerfIdx)
    {
        if (PerfIdx < CFE_ES_PERF_TRIGGERMASK_INT_SIZE)
        {
            CFE_ES_Global.TaskData.HkPacket.Payload.PerfTriggerMask[PerfIdx] =
                CFE_ES_Global.ResetDataPtr->Perf.MetaData.TriggerMask[PerfIdx];
        }
        else
        {
            CFE_ES_Global.TaskData.HkPacket.Payload.PerfTriggerMask[PerfIdx] = 0;
        }
    }

    for (PerfIdx = 0; PerfIdx < CFE_ES_PERF_FILTERMASK_EXT_SIZE; ++PerfIdx)
    {
        if (PerfIdx < CFE_ES_PERF_FILTERMASK_INT_SIZE)
        {
            CFE_ES_Global.TaskData.HkPacket.Payload.PerfFilterMask[PerfIdx] =
                CFE_ES_Global.ResetDataPtr->Perf.MetaData.FilterMask[PerfIdx];
        }
        else
        {
            CFE_ES_Global.TaskData.HkPacket.Payload.PerfFilterMask[PerfIdx] = 0;
        }
    }

    /* Fill in heap info if get successful/supported */
    OsStatus = OS_HeapGetInfo(&HeapProp);
    if (OsStatus == OS_SUCCESS)
    {
        CFE_ES_Global.TaskData.HkPacket.Payload.HeapBytesFree    = CFE_ES_MEMOFFSET_C(HeapProp.free_bytes);
        CFE_ES_Global.TaskData.HkPacket.Payload.HeapBlocksFree   = CFE_ES_MEMOFFSET_C(HeapProp.free_blocks);
        CFE_ES_Global.TaskData.HkPacket.Payload.HeapMaxBlockSize = CFE_ES_MEMOFFSET_C(HeapProp.largest_free_block);
    }
    else
    {
        CFE_ES_Global.TaskData.HkPacket.Payload.HeapBytesFree    = CFE_ES_MEMOFFSET_C(0);
        CFE_ES_Global.TaskData.HkPacket.Payload.HeapBlocksFree   = CFE_ES_MEMOFFSET_C(0);
        CFE_ES_Global.TaskData.HkPacket.Payload.HeapMaxBlockSize = CFE_ES_MEMOFFSET_C(0);
    }

    /*
    ** Send housekeeping telemetry packet.
    */
    CFE_SB_TimeStampMsg(CFE_MSG_PTR(CFE_ES_Global.TaskData.HkPacket.TelemetryHeader));
    CFE_SB_TransmitMsg(CFE_MSG_PTR(CFE_ES_Global.TaskData.HkPacket.TelemetryHeader), true);

    /*
    ** This command does not affect the command execution counter.
    */

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_NoopCmd(const CFE_ES_NoopCmd_t *Cmd)
{
    /*
    ** Advertise the build and version information with the no-op command
    ** For unit testing purposes, it helps to put this first - the UT
    ** is checking for the last event sent to be NOOP_INF_EID.
    */
    CFE_ES_GenerateBuildInfoEvents();

    /*
    ** This command will always succeed.
    */
    CFE_ES_Global.TaskData.CommandCounter++;

    CFE_EVS_SendEvent(CFE_ES_NOOP_INF_EID, CFE_EVS_EventType_INFORMATION,
                      "No-op command:\n cFS Versions: cfe %s, osal %s, psp %s", CFE_SRC_VERSION, OS_GetVersionString(),
                      CFE_PSP_GetVersionString());

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_ResetCountersCmd(const CFE_ES_ResetCountersCmd_t *data)
{
    CFE_ES_Global.TaskData.CommandCounter      = 0;
    CFE_ES_Global.TaskData.CommandErrorCounter = 0;

    /*
    ** This command will always succeed.
    */
    CFE_EVS_SendEvent(CFE_ES_RESET_INF_EID, CFE_EVS_EventType_INFORMATION, "Reset Counters command");

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_RestartCmd(const CFE_ES_RestartCmd_t *data)
{
    const CFE_ES_RestartCmd_Payload_t *cmd = &data->Payload;

    if ((cmd->RestartType != CFE_PSP_RST_TYPE_PROCESSOR) && (cmd->RestartType != CFE_PSP_RST_TYPE_POWERON))
    {
        CFE_ES_Global.TaskData.CommandErrorCounter++;
        CFE_EVS_SendEvent(CFE_ES_BOOT_ERR_EID, CFE_EVS_EventType_ERROR, "Invalid cFE restart type: %d",
                          (int)cmd->RestartType);
    }
    else
    {
        /*
        ** This function will not return.
        */
        CFE_ES_ResetCFE(cmd->RestartType);
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_StartAppCmd(const CFE_ES_StartAppCmd_t *data)
{
    const CFE_ES_StartAppCmd_Payload_t *cmd = &data->Payload;
    CFE_ES_AppId_t                      AppID;
    int32                               Result;
    int32                               AppEntryLen;
    int32                               AppNameLen;
    char                                LocalAppName[OS_MAX_API_NAME];
    CFE_ES_AppStartParams_t             StartParams;

    memset(&StartParams, 0, sizeof(StartParams));

    /* Create local copies of all input strings and ensure null termination */
    Result = CFE_FS_ParseInputFileNameEx(StartParams.BasicInfo.FileName, cmd->AppFileName,
                                         sizeof(StartParams.BasicInfo.FileName), sizeof(cmd->AppFileName), NULL,
                                         CFE_FS_GetDefaultMountPoint(CFE_FS_FileCategory_DYNAMIC_MODULE),
                                         CFE_FS_GetDefaultExtension(CFE_FS_FileCategory_DYNAMIC_MODULE));

    AppEntryLen = CFE_SB_MessageStringGet(StartParams.BasicInfo.InitSymbolName, cmd->AppEntryPoint, NULL,
                                          sizeof(StartParams.BasicInfo.InitSymbolName), sizeof(cmd->AppEntryPoint));

    AppNameLen =
        CFE_SB_MessageStringGet(LocalAppName, cmd->Application, NULL, sizeof(LocalAppName), sizeof(cmd->Application));

    /*
    ** Verify command parameters
    */
    if (Result != CFE_SUCCESS)
    {
        CFE_ES_Global.TaskData.CommandErrorCounter++;
        CFE_EVS_SendEvent(CFE_ES_START_INVALID_FILENAME_ERR_EID, CFE_EVS_EventType_ERROR,
                          "CFE_ES_StartAppCmd: invalid filename, status=%lx", (unsigned long)Result);
    }
    else if (AppEntryLen <= 0)
    {
        CFE_ES_Global.TaskData.CommandErrorCounter++;
        CFE_EVS_SendEvent(CFE_ES_START_INVALID_ENTRY_POINT_ERR_EID, CFE_EVS_EventType_ERROR,
                          "CFE_ES_StartAppCmd: App Entry Point is empty.");
    }
    else if (AppNameLen <= 0)
    {
        CFE_ES_Global.TaskData.CommandErrorCounter++;
        CFE_EVS_SendEvent(CFE_ES_START_NULL_APP_NAME_ERR_EID, CFE_EVS_EventType_ERROR,
                          "CFE_ES_StartAppCmd: App Name is empty.");
    }
    else if (cmd->Priority > OS_MAX_PRIORITY)
    {
        CFE_ES_Global.TaskData.CommandErrorCounter++;
        CFE_EVS_SendEvent(CFE_ES_START_PRIORITY_ERR_EID, CFE_EVS_EventType_ERROR,
                          "CFE_ES_StartAppCmd: Priority is too large: %d.", (int)cmd->Priority);
    }
    else if ((cmd->ExceptionAction != CFE_ES_ExceptionAction_RESTART_APP) &&
             (cmd->ExceptionAction != CFE_ES_ExceptionAction_PROC_RESTART))
    {
        CFE_ES_Global.TaskData.CommandErrorCounter++;
        CFE_EVS_SendEvent(CFE_ES_START_EXC_ACTION_ERR_EID, CFE_EVS_EventType_ERROR,
                          "CFE_ES_StartAppCmd: Invalid Exception Action: %d.", (int)cmd->ExceptionAction);
    }
    else
    {
        /* If stack size was provided, use it, otherwise use default. */
        StartParams.MainTaskInfo.StackSize = CFE_ES_MEMOFFSET_TO_SIZET(cmd->StackSize);
        if (StartParams.MainTaskInfo.StackSize == 0)
        {
            StartParams.MainTaskInfo.StackSize = CFE_PLATFORM_ES_DEFAULT_STACK_SIZE;
        }

        StartParams.MainTaskInfo.Priority = cmd->Priority;
        StartParams.ExceptionAction       = cmd->ExceptionAction;

        /*
        ** Invoke application loader/startup function.
        */
        Result = CFE_ES_AppCreate(&AppID, LocalAppName, &StartParams);

        /*
        ** Send appropriate event message
        */
        if (Result == CFE_SUCCESS)
        {
            CFE_ES_Global.TaskData.CommandCounter++;
            CFE_EVS_SendEvent(CFE_ES_START_INF_EID, CFE_EVS_EventType_INFORMATION, "Started %s from %s, AppID = %lu",
                              LocalAppName, StartParams.BasicInfo.FileName, CFE_RESOURCEID_TO_ULONG(AppID));
        }
        else
        {
            CFE_ES_Global.TaskData.CommandErrorCounter++;
            CFE_EVS_SendEvent(CFE_ES_START_ERR_EID, CFE_EVS_EventType_ERROR, "Failed to start %s from %s, RC = 0x%08X",
                              LocalAppName, StartParams.BasicInfo.FileName, (unsigned int)Result);
        }

    } /* End if -- command parameter validation */

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_StopAppCmd(const CFE_ES_StopAppCmd_t *data)
{
    const CFE_ES_AppNameCmd_Payload_t *cmd = &data->Payload;
    char                               LocalApp[OS_MAX_API_NAME];
    CFE_ES_AppId_t                     AppID;
    int32                              Result;

    CFE_SB_MessageStringGet(LocalApp, (char *)cmd->Application, NULL, sizeof(LocalApp), sizeof(cmd->Application));

    Result = CFE_ES_GetAppIDByName(&AppID, LocalApp);

    if (Result == CFE_SUCCESS)
    {
        /*
        ** Delete the App
        */
        Result = CFE_ES_DeleteApp(AppID);

        /*
        ** Send appropriate event message.
        */
        if (Result == CFE_SUCCESS)
        {
            CFE_ES_Global.TaskData.CommandCounter++;
            CFE_EVS_SendEvent(CFE_ES_STOP_DBG_EID, CFE_EVS_EventType_DEBUG, "Stop Application %s Initiated.", LocalApp);
        }
        else
        {
            CFE_ES_Global.TaskData.CommandErrorCounter++;
            CFE_EVS_SendEvent(CFE_ES_STOP_ERR1_EID, CFE_EVS_EventType_ERROR, "Stop Application %s Failed, RC = 0x%08X",
                              LocalApp, (unsigned int)Result);
        }
    }
    else
    {
        CFE_ES_Global.TaskData.CommandErrorCounter++;
        CFE_EVS_SendEvent(CFE_ES_STOP_ERR2_EID, CFE_EVS_EventType_ERROR,
                          "Stop Application %s, GetAppIDByName failed. RC = 0x%08X.", LocalApp, (unsigned int)Result);
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_RestartAppCmd(const CFE_ES_RestartAppCmd_t *data)
{
    const CFE_ES_AppNameCmd_Payload_t *cmd = &data->Payload;
    char                               LocalApp[OS_MAX_API_NAME];
    CFE_ES_AppId_t                     AppID;
    int32                              Result;

    CFE_SB_MessageStringGet(LocalApp, (char *)cmd->Application, NULL, sizeof(LocalApp), sizeof(cmd->Application));

    Result = CFE_ES_GetAppIDByName(&AppID, LocalApp);

    if (Result == CFE_SUCCESS)
    {
        Result = CFE_ES_RestartApp(AppID);

        /*
        ** Send appropriate event message.
        */
        if (Result == CFE_SUCCESS)
        {
            CFE_ES_Global.TaskData.CommandCounter++;
            CFE_EVS_SendEvent(CFE_ES_RESTART_APP_DBG_EID, CFE_EVS_EventType_DEBUG, "Restart Application %s Initiated.",
                              LocalApp);
        }
        else
        {
            CFE_ES_Global.TaskData.CommandErrorCounter++;
            CFE_EVS_SendEvent(CFE_ES_RESTART_APP_ERR1_EID, CFE_EVS_EventType_ERROR,
                              "Restart Application %s Failed, RC = 0x%08X", LocalApp, (unsigned int)Result);
        }
    }
    else
    {
        CFE_ES_Global.TaskData.CommandErrorCounter++;
        CFE_EVS_SendEvent(CFE_ES_RESTART_APP_ERR2_EID, CFE_EVS_EventType_ERROR,
                          "Restart Application %s, GetAppIDByName failed. RC = 0x%08X.", LocalApp,
                          (unsigned int)Result);
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_ReloadAppCmd(const CFE_ES_ReloadAppCmd_t *data)
{
    const CFE_ES_AppReloadCmd_Payload_t *cmd = &data->Payload;
    char                                 LocalApp[OS_MAX_API_NAME];
    char                                 LocalFileName[OS_MAX_PATH_LEN];
    CFE_ES_AppId_t                       AppID;
    int32                                Result;

    CFE_SB_MessageStringGet(LocalApp, (char *)cmd->Application, NULL, sizeof(LocalApp), sizeof(cmd->Application));

    Result = CFE_ES_GetAppIDByName(&AppID, LocalApp);

    if (Result == CFE_SUCCESS)
    {
        /* Read input string as a file name for dynamic module */
        Result = CFE_FS_ParseInputFileNameEx(LocalFileName, cmd->AppFileName, sizeof(LocalFileName),
                                             sizeof(cmd->AppFileName), NULL,
                                             CFE_FS_GetDefaultMountPoint(CFE_FS_FileCategory_DYNAMIC_MODULE),
                                             CFE_FS_GetDefaultExtension(CFE_FS_FileCategory_DYNAMIC_MODULE));

        if (Result == CFE_SUCCESS)
        {
            Result = CFE_ES_ReloadApp(AppID, LocalFileName);
        }

        /*
        ** Send appropriate event message.
        */
        if (Result == CFE_SUCCESS)
        {
            CFE_ES_Global.TaskData.CommandCounter++;
            CFE_EVS_SendEvent(CFE_ES_RELOAD_APP_DBG_EID, CFE_EVS_EventType_DEBUG, "Reload Application %s Initiated.",
                              LocalApp);
        }
        else
        {
            CFE_ES_Global.TaskData.CommandErrorCounter++;
            CFE_EVS_SendEvent(CFE_ES_RELOAD_APP_ERR1_EID, CFE_EVS_EventType_ERROR,
                              "Reload Application %s Failed, RC = 0x%08X", LocalApp, (unsigned int)Result);
        }
    }
    else
    {
        CFE_ES_Global.TaskData.CommandErrorCounter++;
        CFE_EVS_SendEvent(CFE_ES_RELOAD_APP_ERR2_EID, CFE_EVS_EventType_ERROR,
                          "Reload Application %s, GetAppIDByName failed. RC = 0x%08X.", LocalApp, (unsigned int)Result);
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_QueryOneCmd(const CFE_ES_QueryOneCmd_t *data)
{
    const CFE_ES_AppNameCmd_Payload_t *cmd = &data->Payload;
    char                               LocalApp[OS_MAX_API_NAME];
    union
    {
        CFE_ES_AppId_t   AppId;
        CFE_ES_LibId_t   LibId;
        CFE_ResourceId_t ResourceID;
    } IdBuf;
    int32 Result;

    CFE_SB_MessageStringGet(LocalApp, (char *)cmd->Application, NULL, sizeof(LocalApp), sizeof(cmd->Application));

    Result = CFE_ES_GetAppIDByName(&IdBuf.AppId, LocalApp);
    if (Result == CFE_ES_ERR_NAME_NOT_FOUND)
    {
        /* Also check for a matching library name */
        Result = CFE_ES_GetLibIDByName(&IdBuf.LibId, LocalApp);
    }

    if (Result == CFE_SUCCESS)
    {
        Result = CFE_ES_GetModuleInfo(&(CFE_ES_Global.TaskData.OneAppPacket.Payload.AppInfo), IdBuf.ResourceID);
    }

    /*
    ** Send appropriate event message...
    */
    if (Result == CFE_SUCCESS)
    {
        /*
        ** Send application status telemetry packet.
        */
        CFE_SB_TimeStampMsg(CFE_MSG_PTR(CFE_ES_Global.TaskData.OneAppPacket.TelemetryHeader));
        Result = CFE_SB_TransmitMsg(CFE_MSG_PTR(CFE_ES_Global.TaskData.OneAppPacket.TelemetryHeader), true);
        if (Result == CFE_SUCCESS)
        {
            CFE_ES_Global.TaskData.CommandCounter++;
            CFE_EVS_SendEvent(CFE_ES_ONE_APP_EID, CFE_EVS_EventType_DEBUG, "Sent %s application data", LocalApp);
        }
        else
        {
            CFE_ES_Global.TaskData.CommandErrorCounter++;
            CFE_EVS_SendEvent(CFE_ES_ONE_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Failed to send %s application data, RC = 0x%08X", LocalApp, (unsigned int)Result);
        }
    }
    else
    {
        CFE_ES_Global.TaskData.CommandErrorCounter++;
        CFE_EVS_SendEvent(CFE_ES_ONE_APPID_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Failed to send %s application data: GetAppIDByName Failed, RC = 0x%08X", LocalApp,
                          (unsigned int)Result);
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_QueryAllCmd(const CFE_ES_QueryAllCmd_t *data)
{
    CFE_FS_Header_t                     FileHeader;
    osal_id_t                           FileDescriptor = OS_OBJECT_ID_UNDEFINED;
    uint32                              i;
    uint32                              EntryCount = 0;
    size_t                              FileSize   = 0;
    int32                               OsStatus;
    int32                               Result;
    CFE_ES_AppInfo_t                    AppInfo;
    const CFE_ES_FileNameCmd_Payload_t *CmdPtr = &data->Payload;
    char                                QueryAllFilename[OS_MAX_PATH_LEN];
    CFE_ResourceId_t                    ResourceList[CFE_ES_QUERY_ALL_MAX_ENTRIES];
    uint32                              NumResources;
    CFE_ES_AppRecord_t *                AppRecPtr;
    CFE_ES_LibRecord_t *                LibRecPtr;

    /*
     * Collect list of active resource IDs.
     *
     * This should be done while locked, but the actual writing
     * of the AppInfo data should be done while NOT locked.
     */
    CFE_ES_LockSharedData(__func__, __LINE__);
    NumResources = 0;
    AppRecPtr    = CFE_ES_Global.AppTable;
    for (i = 0; i < CFE_PLATFORM_ES_MAX_APPLICATIONS && NumResources < CFE_ES_QUERY_ALL_MAX_ENTRIES; ++i)
    {
        if (CFE_ES_AppRecordIsUsed(AppRecPtr))
        {
            ResourceList[NumResources] = CFE_RESOURCEID_UNWRAP(CFE_ES_AppRecordGetID(AppRecPtr));
            ++NumResources;
        }
        ++AppRecPtr;
    }
    LibRecPtr = CFE_ES_Global.LibTable;
    for (i = 0; i < CFE_PLATFORM_ES_MAX_LIBRARIES && NumResources < CFE_ES_QUERY_ALL_MAX_ENTRIES; ++i)
    {
        if (CFE_ES_LibRecordIsUsed(LibRecPtr))
        {
            ResourceList[NumResources] = CFE_RESOURCEID_UNWRAP(CFE_ES_LibRecordGetID(LibRecPtr));
            ++NumResources;
        }
        ++LibRecPtr;
    }
    CFE_ES_UnlockSharedData(__func__, __LINE__);

    /* Copy the commanded filename, using default if unspecified */
    Result = CFE_FS_ParseInputFileNameEx(QueryAllFilename, CmdPtr->FileName, sizeof(QueryAllFilename),
                                         sizeof(CmdPtr->FileName), CFE_PLATFORM_ES_DEFAULT_APP_LOG_FILE,
                                         CFE_FS_GetDefaultMountPoint(CFE_FS_FileCategory_BINARY_DATA_DUMP),
                                         CFE_FS_GetDefaultExtension(CFE_FS_FileCategory_BINARY_DATA_DUMP));

    if (Result == CFE_SUCCESS)
    {
        /*
        ** Create (or truncate) ES task log data file
        */
        OsStatus = OS_OpenCreate(&FileDescriptor, QueryAllFilename, OS_FILE_FLAG_CREATE | OS_FILE_FLAG_TRUNCATE,
                                 OS_WRITE_ONLY);

        if (OsStatus != OS_SUCCESS)
        {
            CFE_EVS_SendEvent(CFE_ES_OSCREATE_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Failed to write App Info file, OS_OpenCreate RC = %ld", (long)OsStatus);
            Result = CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
        }
    }
    else
    {
        CFE_EVS_SendEvent(CFE_ES_OSCREATE_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Failed to write App Info file, CFE_FS_ParseInputFileNameEx RC = %08x", (unsigned int)Result);
    }

    if (Result >= 0)
    {
        /*
        ** Initialize cFE file header
        */
        CFE_FS_InitHeader(&FileHeader, CFE_ES_APP_LOG_DESC, CFE_FS_SubType_ES_QUERYALL);

        /*
        ** Output the Standard cFE File Header to the App File
        */
        Result = CFE_FS_WriteHeader(FileDescriptor, &FileHeader);

        if (Result != sizeof(CFE_FS_Header_t))
        {
            OS_close(FileDescriptor);
            CFE_ES_Global.TaskData.CommandErrorCounter++;
            CFE_EVS_SendEvent(CFE_ES_WRHDR_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Failed to write App Info file, WriteHdr RC = 0x%08X, exp %d", (unsigned int)Result,
                              (int)sizeof(CFE_FS_Header_t));
            /*
             * returning "success" here as there is no other recourse;
             * the full extent of the error recovery has been done
             */
            return CFE_SUCCESS;
        }

        /*
        ** Maintain statistics of amount of data written to file
        */
        FileSize += sizeof(CFE_FS_Header_t);

        /*
        ** Loop through the ES AppTable for main applications
        */
        for (i = 0; i < NumResources; ++i)
        {
            /*
             ** Populate the AppInfo entry
             */
            Result = CFE_ES_GetModuleInfo(&AppInfo, ResourceList[i]);
            if (Result == CFE_SUCCESS)
            {
                /*
                ** Write the local entry to file
                */
                OsStatus = OS_write(FileDescriptor, &AppInfo, sizeof(CFE_ES_AppInfo_t));
                if (OsStatus != sizeof(CFE_ES_AppInfo_t))
                {
                    OS_close(FileDescriptor);
                    CFE_ES_Global.TaskData.CommandErrorCounter++;
                    CFE_EVS_SendEvent(CFE_ES_TASKWR_ERR_EID, CFE_EVS_EventType_ERROR,
                                      "Failed to write App Info file, Task write RC = %ld, exp %d", (long)OsStatus,
                                      (int)sizeof(CFE_ES_AppInfo_t));
                    /*
                     * returning "success" here as there is no other recourse;
                     * the full extent of the error recovery has been done
                     */
                    return CFE_SUCCESS;
                }

                FileSize += sizeof(CFE_ES_AppInfo_t);
                EntryCount++;
            }

        } /* end for */

        OS_close(FileDescriptor);
        CFE_ES_Global.TaskData.CommandCounter++;
        CFE_EVS_SendEvent(CFE_ES_ALL_APPS_EID, CFE_EVS_EventType_DEBUG,
                          "App Info file written to %s, Entries=%d, FileSize=%lu", QueryAllFilename, (int)EntryCount,
                          (unsigned long)FileSize);
    }
    else
    {
        CFE_ES_Global.TaskData.CommandErrorCounter++;
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_QueryAllTasksCmd(const CFE_ES_QueryAllTasksCmd_t *data)
{
    CFE_FS_Header_t                     FileHeader;
    osal_id_t                           FileDescriptor = OS_OBJECT_ID_UNDEFINED;
    uint32                              i;
    uint32                              EntryCount = 0;
    size_t                              FileSize   = 0;
    int32                               OsStatus;
    int32                               Result;
    CFE_ES_TaskInfo_t                   TaskInfo;
    const CFE_ES_FileNameCmd_Payload_t *CmdPtr = &data->Payload;
    char                                QueryAllFilename[OS_MAX_PATH_LEN];
    CFE_ES_TaskId_t                     TaskList[OS_MAX_TASKS];
    uint32                              NumTasks;
    CFE_ES_TaskRecord_t *               TaskRecPtr;

    /*
     * Collect list of active task IDs.
     *
     * This should be done while locked, but the actual writing
     * of the AppInfo data should be done while NOT locked.
     */
    CFE_ES_LockSharedData(__func__, __LINE__);
    NumTasks   = 0;
    TaskRecPtr = CFE_ES_Global.TaskTable;
    for (i = 0; i < OS_MAX_TASKS; ++i)
    {
        if (CFE_ES_TaskRecordIsUsed(TaskRecPtr))
        {
            TaskList[NumTasks] = CFE_ES_TaskRecordGetID(TaskRecPtr);
            ++NumTasks;
        }
        ++TaskRecPtr;
    }
    CFE_ES_UnlockSharedData(__func__, __LINE__);

    /*
    ** Copy the commanded filename into local buffer to ensure size limitation and to allow for modification
    */
    Result = CFE_FS_ParseInputFileNameEx(QueryAllFilename, CmdPtr->FileName, sizeof(QueryAllFilename),
                                         sizeof(CmdPtr->FileName), CFE_PLATFORM_ES_DEFAULT_TASK_LOG_FILE,
                                         CFE_FS_GetDefaultMountPoint(CFE_FS_FileCategory_BINARY_DATA_DUMP),
                                         CFE_FS_GetDefaultExtension(CFE_FS_FileCategory_BINARY_DATA_DUMP));

    if (Result == CFE_SUCCESS)
    {
        /*
        ** Create (or truncate) ES task log data file
        */
        OsStatus = OS_OpenCreate(&FileDescriptor, QueryAllFilename, OS_FILE_FLAG_CREATE | OS_FILE_FLAG_TRUNCATE,
                                 OS_WRITE_ONLY);

        if (OsStatus != OS_SUCCESS)
        {
            CFE_EVS_SendEvent(CFE_ES_TASKINFO_OSCREATE_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Failed to write Task Info file, OS_OpenCreate RC = %ld", (long)OsStatus);
            Result = CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
        }
    }
    else
    {
        CFE_EVS_SendEvent(CFE_ES_TASKINFO_OSCREATE_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Failed to write Task Info file, CFE_FS_ParseInputFileNameEx RC = %08x",
                          (unsigned int)Result);
    }

    if (Result >= 0)
    {
        /*
        ** Initialize cFE file header
        */
        CFE_FS_InitHeader(&FileHeader, CFE_ES_TASK_LOG_DESC, CFE_FS_SubType_ES_QUERYALLTASKS);

        /*
        ** Output the Standard cFE File Header to the App File
        */
        Result = CFE_FS_WriteHeader(FileDescriptor, &FileHeader);

        if (Result != sizeof(CFE_FS_Header_t))
        {
            OS_close(FileDescriptor);
            CFE_ES_Global.TaskData.CommandErrorCounter++;
            CFE_EVS_SendEvent(CFE_ES_TASKINFO_WRHDR_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Failed to write Task Info file, WriteHdr RC = 0x%08X, exp %d", (unsigned int)Result,
                              (int)sizeof(CFE_FS_Header_t));
            /*
             * returning "success" here as there is no other recourse;
             * the full extent of the error recovery has been done
             */
            return CFE_SUCCESS;
        }

        /*
        ** Maintain statistics of amount of data written to file
        */
        FileSize += sizeof(CFE_FS_Header_t);

        /*
        ** Loop through the ES AppTable for main applications
        */
        for (i = 0; i < NumTasks; ++i)
        {
            /*
            ** Populate the AppInfo entry
            */
            Result = CFE_ES_GetTaskInfo(&TaskInfo, TaskList[i]);
            if (Result == CFE_SUCCESS)
            {
                /*
                ** Write the local entry to file
                */
                OsStatus = OS_write(FileDescriptor, &TaskInfo, sizeof(CFE_ES_TaskInfo_t));
                if (OsStatus != sizeof(CFE_ES_TaskInfo_t))
                {
                    OS_close(FileDescriptor);
                    CFE_ES_Global.TaskData.CommandErrorCounter++;
                    CFE_EVS_SendEvent(CFE_ES_TASKINFO_WR_ERR_EID, CFE_EVS_EventType_ERROR,
                                      "Failed to write Task Info file, Task write RC = %ld, exp %d", (long)OsStatus,
                                      (int)sizeof(CFE_ES_TaskInfo_t));
                    /*
                     * returning "success" here as there is no other recourse;
                     * the full extent of the error recovery has been done
                     */
                    return CFE_SUCCESS;
                }

                FileSize += sizeof(CFE_ES_TaskInfo_t);
                EntryCount++;
            }

        } /* end for */

        OS_close(FileDescriptor);
        CFE_ES_Global.TaskData.CommandCounter++;
        CFE_EVS_SendEvent(CFE_ES_TASKINFO_EID, CFE_EVS_EventType_DEBUG,
                          "Task Info file written to %s, Entries=%d, FileSize=%lu", QueryAllFilename, (int)EntryCount,
                          (unsigned long)FileSize);
    }
    else
    {
        CFE_ES_Global.TaskData.CommandErrorCounter++;
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_ClearSysLogCmd(const CFE_ES_ClearSysLogCmd_t *data)
{
    /*
    ** Clear syslog index and memory area
    */

    CFE_ES_LockSharedData(__func__, __LINE__);
    CFE_ES_SysLogClear_Unsync();
    CFE_ES_UnlockSharedData(__func__, __LINE__);

    /*
    ** This command will always succeed...
    */
    CFE_ES_Global.TaskData.CommandCounter++;
    CFE_EVS_SendEvent(CFE_ES_SYSLOG1_INF_EID, CFE_EVS_EventType_INFORMATION, "Cleared Executive Services log data");

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_OverWriteSysLogCmd(const CFE_ES_OverWriteSysLogCmd_t *data)
{
    int32                                      Status;
    const CFE_ES_OverWriteSysLogCmd_Payload_t *CmdPtr = &data->Payload;

    Status = CFE_ES_SysLogSetMode(CmdPtr->Mode);

    if (Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CFE_ES_ERR_SYSLOGMODE_EID, CFE_EVS_EventType_ERROR,
                          "Set OverWriteSysLog Command: Invalid Mode setting = %d", (int)CmdPtr->Mode);

        CFE_ES_Global.TaskData.CommandErrorCounter++;
    }
    else
    {
        CFE_EVS_SendEvent(CFE_ES_SYSLOGMODE_EID, CFE_EVS_EventType_DEBUG,
                          "Set OverWriteSysLog Command Received with Mode setting = %d", (int)CmdPtr->Mode);

        CFE_ES_Global.TaskData.CommandCounter++;
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_WriteSysLogCmd(const CFE_ES_WriteSysLogCmd_t *data)
{
    const CFE_ES_FileNameCmd_Payload_t *CmdPtr = &data->Payload;
    int32                               Stat;
    char                                LogFilename[OS_MAX_PATH_LEN];

    /*
    ** Copy the filename into local buffer with default name/path/extension if not specified
    **
    ** Note even though this fundamentally contains strings, it is written as a binary file with an FS header,
    ** not as normal text file, so still using the BINARY DATA DUMP category for its default extension.
    */
    Stat = CFE_FS_ParseInputFileNameEx(LogFilename, CmdPtr->FileName, sizeof(LogFilename), sizeof(CmdPtr->FileName),
                                       CFE_PLATFORM_ES_DEFAULT_SYSLOG_FILE,
                                       CFE_FS_GetDefaultMountPoint(CFE_FS_FileCategory_BINARY_DATA_DUMP),
                                       CFE_FS_GetDefaultExtension(CFE_FS_FileCategory_BINARY_DATA_DUMP));

    if (Stat != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CFE_ES_SYSLOG2_ERR_EID, CFE_EVS_EventType_ERROR, "Error parsing file name RC = 0x%08X",
                          (unsigned int)Stat);
    }
    else
    {
        Stat = CFE_ES_SysLogDump(LogFilename);
    }

    if (Stat == CFE_SUCCESS)
    {
        CFE_ES_Global.TaskData.CommandCounter++;
    }
    else
    {
        CFE_ES_Global.TaskData.CommandErrorCounter++;
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_ClearERLogCmd(const CFE_ES_ClearERLogCmd_t *data)
{
    /*
    ** Clear ER log data buffer
    */

    memset(CFE_ES_Global.ResetDataPtr->ERLog, 0, sizeof(CFE_ES_Global.ResetDataPtr->ERLog));

    /*
    ** Reset ER log buffer index
    */

    CFE_ES_Global.ResetDataPtr->ERLogIndex = 0;

    /*
    ** Set Number of Entries in ER log buffer back to zero
    */
    CFE_ES_Global.ResetDataPtr->ERLogEntries = 0;

    /*
    ** This command will always succeed
    */
    CFE_ES_Global.TaskData.CommandCounter++;
    CFE_EVS_SendEvent(CFE_ES_ERLOG1_INF_EID, CFE_EVS_EventType_INFORMATION, "Cleared ES Exception and Reset Log data");

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_WriteERLogCmd(const CFE_ES_WriteERLogCmd_t *data)
{
    const CFE_ES_FileNameCmd_Payload_t *CmdPtr = &data->Payload;
    CFE_ES_BackgroundLogDumpGlobal_t *  StatePtr;
    int32                               Status;

    StatePtr = &CFE_ES_Global.BackgroundERLogDumpState;

    /* check if pending before overwriting fields in the structure */
    if (CFE_FS_BackgroundFileDumpIsPending(&StatePtr->FileWrite))
    {
        Status = CFE_STATUS_REQUEST_ALREADY_PENDING;
    }
    else
    {
        /* Reset the entire state object (just for good measure, ensure no stale data) */
        memset(StatePtr, 0, sizeof(*StatePtr));

        /*
         * Fill out the remainder of meta data.
         * This data is currently the same for every request
         */
        StatePtr->FileWrite.FileSubType = CFE_FS_SubType_ES_ERLOG;
        snprintf(StatePtr->FileWrite.Description, sizeof(StatePtr->FileWrite.Description), CFE_ES_ER_LOG_DESC);

        StatePtr->FileWrite.GetData = CFE_ES_BackgroundERLogFileDataGetter;
        StatePtr->FileWrite.OnEvent = CFE_ES_BackgroundERLogFileEventHandler;

        /*
        ** Copy the filename into local buffer with default name/path/extension if not specified
        */
        Status = CFE_FS_ParseInputFileNameEx(StatePtr->FileWrite.FileName, CmdPtr->FileName,
                                             sizeof(StatePtr->FileWrite.FileName), sizeof(CmdPtr->FileName),
                                             CFE_PLATFORM_ES_DEFAULT_ER_LOG_FILE,
                                             CFE_FS_GetDefaultMountPoint(CFE_FS_FileCategory_BINARY_DATA_DUMP),
                                             CFE_FS_GetDefaultExtension(CFE_FS_FileCategory_BINARY_DATA_DUMP));

        if (Status == CFE_SUCCESS)
        {
            Status = CFE_FS_BackgroundFileDumpRequest(&StatePtr->FileWrite);
        }
    }

    if (Status != CFE_SUCCESS)
    {
        if (Status == CFE_STATUS_REQUEST_ALREADY_PENDING)
        {
            /* Specific event if already pending */
            CFE_EVS_SendEvent(CFE_ES_ERLOG_PENDING_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Error log write already in progress");
        }
        else
        {
            /* Some other validation issue e.g. bad file name */
            CFE_EVS_SendEvent(CFE_ES_ERLOG2_ERR_EID, CFE_EVS_EventType_ERROR, "Error creating file, RC = %d",
                              (int)Status);
        }

        /* background dump did not start, consider this an error */
        CFE_ES_Global.TaskData.CommandErrorCounter++;
    }
    else
    {
        CFE_ES_Global.TaskData.CommandCounter++;
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_ResetPRCountCmd(const CFE_ES_ResetPRCountCmd_t *data)
{
    /*
    ** Reset the processor reset count
    */
    CFE_ES_Global.ResetDataPtr->ResetVars.ProcessorResetCount = 0;

    /*
    ** This command will always succeed.
    */
    CFE_EVS_SendEvent(CFE_ES_RESET_PR_COUNT_EID, CFE_EVS_EventType_INFORMATION, "Set Processor Reset Count to Zero");

    CFE_ES_Global.TaskData.CommandCounter++;

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_SetMaxPRCountCmd(const CFE_ES_SetMaxPRCountCmd_t *data)
{
    const CFE_ES_SetMaxPRCountCmd_Payload_t *cmd = &data->Payload;

    /*
    ** Set the MAX Processor reset count
    */
    CFE_ES_Global.ResetDataPtr->ResetVars.MaxProcessorResetCount = cmd->MaxPRCount;

    /*
    ** This command will always succeed.
    */
    CFE_EVS_SendEvent(CFE_ES_SET_MAX_PR_COUNT_EID, CFE_EVS_EventType_INFORMATION,
                      "Maximum Processor Reset Count set to: %d", (int)cmd->MaxPRCount);

    CFE_ES_Global.TaskData.CommandCounter++;

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_DeleteCDSCmd(const CFE_ES_DeleteCDSCmd_t *data)
{
    int32                                Status;
    const CFE_ES_DeleteCDSCmd_Payload_t *cmd = &data->Payload;
    char                                 LocalCdsName[CFE_MISSION_ES_CDS_MAX_FULL_NAME_LEN];

    CFE_SB_MessageStringGet(LocalCdsName, (char *)cmd->CdsName, NULL, sizeof(LocalCdsName), sizeof(cmd->CdsName));

    Status = CFE_ES_DeleteCDS(LocalCdsName, false);

    if (Status == CFE_ES_CDS_WRONG_TYPE_ERR)
    {
        CFE_EVS_SendEvent(CFE_ES_CDS_DELETE_TBL_ERR_EID, CFE_EVS_EventType_ERROR,
                          "CDS '%s' is a Critical Table CDS. Must be deleted via TBL Command", LocalCdsName);

        CFE_ES_Global.TaskData.CommandErrorCounter++;
    }
    else if (Status == CFE_ES_CDS_OWNER_ACTIVE_ERR)
    {
        CFE_EVS_SendEvent(CFE_ES_CDS_OWNER_ACTIVE_EID, CFE_EVS_EventType_ERROR,
                          "CDS '%s' not deleted because owning app is active", LocalCdsName);

        CFE_ES_Global.TaskData.CommandErrorCounter++;
    }
    else if (Status == CFE_ES_ERR_NAME_NOT_FOUND)
    {
        CFE_EVS_SendEvent(CFE_ES_CDS_NAME_ERR_EID, CFE_EVS_EventType_ERROR, "Unable to locate '%s' in CDS Registry",
                          LocalCdsName);

        CFE_ES_Global.TaskData.CommandErrorCounter++;
    }
    else if (Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CFE_ES_CDS_DELETE_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Error while deleting '%s' from CDS, See SysLog.(Err=0x%08X)", LocalCdsName,
                          (unsigned int)Status);

        CFE_ES_Global.TaskData.CommandErrorCounter++;
    }
    else
    {
        CFE_EVS_SendEvent(CFE_ES_CDS_DELETED_INFO_EID, CFE_EVS_EventType_INFORMATION,
                          "Successfully removed '%s' from CDS", LocalCdsName);

        CFE_ES_Global.TaskData.CommandCounter++;
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_SendMemPoolStatsCmd(const CFE_ES_SendMemPoolStatsCmd_t *data)
{
    const CFE_ES_SendMemPoolStatsCmd_Payload_t *Cmd;
    CFE_ES_MemHandle_t                          MemHandle;
    bool                                        ValidHandle;

    Cmd = &data->Payload;

    /* Verify the handle to make sure it is legit */
    MemHandle   = Cmd->PoolHandle;
    ValidHandle = CFE_ES_ValidateHandle(MemHandle);

    if (ValidHandle)
    {
        /* Extract the memory statistics from the memory pool */
        CFE_ES_GetMemPoolStats(&CFE_ES_Global.TaskData.MemStatsPacket.Payload.PoolStats, MemHandle);

        /* Echo the specified pool handle in the telemetry packet */
        CFE_ES_Global.TaskData.MemStatsPacket.Payload.PoolHandle = MemHandle;

        /*
        ** Send memory statistics telemetry packet.
        */
        CFE_SB_TimeStampMsg(CFE_MSG_PTR(CFE_ES_Global.TaskData.MemStatsPacket.TelemetryHeader));
        CFE_SB_TransmitMsg(CFE_MSG_PTR(CFE_ES_Global.TaskData.MemStatsPacket.TelemetryHeader), true);

        CFE_ES_Global.TaskData.CommandCounter++;
        CFE_EVS_SendEvent(CFE_ES_TLM_POOL_STATS_INFO_EID, CFE_EVS_EventType_DEBUG,
                          "Successfully telemetered memory pool stats for 0x%08lX",
                          CFE_RESOURCEID_TO_ULONG(Cmd->PoolHandle));
    }
    else
    {
        CFE_ES_Global.TaskData.CommandErrorCounter++;
        CFE_EVS_SendEvent(CFE_ES_INVALID_POOL_HANDLE_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Cannot telemeter memory pool stats. Illegal Handle (0x%08lX)",
                          CFE_RESOURCEID_TO_ULONG(Cmd->PoolHandle));
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_DumpCDSRegistryCmd(const CFE_ES_DumpCDSRegistryCmd_t *data)
{
    CFE_FS_Header_t                            StdFileHeader;
    osal_id_t                                  FileDescriptor = OS_OBJECT_ID_UNDEFINED;
    int32                                      OsStatus;
    int32                                      Status;
    int16                                      RegIndex = 0;
    const CFE_ES_DumpCDSRegistryCmd_Payload_t *CmdPtr   = &data->Payload;
    char                                       DumpFilename[OS_MAX_PATH_LEN];
    CFE_ES_CDS_RegRec_t *                      RegRecPtr;
    CFE_ES_CDSRegDumpRec_t                     DumpRecord;
    int32                                      FileSize   = 0;
    int32                                      NumEntries = 0;

    /*
    ** Copy the filename into local buffer with default name/path/extension if not specified
    */
    Status = CFE_FS_ParseInputFileNameEx(DumpFilename, CmdPtr->DumpFilename, sizeof(DumpFilename),
                                         sizeof(CmdPtr->DumpFilename), CFE_PLATFORM_ES_DEFAULT_CDS_REG_DUMP_FILE,
                                         CFE_FS_GetDefaultMountPoint(CFE_FS_FileCategory_BINARY_DATA_DUMP),
                                         CFE_FS_GetDefaultExtension(CFE_FS_FileCategory_BINARY_DATA_DUMP));

    if (Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CFE_ES_CREATING_CDS_DUMP_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Error parsing CDS dump filename, Status=0x%08X", (unsigned int)Status);
    }
    else
    {
        /* Create a new dump file, overwriting anything that may have existed previously */
        OsStatus =
            OS_OpenCreate(&FileDescriptor, DumpFilename, OS_FILE_FLAG_CREATE | OS_FILE_FLAG_TRUNCATE, OS_WRITE_ONLY);

        if (OsStatus != OS_SUCCESS)
        {
            CFE_EVS_SendEvent(CFE_ES_CREATING_CDS_DUMP_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Error creating CDS dump file '%s', Status=%ld", DumpFilename, (long)OsStatus);
            Status = CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
        }
    }

    if (Status == OS_SUCCESS)
    {
        /* Initialize the standard cFE File Header for the Dump File */
        CFE_FS_InitHeader(&StdFileHeader, "CDS_Registry", CFE_FS_SubType_ES_CDS_REG);

        /* Output the Standard cFE File Header to the Dump File */
        Status = CFE_FS_WriteHeader(FileDescriptor, &StdFileHeader);

        /* Maintain statistics of amount of data written to file */
        FileSize += Status;

        if (Status == sizeof(CFE_FS_Header_t))
        {
            OsStatus  = sizeof(CFE_ES_CDSRegDumpRec_t);
            RegRecPtr = CFE_ES_Global.CDSVars.Registry;
            while ((RegIndex < CFE_PLATFORM_ES_CDS_MAX_NUM_ENTRIES) && (OsStatus == sizeof(CFE_ES_CDSRegDumpRec_t)))
            {
                /* Check to see if the Registry entry is empty */
                if (CFE_ES_CDSBlockRecordIsUsed(RegRecPtr))
                {
                    /* Fill CDS Registry Dump Record with relevant information */
                    memset(&DumpRecord, 0, sizeof(DumpRecord));
                    DumpRecord.Size   = CFE_ES_MEMOFFSET_C(CFE_ES_CDSBlockRecordGetUserSize(RegRecPtr));
                    DumpRecord.Handle = CFE_ES_CDSBlockRecordGetID(RegRecPtr);
                    DumpRecord.Table  = RegRecPtr->Table;
                    strncpy(DumpRecord.Name, RegRecPtr->Name, sizeof(DumpRecord.Name) - 1);

                    /* Output Registry Dump Record to Registry Dump File */
                    OsStatus = OS_write(FileDescriptor, &DumpRecord, sizeof(CFE_ES_CDSRegDumpRec_t));

                    FileSize += (long)OsStatus;
                    NumEntries++;
                }

                /* Look at the next entry in the Registry */
                ++RegIndex;
                ++RegRecPtr;
            }

            if (OsStatus == sizeof(CFE_ES_CDSRegDumpRec_t))
            {
                CFE_EVS_SendEvent(CFE_ES_CDS_REG_DUMP_INF_EID, CFE_EVS_EventType_DEBUG,
                                  "Successfully dumped CDS Registry to '%s':Size=%d,Entries=%d", DumpFilename,
                                  (int)FileSize, (int)NumEntries);

                /* Increment Successful Command Counter */
                CFE_ES_Global.TaskData.CommandCounter++;
            }
            else
            {
                CFE_EVS_SendEvent(CFE_ES_CDS_DUMP_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "Error writing CDS Registry to '%s', Status=%ld", DumpFilename, (long)OsStatus);

                /* Increment Command Error Counter */
                CFE_ES_Global.TaskData.CommandErrorCounter++;
            }
        }
        else
        {
            CFE_EVS_SendEvent(CFE_ES_WRITE_CFE_HDR_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Error writing cFE File Header to '%s', Status=0x%08X", DumpFilename,
                              (unsigned int)Status);

            /* Increment Command Error Counter */
            CFE_ES_Global.TaskData.CommandErrorCounter++;
        }

        /* We are done outputting data to the dump file.  Close it. */
        OS_close(FileDescriptor);
    }
    else
    {
        /* Increment Command Error Counter */
        CFE_ES_Global.TaskData.CommandErrorCounter++;
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_ES_FileWriteByteCntErr(const char *Filename, size_t Requested, int32 Status)
{
    CFE_EVS_SendEvent(CFE_ES_FILEWRITE_ERR_EID, CFE_EVS_EventType_ERROR,
                      "File write,byte cnt err,file %s,request=%u,status=0x%08x", Filename, (unsigned int)Requested,
                      (unsigned int)Status);
}
