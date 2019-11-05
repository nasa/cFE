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
** File: ut_time_stubs.c
**
** Purpose:
** Unit test stubs for Time routines
** 
** Notes:
** Minimal work is done, only what is required for unit testing
**
*/

/*
** Includes
*/
#include <string.h>
#include "cfe_time.h"
#include "cfe_time_utils.h"
#include "ut_support.h"
#include "utstubs.h"

/*
** Functions
*/
/*****************************************************************************/
/**
** \brief CFE_TIME_EarlyInit stub function
**
** \par Description
**        This function is used to mimic the response of the cFE TIME function
**        CFE_TIME_EarlyInit.  It always returns OS_SUCCESS when called.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns OS_SUCCESS.
**
******************************************************************************/
int32 CFE_TIME_EarlyInit(void)
{
    int32 status;

    status = UT_DEFAULT_IMPL(CFE_TIME_EarlyInit);

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_TIME_TaskMain stub function
**
** \par Description
**        This function is used as a placeholder for the cFE TIME function
**        CFE_TIME_TaskMain.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void CFE_TIME_TaskMain(void)
{
    UT_DEFAULT_IMPL(CFE_TIME_TaskMain);
}

/*****************************************************************************/
/**
** \brief CFE_TIME_Print stub function
**
** \par Description
**        This function is used to mimic the response of the cFE TIME function
**        CFE_TIME_Print.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void CFE_TIME_Print(char *PrintBuffer, CFE_TIME_SysTime_t TimeToPrint)
{
    snprintf(PrintBuffer,
            CFE_TIME_PRINTED_STRING_SIZE,
             "UT %lu.%lu -",
             (unsigned long)TimeToPrint.Seconds,
             (unsigned long)TimeToPrint.Subseconds);


    UT_Stub_RegisterContext(UT_KEY(CFE_TIME_Print), PrintBuffer);
    UT_Stub_RegisterContext(UT_KEY(CFE_TIME_Print), &TimeToPrint);
    UT_DEFAULT_IMPL(CFE_TIME_Print);
}

/*****************************************************************************/
/**
** \brief CFE_TIME_GetTime stub function
**
** \par Description
**        This function is used to mimic the response of the cFE TIME function
**        CFE_TIME_GetTime.  It increments the time structure values each time
**        it's called to provide a non-static time value for the unit tests.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns the time structure.
**
******************************************************************************/
CFE_TIME_SysTime_t CFE_TIME_GetTime(void)
{
    static CFE_TIME_SysTime_t SimTime = { 0 };
    CFE_TIME_SysTime_t Result = { 0 };
    int32 status;

    status = UT_DEFAULT_IMPL(CFE_TIME_GetTime);

    if (status >= 0)
    {
        if (UT_Stub_CopyToLocal(UT_KEY(CFE_TIME_GetTime), (uint8*)&Result, sizeof(Result)) < sizeof(Result))
        {
            SimTime.Seconds++;
            SimTime.Subseconds++;
            Result = SimTime;
        }
    }

    return Result;
}

/*****************************************************************************/
/**
** \brief CFE_TIME_CleanUpApp stub function
**
** \par Description
**        This function is used to mimic the response of the cFE TIME function
**        CFE_TIME_CleanUpApp.  The user can adjust the response by setting
**        the values in the TIMECleanUpRtn structure prior to this function
**        being called.  If the value TIMECleanUpRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value TIMECleanUpRtn.value.
**        A positive value, indicating success, is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_TIME_CleanUpApp(uint32 AppId)
{
    int32 status;

    status = UT_DEFAULT_IMPL(CFE_TIME_CleanUpApp);

    return status;
}


CFE_TIME_Compare_t  CFE_TIME_Compare(CFE_TIME_SysTime_t TimeA, CFE_TIME_SysTime_t TimeB)
{
    int32 status;

    status = UT_DEFAULT_IMPL(CFE_TIME_Compare);
    
    return (CFE_TIME_Compare_t) status;
}

CFE_TIME_SysTime_t  CFE_TIME_Add(CFE_TIME_SysTime_t Time1, CFE_TIME_SysTime_t Time2)
{
    static CFE_TIME_SysTime_t SimTime = { 0 };
    CFE_TIME_SysTime_t Result = { 0 };
    int32 status;

    status = UT_DEFAULT_IMPL(CFE_TIME_Add);

    if (status >= 0)
    {
        if (UT_Stub_CopyToLocal(UT_KEY(CFE_TIME_Add), (uint8*)&Result, sizeof(Result)) < sizeof(Result))
        {
            SimTime.Seconds++;
            SimTime.Subseconds++;
            Result = SimTime;
        }
    }

    return Result;
}

uint32  CFE_TIME_Sub2MicroSecs(uint32 SubSeconds)
{
    int32 status;

    status = UT_DEFAULT_IMPL(CFE_TIME_Sub2MicroSecs);

    return (uint32) status;
}

uint32 CFE_TIME_FS2CFESeconds(uint32 SecondsFS)
{
    int32 status;

    status = UT_DEFAULT_IMPL(CFE_TIME_FS2CFESeconds);

    return (uint32) status;
}

UT_DEFAULT_STUB(CFE_TIME_UnregisterSynchCallback, (CFE_TIME_SynchCallbackPtr_t CallbackFuncPtr))   

