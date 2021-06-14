/*************************************************************************
**
**      GSC-18128-1, "Core Flight Executive Version 6.7"
**
**      Copyright (c) 2006-2019 United States Government as represented by
**      the Administrator of the National Aeronautics and Space Administration.
**      All Rights Reserved.
**
**      Licensed under the Apache License, Version 2.0 (the "License");
**      you may not use this file except in compliance with the License.
**      You may obtain a copy of the License at
**
**        http://www.apache.org/licenses/LICENSE-2.0
**
**      Unless required by applicable law or agreed to in writing, software
**      distributed under the License is distributed on an "AS IS" BASIS,
**      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**      See the License for the specific language governing permissions and
**      limitations under the License.
**
** File: es_info_test.c
**
** Purpose:
**   Functional test of basic Time Current APIs
**
**   Demonstration of how to register and use the UT assert functions.
**
*************************************************************************/

/*
 * Includes
 */

#include "cfe_test.h"

bool TimeInRange(CFE_TIME_SysTime_t Time, CFE_TIME_SysTime_t Target, OS_time_t difference)
{
    OS_time_t Max;
    OS_time_t TimeT   = OS_TimeAssembleFromSubseconds(Time.Seconds, Time.Subseconds);
    OS_time_t TargetT = OS_TimeAssembleFromSubseconds(Target.Seconds, Target.Subseconds);

    Max = OS_TimeAdd(TimeT, difference);

    if (TargetT.ticks >= TimeT.ticks && TargetT.ticks <= Max.ticks)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void TestGetTime(void)
{
    UtPrintf("Testing: CFE_TIME_GetTime, CFE_TIME_GetTAI, CFE_TIME_GetUTC, CFE_TIME_GetMET, CFE_TIME_GetSTCF, "
             "CFE_TIME_GetLeapSeconds");
    OS_time_t          start;
    OS_time_t          end;
    OS_time_t          difference;
    CFE_TIME_SysTime_t Time;
    CFE_TIME_SysTime_t TAI;
    CFE_TIME_SysTime_t UTC;
    CFE_TIME_SysTime_t MET;
    CFE_TIME_SysTime_t STCF;
    uint32             METSeconds;
    uint32             METSubSeconds;

    int16              LeapSeconds;
    CFE_TIME_SysTime_t Buf;

    char timeBuf1[sizeof("yyyy-ddd-hh:mm:ss.xxxxx_")];
    char timeBuf2[sizeof("yyyy-ddd-hh:mm:ss.xxxxx_")];

    OS_GetLocalTime(&start);

    Time          = CFE_TIME_GetTime();
    TAI           = CFE_TIME_GetTAI();
    UTC           = CFE_TIME_GetUTC();
    MET           = CFE_TIME_GetMET();
    METSeconds    = CFE_TIME_GetMETseconds();
    METSubSeconds = CFE_TIME_GetMETsubsecs();
    STCF          = CFE_TIME_GetSTCF();
    LeapSeconds   = CFE_TIME_GetLeapSeconds();

    OS_GetLocalTime(&end);

    CFE_TIME_Print(timeBuf1, Time);
    UtPrintf("The current time is (%ld) %s", (long)Time.Seconds, timeBuf1);

    difference = OS_TimeSubtract(end, start);

#if (CFE_MISSION_TIME_CFG_DEFAULT_TAI == true)
    CFE_TIME_Print(timeBuf2, TAI);
    UtAssert_True(TimeInRange(Time, TAI, difference), "Get Time (%s) = TAI (%s)", timeBuf1, timeBuf2);
#else
    CFE_TIME_Print(timeBuf2, UTC);
    UtAssert_True(TimeInRange(Time, UTC, difference), "Get Time (%s) = UTC(%s)", timeBuf1, timeBuf2);
#endif

    CFE_TIME_Print(timeBuf1, TAI);
    Buf = CFE_TIME_Add(MET, STCF);
    CFE_TIME_Print(timeBuf2, Buf);
    UtAssert_True(TimeInRange(TAI, Buf, difference), "TAI (%s) = MET + STCF (%s)", timeBuf1, timeBuf2);

    CFE_TIME_Print(timeBuf1, UTC);
    Buf.Seconds = Buf.Seconds - LeapSeconds;
    CFE_TIME_Print(timeBuf2, Buf);
    UtAssert_True(TimeInRange(UTC, Buf, difference), "UTC (%s) = MET + STCF - Leap Seconds (%s)", timeBuf1, timeBuf2);

    CFE_TIME_Print(timeBuf1, MET);
    Buf.Seconds    = METSeconds;
    Buf.Subseconds = MET.Subseconds;
    CFE_TIME_Print(timeBuf2, Buf);
    UtAssert_True(TimeInRange(MET, Buf, difference), "MET (%s) = METSeconds (%s)", timeBuf1, timeBuf2);

    Buf.Seconds    = MET.Seconds;
    Buf.Subseconds = METSubSeconds;
    CFE_TIME_Print(timeBuf2, Buf);
    UtAssert_True(TimeInRange(MET, Buf, difference), "MET (%s) = METSubSeconds (%s)", timeBuf1, timeBuf2);
}

void TimeCurrentTestSetup(void)
{
    UtTest_Add(TestGetTime, NULL, NULL, "Test Current Time");
}
