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
**  File: cfe_evs_log.c
**
**  Title: Event Services API - Log Control Interfaces
**
**  Purpose: This module defines the top level functions of the
**           Event Services Log control interfaces
**
*/

/* Include Files */
#include "cfe_evs_module_all.h" /* All EVS internal definitions and API */

#include <string.h>

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void EVS_AddLog(CFE_EVS_LongEventTlm_t *EVS_PktPtr)
{
    /* Serialize access to event log control variables */
    OS_MutSemTake(CFE_EVS_Global.EVS_SharedDataMutexID);

    if (CFE_EVS_Global.EVS_LogPtr->LogFullFlag == true)
    {
        CFE_EVS_Global.EVS_LogPtr->LogOverflowCounter++;
    }

    /* If the log is not full, _or_ if it is in OVERWRITE mode, add the event to the log */
    if ((CFE_EVS_Global.EVS_LogPtr->LogFullFlag == false) ||
        (CFE_EVS_Global.EVS_LogPtr->LogMode == CFE_EVS_LogMode_OVERWRITE))
    {
        /* Copy the event data to the next available entry in the log */
        memcpy(&CFE_EVS_Global.EVS_LogPtr->LogEntry[CFE_EVS_Global.EVS_LogPtr->Next], EVS_PktPtr, sizeof(*EVS_PktPtr));

        CFE_EVS_Global.EVS_LogPtr->Next++;

        if (CFE_EVS_Global.EVS_LogPtr->Next >= CFE_PLATFORM_EVS_LOG_MAX)
        {
            /* This is important, even if we are in discard mode */
            CFE_EVS_Global.EVS_LogPtr->Next = 0;
        }

        /* Log count cannot exceed the number of entries in the log */
        if (CFE_EVS_Global.EVS_LogPtr->LogCount < CFE_PLATFORM_EVS_LOG_MAX)
        {
            CFE_EVS_Global.EVS_LogPtr->LogCount++;

            if (CFE_EVS_Global.EVS_LogPtr->LogCount == CFE_PLATFORM_EVS_LOG_MAX)
            {
                /* The full flag and log count are somewhat redundant */
                CFE_EVS_Global.EVS_LogPtr->LogFullFlag = true;
            }
        }
    }

    OS_MutSemGive(CFE_EVS_Global.EVS_SharedDataMutexID);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void EVS_ClearLog(void)
{
    /* Serialize access to event log control variables */
    OS_MutSemTake(CFE_EVS_Global.EVS_SharedDataMutexID);

    /* Clears everything but LogMode (overwrite vs discard) */
    CFE_EVS_Global.EVS_LogPtr->Next               = 0;
    CFE_EVS_Global.EVS_LogPtr->LogCount           = 0;
    CFE_EVS_Global.EVS_LogPtr->LogFullFlag        = false;
    CFE_EVS_Global.EVS_LogPtr->LogOverflowCounter = 0;

    memset(CFE_EVS_Global.EVS_LogPtr->LogEntry, 0, sizeof(CFE_EVS_Global.EVS_LogPtr->LogEntry));

    OS_MutSemGive(CFE_EVS_Global.EVS_SharedDataMutexID);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_EVS_WriteLogDataFileCmd(const CFE_EVS_WriteLogDataFileCmd_t *data)
{
    const CFE_EVS_LogFileCmd_Payload_t *CmdPtr = &data->Payload;
    int32                               Result;
    int32                               LogIndex;
    int32                               OsStatus;
    int32                               BytesWritten;
    osal_id_t                           LogFileHandle = OS_OBJECT_ID_UNDEFINED;
    uint32                              i;
    CFE_FS_Header_t                     LogFileHdr;
    char                                LogFilename[OS_MAX_PATH_LEN];

    /*
    ** Copy the filename into local buffer with default name/path/extension if not specified
    */
    Result = CFE_FS_ParseInputFileNameEx(LogFilename, CmdPtr->LogFilename, sizeof(LogFilename),
                                         sizeof(CmdPtr->LogFilename), CFE_PLATFORM_EVS_DEFAULT_LOG_FILE,
                                         CFE_FS_GetDefaultMountPoint(CFE_FS_FileCategory_BINARY_DATA_DUMP),
                                         CFE_FS_GetDefaultExtension(CFE_FS_FileCategory_BINARY_DATA_DUMP));

    if (Result != CFE_SUCCESS)
    {
        EVS_SendEvent(CFE_EVS_ERR_CRLOGFILE_EID, CFE_EVS_EventType_ERROR,
                      "Write Log File Command Error: CFE_FS_ParseInputFileNameEx() = 0x%08X", (unsigned int)Result);
    }
    else
    {
        /* Create the log file */
        OsStatus =
            OS_OpenCreate(&LogFileHandle, LogFilename, OS_FILE_FLAG_CREATE | OS_FILE_FLAG_TRUNCATE, OS_WRITE_ONLY);
        if (OsStatus != OS_SUCCESS)
        {
            EVS_SendEvent(CFE_EVS_ERR_CRLOGFILE_EID, CFE_EVS_EventType_ERROR,
                          "Write Log File Command Error: OS_OpenCreate = %ld, filename = %s", (long)OsStatus,
                          LogFilename);
            Result = CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
        }
    }

    if (Result == OS_SUCCESS)
    {
        /* Result will be overridden if everything works */
        Result = CFE_EVS_FILE_WRITE_ERROR;

        /* Initialize cFE file header for an event log file */
        CFE_FS_InitHeader(&LogFileHdr, "cFE EVS Log File", CFE_FS_SubType_EVS_EVENTLOG);

        /* Write the file header to the log file */
        BytesWritten = CFE_FS_WriteHeader(LogFileHandle, &LogFileHdr);

        if (BytesWritten == sizeof(LogFileHdr))
        {
            /* Serialize access to event log control variables */
            OS_MutSemTake(CFE_EVS_Global.EVS_SharedDataMutexID);

            /* Is the log full? -- Doesn't matter if wrap mode is enabled */
            if (CFE_EVS_Global.EVS_LogPtr->LogCount == CFE_PLATFORM_EVS_LOG_MAX)
            {
                /* Start with log entry that will be overwritten next (oldest) */
                LogIndex = CFE_EVS_Global.EVS_LogPtr->Next;
            }
            else
            {
                /* Start with the first entry in the log (oldest) */
                LogIndex = 0;
            }

            /* Write all the "in-use" event log entries to the file */
            for (i = 0; i < CFE_EVS_Global.EVS_LogPtr->LogCount; i++)
            {
                OsStatus = OS_write(LogFileHandle, &CFE_EVS_Global.EVS_LogPtr->LogEntry[LogIndex],
                                    sizeof(CFE_EVS_Global.EVS_LogPtr->LogEntry[LogIndex]));

                if (OsStatus == sizeof(CFE_EVS_Global.EVS_LogPtr->LogEntry[LogIndex]))
                {
                    LogIndex++;

                    if (LogIndex >= CFE_PLATFORM_EVS_LOG_MAX)
                    {
                        LogIndex = 0;
                    }
                }
                else
                {
                    break;
                }
            }

            OS_MutSemGive(CFE_EVS_Global.EVS_SharedDataMutexID);

            /* Process command handler success result */
            if (i == CFE_EVS_Global.EVS_LogPtr->LogCount)
            {
                EVS_SendEvent(CFE_EVS_WRLOG_EID, CFE_EVS_EventType_DEBUG,
                              "Write Log File Command: %d event log entries written to %s",
                              (int)CFE_EVS_Global.EVS_LogPtr->LogCount, LogFilename);
                Result = CFE_SUCCESS;
            }
            else
            {
                EVS_SendEvent(CFE_EVS_ERR_WRLOGFILE_EID, CFE_EVS_EventType_ERROR,
                              "Write Log File Command Error: OS_write = %ld, filename = %s", (long)OsStatus,
                              LogFilename);
            }
        }
        else
        {
            EVS_SendEvent(CFE_EVS_WRITE_HEADER_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Write File Header to Log File Error: WriteHdr RC: %d, Expected: %d, filename = %s",
                          (int)BytesWritten, (int)sizeof(LogFileHdr), LogFilename);
        }

        OS_close(LogFileHandle);
    }

    return Result;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_EVS_SetLogModeCmd(const CFE_EVS_SetLogModeCmd_t *data)
{
    const CFE_EVS_SetLogMode_Payload_t *CmdPtr = &data->Payload;
    int32                               Status;

    if ((CmdPtr->LogMode == CFE_EVS_LogMode_OVERWRITE) || (CmdPtr->LogMode == CFE_EVS_LogMode_DISCARD))
    {
        /* Serialize access to event log control variables */
        OS_MutSemTake(CFE_EVS_Global.EVS_SharedDataMutexID);
        CFE_EVS_Global.EVS_LogPtr->LogMode = CmdPtr->LogMode;
        OS_MutSemGive(CFE_EVS_Global.EVS_SharedDataMutexID);

        EVS_SendEvent(CFE_EVS_LOGMODE_EID, CFE_EVS_EventType_DEBUG, "Set Log Mode Command: Log Mode = %d",
                      (int)CmdPtr->LogMode);

        Status = CFE_SUCCESS;
    }
    else
    {
        Status = CFE_EVS_INVALID_PARAMETER;
        EVS_SendEvent(CFE_EVS_ERR_LOGMODE_EID, CFE_EVS_EventType_ERROR, "Set Log Mode Command Error: Log Mode = %d",
                      (int)CmdPtr->LogMode);
    }

    return Status;
}
