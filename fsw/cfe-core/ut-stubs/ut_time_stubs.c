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
#include <stdio.h>
#include <string.h>
#include "cfe_time.h"
#include "private/cfe_private.h"
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
    UT_Stub_RegisterContext(UT_KEY(CFE_TIME_Print), PrintBuffer);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_TIME_Print), TimeToPrint);

    snprintf(PrintBuffer,
             CFE_TIME_PRINTED_STRING_SIZE,
             "UT %u.%u -",
             (unsigned int)TimeToPrint.Seconds,
             (unsigned int)TimeToPrint.Subseconds);

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
int32 CFE_TIME_CleanUpApp(CFE_ES_ResourceID_t AppId)
{
    int32 status;

    status = UT_DEFAULT_IMPL(CFE_TIME_CleanUpApp);

    return status;
}


CFE_TIME_Compare_t  CFE_TIME_Compare(CFE_TIME_SysTime_t TimeA, CFE_TIME_SysTime_t TimeB)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_TIME_Compare), TimeA);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_TIME_Compare), TimeB);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_TIME_Compare);
    
    return (CFE_TIME_Compare_t) status;
}

CFE_TIME_SysTime_t  CFE_TIME_Add(CFE_TIME_SysTime_t Time1, CFE_TIME_SysTime_t Time2)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_TIME_Add), Time1);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_TIME_Add), Time2);

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
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_TIME_Sub2MicroSecs), SubSeconds);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_TIME_Sub2MicroSecs);

    return (uint32) status;
}

int32 CFE_TIME_UnregisterSynchCallback(CFE_TIME_SynchCallbackPtr_t CallbackFuncPtr)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_TIME_UnregisterSynchCallback), CallbackFuncPtr);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_TIME_UnregisterSynchCallback);

    return status;
}

void CFE_TIME_ExternalGPS(CFE_TIME_SysTime_t NewTime, int16 NewLeaps)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_TIME_ExternalGPS), NewTime);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_TIME_ExternalGPS), NewLeaps);

    UT_DEFAULT_IMPL(CFE_TIME_ExternalGPS);
}

void CFE_TIME_ExternalMET(CFE_TIME_SysTime_t NewMET)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_TIME_ExternalMET), NewMET);

    UT_DEFAULT_IMPL(CFE_TIME_ExternalMET);
}

void CFE_TIME_ExternalTime(CFE_TIME_SysTime_t NewTime)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_TIME_ExternalTime), NewTime);

    UT_DEFAULT_IMPL(CFE_TIME_ExternalTime);
}

void CFE_TIME_ExternalTone(void)
{
    UT_DEFAULT_IMPL(CFE_TIME_ExternalTone);
}

uint16 CFE_TIME_GetClockInfo(void)
{
    int32 status;

    status = UT_DEFAULT_IMPL(CFE_TIME_GetClockInfo);

    return status;
}

CFE_TIME_ClockState_Enum_t CFE_TIME_GetClockState(void)
{
    int32 status;

    status = UT_DEFAULT_IMPL(CFE_TIME_GetClockState);

    return status;
}

int16 CFE_TIME_GetLeapSeconds(void)
{
    int32 status;

    status = UT_DEFAULT_IMPL(CFE_TIME_GetLeapSeconds);

    return status;
}

CFE_TIME_SysTime_t CFE_TIME_GetMET(void)
{
    int32 status;
    CFE_TIME_SysTime_t Result = { 0 };

    status = UT_DEFAULT_IMPL(CFE_TIME_GetMET);
    if (status == 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(CFE_TIME_GetMET), &Result, sizeof(Result));
    }

    return Result;
}

uint32 CFE_TIME_GetMETseconds(void)
{
    int32 status;

    status = UT_DEFAULT_IMPL(CFE_TIME_GetMETseconds);

    return status;
}

uint32 CFE_TIME_GetMETsubsecs(void)
{
    int32 status;

    status = UT_DEFAULT_IMPL(CFE_TIME_GetMETsubsecs);

    return status;
}

CFE_TIME_SysTime_t CFE_TIME_GetSTCF(void)
{
    int32 status;
    CFE_TIME_SysTime_t Result = { 0 };

    status = UT_DEFAULT_IMPL(CFE_TIME_GetSTCF);
    if (status == 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(CFE_TIME_GetSTCF), &Result, sizeof(Result));
    }

    return Result;
}

CFE_TIME_SysTime_t CFE_TIME_GetTAI(void)
{
    int32 status;
    CFE_TIME_SysTime_t Result = { 0 };

    status = UT_DEFAULT_IMPL(CFE_TIME_GetTAI);
    if (status == 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(CFE_TIME_GetTAI), &Result, sizeof(Result));
    }

    return Result;
}

CFE_TIME_SysTime_t CFE_TIME_GetUTC(void)
{
    int32 status;
    CFE_TIME_SysTime_t Result = { 0 };

    status = UT_DEFAULT_IMPL(CFE_TIME_GetUTC);
    if (status == 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(CFE_TIME_GetUTC), &Result, sizeof(Result));
    }

    return Result;
}

CFE_TIME_SysTime_t CFE_TIME_MET2SCTime(CFE_TIME_SysTime_t METTime)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_TIME_MET2SCTime), METTime);

    int32 status;
    CFE_TIME_SysTime_t Result = { 0 };

    status = UT_DEFAULT_IMPL(CFE_TIME_MET2SCTime);
    if (status == 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(CFE_TIME_MET2SCTime), &Result, sizeof(Result));
    }

    return Result;
}

uint32 CFE_TIME_Micro2SubSecs(uint32 MicroSeconds)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_TIME_Micro2SubSecs), MicroSeconds);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_TIME_Micro2SubSecs);

    return status;
}

int32 CFE_TIME_RegisterSynchCallback(CFE_TIME_SynchCallbackPtr_t CallbackFuncPtr)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_TIME_RegisterSynchCallback), CallbackFuncPtr);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_TIME_RegisterSynchCallback);

    return status;
}

CFE_TIME_SysTime_t CFE_TIME_Subtract(CFE_TIME_SysTime_t Time1, CFE_TIME_SysTime_t Time2)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_TIME_Subtract), Time1);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_TIME_Subtract), Time2);

    int32 status;
    CFE_TIME_SysTime_t Result = { 0 };

    status = UT_DEFAULT_IMPL(CFE_TIME_Subtract);
    if (status == 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(CFE_TIME_Subtract), &Result, sizeof(Result));
    }

    return Result;
}

