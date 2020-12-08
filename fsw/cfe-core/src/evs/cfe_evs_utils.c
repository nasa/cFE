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
**  File: cfe_evs_utils.c
**
**  Title: Event Services Utility functions
**
**  Purpose: This module defines the utility functions of the
**           Event Services Task and API
**
*/

/* Include Files */
#include "cfe_evs.h"         /* EVS library function definitions */
#include "cfe_evs_log.h"     /* EVS local event log definitions */
#include "cfe_evs_task.h"    /* EVS internal definitions */
#include "cfe_evs_utils.h"   /* EVS utility function definitions */

#include <stdio.h>
#include <string.h>

#include "cfe_error.h"       /* cFE error code definitions */
#include "common_types.h"    /* Basic data types */
#include "osapi.h"           /* OS API interface definitions */
#include "cfe_psp.h"          /* cFE PSP glue functions */
#include "cfe_sb.h"          /* Software Bus library function definitions */
#include "cfe_es.h"

/* Local Function Prototypes */
void EVS_SendViaPorts (CFE_EVS_LongEventTlm_t *EVS_PktPtr);
void EVS_OutputPort1 (char *Message);
void EVS_OutputPort2 (char *Message);
void EVS_OutputPort3 (char *Message);
void EVS_OutputPort4 (char *Message);

/* Function Definitions */


/*
**             Function Prologue
**
** Function Name:      EVS_GetAppID
**
** Purpose:  This routine gets and validates the caller's AppID
**
** Assumptions and Notes:
**
*/
EVS_AppData_t *EVS_GetAppDataByID (CFE_ES_ResourceID_t AppID)
{
   uint32 AppIndex;
   EVS_AppData_t *AppDataPtr;

   if (CFE_ES_AppID_ToIndex(AppID, &AppIndex) == CFE_SUCCESS &&
           AppIndex < CFE_PLATFORM_ES_MAX_APPLICATIONS)
   {
       AppDataPtr = &CFE_EVS_GlobalData.AppData[AppIndex];
   }
   else
   {
       AppDataPtr = NULL;
   }

   return(AppDataPtr);

} /* End EVS_GetAppDataByID */

int32 EVS_GetCurrentContext (EVS_AppData_t **AppDataOut, CFE_ES_ResourceID_t *AppIDOut)
{
   CFE_ES_ResourceID_t AppID;
   EVS_AppData_t *AppDataPtr;
   int32 Status;

   /* Get the caller's AppID */
   Status = CFE_ES_GetAppID(&AppID);
   if (Status == CFE_SUCCESS)
   {
       AppDataPtr = EVS_GetAppDataByID (AppID);
   }
   else
   {
       AppDataPtr = NULL;
   }

   if (AppDataPtr == NULL)
   {
       /* use EVS error/status code */
       Status = CFE_EVS_APP_ILLEGAL_APP_ID;
   }

   if (AppIDOut)
   {
       *AppIDOut = AppID;
   }
   if (AppDataOut)
   {
       *AppDataOut = AppDataPtr;
   }

   return Status;

} /* End EVS_GetCurrentContext */


/*
**             Function Prologue
**
** Function Name:      EVS_GetApplicationInfo
**
** Purpose:  This routine returns the application ID and
**           status specifying the validity of the ID
**
** Assumptions and Notes:
**
*/
int32 EVS_GetApplicationInfo (EVS_AppData_t **AppDataOut, const char *pAppName)
{
   int32 Status;
   CFE_ES_ResourceID_t AppID;
   EVS_AppData_t *AppDataPtr;

   Status = CFE_ES_GetAppIDByName(&AppID, pAppName);
   if (Status != CFE_SUCCESS)
   {
       AppDataPtr = NULL;
   }
   else
   {
       AppDataPtr = EVS_GetAppDataByID(AppID);
       if (AppDataPtr == NULL)
       {
           /*
            * should not happen - it means the CFE_ES_GetAppIDByName()
            * returned a success code with an AppID which was in subsequently
            * not accepted by CFE_ES_AppID_ToIndex()
            */
           Status = CFE_EVS_APP_ILLEGAL_APP_ID;
       }
       else if (!EVS_AppDataIsMatch(AppDataPtr, AppID))
       {
           /* Avoid outputting a bad pointer */
           AppDataPtr = NULL;
           Status = CFE_EVS_APP_NOT_REGISTERED;
       }
   }



   *AppDataOut = AppDataPtr;
   return Status;

} /* End EVS_GetApplicationInfo */


/*
**             Function Prologue
**
** Function Name:      EVS_NotRegistered
**
** Purpose:  This routine sends one "not registered" event per application
**
** Assumptions and Notes:
**
*/
int32 EVS_NotRegistered (EVS_AppData_t *AppDataPtr, CFE_ES_ResourceID_t CallerID)
{
   char AppName[OS_MAX_API_NAME];

   /* Send only one "not registered" event per application */
   if ( !CFE_ES_ResourceID_Equal(AppDataPtr->UnregAppID, CallerID) )
   { 
      /* Increment count of "not registered" applications */
      CFE_EVS_GlobalData.EVS_TlmPkt.Payload.UnregisteredAppCounter++;

      /* Indicate that "not registered" event has been sent for this app */
      AppDataPtr->UnregAppID = CallerID;

      /* Get the name of the "not registered" app */
      CFE_ES_GetAppName(AppName, CallerID, sizeof(AppName));

      /* Send the "not registered" event */
      EVS_SendEvent(CFE_EVS_ERR_UNREGISTERED_EVS_APP, CFE_EVS_EventType_ERROR,
                   "App %s not registered with Event Services. Unable to send event.", AppName);

      /* Write the "not registered" info to the system log */
      CFE_ES_WriteToSysLog("App %s not registered with Event Services. Unable to send event.\n", AppName);
   }

   return(CFE_EVS_APP_NOT_REGISTERED);

} /* End EVS_NotRegistered */


/*
**             Function Prologue
**
** Function Name:      EVS_IsFiltered
**
** Purpose:  This routine returns true if the given event identifier and event type
**           is filtered for the given application identifier.  Otherwise a value of
**           false is returned.
**
** Assumptions and Notes:
**
*/
bool EVS_IsFiltered (EVS_AppData_t *AppDataPtr, uint16 EventID, uint16 EventType)
{
   EVS_BinFilter_t *FilterPtr;
   bool             Filtered = false;
   char             AppName[OS_MAX_API_NAME];


   if (AppDataPtr->ActiveFlag == false)
   {
      /* All events are disabled for this application */
      Filtered = true;
   }
   else switch (EventType)
   {
      case CFE_EVS_EventType_DEBUG:

         if ((AppDataPtr->EventTypesActiveFlag & CFE_EVS_DEBUG_BIT) == 0)
         {
            /* Debug events are disabled for this application */
            Filtered = true;
         }
         break;

      case CFE_EVS_EventType_INFORMATION:

         if ((AppDataPtr->EventTypesActiveFlag & CFE_EVS_INFORMATION_BIT) == 0)
         {
            /* Informational events are disabled for this application */
            Filtered = true;
         }
         break;

      case CFE_EVS_EventType_ERROR:

         if ((AppDataPtr->EventTypesActiveFlag & CFE_EVS_ERROR_BIT) == 0)
         {
            /* Error events are disabled for this application */
            Filtered = true;
         }
         break;

      case CFE_EVS_EventType_CRITICAL:

         if ((AppDataPtr->EventTypesActiveFlag & CFE_EVS_CRITICAL_BIT) == 0)
         {
            /* Critical events are disabled for this application */
            Filtered = true;
         }
         break;

      default:

         /* Invalid Event Type */
         Filtered = true;
         break;
   }

   /* Is this type of event enabled for this application? */
   if (Filtered == false)
   {
      FilterPtr = EVS_FindEventID(EventID, AppDataPtr->BinFilters);

      /* Does this event ID have an event filter table entry? */
      if (FilterPtr != NULL)
      {
         if ((FilterPtr->Mask & FilterPtr->Count) != 0)
         {
            /* This iteration of the event ID is filtered */
            Filtered = true;
         }

         if (FilterPtr->Count < CFE_EVS_MAX_FILTER_COUNT)
         {
            /* Maintain event iteration count */
            FilterPtr->Count++;

            /* Is it time to lock this filter? */
            if (FilterPtr->Count == CFE_EVS_MAX_FILTER_COUNT)
            {
               CFE_ES_GetAppName(AppName, EVS_AppDataGetID(AppDataPtr), sizeof(AppName));

               EVS_SendEvent(CFE_EVS_FILTER_MAX_EID, CFE_EVS_EventType_INFORMATION,
                  "Max filter count reached, AppName = %s, EventID = 0x%08x: Filter locked until reset",
                   AppName, (unsigned int)EventID);
            }
         }
      }
   }

   return(Filtered);

} /* End EVS_IsFiltered */


/*
**             Function Prologue
**
** Function Name:      EVS_FindEventID
**
** Purpose:  This routine searches and returns an index to the given Event ID with the
**           given application filter array.
**
** Assumptions and Notes:
**
*/
EVS_BinFilter_t *EVS_FindEventID (int16 EventID, EVS_BinFilter_t *FilterArray)
{
   uint32 i;

   for (i = 0; i < CFE_PLATFORM_EVS_MAX_EVENT_FILTERS; i++)
   {
      if (FilterArray[i].EventID == EventID)
      {
         return(&FilterArray[i]);
      }
   }

   return((EVS_BinFilter_t *) NULL);

} /* End EVS_FindEventID */


/*
**             Function Prologue
**
** Function Name:      EVS_EnableTypes
**
** Purpose:  This routine enables event types selected in BitMask
**
** Assumptions and Notes:
**
*/
void EVS_EnableTypes (EVS_AppData_t *AppDataPtr, uint8 BitMask)
{
   uint8 EventTypeBits = (CFE_EVS_DEBUG_BIT | CFE_EVS_INFORMATION_BIT | CFE_EVS_ERROR_BIT | CFE_EVS_CRITICAL_BIT);

   /* Enable selected event type bits from bitmask */
   AppDataPtr->EventTypesActiveFlag |= (BitMask & EventTypeBits);

} /* End EVS_EnableTypes */


/*
**             Function Prologue
**
** Function Name:      EVS_DisableTypes
**
** Purpose:  This routine disables event types selected in BitMask
**
** Assumptions and Notes:
**
*/
void EVS_DisableTypes (EVS_AppData_t *AppDataPtr, uint8 BitMask)
{
   uint8 EventTypeBits = (CFE_EVS_DEBUG_BIT | CFE_EVS_INFORMATION_BIT | CFE_EVS_ERROR_BIT | CFE_EVS_CRITICAL_BIT);

   /* Disable selected event type bits from bitmask */
   AppDataPtr->EventTypesActiveFlag &= ~(BitMask & EventTypeBits);

} /* End EVS_DisableTypes */


/*
**             Function Prologue
**
** Function Name:      EVS_GenerateEventTelemetry
**
** Purpose:  This routine sends an EVS event message out the software bus and all
**           enabled output ports
**
** Assumptions and Notes:
**           This always generates a "long" style message for logging purposes.
**           If configured for long events the same message is sent on the software bus as well.
**           If configured for short events, a separate short message is generated using a subset
**           of the information from the long message.
*/
void EVS_GenerateEventTelemetry(EVS_AppData_t *AppDataPtr, uint16 EventID, uint16 EventType, const CFE_TIME_SysTime_t *TimeStamp, const char *MsgSpec, va_list ArgPtr)
{
    CFE_EVS_LongEventTlm_t   LongEventTlm;      /* The "long" flavor is always generated, as this is what is logged */
    CFE_EVS_ShortEventTlm_t  ShortEventTlm;     /* The "short" flavor is only generated if selected */
    int                      ExpandedLength;

    /* Initialize EVS event packets */
    CFE_MSG_Init(&LongEventTlm.TlmHeader.Msg, CFE_SB_ValueToMsgId(CFE_EVS_LONG_EVENT_MSG_MID),
                 sizeof(LongEventTlm));
    LongEventTlm.Payload.PacketID.EventID   = EventID;
    LongEventTlm.Payload.PacketID.EventType = EventType;

    /* vsnprintf() returns the total expanded length of the formatted string */
    /* vsnprintf() copies and zero terminates portion that fits in the buffer */
    ExpandedLength = vsnprintf((char *)LongEventTlm.Payload.Message, sizeof(LongEventTlm.Payload.Message), MsgSpec, ArgPtr);

    /* Were any characters truncated in the buffer? */
    if (ExpandedLength >= sizeof(LongEventTlm.Payload.Message))
    {
       /* Mark character before zero terminator to indicate truncation */
       LongEventTlm.Payload.Message[sizeof(LongEventTlm.Payload.Message) - 2] = CFE_EVS_MSG_TRUNCATED;
       CFE_EVS_GlobalData.EVS_TlmPkt.Payload.MessageTruncCounter++;
    }

    /* Obtain task and system information */
    CFE_ES_GetAppName((char *)LongEventTlm.Payload.PacketID.AppName, EVS_AppDataGetID(AppDataPtr),
            sizeof(LongEventTlm.Payload.PacketID.AppName));
    LongEventTlm.Payload.PacketID.SpacecraftID = CFE_PSP_GetSpacecraftId();
    LongEventTlm.Payload.PacketID.ProcessorID  = CFE_PSP_GetProcessorId();

    /* Set the packet timestamp */
    CFE_MSG_SetMsgTime(&LongEventTlm.TlmHeader.Msg, *TimeStamp);

    /* Write event to the event log */
    EVS_AddLog(&LongEventTlm);

    /* Send event via selected ports */
    EVS_SendViaPorts(&LongEventTlm);

    if (CFE_EVS_GlobalData.EVS_TlmPkt.Payload.MessageFormatMode == CFE_EVS_MsgFormat_LONG)
    {
        /* Send long event via SoftwareBus */
        CFE_SB_TransmitMsg(&LongEventTlm.TlmHeader.Msg, true);
    }
    else if (CFE_EVS_GlobalData.EVS_TlmPkt.Payload.MessageFormatMode == CFE_EVS_MsgFormat_SHORT)
    {
        /*
         * Initialize the short format event message from data that was already
         * gathered in the long format message (short format is a subset)
         *
         * This goes out on a separate message ID.
         */
        CFE_MSG_Init(&ShortEventTlm.TlmHeader.Msg, CFE_SB_ValueToMsgId(CFE_EVS_SHORT_EVENT_MSG_MID),
                     sizeof(ShortEventTlm));
        CFE_MSG_SetMsgTime(&ShortEventTlm.TlmHeader.Msg, *TimeStamp);
        ShortEventTlm.Payload.PacketID = LongEventTlm.Payload.PacketID;
        CFE_SB_TransmitMsg(&ShortEventTlm.TlmHeader.Msg, true);
    }

    /* Increment message send counters (prevent rollover) */
    if (CFE_EVS_GlobalData.EVS_TlmPkt.Payload.MessageSendCounter < CFE_EVS_MAX_EVENT_SEND_COUNT)
    {
       CFE_EVS_GlobalData.EVS_TlmPkt.Payload.MessageSendCounter++;
    }

    if (AppDataPtr->EventCount < CFE_EVS_MAX_EVENT_SEND_COUNT)
    {
       AppDataPtr->EventCount++;
    }

} /* End EVS_GenerateEventTelemetry */


/*
**             Function Prologue
**
** Function Name:      EVS_SendViaPorts
**
** Purpose:  This routine sends a string event message out all enabled
**           output ports
**
** Assumptions and Notes:
*/
void EVS_SendViaPorts (CFE_EVS_LongEventTlm_t *EVS_PktPtr)
{
   char PortMessage[CFE_EVS_MAX_PORT_MSG_LENGTH];

   if (((CFE_EVS_GlobalData.EVS_TlmPkt.Payload.OutputPort & CFE_EVS_PORT1_BIT) >> 0) == true)
   {
      /* Copy event message to string format */
      snprintf(PortMessage, CFE_EVS_MAX_PORT_MSG_LENGTH, "EVS Port1 %u/%u/%s %u: %s", (unsigned int) EVS_PktPtr->Payload.PacketID.SpacecraftID,
                                                        (unsigned int) EVS_PktPtr->Payload.PacketID.ProcessorID,
                                                        EVS_PktPtr->Payload.PacketID.AppName,
                                                        (unsigned int) EVS_PktPtr->Payload.PacketID.EventID,
                                                        EVS_PktPtr->Payload.Message);
      /* Send string event out port #1 */
      EVS_OutputPort1(PortMessage);
   }

   if (((CFE_EVS_GlobalData.EVS_TlmPkt.Payload.OutputPort & CFE_EVS_PORT2_BIT) >> 1) == true)
   {
      /* Copy event message to string format */
      snprintf(PortMessage, CFE_EVS_MAX_PORT_MSG_LENGTH, "EVS Port2 %u/%u/%s %u: %s", (unsigned int) EVS_PktPtr->Payload.PacketID.SpacecraftID,
                                                        (unsigned int) EVS_PktPtr->Payload.PacketID.ProcessorID,
                                                        EVS_PktPtr->Payload.PacketID.AppName,
                                                        (unsigned int) EVS_PktPtr->Payload.PacketID.EventID,
                                                        EVS_PktPtr->Payload.Message);
      /* Send string event out port #2 */
      EVS_OutputPort2(PortMessage);
   }

   if (((CFE_EVS_GlobalData.EVS_TlmPkt.Payload.OutputPort & CFE_EVS_PORT3_BIT) >> 2) == true)
   {
      /* Copy event message to string format */
      snprintf(PortMessage, CFE_EVS_MAX_PORT_MSG_LENGTH, "EVS Port3 %u/%u/%s %u: %s", (unsigned int) EVS_PktPtr->Payload.PacketID.SpacecraftID,
                                                        (unsigned int) EVS_PktPtr->Payload.PacketID.ProcessorID,
                                                        EVS_PktPtr->Payload.PacketID.AppName,
                                                        (unsigned int) EVS_PktPtr->Payload.PacketID.EventID,
                                                        EVS_PktPtr->Payload.Message);
      /* Send string event out port #3 */
      EVS_OutputPort3(PortMessage);
   }

   if (((CFE_EVS_GlobalData.EVS_TlmPkt.Payload.OutputPort & CFE_EVS_PORT4_BIT) >> 3) == true)
   {
      /* Copy event message to string format */
      snprintf(PortMessage, CFE_EVS_MAX_PORT_MSG_LENGTH, "EVS Port4 %u/%u/%s %u: %s", (unsigned int) EVS_PktPtr->Payload.PacketID.SpacecraftID,
                                                        (unsigned int) EVS_PktPtr->Payload.PacketID.ProcessorID,
                                                        EVS_PktPtr->Payload.PacketID.AppName,
                                                        (unsigned int) EVS_PktPtr->Payload.PacketID.EventID,
                                                        EVS_PktPtr->Payload.Message);
      /* Send string event out port #4 */
      EVS_OutputPort4(PortMessage);
   }

} /* End SendViaPorts */


/*
**             Function Prologue
**
** Function Name:      EVS_OutputPort1
**
** Purpose:  This routine sends the input message string out
**           EVS port 1
**
** Assumptions and Notes:
**
*/
void EVS_OutputPort1 (char *Message)
{
    OS_printf("%s\n", Message);

} /* End ES_OutputPort1 */


/*
**             Function Prologue
**
** Function Name:      EVS_OutputPort2
**
** Purpose:  This routine sends the input message string out
**           EVS port 2
**
** Assumptions and Notes:
**
*/
void EVS_OutputPort2 (char *Message)
{
   OS_printf("%s\n", Message);

} /* End ES_OutputPort2 */


/*
**             Function Prologue
**
** Function Name:      EVS_OutputPort3
**
** Purpose:  This routine sends the input message string out
**           EVS port 3
**
** Assumptions and Notes:
**
*/
void EVS_OutputPort3 (char *Message)
{
   OS_printf("%s\n", Message);

} /* End ES_OutputPort3 */


/*
**             Function Prologue
**
** Function Name:      EVS_OutputPort4
**
** Purpose:  This routine sends the input message string out
**           EVS port 4
**
** Assumptions and Notes:
**
*/
void EVS_OutputPort4 (char *Message)
{
   OS_printf("%s\n", Message);

} /* End ES_OutputPort4 */


/*
**             Function Prologue
**
** Function Name:      EVS_SendEvent
**
** Purpose:  This routine allows EVS to send events without having to verify
**           that the caller has a valid AppID and has registered with EVS.
**           This routine also does not need to acquire the mutex semaphore,
**           which can be time consuming on some platforms.
**
** Assumptions and Notes:
*/
int32 EVS_SendEvent (uint16 EventID, uint16 EventType, const char *Spec, ... )
{
   CFE_TIME_SysTime_t Time;
   va_list            Ptr;
   EVS_AppData_t      *AppDataPtr;

   /*
    * Must check that EVS_AppID is valid, which can happen if this is called
    * by some other thread before CFE_EVS_TaskInit() runs
    */
   AppDataPtr = EVS_GetAppDataByID(CFE_EVS_GlobalData.EVS_AppID);

   /* Unlikely, but possible that an EVS event filter was added by command */
   if ( EVS_AppDataIsMatch(AppDataPtr, CFE_EVS_GlobalData.EVS_AppID) &&
           EVS_IsFiltered(AppDataPtr, EventID, EventType) == false)
   {
      /* Get current spacecraft time */
      Time = CFE_TIME_GetTime();

      /* Send the event packets */
      va_start(Ptr, Spec);
      EVS_GenerateEventTelemetry(AppDataPtr, EventID, EventType, &Time, Spec, Ptr);
      va_end(Ptr);
   }

   return(CFE_SUCCESS);

} /* End EVS_SendEvent */


/* End cfe_evs_utils */
