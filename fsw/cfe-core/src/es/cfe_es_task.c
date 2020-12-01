/*
**  GSC-18128-1, "Core Flight Executive Version 6.7"
**
**  Copyright (c) 2006-2019 United States Government as represented by
**  the Administrator of the National Aeronautics and Space Administration.
**  All Rights Reserved.
**
**  Licensed under the Apache License, Version 2.0 (the "License");
**  you may not use this file except in compliance with the License.
**  You may obtain a copy of the License at
**
**    http://www.apache.org/licenses/LICENSE-2.0
**
**  Unless required by applicable law or agreed to in writing, software
**  distributed under the License is distributed on an "AS IS" BASIS,
**  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**  See the License for the specific language governing permissions and
**  limitations under the License.
*/

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
#include "private/cfe_private.h"
#include "cfe_platform_cfg.h"
#include "cfe_version.h"
#include "cfe_es_global.h"
#include "cfe_es_apps.h"
#include "cfe_es_resource.h"
#include "cfe_es_events.h"
#include "cfe_es_verify.h"
#include "cfe_es_task.h"
#include "cfe_es_log.h"
#include "cfe_es_cds.h"
#include "cfe_fs.h"
#include "cfe_psp.h"
#include "cfe_msgids.h"

#include <string.h>

/*
** Defines
*/
#define CFE_ES_PERF_TRIGGERMASK_INT_SIZE    (sizeof(CFE_ES_ResetDataPtr->Perf.MetaData.TriggerMask) / sizeof(uint32))
#define CFE_ES_PERF_TRIGGERMASK_EXT_SIZE    (sizeof(CFE_ES_TaskData.HkPacket.Payload.PerfTriggerMask) / sizeof(uint32))
#define CFE_ES_PERF_FILTERMASK_INT_SIZE     (sizeof(CFE_ES_ResetDataPtr->Perf.MetaData.FilterMask) / sizeof(uint32))
#define CFE_ES_PERF_FILTERMASK_EXT_SIZE     (sizeof(CFE_ES_TaskData.HkPacket.Payload.PerfFilterMask) / sizeof(uint32))

/*
** This define should be put in the OS API headers -- Right now it matches what the OS API uses
*/
#define OS_MAX_PRIORITY 255

/*
** Executive Services (ES) task global data.
*/
CFE_ES_TaskData_t CFE_ES_TaskData;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_TaskMain() -- Task entry point and main process loop     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_ES_TaskMain(void)
{
    int32   Status;
    uint32  AppRunStatus = CFE_ES_RunStatus_APP_RUN;


    /*
    ** Performance Time Stamp Entry
    */
    CFE_ES_PerfLogEntry(CFE_MISSION_ES_MAIN_PERF_ID);

    /*
    ** Perform task specific initialization.
    */
    Status = CFE_ES_TaskInit();
    if ( Status != CFE_SUCCESS )
    {
       /*
       ** Create a syslog entry
       */
       CFE_ES_WriteToSysLog("ES:Application Init Failed,RC=0x%08X\n", (unsigned int)Status);


       /*
       ** Allow Core App to Exit
       */
       AppRunStatus = CFE_ES_RunStatus_CORE_APP_INIT_ERROR;

    } /* end if */

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
        Status = CFE_SB_RcvMsg(&CFE_ES_TaskData.MsgPtr,
                                  CFE_ES_TaskData.CmdPipe,
                                  CFE_SB_PEND_FOREVER);

        /*
        ** Performance Time Stamp Entry
        */
        CFE_ES_PerfLogEntry(CFE_MISSION_ES_MAIN_PERF_ID);

        if (Status == CFE_SUCCESS)
        {
           /*
           ** Process Software Bus message.
           */
           CFE_ES_TaskPipe(CFE_ES_TaskData.MsgPtr);

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
            CFE_ES_WriteToSysLog("ES:Error reading cmd pipe,RC=0x%08X\n",(unsigned int)Status);

            /*
            ** Allow Core App to Exit
            */
            AppRunStatus = CFE_ES_RunStatus_CORE_APP_RUNTIME_ERROR;

        }  /* end if */

    } /* end while */

    /*
    ** Performance Time Stamp Exit
    */
    CFE_ES_PerfLogExit(CFE_MISSION_ES_MAIN_PERF_ID);

    /*
    ** Exit the application, CFE_ES_ExitApp will not return.
    */
    CFE_ES_ExitApp(AppRunStatus);

} /* End of CFE_ES_TaskMain() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_TaskInit() -- ES task initialization                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 CFE_ES_TaskInit(void)
{
    int32   Status;
    uint32  SizeofCfeSegment;
    cpuaddr CfeSegmentAddr;
    char    EventBuffer[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    char    VersionBuffer[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    uint32  Remaining;

    /*
    ** Register the Application
    */
    Status = CFE_ES_RegisterApp();
    if ( Status != CFE_SUCCESS )
    {
        CFE_ES_WriteToSysLog("ES:Call to CFE_ES_RegisterApp Failed, RC = 0x%08X\n", (unsigned int)Status);
        return(Status);
    }

    /*
    ** Initialize task command execution counters
    */
    CFE_ES_TaskData.CommandCounter = 0;
    CFE_ES_TaskData.CommandErrorCounter = 0;

    /*
    ** Initialize task configuration data
    */
    strcpy(CFE_ES_TaskData.PipeName, "ES_CMD_PIPE");
    CFE_ES_TaskData.PipeDepth = 12;

    CFE_ES_TaskData.LimitHK   = 2;
    CFE_ES_TaskData.LimitCmd  = 4;

    /*
    ** Initialize systemlog to default Power On or Processor Reset mode
    */
    if (CFE_ES_GetResetType(NULL) == CFE_PSP_RST_TYPE_POWERON)                                                                   
    {
        CFE_ES_ResetDataPtr->SystemLogMode = CFE_PLATFORM_ES_DEFAULT_POR_SYSLOG_MODE;
    }
    else
    {
        CFE_ES_ResetDataPtr->SystemLogMode = CFE_PLATFORM_ES_DEFAULT_PR_SYSLOG_MODE;
    }

    /*
    ** Register event filter table.
    */
    Status = CFE_EVS_Register(NULL,0,CFE_EVS_EventFilter_BINARY);
    if ( Status != CFE_SUCCESS )
    {
        CFE_ES_WriteToSysLog("ES:Call to CFE_EVS_Register Failed, RC = 0x%08X\n", (unsigned int)Status);
        return(Status);
    }

    /*
    ** Initialize housekeeping packet (clear user data area)
    */
    CFE_MSG_Init(&CFE_ES_TaskData.HkPacket.TlmHeader.BaseMsg,
                 CFE_SB_ValueToMsgId(CFE_ES_HK_TLM_MID),
                 sizeof(CFE_ES_TaskData.HkPacket));

    /*
    ** Initialize single application telemetry packet
    */
    CFE_MSG_Init(&CFE_ES_TaskData.OneAppPacket.TlmHeader.BaseMsg,
                 CFE_SB_ValueToMsgId(CFE_ES_APP_TLM_MID),
                 sizeof(CFE_ES_TaskData.OneAppPacket));

    /*
    ** Initialize memory pool statistics telemetry packet
    */
    CFE_MSG_Init(&CFE_ES_TaskData.MemStatsPacket.TlmHeader.BaseMsg,
                 CFE_SB_ValueToMsgId(CFE_ES_MEMSTATS_TLM_MID),
                 sizeof(CFE_ES_TaskData.MemStatsPacket));

    /*
    ** Create Software Bus message pipe
    */
    Status = CFE_SB_CreatePipe(&CFE_ES_TaskData.CmdPipe, CFE_ES_TaskData.PipeDepth, CFE_ES_TaskData.PipeName);
    if ( Status != CFE_SUCCESS )
    {
        CFE_ES_WriteToSysLog("ES:Cannot Create SB Pipe, RC = 0x%08X\n", (unsigned int)Status);
        return(Status);
    }

    /*
    ** Subscribe to Housekeeping request commands
    */
    Status = CFE_SB_SubscribeEx(CFE_SB_ValueToMsgId(CFE_ES_SEND_HK_MID), CFE_ES_TaskData.CmdPipe,
                                CFE_SB_Default_Qos, CFE_ES_TaskData.LimitHK);
    if ( Status != CFE_SUCCESS )
    {
        CFE_ES_WriteToSysLog("ES:Cannot Subscribe to HK packet, RC = 0x%08X\n", (unsigned int)Status);
        return(Status);
    }

    /*
    ** Subscribe to ES task ground command packets
    */
    Status = CFE_SB_SubscribeEx(CFE_SB_ValueToMsgId(CFE_ES_CMD_MID), CFE_ES_TaskData.CmdPipe,
                                CFE_SB_Default_Qos, CFE_ES_TaskData.LimitCmd);
    if ( Status != CFE_SUCCESS )
    {
        CFE_ES_WriteToSysLog("ES:Cannot Subscribe to ES ground commands, RC = 0x%08X\n", (unsigned int)Status);
        return(Status);
    }

    /*
    ** Compute the CRC for the cfe core code segment and place
    ** in ES Housekeeping pkt.
    */
    Status = CFE_PSP_GetCFETextSegmentInfo( &CfeSegmentAddr, &SizeofCfeSegment);

    if ( Status == CFE_PSP_SUCCESS )
    {
       CFE_ES_TaskData.HkPacket.Payload.CFECoreChecksum = CFE_ES_CalculateCRC(
                                  (void *)(CfeSegmentAddr), SizeofCfeSegment, 0, CFE_MISSION_ES_DEFAULT_CRC);
    }
    else
    {
       CFE_ES_TaskData.HkPacket.Payload.CFECoreChecksum = 0xFFFF;
    }

    /*
    ** Initialize the version numbers in the ES Housekeeping pkt
    */
    CFE_ES_TaskData.HkPacket.Payload.CFEMajorVersion     = CFE_MAJOR_VERSION;
    CFE_ES_TaskData.HkPacket.Payload.CFEMinorVersion     = CFE_MINOR_VERSION;
    CFE_ES_TaskData.HkPacket.Payload.CFERevision         = CFE_REVISION;
    CFE_ES_TaskData.HkPacket.Payload.CFEMissionRevision  = CFE_MISSION_REV;
    CFE_ES_TaskData.HkPacket.Payload.OSALMajorVersion    = OS_MAJOR_VERSION;
    CFE_ES_TaskData.HkPacket.Payload.OSALMinorVersion    = OS_MINOR_VERSION;
    CFE_ES_TaskData.HkPacket.Payload.OSALRevision        = OS_REVISION;
    CFE_ES_TaskData.HkPacket.Payload.OSALMissionRevision = OS_MISSION_REV;

    /*
    ** Task startup event message.
    */
    Status = CFE_EVS_SendEvent(CFE_ES_INIT_INF_EID,
                      CFE_EVS_EventType_INFORMATION,
                      "cFE ES Initialized");
    if ( Status != CFE_SUCCESS )
    {
        CFE_ES_WriteToSysLog("ES:Error sending init event:RC=0x%08X\n", (unsigned int)Status);
        return(Status);
    }

#ifdef CFE_PSP_VERSION 

    Status = CFE_EVS_SendEvent(CFE_ES_INITSTATS_INF_EID, CFE_EVS_EventType_INFORMATION, 
                                "%s%s. cFE chksm %d",
                                CFS_VERSIONS, CFE_PSP_VERSION, (int)CFE_ES_TaskData.HkPacket.Payload.CFECoreChecksum);

#else  /* if CFE_PSP_VERSION not defined use integer version macros*/
    Status = CFE_EVS_SendEvent(CFE_ES_INITSTATS_INF_EID, CFE_EVS_EventType_INFORMATION, 
                                "\n%sv%d.%d.%d.%d\n cFE chksm %d",
                                CFS_VERSIONS, 
                                CFE_PSP_MAJOR_VERSION, CFE_PSP_MINOR_VERSION, CFE_PSP_REVISION, CFE_PSP_MISSION_REV,
                                (int)CFE_ES_TaskData.HkPacket.Payload.CFECoreChecksum);

#endif  /* CFE_PSP_VERSION */

    if ( Status != CFE_SUCCESS )
    {
        CFE_ES_WriteToSysLog("ES:Error sending version event:RC=0x%08X\n", (unsigned int)Status);
        return(Status);
    }

    /*
     ** Advertise the build and version information at start up
     ** If unique and non-error, reports component information
     */
    if (strstr(GLOBAL_CONFIGDATA.MissionVersion, "error"))
    {
       snprintf(EventBuffer, sizeof(EventBuffer), "Mission %s", GLOBAL_CONFIGDATA.Config);
    }
    else
    {
       snprintf(EventBuffer, sizeof(EventBuffer), "Mission %s.%s",
                GLOBAL_CONFIGDATA.MissionVersion, GLOBAL_CONFIGDATA.Config);
    }
    Remaining = sizeof(EventBuffer)-strlen(EventBuffer)-1;
    if(Remaining > 0 && strcmp(GLOBAL_CONFIGDATA.MissionVersion, GLOBAL_CONFIGDATA.CfeVersion))
    {
       snprintf(VersionBuffer, sizeof(VersionBuffer), ", CFE git version: %s",
                GLOBAL_CONFIGDATA.CfeVersion);
       VersionBuffer[Remaining] = 0;
       strcat(EventBuffer, VersionBuffer);
       Remaining = sizeof(EventBuffer)-strlen(EventBuffer)-1;
    }
    if(Remaining > 0 && strcmp(GLOBAL_CONFIGDATA.MissionVersion, GLOBAL_CONFIGDATA.OsalVersion))
    {
       snprintf(VersionBuffer, sizeof(VersionBuffer), ", OSAL git version: %s",
                GLOBAL_CONFIGDATA.OsalVersion);
       VersionBuffer[Remaining] = 0;
       strcat(EventBuffer, VersionBuffer);
    }

    Status = CFE_EVS_SendEvent(CFE_ES_VERSION_INF_EID,
          CFE_EVS_EventType_INFORMATION, "%s", EventBuffer);
    if ( Status != CFE_SUCCESS )
    {
       CFE_ES_WriteToSysLog("ES:Error sending version event:RC=0x%08X\n", (unsigned int)Status);
       return(Status);
    }

    Status = CFE_EVS_SendEvent(CFE_ES_BUILD_INF_EID,
          CFE_EVS_EventType_INFORMATION,
          "Build %s %s",
          GLOBAL_CONFIGDATA.Date, GLOBAL_CONFIGDATA.User);
    if ( Status != CFE_SUCCESS )
    {
       CFE_ES_WriteToSysLog("ES:Error sending build info event:RC=0x%08X\n", (unsigned int)Status);
       return(Status);
    }

    /*
     * Initialize the "background task" which is a low priority child task
     * devoted to maintence duties that do not need to execute on a 
     * strict/precise schedule.
     */
    Status = CFE_ES_BackgroundInit();
    if ( Status != CFE_SUCCESS )
    {
       CFE_ES_WriteToSysLog("ES:Error initializing background task:RC=0x%08X\n", (unsigned int)Status);
       return(Status);
    }

   return(CFE_SUCCESS);

} /* End of CFE_ES_TaskInit() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_TaskPipe() -- Process command pipe message               */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_ES_TaskPipe(CFE_MSG_Message_t *MsgPtr)
{
    CFE_SB_MsgId_t    MessageID   = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t CommandCode = 0;

    CFE_MSG_GetMsgId(MsgPtr, &MessageID);
    switch (CFE_SB_MsgIdToValue(MessageID))
    {
        /*
        ** Housekeeping telemetry request
        */
        case CFE_ES_SEND_HK_MID:
            CFE_ES_HousekeepingCmd((CFE_SB_CmdHdr_t*)MsgPtr);
            break;

        /*
        ** ES task ground commands
        */
        case CFE_ES_CMD_MID:

            CFE_MSG_GetFcnCode(MsgPtr, &CommandCode);
            switch (CommandCode)
            {
                case CFE_ES_NOOP_CC:
                    if (CFE_ES_VerifyCmdLength(MsgPtr, sizeof(CFE_ES_Noop_t)))
                    {
                        CFE_ES_NoopCmd((CFE_ES_Noop_t*)MsgPtr);
                    }
                    break;

                case CFE_ES_RESET_COUNTERS_CC:
                    if (CFE_ES_VerifyCmdLength(MsgPtr, sizeof(CFE_ES_ResetCounters_t)))
                    {
                        CFE_ES_ResetCountersCmd((CFE_ES_ResetCounters_t*)MsgPtr);
                    }
                    break;

                case CFE_ES_RESTART_CC:
                    if (CFE_ES_VerifyCmdLength(MsgPtr, sizeof(CFE_ES_Restart_t)))
                    {
                        CFE_ES_RestartCmd((CFE_ES_Restart_t*)MsgPtr);
                    }
                    break;

                case CFE_ES_START_APP_CC:
                    if (CFE_ES_VerifyCmdLength(MsgPtr, sizeof(CFE_ES_StartApp_t)))
                    {
                        CFE_ES_StartAppCmd((CFE_ES_StartApp_t*)MsgPtr);
                    }
                    break;

                case CFE_ES_STOP_APP_CC:
                    if (CFE_ES_VerifyCmdLength(MsgPtr, sizeof(CFE_ES_StopApp_t)))
                    {
                        CFE_ES_StopAppCmd((CFE_ES_StopApp_t*)MsgPtr);
                    }
                    break;

                case CFE_ES_RESTART_APP_CC:
                    if (CFE_ES_VerifyCmdLength(MsgPtr, sizeof(CFE_ES_RestartApp_t)))
                    {
                        CFE_ES_RestartAppCmd((CFE_ES_RestartApp_t*)MsgPtr);
                    }
                    break;

                case CFE_ES_RELOAD_APP_CC:
                    if (CFE_ES_VerifyCmdLength(MsgPtr, sizeof(CFE_ES_ReloadApp_t)))
                    {
                        CFE_ES_ReloadAppCmd((CFE_ES_ReloadApp_t*)MsgPtr);
                    }
                    break;

                case CFE_ES_QUERY_ONE_CC:
                    if (CFE_ES_VerifyCmdLength(MsgPtr, sizeof(CFE_ES_QueryOne_t)))
                    {
                        CFE_ES_QueryOneCmd((CFE_ES_QueryOne_t*)MsgPtr);
                    }
                    break;

                case CFE_ES_QUERY_ALL_CC:
                    if (CFE_ES_VerifyCmdLength(MsgPtr, sizeof(CFE_ES_QueryAll_t)))
                    {
                        CFE_ES_QueryAllCmd((CFE_ES_QueryAll_t*)MsgPtr);
                    }
                    break;

                case CFE_ES_QUERY_ALL_TASKS_CC:
                    if (CFE_ES_VerifyCmdLength(MsgPtr, sizeof(CFE_ES_QueryAllTasks_t)))
                    {
                        CFE_ES_QueryAllTasksCmd((CFE_ES_QueryAllTasks_t*)MsgPtr);
                    }
                    break;

                case CFE_ES_CLEAR_SYSLOG_CC:
                    if (CFE_ES_VerifyCmdLength(MsgPtr, sizeof(CFE_ES_ClearSyslog_t)))
                    {
                        CFE_ES_ClearSyslogCmd((CFE_ES_ClearSyslog_t*)MsgPtr);
                    }
                    break;

                case CFE_ES_WRITE_SYSLOG_CC:
                    if (CFE_ES_VerifyCmdLength(MsgPtr, sizeof(CFE_ES_WriteSyslog_t)))
                    {
                        CFE_ES_WriteSyslogCmd((CFE_ES_WriteSyslog_t*)MsgPtr);
                    }
                    break;

                case CFE_ES_OVER_WRITE_SYSLOG_CC:
                    if (CFE_ES_VerifyCmdLength(MsgPtr, sizeof(CFE_ES_OverWriteSyslog_t)))
                    {
                        CFE_ES_OverWriteSyslogCmd((CFE_ES_OverWriteSyslog_t*)MsgPtr);
                    }
                    break;

                case CFE_ES_CLEAR_ER_LOG_CC:
                    if (CFE_ES_VerifyCmdLength(MsgPtr, sizeof(CFE_ES_ClearERLog_t)))
                    {
                        CFE_ES_ClearERLogCmd((CFE_ES_ClearERLog_t*)MsgPtr);
                    }
                    break;

                case CFE_ES_WRITE_ER_LOG_CC:
                    if (CFE_ES_VerifyCmdLength(MsgPtr, sizeof(CFE_ES_WriteERLog_t)))
                    {
                        CFE_ES_WriteERLogCmd((CFE_ES_WriteERLog_t*)MsgPtr);
                    }
                    break;

                case CFE_ES_START_PERF_DATA_CC:
                    if (CFE_ES_VerifyCmdLength(MsgPtr, sizeof(CFE_ES_StartPerfData_t)))
                    {
                        CFE_ES_StartPerfDataCmd((CFE_ES_StartPerfData_t*)MsgPtr);
                    }
                    break;

                case CFE_ES_STOP_PERF_DATA_CC:
                    if (CFE_ES_VerifyCmdLength(MsgPtr, sizeof(CFE_ES_StopPerfData_t)))
                    {
                        CFE_ES_StopPerfDataCmd((CFE_ES_StopPerfData_t*)MsgPtr);
                    }
                    break;

                case CFE_ES_SET_PERF_FILTER_MASK_CC:
                    if (CFE_ES_VerifyCmdLength(MsgPtr, sizeof(CFE_ES_SetPerfFilterMask_t)))
                    {
                        CFE_ES_SetPerfFilterMaskCmd((CFE_ES_SetPerfFilterMask_t*)MsgPtr);
                    }
                    break;

                case CFE_ES_SET_PERF_TRIGGER_MASK_CC:
                    if (CFE_ES_VerifyCmdLength(MsgPtr, sizeof(CFE_ES_SetPerfTriggerMask_t)))
                    {
                        CFE_ES_SetPerfTriggerMaskCmd((CFE_ES_SetPerfTriggerMask_t*)MsgPtr);
                    }
                    break;

                case CFE_ES_RESET_PR_COUNT_CC:
                    if (CFE_ES_VerifyCmdLength(MsgPtr, sizeof(CFE_ES_ResetPRCount_t)))
                    {
                        CFE_ES_ResetPRCountCmd((CFE_ES_ResetPRCount_t*)MsgPtr);
                    }
                    break;

                case CFE_ES_SET_MAX_PR_COUNT_CC:
                    if (CFE_ES_VerifyCmdLength(MsgPtr, sizeof(CFE_ES_SetMaxPRCount_t)))
                    {
                        CFE_ES_SetMaxPRCountCmd((CFE_ES_SetMaxPRCount_t*)MsgPtr);
                    }
                    break;

                case CFE_ES_DELETE_CDS_CC:
                    if (CFE_ES_VerifyCmdLength(MsgPtr, sizeof(CFE_ES_DeleteCDS_t)))
                    {
                        CFE_ES_DeleteCDSCmd((CFE_ES_DeleteCDS_t*)MsgPtr);
                    }
                    break;

                case CFE_ES_SEND_MEM_POOL_STATS_CC:
                    if (CFE_ES_VerifyCmdLength(MsgPtr, sizeof(CFE_ES_SendMemPoolStats_t)))
                    {
                        CFE_ES_SendMemPoolStatsCmd((CFE_ES_SendMemPoolStats_t*)MsgPtr);
                    }
                    break;

                case CFE_ES_DUMP_CDS_REGISTRY_CC:
                    if (CFE_ES_VerifyCmdLength(MsgPtr, sizeof(CFE_ES_DumpCDSRegistry_t)))
                    {
                        CFE_ES_DumpCDSRegistryCmd((CFE_ES_DumpCDSRegistry_t*)MsgPtr);
                    }
                    break;

                default:
                    CFE_EVS_SendEvent(CFE_ES_CC1_ERR_EID, CFE_EVS_EventType_ERROR,
                     "Invalid ground command code: ID = 0x%X, CC = %d",
                     (unsigned int)CFE_SB_MsgIdToValue(MessageID), (int)CommandCode);
                    CFE_ES_TaskData.CommandErrorCounter++;
                    break;
            }
            break;

        default:

            CFE_EVS_SendEvent(CFE_ES_MID_ERR_EID, CFE_EVS_EventType_ERROR,
                             "Invalid command pipe message ID: 0x%X",
                              (unsigned int)CFE_SB_MsgIdToValue(MessageID));
            CFE_ES_TaskData.CommandErrorCounter++;
            break;
    }

} /* End of CFE_ES_TaskPipe() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_HousekeepingCmd() -- On-board command (HK request)       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 CFE_ES_HousekeepingCmd(const CFE_SB_CmdHdr_t *data)
{
    OS_heap_prop_t HeapProp;
    int32          stat;
    uint32         PerfIdx;

    /*
    ** Get command execution counters, system log entry count & bytes used.
    */
    CFE_ES_TaskData.HkPacket.Payload.CommandCounter = CFE_ES_TaskData.CommandCounter;
    CFE_ES_TaskData.HkPacket.Payload.CommandErrorCounter = CFE_ES_TaskData.CommandErrorCounter;

    CFE_ES_TaskData.HkPacket.Payload.SysLogBytesUsed = CFE_ES_MEMOFFSET_C(CFE_ES_ResetDataPtr->SystemLogEndIdx);
    CFE_ES_TaskData.HkPacket.Payload.SysLogSize = CFE_ES_MEMOFFSET_C(CFE_PLATFORM_ES_SYSTEM_LOG_SIZE);
    CFE_ES_TaskData.HkPacket.Payload.SysLogEntries   = CFE_ES_ResetDataPtr->SystemLogEntryNum;
    CFE_ES_TaskData.HkPacket.Payload.SysLogMode = CFE_ES_ResetDataPtr->SystemLogMode;

    CFE_ES_TaskData.HkPacket.Payload.ERLogIndex      = CFE_ES_ResetDataPtr->ERLogIndex;
    CFE_ES_TaskData.HkPacket.Payload.ERLogEntries    = CFE_ES_ResetDataPtr->ERLogEntries;

    CFE_ES_TaskData.HkPacket.Payload.RegisteredCoreApps      = CFE_ES_Global.RegisteredCoreApps;
    CFE_ES_TaskData.HkPacket.Payload.RegisteredExternalApps  = CFE_ES_Global.RegisteredExternalApps;
    CFE_ES_TaskData.HkPacket.Payload.RegisteredTasks         = CFE_ES_Global.RegisteredTasks;
    CFE_ES_TaskData.HkPacket.Payload.RegisteredLibs          = CFE_ES_Global.RegisteredLibs;

    CFE_ES_TaskData.HkPacket.Payload.ResetType = CFE_ES_ResetDataPtr->ResetVars.ResetType;
    CFE_ES_TaskData.HkPacket.Payload.ResetSubtype = CFE_ES_ResetDataPtr->ResetVars.ResetSubtype;
    CFE_ES_TaskData.HkPacket.Payload.ProcessorResets = CFE_ES_ResetDataPtr->ResetVars.ProcessorResetCount;
    CFE_ES_TaskData.HkPacket.Payload.MaxProcessorResets = CFE_ES_ResetDataPtr->ResetVars.MaxProcessorResetCount;
    CFE_ES_TaskData.HkPacket.Payload.BootSource = CFE_ES_ResetDataPtr->ResetVars.BootSource;

    CFE_ES_TaskData.HkPacket.Payload.PerfState = CFE_ES_ResetDataPtr->Perf.MetaData.State;
    CFE_ES_TaskData.HkPacket.Payload.PerfMode = CFE_ES_ResetDataPtr->Perf.MetaData.Mode;
    CFE_ES_TaskData.HkPacket.Payload.PerfTriggerCount = CFE_ES_ResetDataPtr->Perf.MetaData.TriggerCount;
    CFE_ES_TaskData.HkPacket.Payload.PerfDataStart = CFE_ES_ResetDataPtr->Perf.MetaData.DataStart;
    CFE_ES_TaskData.HkPacket.Payload.PerfDataEnd = CFE_ES_ResetDataPtr->Perf.MetaData.DataEnd;
    CFE_ES_TaskData.HkPacket.Payload.PerfDataCount = CFE_ES_ResetDataPtr->Perf.MetaData.DataCount;
    CFE_ES_TaskData.HkPacket.Payload.PerfDataToWrite = CFE_ES_GetPerfLogDumpRemaining();

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
            CFE_ES_TaskData.HkPacket.Payload.PerfTriggerMask[PerfIdx] =
                    CFE_ES_ResetDataPtr->Perf.MetaData.TriggerMask[PerfIdx];
        }
        else
        {
            CFE_ES_TaskData.HkPacket.Payload.PerfTriggerMask[PerfIdx] = 0;
        }
    }

    for (PerfIdx = 0; PerfIdx < CFE_ES_PERF_FILTERMASK_EXT_SIZE; ++PerfIdx)
    {
        if (PerfIdx < CFE_ES_PERF_FILTERMASK_INT_SIZE)
        {
            CFE_ES_TaskData.HkPacket.Payload.PerfFilterMask[PerfIdx] =
                    CFE_ES_ResetDataPtr->Perf.MetaData.FilterMask[PerfIdx];
        }
        else
        {
            CFE_ES_TaskData.HkPacket.Payload.PerfFilterMask[PerfIdx] = 0;
        }
    }

    stat = OS_HeapGetInfo(&HeapProp);

    /* 
     * If retrieving info from OSAL was not successful, 
     * zero out the property struct, so all sizes will
     * in turn be reported in telemetry as 0.
     */
    if(stat != OS_SUCCESS)
    {
        memset(&HeapProp, 0, sizeof(HeapProp));
    }

    CFE_ES_TaskData.HkPacket.Payload.HeapBytesFree = CFE_ES_MEMOFFSET_C(HeapProp.free_bytes);
    CFE_ES_TaskData.HkPacket.Payload.HeapBlocksFree = CFE_ES_MEMOFFSET_C(HeapProp.free_blocks);
    CFE_ES_TaskData.HkPacket.Payload.HeapMaxBlockSize = CFE_ES_MEMOFFSET_C(HeapProp.largest_free_block);

    /*
    ** Send housekeeping telemetry packet.
    */
    CFE_SB_TimeStampMsg((CFE_MSG_Message_t *) &CFE_ES_TaskData.HkPacket);
    CFE_SB_SendMsg((CFE_MSG_Message_t *) &CFE_ES_TaskData.HkPacket);

    /*
    ** This command does not affect the command execution counter.
    */

    return CFE_SUCCESS;
} /* End of CFE_ES_HousekeepingCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_NoopCmd() -- ES task ground command (NO-OP)              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 CFE_ES_NoopCmd(const CFE_ES_Noop_t *Cmd)
{
    /*
    ** Advertise the build and version information with the no-op command
    ** For unit testing purposes, it helps to put this first - the UT
    ** is checking for the last event sent to be NOOP_INF_EID.
    */
    CFE_EVS_SendEvent(CFE_ES_BUILD_INF_EID,
            CFE_EVS_EventType_INFORMATION,
            "Build %s %s",
            GLOBAL_CONFIGDATA.Date, GLOBAL_CONFIGDATA.User);

    /*
    ** This command will always succeed.
    */
    CFE_ES_TaskData.CommandCounter++;

                    
#ifdef CFE_PSP_VERSION
    CFE_EVS_SendEvent(CFE_ES_NOOP_INF_EID, CFE_EVS_EventType_INFORMATION,
                      "No-op command:\n %s%s",                    
                        CFS_VERSIONS, CFE_PSP_VERSION);

#else /* CFE_PSP_VERSION */

    CFE_EVS_SendEvent(CFE_ES_NOOP_INF_EID, CFE_EVS_EventType_INFORMATION,
                      "No-op command:\n %sv%d.%d.%d.%d",                    
                        CFS_VERSIONS,
                        CFE_PSP_MAJOR_VERSION, CFE_PSP_MINOR_VERSION, CFE_PSP_REVISION, CFE_PSP_MISSION_REV);

#endif /* CFE_PSP_VERSION */
    return CFE_SUCCESS;
} /* End of CFE_ES_NoopCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_ResetCountersCmd() -- ES task ground command (reset counters)    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 CFE_ES_ResetCountersCmd(const CFE_ES_ResetCounters_t *data)
{
    CFE_ES_TaskData.CommandCounter = 0;
    CFE_ES_TaskData.CommandErrorCounter = 0;

    /*
    ** This command will always succeed.
    */
    CFE_EVS_SendEvent(CFE_ES_RESET_INF_EID, CFE_EVS_EventType_INFORMATION,
            "Reset Counters command");

    return CFE_SUCCESS;
} /* End of CFE_ES_ResetCountersCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_RestartCmd() -- Restart cFE (may reset processor)        */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 CFE_ES_RestartCmd(const CFE_ES_Restart_t *data)
{
    const CFE_ES_RestartCmd_Payload_t *cmd = &data->Payload;

    if ((cmd->RestartType != CFE_PSP_RST_TYPE_PROCESSOR) &&
            (cmd->RestartType != CFE_PSP_RST_TYPE_POWERON))
    {
        CFE_ES_TaskData.CommandErrorCounter++;
        CFE_EVS_SendEvent(CFE_ES_BOOT_ERR_EID, CFE_EVS_EventType_ERROR,
                "Invalid cFE restart type: %d",
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
} /* End of CFE_ES_RestartCmd() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_StartAppCmd() -- Load (and start) single application     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 CFE_ES_StartAppCmd(const CFE_ES_StartApp_t *data)
{
    const CFE_ES_StartAppCmd_Payload_t *cmd = &data->Payload;
    CFE_ES_ResourceID_t   AppID;
    int32                 Result;
    int32                 FilenameLen;
    int32                 AppEntryLen;
    int32                 AppNameLen;
    char                  LocalFile[OS_MAX_PATH_LEN];
    char                  LocalEntryPt[OS_MAX_API_NAME];
    char                  LocalAppName[OS_MAX_API_NAME];

    /* Create local copies of all input strings and ensure null termination */
    FilenameLen = CFE_SB_MessageStringGet(LocalFile, (char *)cmd->AppFileName, NULL,
            sizeof(LocalFile), sizeof(cmd->AppFileName));

    AppEntryLen = CFE_SB_MessageStringGet(LocalEntryPt, (char *)cmd->AppEntryPoint, NULL,
            sizeof(LocalEntryPt), sizeof(cmd->AppEntryPoint));

    AppNameLen = CFE_SB_MessageStringGet(LocalAppName, (char *)cmd->Application, NULL,
            sizeof(LocalAppName), sizeof(cmd->Application));

    /*
    ** Verify command parameters
    */
    if (FilenameLen < 4)
    {
        CFE_ES_TaskData.CommandErrorCounter++;
        CFE_EVS_SendEvent(CFE_ES_START_INVALID_FILENAME_ERR_EID, CFE_EVS_EventType_ERROR,
                "CFE_ES_StartAppCmd: invalid filename: %s",
                LocalFile);
    }
    else if (AppEntryLen <= 0)
    {
        CFE_ES_TaskData.CommandErrorCounter++;
        CFE_EVS_SendEvent(CFE_ES_START_INVALID_ENTRY_POINT_ERR_EID, CFE_EVS_EventType_ERROR,
                "CFE_ES_StartAppCmd: App Entry Point is NULL.");
    }
    else if (AppNameLen <= 0)
    {
        CFE_ES_TaskData.CommandErrorCounter++;
        CFE_EVS_SendEvent(CFE_ES_START_NULL_APP_NAME_ERR_EID, CFE_EVS_EventType_ERROR,
                "CFE_ES_StartAppCmd: App Name is NULL.");
    }
    else if (cmd->StackSize < CFE_PLATFORM_ES_DEFAULT_STACK_SIZE)
    {
        CFE_ES_TaskData.CommandErrorCounter++;
        CFE_EVS_SendEvent(CFE_ES_START_STACK_ERR_EID, CFE_EVS_EventType_ERROR,
                "CFE_ES_StartAppCmd: Stack size is less than system Minimum: %d.",
                CFE_PLATFORM_ES_DEFAULT_STACK_SIZE);
    }
    else if (cmd->Priority > OS_MAX_PRIORITY)
    {
        CFE_ES_TaskData.CommandErrorCounter++;
        CFE_EVS_SendEvent(CFE_ES_START_PRIORITY_ERR_EID, CFE_EVS_EventType_ERROR,
                "CFE_ES_StartAppCmd: Priority is too large: %d.",
                (int)cmd->Priority);
    }
    else if ((cmd->ExceptionAction != CFE_ES_ExceptionAction_RESTART_APP) &&
            (cmd->ExceptionAction != CFE_ES_ExceptionAction_PROC_RESTART ))
    {
        CFE_ES_TaskData.CommandErrorCounter++;
        CFE_EVS_SendEvent(CFE_ES_START_EXC_ACTION_ERR_EID, CFE_EVS_EventType_ERROR,
                "CFE_ES_StartAppCmd: Invalid Exception Action: %d.",
                (int)cmd->ExceptionAction);
    }
    else
    {
       /*
       ** Invoke application loader/startup function.
       */
       Result = CFE_ES_AppCreate(&AppID, LocalFile,
                   LocalEntryPt,
                   LocalAppName,
                   (uint32) cmd->Priority,
                   (uint32) cmd->StackSize,
                   (uint32) cmd->ExceptionAction);

        /*
        ** Send appropriate event message
        */
        if (Result == CFE_SUCCESS)
        {
            CFE_ES_TaskData.CommandCounter++;
            CFE_EVS_SendEvent(CFE_ES_START_INF_EID, CFE_EVS_EventType_INFORMATION,
                    "Started %s from %s, AppID = %lu",
                    LocalAppName, LocalFile, CFE_ES_ResourceID_ToInteger(AppID));
        }
        else
        {
            CFE_ES_TaskData.CommandErrorCounter++;
            CFE_EVS_SendEvent(CFE_ES_START_ERR_EID, CFE_EVS_EventType_ERROR,
                    "Failed to start %s from %s, RC = 0x%08X",
                    LocalAppName, LocalFile, (unsigned int)Result);
        }

    } /* End if -- command parameter validation */

    return CFE_SUCCESS;
} /* End of CFE_ES_StartAppCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_StopAppCmd() -- Stop single application                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 CFE_ES_StopAppCmd(const CFE_ES_StopApp_t *data)
{
    const CFE_ES_AppNameCmd_Payload_t *cmd = &data->Payload;
    char LocalApp[OS_MAX_API_NAME];
    CFE_ES_ResourceID_t AppID;
    int32 Result;

    CFE_SB_MessageStringGet(LocalApp, (char *)cmd->Application, NULL,
            sizeof(LocalApp), sizeof(cmd->Application));

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
            CFE_ES_TaskData.CommandCounter++;
            CFE_EVS_SendEvent(CFE_ES_STOP_DBG_EID, CFE_EVS_EventType_DEBUG,
                    "Stop Application %s Initiated.", LocalApp);
        }
        else
        {
            CFE_ES_TaskData.CommandErrorCounter++;
            CFE_EVS_SendEvent(CFE_ES_STOP_ERR1_EID, CFE_EVS_EventType_ERROR,
                    "Stop Application %s Failed, RC = 0x%08X",
                    LocalApp, (unsigned int)Result);
        }
    }
    else
    {
        CFE_ES_TaskData.CommandErrorCounter++;
        CFE_EVS_SendEvent(CFE_ES_STOP_ERR2_EID, CFE_EVS_EventType_ERROR,
                "Stop Application %s, GetAppIDByName failed. RC = 0x%08X.",
                LocalApp, (unsigned int)Result);
    }

    return CFE_SUCCESS;
} /* End of CFE_ES_StopAppCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_RestartAppCmd() -- Restart a single application            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 CFE_ES_RestartAppCmd(const CFE_ES_RestartApp_t *data)
{
    const CFE_ES_AppNameCmd_Payload_t *cmd = &data->Payload;
    char LocalApp[OS_MAX_API_NAME];
    CFE_ES_ResourceID_t AppID;
    int32 Result;

    CFE_SB_MessageStringGet(LocalApp, (char *)cmd->Application, NULL,
            sizeof(LocalApp), sizeof(cmd->Application));

    Result = CFE_ES_GetAppIDByName(&AppID, LocalApp);

    if (Result == CFE_SUCCESS)
    {
        Result = CFE_ES_RestartApp(AppID);

        /*
        ** Send appropriate event message.
        */
        if (Result == CFE_SUCCESS)
        {
            CFE_ES_TaskData.CommandCounter++;
            CFE_EVS_SendEvent(CFE_ES_RESTART_APP_DBG_EID, CFE_EVS_EventType_DEBUG,
                    "Restart Application %s Initiated.", LocalApp);
        }
        else
        {
            CFE_ES_TaskData.CommandErrorCounter++;
            CFE_EVS_SendEvent(CFE_ES_RESTART_APP_ERR1_EID, CFE_EVS_EventType_ERROR,
                    "Restart Application %s Failed, RC = 0x%08X",
                    LocalApp, (unsigned int)Result);
        }
    }
    else
    {
        CFE_ES_TaskData.CommandErrorCounter++;
        CFE_EVS_SendEvent(CFE_ES_RESTART_APP_ERR2_EID, CFE_EVS_EventType_ERROR,
                "Restart Application %s, GetAppIDByName failed. RC = 0x%08X.",
                LocalApp, (unsigned int)Result);
    }

    return CFE_SUCCESS;
} /* End of CFE_ES_ResetAppCmd() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_ReloadAppCmd() -- Reload a single application            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 CFE_ES_ReloadAppCmd(const CFE_ES_ReloadApp_t *data)
{
    const CFE_ES_AppReloadCmd_Payload_t *cmd = &data->Payload;
    char LocalApp[OS_MAX_API_NAME];
    char LocalFileName[OS_MAX_PATH_LEN];
    CFE_ES_ResourceID_t  AppID;
    int32   Result;

    CFE_SB_MessageStringGet(LocalFileName, (char *)cmd->AppFileName, NULL,
            sizeof(LocalFileName), sizeof(cmd->AppFileName));
    CFE_SB_MessageStringGet(LocalApp, (char *)cmd->Application, NULL,
            sizeof(LocalApp), sizeof(cmd->Application));

    Result = CFE_ES_GetAppIDByName(&AppID, LocalApp);

    if (Result == CFE_SUCCESS)
    {
        Result = CFE_ES_ReloadApp(AppID, LocalFileName);

        /*
        ** Send appropriate event message.
        */
        if (Result == CFE_SUCCESS)
        {
            CFE_ES_TaskData.CommandCounter++;
            CFE_EVS_SendEvent(CFE_ES_RELOAD_APP_DBG_EID, CFE_EVS_EventType_DEBUG,
                    "Reload Application %s Initiated.", LocalApp);
        }
        else
        {
            CFE_ES_TaskData.CommandErrorCounter++;
            CFE_EVS_SendEvent(CFE_ES_RELOAD_APP_ERR1_EID, CFE_EVS_EventType_ERROR,
                    "Reload Application %s Failed, RC = 0x%08X",
                    LocalApp, (unsigned int)Result);
        }
    }
    else
    {
        CFE_ES_TaskData.CommandErrorCounter++;
        CFE_EVS_SendEvent(CFE_ES_RELOAD_APP_ERR2_EID, CFE_EVS_EventType_ERROR,
                "Reload Application %s, GetAppIDByName failed. RC = 0x%08X.",
                LocalApp, (unsigned int)Result);
    }

    return CFE_SUCCESS;
} /* End of CFE_ES_ReloadAppCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_QueryOneCmd() -- Request tlm packet with single app data */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 CFE_ES_QueryOneCmd(const CFE_ES_QueryOne_t *data)
{
    const CFE_ES_AppNameCmd_Payload_t *cmd = &data->Payload;
    char LocalApp[OS_MAX_API_NAME];
    CFE_ES_ResourceID_t ResourceID;
    int32 Result;

    CFE_SB_MessageStringGet(LocalApp, (char *)cmd->Application, NULL,
            sizeof(LocalApp), sizeof(cmd->Application));

    Result = CFE_ES_GetAppIDByName(&ResourceID, LocalApp);
    if (Result == CFE_ES_ERR_NAME_NOT_FOUND)
    {
        /* Also check for a matching library name */
        Result = CFE_ES_GetLibIDByName(&ResourceID, LocalApp);
    }

    if (Result == CFE_SUCCESS)
    {
        Result = CFE_ES_GetModuleInfo(&(CFE_ES_TaskData.OneAppPacket.Payload.AppInfo), ResourceID);
    }

    /*
    ** Send appropriate event message...
    */
    if (Result == CFE_SUCCESS)
    {
        /*
        ** Send application status telemetry packet.
        */
        CFE_SB_TimeStampMsg((CFE_MSG_Message_t *) &CFE_ES_TaskData.OneAppPacket);
        Result = CFE_SB_SendMsg((CFE_MSG_Message_t *) &CFE_ES_TaskData.OneAppPacket);
        if ( Result == CFE_SUCCESS )
        {
            CFE_ES_TaskData.CommandCounter++;
            CFE_EVS_SendEvent(CFE_ES_ONE_APP_EID, CFE_EVS_EventType_DEBUG,
                    "Sent %s application data", LocalApp);
        }
        else
        {
            CFE_ES_TaskData.CommandErrorCounter++;
            CFE_EVS_SendEvent(CFE_ES_ONE_ERR_EID, CFE_EVS_EventType_ERROR,
                    "Failed to send %s application data, RC = 0x%08X",
                    LocalApp, (unsigned int)Result);
        }
    }
    else
    {
        CFE_ES_TaskData.CommandErrorCounter++;
        CFE_EVS_SendEvent(CFE_ES_ONE_APPID_ERR_EID, CFE_EVS_EventType_ERROR,
                "Failed to send %s application data: GetAppIDByName Failed, RC = 0x%08X",
                LocalApp, (unsigned int)Result);
    }

    return CFE_SUCCESS;
} /* End of CFE_ES_QueryOneCmd() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_QueryAllCmd() -- Write all app data to file              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 CFE_ES_QueryAllCmd(const CFE_ES_QueryAll_t *data)
{
    CFE_FS_Header_t       FileHeader;
    osal_id_t             FileDescriptor;
    uint32                i;
    uint32                EntryCount = 0;
    uint32                FileSize = 0;
    int32                 Result;
    CFE_ES_AppInfo_t      AppInfo;
    const CFE_ES_FileNameCmd_Payload_t *CmdPtr = &data->Payload;
    char                  QueryAllFilename[OS_MAX_PATH_LEN];
    CFE_ES_ResourceID_t   ResourceList[CFE_ES_QUERY_ALL_MAX_ENTRIES];
    uint32                NumResources;
    CFE_ES_AppRecord_t    *AppRecPtr;
    CFE_ES_LibRecord_t    *LibRecPtr;

    /*
    ** Copy the commanded filename into local buffer to ensure size limitation and to allow for modification
    */
    CFE_SB_MessageStringGet(QueryAllFilename, (char *)CmdPtr->FileName, CFE_PLATFORM_ES_DEFAULT_APP_LOG_FILE,
            sizeof(QueryAllFilename), sizeof(CmdPtr->FileName));

    /*
     * Collect list of active resource IDs.
     *
     * This should be done while locked, but the actual writing
     * of the AppInfo data should be done while NOT locked.
     */
    CFE_ES_LockSharedData(__func__, __LINE__);
    NumResources = 0;
    AppRecPtr = CFE_ES_Global.AppTable;
    for(i=0; i < CFE_PLATFORM_ES_MAX_APPLICATIONS &&
             NumResources < CFE_ES_QUERY_ALL_MAX_ENTRIES; ++i)
    {
        if (CFE_ES_AppRecordIsUsed(AppRecPtr))
        {
            ResourceList[NumResources] = CFE_ES_AppRecordGetID(AppRecPtr);
            ++NumResources;
        }
        ++AppRecPtr;
    }
    LibRecPtr = CFE_ES_Global.LibTable;
    for(i=0; i < CFE_PLATFORM_ES_MAX_LIBRARIES &&
             NumResources < CFE_ES_QUERY_ALL_MAX_ENTRIES; ++i)
    {
        if (CFE_ES_LibRecordIsUsed(LibRecPtr))
        {
            ResourceList[NumResources] = CFE_ES_LibRecordGetID(LibRecPtr);
            ++NumResources;
        }
        ++LibRecPtr;
    }
    CFE_ES_UnlockSharedData(__func__, __LINE__);

    /*
    ** Check to see if the file already exists
    */
    Result = OS_OpenCreate(&FileDescriptor, QueryAllFilename,
            OS_FILE_FLAG_NONE, OS_READ_ONLY);
    if (Result >= 0)
    {
        OS_close(FileDescriptor);
        OS_remove(QueryAllFilename);
    }

    /*
    ** Create ES task log data file
    */
    Result = OS_OpenCreate(&FileDescriptor, QueryAllFilename,
            OS_FILE_FLAG_CREATE | OS_FILE_FLAG_TRUNCATE, OS_WRITE_ONLY);
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
            CFE_ES_TaskData.CommandErrorCounter++;
            CFE_EVS_SendEvent(CFE_ES_WRHDR_ERR_EID, CFE_EVS_EventType_ERROR,
                    "Failed to write App Info file, WriteHdr RC = 0x%08X, exp %d",
                    (unsigned int)Result,(int)sizeof(CFE_FS_Header_t));
            /*
             * returning "success" here as there is no other recourse;
             * the full extent of the error recovery has been done
             */
            return CFE_SUCCESS;
        }/* end if */

        /*
        ** Maintain statistics of amount of data written to file
        */
        FileSize += Result;

        /*
        ** Loop through the ES AppTable for main applications
        */
        for(i=0; i < NumResources; ++i)
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
                Result = OS_write(FileDescriptor, &AppInfo, sizeof(CFE_ES_AppInfo_t));
                if (Result !=  sizeof(CFE_ES_AppInfo_t))
                {
                    OS_close(FileDescriptor);
                    CFE_ES_TaskData.CommandErrorCounter++;
                    CFE_EVS_SendEvent(CFE_ES_TASKWR_ERR_EID, CFE_EVS_EventType_ERROR,
                            "Failed to write App Info file, Task write RC = 0x%08X, exp %d",
                            (unsigned int)Result,(int)sizeof(CFE_ES_AppInfo_t));
                    /*
                     * returning "success" here as there is no other recourse;
                     * the full extent of the error recovery has been done
                     */
                    return CFE_SUCCESS;
                }/* end if */

                FileSize += Result;
                EntryCount ++;
            }

            ++AppRecPtr;
        } /* end for */

        OS_close(FileDescriptor);
        CFE_ES_TaskData.CommandCounter++;
        CFE_EVS_SendEvent(CFE_ES_ALL_APPS_EID, CFE_EVS_EventType_DEBUG,
                "App Info file written to %s, Entries=%d, FileSize=%d",
                QueryAllFilename,(int)EntryCount,(int)FileSize);
    }
    else
    {
        CFE_ES_TaskData.CommandErrorCounter++;
        CFE_EVS_SendEvent(CFE_ES_OSCREATE_ERR_EID, CFE_EVS_EventType_ERROR,
                "Failed to write App Info file, OS_OpenCreate RC = 0x%08X",(unsigned int)Result);
    }

    return CFE_SUCCESS;
} /* End of CFE_ES_QueryAllCmd() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_QueryAllTasksCmd() -- Write all Task Data to a file      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 CFE_ES_QueryAllTasksCmd(const CFE_ES_QueryAllTasks_t *data)
{
    CFE_FS_Header_t            FileHeader;
    osal_id_t                  FileDescriptor;
    uint32                     i;
    uint32                     EntryCount = 0;
    uint32                     FileSize = 0;
    int32                      Result;
    CFE_ES_TaskInfo_t          TaskInfo;
    const CFE_ES_FileNameCmd_Payload_t *CmdPtr = &data->Payload;
    char                       QueryAllFilename[OS_MAX_PATH_LEN];
    CFE_ES_ResourceID_t        TaskList[OS_MAX_TASKS];
    uint32                     NumTasks;
    CFE_ES_TaskRecord_t        *TaskRecPtr;

    /*
    ** Copy the commanded filename into local buffer to ensure size limitation and to allow for modification
    */
    CFE_SB_MessageStringGet(QueryAllFilename, (char *)CmdPtr->FileName, CFE_PLATFORM_ES_DEFAULT_TASK_LOG_FILE,
            sizeof(QueryAllFilename), sizeof(CmdPtr->FileName));

    /*
     * Collect list of active task IDs.
     *
     * This should be done while locked, but the actual writing
     * of the AppInfo data should be done while NOT locked.
     */
    CFE_ES_LockSharedData(__func__, __LINE__);
    NumTasks = 0;
    TaskRecPtr = CFE_ES_Global.TaskTable;
    for(i=0; i < OS_MAX_TASKS; ++i)
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
    ** Check to see if the file already exists
    */
    Result = OS_OpenCreate(&FileDescriptor, QueryAllFilename,
            OS_FILE_FLAG_NONE, OS_READ_ONLY);
    if (Result >= 0)
    {
        OS_close(FileDescriptor);
        OS_remove(QueryAllFilename);
    }

    /*
    ** Create ES task log data file
    */
    Result = OS_OpenCreate(&FileDescriptor, QueryAllFilename,
            OS_FILE_FLAG_CREATE | OS_FILE_FLAG_TRUNCATE, OS_WRITE_ONLY);
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
            CFE_ES_TaskData.CommandErrorCounter++;
            CFE_EVS_SendEvent(CFE_ES_TASKINFO_WRHDR_ERR_EID, CFE_EVS_EventType_ERROR,
                    "Failed to write Task Info file, WriteHdr RC = 0x%08X, exp %d",
                    (unsigned int)Result,(int)sizeof(CFE_FS_Header_t));
            /*
             * returning "success" here as there is no other recourse;
             * the full extent of the error recovery has been done
             */
            return CFE_SUCCESS;
        }/* end if */

        /*
        ** Maintain statistics of amount of data written to file
        */
        FileSize += Result;

        /*
        ** Loop through the ES AppTable for main applications
        */
        for(i=0; i < NumTasks; ++i)
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
                Result = OS_write(FileDescriptor, &TaskInfo, sizeof(CFE_ES_TaskInfo_t));
                if (Result !=  sizeof(CFE_ES_TaskInfo_t))
                {
                    OS_close(FileDescriptor);
                    CFE_ES_TaskData.CommandErrorCounter++;
                    CFE_EVS_SendEvent(CFE_ES_TASKINFO_WR_ERR_EID, CFE_EVS_EventType_ERROR,
                            "Failed to write Task Info file, Task write RC = 0x%08X, exp %d",
                            (unsigned int)Result,(int)sizeof(CFE_ES_TaskInfo_t));
                    /*
                     * returning "success" here as there is no other recourse;
                     * the full extent of the error recovery has been done
                     */
                    return CFE_SUCCESS;
                }/* end if */

                FileSize += Result;
                EntryCount ++;
            }

        } /* end for */

        OS_close(FileDescriptor);
        CFE_ES_TaskData.CommandCounter++;
        CFE_EVS_SendEvent(CFE_ES_TASKINFO_EID, CFE_EVS_EventType_DEBUG,
                "Task Info file written to %s, Entries=%d, FileSize=%d",
                QueryAllFilename,(int)EntryCount,(int)FileSize);
    }
    else
    {
        CFE_ES_TaskData.CommandErrorCounter++;
        CFE_EVS_SendEvent(CFE_ES_TASKINFO_OSCREATE_ERR_EID, CFE_EVS_EventType_ERROR,
                "Failed to write Task Info file, OS_OpenCreate RC = 0x%08X",(unsigned int)Result);
    }

    return CFE_SUCCESS;
} /* End of CFE_ES_QueryAllTasksCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_ClearSyslogCmd() -- Clear executive services system log  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 CFE_ES_ClearSyslogCmd(const CFE_ES_ClearSyslog_t *data)
{
    /*
    ** Clear syslog index and memory area
    */

    CFE_ES_LockSharedData(__func__,__LINE__);
    CFE_ES_SysLogClear_Unsync();
    CFE_ES_UnlockSharedData(__func__,__LINE__);

    /*
    ** This command will always succeed...
    */
    CFE_ES_TaskData.CommandCounter++;
    CFE_EVS_SendEvent(CFE_ES_SYSLOG1_INF_EID, CFE_EVS_EventType_INFORMATION,
            "Cleared Executive Services log data");

    return CFE_SUCCESS;
} /* End of CFE_ES_ClearSyslogCmd() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_OverWriteSyslogCmd() -- set syslog mode                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 CFE_ES_OverWriteSyslogCmd(const CFE_ES_OverWriteSyslog_t *data)
{
    int32 Status;
    const CFE_ES_OverWriteSysLogCmd_Payload_t *CmdPtr = &data->Payload;

    Status = CFE_ES_SysLogSetMode(CmdPtr->Mode);

    if (Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CFE_ES_ERR_SYSLOGMODE_EID,
                CFE_EVS_EventType_ERROR,
                "Set OverWriteSysLog Command: Invalid Mode setting = %d", (int)CmdPtr->Mode);

        CFE_ES_TaskData.CommandErrorCounter++;
    }
    else
    {
        CFE_EVS_SendEvent(CFE_ES_SYSLOGMODE_EID,
                CFE_EVS_EventType_DEBUG,
                "Set OverWriteSysLog Command Received with Mode setting = %d", (int)CmdPtr->Mode);

        CFE_ES_TaskData.CommandCounter++;
    }

    return CFE_SUCCESS;
} /* End CFE_ES_OverWriteSyslogCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                       */
/* CFE_ES_WriteSyslogCmd() -- Process Cmd to write ES System Log to file */
/*                                                                       */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 CFE_ES_WriteSyslogCmd(const CFE_ES_WriteSyslog_t *data)
{
    const CFE_ES_FileNameCmd_Payload_t *CmdPtr = &data->Payload;
    int32                     Stat;
    char                      LogFilename[OS_MAX_PATH_LEN];

    CFE_SB_MessageStringGet(LogFilename, (char *)CmdPtr->FileName, CFE_PLATFORM_ES_DEFAULT_SYSLOG_FILE,
            sizeof(LogFilename), sizeof(CmdPtr->FileName));

    Stat = CFE_ES_SysLogDump(LogFilename);

    if(Stat == CFE_SUCCESS)
    {
        CFE_ES_TaskData.CommandCounter++;
    }
    else
    {
        CFE_ES_TaskData.CommandErrorCounter++;
    }/* end if */

    return CFE_SUCCESS;
}/* end CFE_ES_WriteSyslogCmd */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_ClearERLogCmd() -- Clear The exception and reset log.    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 CFE_ES_ClearERLogCmd(const CFE_ES_ClearERLog_t *data)
{
    /*
    ** Clear ER log data buffer
    */

    memset(CFE_ES_ResetDataPtr->ERLog, 0, sizeof(CFE_ES_ResetDataPtr->ERLog));

    /*
    ** Reset ER log buffer index
    */

    CFE_ES_ResetDataPtr->ERLogIndex = 0;

    /*
    ** Set Number of Entries in ER log buffer back to zero
    */
    CFE_ES_ResetDataPtr->ERLogEntries = 0;

    /*
    ** This command will always succeed
    */
    CFE_ES_TaskData.CommandCounter++;
    CFE_EVS_SendEvent(CFE_ES_ERLOG1_INF_EID, CFE_EVS_EventType_INFORMATION,
            "Cleared ES Exception and Reset Log data");

    return CFE_SUCCESS;
} /* End of CFE_ES_ClearERLogCmd() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_WriteERLogCmd() -- Process Cmd to write exception & reset*/
/*                           log to a file.                        */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 CFE_ES_WriteERLogCmd(const CFE_ES_WriteERLog_t *data)
{
    const CFE_ES_FileNameCmd_Payload_t *CmdPtr = &data->Payload;

    if (CFE_ES_TaskData.BackgroundERLogDumpState.IsPending)
    {
        CFE_EVS_SendEvent(CFE_ES_ERLOG_PENDING_ERR_EID,CFE_EVS_EventType_ERROR,
                "Error log write to file %s already in progress",
                CFE_ES_TaskData.BackgroundERLogDumpState.DataFileName);

        /* background dump already running, consider this an error */
        CFE_ES_TaskData.CommandErrorCounter++;
    }
    else
    {
        CFE_SB_MessageStringGet(CFE_ES_TaskData.BackgroundERLogDumpState.DataFileName, (char *)CmdPtr->FileName,
                CFE_PLATFORM_ES_DEFAULT_ER_LOG_FILE,
                sizeof(CFE_ES_TaskData.BackgroundERLogDumpState.DataFileName), sizeof(CmdPtr->FileName));

        CFE_ES_TaskData.BackgroundERLogDumpState.IsPending = true;
        CFE_ES_TaskData.CommandCounter++;
        CFE_ES_BackgroundWakeup();
    }

    return CFE_SUCCESS;
}/* end CFE_ES_WriteERLogCmd */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_VerifyCmdLength() -- Verify command packet length        */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

bool CFE_ES_VerifyCmdLength(CFE_MSG_Message_t *MsgPtr, size_t ExpectedLength)
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
                          (unsigned int)CFE_SB_MsgIdToValue(MsgId), (unsigned int)FcnCode,
                          (unsigned int)ActualLength, (unsigned int)ExpectedLength);
        result = false;
        CFE_ES_TaskData.CommandErrorCounter++;
    }

    return(result);

} /* End of CFE_ES_VerifyCmdLength() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_ResetPRCountCmd() -- ES task ground command              */
/*                              (Processor Reset Count)            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 CFE_ES_ResetPRCountCmd(const CFE_ES_ResetPRCount_t *data)
{
    /*
    ** Reset the processor reset count
    */
    CFE_ES_ResetDataPtr->ResetVars.ProcessorResetCount = 0;

    /*
    ** This command will always succeed.
    */
    CFE_EVS_SendEvent(CFE_ES_RESET_PR_COUNT_EID, CFE_EVS_EventType_INFORMATION,
            "Set Processor Reset Count to Zero");

    CFE_ES_TaskData.CommandCounter++;

    return CFE_SUCCESS;
} /* End of CFE_ES_ResetPRCountCmd() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_SetMaxPRCountCmd() -- Set Maximum Processor reset count  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 CFE_ES_SetMaxPRCountCmd(const CFE_ES_SetMaxPRCount_t *data)
{
    const CFE_ES_SetMaxPRCountCmd_Payload_t *cmd = &data->Payload;

    /*
    ** Set the MAX Processor reset count
    */
    CFE_ES_ResetDataPtr->ResetVars.MaxProcessorResetCount = cmd->MaxPRCount;

    /*
    ** This command will always succeed.
    */
    CFE_EVS_SendEvent(CFE_ES_SET_MAX_PR_COUNT_EID, CFE_EVS_EventType_INFORMATION,
            "Maximum Processor Reset Count set to: %d", (int)cmd->MaxPRCount);

    CFE_ES_TaskData.CommandCounter++;

    return CFE_SUCCESS;
} /* End of CFE_ES_RestartCmd() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_DeleteCDSCmd() -- Delete Specified Critical Data Store   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 CFE_ES_DeleteCDSCmd(const CFE_ES_DeleteCDS_t *data)
{
    int32   Status;
    const CFE_ES_DeleteCDSCmd_Payload_t *cmd = &data->Payload;
    char LocalCdsName[CFE_MISSION_ES_CDS_MAX_FULL_NAME_LEN];

    CFE_SB_MessageStringGet(LocalCdsName, (char *)cmd->CdsName, NULL,
            sizeof(LocalCdsName), sizeof(cmd->CdsName));

    Status = CFE_ES_DeleteCDS(LocalCdsName, false);

    if (Status == CFE_ES_CDS_WRONG_TYPE_ERR)
    {
        CFE_EVS_SendEvent(CFE_ES_CDS_DELETE_TBL_ERR_EID, CFE_EVS_EventType_ERROR,
                "CDS '%s' is a Critical Table CDS. Must be deleted via TBL Command",
                LocalCdsName);

        CFE_ES_TaskData.CommandErrorCounter++;
    }
    else if (Status == CFE_ES_CDS_OWNER_ACTIVE_ERR)
    {
        CFE_EVS_SendEvent(CFE_ES_CDS_OWNER_ACTIVE_EID, CFE_EVS_EventType_ERROR,
                "CDS '%s' not deleted because owning app is active", LocalCdsName);

        CFE_ES_TaskData.CommandErrorCounter++;
    }
    else if (Status == CFE_ES_ERR_NAME_NOT_FOUND)
    {
        CFE_EVS_SendEvent(CFE_ES_CDS_NAME_ERR_EID, CFE_EVS_EventType_ERROR,
                "Unable to locate '%s' in CDS Registry", LocalCdsName);

        CFE_ES_TaskData.CommandErrorCounter++;
    }
    else if (Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CFE_ES_CDS_DELETE_ERR_EID, CFE_EVS_EventType_ERROR,
                "Error while deleting '%s' from CDS, See SysLog.(Err=0x%08X)",
                LocalCdsName, (unsigned int)Status);

        CFE_ES_TaskData.CommandErrorCounter++;
    }
    else
    {
        CFE_EVS_SendEvent(CFE_ES_CDS_DELETED_INFO_EID, CFE_EVS_EventType_INFORMATION,
                "Successfully removed '%s' from CDS", LocalCdsName);

        CFE_ES_TaskData.CommandCounter++;
    }

    return CFE_SUCCESS;
} /* End of CFE_ES_DeleteCDSCmd() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                   */
/* CFE_ES_SendMemPoolStatsCmd() -- Telemeter Memory Pool Statistics  */
/*                                                                   */
/* Note: The "Application" parameter of the                          */
/*       CFE_ES_TlmPoolStats_t structure is not used.                */
/*                                                                   */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 CFE_ES_SendMemPoolStatsCmd(const CFE_ES_SendMemPoolStats_t *data)
{
    const CFE_ES_SendMemPoolStatsCmd_Payload_t *Cmd;
    CFE_ES_MemHandle_t        MemHandle;
    bool                      ValidHandle;

    Cmd = &data->Payload;

    /* Verify the handle to make sure it is legit */
    MemHandle = Cmd->PoolHandle;
    ValidHandle = CFE_ES_ValidateHandle(MemHandle);

    if (ValidHandle)
    {
        /* Extract the memory statistics from the memory pool */
        CFE_ES_GetMemPoolStats(&CFE_ES_TaskData.MemStatsPacket.Payload.PoolStats, MemHandle);

        /* Echo the specified pool handle in the telemetry packet */
        CFE_ES_TaskData.MemStatsPacket.Payload.PoolHandle = MemHandle;

        /*
        ** Send memory statistics telemetry packet.
        */
        CFE_SB_TimeStampMsg((CFE_MSG_Message_t *) &CFE_ES_TaskData.MemStatsPacket);
        CFE_SB_SendMsg((CFE_MSG_Message_t *) &CFE_ES_TaskData.MemStatsPacket);

        CFE_ES_TaskData.CommandCounter++;
        CFE_EVS_SendEvent(CFE_ES_TLM_POOL_STATS_INFO_EID, CFE_EVS_EventType_DEBUG,
                "Successfully telemetered memory pool stats for 0x%08lX",
                CFE_ES_ResourceID_ToInteger(Cmd->PoolHandle));
    }
    else
    {
        CFE_ES_TaskData.CommandErrorCounter++;
        CFE_EVS_SendEvent(CFE_ES_INVALID_POOL_HANDLE_ERR_EID, CFE_EVS_EventType_ERROR,
                "Cannot telemeter memory pool stats. Illegal Handle (0x%08lX)",
                CFE_ES_ResourceID_ToInteger(Cmd->PoolHandle));
    }

    return CFE_SUCCESS;
} /* End of CFE_ES_SendMemPoolStatsCmd() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_DumpCDSRegistryCmd() -- Dump CDS Registry to a file           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 CFE_ES_DumpCDSRegistryCmd(const CFE_ES_DumpCDSRegistry_t *data)
{
    CFE_FS_Header_t               StdFileHeader;
    osal_id_t                     FileDescriptor;
    int32                         Status;
    int16                         RegIndex=0;
    const CFE_ES_DumpCDSRegistryCmd_Payload_t *CmdPtr = &data->Payload;
    char                          DumpFilename[OS_MAX_PATH_LEN];
    CFE_ES_CDS_RegRec_t          *RegRecPtr;
    CFE_ES_CDSRegDumpRec_t        DumpRecord;
    int32                         FileSize=0;
    int32                         NumEntries=0;

    /* Copy the commanded filename into local buffer to ensure size limitation and to allow for modification */
    CFE_SB_MessageStringGet(DumpFilename, CmdPtr->DumpFilename, CFE_PLATFORM_ES_DEFAULT_CDS_REG_DUMP_FILE,
            sizeof(DumpFilename), sizeof(CmdPtr->DumpFilename));

    /* Create a new dump file, overwriting anything that may have existed previously */
    Status = OS_OpenCreate(&FileDescriptor, DumpFilename,
            OS_FILE_FLAG_CREATE | OS_FILE_FLAG_TRUNCATE, OS_WRITE_ONLY);

    if (Status >= OS_SUCCESS)
    {
        /* Initialize the standard cFE File Header for the Dump File */
        CFE_FS_InitHeader(&StdFileHeader, "CDS_Registry", CFE_FS_SubType_ES_CDS_REG);

        /* Output the Standard cFE File Header to the Dump File */
        Status = CFE_FS_WriteHeader(FileDescriptor, &StdFileHeader);

        /* Maintain statistics of amount of data written to file */
        FileSize += Status;

        if (Status == sizeof(CFE_FS_Header_t))
        {
            Status = sizeof(CFE_ES_CDSRegDumpRec_t);
            RegRecPtr = CFE_ES_Global.CDSVars.Registry;
            while ((RegIndex < CFE_PLATFORM_ES_CDS_MAX_NUM_ENTRIES) && (Status == sizeof(CFE_ES_CDSRegDumpRec_t)))
            {
                /* Check to see if the Registry entry is empty */
                if ( CFE_ES_CDSBlockRecordIsUsed(RegRecPtr) )
                {
                    /* Fill CDS Registry Dump Record with relevant information */
                    memset(&DumpRecord, 0, sizeof(DumpRecord));
                    DumpRecord.Size             = CFE_ES_MEMOFFSET_C(CFE_ES_CDSBlockRecordGetUserSize(RegRecPtr));
                    DumpRecord.Handle           = CFE_ES_CDSBlockRecordGetID(RegRecPtr);
                    DumpRecord.Table            = RegRecPtr->Table;
                    strncpy(DumpRecord.Name, RegRecPtr->Name, sizeof(DumpRecord.Name)-1);

                    /* Output Registry Dump Record to Registry Dump File */
                    Status = OS_write(FileDescriptor,
                                      &DumpRecord,
                                      sizeof(CFE_ES_CDSRegDumpRec_t));

                    FileSize += Status;
                    NumEntries++;
                }

                /* Look at the next entry in the Registry */
                ++RegIndex;
                ++RegRecPtr;
            }

            if (Status == sizeof(CFE_ES_CDSRegDumpRec_t))
            {
                CFE_EVS_SendEvent(CFE_ES_CDS_REG_DUMP_INF_EID,
                        CFE_EVS_EventType_DEBUG,
                        "Successfully dumped CDS Registry to '%s':Size=%d,Entries=%d",
                        DumpFilename, (int)FileSize, (int)NumEntries);

                /* Increment Successful Command Counter */
                CFE_ES_TaskData.CommandCounter++;
            }
            else
            {
                CFE_EVS_SendEvent(CFE_ES_CDS_DUMP_ERR_EID,
                        CFE_EVS_EventType_ERROR,
                        "Error writing CDS Registry to '%s', Status=0x%08X",
                        DumpFilename, (unsigned int)Status);

                /* Increment Command Error Counter */
                CFE_ES_TaskData.CommandErrorCounter++;
            }
        }
        else
        {
            CFE_EVS_SendEvent(CFE_ES_WRITE_CFE_HDR_ERR_EID,
                    CFE_EVS_EventType_ERROR,
                    "Error writing cFE File Header to '%s', Status=0x%08X",
                    DumpFilename, (unsigned int)Status);

            /* Increment Command Error Counter */
            CFE_ES_TaskData.CommandErrorCounter++;
        }

        /* We are done outputting data to the dump file.  Close it. */
        OS_close(FileDescriptor);
    }
    else
    {
        CFE_EVS_SendEvent(CFE_ES_CREATING_CDS_DUMP_ERR_EID,
                CFE_EVS_EventType_ERROR,
                "Error creating CDS dump file '%s', Status=0x%08X",
                DumpFilename, (unsigned int)Status);

        /* Increment Command Error Counter */
        CFE_ES_TaskData.CommandErrorCounter++;
    }

    return CFE_SUCCESS;
} /* End of CFE_ES_DumpCDSRegistryCmd() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_FileWriteByteCntErr() -- Send event to inform ground that*/
/*                                a byte count discrepancy has been*/
/*                                detected during the file write   */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CFE_ES_FileWriteByteCntErr(const char *Filename,size_t Requested,size_t Actual)
{

    CFE_EVS_SendEvent(CFE_ES_FILEWRITE_ERR_EID,CFE_EVS_EventType_ERROR,
                      "File write,byte cnt err,file %s,request=%u,actual=%u",
                       Filename,(unsigned int)Requested,(unsigned int)Actual);


}/* End of CFE_ES_FileWriteByteCntErr() */

/************************/
/*  End of File Comment */
/************************/


