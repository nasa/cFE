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
**  File: cfe_evs_log.c
**
**  Title: Event Services API - Log Control Interfaces
**
**  Purpose: This module defines the top level functions of the
**           Event Services Log control interfaces
**
*/

/* Include Files */
#include "cfe_evs_task.h"     /* EVS internal definitions */
#include "cfe_evs_log.h"      /* EVS log file definitions */
#include "cfe_evs.h"          /* EVS API definitions */
#include "cfe_evs_utils.h"    /* EVS utility function definitions */
#include "cfe_fs.h"           /* File Service definitions */
#include "cfe_error.h"        /* cFE error code definitions */
#include "cfe_psp.h"          /* Get reset area function prototype */


#include <string.h>


/*
**             Function Prologue
**
** Function Name:      EVS_AddLog
**
** Purpose:  This routine adds an event packet to the internal event log.
**
** Assumptions and Notes:
**
*/
void EVS_AddLog (CFE_EVS_LongEventTlm_t *EVS_PktPtr)
{

   if (CFE_EVS_GlobalData.EVS_TlmPkt.Payload.LogEnabled == true)
   {   
      /* Serialize access to event log control variables */
      OS_MutSemTake(CFE_EVS_GlobalData.EVS_SharedDataMutexID);

      if ((CFE_EVS_GlobalData.EVS_LogPtr->LogFullFlag == true) &&
          (CFE_EVS_GlobalData.EVS_LogPtr->LogMode == CFE_EVS_LogMode_DISCARD))
      {
         /* If log is full and in discard mode, just count the event */
         CFE_EVS_GlobalData.EVS_LogPtr->LogOverflowCounter++;
      }
      else
      {
         if (CFE_EVS_GlobalData.EVS_LogPtr->LogFullFlag == true)
         {
            /* If log is full and in wrap mode, count it and store it */
            CFE_EVS_GlobalData.EVS_LogPtr->LogOverflowCounter++;
         }

         /* Copy the event data to the next available entry in the log */
         memcpy(&CFE_EVS_GlobalData.EVS_LogPtr->LogEntry[CFE_EVS_GlobalData.EVS_LogPtr->Next],
                        EVS_PktPtr, sizeof(*EVS_PktPtr));

         CFE_EVS_GlobalData.EVS_LogPtr->Next++;

         if (CFE_EVS_GlobalData.EVS_LogPtr->Next >= CFE_PLATFORM_EVS_LOG_MAX)
         {
            /* This is important, even if we are in discard mode */
            CFE_EVS_GlobalData.EVS_LogPtr->Next = 0;
         }

         /* Log count cannot exceed the number of entries in the log */
         if (CFE_EVS_GlobalData.EVS_LogPtr->LogCount < CFE_PLATFORM_EVS_LOG_MAX)
         {
            CFE_EVS_GlobalData.EVS_LogPtr->LogCount++;

            if (CFE_EVS_GlobalData.EVS_LogPtr->LogCount == CFE_PLATFORM_EVS_LOG_MAX)
            {
               /* The full flag and log count are somewhat redundant */
               CFE_EVS_GlobalData.EVS_LogPtr->LogFullFlag = true;
            }
         }
      }
    
      OS_MutSemGive(CFE_EVS_GlobalData.EVS_SharedDataMutexID);
   }

   return;

} /* End EVS_AddLog */


/*
**             Function Prologue
**
** Function Name:      EVS_ClearLog
**
** Purpose:  This routine clears the contents of the internal event log.
**
** Assumptions and Notes:
**
*/
void EVS_ClearLog ( void )
{

   /* Serialize access to event log control variables */
   OS_MutSemTake(CFE_EVS_GlobalData.EVS_SharedDataMutexID);

   /* Clears everything but LogMode (overwrite vs discard) */
   CFE_EVS_GlobalData.EVS_LogPtr->Next = 0;
   CFE_EVS_GlobalData.EVS_LogPtr->LogCount = 0;
   CFE_EVS_GlobalData.EVS_LogPtr->LogFullFlag = false;
   CFE_EVS_GlobalData.EVS_LogPtr->LogOverflowCounter = 0;

   memset(CFE_EVS_GlobalData.EVS_LogPtr->LogEntry, 0,
                  sizeof(CFE_EVS_GlobalData.EVS_LogPtr->LogEntry));

   OS_MutSemGive(CFE_EVS_GlobalData.EVS_SharedDataMutexID);

   return;

} /* End EVS_ClearLog */


/*
**             Function Prologue
**
** Function Name:      CFE_EVS_WriteLogDataFileCmd
**
** Purpose:  This routine writes the contents of the internal event log to a file
**
** Assumptions and Notes:
**
*/
int32 CFE_EVS_WriteLogDataFileCmd(const CFE_EVS_WriteLogDataFile_t *data)
{
    const CFE_EVS_LogFileCmd_Payload_t *CmdPtr = &data->Payload;
    int32           Result;
    int32           LogIndex;
    int32           BytesWritten;
    int32           LogFileHandle;
    uint32          i;
    CFE_FS_Header_t LogFileHdr;
    char            LogFilename[OS_MAX_PATH_LEN];

    if (CFE_EVS_GlobalData.EVS_TlmPkt.Payload.LogEnabled == false)
    {
        EVS_SendEvent(CFE_EVS_NO_LOGWR_EID, CFE_EVS_EventType_ERROR,
                "Write Log Command: Event Log is Disabled");
        Result = CFE_EVS_FUNCTION_DISABLED;
    }
    else
    {
        /* Copy the commanded filename into local buffer to ensure size limitation and to allow for modification */
        CFE_SB_MessageStringGet(LogFilename, (const char *)CmdPtr->LogFilename, CFE_PLATFORM_EVS_DEFAULT_LOG_FILE,
                OS_MAX_PATH_LEN, sizeof(CmdPtr->LogFilename));

        /* Create the log file */
        LogFileHandle = OS_creat(LogFilename, OS_WRITE_ONLY);

        if (LogFileHandle < OS_FS_SUCCESS)
        {
            EVS_SendEvent(CFE_EVS_ERR_CRLOGFILE_EID, CFE_EVS_EventType_ERROR,
                    "Write Log File Command Error: OS_creat = 0x%08X, filename = %s",
                    (unsigned int)LogFileHandle, LogFilename);

            Result = LogFileHandle;
        }
        else
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
                OS_MutSemTake(CFE_EVS_GlobalData.EVS_SharedDataMutexID);

                /* Is the log full? -- Doesn't matter if wrap mode is enabled */
                if (CFE_EVS_GlobalData.EVS_LogPtr->LogCount == CFE_PLATFORM_EVS_LOG_MAX)
                {
                    /* Start with log entry that will be overwritten next (oldest) */
                    LogIndex = CFE_EVS_GlobalData.EVS_LogPtr->Next;
                }
                else
                {
                    /* Start with the first entry in the log (oldest) */
                    LogIndex = 0;
                }

                /* Write all the "in-use" event log entries to the file */
                for (i = 0; i < CFE_EVS_GlobalData.EVS_LogPtr->LogCount; i++)
                {
                    BytesWritten = OS_write(LogFileHandle,
                            &CFE_EVS_GlobalData.EVS_LogPtr->LogEntry[LogIndex],
                            sizeof(CFE_EVS_GlobalData.EVS_LogPtr->LogEntry[LogIndex]));

                    if (BytesWritten == sizeof(CFE_EVS_GlobalData.EVS_LogPtr->LogEntry[LogIndex]))
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

                OS_MutSemGive(CFE_EVS_GlobalData.EVS_SharedDataMutexID);

                /* Process command handler success result */
                if (i == CFE_EVS_GlobalData.EVS_LogPtr->LogCount)
                {
                    EVS_SendEvent(CFE_EVS_WRLOG_EID, CFE_EVS_EventType_DEBUG,
                            "Write Log File Command: %d event log entries written to %s",
                            (int)CFE_EVS_GlobalData.EVS_LogPtr->LogCount, LogFilename);
                    Result = CFE_SUCCESS;
                }
                else
                {
                    EVS_SendEvent(CFE_EVS_ERR_WRLOGFILE_EID, CFE_EVS_EventType_ERROR,
                            "Write Log File Command Error: OS_write = 0x%08X, filename = %s",
                            (unsigned int)BytesWritten, LogFilename);
                }
            }

            OS_close(LogFileHandle);
        }

    }

    return(Result);

} /* End CFE_EVS_WriteLogDataFileCmd */


/*
**             Function Prologue
**
** Function Name:      CFE_EVS_SetLogModeCmd
**
** Purpose:  This routine sets the internal event log mode.
**
** Assumptions and Notes:
**
*/
int32 CFE_EVS_SetLogModeCmd(const CFE_EVS_SetLogMode_t *data)
{
    const CFE_EVS_SetLogMode_Payload_t *CmdPtr = &data->Payload;
    int32 Status;

    if (CFE_EVS_GlobalData.EVS_TlmPkt.Payload.LogEnabled == true)
    {
        if ((CmdPtr->LogMode == CFE_EVS_LogMode_OVERWRITE) || (CmdPtr->LogMode == CFE_EVS_LogMode_DISCARD))
        {
            /* Serialize access to event log control variables */
            OS_MutSemTake(CFE_EVS_GlobalData.EVS_SharedDataMutexID);
            CFE_EVS_GlobalData.EVS_LogPtr->LogMode = CmdPtr->LogMode;
            OS_MutSemGive(CFE_EVS_GlobalData.EVS_SharedDataMutexID);

            EVS_SendEvent(CFE_EVS_LOGMODE_EID, CFE_EVS_EventType_DEBUG,
                    "Set Log Mode Command: Log Mode = %d", (int)CmdPtr->LogMode);

            Status = CFE_SUCCESS;
        }
        else
        {
            Status = CFE_EVS_INVALID_PARAMETER;
            EVS_SendEvent(CFE_EVS_ERR_LOGMODE_EID, CFE_EVS_EventType_ERROR,
                    "Set Log Mode Command Error: Log Mode = %d", (int)CmdPtr->LogMode);
        }
    }
    else
    {
        Status = CFE_EVS_FUNCTION_DISABLED;
        EVS_SendEvent(CFE_EVS_NO_LOGSET_EID, CFE_EVS_EventType_ERROR,
                "Set Log Mode Command: Event Log is Disabled");
    }


    return Status;

} /* End CFE_EVS_SetLogModeCmd */

/* END EVSLOG.C */
