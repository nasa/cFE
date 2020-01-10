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
#include "cfe_evs_task.h"       /* EVS internal definitions */
#include "cfe_evs_log.h"        /* EVS log file definitions */
#include "cfe_evs_utils.h"      /* EVS utility function definitions */
#include "cfe_evs.h"            /* EVS API definitions */

#include <string.h>

#include "cfe_version.h"      /* cFE version definitions */
#include "cfe_error.h"        /* cFE error code definitions */
#include "cfe_es.h"           /* Executive Service definitions */
#include "cfe_fs.h"           /* File Service definitions */
#include "cfe_psp.h"          /* cFE Platform Support Package definitions */
#include "osapi.h"            /* OS API file system definitions */

#include "private/cfe_es_resetdata_typedef.h"  /* Definition of CFE_ES_ResetData_t */

/* Global Data */
CFE_EVS_GlobalData_t CFE_EVS_GlobalData;


/*
** Local function prototypes.
*/
void  CFE_EVS_ProcessGroundCommand ( CFE_SB_MsgPtr_t EVS_MsgPtr );
bool CFE_EVS_VerifyCmdLength(CFE_SB_MsgPtr_t Msg, uint16 ExpectedLength);

/* Function Definitions */


/*
**             Function Prologue
**
** Function Name:      CFE_EVS_EarlyInit
**
** Purpose:  This routine provides initialization for the EVS API.
**
** Assumptions and Notes: This routine must be called before the EVS
**      application is started.  CFE_EVS_EarlyInit performs initialization
**      necessary to support EVS API calls that might occur before
**      the EVS application has completed its startup initialization.
*/
int32 CFE_EVS_EarlyInit ( void )
{

#ifdef CFE_PLATFORM_EVS_LOG_ON

   int32                Status;
   uint32               resetAreaSize = 0;
   cpuaddr              resetAreaAddr;
   CFE_ES_ResetData_t  *CFE_EVS_ResetDataPtr = (CFE_ES_ResetData_t *) NULL;

#endif

   memset(&CFE_EVS_GlobalData, 0, sizeof(CFE_EVS_GlobalData_t));

   CFE_EVS_GlobalData.EVS_AppID = CFE_EVS_UNDEF_APPID;

   /* Initialize housekeeping packet */
   CFE_SB_InitMsg(&CFE_EVS_GlobalData.EVS_TlmPkt, CFE_EVS_HK_TLM_MID,
           sizeof(CFE_EVS_GlobalData.EVS_TlmPkt), false);
  
   /* Elements stored in the hk packet that have non-zero default values */
   CFE_EVS_GlobalData.EVS_TlmPkt.Payload.MessageFormatMode = CFE_PLATFORM_EVS_DEFAULT_MSG_FORMAT_MODE;
   CFE_EVS_GlobalData.EVS_TlmPkt.Payload.OutputPort = CFE_PLATFORM_EVS_PORT_DEFAULT;
   CFE_EVS_GlobalData.EVS_TlmPkt.Payload.LogFullFlag = false;
   CFE_EVS_GlobalData.EVS_TlmPkt.Payload.LogMode = CFE_PLATFORM_EVS_DEFAULT_LOG_MODE;

#ifdef CFE_PLATFORM_EVS_LOG_ON

   /* Get a pointer to the CFE reset area from the BSP */
   Status = CFE_PSP_GetResetArea(&resetAreaAddr, &resetAreaSize);

   if (Status != CFE_PSP_SUCCESS)
   {
      CFE_ES_WriteToSysLog("EVS call to CFE_PSP_GetResetArea failed, RC=0x%08x\n", (unsigned int)Status);
   }
   else if (resetAreaSize < sizeof(CFE_ES_ResetData_t))
   {
      /* Got the pointer but the size is wrong */
      Status = CFE_EVS_RESET_AREA_POINTER;
      CFE_ES_WriteToSysLog("Unexpected size from CFE_PSP_GetResetArea: expected = 0x%08lX, actual = 0x%08lX\n",
                            (unsigned long)sizeof(CFE_ES_ResetData_t), (unsigned long)resetAreaSize);
   }
   else
   {
      CFE_EVS_ResetDataPtr = (CFE_ES_ResetData_t *)resetAreaAddr;
      /* Save pointer to the EVS portion of the CFE reset area */
      CFE_EVS_GlobalData.EVS_LogPtr = &CFE_EVS_ResetDataPtr->EVS_Log;

      /* Create semaphore to serialize access to event log */
      Status = OS_MutSemCreate(&CFE_EVS_GlobalData.EVS_SharedDataMutexID, "CFE_EVS_DataMutex", 0);

      if (Status != OS_SUCCESS)
      {
         CFE_ES_WriteToSysLog("EVS call to OS_MutSemCreate failed, RC=0x%08x\n", (unsigned int)Status);
      }
      else
      {
         /* Enable access to the EVS event log */                                                            
         CFE_EVS_GlobalData.EVS_TlmPkt.Payload.LogEnabled = true;

         /* Clear event log if power-on reset or bad contents */                                                            
         if (CFE_ES_GetResetType(NULL) == CFE_PSP_RST_TYPE_POWERON)                                                                   
         {
            CFE_ES_WriteToSysLog("Event Log cleared following power-on reset\n");
            EVS_ClearLog();                                                                                         
            CFE_EVS_GlobalData.EVS_LogPtr->LogMode = CFE_PLATFORM_EVS_DEFAULT_LOG_MODE;
         }
         else if (((CFE_EVS_GlobalData.EVS_LogPtr->LogMode != CFE_EVS_LogMode_OVERWRITE) &&
                   (CFE_EVS_GlobalData.EVS_LogPtr->LogMode != CFE_EVS_LogMode_DISCARD))  ||
                  ((CFE_EVS_GlobalData.EVS_LogPtr->LogFullFlag != false)   &&
                   (CFE_EVS_GlobalData.EVS_LogPtr->LogFullFlag != true))   ||
                   (CFE_EVS_GlobalData.EVS_LogPtr->Next >= CFE_PLATFORM_EVS_LOG_MAX))
         {
            CFE_ES_WriteToSysLog("Event Log cleared, n=%d, c=%d, f=%d, m=%d, o=%d\n",
                                  (int)CFE_EVS_GlobalData.EVS_LogPtr->Next,
                                  (int)CFE_EVS_GlobalData.EVS_LogPtr->LogCount,
                                  (int)CFE_EVS_GlobalData.EVS_LogPtr->LogFullFlag,
                                  (int)CFE_EVS_GlobalData.EVS_LogPtr->LogMode,
                                  (int)CFE_EVS_GlobalData.EVS_LogPtr->LogOverflowCounter);
            EVS_ClearLog();                                                                                         
            CFE_EVS_GlobalData.EVS_LogPtr->LogMode = CFE_PLATFORM_EVS_DEFAULT_LOG_MODE;
         }
         else
         {
            CFE_ES_WriteToSysLog("Event Log restored, n=%d, c=%d, f=%d, m=%d, o=%d\n",
                                  (int)CFE_EVS_GlobalData.EVS_LogPtr->Next,
                                  (int)CFE_EVS_GlobalData.EVS_LogPtr->LogCount,
                                  (int)CFE_EVS_GlobalData.EVS_LogPtr->LogFullFlag,
                                  (int)CFE_EVS_GlobalData.EVS_LogPtr->LogMode,
                                  (int)CFE_EVS_GlobalData.EVS_LogPtr->LogOverflowCounter);
         }
      }
   }

#endif

   return(CFE_SUCCESS);

} /* End CFE_EVS_EarlyInit */


/*
**             Function Prologue
**
** Function Name:      CFE_EVS_CleanUpApp
**
** Purpose:  ES calls this routine when an app is being terminated.
**
** Assumptions and Notes:
*/
int32 CFE_EVS_CleanUpApp(uint32 AppID)
{
   int32  Status = CFE_SUCCESS;

   if (AppID >= CFE_PLATFORM_ES_MAX_APPLICATIONS)
   {
      Status = CFE_EVS_APP_ILLEGAL_APP_ID;
   }
   else if (CFE_EVS_GlobalData.AppData[AppID].RegisterFlag == true)
   {
      /* Same cleanup as CFE_EVS_Unregister() */
      memset(&CFE_EVS_GlobalData.AppData[AppID], 0, sizeof(EVS_AppData_t));
   }
    
   return(Status);
}


/*
**             Function Prologue
**
** Function Name:      EVS_TaskMain
**
** Purpose:  This is the main EVS task process loop.
**
** Assumptions and Notes:
**
*/
void CFE_EVS_TaskMain(void)
{
    int32 Status;    
    CFE_SB_MsgPtr_t    EVS_MsgPtr; /* Pointer to SB message */

    CFE_ES_PerfLogEntry(CFE_MISSION_EVS_MAIN_PERF_ID);    
   
    Status = CFE_EVS_TaskInit();
    
    if(Status != CFE_SUCCESS)
    {
      CFE_ES_WriteToSysLog("EVS:Application Init Failed,RC=0x%08X\n", (unsigned int)Status);
      CFE_ES_PerfLogExit(CFE_MISSION_EVS_MAIN_PERF_ID);
      /* Note: CFE_ES_ExitApp will not return */
      CFE_ES_ExitApp(CFE_ES_RunStatus_CORE_APP_INIT_ERROR);
    }/* end if */      

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
        Status = CFE_SB_RcvMsg(&EVS_MsgPtr, 
                               CFE_EVS_GlobalData.EVS_CommandPipe, 
                               CFE_SB_PEND_FOREVER);

        CFE_ES_PerfLogEntry(CFE_MISSION_EVS_MAIN_PERF_ID);

        if (Status == CFE_SUCCESS)
        {
            /* Process cmd pipe msg */
            CFE_EVS_ProcessCommandPacket(EVS_MsgPtr);
        }else{            
            CFE_ES_WriteToSysLog("EVS:Error reading cmd pipe,RC=0x%08X\n",(unsigned int)Status);
        }/* end if */
    
    }/* end while */

    /* while loop exits only if CFE_SB_RcvMsg returns error */
    CFE_ES_ExitApp(CFE_ES_RunStatus_CORE_APP_RUNTIME_ERROR);

} /* end CFE_EVS_TaskMain */



/*
**             Function Prologue
**
** Function Name:      CFE_EVS_TaskInit
**
** Purpose:  This function performs any necessary EVS task initialization.
**
** Assumptions and Notes:
**
*/
int32 CFE_EVS_TaskInit ( void )
{
   int32 Status;
   uint32 AppID;
 
   /* Register EVS application */
   Status = CFE_ES_RegisterApp();
   if (Status != CFE_SUCCESS)
   {
      CFE_ES_WriteToSysLog("EVS:Call to CFE_ES_RegisterApp Failed:RC=0x%08X\n",(unsigned int)Status);
      return Status;
   }

   /* Query and verify the EVS AppID */
   Status = EVS_GetAppID(&AppID);
   if (Status != CFE_SUCCESS)
   {
      CFE_ES_WriteToSysLog("EVS:Call to CFE_ES_GetAppID Failed:RC=0x%08X\n",(unsigned int)Status);
      return Status;
   }

   /* Register EVS task for event services */
   Status = CFE_EVS_Register(NULL, 0, CFE_EVS_EventFilter_BINARY);
   if (Status != CFE_SUCCESS)
   {
      CFE_ES_WriteToSysLog("EVS:Call to CFE_EVS_Register Failed:RC=0x%08X\n",(unsigned int)Status);
      return Status;
   }
    
   /* Create software bus command pipe */
   Status = CFE_SB_CreatePipe(&CFE_EVS_GlobalData.EVS_CommandPipe,
                               CFE_EVS_PIPE_DEPTH, CFE_EVS_PIPE_NAME);
   if (Status != CFE_SUCCESS)
   {
      CFE_ES_WriteToSysLog("EVS:Call to CFE_SB_CreatePipe Failed:RC=0x%08X\n",(unsigned int)Status);
      return Status;
   }
      
   /* Subscribe to command and telemetry requests coming in on the command pipe */
   Status = CFE_SB_SubscribeEx(CFE_EVS_CMD_MID, CFE_EVS_GlobalData.EVS_CommandPipe,
                               CFE_SB_Default_Qos, CFE_EVS_MSG_LIMIT);
   if (Status != CFE_SUCCESS)
   {
      CFE_ES_WriteToSysLog("EVS:Subscribing to Cmds Failed:RC=0x%08X\n",(unsigned int)Status);
      return Status;
   }
  
   Status = CFE_SB_SubscribeEx(CFE_EVS_SEND_HK_MID, CFE_EVS_GlobalData.EVS_CommandPipe,
                               CFE_SB_Default_Qos, CFE_EVS_MSG_LIMIT);
   if (Status != CFE_SUCCESS)
   {
      CFE_ES_WriteToSysLog("EVS:Subscribing to HK Request Failed:RC=0x%08X\n",(unsigned int)Status);
      return Status;
   }
  
   /* Write the AppID to the global location, now that the rest of initialization is done */
   CFE_EVS_GlobalData.EVS_AppID = AppID;
   EVS_SendEvent(CFE_EVS_STARTUP_EID, CFE_EVS_EventType_INFORMATION, "cFE EVS Initialized. cFE Version %d.%d.%d.%d",
                 CFE_MAJOR_VERSION,CFE_MINOR_VERSION,CFE_REVISION,CFE_MISSION_REV);

   return CFE_SUCCESS;

} /* End CFE_EVS_TaskInit */



/*
**             Function Prologue
**
** Function Name:      CFE_EVS_ProcessCommandPacket
**
** Purpose:  This function processes packets received on the EVS command pipe.
**
** Assumptions and Notes:
**
*/
void CFE_EVS_ProcessCommandPacket ( CFE_SB_MsgPtr_t EVS_MsgPtr )
{
    /* Process all SB messages */
    switch (CFE_SB_GetMsgId(EVS_MsgPtr))
    {
        case CFE_EVS_CMD_MID:
            /* EVS task specific command */
            CFE_EVS_ProcessGroundCommand(EVS_MsgPtr);
            break;

        case CFE_EVS_SEND_HK_MID:
            /* Housekeeping request */
            CFE_EVS_ReportHousekeepingCmd((CCSDS_CommandPacket_t*)EVS_MsgPtr);
            break;

        default:
            /* Unknown command -- should never occur */
            CFE_EVS_GlobalData.EVS_TlmPkt.Payload.CommandErrorCounter++;
            EVS_SendEvent(CFE_EVS_ERR_MSGID_EID, CFE_EVS_EventType_ERROR,
                         "Invalid command packet, Message ID = 0x%08X",
                          (unsigned int)CFE_SB_GetMsgId(EVS_MsgPtr));
            break;
    }

    return;

} /* End CFE_EVS_ProcessCommandPacket */


/*
**             Function Prologue
**
** Function Name:      CFE_EVS_ProcessGroundCommand
**
** Purpose:  This function processes a command, verifying that it is valid and of
**           proper length.
**
** Assumptions and Notes:
**
*/
void CFE_EVS_ProcessGroundCommand ( CFE_SB_MsgPtr_t EVS_MsgPtr )
{
   /* status will get reset if it passes length check */
   int32 Status = CFE_STATUS_WRONG_MSG_LENGTH;

   /* Process "known" EVS task ground commands */
   switch (CFE_SB_GetCmdCode(EVS_MsgPtr))
   {
      case CFE_EVS_NOOP_CC:

         if (CFE_EVS_VerifyCmdLength(EVS_MsgPtr, sizeof(CFE_EVS_Noop_t)))
         {
            Status = CFE_EVS_NoopCmd((CFE_EVS_Noop_t*)EVS_MsgPtr);
         }
         break;

      case CFE_EVS_RESET_COUNTERS_CC:

         if (CFE_EVS_VerifyCmdLength(EVS_MsgPtr, sizeof(CFE_EVS_ResetCounters_t)))
         {
            Status = CFE_EVS_ResetCountersCmd((CFE_EVS_ResetCounters_t*)EVS_MsgPtr);
         }
         break;

      case CFE_EVS_ENABLE_EVENT_TYPE_CC:

         if (CFE_EVS_VerifyCmdLength(EVS_MsgPtr, sizeof(CFE_EVS_EnableEventType_t)))
         {
             Status = CFE_EVS_EnableEventTypeCmd((CFE_EVS_EnableEventType_t*)EVS_MsgPtr);
         }
         break;

      case CFE_EVS_DISABLE_EVENT_TYPE_CC:

         if (CFE_EVS_VerifyCmdLength(EVS_MsgPtr, sizeof(CFE_EVS_DisableEventType_t)))
         {
             Status = CFE_EVS_DisableEventTypeCmd((CFE_EVS_DisableEventType_t*)EVS_MsgPtr);
         }
         break;

      case CFE_EVS_SET_EVENT_FORMAT_MODE_CC:

         if (CFE_EVS_VerifyCmdLength(EVS_MsgPtr, sizeof(CFE_EVS_SetEventFormatMode_t)))
         {
             Status = CFE_EVS_SetEventFormatModeCmd((CFE_EVS_SetEventFormatMode_t*)EVS_MsgPtr);
         }
         break;

      case CFE_EVS_ENABLE_APP_EVENT_TYPE_CC:

         if (CFE_EVS_VerifyCmdLength(EVS_MsgPtr, sizeof(CFE_EVS_EnableAppEventType_t)))
         {
             Status = CFE_EVS_EnableAppEventTypeCmd((CFE_EVS_EnableAppEventType_t*)EVS_MsgPtr);
         }
         break;

      case CFE_EVS_DISABLE_APP_EVENT_TYPE_CC:

         if (CFE_EVS_VerifyCmdLength(EVS_MsgPtr, sizeof(CFE_EVS_DisableAppEventType_t)))
         {
             Status = CFE_EVS_DisableAppEventTypeCmd((CFE_EVS_DisableAppEventType_t*)EVS_MsgPtr);
         }
         break;

      case CFE_EVS_ENABLE_APP_EVENTS_CC:

         if (CFE_EVS_VerifyCmdLength(EVS_MsgPtr, sizeof(CFE_EVS_EnableAppEvents_t)))
         {
             Status = CFE_EVS_EnableAppEventsCmd((CFE_EVS_EnableAppEvents_t*)EVS_MsgPtr);
         }
         break;

      case CFE_EVS_DISABLE_APP_EVENTS_CC:

         if (CFE_EVS_VerifyCmdLength(EVS_MsgPtr, sizeof(CFE_EVS_DisableAppEvents_t)))
         {
             Status = CFE_EVS_DisableAppEventsCmd((CFE_EVS_DisableAppEvents_t*)EVS_MsgPtr);
         }
         break;

      case CFE_EVS_RESET_APP_COUNTER_CC:

         if (CFE_EVS_VerifyCmdLength(EVS_MsgPtr, sizeof(CFE_EVS_ResetAppCounter_t)))
         {
             Status = CFE_EVS_ResetAppCounterCmd((CFE_EVS_ResetAppCounter_t*)EVS_MsgPtr);
         }
         break;

      case CFE_EVS_SET_FILTER_CC:

         if (CFE_EVS_VerifyCmdLength(EVS_MsgPtr, (uint16) sizeof(CFE_EVS_SetFilter_t)))
         {
             Status = CFE_EVS_SetFilterCmd((CFE_EVS_SetFilter_t*)EVS_MsgPtr);
         }
         break;

      case CFE_EVS_ENABLE_PORTS_CC:

         if (CFE_EVS_VerifyCmdLength(EVS_MsgPtr, sizeof(CFE_EVS_EnablePorts_t)))
         {
             Status = CFE_EVS_EnablePortsCmd((CFE_EVS_EnablePorts_t*)EVS_MsgPtr);
         }
         break;

      case CFE_EVS_DISABLE_PORTS_CC:

         if (CFE_EVS_VerifyCmdLength(EVS_MsgPtr, sizeof(CFE_EVS_DisablePorts_t)))
         {
             Status = CFE_EVS_DisablePortsCmd((CFE_EVS_DisablePorts_t*)EVS_MsgPtr);
         }
         break;

      case CFE_EVS_RESET_FILTER_CC:

         if (CFE_EVS_VerifyCmdLength(EVS_MsgPtr, sizeof(CFE_EVS_ResetFilter_t)))
         {
             Status = CFE_EVS_ResetFilterCmd((CFE_EVS_ResetFilter_t*)EVS_MsgPtr);
         }
         break;

      case CFE_EVS_RESET_ALL_FILTERS_CC:

         if (CFE_EVS_VerifyCmdLength(EVS_MsgPtr, sizeof(CFE_EVS_ResetAllFilters_t)))
         {
             Status = CFE_EVS_ResetAllFiltersCmd((CFE_EVS_ResetAllFilters_t*)EVS_MsgPtr);
         }
         break;

      case CFE_EVS_ADD_EVENT_FILTER_CC:

         if (CFE_EVS_VerifyCmdLength(EVS_MsgPtr, sizeof(CFE_EVS_AddEventFilter_t)))
         {
             Status = CFE_EVS_AddEventFilterCmd((CFE_EVS_AddEventFilter_t*)EVS_MsgPtr);
         }
         break;

      case CFE_EVS_DELETE_EVENT_FILTER_CC:

         if (CFE_EVS_VerifyCmdLength(EVS_MsgPtr, sizeof(CFE_EVS_DeleteEventFilter_t)))
         {
             Status = CFE_EVS_DeleteEventFilterCmd((CFE_EVS_DeleteEventFilter_t*)EVS_MsgPtr);
         }
         break;

      case CFE_EVS_WRITE_APP_DATA_FILE_CC:

         if (CFE_EVS_VerifyCmdLength(EVS_MsgPtr, sizeof(CFE_EVS_WriteAppDataFile_t)))
         {
             Status = CFE_EVS_WriteAppDataFileCmd((CFE_EVS_WriteAppDataFile_t*)EVS_MsgPtr);
         }
         break;

      case CFE_EVS_SET_LOG_MODE_CC:

         if (CFE_EVS_VerifyCmdLength(EVS_MsgPtr, sizeof(CFE_EVS_SetLogMode_t)))
         {
             Status = CFE_EVS_SetLogModeCmd((CFE_EVS_SetLogMode_t*)EVS_MsgPtr);
         }
         break;

      case CFE_EVS_CLEAR_LOG_CC:

         if (CFE_EVS_VerifyCmdLength(EVS_MsgPtr, sizeof(CFE_EVS_ClearLog_t)))
         {
             Status = CFE_EVS_ClearLogCmd((CFE_EVS_ClearLog_t *)EVS_MsgPtr);
         }
         break;

      case CFE_EVS_WRITE_LOG_DATA_FILE_CC:

         if (CFE_EVS_VerifyCmdLength(EVS_MsgPtr, sizeof(CFE_EVS_WriteLogDataFile_t)))
         {
             Status = CFE_EVS_WriteLogDataFileCmd((CFE_EVS_WriteLogDataFile_t*)EVS_MsgPtr);
         }
         break;

       /* default is a bad command code as it was not found above */
       default:

          EVS_SendEvent(CFE_EVS_ERR_CC_EID, CFE_EVS_EventType_ERROR,
                       "Invalid command code -- ID = 0x%08x, CC = %d",
                        (unsigned int)CFE_SB_GetMsgId(EVS_MsgPtr), (int)CFE_SB_GetCmdCode(EVS_MsgPtr));
          Status = CFE_STATUS_BAD_COMMAND_CODE;

          break;
   }

   if (Status == CFE_SUCCESS)
   {
      CFE_EVS_GlobalData.EVS_TlmPkt.Payload.CommandCounter++;
   }
   else if (Status < 0)  /* Negative values indicate errors */
   {
      CFE_EVS_GlobalData.EVS_TlmPkt.Payload.CommandErrorCounter++;
   }

   return;

} /* End of EVS_ProcessGroundCommand() */


/*
**             Function Prologue
**
** Function Name:      CFE_EVS_VerifyCmdLength
**
** Purpose:  This function validates the length of incoming commands.
**
** Assumptions and Notes:
**
*/
bool CFE_EVS_VerifyCmdLength(CFE_SB_MsgPtr_t Msg, uint16 ExpectedLength)
{
    bool    result       = true;
    uint16  ActualLength = CFE_SB_GetTotalMsgLength(Msg);

    /*
    ** Verify the command packet length
    */
    if (ExpectedLength != ActualLength)
    {
        CFE_SB_MsgId_t MessageID = CFE_SB_GetMsgId(Msg);
        uint16 CommandCode = CFE_SB_GetCmdCode(Msg);

        EVS_SendEvent(CFE_EVS_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
           "Invalid cmd length: ID = 0x%X, CC = %d, Exp Len = %d, Len = %d",
                          (unsigned int)MessageID, (int)CommandCode, (int)ExpectedLength, (int)ActualLength);
        result = false;
    }

    return(result);

} /* End of CFE_EVS_VerifyCmdLength() */

/*
**             Function Prologue
**
** Function Name:      CFE_EVS_NoopCmd
**
** Purpose:  This function processes "no-op" commands received on the EVS command pipe.
**
** Assumptions and Notes:
**
*/
int32 CFE_EVS_NoopCmd(const CFE_EVS_Noop_t *data)
{
   EVS_SendEvent(CFE_EVS_NOOP_EID, CFE_EVS_EventType_INFORMATION,"No-op command. cFE Version %d.%d.%d.%d",
                 CFE_MAJOR_VERSION,CFE_MINOR_VERSION,CFE_REVISION,CFE_MISSION_REV);
   return CFE_SUCCESS;
}

/*
**             Function Prologue
**
** Function Name:      CFE_EVS_ClearLogCmd
**
** Purpose:  This function processes "clear log" commands received on the EVS command pipe.
**
** Assumptions and Notes:
**
*/
int32 CFE_EVS_ClearLogCmd(const CFE_EVS_ClearLog_t *data)
{
    int32 Status;

    if (CFE_EVS_GlobalData.EVS_TlmPkt.Payload.LogEnabled == true)
    {
        EVS_ClearLog();
        Status = CFE_SUCCESS;
    }
    else
    {
       EVS_SendEvent(CFE_EVS_NO_LOGCLR_EID, CFE_EVS_EventType_ERROR,
                    "Clear Log Command: Event Log is Disabled");
       Status = CFE_EVS_FUNCTION_DISABLED;
    }
    return Status;
}

/*
**             Function Prologue
**
** Function Name:      CFE_EVS_ReportHousekeepingCmd
**
** Purpose:  Request for housekeeping status telemetry packet.
**
** Assumptions and Notes:
**
*/
int32 CFE_EVS_ReportHousekeepingCmd (const CCSDS_CommandPacket_t *data)
{
   uint32 i, j;


   if (CFE_EVS_GlobalData.EVS_TlmPkt.Payload.LogEnabled == true)
   {   
      /* Copy hk variables that are maintained in the event log */
      CFE_EVS_GlobalData.EVS_TlmPkt.Payload.LogFullFlag = CFE_EVS_GlobalData.EVS_LogPtr->LogFullFlag;
      CFE_EVS_GlobalData.EVS_TlmPkt.Payload.LogMode = CFE_EVS_GlobalData.EVS_LogPtr->LogMode;
      CFE_EVS_GlobalData.EVS_TlmPkt.Payload.LogOverflowCounter = CFE_EVS_GlobalData.EVS_LogPtr->LogOverflowCounter;
   }

   /* Write event state data for registered apps to telemetry packet */
   for (i = 0, j = 0; j < CFE_MISSION_ES_MAX_APPLICATIONS && i < CFE_PLATFORM_ES_MAX_APPLICATIONS; i++)
   {
      if (CFE_EVS_GlobalData.AppData[i].RegisterFlag == true)
      {
         CFE_EVS_GlobalData.EVS_TlmPkt.Payload.AppData[j].AppID = i;
         CFE_EVS_GlobalData.EVS_TlmPkt.Payload.AppData[j].AppEnableStatus = CFE_EVS_GlobalData.AppData[i].ActiveFlag;
         CFE_EVS_GlobalData.EVS_TlmPkt.Payload.AppData[j].AppMessageSentCounter = CFE_EVS_GlobalData.AppData[i].EventCount;
         j++;
      }
   }

   /* Clear unused portion of event state data in telemetry packet */
   for (i = j; i < CFE_MISSION_ES_MAX_APPLICATIONS; i++)
   {
      CFE_EVS_GlobalData.EVS_TlmPkt.Payload.AppData[i].AppID = 0;
      CFE_EVS_GlobalData.EVS_TlmPkt.Payload.AppData[i].AppEnableStatus = false;
      CFE_EVS_GlobalData.EVS_TlmPkt.Payload.AppData[i].AppMessageSentCounter = 0;
   }

   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &CFE_EVS_GlobalData.EVS_TlmPkt);

   CFE_SB_SendMsg((CFE_SB_Msg_t *) &CFE_EVS_GlobalData.EVS_TlmPkt);

   return CFE_STATUS_NO_COUNTER_INCREMENT;
} /* End of CFE_EVS_ReportHousekeepingCmd() */


/*
**             Function Prologue
**
** Function Name:      CFE_EVS_ResetCountersCmd
**
** Purpose:  This function resets all the global counter variables that are
**           part of the task telemetry.
**
** Assumptions and Notes:
**
*/
int32 CFE_EVS_ResetCountersCmd(const CFE_EVS_ResetCounters_t *data)
{
    /* Status of commands processed by EVS task */
    CFE_EVS_GlobalData.EVS_TlmPkt.Payload.CommandCounter  = 0;
    CFE_EVS_GlobalData.EVS_TlmPkt.Payload.CommandErrorCounter   =  0;

    /* EVS telemetry counters */
    CFE_EVS_GlobalData.EVS_TlmPkt.Payload.MessageSendCounter = 0;
    CFE_EVS_GlobalData.EVS_TlmPkt.Payload.MessageTruncCounter = 0;
    CFE_EVS_GlobalData.EVS_TlmPkt.Payload.UnregisteredAppCounter = 0;

    EVS_SendEvent(CFE_EVS_RSTCNT_EID, CFE_EVS_EventType_DEBUG, "Reset Counters Command Received");

    /* NOTE: Historically the reset counters command does _NOT_ increment the command counter */

    return CFE_STATUS_NO_COUNTER_INCREMENT;
} /* End of CFE_EVS_ResetCountersCmd() */


/*
**             Function Prologue
**
** Function Name:      CFE_EVS_SetEventFilterMaskCmd
**
** Purpose:  This routine sets the filter mask for the given event_id in the
**           calling task's filter array
**
** Assumptions and Notes:
**
*/
int32 CFE_EVS_SetFilterCmd(const CFE_EVS_SetFilter_t *data)
{
   const CFE_EVS_AppNameEventIDMaskCmd_Payload_t *CmdPtr = &data->Payload;
   EVS_BinFilter_t    *FilterPtr;
   uint32              AppID = CFE_EVS_UNDEF_APPID;
   int32               Status;
   EVS_AppData_t      *AppDataPtr;
   char                LocalName[OS_MAX_API_NAME];

   /*
    * Althgouh EVS_GetApplicationInfo() does not require a null terminated argument,
    * the value is passed to EVS_SendEvent which does require termination (normal C string)
    */
   CFE_SB_MessageStringGet(LocalName, (char *)CmdPtr->AppName, NULL, OS_MAX_API_NAME, sizeof(CmdPtr->AppName));

   /* Retreive application data */
   Status = EVS_GetApplicationInfo(&AppID, LocalName);

   if (Status == CFE_SUCCESS)
   {
      AppDataPtr = &CFE_EVS_GlobalData.AppData[AppID];

      FilterPtr = EVS_FindEventID(CmdPtr->EventID, AppDataPtr->BinFilters);

      if(FilterPtr != NULL)
      {
         /* Set application filter mask */
         FilterPtr->Mask = CmdPtr->Mask;

         EVS_SendEvent(CFE_EVS_SETFILTERMSK_EID, CFE_EVS_EventType_DEBUG,
                           "Set Filter Mask Command Received with AppName=%s, EventID=0x%08x, Mask=0x%04x",
                           LocalName, (unsigned int)CmdPtr->EventID, (unsigned int)CmdPtr->Mask);

      }
      else
      {
         EVS_SendEvent(CFE_EVS_ERR_EVTIDNOREGS_EID, CFE_EVS_EventType_ERROR,
                           "%s Event ID %d not registered for filtering: CC = %lu ",
                           LocalName, (int)CmdPtr->EventID, (long unsigned int)CFE_EVS_SET_FILTER_CC);

         Status = CFE_EVS_EVT_NOT_REGISTERED;
      }
   }
   else if(Status == CFE_EVS_APP_NOT_REGISTERED)
   {
      EVS_SendEvent(CFE_EVS_ERR_APPNOREGS_EID, CFE_EVS_EventType_ERROR,
                        "%s not registered with EVS: CC = %lu",
                        LocalName, (long unsigned int)CFE_EVS_SET_FILTER_CC);
   }
   else if(Status == CFE_EVS_APP_ILLEGAL_APP_ID)
   {
      EVS_SendEvent(CFE_EVS_ERR_ILLAPPIDRANGE_EID, CFE_EVS_EventType_ERROR,
                        "Illegal application ID %d retrieved for %s: CC = %lu",
                        (int)AppID, LocalName, (long unsigned int)CFE_EVS_SET_FILTER_CC);
   }
   else
   {
      EVS_SendEvent(CFE_EVS_ERR_NOAPPIDFOUND_EID, CFE_EVS_EventType_ERROR,
                        "Unable to retrieve application ID for %s: CC = %lu",
                        LocalName, (long unsigned int)CFE_EVS_SET_FILTER_CC);
   }

   return Status;

} /* End CFE_EVS_SetFilterMaskCmd */



/*
**             Function Prologue
**
** Function Name:      CFE_EVS_EnablePortsCmd
**
** Purpose:  This routine sets the command given ports to an enabled state
**
** Assumptions and Notes:
** Shifting is done so the value not masked off is placed in the ones spot:
** necessary for comparing with true.
*/
int32 CFE_EVS_EnablePortsCmd(const CFE_EVS_EnablePorts_t *data)
{
    const CFE_EVS_BitMaskCmd_Payload_t *CmdPtr = &data->Payload;
    int32 ReturnCode;

    /* Need to check for an out of range bitmask, since oue bit masks are only 4 bits */
    if (CmdPtr->BitMask == 0x0 || CmdPtr->BitMask > 0x0F)
    {
        EVS_SendEvent(CFE_EVS_ERR_INVALID_BITMASK_EID, CFE_EVS_EventType_ERROR,
                          "Bit Mask = 0x%08x out of range: CC = %lu",
                          (unsigned int)CmdPtr->BitMask, (long unsigned int)CFE_EVS_ENABLE_PORTS_CC);
        ReturnCode = CFE_EVS_INVALID_PARAMETER;
    }
    else
    {

        /* Process command data */
        if(((CmdPtr->BitMask & CFE_EVS_PORT1_BIT) >> 0) == true)
        {
            CFE_EVS_GlobalData.EVS_TlmPkt.Payload.OutputPort |= CFE_EVS_PORT1_BIT;
        }
        if(((CmdPtr->BitMask & CFE_EVS_PORT2_BIT) >>1) == true)
        {
            CFE_EVS_GlobalData.EVS_TlmPkt.Payload.OutputPort |= CFE_EVS_PORT2_BIT;
        }
        if(((CmdPtr->BitMask & CFE_EVS_PORT3_BIT) >> 2) == true)
        {
            CFE_EVS_GlobalData.EVS_TlmPkt.Payload.OutputPort |= CFE_EVS_PORT3_BIT;
        }
        if(((CmdPtr->BitMask & CFE_EVS_PORT4_BIT) >>3) == true)
        {
            CFE_EVS_GlobalData.EVS_TlmPkt.Payload.OutputPort |= CFE_EVS_PORT4_BIT;
        }

        EVS_SendEvent(CFE_EVS_ENAPORT_EID, CFE_EVS_EventType_DEBUG,
                        "Enable Ports Command Received with Port Bit Mask = 0x%02x",
                        (unsigned int)CmdPtr->BitMask);
        ReturnCode = CFE_SUCCESS;
    }

    return ReturnCode;

} /* End CFE_EVS_EnablePortsCmd */


/*
**             Function Prologue
**
** Function Name:      CFE_EVS_DisablePortsCmd
**
** Purpose:  This routine sets the command given ports to a disabled state
**
** Assumptions and Notes:
** Shifting is done so the value not masked off is placed in the ones spot:
** necessary for comparing with true.
*/
int32 CFE_EVS_DisablePortsCmd(const CFE_EVS_DisablePorts_t *data)
{
    const CFE_EVS_BitMaskCmd_Payload_t *CmdPtr = &data->Payload;
    int32 ReturnCode;

    /* Need to check for an out of range bitmask, since oue bit masks are only 4 bits */
    if (CmdPtr->BitMask == 0x0 || CmdPtr->BitMask > 0x0F)
    {
        EVS_SendEvent(CFE_EVS_ERR_INVALID_BITMASK_EID, CFE_EVS_EventType_ERROR,
                          "Bit Mask = 0x%08x out of range: CC = %lu",
                          (unsigned int)CmdPtr->BitMask, (long unsigned int)CFE_EVS_DISABLE_PORTS_CC);
        ReturnCode = CFE_EVS_INVALID_PARAMETER;
    }
    else
    {

        /* Process command data */
        if(((CmdPtr->BitMask & CFE_EVS_PORT1_BIT) >>0) == true)
        {
            CFE_EVS_GlobalData.EVS_TlmPkt.Payload.OutputPort &= ~CFE_EVS_PORT1_BIT;
        }
        if(((CmdPtr->BitMask & CFE_EVS_PORT2_BIT) >> 1) == true)
        {
            CFE_EVS_GlobalData.EVS_TlmPkt.Payload.OutputPort &= ~CFE_EVS_PORT2_BIT;
        }
        if(((CmdPtr->BitMask & CFE_EVS_PORT3_BIT) >> 2) == true)
        {
            CFE_EVS_GlobalData.EVS_TlmPkt.Payload.OutputPort &= ~CFE_EVS_PORT3_BIT;
        }
        if(((CmdPtr->BitMask & CFE_EVS_PORT4_BIT) >>3) == true)
        {
            CFE_EVS_GlobalData.EVS_TlmPkt.Payload.OutputPort &= ~CFE_EVS_PORT4_BIT;
        }

        EVS_SendEvent(CFE_EVS_DISPORT_EID, CFE_EVS_EventType_DEBUG,
                         "Disable Ports Command Received with Port Bit Mask = 0x%02x",
                         (unsigned int)CmdPtr->BitMask);
        
        ReturnCode = CFE_SUCCESS;
    }
   
    return ReturnCode;

} /* End CFE_EVS_DisablePortsCmd */



/*
**             Function Prologue
**
** Function Name:      CFE_EVS_EnableEventTypesCmd
**
** Purpose:  This routine sets the given event types to an enabled state across all
**           registered applications
**
** Assumptions and Notes:
**
*/
int32 CFE_EVS_EnableEventTypeCmd(const CFE_EVS_EnableEventType_t *data)
{
   uint32      i;
   const CFE_EVS_BitMaskCmd_Payload_t *CmdPtr = &data->Payload;
   int32  ReturnCode;

   /* Need to check for an out of range bitmask, since our bit masks are only 4 bits */
   if (CmdPtr->BitMask == 0x0 || CmdPtr->BitMask > 0x0F)
   {
       EVS_SendEvent(CFE_EVS_ERR_INVALID_BITMASK_EID, CFE_EVS_EventType_ERROR,
                         "Bit Mask = 0x%08x out of range: CC = %lu",
                         (unsigned int)CmdPtr->BitMask, (long unsigned int)CFE_EVS_ENABLE_EVENT_TYPE_CC);
       ReturnCode = CFE_EVS_INVALID_PARAMETER;
   }
   else
   {
       for (i = 0; i < CFE_PLATFORM_ES_MAX_APPLICATIONS; i++)
       {
           /* Make sure application is registered for event services */
           if (CFE_EVS_GlobalData.AppData[i].RegisterFlag == true)
           {
               EVS_EnableTypes(CmdPtr->BitMask, i);
           }
       }

       EVS_SendEvent(CFE_EVS_ENAEVTTYPE_EID, CFE_EVS_EventType_DEBUG,
               "Enable Event Type Command Received with Event Type Bit Mask = 0x%02x",
               (unsigned int)CmdPtr->BitMask);

       ReturnCode = CFE_SUCCESS;
   }

   return ReturnCode;

} /* End CFE_EVS_EnableEventTypesCmd */


/*
**             Function Prologue
**
** Function Name:      CFE_EVS_DisableEventTypesCmd
**
** Purpose:  This routine sets the given event types to a disabled state across all
**           registered applications
**
** Assumptions and Notes:
**
*/
int32 CFE_EVS_DisableEventTypeCmd(const CFE_EVS_DisableEventType_t *data)
{
   uint32   i;
   const CFE_EVS_BitMaskCmd_Payload_t *CmdPtr = &data->Payload;
   int32  ReturnCode;

   /* Need to check for an out of range bitmask, since our bit masks are only 4 bits */
   if (CmdPtr->BitMask == 0x0 || CmdPtr->BitMask > 0x0F)
   {
       EVS_SendEvent(CFE_EVS_ERR_INVALID_BITMASK_EID, CFE_EVS_EventType_ERROR,
                         "Bit Mask = 0x%08x out of range: CC = %lu",
                         (unsigned int)CmdPtr->BitMask, (long unsigned int)CFE_EVS_DISABLE_EVENT_TYPE_CC);
       ReturnCode = CFE_EVS_INVALID_PARAMETER;
   }

   else
   {
       for (i = 0; i < CFE_PLATFORM_ES_MAX_APPLICATIONS; i++)
       {
           /* Make sure application is registered for event services */
           if (CFE_EVS_GlobalData.AppData[i].RegisterFlag == true)
           {
               EVS_DisableTypes(CmdPtr->BitMask, i);
           }
       }

       EVS_SendEvent(CFE_EVS_DISEVTTYPE_EID, CFE_EVS_EventType_DEBUG,
               "Disable Event Type Command Received with Event Type Bit Mask = 0x%02x",
               (unsigned int)CmdPtr->BitMask);

       ReturnCode = CFE_SUCCESS;
   }

   return ReturnCode;

} /* End CFE_EVS_DisableEventTypesCmd */



/*
**             Function Prologue
**
** Function Name:      CFE_EVS_SetEventFormatModeCmd
**
** Purpose:  This routine sets the Event Format Mode
**
** Assumptions and Notes:
**
*/
int32 CFE_EVS_SetEventFormatModeCmd(const CFE_EVS_SetEventFormatMode_t *data)
{
   const CFE_EVS_SetEventFormatMode_Payload_t *CmdPtr = &data->Payload;
   int32 Status;

   if((CmdPtr->MsgFormat == CFE_EVS_MsgFormat_SHORT) || (CmdPtr->MsgFormat == CFE_EVS_MsgFormat_LONG))
   {
      CFE_EVS_GlobalData.EVS_TlmPkt.Payload.MessageFormatMode = CmdPtr->MsgFormat;

      EVS_SendEvent(CFE_EVS_SETEVTFMTMOD_EID, CFE_EVS_EventType_DEBUG,
                        "Set Event Format Mode Command Received with Mode = 0x%02x",
                        (unsigned int)CmdPtr->MsgFormat);
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

} /* End CFE_EVS_SetEventFormatModeCmd */


/*
**             Function Prologue
**
** Function Name:      CFE_EVS_EnableAppEventTypesCmd
**
** Purpose:  This routine sets the given event type for the given application identifier to an
**           enabled state
**
** Assumptions and Notes:
**
*/
int32 CFE_EVS_EnableAppEventTypeCmd(const CFE_EVS_EnableAppEventType_t *data)
{
   const CFE_EVS_AppNameBitMaskCmd_Payload_t *CmdPtr = &data->Payload;
   uint32  AppID = CFE_EVS_UNDEF_APPID;
   int32   Status;
   char    LocalName[OS_MAX_API_NAME];

   /*
    * Althgouh EVS_GetApplicationInfo() does not require a null terminated argument,
    * the value is passed to EVS_SendEvent which does require termination (normal C string)
    */
   CFE_SB_MessageStringGet(LocalName, (char *)CmdPtr->AppName, NULL, OS_MAX_API_NAME, sizeof(CmdPtr->AppName));

   /* Retrieve application data */
   Status = EVS_GetApplicationInfo(&AppID, LocalName);

   if(Status == CFE_SUCCESS)
   {

        /* Need to check for an out of range bitmask, since our bit masks are only 4 bits */
        if (CmdPtr->BitMask == 0x0 || CmdPtr->BitMask > 0x0F)
        {
            EVS_SendEvent(CFE_EVS_ERR_INVALID_BITMASK_EID, CFE_EVS_EventType_ERROR,
                              "Bit Mask = 0x%08x out of range: CC = %lu",
                              (unsigned int)CmdPtr->BitMask, (long unsigned int)CFE_EVS_ENABLE_APP_EVENT_TYPE_CC);
            Status = CFE_EVS_INVALID_PARAMETER;
        }
        else
        {
            EVS_EnableTypes(CmdPtr->BitMask, AppID);
        }
   }
   else if(Status == CFE_EVS_APP_NOT_REGISTERED)
   {
      EVS_SendEvent(CFE_EVS_ERR_APPNOREGS_EID, CFE_EVS_EventType_ERROR,
                        "%s not registered with EVS: CC = %lu",
                        LocalName, (long unsigned int)CFE_EVS_ENABLE_APP_EVENT_TYPE_CC);
   }
   else if(Status == CFE_EVS_APP_ILLEGAL_APP_ID)
   {
      EVS_SendEvent(CFE_EVS_ERR_ILLAPPIDRANGE_EID, CFE_EVS_EventType_ERROR,
                        "Illegal application ID %d retrieved for %s: CC = %lu",
                        (int)AppID, LocalName, (long unsigned int)CFE_EVS_ENABLE_APP_EVENT_TYPE_CC);
   }
   else
   {
      EVS_SendEvent(CFE_EVS_ERR_NOAPPIDFOUND_EID, CFE_EVS_EventType_ERROR,
                        "Unable to retrieve application ID for %s: CC = %lu",
                        LocalName, (long unsigned int)CFE_EVS_ENABLE_APP_EVENT_TYPE_CC);
   }

   if(Status == CFE_SUCCESS)
   {
      EVS_SendEvent(CFE_EVS_ENAAPPEVTTYPE_EID, CFE_EVS_EventType_DEBUG,
                        "Enable App Event Type Command Received with AppName = %s, EventType Bit Mask = 0x%02x",
                        LocalName, CmdPtr->BitMask);
   }

   return Status;

} /* End CFE_EVS_EnableAppEventTypesCmd */


/*
**             Function Prologue
**
** Function Name:      CFE_EVS_DisableAppEventTypesCmd
**
** Purpose:  This routine sets the given event type for the given application identifier to a
**           disabled state
**
** Assumptions and Notes:
**
*/
int32 CFE_EVS_DisableAppEventTypeCmd(const CFE_EVS_DisableAppEventType_t *data)
{
   uint32 AppID  = CFE_EVS_UNDEF_APPID;
   const CFE_EVS_AppNameBitMaskCmd_Payload_t *CmdPtr = &data->Payload;
   int32  Status;
   char    LocalName[OS_MAX_API_NAME];

   /*
    * Althgouh EVS_GetApplicationInfo() does not require a null terminated argument,
    * the value is passed to EVS_SendEvent which does require termination (normal C string)
    */
   CFE_SB_MessageStringGet(LocalName, (char *)CmdPtr->AppName, NULL, OS_MAX_API_NAME, sizeof(CmdPtr->AppName));

   /* Retreive application data */
   Status = EVS_GetApplicationInfo(&AppID, LocalName);

   if(Status == CFE_SUCCESS)
   {

        /* Need to check for an out of range bitmask, since our bit masks are only 4 bits */
        if (CmdPtr->BitMask == 0x0 || CmdPtr->BitMask > 0x0F)
        {
            EVS_SendEvent(CFE_EVS_ERR_INVALID_BITMASK_EID, CFE_EVS_EventType_ERROR,
                              "Bit Mask = 0x%08x out of range: CC = %lu",
                              (unsigned int)CmdPtr->BitMask, (long unsigned int)CFE_EVS_DISABLE_APP_EVENT_TYPE_CC);
            Status = CFE_EVS_INVALID_PARAMETER;
        }
        else
        {
            EVS_DisableTypes(CmdPtr->BitMask, AppID);
        }
   }
   else if(Status == CFE_EVS_APP_NOT_REGISTERED)
   {
      EVS_SendEvent(CFE_EVS_ERR_APPNOREGS_EID, CFE_EVS_EventType_ERROR,
                        "%s not registered with EVS,: CC = %lu",
                        LocalName,(long unsigned int)CFE_EVS_DISABLE_APP_EVENT_TYPE_CC);
   }
   else if(Status == CFE_EVS_APP_ILLEGAL_APP_ID)
   {
      EVS_SendEvent(CFE_EVS_ERR_ILLAPPIDRANGE_EID, CFE_EVS_EventType_ERROR,
                        "Illegal application ID %d retrieved for %s: CC = %lu",
                        (int)AppID, LocalName, (long unsigned int)CFE_EVS_DISABLE_APP_EVENT_TYPE_CC);
   }
   else
   {
      EVS_SendEvent(CFE_EVS_ERR_NOAPPIDFOUND_EID, CFE_EVS_EventType_ERROR,
                        "Unable to retrieve application ID for %s: CC = %lu",
                        LocalName, (long unsigned int)CFE_EVS_DISABLE_APP_EVENT_TYPE_CC);
   }

   if(Status == CFE_SUCCESS)
   {
      EVS_SendEvent(CFE_EVS_DISAPPENTTYPE_EID, CFE_EVS_EventType_DEBUG,
                        "Disable App Event Type Command Received with AppName = %s, EventType Bit Mask = 0x%02x",
                        LocalName, (unsigned int)CmdPtr->BitMask);
   }

   return Status;

} /* End CFE_EVS_DisableAppEventTypes */


/*
**             Function Prologue
**
** Function Name:      CFE_EVS_EnableAppEventsCmd
**
** Purpose:  This routine enables application events for the given application identifier
**
** Assumptions and Notes:
**
*/
int32 CFE_EVS_EnableAppEventsCmd(const CFE_EVS_EnableAppEvents_t *data)
{
   uint32 AppID  = CFE_EVS_UNDEF_APPID;
   const CFE_EVS_AppNameCmd_Payload_t *CmdPtr = &data->Payload;
   int32  Status;
   char                LocalName[OS_MAX_API_NAME];

   /*
    * Althgouh EVS_GetApplicationInfo() does not require a null terminated argument,
    * the value is passed to EVS_SendEvent which does require termination (normal C string)
    */
   CFE_SB_MessageStringGet(LocalName, (char *)CmdPtr->AppName, NULL, OS_MAX_API_NAME, sizeof(CmdPtr->AppName));

   /* Retrieve application data */
   Status = EVS_GetApplicationInfo(&AppID, LocalName);

   if(Status == CFE_SUCCESS)
   {
        CFE_EVS_GlobalData.AppData[AppID].ActiveFlag = true;

        EVS_SendEvent(CFE_EVS_ENAAPPEVT_EID, CFE_EVS_EventType_DEBUG,
                          "Enable App Events Command Received with AppName = %s",
                          LocalName);
   }
   else if(Status == CFE_EVS_APP_NOT_REGISTERED)
   {
      EVS_SendEvent(CFE_EVS_ERR_APPNOREGS_EID, CFE_EVS_EventType_ERROR,
                        "%s not registered with EVS: CC = %lu",
                        LocalName, (long unsigned int)CFE_EVS_ENABLE_APP_EVENTS_CC);
   }
   else if(Status == CFE_EVS_APP_ILLEGAL_APP_ID)
   {
      EVS_SendEvent(CFE_EVS_ERR_ILLAPPIDRANGE_EID, CFE_EVS_EventType_ERROR,
                        "Illegal application ID %d retrieved for %s: CC = %lu",
                        (int)AppID, LocalName, (long unsigned int)CFE_EVS_ENABLE_APP_EVENTS_CC);
   }
   else
   {
      EVS_SendEvent(CFE_EVS_ERR_NOAPPIDFOUND_EID, CFE_EVS_EventType_ERROR,
                        "Unable to retrieve application ID for %s: CC = %lu",
                        LocalName, (long unsigned int)CFE_EVS_ENABLE_APP_EVENTS_CC);
   }

   return Status;

} /* End EVS_EnableAppEventsCmd */


/*
**             Function Prologue
**
** Function Name:      CFE_EVS_DisableAppEventsCmd
**
** Purpose:  This routine disables application events for the given application identifier
**
** Assumptions and Notes:
**
*/
int32 CFE_EVS_DisableAppEventsCmd(const CFE_EVS_DisableAppEvents_t *data)
{
   uint32 AppID  = CFE_EVS_UNDEF_APPID;
   const CFE_EVS_AppNameCmd_Payload_t *CmdPtr = &data->Payload;
   int32  Status;
   char                LocalName[OS_MAX_API_NAME];

   /*
    * Althgouh EVS_GetApplicationInfo() does not require a null terminated argument,
    * the value is passed to EVS_SendEvent which does require termination (normal C string)
    */
   CFE_SB_MessageStringGet(LocalName, (char *)CmdPtr->AppName, NULL, OS_MAX_API_NAME, sizeof(CmdPtr->AppName));

   /* Retreive application data */
   Status = EVS_GetApplicationInfo(&AppID, LocalName);

   if(Status == CFE_SUCCESS)
   {
       CFE_EVS_GlobalData.AppData[AppID].ActiveFlag = false;

       EVS_SendEvent(CFE_EVS_DISAPPEVT_EID, CFE_EVS_EventType_DEBUG,
               "Disable App Events Command Received with AppName = %s",
               LocalName);
   }
   else if(Status == CFE_EVS_APP_NOT_REGISTERED)
   {
       EVS_SendEvent(CFE_EVS_ERR_APPNOREGS_EID, CFE_EVS_EventType_ERROR,
               "%s not registered with EVS: CC = %lu",
               LocalName,(long unsigned int)CFE_EVS_DISABLE_APP_EVENTS_CC);
   }
   else if(Status == CFE_EVS_APP_ILLEGAL_APP_ID)
   {
       EVS_SendEvent(CFE_EVS_ERR_ILLAPPIDRANGE_EID, CFE_EVS_EventType_ERROR,
               "Illegal application ID %d retrieved for %s: CC = %lu",
               (int)AppID, LocalName,(long unsigned int)CFE_EVS_DISABLE_APP_EVENTS_CC);
   }
   else
   {
       EVS_SendEvent(CFE_EVS_ERR_NOAPPIDFOUND_EID, CFE_EVS_EventType_ERROR,
               "Disable App Events Command: Unable to retrieve application ID for %s: CC = %lu",
               LocalName, (long unsigned int)CFE_EVS_DISABLE_APP_EVENTS_CC);
   }

   return Status;

} /* End CFE_EVS_DisableAppEventsCmd */


/*
**             Function Prologue
**
** Function Name:      CFE_EVS_ResetAppEventCounterCmd
**
** Purpose:  This routine sets the application event counter to zero for the given
**           application identifier
**
** Assumptions and Notes:
**
*/
int32 CFE_EVS_ResetAppCounterCmd(const CFE_EVS_ResetAppCounter_t *data)
{
   uint32 AppID  = CFE_EVS_UNDEF_APPID;
   const CFE_EVS_AppNameCmd_Payload_t *CmdPtr = &data->Payload;
   int32  Status;
   char   LocalName[OS_MAX_API_NAME];

   /*
    * Althgouh EVS_GetApplicationInfo() does not require a null terminated argument,
    * the value is passed to EVS_SendEvent which does require termination (normal C string)
    */
   CFE_SB_MessageStringGet(LocalName, (char *)CmdPtr->AppName, NULL, OS_MAX_API_NAME, sizeof(CmdPtr->AppName));

   /* Retreive application data */
   Status = EVS_GetApplicationInfo(&AppID, LocalName);

   if(Status == CFE_SUCCESS)
   {
       CFE_EVS_GlobalData.AppData[AppID].EventCount = 0;

       EVS_SendEvent(CFE_EVS_RSTEVTCNT_EID, CFE_EVS_EventType_DEBUG,
               "Reset Event Counter Command Received with AppName = %s",
               LocalName);
   }
   else if(Status == CFE_EVS_APP_NOT_REGISTERED)
   {
       EVS_SendEvent(CFE_EVS_ERR_APPNOREGS_EID, CFE_EVS_EventType_ERROR,
               "%s not registered with EVS: CC = %lu",
               LocalName, (long unsigned int)CFE_EVS_RESET_APP_COUNTER_CC);
   }
   else if(Status == CFE_EVS_APP_ILLEGAL_APP_ID)
   {
       EVS_SendEvent(CFE_EVS_ERR_ILLAPPIDRANGE_EID, CFE_EVS_EventType_ERROR,
               "Illegal application ID %d retrieved for %s: CC = %lu",
               (int)AppID, LocalName,(long unsigned int) CFE_EVS_RESET_APP_COUNTER_CC);
   }
   else
   {
       EVS_SendEvent(CFE_EVS_ERR_NOAPPIDFOUND_EID, CFE_EVS_EventType_ERROR,
               "Reset Event Counter Command: Unable to retrieve application ID for %s: CC = %lu",
               LocalName, (long unsigned int)CFE_EVS_RESET_APP_COUNTER_CC);
   }

   return Status;

} /* End CFE_EVS_ResetAppEventCounterCmd */


/*
**             Function Prologue
**
** Function Name:      CFE_EVS_ResetFilterCmd
**
** Purpose:  This routine sets the application event filter counter to zero for the given
**           application identifier and event identifier
**
** Assumptions and Notes:
**
*/
int32 CFE_EVS_ResetFilterCmd(const CFE_EVS_ResetFilter_t *data)
{
   const CFE_EVS_AppNameEventIDCmd_Payload_t *CmdPtr = &data->Payload;
   EVS_BinFilter_t     *FilterPtr;
   uint32               AppID = CFE_EVS_UNDEF_APPID;
   int32                Status;
   EVS_AppData_t       *AppDataPtr;
   char                 LocalName[OS_MAX_API_NAME];

   /*
    * Althgouh EVS_GetApplicationInfo() does not require a null terminated argument,
    * the value is passed to EVS_SendEvent which does require termination (normal C string)
    */
   CFE_SB_MessageStringGet(LocalName, (char *)CmdPtr->AppName, NULL, OS_MAX_API_NAME, sizeof(CmdPtr->AppName));

   /* Retreive application data */
   Status = EVS_GetApplicationInfo(&AppID, LocalName);

   if(Status == CFE_SUCCESS)
   {
      AppDataPtr = &CFE_EVS_GlobalData.AppData[AppID];

      FilterPtr = EVS_FindEventID(CmdPtr->EventID, AppDataPtr->BinFilters);

      if(FilterPtr != NULL)
      {
         FilterPtr->Count = 0;

         EVS_SendEvent(CFE_EVS_RSTFILTER_EID, CFE_EVS_EventType_DEBUG,
                           "Reset Filter Command Received with AppName = %s, EventID = 0x%08x",
                           LocalName, (unsigned int)CmdPtr->EventID);
      }
      else
      {
         EVS_SendEvent(CFE_EVS_ERR_EVTIDNOREGS_EID, CFE_EVS_EventType_ERROR,
                           "%s Event ID %d not registered for filtering: CC = %lu",
                           LocalName, (int)CmdPtr->EventID, (long unsigned int)CFE_EVS_RESET_FILTER_CC);

         Status = CFE_EVS_EVT_NOT_REGISTERED;
      }
   }
   else if(Status == CFE_EVS_APP_NOT_REGISTERED)
   {
      EVS_SendEvent(CFE_EVS_ERR_APPNOREGS_EID, CFE_EVS_EventType_ERROR,
                        "%s not registered with EVS: CC = %lu",
                        LocalName, (long unsigned int)CFE_EVS_RESET_FILTER_CC);
   }
   else if(Status == CFE_EVS_APP_ILLEGAL_APP_ID)
   {
      EVS_SendEvent(CFE_EVS_ERR_ILLAPPIDRANGE_EID, CFE_EVS_EventType_ERROR,
                        "Illegal application ID %d retrieved for %s: CC = %lu",
                        (int)AppID, LocalName, (long unsigned int)CFE_EVS_RESET_FILTER_CC);
   }
   else
   {
      EVS_SendEvent(CFE_EVS_ERR_NOAPPIDFOUND_EID, CFE_EVS_EventType_ERROR,
                        "Unable to retrieve application ID for %s: CC = %lu",
                        LocalName, (long unsigned int)CFE_EVS_RESET_FILTER_CC);
   }

   return Status;

} /* End CFE_EVS_ResetFilterCmd */


/*
**             Function Prologue
**
** Function Name:      CFE_EVS_ResetAllFiltersCmd
**
** Purpose:  This routine sets all application event filter counters to zero for the given
**           application identifier
**
** Assumptions and Notes:
**
*/
int32 CFE_EVS_ResetAllFiltersCmd(const CFE_EVS_ResetAllFilters_t *data)
{
   uint32                    AppID  = CFE_EVS_UNDEF_APPID;
   const CFE_EVS_AppNameCmd_Payload_t *CmdPtr = &data->Payload;
   int32                     Status;
   uint32                    i;
   char                      LocalName[OS_MAX_API_NAME];

   /*
    * Althgouh EVS_GetApplicationInfo() does not require a null terminated argument,
    * the value is passed to EVS_SendEvent which does require termination (normal C string)
    */
   CFE_SB_MessageStringGet(LocalName, (char *)CmdPtr->AppName, NULL, OS_MAX_API_NAME, sizeof(CmdPtr->AppName));

   /* Retreive application data */
   Status = EVS_GetApplicationInfo(&AppID, LocalName);

   if(Status == CFE_SUCCESS)
   {
       for(i=0; i<CFE_PLATFORM_EVS_MAX_EVENT_FILTERS; i++)
       {
           CFE_EVS_GlobalData.AppData[AppID].BinFilters[i].Count = 0;
       }

       EVS_SendEvent(CFE_EVS_RSTALLFILTER_EID, CFE_EVS_EventType_DEBUG,
               "Reset All Filters Command Received with AppName = %s",
               LocalName);
   }
   else if(Status == CFE_EVS_APP_NOT_REGISTERED)
   {
       EVS_SendEvent(CFE_EVS_ERR_APPNOREGS_EID, CFE_EVS_EventType_ERROR,
               "%s not registered with EVS: CC = %lu",
               LocalName, (long unsigned int)CFE_EVS_RESET_ALL_FILTERS_CC);
   }
   else if(Status == CFE_EVS_APP_ILLEGAL_APP_ID)
   {
       EVS_SendEvent(CFE_EVS_ERR_ILLAPPIDRANGE_EID, CFE_EVS_EventType_ERROR,
               "Illegal application ID %d retrieved for %s: CC = %lu",
               (int)AppID, LocalName, (long unsigned int)CFE_EVS_RESET_ALL_FILTERS_CC);
   }
   else
   {
       EVS_SendEvent(CFE_EVS_ERR_NOAPPIDFOUND_EID, CFE_EVS_EventType_ERROR,
               "Unable to retrieve application ID for %s: CC = %lu",
               LocalName, (long unsigned int)CFE_EVS_RESET_ALL_FILTERS_CC);
   }

   return Status;

} /* End CFE_EVS_ResetAllFiltersCmd */

/*
**             Function Prologue
**
** Function Name:      CFE_EVS_AddEventFilterCmd
**
** Purpose:  This routine adds the given event filter for the given application
**           identifier and event identifier.
**
** Assumptions and Notes:
**
*/
int32 CFE_EVS_AddEventFilterCmd(const CFE_EVS_AddEventFilter_t *data)
{
   const CFE_EVS_AppNameEventIDMaskCmd_Payload_t *CmdPtr = &data->Payload;
   EVS_BinFilter_t     *FilterPtr;
   uint32               AppID = CFE_EVS_UNDEF_APPID;
   int32                Status;
   EVS_AppData_t       *AppDataPtr;
   char                 LocalName[OS_MAX_API_NAME];

   /*
    * Althgouh EVS_GetApplicationInfo() does not require a null terminated argument,
    * the value is passed to EVS_SendEvent which does require termination (normal C string)
    */
   CFE_SB_MessageStringGet(LocalName, (char *)CmdPtr->AppName, NULL, OS_MAX_API_NAME, sizeof(CmdPtr->AppName));

   /* Retreive application data */
   Status = EVS_GetApplicationInfo(&AppID, LocalName);

   if(Status == CFE_SUCCESS)
   {
      AppDataPtr = &CFE_EVS_GlobalData.AppData[AppID];

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
               FilterPtr->Mask = CmdPtr->Mask;
               FilterPtr->Count = 0;

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
      }/* end else*/
   } /* end if (Status == CFE_SUCCESS) */

   else if(Status == CFE_EVS_APP_NOT_REGISTERED)
   {
      EVS_SendEvent(CFE_EVS_ERR_APPNOREGS_EID, CFE_EVS_EventType_ERROR,
                        "%s not registered with EVS: CC = %lu",
                        LocalName, (long unsigned int)CFE_EVS_ADD_EVENT_FILTER_CC);
   }
   else if(Status == CFE_EVS_APP_ILLEGAL_APP_ID)
   {
      EVS_SendEvent(CFE_EVS_ERR_ILLAPPIDRANGE_EID, CFE_EVS_EventType_ERROR,
                        "Illegal application ID %d retrieved for %s: CC = %lu",
                        (int)AppID, LocalName, (long unsigned int)CFE_EVS_ADD_EVENT_FILTER_CC);
   }
   else
   {
      EVS_SendEvent(CFE_EVS_ERR_NOAPPIDFOUND_EID, CFE_EVS_EventType_ERROR,
                        "Unable to retrieve application ID for %s: CC = %lu",
                        LocalName,(long unsigned int)CFE_EVS_ADD_EVENT_FILTER_CC);
   }

   return Status;

} /* CFE_End EVS_AddEventFilterCmd */

/*
**             Function Prologue
**
** Function Name:      CFE_EVS_DeleteEventFilterCmd
**
** Purpose:  This routine deletes the event filter for the given application
**           identifer and event identifier
**
** Assumptions and Notes:
**
*/
int32 CFE_EVS_DeleteEventFilterCmd(const CFE_EVS_DeleteEventFilter_t *data)
{
   const CFE_EVS_AppNameEventIDCmd_Payload_t *CmdPtr = &data->Payload;
   EVS_BinFilter_t     *FilterPtr;
   uint32               AppID = CFE_EVS_UNDEF_APPID;
   int32                Status;
   EVS_AppData_t       *AppDataPtr;
   char                 LocalName[OS_MAX_API_NAME];

   /*
    * Althgouh EVS_GetApplicationInfo() does not require a null terminated argument,
    * the value is passed to EVS_SendEvent which does require termination (normal C string)
    */
   CFE_SB_MessageStringGet(LocalName, (char *)CmdPtr->AppName, NULL, OS_MAX_API_NAME, sizeof(CmdPtr->AppName));

   /* Retreive application data */
   Status = EVS_GetApplicationInfo(&AppID, LocalName);

   if(Status == CFE_SUCCESS)
   {
      AppDataPtr = &CFE_EVS_GlobalData.AppData[AppID];

      FilterPtr = EVS_FindEventID(CmdPtr->EventID, AppDataPtr->BinFilters);

      if(FilterPtr != NULL)
      {
         /* Clear Filter Contents */
         FilterPtr->EventID = CFE_EVS_FREE_SLOT;
         FilterPtr->Mask = CFE_EVS_NO_MASK;
         FilterPtr->Count = 0;

         EVS_SendEvent(CFE_EVS_DELFILTER_EID, CFE_EVS_EventType_DEBUG,
                           "Delete Filter Command Received with AppName = %s, EventID = 0x%08x",
                           LocalName, (unsigned int)CmdPtr->EventID);

      }
      else
      {
         EVS_SendEvent(CFE_EVS_ERR_EVTIDNOREGS_EID, CFE_EVS_EventType_ERROR,
                           "%s Event ID %d not registered for filtering: CC = %lu",
                           LocalName, (int)CmdPtr->EventID, (long unsigned int)CFE_EVS_DELETE_EVENT_FILTER_CC);
         Status = CFE_EVS_EVT_NOT_REGISTERED;
      }
   }
   else if(Status == CFE_EVS_APP_NOT_REGISTERED)
   {
      EVS_SendEvent(CFE_EVS_ERR_APPNOREGS_EID, CFE_EVS_EventType_ERROR,
                        "%s not registered with EVS: CC = %lu",
                        LocalName, (long unsigned int)CFE_EVS_DELETE_EVENT_FILTER_CC);
   }
   else if(Status == CFE_EVS_APP_ILLEGAL_APP_ID)
   {
      EVS_SendEvent(CFE_EVS_ERR_ILLAPPIDRANGE_EID, CFE_EVS_EventType_ERROR,
                        "Illegal application ID %d retrieved for %s: CC = %lu",
                        (int)AppID, LocalName, (long unsigned int)CFE_EVS_DELETE_EVENT_FILTER_CC);
   }
   else
   {
      EVS_SendEvent(CFE_EVS_ERR_NOAPPIDFOUND_EID, CFE_EVS_EventType_ERROR,
                        "Unable to retrieve application ID for %s: CC = %lu",
                        LocalName, (long unsigned int)CFE_EVS_DELETE_EVENT_FILTER_CC);
   }

   return Status;

} /* End EVS_DeleteEventFilterCmd */


/*
**             Function Prologue
**
** Function Name:      CFE_EVS_WriteAppDataFileCmd
**
** Purpose:  This routine writes all application data to a file for all applications that
**           have registered with the EVS.  The application data includes the Application ID,
**           Active Flag, Event Count, Event Types Active Flag, and Filter Data.
**
** Assumptions and Notes:
**
*/
int32 CFE_EVS_WriteAppDataFileCmd(const CFE_EVS_WriteAppDataFile_t *data)
{
   int32                             Result;
   int32                             FileHandle;
   int32                             BytesWritten;
   uint32                            EntryCount = 0;
   uint32                            i;
   static CFE_EVS_AppDataFile_t      AppDataFile;
   CFE_FS_Header_t                   FileHdr;
   EVS_AppData_t                    *AppDataPtr;
   const CFE_EVS_AppDataCmd_Payload_t *CmdPtr = &data->Payload;
   char                              LocalName[OS_MAX_PATH_LEN];

   /* Copy the commanded filename into local buffer to ensure size limitation and to allow for modification */
   CFE_SB_MessageStringGet(LocalName, CmdPtr->AppDataFilename, CFE_PLATFORM_EVS_DEFAULT_APP_DATA_FILE,
           OS_MAX_PATH_LEN, sizeof(CmdPtr->AppDataFilename));

   /* Create Application Data File */
   FileHandle = OS_creat(LocalName, OS_WRITE_ONLY);

   if (FileHandle < OS_FS_SUCCESS)
   {
      EVS_SendEvent(CFE_EVS_ERR_CRDATFILE_EID, CFE_EVS_EventType_ERROR,
                   "Write App Data Command Error: OS_creat = 0x%08X, filename = %s",
                    (unsigned int)FileHandle, LocalName);

      Result = FileHandle;
   }
   else
   {
      /* Result will be overridden if everything works */
      Result = CFE_EVS_FILE_WRITE_ERROR;

      /* Initialize cFE file header */
      CFE_FS_InitHeader(&FileHdr, "EVS Application Data File", CFE_FS_SubType_EVS_APPDATA);

      /* Write cFE file header to the App File */
      BytesWritten = CFE_FS_WriteHeader(FileHandle, &FileHdr);

      if (BytesWritten == sizeof(CFE_FS_Header_t))
      {
         for (i = 0; i < CFE_PLATFORM_ES_MAX_APPLICATIONS; i++)
         {
            /* Only have data for apps that are registered */
            if (CFE_EVS_GlobalData.AppData[i].RegisterFlag == true)
            {
               AppDataPtr = &CFE_EVS_GlobalData.AppData[i];

               /* Clear application file data record */
               memset(&AppDataFile, 0, sizeof(CFE_EVS_AppDataFile_t));

               /* Copy application data to application file data record */
               CFE_ES_GetAppName(AppDataFile.AppName, i, OS_MAX_API_NAME);
               AppDataFile.ActiveFlag = AppDataPtr->ActiveFlag;
               AppDataFile.EventCount = AppDataPtr->EventCount;
               AppDataFile.EventTypesActiveFlag = AppDataPtr->EventTypesActiveFlag;

               /* Copy application filter data to application file data record */
               memcpy(AppDataFile.Filters, AppDataPtr->BinFilters,
                              CFE_PLATFORM_EVS_MAX_EVENT_FILTERS * sizeof(EVS_BinFilter_t));

               /* Write application data record to file */
               BytesWritten = OS_write(FileHandle, &AppDataFile, sizeof(CFE_EVS_AppDataFile_t));

               if (BytesWritten == sizeof(CFE_EVS_AppDataFile_t))
               {
                  EntryCount++;
               }
               else
               {
                  EVS_SendEvent(CFE_EVS_ERR_WRDATFILE_EID, CFE_EVS_EventType_ERROR,
                               "Write App Data Command Error: OS_write = 0x%08X, filename = %s",
                                (unsigned int)BytesWritten, LocalName);
                  break;
               }
            }
         }

         /* Process command handler success result */
         if (i == CFE_PLATFORM_ES_MAX_APPLICATIONS)
         {
            EVS_SendEvent(CFE_EVS_WRDAT_EID, CFE_EVS_EventType_DEBUG,
                         "Write App Data Command: %d application data entries written to %s",
                          (int)EntryCount, LocalName);
            Result = CFE_SUCCESS;
         }
      }

      OS_close(FileHandle);
   }

   return(Result);

} /* End CFE_EVS_WriteAppDataFileCmd */


/* End cfe_evs_task */
