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
 *    ES unit test header
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

#ifndef ES_UT_H
#define ES_UT_H

/*
** Macro definitions
*/
/* Maximum size of startup script buffer (sized for lines * 80 chars/line) */
#define MAX_STARTUP_SCRIPT (80 * 5)

/*
** Includes
*/
#include <string.h>
#include "cfe_es_module_all.h"
#include "cfe_time_core_internal.h"
#include "cfe_tbl_core_internal.h"
#include "cfe_evs_core_internal.h"
#include "cfe_sb_core_internal.h"
#include "cfe_version.h"
#include "ut_support.h"
#include "ut_osprintf_stubs.h"

/* ES unit test functions */
/*****************************************************************************/
/**
** \brief Initialize executive services (ES) unit tests and begin testing the
**        main ES startup function
**
** \par Description
**        This function initializes the unit test variables for subsequent
**        tests. It then performs a normal ES main startup, which constitutes
**        test #1.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void TestInit(void);

/*****************************************************************************/
/**
** \brief Perform tests for non-normal ES main startup and provide coverage
**        for remaining functions in cfe_es_start.c
**
** \par Description
**        This function exercises the remaining (error condition) paths for the
**        ES main startup that were not covered in TestInit.  The remaining
**        functions in cfe_es_start.c for initializing the ES reset
**        variables, the file system, and the application layer are then
**        tested.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void TestStartupErrorPaths(void);

/*****************************************************************************/
/**
** \brief Perform tests on the ES application control internal interface
**        functions contained in cfe_es_api.c
**
** \par Description
**        This function tests all paths for the application control functions
**        which are used for creating, starting, managing, and deleting
**        application and child tasks.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void TestApps(void);

/*****************************************************************************/
/**
** \brief Perform tests of the ES exception and reset log function contained in
**        cfe_es_erlog.c
**
** \par Description
**        This function tests the possible paths through the write to log
**        function including log roll overs, valid and over-sized contexts, and
**        both null and non null descriptions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void TestERLog(void);

/*****************************************************************************/
/**
** \brief Performs tests of the ground command functions contained in
**        cfe_es_task.c
**
** \par Description
**        This function tests the ground command paths for the commands handled
**        by CFE_ES_TaskPipe.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void TestTask(void);

/*****************************************************************************/
/**
** \brief Performs tests of the background task contained in
**        cfe_es_backgroundtask.c
**
** \par Description
**        Gets Coverage on all lines/functions in this unit
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void TestBackground(void);

/*****************************************************************************/
/**
** \brief Performs tests on the functions that implement the software timing
**        performance marker functions contained in cfe_es_perf.c
**
** \par Description
**        This function tests the performance logging functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void TestPerf(void);

/*****************************************************************************/
/**
** \brief Perform tests on the ES API functions contained in cfe_es_api.c
**
** \par Description
**        This function tests the application programmer's interface functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void TestAPI(void);

/*****************************************************************************/
/**
** \brief Perform tests on critical data store functions contained in
**        cfe_es_cds.c
**
** \par Description
**        This function tests the CDS registry manipulation functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void TestCDS(void);

/*****************************************************************************/
/**
** \brief Performs tests on the functions for managing the CDS discrete sized
**        memory pools contained in cfe_es_cds_mempools.c
**
** \par Description
**        This function tests the CDS memory pool create, allocate, rebuild,
**        read, and write functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void TestCDSMempool(void);

/*****************************************************************************/
/**
** \brief Perform tests on the set of services for management of discrete sized
**        memory pools contained in cde_esmempool.c
**
** \par Description
**        This function tests the ES memory pool create, allocate, rebuild,
**        read, and write functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void TestESMempool(void);

void TestSysLog(void);
void TestResourceID(void);
void TestGenericCounterAPI(void);
void TestGenericPool(void);
void TestLibs(void);
void TestStatusToString(void);

#endif /* ES_UT_H */
