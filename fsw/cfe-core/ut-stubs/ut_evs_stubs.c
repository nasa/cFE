/*
**  GSC-18128-1, "Core Flight Executive Version 6.6"
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
#include "cfe.h"
#include "cfe_platform_cfg.h"
#include "utstubs.h"

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
int32 CFE_EVS_SendEvent(uint16 EventID,
                        uint16 EventType,
                        const char *Spec,
                        ...)
{
    int32 status;

    UT_Stub_RegisterContext(UT_KEY(CFE_EVS_SendEvent), &EventID);
    status = UT_DEFAULT_IMPL(CFE_EVS_SendEvent);

    if (status >= 0)
    {
        UT_Stub_CopyFromLocal(UT_KEY(CFE_EVS_SendEvent), (uint8*)&EventID, sizeof(EventID));
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
    int32 status;

    UT_Stub_RegisterContext(UT_KEY(CFE_EVS_SendEvent), &EventID);
    status = UT_DEFAULT_IMPL(CFE_EVS_SendEvent);

    if (status >= 0)
    {
        UT_Stub_CopyFromLocal(UT_KEY(CFE_EVS_SendTimedEvent), (uint8*)&EventID, sizeof(EventID));
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
int32 CFE_EVS_Register(void *Filters,
                       uint16 NumEventFilters,
                       uint16 FilterScheme)
{
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
int32 CFE_EVS_SendEventWithAppID(uint16 EventID,
                                 uint16 EventType,
                                 uint32 AppID,
                                 const char *Spec,
                                 ...)
{
    int32 status;

    UT_Stub_RegisterContext(UT_KEY(CFE_EVS_SendEvent), &EventID);
    status = UT_DEFAULT_IMPL(CFE_EVS_SendEventWithAppID);

    if (status >= 0)
    {
        UT_Stub_CopyFromLocal(UT_KEY(CFE_EVS_SendEventWithAppID), (uint8*)&EventID, sizeof(EventID));
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
int32 CFE_EVS_CleanUpApp(uint32 AppId)
{
    int32 status;

    status = UT_DEFAULT_IMPL(CFE_EVS_CleanUpApp);

    return status;
}

UT_DEFAULT_STUB(CFE_EVS_ResetAllFilters, ( void ))
