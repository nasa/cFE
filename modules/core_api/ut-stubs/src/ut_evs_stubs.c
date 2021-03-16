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
** File: ut_evs_stubs.c
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

#include "utstubs.h"
#include "uttools.h"

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
    int32 status;

    status = UT_DEFAULT_IMPL(CFE_EVS_EarlyInit);

    return status;
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
    UT_DEFAULT_IMPL(CFE_EVS_TaskMain);
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
int32 CFE_EVS_SendEvent(uint16 EventID, uint16 EventType, const char *Spec, ...)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_EVS_SendEvent), EventID);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_EVS_SendEvent), EventType);
    UT_Stub_RegisterContext(UT_KEY(CFE_EVS_SendEvent), Spec);

    UtDebug("CFE_EVS_SendEvent: %u - %s", EventID, Spec);

    int32   status;
    va_list va;

    va_start(va, Spec);
    status = UT_DEFAULT_IMPL_VARARGS(CFE_EVS_SendEvent, va);
    va_end(va);

    if (status >= 0)
    {
        UT_Stub_CopyFromLocal(UT_KEY(CFE_EVS_SendEvent), (uint8 *)&EventID, sizeof(EventID));
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
int32 CFE_EVS_SendTimedEvent(CFE_TIME_SysTime_t Time, uint16 EventID, uint16 EventType, const char *Spec, ...)
{
    /*
     * NOTE: These args are out of order so that Arg[0] and Arg[1] will
     * be the same as they are for other EVS calls.  This keeps it
     * compatible with old/existing UT hook routines.
     * Newly-implemented hooks should use the name-based argument
     * retrieval so it is independent of the order.
     */
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_EVS_SendTimedEvent), EventID);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_EVS_SendTimedEvent), EventType);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_EVS_SendTimedEvent), Time);
    UT_Stub_RegisterContext(UT_KEY(CFE_EVS_SendTimedEvent), Spec);

    int32   status;
    va_list va;

    UtDebug("CFE_EVS_SendTimedEvent: %u - %s", EventID, Spec);

    va_start(va, Spec);
    status = UT_DefaultStubImplWithArgs(__func__, UT_KEY(CFE_EVS_SendTimedEvent), CFE_SUCCESS, va);
    va_end(va);

    if (status >= 0)
    {
        UT_Stub_CopyFromLocal(UT_KEY(CFE_EVS_SendTimedEvent), (uint8 *)&EventID, sizeof(EventID));
    }

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
int32 CFE_EVS_Register(const void *Filters, uint16 NumFilteredEvents, uint16 FilterScheme)
{
    UT_Stub_RegisterContext(UT_KEY(CFE_EVS_Register), Filters);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_EVS_Register), NumFilteredEvents);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_EVS_Register), FilterScheme);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_EVS_Register);

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
int32 CFE_EVS_SendEventWithAppID(uint16 EventID, uint16 EventType, CFE_ES_AppId_t AppID, const char *Spec, ...)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_EVS_SendEventWithAppID), EventID);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_EVS_SendEventWithAppID), EventType);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_EVS_SendEventWithAppID), AppID);
    UT_Stub_RegisterContext(UT_KEY(CFE_EVS_SendEventWithAppID), Spec);

    int32   status;
    va_list va;

    UtDebug("CFE_EVS_SendEventWithAppID: %u - %s", EventID, Spec);

    va_start(va, Spec);
    status = UT_DefaultStubImplWithArgs(__func__, UT_KEY(CFE_EVS_SendEventWithAppID), CFE_SUCCESS, va);
    va_end(va);

    if (status >= 0)
    {
        UT_Stub_CopyFromLocal(UT_KEY(CFE_EVS_SendEventWithAppID), (uint8 *)&EventID, sizeof(EventID));
    }

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
int32 CFE_EVS_CleanUpApp(CFE_ES_AppId_t AppId)
{
    int32 status;

    status = UT_DEFAULT_IMPL(CFE_EVS_CleanUpApp);

    return status;
}

int32 CFE_EVS_ResetAllFilters(void)
{
    int32 status;

    status = UT_DEFAULT_IMPL(CFE_EVS_ResetAllFilters);

    return status;
}

int32 CFE_EVS_ResetFilter(int16 EventID)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_EVS_ResetFilter), EventID);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_EVS_ResetFilter);

    return status;
}

int32 CFE_EVS_Unregister(void)
{
    int32 status;

    status = UT_DEFAULT_IMPL(CFE_EVS_Unregister);

    return status;
}
