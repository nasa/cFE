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
**    cfe_ta_task.c
**
** Purpose:
**      This file contains the source code for the TA task.
**
** Authors:   V.Bigio, A. Rodriguez
**
*/

/* Include Files */

#include "cfe_ta_task.h"
#include "cfe_config.h" /* For version string construction */
#include "cfe_config.h"
#include "cfe_config_ids.h"
#include "cfe_ta_module_all.h" /* All TA internal definitions and API */
#include "cfe_version.h"       /* cFE version definitions */
#include "osapi.h"
#include <string.h>

/* Task Globals */
CFE_TA_Global_t CFE_TA_Global;

/* The live, modifiable working array for TA */
CFE_TA_CpuTaskAffinity_t CFE_TA_CpuTaskAffinity[OS_MAX_TASKS];

/* Function Definitions */

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_TA_EarlyInit(void)
{
    const CFE_TA_CpuTaskAffinity_t *TablePtr;
    uint32                          i;

    /* Clear task global and live affinity array */
    memset(&CFE_TA_Global, 0, sizeof(CFE_TA_Global));
    memset(CFE_TA_CpuTaskAffinity, 0, sizeof(CFE_TA_CpuTaskAffinity));

    /* Query the central config database (returns const void *) and cast it */
    TablePtr = (const CFE_TA_CpuTaskAffinity_t *)CFE_Config_GetObjPointer(CFE_CONFIGID_TA_AFFINITY_TABLE);

    /* If the target provided a table, copy it into live memory */
    if (TablePtr != NULL)
    {
        for (i = 0; i < OS_MAX_TASKS; i++)
        {
            /* Check for the empty string terminator */
            if (TablePtr[i].TaskName[0] == '\0')
            {
                break;
            }
            CFE_TA_CpuTaskAffinity[i] = TablePtr[i];
        }
    }

    /* Register TA function as the task event handler */
    OS_RegisterEventHandler(CFE_TA_OS_EventHandler);

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TA_TaskMain(void)
{
    int32            Status;
    CFE_SB_Buffer_t *SBBufPtr;

    CFE_ES_PerfLogEntry(CFE_MISSION_TA_MAIN_PERF_ID);

    Status = CFE_TA_TaskInit();

    if (Status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: Application Init Failed,RC=0x%08X\n", __func__, (unsigned int)Status);
        CFE_ES_PerfLogExit(CFE_MISSION_TA_MAIN_PERF_ID);
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

        CFE_ES_PerfLogExit(CFE_MISSION_TA_MAIN_PERF_ID);

        /* Pend on receipt of packet */
        Status = CFE_SB_ReceiveBuffer(&SBBufPtr, CFE_TA_Global.TA_CommandPipe, CFE_SB_PEND_FOREVER);

        CFE_ES_PerfLogEntry(CFE_MISSION_TA_MAIN_PERF_ID);

        if (Status == CFE_SUCCESS)
        {
            /* Process cmd pipe msg */
            CFE_TA_TaskPipe(SBBufPtr);
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
int32 CFE_TA_TaskInit(void)
{
    int32          Status;
    CFE_ES_AppId_t AppID;
    char           VersionString[CFE_CFG_MAX_VERSION_STR_LEN];

    /* Initialize housekeeping packet*/
    Status = CFE_MSG_Init(CFE_MSG_PTR(CFE_TA_Global.TA_TlmPkt.TelemetryHeader),
                          CFE_SB_ValueToMsgId(CFE_TA_HK_TLM_MID),
                          sizeof(CFE_TA_Global.TA_TlmPkt));

    if (Status != CFE_SUCCESS)
    {
        return Status;
    }

    CFE_TA_Global.CFE_TA_DiagInfo.CoresConfigured = OS_TaskAffinityCoresConfigured();
    CFE_TA_Global.CFE_TA_DiagInfo.TACoresMax      = OS_MAX_CPUS;

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
        CFE_ES_WriteToSysLog("%s: Call to CFE_TA_Register Failed:RC=0x%08X\n", __func__, (unsigned int)Status);
        return Status;
    }

    /* Create software bus command pipe */
    Status = CFE_SB_CreatePipe(&CFE_TA_Global.TA_CommandPipe, CFE_TA_PIPE_DEPTH, CFE_TA_PIPE_NAME);
    if (Status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: Call to CFE_SB_CreatePipe Failed:RC=0x%08X\n", __func__, (unsigned int)Status);
        return Status;
    }

    /* Subscribe to command and telemetry requests coming in on the command pipe
     */
    Status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(CFE_TA_CMD_MID), CFE_TA_Global.TA_CommandPipe);
    if (Status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: Subscribing to Cmds Failed:RC=0x%08X\n", __func__, (unsigned int)Status);
        return Status;
    }

    Status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(CFE_TA_SEND_HK_MID), CFE_TA_Global.TA_CommandPipe);
    if (Status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: Subscribing to HK Request Failed:RC=0x%08X\n", __func__, (unsigned int)Status);
        return Status;
    }

    /* Write the AppID to the global location, now that the rest of initialization
     * is done */
    CFE_TA_Global.TA_AppID = AppID;

    CFE_Config_GetVersionString(VersionString,
                                CFE_CFG_MAX_VERSION_STR_LEN,
                                "cFE",
                                CFE_SRC_VERSION,
                                CFE_BUILD_CODENAME,
                                CFE_LAST_OFFICIAL);

    CFE_EVS_SendEvent(CFE_TA_CORE_INF_EID,
                      CFE_EVS_EventType_INFORMATION,
                      "Configured cores = %u, TA support max limit = %u",
                      OS_TaskAffinityCoresConfigured(),
                      CFE_MISSION_TA_MAX_NUM_CPUS);

    CFE_EVS_SendEvent(CFE_TA_INIT_INF_EID, CFE_EVS_EventType_INFORMATION, "cFE TA Initialized: %s", VersionString);

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_TA_CleanUpApp(uint32 AppId)
{
    /* Placeholder */

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_TA_NoopCmd(const CFE_TA_NoopCmd_t *data)
{
    char VersionString[CFE_CFG_MAX_VERSION_STR_LEN];
    CFE_Config_GetVersionString(VersionString,
                                CFE_CFG_MAX_VERSION_STR_LEN,
                                "cFE",
                                CFE_SRC_VERSION,
                                CFE_BUILD_CODENAME,
                                CFE_LAST_OFFICIAL);

    CFE_EVS_SendEvent(CFE_TA_NOOP_INF_EID, CFE_EVS_EventType_INFORMATION, "No-op Cmd Rcvd: %s", VersionString);

    CFE_TA_Global.CommandCounter++;

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_TA_ReportHousekeepingCmd(const CFE_TA_SendHkCmd_t *data)
{
    CFE_TA_Global.TA_TlmPkt.Payload.CommandCounter      = CFE_TA_Global.CommandCounter;
    CFE_TA_Global.TA_TlmPkt.Payload.CommandErrorCounter = CFE_TA_Global.CommandErrorCounter;
    CFE_TA_Global.TA_TlmPkt.Payload.StartupCounter      = CFE_TA_Global.StartupCounter;
    CFE_TA_Global.TA_TlmPkt.Payload.StartupErrorCounter = CFE_TA_Global.StartupErrorCounter;

    CFE_SB_TimeStampMsg(CFE_MSG_PTR(CFE_TA_Global.TA_TlmPkt.TelemetryHeader));
    CFE_SB_TransmitMsg(CFE_MSG_PTR(CFE_TA_Global.TA_TlmPkt.TelemetryHeader), true);

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_TA_ResetCountersCmd(const CFE_TA_ResetCountersCmd_t *data)
{
    /* Clear commands processed by TA task */
    CFE_TA_Global.CommandCounter      = 0;
    CFE_TA_Global.CommandErrorCounter = 0;

    CFE_EVS_SendEvent(CFE_TA_RESET_COUNTERS_INF_EID, CFE_EVS_EventType_INFORMATION, "Reset Counters Command Received");

    /* NOTE: Historically the reset counters command does _NOT_ increment the
     * command counter */

    return CFE_STATUS_NO_COUNTER_INCREMENT;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_TA_QueryAllTasksCmd(const CFE_TA_QueryAllTasksCmd_t *data)
{
    CFE_FS_Header_t                          FileHeader;
    osal_id_t                                FileDescriptor = OS_OBJECT_ID_UNDEFINED;
    uint32                                   EntryCount     = OS_MAX_TASKS; /* Set to Max Tasks */
    uint32                                   FileSize       = 0;
    int32                                    OsStatus;
    int32                                    Result;
    const CFE_TA_QueryAllTasksCmd_Payload_t *CmdPtr = &data->Payload;
    char                                     QueryAllFilename[OS_MAX_PATH_LEN];

    /* FIX: Copy the ENTIRE task affinity list array */
    memset(CFE_TA_Global.CFE_TA_DiagInfo.CpuTaskAffinity, 0, sizeof(CFE_TA_Global.CFE_TA_DiagInfo.CpuTaskAffinity));

    memcpy(CFE_TA_Global.CFE_TA_DiagInfo.CpuTaskAffinity,
           CFE_TA_CpuTaskAffinity,
           sizeof(CFE_TA_Global.CFE_TA_DiagInfo.CpuTaskAffinity));

    /* Populate the Diag Packet Header just to be safe */
    CFE_TA_Global.CFE_TA_DiagInfo.CoresConfigured = OS_TaskAffinityCoresConfigured();
    CFE_TA_Global.CFE_TA_DiagInfo.TACoresMax      = CFE_MISSION_TA_MAX_NUM_CPUS;

    /*
    ** Copy the commanded filename into local buffer to ensure size limitation and
    *to allow for modification
    */
    Result = CFE_FS_ParseInputFileNameEx(QueryAllFilename,
                                         CmdPtr->FileName,
                                         sizeof(QueryAllFilename),
                                         sizeof(CmdPtr->FileName),
                                         CFE_PLATFORM_ES_DEFAULT_TASK_LOG_FILE,
                                         CFE_FS_GetDefaultMountPoint(CFE_FS_FileCategory_BINARY_DATA_DUMP),
                                         CFE_FS_GetDefaultExtension(CFE_FS_FileCategory_BINARY_DATA_DUMP));

    if (Result == CFE_SUCCESS)
    {
        /* Create (or truncate) TA task log data file */
        OsStatus = OS_OpenCreate(&FileDescriptor,
                                 QueryAllFilename,
                                 OS_FILE_FLAG_CREATE | OS_FILE_FLAG_TRUNCATE,
                                 OS_WRITE_ONLY);

        if (OsStatus != OS_SUCCESS)
        {
            CFE_EVS_SendEvent(CFE_TA_TASKINFO_OSCREATE_ERR_EID,
                              CFE_EVS_EventType_ERROR,
                              "Failed to write Task Info file, OS_OpenCreate RC = %ld",
                              (long)OsStatus);
            Result = CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
        }
    }
    else
    {
        CFE_EVS_SendEvent(CFE_TA_TASKINFO_FILENAME_ERR_EID,
                          CFE_EVS_EventType_ERROR,
                          "Failed to write Task Info file, CFE_FS_ParseInputFileNameEx RC = %08x",
                          (unsigned int)Result);
    }

    if (Result >= 0)
    {
        /* Initialize cFE file header */
        CFE_FS_InitHeader(&FileHeader, CFE_TA_TASK_LOG_DESC, CFE_FS_SubType_TA_QUERYALLTASKS);

        /* Output the Standard cFE File Header to the App File */
        Result = CFE_FS_WriteHeader(FileDescriptor, &FileHeader);

        if (Result != sizeof(CFE_FS_Header_t))
        {
            OS_close(FileDescriptor);
            CFE_TA_Global.CommandErrorCounter++;
            CFE_EVS_SendEvent(CFE_TA_TASKINFO_WRHDR_ERR_EID,
                              CFE_EVS_EventType_ERROR,
                              "Failed to write Task Info file, WriteHdr RC = 0x%08X",
                              (unsigned int)Result);
            return CFE_SUCCESS;
        }

        /* Maintain statistics of amount of data written to file */
        FileSize += sizeof(CFE_FS_Header_t);

        /* Write the entire Diag Packet to file */
        OsStatus = OS_write(FileDescriptor, &CFE_TA_Global.CFE_TA_DiagInfo, sizeof(CFE_TA_Global.CFE_TA_DiagInfo));

        if (OsStatus != sizeof(CFE_TA_Global.CFE_TA_DiagInfo))
        {
            OS_close(FileDescriptor);
            CFE_TA_Global.CommandErrorCounter++;
            CFE_EVS_SendEvent(CFE_TA_TASKINFO_WR_ERR_EID,
                              CFE_EVS_EventType_ERROR,
                              "Failed to write Task Info file, Task write RC = %ld",
                              (long)OsStatus);
            return CFE_SUCCESS;
        }

        /* Add the full size of the payload, not just one task */
        FileSize += sizeof(CFE_TA_Global.CFE_TA_DiagInfo);

        OS_close(FileDescriptor);

        /* Increment success counter and send event */
        CFE_TA_Global.CommandCounter++;

        CFE_EVS_SendEvent(CFE_TA_TASKINFO_EID,
                          CFE_EVS_EventType_INFORMATION,
                          "Task Info file written to %s, Entries=%d, FileSize=%d",
                          QueryAllFilename,
                          (int)EntryCount,
                          (int)FileSize);
    }
    else
    {
        CFE_TA_Global.CommandErrorCounter++;
    }

    return CFE_SUCCESS;
}

/* Convert from the ground payload array to the OSAL Affinity Mask */
void CFE_TA_Cpu_Set(const uint8 *AffinityMask, OS_cpuset_t *cpuset)
{
    OS_CPUSET_ZERO(cpuset);

    for (uint32 i = 0; (i < CFE_MISSION_TA_MAX_NUM_CPUS) && (i < OS_MAX_CPUS) && (i < OS_TaskAffinityCoresConfigured());
         i++)
    {
        if (AffinityMask[i / 8] & (1 << (i % 8)))
        {
            OS_CPUSET_SET(i, cpuset);
        }
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_TA_SetTaskAffinityCmd(const CFE_TA_SetTaskAffinityCmd_t *data)
{
    const CFE_TA_SetTaskAffinityCmd_Payload_t *CmdPtr = &data->Payload;
    int32                                      Status = CFE_SUCCESS;
    OS_cpuset_t                                cpuset, cpuget;
    osal_id_t                                  task_id         = OS_OBJECT_ID_UNDEFINED;
    /* Create 64-bit printable variables for the event message */
    uint64                                     PrintableMask   = 0;
    uint64                                     PrintableOsMask = 0;

    /* Look for the task in the task affinity list */
    for (uint32 i = 0; i < OS_MAX_TASKS; i++)
    {
        /* Reached the end of the entries, task not found */
        if (strlen(CFE_TA_CpuTaskAffinity[i].TaskName) == 0)
        {
            Status = CFE_TA_APP_TASK_NOT_FOUND;

            CFE_EVS_SendEvent(CFE_TA_SET_AFFINITY_CMD_NOTFOUND_ERR_EID,
                              CFE_EVS_EventType_ERROR,
                              "Set Task Affinity Failed, task (%s) not found",
                              CmdPtr->TaskName);

            CFE_TA_Global.CommandErrorCounter++;
            break;
        }
        else if (strcmp(CmdPtr->TaskName, CFE_TA_CpuTaskAffinity[i].TaskName) == 0)
        {
            /* Task found in the list */
            CFE_TA_CpuTaskAffinity[i].Status = CFE_TA_Status_Ok;

            /* Translate from user payload byte array to OSAL affinity struct */
            CFE_TA_Cpu_Set(CmdPtr->AffinityMask, &cpuset);

            /* For tasks IDs, defer to OSAL for name lookup */
            Status = OS_TaskGetIdByName(&task_id, CmdPtr->TaskName);

            if (Status == OS_SUCCESS)
            {
                Status = OS_TaskAffinitySetAffinity(task_id, cpuset);
            }

            if (Status == OS_SUCCESS)
            {
                /* Save the assigned cpuset natively */
                CFE_TA_CpuTaskAffinity[i].CpuMask = cpuset;

                /* Get the task affinity back from OSAL to verify */
                Status = OS_TaskAffinityGetAffinity(task_id, &cpuget);

                if (Status == OS_SUCCESS)
                {
                    /* Save the OSAL-verified struct directly into our table */
                    CFE_TA_CpuTaskAffinity[i].CpuFromOS = cpuget;
                    CFE_TA_CpuTaskAffinity[i].Type      = CFE_TA_Mask;

                    /* Pack the command byte array into the printable variable */
                    memcpy(&PrintableMask,
                           CmdPtr->AffinityMask,
                           (sizeof(CmdPtr->AffinityMask) <= 8) ? sizeof(CmdPtr->AffinityMask) : 8);

                    /* Copy the raw OSAL struct directly into the printable variable (up
                     * to 8 bytes) */
                    memcpy(&PrintableOsMask, &cpuget, (sizeof(OS_cpuset_t) <= 8) ? sizeof(OS_cpuset_t) : 8);

                    CFE_EVS_SendEvent(CFE_TA_SET_TASK_AFFINITY_CMD_INF_EID,
                                      CFE_EVS_EventType_INFORMATION,
                                      "Set Task Affinity for %s: Type = %d, mask = "
                                      "0x%016llX, from O/S = 0x%016llX",
                                      CFE_TA_CpuTaskAffinity[i].TaskName,
                                      CFE_TA_CpuTaskAffinity[i].Type,
                                      (unsigned long long)PrintableMask,
                                      (unsigned long long)PrintableOsMask);

                    CFE_TA_Global.CommandCounter++;
                }
                else
                {
                    /* Get Task Affinity Failed */
                    Status                           = CFE_TA_APP_GET_TASK_AFFINITY_FAILED;
                    CFE_TA_CpuTaskAffinity[i].Status = CFE_TA_Status_Error;

                    CFE_EVS_SendEvent(CFE_TA_SET_AFFINITY_CMD_GET_ERR_EID,
                                      CFE_EVS_EventType_ERROR,
                                      "Set Task Affinity Failed for task (%s), could not "
                                      "Get Task Affinity, Status = %d",
                                      CmdPtr->TaskName,
                                      Status);

                    CFE_TA_Global.CommandErrorCounter++;
                }
            }
            else
            {
                /* Set Task Affinity Failed */
                Status                           = CFE_TA_APP_SET_TASK_AFFINITY_FAILED;
                CFE_TA_CpuTaskAffinity[i].Status = CFE_TA_Status_Error;

                CFE_EVS_SendEvent(CFE_TA_SET_AFFINITY_CMD_SET_ERR_EID,
                                  CFE_EVS_EventType_ERROR,
                                  "Set Task Affinity Failed for task (%s), could not "
                                  "Set Task Affinity, Status = %d",
                                  CmdPtr->TaskName,
                                  Status);

                CFE_TA_Global.CommandErrorCounter++;
            }
            break;
        }
    }

    return Status;
}
/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_TA_GetTaskAffinityCmd(const CFE_TA_GetTaskAffinityCmd_t *data)
{
    const CFE_TA_GetTaskAffinityCmd_Payload_t *CmdPtr = &data->Payload;
    int32                                      Status = CFE_SUCCESS;
    OS_cpuset_t                                cpuset;
    osal_id_t                                  task_id         = OS_OBJECT_ID_UNDEFINED;
    /* Create 64-bit printable variable for the event message */
    uint64                                     PrintableOsMask = 0;

    /* Look for the task in the task affinity list */
    for (uint32 i = 0; i < OS_MAX_TASKS; i++)
    {
        if (strlen(CFE_TA_CpuTaskAffinity[i].TaskName) == 0)
        {
            Status = CFE_TA_APP_TASK_NOT_FOUND;

            /* Reached the end of the entries, task not found */
            CFE_EVS_SendEvent(CFE_TA_GET_AFFINITY_CMD_NOTFOUND_ERR_EID,
                              CFE_EVS_EventType_ERROR,
                              "Get Task Affinity Failed, task (%s) not found",
                              CmdPtr->TaskName);

            CFE_TA_Global.CommandErrorCounter++;
            break;
        }
        else if (strcmp(CmdPtr->TaskName, CFE_TA_CpuTaskAffinity[i].TaskName) == 0)
        {
            /* Task found in the list */
            CFE_TA_CpuTaskAffinity[i].Status = CFE_TA_Status_Ok;

            /* For tasks IDs, defer to OSAL for name lookup */
            Status = OS_TaskGetIdByName(&task_id, CmdPtr->TaskName);

            if (Status == OS_SUCCESS)
            {
                /* Get the task affinity */
                Status = OS_TaskAffinityGetAffinity(task_id, &cpuset);
            }

            if (Status == OS_SUCCESS)
            {
                /* Compare table task affinity with value just read using memcmp on
                 * OS_cpuset_t */
                if (memcmp(&cpuset, &CFE_TA_CpuTaskAffinity[i].CpuFromOS, sizeof(OS_cpuset_t)) != 0)
                {
                    CFE_EVS_SendEvent(CFE_TA_GET_TASK_AFFINITY_DISCREPANCY_INF_EID,
                                      CFE_EVS_EventType_INFORMATION,
                                      "Get Task Affinity: Affinity stored in TA was "
                                      "different than current O/S return");

                    /* Put latest affinity from OS into TA internal structure */
                    CFE_TA_CpuTaskAffinity[i].CpuFromOS = cpuset;
                }

                /* Copy the raw OSAL struct directly into the printable variable (up to
                 * 8 bytes) */
                memcpy(&PrintableOsMask, &cpuset, (sizeof(OS_cpuset_t) <= 8) ? sizeof(OS_cpuset_t) : 8);

                CFE_EVS_SendEvent(CFE_TA_GET_TASK_AFFINITY_CMD_INF_EID,
                                  CFE_EVS_EventType_INFORMATION,
                                  "Get Task Affinity for %s: Type = %d, O/S Mask = 0x%016llX",
                                  CFE_TA_CpuTaskAffinity[i].TaskName,
                                  CFE_TA_CpuTaskAffinity[i].Type,
                                  (unsigned long long)PrintableOsMask);

                CFE_TA_Global.CommandCounter++;
            }
            else
            {
                /* Get Task Affinity Failed */
                Status                           = CFE_TA_APP_GET_TASK_AFFINITY_FAILED;
                CFE_TA_CpuTaskAffinity[i].Status = CFE_TA_Status_Error;

                CFE_EVS_SendEvent(CFE_TA_GET_AFFINITY_CMD_GET_ERR_EID,
                                  CFE_EVS_EventType_ERROR,
                                  "Get Task Affinity Failed for task (%s), Status = %d",
                                  CmdPtr->TaskName,
                                  Status);

                CFE_TA_Global.CommandErrorCounter++;
            }

            break;
        }
    }

    return Status;
}
/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_TA_SetStartupAffinity(const char *taskname)
{
    int32       Status = CFE_SUCCESS;
    OS_cpuset_t cpuget;
    osal_id_t   task_id = OS_OBJECT_ID_UNDEFINED;

    /* For tasks IDs, defer to OSAL for name lookup */
    Status = OS_TaskGetIdByName(&task_id, taskname);

    if (Status == OS_SUCCESS)
    {
        for (uint32 i = 0; i < OS_MAX_TASKS; i++)
        {
            if (strlen(CFE_TA_CpuTaskAffinity[i].TaskName) == 0)
            {
                /* Couldn't find the task in the task affinity table */
                CFE_ES_WriteToSysLog("%s: Set Startup Affinity - task (%s) not found "
                                     "in TA list, adding\n",
                                     __func__,
                                     taskname);

                /* Add the task to the affinity table */
                strncpy(CFE_TA_CpuTaskAffinity[i].TaskName, taskname, CFE_MISSION_MAX_API_LEN);

                /* Safely zero out the newly added OS_cpuset_t structs */
                OS_CPUSET_ZERO(&CFE_TA_CpuTaskAffinity[i].CpuMask);
                OS_CPUSET_ZERO(&CFE_TA_CpuTaskAffinity[i].CpuFromOS);
                CFE_TA_CpuTaskAffinity[i].Type = CFE_TA_Added;

                /* Get the task affinity */
                Status = OS_TaskAffinityGetAffinity(task_id, &cpuget);

                if (Status == OS_SUCCESS)
                {
                    /* Save the OS_cpuset_t directly into our table */
                    CFE_TA_CpuTaskAffinity[i].CpuFromOS = cpuget;
                    CFE_TA_CpuTaskAffinity[i].Status    = CFE_TA_Status_Ok;
                }
                else
                {
                    Status                           = CFE_TA_APP_GET_TASK_AFFINITY_FAILED;
                    CFE_TA_CpuTaskAffinity[i].Status = CFE_TA_Status_Error;

                    CFE_ES_WriteToSysLog("%s: Set Startup Affinity for task (%s) could "
                                         "not Get Task Affinity, Status = %d\n",
                                         __func__,
                                         taskname,
                                         Status);
                }
                break;
            }
            else if (strcmp(taskname, CFE_TA_CpuTaskAffinity[i].TaskName) == 0)
            {
                /* Task found in the list */
                CFE_TA_CpuTaskAffinity[i].Status = CFE_TA_Status_Ok;

                if (CFE_TA_CpuTaskAffinity[i].Type == CFE_TA_OsDefault)
                {
                    /* Use the O/S default behavior */
                    Status = OS_TaskAffinityGetAffinity(task_id, &cpuget);

                    if (Status == OS_SUCCESS)
                    {
                        CFE_TA_CpuTaskAffinity[i].CpuFromOS = cpuget;
                    }
                    else
                    {
                        Status                           = CFE_TA_APP_GET_TASK_AFFINITY_FAILED;
                        CFE_TA_CpuTaskAffinity[i].Status = CFE_TA_Status_Error;

                        CFE_ES_WriteToSysLog("%s: Set Startup Affinity for task (%s) could "
                                             "not Get Task Affinity, Status = %d\n",
                                             __func__,
                                             taskname,
                                             Status);
                    }
                }
                else if (CFE_TA_CpuTaskAffinity[i].Type == CFE_TA_Mask)
                {
                    /* Use the User defined affinity mask (directly passing OS_cpuset_t!)
                     */
                    Status = OS_TaskAffinitySetAffinity(task_id, CFE_TA_CpuTaskAffinity[i].CpuMask);

                    if (Status == OS_SUCCESS)
                    {
                        Status = OS_TaskAffinityGetAffinity(task_id, &cpuget);

                        if (Status == OS_SUCCESS)
                        {
                            CFE_TA_CpuTaskAffinity[i].CpuFromOS = cpuget;
                        }
                        else
                        {
                            Status                           = CFE_TA_APP_GET_TASK_AFFINITY_FAILED;
                            CFE_TA_CpuTaskAffinity[i].Status = CFE_TA_Status_Error;

                            CFE_ES_WriteToSysLog("%s: Set Startup Affinity for task (%s) "
                                                 "could not Get Task Affinity, Status = %d\n",
                                                 __func__,
                                                 taskname,
                                                 Status);
                        }
                    }
                    else
                    {
                        Status                           = CFE_TA_APP_SET_TASK_AFFINITY_FAILED;
                        CFE_TA_CpuTaskAffinity[i].Status = CFE_TA_Status_Error;

                        CFE_ES_WriteToSysLog("%s: Set Startup Affinity for task (%s) could "
                                             "not Set Task Affinity, Status = %d\n",
                                             __func__,
                                             taskname,
                                             Status);
                    }
                }
                else
                {
                    Status                           = CFE_TA_APP_INVALID_TASK_AFFINITY_TYPE;
                    CFE_TA_CpuTaskAffinity[i].Status = CFE_TA_Status_Error;

                    CFE_ES_WriteToSysLog("%s: Invalid affinity type (%d) for task (%s)\n",
                                         __func__,
                                         CFE_TA_CpuTaskAffinity[i].Type,
                                         taskname);
                }
                break;
            }
        }
    }

    if (Status == CFE_SUCCESS)
    {
        CFE_TA_Global.StartupCounter++;
    }
    else
    {
        CFE_TA_Global.StartupErrorCounter++;
    }

    return Status;
}

/******************************************************************************
**
**  Purpose:
**    Task creation event handler.
**    Note this overrides previously registered event handler
**    Sets the kernel task name using a glibc-specific (non-posix) function.
**    Calls the TA Startup Affinity function
**
*/
int32 CFE_TA_OS_EventHandler(OS_Event_t event, osal_id_t object_id, void *data)
{
    char taskname[OS_MAX_API_NAME];

    memset(taskname, 0, sizeof(taskname));

    switch (event)
    {
        case OS_EVENT_RESOURCE_ALLOCATED:
            /* resource/id is newly allocated but not yet created.  Invoked within
             * locked region. */
            break;
        case OS_EVENT_RESOURCE_CREATED:
            /* resource/id has been fully created/finalized.  Invoked outside locked
             * region. */
            break;
        case OS_EVENT_RESOURCE_DELETED:
            /* resource/id has been deleted.  Invoked outside locked region. */
            break;
        case OS_EVENT_TASK_STARTUP:
        {
            /* New task is starting. Invoked from within the task context. */
            /* Get the name from OSAL and propagate to the pthread/system layer */
            if (OS_GetResourceName(object_id, taskname, sizeof(taskname)) == OS_SUCCESS)
            {
                /* Assign startup affinity via a TA app function */
                CFE_TA_SetStartupAffinity(taskname);
            }
            break;
        }

        default:
            break;
    }

    return OS_SUCCESS;
}
