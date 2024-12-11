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
**  File: cfe_evs_utils.c
**
**  Title: Event Services Utility functions
**
**  Purpose: This module defines the utility functions of the
**           Event Services Task and API
**
*/

/* Include Files */
#include "cfe_evs_module_all.h" /* All EVS internal definitions and API */
#include "cfe_evs_utils.h"

#include <stdio.h>
#include <string.h>

/* Local Function Prototypes */
void EVS_SendViaPorts(CFE_EVS_LongEventTlm_t *EVS_PktPtr);
void EVS_OutputPort(uint8 PortNum, char *Message);

/* Function Definitions */

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
EVS_AppData_t *EVS_GetAppDataByID(CFE_ES_AppId_t AppID)
{
    uint32         AppIndex;
    EVS_AppData_t *AppDataPtr;

    if (CFE_ES_AppID_ToIndex(AppID, &AppIndex) == CFE_SUCCESS && AppIndex < CFE_PLATFORM_ES_MAX_APPLICATIONS)
    {
        AppDataPtr = &CFE_EVS_Global.AppData[AppIndex];
    }
    else
    {
        AppDataPtr = NULL;
    }

    return AppDataPtr;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 EVS_GetCurrentContext(EVS_AppData_t **AppDataOut, CFE_ES_AppId_t *AppIDOut)
{
    CFE_ES_AppId_t AppID;
    EVS_AppData_t *AppDataPtr;
    int32          Status;

    /* Get the caller's AppID */
    Status = CFE_ES_GetAppID(&AppID);
    if (Status == CFE_SUCCESS)
    {
        AppDataPtr = EVS_GetAppDataByID(AppID);
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
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 EVS_GetApplicationInfo(EVS_AppData_t **AppDataOut, const char *pAppName)
{
    int32          Status;
    CFE_ES_AppId_t AppID;
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
            Status     = CFE_EVS_APP_NOT_REGISTERED;
        }
    }

    *AppDataOut = AppDataPtr;
    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 EVS_NotRegistered(EVS_AppData_t *AppDataPtr, CFE_ES_AppId_t CallerID)
{
    char AppName[OS_MAX_API_NAME];

    /* Send only one "not registered" event per application */
    if (!CFE_RESOURCEID_TEST_EQUAL(AppDataPtr->UnregAppID, CallerID))
    {
        /* Increment count of "not registered" applications */
        CFE_EVS_Global.EVS_TlmPkt.Payload.UnregisteredAppCounter++;

        /* Indicate that "not registered" event has been sent for this app */
        AppDataPtr->UnregAppID = CallerID;

        /* Get the name of the "not registered" app */
        CFE_ES_GetAppName(AppName, CallerID, sizeof(AppName));

        /* Send the "not registered" event */
        EVS_SendEvent(CFE_EVS_ERR_UNREGISTERED_EVS_APP, CFE_EVS_EventType_ERROR,
                      "App %s not registered with Event Services. Unable to send event.", AppName);

        /* Write the "not registered" info to the system log */
        CFE_ES_WriteToSysLog("%s: App %s not registered with Event Services. Unable to send event.\n", __func__,
                             AppName);
    }

    return CFE_EVS_APP_NOT_REGISTERED;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
bool EVS_IsFiltered(EVS_AppData_t *AppDataPtr, uint16 EventID, CFE_EVS_EventType_Enum_t EventType)
{
    EVS_BinFilter_t *FilterPtr;
    bool             Filtered = false;
    char             AppName[OS_MAX_API_NAME];

    if (AppDataPtr->ActiveFlag == false)
    {
        /* All events are disabled for this application */
        Filtered = true;
    }
    else
        switch (EventType)
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

    return Filtered;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
bool EVS_CheckAndIncrementSquelchTokens(EVS_AppData_t *AppDataPtr)
{
    bool      NotSquelched     = true;
    bool      SendSquelchEvent = false;
    OS_time_t CurrentTime      = {0};
    int64     DeltaTimeMs;
    int64     CreditCount;
    char      AppName[OS_MAX_API_NAME];

    /* Set maximum token credits to burst size */
    const int32 UPPER_THRESHOLD = CFE_EVS_Global.EVS_EventBurstMax * 1000;
    /*
     * Set lower threshold to stop decrementing
     * Make this -CFE_PLATFORM_EVS_MAX_APP_EVENT_BURST to add some hysteresis
     * Events will resume (CFE_PLATFORM_EVS_MAX_APP_EVENT_BURST /
     * CFE_PLATFORM_EVS_APP_EVENTS_PER_SEC + 1 /
     * CFE_PLATFORM_EVS_APP_EVENTS_PER_SEC) seconds after flooding stops if
     * saturated
     */
    const int32 LOWER_THRESHOLD = -(int32)CFE_EVS_Global.EVS_EventBurstMax * 1000;

    /*
     * Set this to 1000 to avoid integer division while computing CreditCount
     */
    const int32 EVENT_COST = 1000;

    if (CFE_EVS_Global.EVS_EventBurstMax != 0)
    {
        /*
         * We use a timer here since configurations are not guaranteed to send EVS HK wakeups at 1Hz
         * Use a non-settable timer to prevent this from breaking w/ time changes
         */
        OS_MutSemTake(CFE_EVS_Global.EVS_SharedDataMutexID);
        CFE_PSP_GetTime(&CurrentTime);
        DeltaTimeMs = OS_TimeGetTotalMilliseconds(OS_TimeSubtract(CurrentTime, AppDataPtr->LastSquelchCreditableTime));

        /* Calculate how many tokens to credit in elapsed time since last creditable event */
        CreditCount = DeltaTimeMs * CFE_PLATFORM_EVS_APP_EVENTS_PER_SEC;

        /*
         * Don't immediately credit < 1 event worth of credits; defer until
         * enough time that CreditCount > EVENT_COST
         *
         * This prevents condition where credits would creep down slowly
         * through the range which squelch event messages are emitted causing
         * those events to be spammed instead, defeating the suppression.
         */
        if (CreditCount >= EVENT_COST)
        {
            /* Update last squelch returned time if we credited any tokens */
            AppDataPtr->LastSquelchCreditableTime = CurrentTime;

            /*
             * Add Credits, to a maximum of UPPER_THRESHOLD
             * Shouldn't rollover, as calculations are done in int64 space due to
             * promotion rules then bounded before demotion
             */
            if (AppDataPtr->SquelchTokens + CreditCount > UPPER_THRESHOLD)
            {
                AppDataPtr->SquelchTokens = UPPER_THRESHOLD;
            }
            else
            {
                AppDataPtr->SquelchTokens += (int32)CreditCount;
            }
        }

        if (AppDataPtr->SquelchTokens <= 0)
        {
            if (AppDataPtr->SquelchedCount < CFE_EVS_MAX_SQUELCH_COUNT)
            {
                AppDataPtr->SquelchedCount++;
            }
            NotSquelched = false;

            /*
             * Send squelch event message if cross threshold. This has to be a
             * range between -EVENT_COST and 0 due to non-whole event-cost credits being
             * returned allowing 0 to be skipped over. This is solved by
             * checking a range and ensuring EVENT_COST credits are returned at minimum.
             */
            if (AppDataPtr->SquelchTokens > -EVENT_COST && CreditCount < EVENT_COST)
            {
                /* Set flag and send event later, since we still own mutex */
                SendSquelchEvent = true;
            }
        }

        /*
         * Subtract event cost
         */
        if (AppDataPtr->SquelchTokens - EVENT_COST < LOWER_THRESHOLD)
        {
            AppDataPtr->SquelchTokens = LOWER_THRESHOLD;
        }
        else
        {
            AppDataPtr->SquelchTokens -= EVENT_COST;
        }

        OS_MutSemGive(CFE_EVS_Global.EVS_SharedDataMutexID);

        if (SendSquelchEvent)
        {
            CFE_ES_GetAppName(AppName, EVS_AppDataGetID(AppDataPtr), sizeof(AppName));
            EVS_SendEvent(CFE_EVS_SQUELCHED_ERR_EID, CFE_EVS_EventType_ERROR, "Events squelched, AppName = %s",
                          AppName);
        }
    }
    return NotSquelched;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
EVS_BinFilter_t *EVS_FindEventID(uint16 EventID, EVS_BinFilter_t *FilterArray)
{
    uint32 i;

    for (i = 0; i < CFE_PLATFORM_EVS_MAX_EVENT_FILTERS; i++)
    {
        if (FilterArray[i].EventID == EventID)
        {
            return &FilterArray[i];
        }
    }

    return (EVS_BinFilter_t *)NULL;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void EVS_EnableTypes(EVS_AppData_t *AppDataPtr, uint8 BitMask)
{
    uint8 EventTypeBits = (CFE_EVS_DEBUG_BIT | CFE_EVS_INFORMATION_BIT | CFE_EVS_ERROR_BIT | CFE_EVS_CRITICAL_BIT);

    /* Enable selected event type bits from bitmask */
    AppDataPtr->EventTypesActiveFlag |= (BitMask & EventTypeBits);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void EVS_DisableTypes(EVS_AppData_t *AppDataPtr, uint8 BitMask)
{
    uint8 EventTypeBits = (CFE_EVS_DEBUG_BIT | CFE_EVS_INFORMATION_BIT | CFE_EVS_ERROR_BIT | CFE_EVS_CRITICAL_BIT);

    /* Disable selected event type bits from bitmask */
    AppDataPtr->EventTypesActiveFlag &= ~(BitMask & EventTypeBits);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void EVS_GenerateEventTelemetry(EVS_AppData_t *AppDataPtr, uint16 EventID, CFE_EVS_EventType_Enum_t EventType,
                                const CFE_TIME_SysTime_t *TimeStamp, const char *MsgSpec, va_list ArgPtr)
{
    CFE_EVS_LongEventTlm_t  LongEventTlm;  /* The "long" flavor is always generated, as this is what is logged */
    CFE_EVS_ShortEventTlm_t ShortEventTlm; /* The "short" flavor is only generated if selected */
    int                     ExpandedLength;

    memset(&LongEventTlm, 0, sizeof(LongEventTlm));
    memset(&ShortEventTlm, 0, sizeof(ShortEventTlm));

    /* Initialize EVS event packets */
    CFE_MSG_Init(CFE_MSG_PTR(LongEventTlm.TelemetryHeader), CFE_SB_ValueToMsgId(CFE_EVS_LONG_EVENT_MSG_MID),
                 sizeof(LongEventTlm));
    LongEventTlm.Payload.PacketID.EventID   = EventID;
    LongEventTlm.Payload.PacketID.EventType = EventType;

    /* vsnprintf() returns the total expanded length of the formatted string */
    /* vsnprintf() copies and zero terminates portion that fits in the buffer */
    ExpandedLength =
        vsnprintf((char *)LongEventTlm.Payload.Message, sizeof(LongEventTlm.Payload.Message), MsgSpec, ArgPtr);

    /*
     * If vsnprintf is bigger than message size, mark with truncation character
     * Note negative returns (error from vsnprintf) will just leave the message as-is
     */
    if (ExpandedLength >= (int)sizeof(LongEventTlm.Payload.Message))
    {
        /* Mark character before zero terminator to indicate truncation */
        LongEventTlm.Payload.Message[sizeof(LongEventTlm.Payload.Message) - 2] = CFE_EVS_MSG_TRUNCATED;
        CFE_EVS_Global.EVS_TlmPkt.Payload.MessageTruncCounter++;
    }

    /* Obtain task and system information */
    CFE_ES_GetAppName((char *)LongEventTlm.Payload.PacketID.AppName, EVS_AppDataGetID(AppDataPtr),
                      sizeof(LongEventTlm.Payload.PacketID.AppName));
    LongEventTlm.Payload.PacketID.SpacecraftID = CFE_PSP_GetSpacecraftId();
    LongEventTlm.Payload.PacketID.ProcessorID  = CFE_PSP_GetProcessorId();

    /* Set the packet timestamp */
    CFE_MSG_SetMsgTime(CFE_MSG_PTR(LongEventTlm.TelemetryHeader), *TimeStamp);

    /* Write event to the event log */
    EVS_AddLog(&LongEventTlm);

    /* Send event via selected ports */
    EVS_SendViaPorts(&LongEventTlm);

    if (CFE_EVS_Global.EVS_TlmPkt.Payload.MessageFormatMode == CFE_EVS_MsgFormat_LONG)
    {
        /* Send long event via SoftwareBus */
        CFE_SB_TransmitMsg(CFE_MSG_PTR(LongEventTlm.TelemetryHeader), true);
    }
    else if (CFE_EVS_Global.EVS_TlmPkt.Payload.MessageFormatMode == CFE_EVS_MsgFormat_SHORT)
    {
        /*
         * Initialize the short format event message from data that was already
         * gathered in the long format message (short format is a subset)
         *
         * This goes out on a separate message ID.
         */
        CFE_MSG_Init(CFE_MSG_PTR(ShortEventTlm.TelemetryHeader), CFE_SB_ValueToMsgId(CFE_EVS_SHORT_EVENT_MSG_MID),
                     sizeof(ShortEventTlm));
        CFE_MSG_SetMsgTime(CFE_MSG_PTR(ShortEventTlm.TelemetryHeader), *TimeStamp);
        ShortEventTlm.Payload.PacketID = LongEventTlm.Payload.PacketID;
        CFE_SB_TransmitMsg(CFE_MSG_PTR(ShortEventTlm.TelemetryHeader), true);
    }

    /* Increment message send counters (prevent rollover) */
    if (CFE_EVS_Global.EVS_TlmPkt.Payload.MessageSendCounter < CFE_EVS_MAX_EVENT_SEND_COUNT)
    {
        CFE_EVS_Global.EVS_TlmPkt.Payload.MessageSendCounter++;
    }

    if (AppDataPtr->EventCount < CFE_EVS_MAX_EVENT_SEND_COUNT)
    {
        AppDataPtr->EventCount++;
    }
}

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 * This routine sends a string event message out all enabled
 * output ports
 *
 *-----------------------------------------------------------------*/
void EVS_SendViaPorts(CFE_EVS_LongEventTlm_t *EVS_PktPtr)
{
    char               PortMessage[CFE_EVS_MAX_PORT_MSG_LENGTH];
    char               TimeBuffer[CFE_TIME_PRINTED_STRING_SIZE];
    CFE_TIME_SysTime_t PktTime = {0};

    CFE_MSG_GetMsgTime(CFE_MSG_PTR(EVS_PktPtr->TelemetryHeader), &PktTime);
    CFE_TIME_Print(TimeBuffer, PktTime);

    /* SAD: No need to check snprintf return; CFE_EVS_MAX_PORT_MSG_LENGTH is sized to accommodate buffer limits */
    snprintf(PortMessage, sizeof(PortMessage), "%s %u/%u/%s %u: %s", TimeBuffer,
             (unsigned int)EVS_PktPtr->Payload.PacketID.SpacecraftID,
             (unsigned int)EVS_PktPtr->Payload.PacketID.ProcessorID, EVS_PktPtr->Payload.PacketID.AppName,
             (unsigned int)EVS_PktPtr->Payload.PacketID.EventID, EVS_PktPtr->Payload.Message);

    if (CFE_EVS_Global.EVS_TlmPkt.Payload.OutputPort & CFE_EVS_PORT1_BIT)
    {
        /* Send string event out port #1 */
        EVS_OutputPort(1, PortMessage);
    }

    if (CFE_EVS_Global.EVS_TlmPkt.Payload.OutputPort & CFE_EVS_PORT2_BIT)
    {
        /* Send string event out port #2 */
        EVS_OutputPort(2, PortMessage);
    }

    if (CFE_EVS_Global.EVS_TlmPkt.Payload.OutputPort & CFE_EVS_PORT3_BIT)
    {
        /* Send string event out port #3 */
        EVS_OutputPort(3, PortMessage);
    }

    if (CFE_EVS_Global.EVS_TlmPkt.Payload.OutputPort & CFE_EVS_PORT4_BIT)
    {
        /* Send string event out port #4 */
        EVS_OutputPort(4, PortMessage);
    }
}

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 *-----------------------------------------------------------------*/
void EVS_OutputPort(uint8 PortNum, char *Message)
{
    OS_printf("EVS Port%u %s\n", PortNum, Message);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 EVS_SendEvent(uint16 EventID, CFE_EVS_EventType_Enum_t EventType, const char *Spec, ...)
{
    CFE_TIME_SysTime_t Time;
    va_list            Ptr;
    EVS_AppData_t *    AppDataPtr;

    /*
     * Must check that EVS_AppID is valid, which can happen if this is called
     * by some other thread before CFE_EVS_TaskInit() runs
     */
    AppDataPtr = EVS_GetAppDataByID(CFE_EVS_Global.EVS_AppID);

    /* Unlikely, but possible that an EVS event filter was added by command */
    /* Note that we do not squelch events coming from EVS to prevent event recursion,
     * and EVS is assumed to be "well-behaved" */
    if (EVS_AppDataIsMatch(AppDataPtr, CFE_EVS_Global.EVS_AppID) &&
        EVS_IsFiltered(AppDataPtr, EventID, EventType) == false)
    {
        /* Get current spacecraft time */
        Time = CFE_TIME_GetTime();

        /* Send the event packets */
        va_start(Ptr, Spec);
        EVS_GenerateEventTelemetry(AppDataPtr, EventID, EventType, &Time, Spec, Ptr);
        va_end(Ptr);
    }

    return CFE_SUCCESS;
}
