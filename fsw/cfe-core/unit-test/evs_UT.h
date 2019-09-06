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
**    evs_UT.h
**
** Purpose:
**    EVS unit test header
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
#ifndef _evs_UT_h_
#define _evs_UT_h_

/*
** Includes
*/
#include <string.h>
#include "cfe.h"
#include "common_types.h"
#include "osapi.h"
#include "cfe_evs.h"
#include "cfe_evs_log.h"
#include "cfe_evs_task.h"
#include "cfe_evs_utils.h"
#include "cfe_sb.h"
#include "cfe_es.h"
#include "cfe_time.h"
#include "ut_support.h"

/* EVS unit test functions */
/*****************************************************************************/
/**
** \brief Initializes the EVS for subsequent tests and performs tests on the
**        initialization and main functions
**
** \par Description
**        This function configures the EVS for subsequent tests and must be
**        called, once, prior to any subsequent test routines.  Additionally,
**        this routine performs tests on the EVS early initialization, task
**        initialization, and main functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.  
**
** \sa #UT_Text, UT_InitData, #UT_SetSizeofESResetArea, #UT_SetRtnCode
** \sa #CFE_EVS_EarlyInit, #UT_Report, #CFE_EVS_TaskMain
** \sa #UT_SetStatusBSPResetArea, #UT_SetAppID, #UT_SendMsg
** \sa #UT_SetSBTotalMsgLen
**
******************************************************************************/
void Test_Init(void);

/*****************************************************************************/
/**
** \brief Test API function response to an illegal application ID
**
** \par Description
**        This function tests the response to an illegal application ID.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.  
**
** \sa #UT_Text, #UT_InitData, #UT_SetAppID, #UT_Report, #CFE_EVS_Register
** \sa #CFE_EVS_SendEvent, #CFE_EVS_SendTimedEvent, #CFE_EVS_SendEventWithAppID
** \sa #CFE_EVS_ResetFilter, #CFE_EVS_ResetAllFilters, #CFE_EVS_CleanUpApp
** \sa #CFE_EVS_EnableAppEventTypesCmd, #CFE_EVS_DisablePortsCmd
**
******************************************************************************/
void Test_IllegalAppID(void);

/*****************************************************************************/
/**
** \brief Test API function response to an unregistered application
**
** \par Description
**        This function tests the response to an unregistered application.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.  
**
** \sa #UT_Text, #UT_InitData, #CFE_EVS_Unregister, #UT_Report
** \sa #CFE_EVS_SendEvent, #CFE_EVS_ResetFilter, #CFE_EVS_ResetAllFilters
** \sa #CFE_EVS_SendEventWithAppID, #CFE_EVS_SendTimedEvent
** \sa #CFE_EVS_CleanUpApp, #CFE_EVS_Register
**
******************************************************************************/
void Test_UnregisteredApp(void);

/*****************************************************************************/
/**
** \brief Test filter registration related calls
**
** \par Description
**        This function tests the filter registration related calls.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.  
**
** \sa #UT_Text, #UT_InitData, #UT_Report, #CFE_EVS_Register
** \sa #UT_SetPutPoolFail, #UT_SetRtnCode, #CFE_EVS_Unregister
** \sa #CFE_EVS_SendEvent, #CFE_ES_GetAppID
**
******************************************************************************/
void Test_FilterRegistration(void);

/*****************************************************************************/
/**
** \brief Test reset filter API calls
**
** \par Description
**        This function tests the reset filter API calls.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.  
**
** \sa #UT_Text, #UT_InitData, #UT_Report, #CFE_EVS_Register
** \sa #CFE_EVS_ResetFilter, #CFE_EVS_ResetAllFilters
**
******************************************************************************/
void Test_FilterReset(void);

/*****************************************************************************/
/**
** \brief Test long and short format events, and event strings
**        greater than the maximum length allowed
**
** \par Description
**        This function tests long and short format events, and event strings
**        greater than the maximum length allowed.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.  
**
** \sa #UT_Text, #UT_InitData, #UT_SetSBTotalMsgLen, #UT_SendMsg, #UT_Report
** \sa #CFE_EVS_SendEvent, #UT_SetRtnCode, #CFE_EVS_SendTimedEvent
** \sa #CFE_EVS_SendEventWithAppID, #CFE_EVS_EnableAppEventTypesCmd
** \sa #CFE_EVS_SetEventFormatModeCmd
**
******************************************************************************/
void Test_Format(void);

/*****************************************************************************/
/**
** \brief Test enable/disable of port outputs
**
** \par Description
**        This function tests enable/disable of port outputs.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.  
**
** \sa #UT_Text, #UT_InitData, #UT_SetRtnCode, #UT_SetSBTotalMsgLen
** \sa #UT_SendMsg, #UT_Report, #CFE_EVS_SendEvent, #CFE_EVS_EnablePortsCmd
** \sa #CFE_EVS_DisablePortsCmd
**
******************************************************************************/
void Test_Ports(void);

/*****************************************************************************/
/**
** \brief Test event logging
**
** \par Description
**        This function tests event logging.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.  
**
** \sa #UT_Text, #UT_InitData, #UT_SetSBTotalMsgLen, #UT_SendMsg
** \sa #UT_Report, #UT_SetSizeofESResetArea, #UT_SetRtnCode
** \sa #CFE_PSP_GetResetArea, #CFE_EVS_SendEvent, #UT_SetOSFail
** \sa #CFE_EVS_SetLogModeCmd, #CFE_EVS_WriteLogDataFileCmd, #EVS_ClearLog
** \sa #CFE_EVS_ProcessGroundCommand
**
******************************************************************************/
void Test_Logging(void);

/*****************************************************************************/
/**
** \brief Test writing application data
**
** \par Description
**        This function tests writing application data.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.  
**
** \sa #UT_Text, #UT_InitData, #UT_SetSBTotalMsgLen, #UT_SendMsg
** \sa #UT_Report, #UT_SetOSFail, #CFE_EVS_EnableAppEventTypesCmd
** \sa #CFE_EVS_ResetCountersCmd, #CFE_EVS_WriteAppDataFileCmd
**
******************************************************************************/
void Test_WriteApp(void);

/*****************************************************************************/
/**
** \brief Test commands with bad application names (unknown, illegal,
**        unregistered)
**
** \par Description
**        This function tests the commands with bad application names
**        (unknown, illegal, unregistered).
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.  
**
** \sa #UT_Text, #UT_InitData, #UT_SetSBTotalMsgLen, #UT_SendMsg
** \sa #UT_Report, #CFE_EVS_DisableAppEventTypesCmd
** \sa #CFE_EVS_EnableAppEventTypesCmd, #CFE_EVS_EnableAppEventsCmd
** \sa #CFE_EVS_DisableAppEventsCmd, #CFE_EVS_ResetAppEventCounterCmd
** \sa #CFE_EVS_AddEventFilterCmd, #CFE_EVS_DeleteEventFilterCmd
** \sa #CFE_EVS_SetFilterMaskCmd, #CFE_EVS_ResetFilterCmd
** \sa #CFE_EVS_ResetAllFiltersCmd, #CFE_EVS_DisableAppEventTypesCmd
**
******************************************************************************/
void Test_BadAppCmd(void);

/*****************************************************************************/
/**
** \brief Test event commands (and reset counter)
**
** \par Description
**        This function tests the event commands (and reset counter).
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.  
**
** \sa #UT_Text, #UT_InitData, #UT_SetSBTotalMsgLen, #UT_SendMsg
** \sa #CFE_EVS_SendEvent, #UT_Report, #CFE_EVS_DisableAppEventTypesCmd
** \sa #CFE_EVS_EnableAppEventTypesCmd, #CFE_EVS_DisableAppEventsCmd
** \sa #CFE_EVS_EnableAppEventsCmd, #CFE_EVS_DisableEventTypesCmd
** \sa #CFE_EVS_EnableEventTypesCmd, #CFE_EVS_ResetAppEventCounterCmd
**
******************************************************************************/
void Test_EventCmd(void);

/*****************************************************************************/
/**
** \brief Test filter commands
**
** \par Description
**        This function tests the .
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.  
**
** \sa #UT_Text, #UT_InitData, #UT_SetSBTotalMsgLen, #UT_SendMsg
** \sa #UT_Report, #CFE_EVS_Register, #CFE_EVS_EnableAppEventTypesCmd
** \sa #CFE_EVS_DeleteEventFilterCmd, #CFE_EVS_SetFilterMaskCmd
** \sa #CFE_EVS_ResetFilterCmd, #CFE_EVS_ResetAllFiltersCmd
** \sa #CFE_EVS_AddEventFilterCmd
**
******************************************************************************/
void Test_FilterCmd(void);

/*****************************************************************************/
/**
** \brief Test commands with invalid command, command packet, and
**        command length
**
** \par Description
**        This function tests the commands with invalid command, command
**        packet, and command length.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.  
**
** \sa #UT_Text, #UT_InitData, #UT_SetSBTotalMsgLen, #UT_SendMsg
** \sa #UT_Report, #CFE_EVS_ProcessGroundCommand, #CFE_EVS_VerifyCmdLength
**
******************************************************************************/
void Test_InvalidCmd(void);

/*****************************************************************************/
/**
** \brief Test miscellaneous functionality
**
** \par Description
**        This function tests miscellaneous functionality.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.  
**
** \sa #UT_Text, #UT_InitData, #UT_Report, #EVS_GetApplicationInfo
** \sa #UT_SetSBTotalMsgLen, #UT_SendMsg, #CFE_EVS_CleanUpApp
** \sa #CFE_EVS_Register, #CFE_EVS_WriteLogDataFileCmd, #CFE_EVS_SetLogModeCmd
** \sa #CFE_EVS_ReportHousekeepingCmd
**
******************************************************************************/
void Test_Misc(void);

#endif /* _evs_UT_h_ */
