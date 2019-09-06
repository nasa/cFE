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
** File: ut_arinc653_stubs.c
**
** Purpose:
** Unit test stubs for ARINC653 routines
**
** Notes:
** Minimal work is done, only what is required for unit testing
**
*/
/* Define CFE_ARINC653 when compiling using the ARINC653 version of the cFE */
#ifdef CFE_ARINC653

/*
** Includes
*/
#include "ut_stubs.h"
#include "ut_arinc653_stubs.h"
#include "common_types.h"

#ifndef CFE_LINUX
#include <TUTF.h>
#endif

/*
** Global variables
*/
int32 UT_LibInitRtn;

/*
** Functions
*/
/*
** Set the library initialization return code
*/
void UT_SetLibInitRtn(int32 LibInitRtn)
{
    UT_LibInitRtn = LibInitRtn;
}

/*
** Return the library initialization return code
*/
uint32 UT_LibInit(void)
{
    return UT_LibInitRtn;
}

/*
** CFS Library Initialization Routine
*/
int32 CFS_LibInit(void)
{
    return CFE_SUCCESS;
}

/*
** Initializes the SCH Library
*/
int32 SCH_LibInit(void)
{
    return CFE_SUCCESS;
}

/*
** Initialize the Scheduler CFS application
*/
int32 SCH_AppInit(void)
{
    return CFE_SUCCESS;
}

/*
** CFS Scheduler (SCH) Application Entry Point
*/
void SCH_AppMain(void)
{
}

/*
** Initialize the housekeeping application
*/
int32 HK_AppInit(void)
{
    return CFE_SUCCESS;
}

/*
** CFS Housekeeping (HK) application entry point
*/
void HK_AppMain(void)
{
}

/*
** CI initialization
*/
int32 CI_TaskInit(void)
{
    return CFE_SUCCESS;
}

/*
** Application entry point and main process loop
*/
void CI_AppMain(void)
{
}

/*
** TO initialization
*/
int32 TO_init(void)
{
    return CFE_SUCCESS;
}

/*
** Application entry point and main process loop
*/
void TO_AppMain(void)
{
}

/*
** Initialize all data used by SBN653 application
*/
int32 SBN653_InitApp(void)
{
    return CFE_SUCCESS;
}

/*
** SBN653 application's entry point and main loop
*/
void SBN653_AppMain(void)
{
}

#endif
