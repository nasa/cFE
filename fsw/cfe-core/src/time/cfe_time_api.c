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
#include "cfe_time_utils.h"

#include <string.h>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_TIME_GetTime() -- get time in default format (TAI vs UTC)           */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

CFE_TIME_SysTime_t   CFE_TIME_GetTime(void)
{
    CFE_TIME_SysTime_t CurrentTime;

#if (CFE_MISSION_TIME_CFG_DEFAULT_TAI == true)

    CurrentTime = CFE_TIME_GetTAI();

#else

    CurrentTime = CFE_TIME_GetUTC();

#endif

    return(CurrentTime);

} /* End of CFE_TIME_GetTime() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_TIME_GetTAI() -- get time in TAI format (TAI vs UTC)                */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

CFE_TIME_SysTime_t   CFE_TIME_GetTAI(void)
{
    CFE_TIME_Reference_t Reference;
    CFE_TIME_SysTime_t tai;

    /* Zero out the Reference variable because we pass it into
     * a function before using it
     * */
    memset(&Reference, 0 , sizeof(CFE_TIME_Reference_t));
    
    /*
    ** Get reference time values (local time, time at tone, etc.)...
    */
    CFE_TIME_GetReference(&Reference);

    /*
    ** Calculate current TAI...
    */
    tai = CFE_TIME_CalculateTAI(&Reference);

    return(tai);

} /* End of CFE_TIME_GetTAI() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_TIME_GetUTC() -- get time in UTC format (TAI vs UTC)                */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

CFE_TIME_SysTime_t   CFE_TIME_GetUTC(void)
{
    CFE_TIME_Reference_t Reference;
    CFE_TIME_SysTime_t utc;

    /* Zero out the Reference variable because we pass it into
     * a function before using it
     * */
    memset(&Reference, 0 , sizeof(CFE_TIME_Reference_t));
    /*
    ** Get reference time values (local time, time at tone, etc.)...
    */
    CFE_TIME_GetReference(&Reference);

    /*
    ** Calculate current UTC...
    */
    utc = CFE_TIME_CalculateUTC(&Reference);

    return(utc);

} /* End of CFE_TIME_GetUTC() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_TIME_METtoSCTime() -- convert MET Time to default spacecraft time   */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

CFE_TIME_SysTime_t CFE_TIME_MET2SCTime (CFE_TIME_SysTime_t METTime)
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
    LeapSecsAsSysTime.Seconds       = CFE_TIME_GetLeapSeconds();
    LeapSecsAsSysTime.Subseconds    = 0;
    
    /* UTC Time = TIA Time - Leap Seconds */
    ReturnTime = CFE_TIME_Subtract(TIATime, LeapSecsAsSysTime);

#endif

    return (ReturnTime);
}/* end CFE_TIME_MET2SCTime() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_TIME_GetClockState() -- get time data (status)                      */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

CFE_TIME_ClockState_Enum_t   CFE_TIME_GetClockState(void)
{
    CFE_TIME_Reference_t Reference;
    CFE_TIME_ClockState_Enum_t state;
    
    /* Zero out the Reference variable because we pass it into
     * a function before using it
     * */
    memset(&Reference, 0 , sizeof(CFE_TIME_Reference_t));
    /*
    ** Get reference time values (local time, time at tone, etc.)...
    */
    CFE_TIME_GetReference(&Reference);

    /*
    ** Determine the current clock state...
    */
    state = (CFE_TIME_ClockState_Enum_t) CFE_TIME_CalculateState(&Reference);

    return(state);

} /* End of CFE_TIME_GetClockState() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_GetClockInfo() -- Convert state data to flag values    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

uint16 CFE_TIME_GetClockInfo(void)
{
    uint16 StateFlags = 0;
    volatile CFE_TIME_ReferenceState_t *RefState = CFE_TIME_GetReferenceState();

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
    if (CFE_TIME_TaskData.ClockSource == CFE_TIME_SourceSelect_INTERNAL)
    {
        StateFlags |= CFE_TIME_FLAG_SRCINT;
    }
    /*
    ** Clock signal set to "primary"...
    */
    if (CFE_TIME_TaskData.ClockSignal == CFE_TIME_ToneSignalSelect_PRIMARY)
    {
        StateFlags |= CFE_TIME_FLAG_SIGPRI;
    }
    /*
    ** Time Server is in FLYWHEEL mode...
    */
    if (CFE_TIME_TaskData.ServerFlyState == CFE_TIME_FlywheelState_IS_FLY)
    {
        StateFlags |= CFE_TIME_FLAG_SRVFLY;
    }
    /*
    ** This instance of Time Services commanded into FLYWHEEL...
    */
    if (CFE_TIME_TaskData.Forced2Fly)
    {
        StateFlags |= CFE_TIME_FLAG_CMDFLY;
    }
    /*
    ** One time STCF adjustment direction...
    */
    if (CFE_TIME_TaskData.OneTimeDirection == CFE_TIME_AdjustDirection_ADD)
    {
        StateFlags |= CFE_TIME_FLAG_ADDADJ;
    }
    /*
    ** 1 Hz STCF adjustment direction...
    */
    if (CFE_TIME_TaskData.OneHzDirection == CFE_TIME_AdjustDirection_ADD)
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
    if (CFE_TIME_TaskData.IsToneGood == true)
    {
        StateFlags |= CFE_TIME_FLAG_GDTONE;
    }   

    return(StateFlags);

} /* End of CFE_TIME_GetClockInfo() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_TIME_GetLeapSeconds() -- get time data (leap seconds)               */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int16   CFE_TIME_GetLeapSeconds(void)
{
    CFE_TIME_Reference_t Reference;

    /* Zero out the Reference variable because we pass it into
     * a function before using it
     * */
    memset(&Reference, 0 , sizeof(CFE_TIME_Reference_t));

    /*
    ** Get reference time values (local time, time at tone, etc.)...
    */
    CFE_TIME_GetReference(&Reference);

    return(Reference.AtToneLeapSeconds);

} /* End of CFE_TIME_GetLeapSeconds() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_TIME_GetSTCF() -- get time data (Spacecraft Time Correlation Factor)*/
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

CFE_TIME_SysTime_t   CFE_TIME_GetSTCF(void)
{
    CFE_TIME_Reference_t Reference;
    
    /* Zero out the Reference variable because we pass it into
     * a function before using it
     * */
    memset(&Reference, 0 , sizeof(CFE_TIME_Reference_t));
    
    /*
    ** Get reference time values (local time, time at tone, etc.)...
    */
    CFE_TIME_GetReference(&Reference);

    return(Reference.AtToneSTCF);

} /* End of CFE_TIME_GetSTCF() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_TIME_GetMET() -- get time data (Mission Elapsed Timer)              */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

CFE_TIME_SysTime_t   CFE_TIME_GetMET(void)
{
    CFE_TIME_Reference_t Reference;
    
     /* Zero out the Reference variable because we pass it into
     * a function before using it
     */
    memset(&Reference, 0 , sizeof(CFE_TIME_Reference_t));


    /*
    ** Get reference time values (local time, time at tone, etc.)...
    */
    CFE_TIME_GetReference(&Reference);

    return(Reference.CurrentMET);

} /* End of CFE_TIME_GetMET() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_TIME_GetMETseconds() -- get time data (seconds portion of MET)      */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

uint32   CFE_TIME_GetMETseconds(void)
{
    CFE_TIME_Reference_t Reference;

    /* Zero out the Reference variable because we pass it into
     * a function before using it
     * */
    memset(&Reference, 0 , sizeof(CFE_TIME_Reference_t));

    
    /*
    ** Get reference time values (local time, time at tone, etc.)...
    */
    CFE_TIME_GetReference(&Reference);

    return(Reference.CurrentMET.Seconds);

} /* End of CFE_TIME_GetMETseconds() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_TIME_GetMETsubsecs() -- get time data (sub-seconds portion of MET)  */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

uint32   CFE_TIME_GetMETsubsecs(void)
{
    CFE_TIME_Reference_t Reference;

    /* Zero out the Reference variable because we pass it into
     * a function before using it
     * */
    memset(&Reference, 0 , sizeof(CFE_TIME_Reference_t));
    
    /*
    ** Get reference time values (local time, time at tone, etc.)...
    */
    CFE_TIME_GetReference(&Reference);

    return(Reference.CurrentMET.Subseconds);

} /* End of CFE_TIME_GetMETsubsecs() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_TIME_Add() -- add two CFE_TIME_SysTime_t values                     */
/*                                                                         */
/*   result = time1 + time2                                                */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

CFE_TIME_SysTime_t  CFE_TIME_Add(CFE_TIME_SysTime_t Time1, CFE_TIME_SysTime_t Time2)
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

    return(Result);

} /* End of CFE_TIME_Add() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_TIME_Subtract() -- subtract a CFE_TIME_SysTime_t value from another */
/*                                                                         */
/*   result = time1 - time2                                                */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

CFE_TIME_SysTime_t  CFE_TIME_Subtract(CFE_TIME_SysTime_t Time1, CFE_TIME_SysTime_t Time2)
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

    return(Result);

} /* End of CFE_TIME_Subtract() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_TIME_Compare() -- compare two CFE_TIME_SysTime_t values             */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

CFE_TIME_Compare_t  CFE_TIME_Compare(CFE_TIME_SysTime_t TimeA, CFE_TIME_SysTime_t TimeB)
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

    return(Result);

} /* End of CFE_TIME_Compare() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_TIME_Sub2MicroSecs() -- convert sub-seconds to micro-seconds        */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

uint32  CFE_TIME_Sub2MicroSecs(uint32 SubSeconds)
{
    uint32 MicroSeconds;
	
    /* 0xffffdf00 subseconds = 999999 microseconds, so anything greater 
     * than that we set to 999999 microseconds, so it doesn't get to
     * a million microseconds */
    
	if (SubSeconds > 0xffffdf00)
	{
			MicroSeconds = 999999;
	}
    else
    {
        /*
        **  Convert a 1/2^32 clock tick count to a microseconds count
        **
        **  Conversion factor is  ( ( 2 ** -32 ) / ( 10 ** -6 ) ).
        **
        **  Logic is as follows:
        **    x * ( ( 2 ** -32 ) / ( 10 ** -6 ) )
        **  = x * ( ( 10 ** 6  ) / (  2 ** 32 ) )
        **  = x * ( ( 5 ** 6 ) ( 2 ** 6 ) / ( 2 ** 26 ) ( 2 ** 6) )
        **  = x * ( ( 5 ** 6 ) / ( 2 ** 26 ) )
        **  = x * ( ( 5 ** 3 ) ( 5 ** 3 ) / ( 2 ** 7 ) ( 2 ** 7 ) (2 ** 12) )
        **
        **  C code equivalent:
        **  = ( ( ( ( ( x >> 7) * 125) >> 7) * 125) >> 12 )
        */   

    	MicroSeconds = (((((SubSeconds >> 7) * 125) >> 7) * 125) >> 12);
    

        /* if the Subseconds % 0x4000000 != 0 then we will need to
         * add 1 to the result. the & is a faster way of doing the % */  
	    if ((SubSeconds & 0x3ffffff) != 0)
    	{
	    	MicroSeconds++;
    	}
    
        /* In the Micro2SubSecs conversion, we added an extra anomaly
         * to get the subseconds to bump up against the end point,
         * 0xFFFFF000. This must be accounted for here. Since we bumped
         * at the half way mark, we must "unbump" at the same mark 
         */
        if (MicroSeconds > 500000)
        {
            MicroSeconds --;
        }
        
    } /* end else */
    
    return(MicroSeconds);

} /* End of CFE_TIME_Sub2MicroSecs() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_TIME_Micro2SubSecs() -- convert micro-seconds to sub-seconds        */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

uint32  CFE_TIME_Micro2SubSecs(uint32 MicroSeconds)
{
    uint32 SubSeconds;

    /*
    ** Conversion amount must be less than one second
    */
    if (MicroSeconds > 999999)
    {
        SubSeconds = 0xFFFFFFFF;
    }
    else
    {
    /*
    **  Convert micro-seconds count to sub-seconds (1/2^32) count
    **
    **  Conversion factor is  ( ( 10 ** -6 ) / ( 2 ** -20 ).
    **
    **  Logic is as follows:
    **    x * ( ( 10 ** -6 ) / ( 2 ** -32 ) )
    **  = x * ( ( 2 ** 32 ) / ( 10 ** 6 ) )
    **  = x * ( ( ( 2 ** 26 ) ( 2 ** 6) ) / ( ( 5 ** 6 ) ( 2 ** 6 ) ) )
    **  = x * ( ( 2 ** 26 ) / ( 5 ** 6 ) )
    **  = x * ( ( ( 2 ** 11) ( 2 ** 3) (2 ** 12) ) / ( 5( 5 ** 5 ) ) )
    **  = x * ( ( ( ( ( 2 ** 11 ) / 5 ) * ( 2 ** 3 ) ) / ( 5 ** 5 ) ) * (2 ** 12) )
    **
    **  C code equivalent:
    **  = ( ( ( ( ( x << 11 ) / 5 ) << 3 ) / 3125 ) << 12 )
    **
    **  Conversion factor was reduced and factored accordingly
    **  to minimize precision loss and register overflow.
    */
        SubSeconds = ( ( ( ( MicroSeconds << 11 ) / 5 ) << 3 ) / 3125 ) << 12;

        /* To get the SubSeconds to "bump up" against 0xFFFFF000 when 
         * MicroSeconds = 9999999, we add in another anomaly to the 
         * conversion at the half-way point  (500000 us). This will bump
         * all of the subseconds up by 0x1000, so 999999 us == 0xFFFFF00,
         * 999998 == 0xFFFFE000, etc. This extra anomaly is accounted for
         * in the Sub2MicroSecs conversion as well.
         */
        
        if (SubSeconds > 0x80001000)
        {
           SubSeconds += 0x1000;
        }

    }

    return(SubSeconds);

} /* End of CFE_TIME_Micro2SubSecs() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_TIME_CFE2FSSeconds() --  convert cFE seconds to file system seconds */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

uint32 CFE_TIME_CFE2FSSeconds(uint32 SecondsCFE)
{
    /*
    ** Using a signed integer allows the factor to be negative...
    */
    int32 ConvertFactor = CFE_MISSION_TIME_FS_FACTOR;

    /*
    ** File system time = cFE time + conversion factor...
    */
    uint32 SecondsFS = SecondsCFE + (uint32) ConvertFactor;

    /*
    ** Prevent file system time from going below zero...
    */
    if (ConvertFactor < 0)
    {
        if (-ConvertFactor > SecondsCFE)
        {
            SecondsFS = 0;
        }
    }

    return(SecondsFS);

} /* End of CFE_TIME_CFE2FSSeconds() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_TIME_FS2CFESeconds() --  convert file system seconds to cFE seconds */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

uint32 CFE_TIME_FS2CFESeconds(uint32 SecondsFS)
{
    /*
    ** Using a signed integer allows the factor to be negative...
    */
    int32 ConvertFactor = CFE_MISSION_TIME_FS_FACTOR;

    /*
    ** cFE time = file system time - conversion factor...
    */
    uint32 SecondsCFE = SecondsFS - (uint32) ConvertFactor;

    /*
    ** Prevent cFE time from going below zero...
    */
    if (ConvertFactor > SecondsFS)
    {
        SecondsCFE = 0;
    }

    return(SecondsCFE);

} /* End of CFE_TIME_FS2CFESeconds() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_TIME_Print() --  create text string representing date and time      */
/*                                                                         */
/* Notes:                                                                  */
/*                                                                         */
/*  1) The value of the time argument is simply added to the configuration */
/*     definitions for the ground epoch and converted into a fixed length  */
/*     string in the buffer provided by the caller.                        */
/*                                                                         */
/*  2) A loss of data during the string conversion will occur if the       */
/*     computed year exceeds 9999.  However, a year that large would       */
/*     require an unrealistic definition for the ground epoch since        */
/*     the maximum amount of time represented by a CFE_TIME_SysTime        */
/*     structure is approximately 136 years.                               */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_TIME_Print(char *PrintBuffer, CFE_TIME_SysTime_t TimeToPrint)
{
    uint32 NumberOfYears;
    uint32 NumberOfDays;
    uint32 NumberOfHours;
    uint32 NumberOfMinutes;
    uint32 NumberOfSeconds;
    uint32 NumberOfMicros;
    uint32 DaysInThisYear;

    bool StillCountingYears = true;

    /*
    ** Convert the cFE time (offset from epoch) into calendar time...
    */
    NumberOfMinutes = (TimeToPrint.Seconds / 60) + CFE_MISSION_TIME_EPOCH_MINUTE;
    NumberOfSeconds = (TimeToPrint.Seconds % 60) + CFE_MISSION_TIME_EPOCH_SECOND;

    /*
    ** Adding the epoch "seconds" after computing the minutes avoids
    **    overflow problems when the input time value (seconds) is
    **    at, or near, 0xFFFFFFFF...
    */
    while (NumberOfSeconds >= 60)
    {
        NumberOfMinutes++;
        NumberOfSeconds -= 60;
    }

    /*
    ** Compute the years/days/hours/minutes...
    */
    NumberOfHours   = (NumberOfMinutes / 60) + CFE_MISSION_TIME_EPOCH_HOUR;
    NumberOfMinutes = (NumberOfMinutes % 60);

    /*
    ** Unlike hours and minutes, epoch days are counted as Jan 1 = day 1...
    */
    NumberOfDays  = (NumberOfHours / 24) + (CFE_MISSION_TIME_EPOCH_DAY - 1);
    NumberOfHours = (NumberOfHours % 24);

    NumberOfYears = CFE_MISSION_TIME_EPOCH_YEAR;

    /*
    ** Convert total number of days into years and remainder days...
    */
    while (StillCountingYears)
    {
        /*
        ** Set number of days in this year (leap year?)...
        */
        DaysInThisYear = 365;

        if ((NumberOfYears % 4) == 0)
        {
            if ((NumberOfYears % 100) != 0)
            {
                DaysInThisYear = 366;
            }
            else if ((NumberOfYears % 400) == 0)
            {
                DaysInThisYear = 366;
            }
            else
            {
                /* Do Nothing. Non-leap year. */ 
            }
        }

        /*
        ** When we have less than a years worth of days, we're done...
        */
        if (NumberOfDays < DaysInThisYear)
        {
            StillCountingYears = false;
        }
        else
        {
            /*
            ** Add a year and remove the number of days in that year...
            */
            NumberOfYears++;
            NumberOfDays -= DaysInThisYear;
        }
    }

    /*
    ** Unlike hours and minutes, days are displayed as Jan 1 = day 1...
    */
    NumberOfDays++;

    /*
    ** After computing microseconds, convert to 5 digits from 6 digits...
    */
    NumberOfMicros = CFE_TIME_Sub2MicroSecs(TimeToPrint.Subseconds) / 10;

    /*
    ** Build formatted output string (yyyy-ddd-hh:mm:ss.xxxxx)...
    */
    *PrintBuffer++ = '0' + (char) (NumberOfYears / 1000); NumberOfYears = NumberOfYears % 1000;
    *PrintBuffer++ = '0' + (char) (NumberOfYears / 100);  NumberOfYears = NumberOfYears % 100;
    *PrintBuffer++ = '0' + (char) (NumberOfYears / 10);
    *PrintBuffer++ = '0' + (char) (NumberOfYears % 10);
    *PrintBuffer++ = '-';

    *PrintBuffer++ = '0' + (char) (NumberOfDays / 100); NumberOfDays = NumberOfDays % 100;
    *PrintBuffer++ = '0' + (char) (NumberOfDays / 10);
    *PrintBuffer++ = '0' + (char) (NumberOfDays % 10);
    *PrintBuffer++ = '-';

    *PrintBuffer++ = '0' + (char) (NumberOfHours / 10);
    *PrintBuffer++ = '0' + (char) (NumberOfHours % 10);
    *PrintBuffer++ = ':';

    *PrintBuffer++ = '0' + (char) (NumberOfMinutes / 10);
    *PrintBuffer++ = '0' + (char) (NumberOfMinutes % 10);
    *PrintBuffer++ = ':';

    *PrintBuffer++ = '0' + (char) (NumberOfSeconds / 10);
    *PrintBuffer++ = '0' + (char) (NumberOfSeconds % 10);
    *PrintBuffer++ = '.';

    *PrintBuffer++ = '0' + (char) (NumberOfMicros / 10000); NumberOfMicros = NumberOfMicros % 10000;
    *PrintBuffer++ = '0' + (char) (NumberOfMicros / 1000);  NumberOfMicros = NumberOfMicros % 1000;
    *PrintBuffer++ = '0' + (char) (NumberOfMicros / 100);   NumberOfMicros = NumberOfMicros % 100;
    *PrintBuffer++ = '0' + (char) (NumberOfMicros / 10);
    *PrintBuffer++ = '0' + (char) (NumberOfMicros % 10);
    *PrintBuffer++ = '\0';

    return;

} /* End of CFE_TIME_Print() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_TIME_ExternalTone() --  external tone signal was detected           */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_TIME_ExternalTone(void)
{
    /*
    ** Call tone signal ISR (OK if called from non-ISR context)...
    */
    CFE_TIME_Tone1HzISR();

    return;

} /* End of CFE_TIME_ExternalTone() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_TIME_RegisterSynchCallback() --  register synch callback func       */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32  CFE_TIME_RegisterSynchCallback(CFE_TIME_SynchCallbackPtr_t CallbackFuncPtr)   
{
    int32  Status;
    uint32 AppId;

    Status = CFE_ES_GetAppID(&AppId);
    if (Status != CFE_SUCCESS)
    {
        /* Called from an invalid context */
        return Status;
    }

    if (AppId >= (sizeof(CFE_TIME_TaskData.SynchCallback) / sizeof(CFE_TIME_TaskData.SynchCallback[0])) ||
        CFE_TIME_TaskData.SynchCallback[AppId].Ptr != NULL)
    {
        Status = CFE_TIME_TOO_MANY_SYNCH_CALLBACKS;
    }
    else
    {
        CFE_TIME_TaskData.SynchCallback[AppId].Ptr = CallbackFuncPtr;
    }
    
    return Status;
} /* End of CFE_TIME_RegisterSynchCallback() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_TIME_UnregisterSynchCallback() --  unregister synch callback func   */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32  CFE_TIME_UnregisterSynchCallback(CFE_TIME_SynchCallbackPtr_t CallbackFuncPtr)   
{
    int32  Status;
    uint32 AppId;

    Status = CFE_ES_GetAppID(&AppId);
    if (Status != CFE_SUCCESS)
    {
        /* Called from an invalid context */
        return Status;
    }

    if (AppId >= (sizeof(CFE_TIME_TaskData.SynchCallback) / sizeof(CFE_TIME_TaskData.SynchCallback[0])) ||
            CFE_TIME_TaskData.SynchCallback[AppId].Ptr != CallbackFuncPtr)
    {
        Status = CFE_TIME_CALLBACK_NOT_REGISTERED;
    }
    else
    {
        CFE_TIME_TaskData.SynchCallback[AppId].Ptr = NULL;
    }
    
    return Status;
} /* End of CFE_TIME_UnregisterSynchCallback() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_TIME_ExternalMET() -- time data from external source (MET)          */
/*                                                                         */
/* Notes: 1) Time Servers use this call as the signal to send the          */
/*           "time at the tone" command packet.                            */
/*                                                                         */
/*        2) This function is only included in the API if the Time         */
/*           Server has been configured to accept time data from an        */
/*           external source (MET).                                        */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#if (CFE_PLATFORM_TIME_CFG_SRC_MET == true)
void CFE_TIME_ExternalMET(CFE_TIME_SysTime_t NewMET)
{
    /*
    ** Process external MET data...
    */
    CFE_TIME_ToneSendMET(NewMET);

    return;

} /* End of CFE_TIME_ExternalMET() */
#endif /* CFE_PLATFORM_TIME_CFG_SRC_MET  */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_TIME_ExternalGPS() -- time data from external source (GPS)          */
/*                                                                         */
/* Notes: 1) Time Servers use this call as the signal to send the          */
/*           "time at the tone" command packet.                            */
/*                                                                         */
/*        2) This function is only included in the API if the Time         */
/*           Server has been configured to accept time data from an        */
/*           external source (GPS).                                        */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#if (CFE_PLATFORM_TIME_CFG_SRC_GPS == true)
void CFE_TIME_ExternalGPS(CFE_TIME_SysTime_t NewTime, int16 NewLeaps)
{
    /*
    ** Process external GPS time data...
    */
    CFE_TIME_ToneSendGPS(NewTime, NewLeaps);

    return;

} /* End of CFE_TIME_ExternalGPS() */
#endif /* CFE_PLATFORM_TIME_CFG_SRC_GPS */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_TIME_ExternalTime() -- time data from external source (S/C, etc.)   */
/*                                                                         */
/* Notes: 1) Time Servers use this call as the signal to send the          */
/*           "time at the tone" command packet.                            */
/*                                                                         */
/*        2) This function is only included in the API if the Time         */
/*           Server has been configured to accept time data from an        */
/*           external source (spacecraft time).                            */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#if (CFE_PLATFORM_TIME_CFG_SRC_TIME == true)
void CFE_TIME_ExternalTime(CFE_TIME_SysTime_t NewTime)
{
    /*
    ** Process external time data...
    */
    CFE_TIME_ToneSendTime(NewTime);

    return;

} /* End of CFE_TIME_ExternalTime() */
#endif /* CFE_PLATFORM_TIME_CFG_SRC_TIME */


/************************/
/*  End of File Comment */
/************************/
