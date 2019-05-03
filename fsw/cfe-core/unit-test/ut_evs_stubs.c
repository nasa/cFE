/*
**      GSC-18128-1, "Core Flight Executive Version 6.6"
**
**      Copyright (c) 2006-2019 United States Government as represented by
**      the Administrator of the National Aeronautics and Space Administration.
**      All Rights Reserved.
**
**      Licensed under the Apache License, Version 2.0 (the "License");
**      you may not use this file except in compliance with the License.
**      You may obtain a copy of the License at
**
**        http://www.apache.org/licenses/LICENSE-2.0
**
**      Unless required by applicable law or agreed to in writing, software
**      distributed under the License is distributed on an "AS IS" BASIS,
**      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**      See the License for the specific language governing permissions and
**      limitations under the License.
**
** File:
** $Id: ut_evs_stubs.c 1.7 2014/05/28 09:21:49GMT-05:00 wmoleski Exp  $
**
** Purpose:
** Unit test stubs for Event Service routines
** 
** Notes:
** Minimal work is done, only what is required for unit testing
**
*/

/*
** Includes
*/
#include <string.h>
#include "cfe_evs.h"
#include "common_types.h"
#include "cfe_error.h"
#include "cfe_evs_task.h"
#include "ut_stubs.h"

/*
** Macro definitions
*/
#ifdef OSP_ARINC653
#define UT_OFFSET_CFE_EVS_SENDEVENT 4
#define UT_OFFSET_CFE_SENDEVENTWITHAPPID 5

#define UT_BREAK_CFE_EVS_SENDEVENT 5
#define UT_BREAK_CFE_SENDEVENTWITHAPPID 4

#define UT_SKIP_CFE_EVS_SENDEVENT 54
#define UT_SKIP_CFE_SENDEVENTWITHAPPID 56
#endif

/*
** External global variables
*/
extern char cMsg[];

extern UT_SetRtn_t EVS_SendEventRtn;
extern UT_SetRtn_t EVS_RegisterRtn;
extern UT_SetRtn_t EVSCleanUpRtn;
extern UT_SetRtn_t SendMsgEventIDRtn;

/*
** Functions
*/
/*****************************************************************************/
/**
** \brief CFE_EVS_EarlyInit stub function
**
** \par Description
**        This function is used to mimic the response of the cFE EVS function
**        CFE_EVS_EarlyInit.  It always returns CFE_SUCCESS when called.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_EVS_EarlyInit(void)
{
    return CFE_SUCCESS;
}

/*****************************************************************************/
/**
** \brief CFE_EVS_TaskMain stub function
**
** \par Description
**        This function is used as a placeholder for the cFE EVS function
**        CFE_EVS_TaskMain.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void CFE_EVS_TaskMain(void)
{
}

/*****************************************************************************/
/**
** \brief CFE_EVS_SendEvent stub function
**
** \par Description
**        This function is used to mimic the response of the cFE EVS function
**        CFE_EVS_SendEvent.  The user can adjust the response by setting
**        the values in the EVS_SendEventRtn structure prior to this function
**        being called.  If the value EVS_SendEventRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value EVS_SendEventRtn.value.
**        CFE_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_EVS_SendEvent(uint16 EventID,
                        uint16 EventType,
                        const char *Spec,
                        ...)
{
    int32   status = CFE_SUCCESS;
    boolean flag = FALSE;
#ifdef UT_VERBOSE
    char    BigBuf[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH*2];
    va_list Ptr;

#ifdef OSP_ARINC653
    va_start(Ptr, Spec, UT_OFFSET_CFE_EVS_SENDEVENT,
             UT_BREAK_CFE_EVS_SENDEVENT, UT_SKIP_CFE_EVS_SENDEVENT);
#else
    va_start(Ptr, Spec);
#endif

    vsnprintf(BigBuf, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, Spec, Ptr);
    va_end(Ptr);
#endif

    if (EVS_SendEventRtn.count > 0)
    {
        EVS_SendEventRtn.count--;

        if (EVS_SendEventRtn.count == 0)
        {
            status = EVS_SendEventRtn.value;
            flag = TRUE;
        }
    }

    if (flag == FALSE)
    {
        UT_AddEventToHistory(EventID);
        SendMsgEventIDRtn.value = EventID;
#ifdef UT_VERBOSE
        snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "  CFE_EVS_SendEvent: %u, %u - %s",
                 EventID, EventType, BigBuf);
        UT_Text(cMsg);
#endif
    }

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_EVS_SendTimedEvent stub function
**
** \par Description
**        This function is used to mimic the response of the cFE EVS function
**        CFE_EVS_SendTimedEvent.  It always returns CFE_SUCCESS when called.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_EVS_SendTimedEvent(CFE_TIME_SysTime_t Time,
                             uint16 EventID,
                             uint16 EventType,
                             const char *Spec,
                             ...)
{
    return CFE_SUCCESS;
}

/*****************************************************************************/
/**
** \brief CFE_EVS_Register stub function
**
** \par Description
**        This function is used to mimic the response of the cFE EVS function
**        CFE_EVS_Register.  The user can adjust the response by setting
**        the values in the EVS_RegisterRtn structure prior to this function
**        being called.  If the value EVS_RegisterRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value EVS_RegisterRtn.value.
**        CFE_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_EVS_Register(void *Filters,
                       uint16 NumEventFilters,
                       uint16 FilterScheme)
{
    int32   status = CFE_SUCCESS;
#ifdef UT_VERBOSE
    boolean flag = FALSE;
#endif

    if (EVS_RegisterRtn.count > 0)
    {
        EVS_RegisterRtn.count--;
        if (EVS_RegisterRtn.count == 0)
        {
            status = EVS_RegisterRtn.value;
#ifdef UT_VERBOSE
            flag = TRUE;
            snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
                     "  CFE_EVS_Register called: %ld", EVS_RegisterRtn.value);
            UT_Text(cMsg);
#endif
        }
    }

#ifdef UT_VERBOSE
    if (flag == FALSE)
    {
        UT_Text("  CFE_EVS_Register called");
    }
#endif

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_EVS_SendEventWithAppID stub function
**
** \par Description
**        This function is used to mimic the response of the cFE EVS function
**        CFE_EVS_SendEventWithAppID.  The event ID, EventID, is added to the
**        unit test' event history.  The number of events can be retrieved by
**        the unit tests for comparison to expected values in order to detect
**        correct functionality.  The user can adjust the response by setting
**        the values in the EVS_SendEventRtn structure prior to this function
**        being called.  If the value EVS_SendEventRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value EVS_SendEventRtn.value.
**        CFE_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_EVS_SendEventWithAppID(uint16 EventID,
                                 uint16 EventType,
                                 uint32 AppID,
                                 const char *Spec,
                                 ...)
{
    int32   status = CFE_SUCCESS;
#ifdef UT_VERBOSE
    boolean flag = FALSE;
    char    BigBuf[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    va_list Ptr;

#ifdef OSP_ARINC653
    va_start(Ptr, Spec, UT_OFFSET_CFE_SENDEVENTWITHAPPID,
             UT_BREAK_CFE_SENDEVENTWITHAPPID, UT_SKIP_CFE_SENDEVENTWITHAPPID);
#else
    va_start(Ptr, Spec);
#endif
    vsnprintf(BigBuf, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, Spec, Ptr);
    va_end(Ptr);
#endif
    UT_AddEventToHistory(EventID);
  
    if (EVS_SendEventRtn.count > 0)
    {
        EVS_SendEventRtn.count--;

        if (EVS_SendEventRtn.count == 0)
        {
            status = EVS_SendEventRtn.value;
#ifdef UT_VERBOSE
            flag = TRUE;
#endif
        }
    }

#ifdef UT_VERBOSE
    if (flag == FALSE)
    {
        snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "  CFE_EVS_SendEvent from app %lu: %u, %u - %s",
                 AppID, EventID, EventType, BigBuf);
        UT_Text(cMsg);
    }
#endif

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_EVS_CleanUpApp stub function
**
** \par Description
**        This function is used to mimic the response of the cFE EVS function
**        CFE_EVS_CleanUpApp.  The user can adjust the response by setting
**        the values in the EVSCleanUpRtn structure prior to this function
**        being called.  If the value EVSCleanUpRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value EVSCleanUpRtn.value.
**        CFE_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_EVS_CleanUpApp(uint32 AppId)
{
    int32 status = CFE_SUCCESS;

    if (EVSCleanUpRtn.count > 0)
    {
      EVSCleanUpRtn.count--;

      if (EVSCleanUpRtn.count == 0)
      {
          status = EVSCleanUpRtn.value;
      }
    }

    return status;
}
