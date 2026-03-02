/************************************************************************
 * NASA Docket No. GSC-19,200-1, and identified as "cFS Draco"
 *
 * Copyright (c) 2023 United States Government as represented by the
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

/*
** File: cfe_time_api.c
**
** Purpose:  cFE Time Services (TIME) library API source file
**
** Author:   S.Walling/Microtel
**
** Notes:    Partially derived from SDO source code
**
*/

/*
** Required header files...
*/
#include "cfe_time_module_all.h"

#include <string.h>

#include <time.h>

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_TIME_SysTime_t CFE_TIME_GetTime(void)
{
    CFE_TIME_SysTime_t CurrentTime;

#if (CFE_MISSION_TIME_CFG_DEFAULT_TAI == true)

    CurrentTime = CFE_TIME_GetTAI();

#else

    CurrentTime = CFE_TIME_GetUTC();

#endif

    return CurrentTime;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_TIME_SysTime_t CFE_TIME_GetTAI(void)
{
    CFE_TIME_Reference_t Reference;
    CFE_TIME_SysTime_t   tai;

    /* Zero out the Reference variable because we pass it into
     * a function before using it
     * */
    memset(&Reference, 0, sizeof(CFE_TIME_Reference_t));

    /*
    ** Get reference time values (local time, time at tone, etc.)...
    */
    CFE_TIME_GetReference(&Reference);

    /*
    ** Calculate current TAI...
    */
    tai = CFE_TIME_CalculateTAI(&Reference);

    return tai;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_TIME_SysTime_t CFE_TIME_GetUTC(void)
{
    CFE_TIME_Reference_t Reference;
    CFE_TIME_SysTime_t   utc;

    /* Zero out the Reference variable because we pass it into
     * a function before using it
     * */
    memset(&Reference, 0, sizeof(CFE_TIME_Reference_t));
    /*
    ** Get reference time values (local time, time at tone, etc.)...
    */
    CFE_TIME_GetReference(&Reference);

    /*
    ** Calculate current UTC...
    */
    utc = CFE_TIME_CalculateUTC(&Reference);

    return utc;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_TIME_SysTime_t CFE_TIME_MET2SCTime(CFE_TIME_SysTime_t METTime)
{
    CFE_TIME_SysTime_t STCF;
    CFE_TIME_SysTime_t TIATime;
    CFE_TIME_SysTime_t ReturnTime;
#if (CFE_MISSION_TIME_CFG_DEFAULT_TAI != true)
    CFE_TIME_SysTime_t LeapSecsAsSysTime;
#endif

    STCF = CFE_TIME_GetSTCF();

    /* TIA = MET + STCF */
    TIATime = CFE_TIME_Add(METTime, STCF);

#if (CFE_MISSION_TIME_CFG_DEFAULT_TAI == true)

    ReturnTime = TIATime;

#else

    /* Put leap seconds in correct format */
    LeapSecsAsSysTime.Seconds    = CFE_TIME_GetLeapSeconds();
    LeapSecsAsSysTime.Subseconds = 0;

    /* UTC Time = TIA Time - Leap Seconds */
    ReturnTime = CFE_TIME_Subtract(TIATime, LeapSecsAsSysTime);

#endif

    return ReturnTime;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_TIME_ClockState_Enum_t CFE_TIME_GetClockState(void)
{
    CFE_TIME_Reference_t       Reference;
    CFE_TIME_ClockState_Enum_t state;

    /* Zero out the Reference variable because we pass it into
     * a function before using it
     * */
    memset(&Reference, 0, sizeof(CFE_TIME_Reference_t));
    /*
    ** Get reference time values (local time, time at tone, etc.)...
    */
    CFE_TIME_GetReference(&Reference);

    /*
    ** Determine the current clock state...
    */
    state = (CFE_TIME_ClockState_Enum_t)CFE_TIME_CalculateState(&Reference);

    return state;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
uint16 CFE_TIME_GetClockInfo(void)
{
    uint16                              StateFlags = 0;
    volatile CFE_TIME_ReferenceState_t *RefState   = CFE_TIME_GetReferenceState();

    /*
    ** Spacecraft time has been set...
    */
    if (RefState->ClockSetState == CFE_TIME_SetState_WAS_SET)
    {
        StateFlags |= CFE_TIME_FLAG_CLKSET;
    }
    /*
    ** This instance of Time Service is in FLYWHEEL mode...
    */
    if (RefState->ClockFlyState == CFE_TIME_FlywheelState_IS_FLY)
    {
        StateFlags |= CFE_TIME_FLAG_FLYING;
    }
    /*
    ** Clock source set to "internal"...
    */
    if (CFE_TIME_Global.ClockSource == CFE_TIME_SourceSelect_INTERNAL)
    {
        StateFlags |= CFE_TIME_FLAG_SRCINT;
    }
    /*
    ** Clock signal set to "primary"...
    */
    if (CFE_TIME_Global.ClockSignal == CFE_TIME_ToneSignalSelect_PRIMARY)
    {
        StateFlags |= CFE_TIME_FLAG_SIGPRI;
    }
    /*
    ** Time Server is in FLYWHEEL mode...
    */
    if (CFE_TIME_Global.ServerFlyState == CFE_TIME_FlywheelState_IS_FLY)
    {
        StateFlags |= CFE_TIME_FLAG_SRVFLY;
    }
    /*
    ** This instance of Time Services commanded into FLYWHEEL...
    */
    if (CFE_TIME_Global.Forced2Fly)
    {
        StateFlags |= CFE_TIME_FLAG_CMDFLY;
    }
    /*
    ** One time STCF adjustment direction...
    */
    if (CFE_TIME_Global.OneTimeDirection == CFE_TIME_AdjustDirection_ADD)
    {
        StateFlags |= CFE_TIME_FLAG_ADDADJ;
    }
    /*
    ** 1 Hz STCF adjustment direction...
    */
    if (CFE_TIME_Global.OneHzDirection == CFE_TIME_AdjustDirection_ADD)
    {
        StateFlags |= CFE_TIME_FLAG_ADD1HZ;
    }
    /*
    ** Time Client Latency adjustment direction...
    */
    if (RefState->DelayDirection == CFE_TIME_AdjustDirection_ADD)
    {
        StateFlags |= CFE_TIME_FLAG_ADDTCL;
    }
/*
** This instance of Time Service is a "server"...
*/
#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
    StateFlags |= CFE_TIME_FLAG_SERVER;
#endif

    /*
    ** The tone is good
    */
    if (CFE_TIME_Global.IsToneGood == true)
    {
        StateFlags |= CFE_TIME_FLAG_GDTONE;
    }

    /*
    ** Check if CFE_TIME_GetReference ever failed to get a good value
    */
    if (CFE_TIME_Global.GetReferenceFail)
    {
        StateFlags |= CFE_TIME_FLAG_REFERR;
    }

    return StateFlags;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int16 CFE_TIME_GetLeapSeconds(void)
{
    CFE_TIME_Reference_t Reference;

    /* Zero out the Reference variable because we pass it into
     * a function before using it
     * */
    memset(&Reference, 0, sizeof(CFE_TIME_Reference_t));

    /*
    ** Get reference time values (local time, time at tone, etc.)...
    */
    CFE_TIME_GetReference(&Reference);

    return Reference.AtToneLeapSeconds;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_TIME_SysTime_t CFE_TIME_GetSTCF(void)
{
    CFE_TIME_Reference_t Reference;

    /* Zero out the Reference variable because we pass it into
     * a function before using it
     * */
    memset(&Reference, 0, sizeof(CFE_TIME_Reference_t));

    /*
    ** Get reference time values (local time, time at tone, etc.)...
    */
    CFE_TIME_GetReference(&Reference);

    return Reference.AtToneSTCF;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_TIME_SysTime_t CFE_TIME_GetMET(void)
{
    CFE_TIME_Reference_t Reference;

    /* Zero out the Reference variable because we pass it into
     * a function before using it
     */
    memset(&Reference, 0, sizeof(CFE_TIME_Reference_t));

    /*
    ** Get reference time values (local time, time at tone, etc.)...
    */
    CFE_TIME_GetReference(&Reference);

    return Reference.CurrentMET;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
uint32 CFE_TIME_GetMETseconds(void)
{
    CFE_TIME_Reference_t Reference;

    /* Zero out the Reference variable because we pass it into
     * a function before using it
     * */
    memset(&Reference, 0, sizeof(CFE_TIME_Reference_t));

    /*
    ** Get reference time values (local time, time at tone, etc.)...
    */
    CFE_TIME_GetReference(&Reference);

    return Reference.CurrentMET.Seconds;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
uint32 CFE_TIME_GetMETsubsecs(void)
{
    CFE_TIME_Reference_t Reference;

    /* Zero out the Reference variable because we pass it into
     * a function before using it
     * */
    memset(&Reference, 0, sizeof(CFE_TIME_Reference_t));

    /*
    ** Get reference time values (local time, time at tone, etc.)...
    */
    CFE_TIME_GetReference(&Reference);

    return Reference.CurrentMET.Subseconds;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_TIME_SysTime_t CFE_TIME_Add(CFE_TIME_SysTime_t Time1, CFE_TIME_SysTime_t Time2)
{
    CFE_TIME_SysTime_t Result;

    Result.Subseconds = Time1.Subseconds + Time2.Subseconds;

    /*
    ** Check for sub-seconds roll-over
    */
    if (Result.Subseconds < Time1.Subseconds)
    {
        Result.Seconds = (Time1.Seconds + Time2.Seconds) + 1;
    }
    else
    {
        Result.Seconds = Time1.Seconds + Time2.Seconds;
    }

    return Result;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_TIME_SysTime_t CFE_TIME_Subtract(CFE_TIME_SysTime_t Time1, CFE_TIME_SysTime_t Time2)
{
    CFE_TIME_SysTime_t Result;

    Result.Subseconds = Time1.Subseconds - Time2.Subseconds;

    if (Result.Subseconds > Time1.Subseconds)
    {
        Result.Seconds = (Time1.Seconds - Time2.Seconds) - 1;
    }
    else
    {
        Result.Seconds = Time1.Seconds - Time2.Seconds;
    }

    return Result;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_TIME_Compare_t CFE_TIME_Compare(CFE_TIME_SysTime_t TimeA, CFE_TIME_SysTime_t TimeB)
{
    CFE_TIME_Compare_t Result;

    if (TimeA.Seconds > TimeB.Seconds)
    {
        /*
        ** Assume rollover if difference is too large...
        */
        if ((TimeA.Seconds - TimeB.Seconds) > CFE_TIME_NEGATIVE)
        {
            Result = CFE_TIME_A_LT_B;
        }
        else
        {
            Result = CFE_TIME_A_GT_B;
        }
    }
    else if (TimeA.Seconds < TimeB.Seconds)
    {
        /*
        ** Assume rollover if difference is too large...
        */
        if ((TimeB.Seconds - TimeA.Seconds) > CFE_TIME_NEGATIVE)
        {
            Result = CFE_TIME_A_GT_B;
        }
        else
        {
            Result = CFE_TIME_A_LT_B;
        }
    }
    else
    {
        /*
        ** Seconds are equal, check sub-seconds
        */
        if (TimeA.Subseconds > TimeB.Subseconds)
        {
            Result = CFE_TIME_A_GT_B;
        }
        else if (TimeA.Subseconds < TimeB.Subseconds)
        {
            Result = CFE_TIME_A_LT_B;
        }
        else
        {
            Result = CFE_TIME_EQUAL;
        }
    }

    return Result;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
uint32 CFE_TIME_Sub2MicroSecs(uint32 SubSeconds)
{
    OS_time_t tm;

    /*
    ** Convert using the OSAL method.  Note that there
    ** is no range check here because any uint32 value is valid,
    ** and OSAL will handle and properly convert any input.
    */
    tm = OS_TimeAssembleFromSubseconds(0, SubSeconds);

    return OS_TimeGetMicrosecondsPart(tm);
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
uint32 CFE_TIME_Micro2SubSecs(uint32 MicroSeconds)
{
    OS_time_t tm;
    uint32    SubSeconds;

    /*
    ** Conversion amount must be less than one second
    ** (preserves existing behavior where output saturates at max value)
    */
    if (MicroSeconds > 999999)
    {
        SubSeconds = 0xFFFFFFFF;
    }
    else
    {
        /*
        ** Convert micro-seconds count to sub-seconds (1/2^32) count using OSAL
        */
        tm         = OS_TimeAssembleFromNanoseconds(0, MicroSeconds * 1000);
        SubSeconds = OS_TimeGetSubsecondsPart(tm);
    }

    return SubSeconds;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TIME_Print(char *PrintBuffer, CFE_TIME_SysTime_t TimeToPrint) 
 { 
     size_t    FmtLen = 0; 
     uint32    Micros = (CFE_TIME_Sub2MicroSecs(TimeToPrint.Subseconds) + CFE_MISSION_TIME_EPOCH_MICROS) / 10; 
     struct tm tm; 
  
     if (PrintBuffer == NULL) 
     { 
        return CFE_TIME_BAD_ARGUMENT; 
     } 
  
     time_t sec = TimeToPrint.Seconds + CFE_MISSION_TIME_EPOCH_SECONDS; // epoch is Jan 1, 1980 
     gmtime_r(&sec, &tm); 
     FmtLen = strftime(PrintBuffer, CFE_TIME_PRINTED_STRING_SIZE - 6, "%Y-%j-%H:%M:%S", &tm); 
     PrintBuffer += FmtLen; 
     *(PrintBuffer++) = '.'; 
  
     *(PrintBuffer++) = '0' + (char)((Micros % 100000) / 10000); 
     *(PrintBuffer++) = '0' + (char)((Micros % 10000) / 1000); 
     *(PrintBuffer++) = '0' + (char)((Micros % 1000) / 100); 
     *(PrintBuffer++) = '0' + (char)((Micros % 100) / 10); 
     *(PrintBuffer++) = '0' + (char)(Micros % 10); 
     *PrintBuffer     = '\0'; 
     return CFE_SUCCESS; 
 } 

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TIME_ExternalTone(void)
{
    /*
    ** Call tone signal ISR (OK if called from non-ISR context)...
    */
    CFE_TIME_Tone1HzISR();
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TIME_RegisterSynchCallback(CFE_TIME_SynchCallbackPtr_t CallbackFuncPtr)
{
    int32          Status;
    CFE_ES_AppId_t AppId;
    uint32         AppIndex;

    if (CallbackFuncPtr == NULL)
    {
        return CFE_TIME_BAD_ARGUMENT;
    }

    Status = CFE_ES_GetAppID(&AppId);
    if (Status == CFE_SUCCESS)
    {
        Status = CFE_ES_AppID_ToIndex(AppId, &AppIndex);

        if (Status == CFE_SUCCESS)
        {
            if (AppIndex >= (sizeof(CFE_TIME_Global.SynchCallback) / sizeof(CFE_TIME_Global.SynchCallback[0])) ||
                CFE_TIME_Global.SynchCallback[AppIndex].Ptr != NULL)
            {
                Status = CFE_TIME_TOO_MANY_SYNCH_CALLBACKS;
            }
            else
            {
                CFE_TIME_Global.SynchCallback[AppIndex].Ptr = CallbackFuncPtr;
            }
        }
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TIME_UnregisterSynchCallback(CFE_TIME_SynchCallbackPtr_t CallbackFuncPtr)
{
    int32          Status;
    CFE_ES_AppId_t AppId;
    uint32         AppIndex;

    if (CallbackFuncPtr == NULL)
    {
        return CFE_TIME_BAD_ARGUMENT;
    }

    Status = CFE_ES_GetAppID(&AppId);
    if (Status == CFE_SUCCESS)
    {
        Status = CFE_ES_AppID_ToIndex(AppId, &AppIndex);

        if (Status == CFE_SUCCESS)
        {
            if (AppIndex >= (sizeof(CFE_TIME_Global.SynchCallback) / sizeof(CFE_TIME_Global.SynchCallback[0])) ||
                CFE_TIME_Global.SynchCallback[AppIndex].Ptr != CallbackFuncPtr)
            {
                Status = CFE_TIME_CALLBACK_NOT_REGISTERED;
            }
            else
            {
                CFE_TIME_Global.SynchCallback[AppIndex].Ptr = NULL;
            }
        }
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
#if (CFE_PLATFORM_TIME_CFG_SRC_MET == true)
void CFE_TIME_ExternalMET(CFE_TIME_SysTime_t NewMET)
{
    /*
    ** Process external MET data...
    */
    CFE_TIME_ToneSendMET(NewMET);
}
#endif /* CFE_PLATFORM_TIME_CFG_SRC_MET  */

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
#if (CFE_PLATFORM_TIME_CFG_SRC_GPS == true)
void CFE_TIME_ExternalGPS(CFE_TIME_SysTime_t NewTime, int16 NewLeaps)
{
    /*
    ** Process external GPS time data...
    */
    CFE_TIME_ToneSendGPS(NewTime, NewLeaps);
}
#endif /* CFE_PLATFORM_TIME_CFG_SRC_GPS */

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
#if (CFE_PLATFORM_TIME_CFG_SRC_TIME == true)
void CFE_TIME_ExternalTime(CFE_TIME_SysTime_t NewTime)
{
    /*
    ** Process external time data...
    */
    CFE_TIME_ToneSendTime(NewTime);
}
#endif /* CFE_PLATFORM_TIME_CFG_SRC_TIME */
