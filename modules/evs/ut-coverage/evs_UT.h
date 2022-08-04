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
 *    EVS unit test header
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

#ifndef EVS_UT_H
#define EVS_UT_H

/*
** Includes
*/
#include <string.h>
#include "cfe_evs_module_all.h"
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
******************************************************************************/
void Test_InvalidCmd(void);

/*****************************************************************************/
/**
** \brief Test squelching of app events
**
** \par Description
**        This function tests squelching of app events that exceeds threshold
**        of events sent per second.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_Squelching(void);

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
******************************************************************************/
void Test_Misc(void);

#endif /* EVS_UT_H */
