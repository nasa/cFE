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
**  File:  
**    cfe_es_erlog.c
**
**  Purpose:  
**    This file implements the cFE Executive Services Exception and Reset Log functions.
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**     cFE Flight Software Application Developers Guide
**
**  Notes:
** 
**  Modification History:
**
*/

/*
** Required header files.
*/
#include "private/cfe_private.h"
#include "cfe_es.h"
#include "cfe_es_apps.h"
#include "cfe_es_global.h"
#include "cfe_es_resource.h"
#include "cfe_es_log.h"
#include "cfe_es_task.h"
#include "cfe_psp.h"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*
** Function: CFE_ES_WriteToERLogWithContext
**
** Purpose:  Create an entry in the ES Exception and Reset Log. 
**           This log API accepts extra context information (AppID and ContextID)
**           and is used when the app/task invoking this API is not the same app
**           as where the event occurred.
**
*/
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CFE_ES_WriteToERLogWithContext( CFE_ES_LogEntryType_Enum_t EntryType,   uint32  ResetType, uint32 ResetSubtype,
                           const char  *Description, CFE_ES_ResourceID_t AppId, uint32 PspContextId)
{
   uint32 LogIdx;
   CFE_ES_ERLog_MetaData_t *EntryPtr;
   CFE_TIME_SysTime_t PendingTime;

   /*
    * Snapshot the time before locking (different subsystem)
    */
   PendingTime = CFE_TIME_GetTime();

   /*
    * Ensure that description string is not NULL.
    */
   if ( Description  == NULL)
   {
       Description  = "No Description String Given.";
   }

   /*
    * This routine needs to lock in case it is called
    * from concurrent threads
    */
   CFE_ES_LockSharedData(__func__,__LINE__);

   /*
   ** Try to clean up an invalid ER log index variable.
   */
   if ( CFE_ES_ResetDataPtr->ERLogIndex >= CFE_PLATFORM_ES_ER_LOG_ENTRIES )
   {
      CFE_ES_ResetDataPtr->ERLogIndex = 0;
   }
   LogIdx = CFE_ES_ResetDataPtr->ERLogIndex;

   /*
   ** Now that the Local Index variable is set, increment the index for the next entry.
   */
   CFE_ES_ResetDataPtr->ERLogIndex++;
   if ( CFE_ES_ResetDataPtr->ERLogIndex >= CFE_PLATFORM_ES_ER_LOG_ENTRIES )
   {
      CFE_ES_ResetDataPtr->ERLogIndex = 0;
   }


   /*
   ** Clear out the log entry we are about to use.
   */
   EntryPtr = &CFE_ES_ResetDataPtr->ERLog[LogIdx];
   memset(EntryPtr, 0, sizeof (*EntryPtr));

   /*
   ** Fill out the log fields
   */
   EntryPtr->BaseInfo.LogEntryType = EntryType;
   EntryPtr->BaseInfo.ResetType    = ResetType;
   EntryPtr->BaseInfo.ResetSubtype = ResetSubtype;
   EntryPtr->BaseInfo.BootSource   = CFE_ES_ResetDataPtr->ResetVars.BootSource;
   EntryPtr->BaseInfo.ProcessorResetCount =
                               CFE_ES_ResetDataPtr->ResetVars.ProcessorResetCount;
   EntryPtr->BaseInfo.MaxProcessorResetCount =
                               CFE_ES_ResetDataPtr->ResetVars.MaxProcessorResetCount;

   /*
   ** Copy the ES Reset variables to the log (before they are modified by the log entry).
   */
   memcpy(&EntryPtr->BaseInfo.DebugVars, &CFE_ES_Global.DebugVars,
          sizeof(EntryPtr->BaseInfo.DebugVars));

   /*
   ** Time Stamp the log entry with the system time
   */
   EntryPtr->BaseInfo.TimeCode = PendingTime;

   /*
   ** Copy the Description string to the log.
   */
   strncpy(EntryPtr->BaseInfo.Description, Description, sizeof(EntryPtr->BaseInfo.Description));

   /*
    * Store the context info (if any)
    */
   EntryPtr->AppID = AppId;
   EntryPtr->PspContextId = PspContextId;

   /*
   ** Increment the number of ER log entries made
   */
   CFE_ES_ResetDataPtr->ERLogEntries++;

   /*
    * Shared data update is complete
    */
   CFE_ES_UnlockSharedData(__func__,__LINE__);

   return(CFE_SUCCESS);

} /* End of CFE_ES_WriteToERLogWithContext() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*
** Function: CFE_ES_WriteToERLog
**
** Purpose:  Create an entry in the ES Exception and Reset Log. 
**           This log API is simplified for cases which do not have a separate context
**
*/
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CFE_ES_WriteToERLog( CFE_ES_LogEntryType_Enum_t EntryType,   uint32  ResetType, uint32 ResetSubtype,
                              const char  *Description)
{
    /* passing 0xFFFFFFFF as the appid avoids confusion with actual appid 0 */
    return CFE_ES_WriteToERLogWithContext(EntryType, ResetType, ResetSubtype,
                               Description, CFE_ES_RESOURCEID_UNDEFINED, CFE_ES_ERLOG_NO_CONTEXT);
    
} /* End of CFE_ES_WriteToERLog() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*  Function:  CFE_ES_RunERLogDump()                                             */
/*                                                                               */
/*  Purpose:                                                                     */
/*    Write exception & reset log to a file.                                     */
/*                                                                               */
/*    Implemented as an ES background job, but the entire file write is done     */
/*    in a single invocation, as the file is expected to be relatively small.    */
/*                                                                               */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
bool CFE_ES_RunERLogDump(uint32 ElapsedTime, void *Arg)
{
    CFE_ES_BackgroundLogDumpGlobal_t *State = (CFE_ES_BackgroundLogDumpGlobal_t *)Arg;
    int32               Status;
    int32               PspStatus;
    CFE_FS_Header_t     FileHdr;
    CFE_ES_ERLog_FileEntry_t FileEntry;
    CFE_ES_ERLog_MetaData_t *EntryPtr;
    uint32              FileSize;
    uint32              i;
    osal_id_t           fd;


    if (!State->IsPending)
    {
        return false;
    }

    FileSize = 0;
    Status = OS_OpenCreate(&fd, State->DataFileName, OS_FILE_FLAG_CREATE | OS_FILE_FLAG_TRUNCATE, OS_WRITE_ONLY);
    if(Status < 0)
    {
        CFE_EVS_SendEvent(CFE_ES_ERLOG2_ERR_EID,CFE_EVS_EventType_ERROR,
                "Error creating file %s, RC = %d",
                State->DataFileName, (int)Status);
    }
    else
    {
        CFE_FS_InitHeader(&FileHdr, CFE_ES_ER_LOG_DESC, CFE_FS_SubType_ES_ERLOG);

        /* write the cFE header to the file */
        Status = CFE_FS_WriteHeader(fd, &FileHdr);
        if(Status != sizeof(CFE_FS_Header_t))
        {
            CFE_ES_FileWriteByteCntErr(State->DataFileName,sizeof(CFE_FS_Header_t),Status);
        }
        else
        {
            FileSize += Status;

            /* write a single ER log entry on each pass */
            for(i=0;i<CFE_PLATFORM_ES_ER_LOG_ENTRIES;i++)
            {
                EntryPtr = &CFE_ES_ResetDataPtr->ERLog[i];

                /* The basic info comes directly from the ES log */
                FileEntry.BaseInfo = EntryPtr->BaseInfo;

                /*
                 * The context info, if available, comes from the PSP.
                 * This returns the actual size of the context info, or <0 on error.
                 */
                PspStatus = CFE_PSP_Exception_CopyContext(EntryPtr->PspContextId, &FileEntry.Context, sizeof(FileEntry.Context));
                if (PspStatus > 0)
                {
                    FileEntry.ContextSize = PspStatus;
                }
                else
                {
                    /*
                     * errors here are OK - just means there is no context available.
                     * Record a size of 0 in the log file.
                     */
                    FileEntry.ContextSize = 0;
                }

                /*
                 * any unused context space should be cleared.
                 *
                 * This is for binary compatibility with historical log files, where a fixed amount
                 * of space is given per-entry, regardless of the actual size.
                 */
                if (FileEntry.ContextSize < sizeof(FileEntry.Context))
                {
                    memset(&FileEntry.Context[FileEntry.ContextSize], 0,
                            sizeof(FileEntry.Context) - FileEntry.ContextSize);
                }

                /*
                 * Now write to file
                 */
                Status = OS_write(fd,&FileEntry,sizeof(FileEntry));

                if(Status != sizeof(FileEntry))
                {
                    CFE_ES_FileWriteByteCntErr(State->DataFileName,sizeof(FileEntry),Status);
                    break;
                }/* end if */

                FileSize += Status;

            } /* end for */

        } /* end if */

        OS_close(fd);

        CFE_EVS_SendEvent(CFE_ES_ERLOG2_EID, CFE_EVS_EventType_DEBUG,
                "%s written:Size=%lu",State->DataFileName,(unsigned long)FileSize);

    } /* end if */

    /*
     * Always clear the "pending" flag whether successful or not.
     * If unsuccessful, an operator needs to investigate the error and re-issue command.
     */
    State->IsPending = false;

    return false;
}/* end CFE_ES_RunERLogDump */


/*
**---------------------------------------------------------------------------------------
** Name: CFE_ES_RunExceptionScan
**
**   Purpose: This function pools the PSP to check if any exceptions have been logged
**            since the last background cycle.  If an exception is present, retreive
**            the details, add it to the ER log, and trigger the action (e.g. app restart).
**---------------------------------------------------------------------------------------
*/
bool CFE_ES_RunExceptionScan(uint32 ElapsedTime, void *Arg)
{
    int32               Status;
    uint32              PspContextId;
    char                ReasonString[CFE_ES_ERLOG_DESCRIPTION_MAX_LENGTH];
    CFE_ES_TaskInfo_t   EsTaskInfo;
    osal_id_t           ExceptionTaskID;
    uint32              ResetType;
    CFE_ES_LogEntryType_Enum_t LogType;
    CFE_ES_AppRecord_t  *AppRecPtr;

    if (CFE_PSP_Exception_GetCount() == 0)
    {
        /* no exceptions pending, nothing to do */
        return false;
    }

    /*
     * Note a reset type of 0 is not defined by the PSP -
     * the real values are all nonzero
     */
    ResetType = 0;
    memset(&EsTaskInfo, 0, sizeof(EsTaskInfo));
    Status = CFE_PSP_Exception_GetSummary(&PspContextId, &ExceptionTaskID, ReasonString, sizeof(ReasonString));
    if (Status != CFE_PSP_SUCCESS)
    {
        /* reason string is not available - populate with something for the log */
        snprintf(ReasonString, sizeof(ReasonString), "Unknown - CFE_PSP_ExceptionGetSummary() error %ld", (long)Status);
        PspContextId = 0;
        ExceptionTaskID = OS_OBJECT_ID_UNDEFINED;
    } /* end if */

    /*
     * Note that writes to the ES ER log actually do not get propagated to the debug console.
     * so by writing to SysLog here it becomes visible in both places.
     */
    CFE_ES_WriteToSysLog("ExceptionID 0x%lx in TaskID %lu: %s\n",
            (unsigned long)PspContextId, OS_ObjectIdToInteger(ExceptionTaskID), ReasonString);

    /*
     * If task ID is 0, this means it was a system level exception and
     * not associated with a specific task.
     *
     * Otherwise, if it was related to a task, determine the associated AppID
     * so the exception action can be checked.
     */
    if (OS_ObjectIdDefined(ExceptionTaskID))
    {
        Status = CFE_ES_GetTaskInfo( &EsTaskInfo, CFE_ES_ResourceID_FromOSAL(ExceptionTaskID) );

        /*
         * The App ID was found, now see if the ExceptionAction is set for a reset
         */
        if (Status == CFE_SUCCESS)
        {
            AppRecPtr = CFE_ES_LocateAppRecordByID(EsTaskInfo.AppId);
            CFE_ES_LockSharedData(__func__,__LINE__);
            if (CFE_ES_AppRecordIsMatch(AppRecPtr, EsTaskInfo.AppId) &&
                AppRecPtr->StartParams.ExceptionAction == CFE_ES_ExceptionAction_RESTART_APP)
            {
                /*
                 * Log the Application reset
                 */
                ResetType = CFE_ES_APP_RESTART;
            }
            CFE_ES_UnlockSharedData(__func__,__LINE__);
        }
    }

    do
    {
        /*
         * If no disposition is identified yet, then trigger a PSP reset.
         * Need to determine if a processor or poweron reset is needed.
         */
        if (ResetType == 0)
        {
            if ( CFE_ES_ResetDataPtr->ResetVars.ProcessorResetCount >=
                    CFE_ES_ResetDataPtr->ResetVars.MaxProcessorResetCount )
            {
                CFE_ES_WriteToSysLog("Maximum Processor Reset count reached (%u)",
                        (unsigned int)CFE_ES_ResetDataPtr->ResetVars.MaxProcessorResetCount);

                ResetType = CFE_PSP_RST_TYPE_POWERON;
            }
            else
            {
                CFE_ES_WriteToSysLog("Processor Reset count not reached (%u/%u)",
                        (unsigned int)CFE_ES_ResetDataPtr->ResetVars.ProcessorResetCount,
                        (unsigned int)CFE_ES_ResetDataPtr->ResetVars.MaxProcessorResetCount);


                /*
                ** Update the reset variables
                */
                CFE_ES_ResetDataPtr->ResetVars.ProcessorResetCount++;
                CFE_ES_ResetDataPtr->ResetVars.ES_CausedReset = true;

                ResetType = CFE_PSP_RST_TYPE_PROCESSOR;
            }
        }

        if (ResetType == CFE_ES_APP_RESTART)
        {
            LogType = CFE_ES_LogEntryType_APPLICATION;
        }
        else
        {
            LogType = CFE_ES_LogEntryType_CORE;
        }


        CFE_ES_WriteToERLogWithContext(LogType,  ResetType,
                CFE_PSP_RST_SUBTYPE_EXCEPTION, ReasonString,
                EsTaskInfo.AppId,
                PspContextId);

        if (ResetType == CFE_ES_APP_RESTART)
        {
            /*
             * Restart the App. This call is just a request
             * to ES, but the request could fail.  If that happens,
             * proceed to a processor reset.
             */
            Status = CFE_ES_RestartApp(EsTaskInfo.AppId);
            if (Status != CFE_SUCCESS)
            {
                ResetType = 0;
                snprintf(ReasonString, sizeof(ReasonString), "App Restart Failed");
            }
        }
        else
        {
            /* normally this will not return */
            CFE_PSP_Restart(ResetType);
        }
    }
    while(ResetType == 0);


    return true;    /* returning true because there was an exception to deal with */
}



/* end of file */
