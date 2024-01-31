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

/*
** File: cfe_time_utils.c
**
** Purpose:  cFE Time Services (TIME) library utilities source file
**
** Author:   S.Walling/Microtel
**
** Notes:
**
*/

/*
** Required header files...
*/
#include "cfe_time_utils.h"
#include "cfe_msgids.h"
#include "cfe_es_resetdata_typedef.h"

#include "cfe_es.h"
#include "cfe_msg.h"
#include "cfe_sb.h"

#include <string.h>

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
volatile CFE_TIME_ReferenceState_t *CFE_TIME_StartReferenceUpdate(void)
{
    uint32                              Version = CFE_TIME_Global.LastVersionCounter;
    volatile CFE_TIME_ReferenceState_t *CurrState;
    volatile CFE_TIME_ReferenceState_t *NextState;

    CurrState = &CFE_TIME_Global.ReferenceState[Version & CFE_TIME_REFERENCE_BUF_MASK];
    ++Version;
    NextState = &CFE_TIME_Global.ReferenceState[Version & CFE_TIME_REFERENCE_BUF_MASK];

    NextState->StateVersion = Version;

    /* initially propagate all previous values to next values */
    NextState->AtToneLeapSeconds = CurrState->AtToneLeapSeconds;
    NextState->ClockSetState     = CurrState->ClockSetState;
    NextState->ClockFlyState     = CurrState->ClockFlyState;
    NextState->DelayDirection    = CurrState->DelayDirection;
    NextState->AtToneMET         = CurrState->AtToneMET;
    NextState->AtToneSTCF        = CurrState->AtToneSTCF;
    NextState->AtToneDelay       = CurrState->AtToneDelay;
    NextState->AtToneLatch       = CurrState->AtToneLatch;

    return NextState;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_TIME_SysTime_t CFE_TIME_LatchClock(void)
{
    CFE_TIME_SysTime_t LatchTime;
    OS_time_t          LocalTime;

    memset(&LocalTime, 0, sizeof(LocalTime));

    /*
    ** Get time in O/S format (seconds : microseconds)...
    */
    CFE_PSP_GetTime(&LocalTime);

    /*
    ** Convert time to cFE format (seconds : 1/2^32 subseconds)...
    */
    LatchTime.Seconds    = OS_TimeGetTotalSeconds(LocalTime);
    LatchTime.Subseconds = OS_TimeGetSubsecondsPart(LocalTime);

    return LatchTime;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TIME_QueryResetVars(void)
{
    CFE_TIME_ResetVars_t                LocalResetVars;
    uint32                              DefSubsMET;
    uint32                              DefSubsSTCF;
    int32                               PspStatus;
    volatile CFE_TIME_ReferenceState_t *RefState;
    uint32                              resetAreaSize;
    cpuaddr                             resetAreaAddr;
    CFE_ES_ResetData_t *                CFE_TIME_ResetDataPtr;

    RefState = CFE_TIME_StartReferenceUpdate();

    /*
    ** Get the pointer to the Reset area from the BSP
    */
    PspStatus = CFE_PSP_GetResetArea(&(resetAreaAddr), &(resetAreaSize));

    if (PspStatus != CFE_PSP_SUCCESS)
    {
        /* There is something wrong with the Reset Area */
        CFE_TIME_Global.DataStoreStatus = CFE_TIME_RESET_AREA_BAD;
    }

    else
    {
        CFE_TIME_ResetDataPtr = (CFE_ES_ResetData_t *)resetAreaAddr;

        /* Get the structure from the Reset Area */
        LocalResetVars = CFE_TIME_ResetDataPtr->TimeResetVars;

        /*
        ** Verify TIME data signature and clock signal selection...
        **    (other data fields have no verifiable limits)
        */
        if ((LocalResetVars.Signature == CFE_TIME_RESET_SIGNATURE) &&
            ((LocalResetVars.ClockSignal == CFE_TIME_ToneSignalSelect_PRIMARY) ||
             (LocalResetVars.ClockSignal == CFE_TIME_ToneSignalSelect_REDUNDANT)))
        {
            /*
            ** Initialize TIME to valid  Reset Area values...
            */
            RefState->AtToneMET         = LocalResetVars.CurrentMET;
            RefState->AtToneSTCF        = LocalResetVars.CurrentSTCF;
            RefState->AtToneDelay       = LocalResetVars.CurrentDelay;
            RefState->AtToneLeapSeconds = LocalResetVars.LeapSeconds;
            CFE_TIME_Global.ClockSignal = LocalResetVars.ClockSignal;

            CFE_TIME_Global.DataStoreStatus = CFE_TIME_RESET_AREA_EXISTING;
        }
        else
        {
            /*
            ** We got a blank area from the reset variables
            */
            CFE_TIME_Global.DataStoreStatus = CFE_TIME_RESET_AREA_NEW;
        }
    }
    /*
    ** Initialize TIME to default values if no valid Reset data...
    */
    if (CFE_TIME_Global.DataStoreStatus != CFE_TIME_RESET_AREA_EXISTING)
    {
        DefSubsMET  = CFE_TIME_Micro2SubSecs(CFE_MISSION_TIME_DEF_MET_SUBS);
        DefSubsSTCF = CFE_TIME_Micro2SubSecs(CFE_MISSION_TIME_DEF_STCF_SUBS);

        RefState->AtToneMET.Seconds      = CFE_MISSION_TIME_DEF_MET_SECS;
        RefState->AtToneMET.Subseconds   = DefSubsMET;
        RefState->AtToneSTCF.Seconds     = CFE_MISSION_TIME_DEF_STCF_SECS;
        RefState->AtToneSTCF.Subseconds  = DefSubsSTCF;
        RefState->AtToneLeapSeconds      = CFE_MISSION_TIME_DEF_LEAPS;
        CFE_TIME_Global.ClockSignal      = CFE_TIME_ToneSignalSelect_PRIMARY;
        RefState->AtToneDelay.Seconds    = 0;
        RefState->AtToneDelay.Subseconds = 0;
    }

    CFE_TIME_FinishReferenceUpdate(RefState);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TIME_UpdateResetVars(const CFE_TIME_Reference_t *Reference)
{
    CFE_TIME_ResetVars_t LocalResetVars;
    uint32               resetAreaSize;
    cpuaddr              resetAreaAddr;
    CFE_ES_ResetData_t * CFE_TIME_ResetDataPtr;
    /*
    ** Update the data only if our Reset Area is valid...
    */
    if (CFE_TIME_Global.DataStoreStatus != CFE_TIME_RESET_AREA_ERROR)
    {
        /* Store all of our critical variables to a ResetVars_t
         * then copy that to the Reset Area */
        LocalResetVars.Signature = CFE_TIME_RESET_SIGNATURE;

        LocalResetVars.CurrentMET   = Reference->CurrentMET;
        LocalResetVars.CurrentSTCF  = Reference->AtToneSTCF;
        LocalResetVars.CurrentDelay = Reference->AtToneDelay;
        LocalResetVars.LeapSeconds  = Reference->AtToneLeapSeconds;

        LocalResetVars.ClockSignal = CFE_TIME_Global.ClockSignal;

        /*
        ** Get the pointer to the Reset area from the BSP
        */
        if (CFE_PSP_GetResetArea(&(resetAreaAddr), &(resetAreaSize)) == CFE_PSP_SUCCESS)
        {
            CFE_TIME_ResetDataPtr                = (CFE_ES_ResetData_t *)resetAreaAddr;
            CFE_TIME_ResetDataPtr->TimeResetVars = LocalResetVars;
        }
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TIME_InitData(void)
{
    uint32                              i;
    volatile CFE_TIME_ReferenceState_t *RefState;

    /* Clear task global */
    memset(&CFE_TIME_Global, 0, sizeof(CFE_TIME_Global));

    /*
    ** Initialize task configuration data...
    */
    for (i = 0; i < CFE_TIME_REFERENCE_BUF_DEPTH; ++i)
    {
        CFE_TIME_Global.ReferenceState[i].StateVersion = 0xFFFFFFFF;
    }

    /*
    ** Try to get values used to compute time from Reset Area...
    */
    CFE_TIME_QueryResetVars();

    RefState = CFE_TIME_StartReferenceUpdate();

    /*
    ** Remaining data values used to compute time...
    */
    RefState->AtToneLatch = CFE_TIME_LatchClock();

    /*
    ** Data values used to define the current clock state...
    */
    RefState->ClockSetState = CFE_TIME_SetState_NOT_SET;
    RefState->ClockFlyState = CFE_TIME_FlywheelState_IS_FLY;

#if (CFE_PLATFORM_TIME_CFG_SOURCE == true)
    CFE_TIME_Global.ClockSource = CFE_TIME_SourceSelect_EXTERNAL;
#else
    CFE_TIME_Global.ClockSource = CFE_TIME_SourceSelect_INTERNAL;
#endif
    CFE_TIME_Global.ServerFlyState = CFE_TIME_FlywheelState_IS_FLY;

    /*
    ** Pending data values (from "time at tone" command data packet)...
    */
    CFE_TIME_Global.PendingState = CFE_TIME_ClockState_INVALID;

    /*
    ** Nonzero adjustment values...
    */
    CFE_TIME_Global.OneTimeDirection = CFE_TIME_AdjustDirection_ADD;
    CFE_TIME_Global.OneHzDirection   = CFE_TIME_AdjustDirection_ADD;
    RefState->DelayDirection         = CFE_TIME_AdjustDirection_ADD;

    /*
    ** Miscellaneous counters...
    */
    CFE_TIME_Global.VirtualMET = RefState->AtToneMET.Seconds;

    /*
    ** Time window verification values...
    */
    CFE_TIME_Global.MinElapsed = CFE_TIME_Micro2SubSecs(CFE_MISSION_TIME_MIN_ELAPSED);
    CFE_TIME_Global.MaxElapsed = CFE_TIME_Micro2SubSecs(CFE_MISSION_TIME_MAX_ELAPSED);

/*
** Range checking for external time source data...
*/
#if (CFE_PLATFORM_TIME_CFG_SOURCE == true)
    CFE_TIME_Global.MaxDelta.Seconds    = CFE_PLATFORM_TIME_MAX_DELTA_SECS;
    CFE_TIME_Global.MaxDelta.Subseconds = CFE_TIME_Micro2SubSecs(CFE_PLATFORM_TIME_MAX_DELTA_SUBS);
#endif

    /*
    ** Maximum local clock value (before roll-over)...
    */
    CFE_TIME_Global.MaxLocalClock.Seconds    = CFE_PLATFORM_TIME_MAX_LOCAL_SECS;
    CFE_TIME_Global.MaxLocalClock.Subseconds = CFE_PLATFORM_TIME_MAX_LOCAL_SUBS;

    /*
    ** Range limits for time between tone signal interrupts...
    */
    CFE_TIME_Global.ToneOverLimit  = CFE_TIME_Micro2SubSecs(CFE_PLATFORM_TIME_CFG_TONE_LIMIT);
    CFE_TIME_Global.ToneUnderLimit = CFE_TIME_Micro2SubSecs((1000000 - CFE_PLATFORM_TIME_CFG_TONE_LIMIT));

    CFE_TIME_FinishReferenceUpdate(RefState);

    /*
    ** Initialize housekeeping packet (clear user data area)...
    */
    CFE_MSG_Init(CFE_MSG_PTR(CFE_TIME_Global.HkPacket.TelemetryHeader), CFE_SB_ValueToMsgId(CFE_TIME_HK_TLM_MID),
                 sizeof(CFE_TIME_Global.HkPacket));

    /*
    ** Initialize diagnostic packet (clear user data area)...
    */
    CFE_MSG_Init(CFE_MSG_PTR(CFE_TIME_Global.DiagPacket.TelemetryHeader), CFE_SB_ValueToMsgId(CFE_TIME_DIAG_TLM_MID),
                 sizeof(CFE_TIME_Global.DiagPacket));

    /*
    ** Initialize "time at the tone" signal command packet...
    */
    CFE_MSG_Init(CFE_MSG_PTR(CFE_TIME_Global.ToneSignalCmd.CommandHeader), CFE_SB_ValueToMsgId(CFE_TIME_TONE_CMD_MID),
                 sizeof(CFE_TIME_Global.ToneSignalCmd));

/*
** Initialize "time at the tone" data command packet...
*/
#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
    CFE_MSG_Init(CFE_MSG_PTR(CFE_TIME_Global.ToneDataCmd.CommandHeader), CFE_SB_ValueToMsgId(CFE_TIME_DATA_CMD_MID),
                 sizeof(CFE_TIME_Global.ToneDataCmd));
#endif

    /*
    ** Initialize simulated tone send message ("fake tone" mode only)...
    */
#if (CFE_MISSION_TIME_CFG_FAKE_TONE == true)
    CFE_MSG_Init(CFE_MSG_PTR(CFE_TIME_Global.ToneSendCmd.CommandHeader), CFE_SB_ValueToMsgId(CFE_TIME_SEND_CMD_MID),
                 sizeof(CFE_TIME_Global.ToneSendCmd));
#endif

    /*
    ** Initialize local 1Hz "wake-up" command packet (optional)...
    */
    CFE_MSG_Init(CFE_MSG_PTR(CFE_TIME_Global.LocalOneHzCmd.CommandHeader), CFE_SB_ValueToMsgId(CFE_TIME_ONEHZ_CMD_MID),
                 sizeof(CFE_TIME_Global.LocalOneHzCmd));
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TIME_GetHkData(const CFE_TIME_Reference_t *Reference)
{
    /*
    ** Get command execution counters...
    */
    CFE_TIME_Global.HkPacket.Payload.CommandCounter      = CFE_TIME_Global.CommandCounter;
    CFE_TIME_Global.HkPacket.Payload.CommandErrorCounter = CFE_TIME_Global.CommandErrorCounter;

    /*
    ** Current "as calculated" clock state...
    */
    CFE_TIME_Global.HkPacket.Payload.ClockStateAPI = (CFE_TIME_ClockState_Enum_t)CFE_TIME_CalculateState(Reference);

    /*
    ** Current clock state flags...
    */
    CFE_TIME_Global.HkPacket.Payload.ClockStateFlags = CFE_TIME_GetClockInfo();

    /*
    ** Leap Seconds...
    */
    CFE_TIME_Global.HkPacket.Payload.LeapSeconds = Reference->AtToneLeapSeconds;

    /*
    ** Current MET and STCF time values...
    */
    CFE_TIME_Global.HkPacket.Payload.SecondsMET = Reference->CurrentMET.Seconds;
    CFE_TIME_Global.HkPacket.Payload.SubsecsMET = Reference->CurrentMET.Subseconds;

    CFE_TIME_Global.HkPacket.Payload.SecondsSTCF = Reference->AtToneSTCF.Seconds;
    CFE_TIME_Global.HkPacket.Payload.SubsecsSTCF = Reference->AtToneSTCF.Subseconds;

/*
** 1Hz STCF adjustment values (server only)...
*/
#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
    CFE_TIME_Global.HkPacket.Payload.Seconds1HzAdj = CFE_TIME_Global.OneHzAdjust.Seconds;
    CFE_TIME_Global.HkPacket.Payload.Subsecs1HzAdj = CFE_TIME_Global.OneHzAdjust.Subseconds;
#endif

/*
** Time at tone delay values (client only)...
*/
#if (CFE_PLATFORM_TIME_CFG_CLIENT == true)
    CFE_TIME_Global.HkPacket.Payload.SecondsDelay = Reference->AtToneDelay.Seconds;
    CFE_TIME_Global.HkPacket.Payload.SubsecsDelay = Reference->AtToneDelay.Subseconds;
#endif
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TIME_GetDiagData(void)
{
    CFE_TIME_Reference_t Reference;
    CFE_TIME_SysTime_t   TempTime;

    /*
    ** Get reference time values (local time, time at tone, etc.)...
    */
    CFE_TIME_GetReference(&Reference);

    CFE_TIME_Copy(&CFE_TIME_Global.DiagPacket.Payload.AtToneMET, &Reference.AtToneMET);
    CFE_TIME_Copy(&CFE_TIME_Global.DiagPacket.Payload.AtToneSTCF, &Reference.AtToneSTCF);
    CFE_TIME_Copy(&CFE_TIME_Global.DiagPacket.Payload.AtToneDelay, &Reference.AtToneDelay);
    CFE_TIME_Copy(&CFE_TIME_Global.DiagPacket.Payload.AtToneLatch, &Reference.AtToneLatch);

    CFE_TIME_Global.DiagPacket.Payload.AtToneLeapSeconds = Reference.AtToneLeapSeconds;
    CFE_TIME_Global.DiagPacket.Payload.ClockStateAPI     = CFE_TIME_CalculateState(&Reference);

    /*
    ** Data values that reflect the time (right now)...
    */
    CFE_TIME_Copy(&CFE_TIME_Global.DiagPacket.Payload.TimeSinceTone, &Reference.TimeSinceTone);
    CFE_TIME_Copy(&CFE_TIME_Global.DiagPacket.Payload.CurrentLatch, &Reference.CurrentLatch);
    CFE_TIME_Copy(&CFE_TIME_Global.DiagPacket.Payload.CurrentMET, &Reference.CurrentMET);
    TempTime = CFE_TIME_CalculateTAI(&Reference);
    CFE_TIME_Copy(&CFE_TIME_Global.DiagPacket.Payload.CurrentTAI, &TempTime);
    TempTime = CFE_TIME_CalculateUTC(&Reference);
    CFE_TIME_Copy(&CFE_TIME_Global.DiagPacket.Payload.CurrentUTC, &TempTime);

    /*
    ** Data values used to define the current clock state...
    */
    CFE_TIME_Global.DiagPacket.Payload.ClockSetState  = Reference.ClockSetState;
    CFE_TIME_Global.DiagPacket.Payload.ClockFlyState  = Reference.ClockFlyState;
    CFE_TIME_Global.DiagPacket.Payload.ClockSource    = CFE_TIME_Global.ClockSource;
    CFE_TIME_Global.DiagPacket.Payload.ClockSignal    = CFE_TIME_Global.ClockSignal;
    CFE_TIME_Global.DiagPacket.Payload.ServerFlyState = CFE_TIME_Global.ServerFlyState;
    CFE_TIME_Global.DiagPacket.Payload.Forced2Fly     = (int16)CFE_TIME_Global.Forced2Fly;

    /*
    ** Clock state flags...
    */
    CFE_TIME_Global.DiagPacket.Payload.ClockStateFlags = CFE_TIME_GetClockInfo();

    /*
    ** STCF adjustment direction values...
    */
    CFE_TIME_Global.DiagPacket.Payload.OneTimeDirection = CFE_TIME_Global.OneTimeDirection;
    CFE_TIME_Global.DiagPacket.Payload.OneHzDirection   = CFE_TIME_Global.OneHzDirection;
    CFE_TIME_Global.DiagPacket.Payload.DelayDirection   = Reference.DelayDirection;

    /*
    ** STCF adjustment values...
    */
    CFE_TIME_Copy(&CFE_TIME_Global.DiagPacket.Payload.OneTimeAdjust, &CFE_TIME_Global.OneTimeAdjust);
    CFE_TIME_Copy(&CFE_TIME_Global.DiagPacket.Payload.OneHzAdjust, &CFE_TIME_Global.OneHzAdjust);

    /*
    ** Most recent local clock latch values...
    */
    CFE_TIME_Copy(&CFE_TIME_Global.DiagPacket.Payload.ToneSignalLatch, &CFE_TIME_Global.ToneSignalLatch);
    CFE_TIME_Copy(&CFE_TIME_Global.DiagPacket.Payload.ToneDataLatch, &CFE_TIME_Global.ToneDataLatch);

    /*
    ** Miscellaneous counters (subject to reset command)...
    */
    CFE_TIME_Global.DiagPacket.Payload.ToneMatchCounter      = CFE_TIME_Global.ToneMatchCounter;
    CFE_TIME_Global.DiagPacket.Payload.ToneMatchErrorCounter = CFE_TIME_Global.ToneMatchErrorCounter;
    CFE_TIME_Global.DiagPacket.Payload.ToneSignalCounter     = CFE_TIME_Global.ToneSignalCounter;
    CFE_TIME_Global.DiagPacket.Payload.ToneDataCounter       = CFE_TIME_Global.ToneDataCounter;
    CFE_TIME_Global.DiagPacket.Payload.ToneIntCounter        = CFE_TIME_Global.ToneIntCounter;
    CFE_TIME_Global.DiagPacket.Payload.ToneIntErrorCounter   = CFE_TIME_Global.ToneIntErrorCounter;
    CFE_TIME_Global.DiagPacket.Payload.ToneTaskCounter       = CFE_TIME_Global.ToneTaskCounter;
    CFE_TIME_Global.DiagPacket.Payload.VersionCounter =
        CFE_TIME_Global.LastVersionCounter - CFE_TIME_Global.ResetVersionCounter;
    CFE_TIME_Global.DiagPacket.Payload.LocalIntCounter  = CFE_TIME_Global.LocalIntCounter;
    CFE_TIME_Global.DiagPacket.Payload.LocalTaskCounter = CFE_TIME_Global.LocalTaskCounter;

    /*
    ** Miscellaneous counters (not subject to reset command)...
    */
    CFE_TIME_Global.DiagPacket.Payload.VirtualMET = CFE_TIME_Global.VirtualMET;

    /*
    ** Time window verification values (converted from micro-secs)...
    **
    ** Regardless of whether the tone follows the time packet, or vice
    **    versa, these values define the acceptable window of time for
    **    the second event to follow the first.  The minimum value may
    **    be as little as zero, and the maximum must be something less
    **    than a second.
    */
    CFE_TIME_Global.DiagPacket.Payload.MinElapsed = CFE_TIME_Global.MinElapsed;
    CFE_TIME_Global.DiagPacket.Payload.MaxElapsed = CFE_TIME_Global.MaxElapsed;

    /*
    ** Maximum local clock value (before roll-over)...
    */
    CFE_TIME_Copy(&CFE_TIME_Global.DiagPacket.Payload.MaxLocalClock, &CFE_TIME_Global.MaxLocalClock);

    /*
    ** Tone signal tolerance limits...
    */
    CFE_TIME_Global.DiagPacket.Payload.ToneOverLimit  = CFE_TIME_Global.ToneOverLimit;
    CFE_TIME_Global.DiagPacket.Payload.ToneUnderLimit = CFE_TIME_Global.ToneUnderLimit;

    /*
    ** Reset Area access status...
    */
    CFE_TIME_Global.DiagPacket.Payload.DataStoreStatus = CFE_TIME_Global.DataStoreStatus;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TIME_GetReference(CFE_TIME_Reference_t *Reference)
{
    CFE_TIME_SysTime_t                  TimeSinceTone;
    CFE_TIME_SysTime_t                  CurrentMET;
    uint32                              VersionCounter;
    uint32                              RetryCount = 4;
    volatile CFE_TIME_ReferenceState_t *RefState;

    /*
    ** VersionCounter is incremented when reference data is modified...
    */
    while (true)
    {
        VersionCounter = CFE_TIME_Global.LastVersionCounter;
        RefState       = &CFE_TIME_Global.ReferenceState[VersionCounter & CFE_TIME_REFERENCE_BUF_MASK];

        Reference->CurrentLatch = CFE_TIME_LatchClock();

        Reference->AtToneMET         = RefState->AtToneMET;
        Reference->AtToneSTCF        = RefState->AtToneSTCF;
        Reference->AtToneLeapSeconds = RefState->AtToneLeapSeconds;
        Reference->AtToneDelay       = RefState->AtToneDelay;
        Reference->AtToneLatch       = RefState->AtToneLatch;

        Reference->ClockSetState  = RefState->ClockSetState;
        Reference->ClockFlyState  = RefState->ClockFlyState;
        Reference->DelayDirection = RefState->DelayDirection;

        /*
         * If the version counter inside the state record
         * is the same value as the global _after_ copying the
         * data, then the value is considered valid.
         */
        if (VersionCounter == RefState->StateVersion)
        {
            /* successful read */
            break;
        }

        /*
         * The value was caught mid-update, so the reference data
         * might not be consistent.  Try again to read it.
         *
         * The number of retries is limited, to prevent getting
         * stuck in this loop forever.  There is currently no
         * way to handle the inability to read the time reference.
         */
        if (RetryCount == 0)
        {
            /* unsuccessful read */
            break;
        }

        --RetryCount;
    }

    /*
     * This should really never happen: if RetryCount reaches its limit, it means something is
     * continuously changing the time structure to the point where this task was not able to
     * get a consistent copy.  The only way this could happen is if some update task got into
     * a continuous loop, or if the memory itself has gone bad and reads inconsistently.  But
     * if the latter is the case, the whole system has undefined behavior.
     */
    if (RetryCount == 0)
    {
        /* set the flag that indicates this failed */
        CFE_TIME_Global.GetReferenceFail = true;

        /*
         * Zeroing out the structure produces an identifiable output, in particular
         * this sets the ClockSetState to CFE_TIME_SetState_NOT_SET, which the CFE_TIME_CalculateState()
         * will in turn translate to CFE_TIME_ClockState_INVALID in TLM.
         */
        memset(Reference, 0, sizeof(*Reference));
        return;
    }

    /*
    ** Compute the amount of time "since" the tone...
    */
    if (CFE_TIME_Compare(Reference->CurrentLatch, Reference->AtToneLatch) == CFE_TIME_A_LT_B)
    {
        /*
        ** Local clock has rolled over since last tone...
        */
        TimeSinceTone = CFE_TIME_Subtract(CFE_TIME_Global.MaxLocalClock, Reference->AtToneLatch);
        TimeSinceTone = CFE_TIME_Add(TimeSinceTone, Reference->CurrentLatch);
    }
    else
    {
        /*
        ** Normal case -- local clock is greater than latch at tone...
        */
        TimeSinceTone = CFE_TIME_Subtract(Reference->CurrentLatch, Reference->AtToneLatch);
    }

    Reference->TimeSinceTone = TimeSinceTone;

    /*
    ** Add in the MET at the tone...
    */
    CurrentMET = CFE_TIME_Add(TimeSinceTone, Reference->AtToneMET);

/*
** Synchronize "this" time client to the time server...
*/
#if (CFE_PLATFORM_TIME_CFG_CLIENT == true)
    if (Reference->DelayDirection == CFE_TIME_AdjustDirection_ADD)
    {
        CurrentMET = CFE_TIME_Add(CurrentMET, Reference->AtToneDelay);
    }
    else
    {
        CurrentMET = CFE_TIME_Subtract(CurrentMET, Reference->AtToneDelay);
    }
#endif

    Reference->CurrentMET = CurrentMET;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_TIME_SysTime_t CFE_TIME_CalculateTAI(const CFE_TIME_Reference_t *Reference)
{
    CFE_TIME_SysTime_t TimeAsTAI;

    TimeAsTAI = CFE_TIME_Add(Reference->CurrentMET, Reference->AtToneSTCF);

    return TimeAsTAI;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_TIME_SysTime_t CFE_TIME_CalculateUTC(const CFE_TIME_Reference_t *Reference)
{
    CFE_TIME_SysTime_t TimeAsUTC;

    TimeAsUTC = CFE_TIME_Add(Reference->CurrentMET, Reference->AtToneSTCF);
    TimeAsUTC.Seconds -= Reference->AtToneLeapSeconds;

    return TimeAsUTC;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_TIME_ClockState_Enum_t CFE_TIME_CalculateState(const CFE_TIME_Reference_t *Reference)
{
    CFE_TIME_ClockState_Enum_t ClockState;

    /*
    ** Determine the current clock state...
    */
    if (Reference->ClockSetState == CFE_TIME_SetState_WAS_SET)
    {
        if (Reference->ClockFlyState == CFE_TIME_FlywheelState_NO_FLY)
        {
            /*
            ** CFE_TIME_ClockState_VALID = clock set and not fly-wheeling...
            */
            ClockState = CFE_TIME_ClockState_VALID;

/*
** If the server is fly-wheel then the client must also
**    report fly-wheel (even if it is not)...
*/
#if (CFE_PLATFORM_TIME_CFG_CLIENT == true)
            if (CFE_TIME_Global.ServerFlyState == CFE_TIME_FlywheelState_IS_FLY)
            {
                ClockState = CFE_TIME_ClockState_FLYWHEEL;
            }
#endif
        }
        else
        {
            /*
            ** CFE_TIME_ClockState_FLYWHEEL = clock set and fly-wheeling...
            */
            ClockState = CFE_TIME_ClockState_FLYWHEEL;
        }
    }
    else
    {
        /*
        ** CFE_TIME_ClockState_INVALID = clock not set...
        */
        ClockState = CFE_TIME_ClockState_INVALID;
    }

    return ClockState;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TIME_SetState(CFE_TIME_ClockState_Enum_t NewState)
{
    volatile CFE_TIME_ReferenceState_t *RefState;

    RefState = CFE_TIME_StartReferenceUpdate();

    /*
    ** If we get a command to set the clock to "flywheel" mode, then
    **    set a global flag so that we can choose to ignore time
    **    updates until we get another clock state command...
    */
    if (NewState == CFE_TIME_ClockState_FLYWHEEL)
    {
        CFE_TIME_Global.Forced2Fly = true;
        RefState->ClockFlyState    = CFE_TIME_FlywheelState_IS_FLY;
#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
        CFE_TIME_Global.ServerFlyState = CFE_TIME_FlywheelState_IS_FLY;
#endif
    }
    else if (NewState == CFE_TIME_ClockState_VALID)
    {
        CFE_TIME_Global.Forced2Fly = false;
        RefState->ClockSetState    = CFE_TIME_SetState_WAS_SET;
    }
    else
    {
        CFE_TIME_Global.Forced2Fly = false;
        RefState->ClockSetState    = CFE_TIME_SetState_NOT_SET;
    }

    /*
    ** Time has changed, force anyone reading time to retry...
    */
    CFE_TIME_FinishReferenceUpdate(RefState);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
#if (CFE_PLATFORM_TIME_CFG_SOURCE == true)
void CFE_TIME_SetSource(int16 NewSource)
{
    CFE_TIME_Global.ClockSource = NewSource;
}
#endif /* CFE_PLATFORM_TIME_CFG_SOURCE */

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
#if (CFE_PLATFORM_TIME_CFG_SIGNAL == true)
void CFE_TIME_SetSignal(int16 NewSignal)
{
    /*
    ** Maintain current tone signal selection for telemetry...
    */
    CFE_TIME_Global.ClockSignal = NewSignal;
}
#endif /* CFE_PLATFORM_TIME_CFG_SIGNAL */

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
#if (CFE_PLATFORM_TIME_CFG_CLIENT == true)
void CFE_TIME_SetDelay(CFE_TIME_SysTime_t NewDelay, int16 Direction)
{
    volatile CFE_TIME_ReferenceState_t *RefState;

    RefState = CFE_TIME_StartReferenceUpdate();

    RefState->AtToneDelay    = NewDelay;
    RefState->DelayDirection = Direction;

    /*
    ** Time has changed, force anyone reading time to retry...
    */
    CFE_TIME_FinishReferenceUpdate(RefState);
}
#endif /* CFE_PLATFORM_TIME_CFG_CLIENT */

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
void CFE_TIME_SetTime(CFE_TIME_SysTime_t NewTime)
{
    volatile CFE_TIME_ReferenceState_t *RefState;

    /*
    ** The input to this function is a time value that includes MET
    **     and STCF.  If the default time format is UTC, the input
    **     time value has had leaps seconds removed from the total.
    */
    CFE_TIME_Reference_t Reference;
    CFE_TIME_SysTime_t   NewSTCF;

    /*
    ** Get reference time values (local time, time at tone, etc.)...
    */
    CFE_TIME_GetReference(&Reference);

    /*
    ** Remove current MET from the new time value (leaves STCF)...
    */
    NewSTCF = CFE_TIME_Subtract(NewTime, Reference.CurrentMET);

/*
** Restore leap seconds if default time format is UTC...
*/
#if (CFE_MISSION_TIME_CFG_DEFAULT_UTC == true)
    NewSTCF.Seconds += Reference.AtToneLeapSeconds;
#endif

    RefState = CFE_TIME_StartReferenceUpdate();

    RefState->AtToneSTCF = NewSTCF;

    /*
    ** Time has changed, force anyone reading time to retry...
    */
    CFE_TIME_FinishReferenceUpdate(RefState);
}
#endif /* CFE_PLATFORM_TIME_CFG_SERVER */

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
void CFE_TIME_SetMET(CFE_TIME_SysTime_t NewMET)
{
    volatile CFE_TIME_ReferenceState_t *RefState;

    RefState = CFE_TIME_StartReferenceUpdate();

    /*
    ** Update reference values used to compute current time...
    */
    RefState->AtToneMET        = NewMET;
    CFE_TIME_Global.VirtualMET = NewMET.Seconds;
    RefState->AtToneLatch      = CFE_TIME_LatchClock();

/*
** Update h/w MET register...
*/
#if (CFE_PLATFORM_TIME_CFG_VIRTUAL != true)
    OS_SetLocalMET(NewMET.Seconds);
#endif

    /*
    ** Time has changed, force anyone reading time to retry...
    */
    CFE_TIME_FinishReferenceUpdate(RefState);
}
#endif /* CFE_PLATFORM_TIME_CFG_SERVER */

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
void CFE_TIME_SetSTCF(CFE_TIME_SysTime_t NewSTCF)
{
    volatile CFE_TIME_ReferenceState_t *RefState;

    RefState = CFE_TIME_StartReferenceUpdate();

    RefState->AtToneSTCF = NewSTCF;

    /*
    ** Time has changed, force anyone reading time to retry...
    */
    CFE_TIME_FinishReferenceUpdate(RefState);
}
#endif /* CFE_PLATFORM_TIME_CFG_SERVER */

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
void CFE_TIME_SetLeapSeconds(int16 NewLeaps)
{
    volatile CFE_TIME_ReferenceState_t *RefState;

    RefState = CFE_TIME_StartReferenceUpdate();

    RefState->AtToneLeapSeconds = NewLeaps;

    /*
    ** Time has changed, force anyone reading time to retry...
    */
    CFE_TIME_FinishReferenceUpdate(RefState);
}
#endif /* CFE_PLATFORM_TIME_CFG_SERVER */

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
void CFE_TIME_SetAdjust(CFE_TIME_SysTime_t NewAdjust, int16 Direction)
{
    CFE_TIME_SysTime_t                  NewSTCF;
    volatile CFE_TIME_ReferenceState_t *RefState;

    RefState = CFE_TIME_StartReferenceUpdate();

    CFE_TIME_Global.OneTimeAdjust    = NewAdjust;
    CFE_TIME_Global.OneTimeDirection = Direction;

    if (Direction == CFE_TIME_AdjustDirection_ADD)
    {
        NewSTCF = CFE_TIME_Add(RefState->AtToneSTCF, NewAdjust);
    }
    else
    {
        NewSTCF = CFE_TIME_Subtract(RefState->AtToneSTCF, NewAdjust);
    }

    RefState->AtToneSTCF = NewSTCF;

    /*
    ** Time has changed, force anyone reading time to retry...
    */
    CFE_TIME_FinishReferenceUpdate(RefState);
}
#endif /* CFE_PLATFORM_TIME_CFG_SERVER */

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
void CFE_TIME_Set1HzAdj(CFE_TIME_SysTime_t NewAdjust, int16 Direction)
{
    /*
    ** Store values for 1Hz adjustment...
    */
    CFE_TIME_Global.OneHzAdjust    = NewAdjust;
    CFE_TIME_Global.OneHzDirection = Direction;
}
#endif /* CFE_PLATFORM_TIME_CFG_SERVER */

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_TIME_CleanUpApp(CFE_ES_AppId_t AppId)
{
    int32  Status;
    uint32 AppIndex;

    Status = CFE_ES_AppID_ToIndex(AppId, &AppIndex);
    if (Status != CFE_SUCCESS)
    {
        /* Do nothing */
    }
    else if (AppIndex < (sizeof(CFE_TIME_Global.SynchCallback) / sizeof(CFE_TIME_Global.SynchCallback[0])))
    {
        CFE_TIME_Global.SynchCallback[AppIndex].Ptr = NULL;
    }
    else
    {
        Status = CFE_TIME_CALLBACK_NOT_REGISTERED;
    }

    return Status;
}
