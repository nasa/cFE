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
** File:
**    time_UT.h
**
** Purpose:
**    TIME unit test header
**
** References:
**    1. cFE Application Developers Guide
**    2. unit test standard 092503
**    3. C Coding Standard 102904
**
** Notes:
**    1. This is unit test code only, not for use in flight
**
*/
#ifndef _time_UT_h_
#define _time_UT_h_

/*
** Includes
*/
#include <string.h>
#include "cfe_platform_cfg.h"
#include "cfe_sb.h"
#include "cfe_time.h"
#include "cfe_time_msg.h"
#include "cfe_time_utils.h"
#include "common_types.h"
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
**
** \sa #UT_Text, #UT_InitData, CFE_TIME_TaskMain, #UT_Report, #UT_SetRtnCode
**
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
**
** \sa #UT_Text, #CFE_TIME_EarlyInit, #UT_InitData, #UT_Report,
** \sa #CFE_TIME_TaskInit, #UT_SetRtnCode
**
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
**
** \sa #UT_Text, #UT_InitData, #UT_SetBSP_Time, #CFE_TIME_Print,
** \sa #CFE_TIME_GetMET, #UT_Report, #CFE_TIME_GetMETseconds,
** \sa #CFE_TIME_GetMETsubsecs, #CFE_TIME_Micro2SubSecs,
** \sa #CFE_TIME_GetLeapSeconds, #CFE_TIME_GetTAI, #CFE_TIME_GetUTC,
** \sa #CFE_TIME_GetSTCF, #CFE_TIME_GetClockState, #CFE_TIME_GetClockInfo
**
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
**
** \sa #UT_Text, #UT_InitData, #CFE_TIME_Add, #UT_Report, #CFE_TIME_Subtract,
** \sa #CFE_TIME_Compare
**
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
**
** \sa #UT_Text, #UT_InitData, #CFE_TIME_Print, #CFE_TIME_MET2SCTime,
** \sa #UT_Report, #CFE_TIME_Sub2MicroSecs, #CFE_TIME_Micro2SubSecs
**
******************************************************************************/
void Test_ConvertTime(void);

/*****************************************************************************/
/**
** \brief Test function for converting cFE seconds to file system (FS) seconds
**        and vice versa
**
** \par Description
**        This function tests the function for converting cFE seconds to file
**        system (FS) seconds and vice versa.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #UT_Report, #CFE_TIME_CFE2FSSeconds,
** \sa #CFE_TIME_FS2CFESeconds
**
******************************************************************************/
void Test_ConvertCFEFS(void);

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
**
** \sa #UT_Text, #UT_InitData, #CFE_TIME_Print, #UT_Report
**
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
**
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
**
** \sa #UT_Text, #UT_InitData, #CFE_TIME_RegisterSynchCallback, #UT_Report
**
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
**
** \sa #Test_RegisterSyncCallback
**
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
**
** \sa #UT_Text, #UT_InitData, #UT_SetBSP_Time, #CFE_TIME_ExternalTone,
** \sa #UT_Report
**
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
**
** \sa #UT_Text, #UT_InitData, #CFE_TIME_ExternalMET, #UT_Report,
** \sa #CFE_TIME_ExternalGPS, #CFE_TIME_ExternalTime
**
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
**
** \sa #UT_Text, #UT_InitData, #UT_SendMsg, #UT_Report, #UT_SetBSP_Time
**
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
**
** \sa #UT_Text, #UT_InitData, #UT_SetStatusBSPResetArea,
** \sa #CFE_TIME_QueryResetVars, #CFE_TIME_UpdateResetVars, #UT_Report
**
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
**
** \sa #UT_Text, #UT_InitData, #UT_Report, #CFE_TIME_CalculateState,
** \sa #CFE_TIME_SetState, #CFE_TIME_GetStateFlags
**
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
**
** \sa #UT_Text, #UT_InitData, #UT_SetBSP_Time, #CFE_TIME_GetReference,
** \sa #UT_Report
**
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
**
** \sa #UT_Text, #UT_InitData, #CFE_TIME_SetState, #UT_SetBSP_Time,
** \sa #UT_Report, #CFE_TIME_ToneSend, #CFE_TIME_ToneVerify, CFE_TIME_SetState,
**
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
**
** \sa #UT_Text, #UT_InitData, #CFE_TIME_Set1HzAdj, #CFE_TIME_Local1HzISR,
** \sa #UT_Report, #UT_SetBSP_Time, #UT_SetBinSemFail, #CFE_TIME_Local1HzTask,
** \sa #CFE_TIME_Tone1HzTask
**
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
**
** \sa #UT_Text, #UT_InitData, #CFE_TIME_UnregisterSynchCallback, #UT_Report
**
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
**
** \sa #UT_Text, #UT_InitData, #Test_RegisterSyncCallback,
** \sa #CFE_TIME_CleanUpApp, #CFE_TIME_UnregisterSynchCallback, #UT_Report
**
******************************************************************************/
void Test_CleanUpApp(void);

#endif /* _time_ut_h_ */
