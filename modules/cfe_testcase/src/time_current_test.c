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
 * \file
 *   Functional test of basic Time Current APIs
 *
 *   Demonstration of how to register and use the UT assert functions.
 */

/*
 * Includes
 */

#include "cfe_test.h"
#include "cfe_time_msg.h"

void TimeInRange(CFE_TIME_SysTime_t Start, CFE_TIME_SysTime_t Time, CFE_TIME_SysTime_t Range, const char *Str)
{
    char               StartStr[sizeof("yyyy-ddd-hh:mm:ss.xxxxx_")];
    char               TimeStr[sizeof("yyyy-ddd-hh:mm:ss.xxxxx_")];
    CFE_TIME_Compare_t Compare;
    CFE_TIME_SysTime_t Delta;

    CFE_TIME_Print(StartStr, Start);
    CFE_TIME_Print(TimeStr, Time);

    Compare = CFE_TIME_Compare(Start, Time);
    UtAssert_True((Compare == CFE_TIME_EQUAL) || (Compare == CFE_TIME_A_LT_B), "%s: %lu %lu (%s) <= %lu %lu (%s)", Str,
                  (long unsigned)Start.Seconds, (long unsigned)Start.Subseconds, StartStr, (long unsigned)Time.Seconds,
                  (long unsigned)Time.Subseconds, TimeStr);

    Delta   = CFE_TIME_Subtract(Time, Start);
    Compare = CFE_TIME_Compare(Delta, Range);
    UtAssert_True((Compare == CFE_TIME_EQUAL) || (Compare == CFE_TIME_A_LT_B), "%s: Delta %lu %lu <= Range %lu %lu",
                  Str, (long unsigned)Delta.Seconds, (long unsigned)Delta.Subseconds, (long unsigned)Range.Seconds,
                  (long unsigned)Range.Subseconds);
}

void TestGetTime(void)
{
    UtPrintf("Testing: CFE_TIME_GetTime, CFE_TIME_GetTAI, CFE_TIME_GetUTC, CFE_TIME_GetMET, CFE_TIME_GetSTCF, "
             "CFE_TIME_GetLeapSeconds");
    CFE_TIME_SysTime_t Start;
    CFE_TIME_SysTime_t End;
    CFE_TIME_SysTime_t TAI;
    CFE_TIME_SysTime_t UTC;
    CFE_TIME_SysTime_t MET;
    CFE_TIME_SysTime_t STCF;
    CFE_TIME_SysTime_t Range;
    uint32             METSeconds;
    uint32             METSubSeconds;
    int16              LeapSeconds;
    CFE_TIME_SysTime_t Buf;

    Start         = CFE_TIME_GetTime();
    TAI           = CFE_TIME_GetTAI();
    UTC           = CFE_TIME_GetUTC();
    MET           = CFE_TIME_GetMET();
    METSeconds    = CFE_TIME_GetMETseconds();
    METSubSeconds = CFE_TIME_GetMETsubsecs();
    STCF          = CFE_TIME_GetSTCF();
    LeapSeconds   = CFE_TIME_GetLeapSeconds();
    End           = CFE_TIME_GetTime();

    Range = CFE_TIME_Subtract(End, Start);

#if (CFE_MISSION_TIME_CFG_DEFAULT_TAI == true)
    TimeInRange(Start, TAI, Range, "default time vs TAI");
#else
    TimeInRange(Start, UTC, Range, "default time vs UTC");
#endif

    Buf = CFE_TIME_Add(MET, STCF);
    TimeInRange(TAI, Buf, Range, "TAI vs MET + STCF");

    Buf.Seconds = Buf.Seconds - LeapSeconds;
    TimeInRange(UTC, Buf, Range, "UTC vs MET + STCF - Leap Seconds");

    /* Handle rollover */
    if (METSubSeconds < MET.Subseconds)
    {
        UtAssert_UINT32_EQ(MET.Seconds + 1, METSeconds);
    }
    else
    {
        UtAssert_UINT32_EQ(MET.Seconds, METSeconds);
    }
    UtAssert_UINT32_LTEQ(METSubSeconds - MET.Subseconds, Range.Subseconds);
    UtPrintf("MET = %lu, %lu, METSeconds = %lu, METSubSeconds = %lu, Range.Subseconds = %lu",
             (long unsigned)MET.Seconds, (long unsigned)MET.Subseconds, (long unsigned)METSeconds,
             (long unsigned)METSubSeconds, (long unsigned)Range.Subseconds);
}

void TestClock(void)
{
    UtPrintf("Testing: CFE_TIME_GetClockState, CFE_TIME_GetClockInfo");

    CFE_TIME_ClockState_Enum_t state = CFE_TIME_GetClockState();

    if (state >= 0)
    {
        UtAssert_BITMASK_SET(CFE_TIME_GetClockInfo(), CFE_TIME_FLAG_CLKSET);

        if (state == 0)
        {
            UtAssert_BITMASK_UNSET(CFE_TIME_GetClockInfo(), CFE_TIME_FLAG_FLYING);
        }
        else
        {
            UtAssert_BITMASK_SET(CFE_TIME_GetClockInfo(), CFE_TIME_FLAG_FLYING);
        }
    }
    else
    {
        UtAssert_BITMASK_UNSET(CFE_TIME_GetClockInfo(), CFE_TIME_FLAG_CLKSET);
    }

    UtAssert_BITMASK_SET(CFE_TIME_GetClockInfo(), CFE_TIME_FLAG_SRCINT);
    UtAssert_BITMASK_SET(CFE_TIME_GetClockInfo(), CFE_TIME_FLAG_SIGPRI);
    UtAssert_BITMASK_UNSET(CFE_TIME_GetClockInfo(), CFE_TIME_FLAG_REFERR);
    UtAssert_BITMASK_UNSET(CFE_TIME_GetClockInfo(), CFE_TIME_FLAG_UNUSED);
}

void TimeCurrentTestSetup(void)
{
    UtTest_Add(TestGetTime, NULL, NULL, "Test Current Time");
    UtTest_Add(TestClock, NULL, NULL, "Test Clock");
}
