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

/**
 * @file
 *
 * Purpose:
 *    TIME unit test header
 *
 * References:
 *    1. cFE Application Developers Guide
 *    2. unit test standard 092503
 *    3. C Coding Standard 102904
 *
 * Notes:
 *    1. This is unit test code only, not for use in flight
 *
 */

#ifndef TIME_UT_H
#define TIME_UT_H

/*
** Includes
*/
#include <string.h>
#include "cfe_time_module_all.h"
#include "ut_support.h"

/*
** Function prototypes
*/

/* TIME unit test functions */
/*****************************************************************************/
/**
** \brief Test task entry point and main process loop
**
** \par Description
**        This function tests the task entry point and main process loop.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_Main(void);

/*****************************************************************************/
/**
** \brief Test API and time task initialization (must be called prior to
**        remaining tests)
**
** \par Description
**        This function tests API and time task initialization (must be called
**        prior to remaining tests).
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_Init(void);

/*****************************************************************************/
/**
** \brief Test functions to retrieve time values
**
** \par Description
**        This function tests the functions to retrieve time values.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_GetTime(void);

/*****************************************************************************/
/**
** \brief Test operations on time (add, subtract, compare)
**
** \par Description
**        This function tests the operations on time (add, subtract, compare).
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_TimeOp(void);

/*****************************************************************************/
/**
** \brief Test time conversion functions
**
** \par Description
**        This function tests the time conversion functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_ConvertTime(void);

/*****************************************************************************/
/**
** \brief Test function for creating a text string representing the date
**        and time
**
** \par Description
**        This function tests the function for creating a text string
**        representing the date and time.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_Print(void);

/*****************************************************************************/
/**
** \brief Test function for use with register and unregister synch callback
**        API tests
**
** \par Description
**        This function tests the function for use with register and unregister
**        synch callback API tests.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function returns CFE_SUCCESS
******************************************************************************/
int32 ut_time_MyCallbackFunc(void);

/*****************************************************************************/
/**
** \brief Test registering a synchronization callback function
**
** \par Description
**        This function tests the registering a synchronization callback
**        function.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in] reportResults  Set to true to report test results for calls to
**                           CFE_TIME_RegisterSynchCallback.  Set to false
**                           to perform the function without reporting (this
**                           function is used by other tests so reporting
**                           is only needed once).
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_RegisterSyncCallback(bool reportResults);

/*****************************************************************************/
/**
** \brief Test registering a synchronization callback function; report results
**
** \par Description
**        This function tests the registering a synchronization callback
**        function.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_RegisterSyncCallbackTrue(void);

/*****************************************************************************/
/**
** \brief Test external tone signal detection
**
** \par Description
**        This function tests external tone signal detection.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_ExternalTone(void);

/*****************************************************************************/
/**
** \brief Test setting time data from an external source
**
** \par Description
**        This function tests setting time data from an external source.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_External(void);

/*****************************************************************************/
/**
** \brief Test processing command pipe messages
**
** \par Description
**        This function tests processing command pipe messages.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_PipeCmds(void);

/*****************************************************************************/
/**
** \brief Test the reset area functionality
**
** \par Description
**        This function tests the reset area functionality.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_ResetArea(void);

/*****************************************************************************/
/**
** \brief Test time state functions
**
** \par Description
**        This function tests the time state functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_State(void);

/*****************************************************************************/
/**
** \brief Test getting reference data (time at "tone")
**
** \par Description
**        This function tests getting reference data (time at "tone").
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_GetReference(void);

/*****************************************************************************/
/**
** \brief Test send tone, and validate tone and data packet functions
**
** \par Description
**        This function tests the send tone, and validate tone and data packet
**        functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_Tone(void);

/*****************************************************************************/
/**
** \brief Test the 1Hz STCF adjustment, local interrupt service request and
**        task (non-tone), and tone task functions
**
** \par Description
**        This function tests the 1Hz STCF adjustment, local interrupt service
**        request and task (non-tone), and tone task functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_1Hz(void);

/*****************************************************************************/
/**
** \brief Test unregistering synchronization callback function
**
** \par Description
**        This function tests unregistering synchronization callback function.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_UnregisterSynchCallback(void);

/*****************************************************************************/
/**
** \brief Test function to free resources associated with an application
**
** \par Description
**        This function tests freeing resources associated with an application.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CleanUpApp(void);

#endif /* TIME_UT_H */
