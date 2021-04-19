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

/**
 * @file
 *
 * Purpose:  cFE Time Services (TIME) library utilities header file
 *
 * Author:   S.Walling/Microtel
 *
 * Notes:
 *
 */

#ifndef CFE_TIME_UTILS_H
#define CFE_TIME_UTILS_H

/*
** Required header files...
*/
#include "cfe_time_module_all.h"

/*************************************************************************/

#define CFE_TIME_NEGATIVE 0x80000000 /* ~ 68 * 31,536,000 seconds */

/*************************************************************************/

/*
** Main task definitions...
*/
#define CFE_TIME_TASK_NAME "CFE_TIME"

/*
** Interrupt task definitions...
*/
#define CFE_TIME_TASK_TONE_NAME "TIME_TONE_TASK"
#define CFE_TIME_TASK_1HZ_NAME  "TIME_1HZ_TASK"
#define CFE_TIME_TASK_STACK_PTR CFE_ES_TASK_STACK_ALLOCATE
#define CFE_TIME_TASK_FLAGS     0

/*
** Interrupt semaphore definitions...
*/
#define CFE_TIME_SEM_TONE_NAME "TIME_TONE_SEM"
#define CFE_TIME_SEM_1HZ_NAME  "TIME_1HZ_SEM"
#define CFE_TIME_SEM_VALUE     0
#define CFE_TIME_SEM_OPTIONS   0

/*
** Main Task Pipe definitions...
*/

#define CFE_TIME_TASK_PIPE_NAME  "TIME_CMD_PIPE"
#define CFE_TIME_TASK_PIPE_DEPTH 12

/*
** Reset Area state state at startup...
*/

#define CFE_TIME_RESET_AREA_ERROR    1 /* no mem available */
#define CFE_TIME_RESET_AREA_BAD      2 /* had invalid data */
#define CFE_TIME_RESET_AREA_NEW      3 /* new memory block */
#define CFE_TIME_RESET_AREA_EXISTING 4 /* had valid data   */

/*
 * Definitions for time reference multi-buffering
 *
 * To allow higher priority tasks and ISRs to get the time reference,
 * it must be buffered in case the ISR occurs mid-update.
 *
 * This controls the depth of the buffer.  Higher values will be
 * more resilient to concurrent updates at the cost of using more
 * memory.
 *
 * Note that the 1Hz state machine can make several updates to this
 * in rapid succession, and the "fake tone" processing tied to 1Hz
 * might make another update.
 *
 * This must be a power of 2.
 */
#define CFE_TIME_REFERENCE_BUF_DEPTH 4
#define CFE_TIME_REFERENCE_BUF_MASK  (CFE_TIME_REFERENCE_BUF_DEPTH - 1)

/*************************************************************************/

/*
** Type definition (time reference data)...
*/
typedef struct
{

    CFE_TIME_SysTime_t AtToneMET;         /* MET at time of tone */
    CFE_TIME_SysTime_t AtToneSTCF;        /* STCF at time of tone */
    int16              AtToneLeapSeconds; /* Leap Seconds at time of tone */
    int16              ClockSetState;     /* Time has been "set" */
    int16              ClockFlyState;     /* Current fly-wheel state */
    int16              DelayDirection;    /* Wheter "AtToneDelay" is add or subtract */
    CFE_TIME_SysTime_t AtToneDelay;       /* Adjustment for slow tone detection */
    CFE_TIME_SysTime_t AtToneLatch;       /* Local clock latched at time of tone */
    CFE_TIME_SysTime_t CurrentLatch;      /* Local clock latched just "now" */
    CFE_TIME_SysTime_t TimeSinceTone;     /* Time elapsed since the tone */
    CFE_TIME_SysTime_t CurrentMET;        /* MET at this instant */

} CFE_TIME_Reference_t;

/*
** Time Synchronization Callback Registry Information
*/
typedef struct
{
    volatile CFE_TIME_SynchCallbackPtr_t Ptr; /**< \brief Pointer to Callback function */
} CFE_TIME_SynchCallbackRegEntry_t;

/*
** Data values used to compute time (in reference to "tone")...
**
** These are all the global values used during CFE_TIME_GetReference()
** to compute the current reference time.  They are kept in a separate
** structure so every update can be synchronized.
*/
typedef struct
{
    uint32 StateVersion;

    int16 AtToneLeapSeconds;
    int16 ClockSetState;
    int16 ClockFlyState;
    int16 DelayDirection;

    CFE_TIME_SysTime_t AtToneMET;
    CFE_TIME_SysTime_t AtToneSTCF;
    CFE_TIME_SysTime_t AtToneDelay;
    CFE_TIME_SysTime_t AtToneLatch;

} CFE_TIME_ReferenceState_t;

/*************************************************************************/

/*
** Type definition (time task global data)...
*/
typedef struct
{
    /*
    ** Task command interface counters...
    */
    uint8 CommandCounter;
    uint8 CommandErrorCounter;

    /*
    ** Task housekeeping and diagnostics telemetry packets...
    */
    CFE_TIME_HousekeepingTlm_t HkPacket;
    CFE_TIME_DiagnosticTlm_t   DiagPacket;

    /*
    ** Task operational data (not reported in housekeeping)...
    */
    CFE_SB_PipeId_t CmdPipe;

    /*
    ** Task initialization data (not reported in housekeeping)...
    */
    int16 ClockSource;
    int16 ClockSignal;
    int16 ServerFlyState;

    /*
    ** Pending data values (from "time at tone" command data)...
    */
    CFE_TIME_SysTime_t PendingMET;
    CFE_TIME_SysTime_t PendingSTCF;
    int16              PendingLeaps;
    int16              PendingState;

    /*
    ** STCF adjustment values...
    */
    CFE_TIME_SysTime_t OneTimeAdjust;
    CFE_TIME_SysTime_t OneHzAdjust;

    int16 OneTimeDirection; /* Add = true */
    int16 OneHzDirection;

    /*
    ** Most recent local clock latch values...
    */
    CFE_TIME_SysTime_t ToneSignalLatch; /* Latched at tone */
    CFE_TIME_SysTime_t ToneDataLatch;   /* Latched at packet */

    /*
    ** Miscellaneous counters...
    */
    uint32 ToneMatchCounter;      /* Tone and data match */
    uint32 ToneMatchErrorCounter; /* Tone and data mismatch */
    uint32 ToneSignalCounter;     /* Tone signal commands */
    uint32 ToneDataCounter;       /* Tone data commands */
    uint32 ToneIntCounter;        /* Tone interrupts (valid) */
    uint32 ToneIntErrorCounter;   /* Tone interrupts (invalid) */
    uint32 ToneTaskCounter;       /* Tone task wake-ups */
    uint32 VirtualMET;            /* Software MET */
    uint32 LocalIntCounter;       /* Local 1Hz interrupts */
    uint32 LocalTaskCounter;      /* Local 1Hz task wake-ups */
    uint32 InternalCount;         /* Time from internal data */
    uint32 ExternalCount;         /* Time from external data */

    volatile CFE_TIME_ReferenceState_t ReferenceState[CFE_TIME_REFERENCE_BUF_DEPTH];
    volatile uint32                    LastVersionCounter;  /* Completed Updates to "AtTone" values */
    uint32                             ResetVersionCounter; /* Version counter at last counter reset */

    /*
    ** Time window verification values (converted from micro-secs)...
    **
    ** Regardless of whether the tone follows the time packet, or vice
    **    versa, these values define the acceptable window of time for
    **    the second event to follow the first.  The minimum value may
    **    be as little as zero, and the maximum must be something less
    **    than a second.
    */
    uint32 MinElapsed;
    uint32 MaxElapsed;

    /*
    ** Maximum local clock value (before roll-over)...
    */
    CFE_TIME_SysTime_t MaxLocalClock;

    /*
    ** Clock state has been commanded into (CFE_TIME_ClockState_FLYWHEEL)...
    */
    bool Forced2Fly;

    /*
    ** Clock state has just transitioned into (CFE_TIME_ClockState_FLYWHEEL)...
    **   (not in HK since it won't be true long enough to detect)
    */

    bool AutoStartFly;
    bool IsToneGood;

    /*
    ** Spare byte for alignment
    */
    bool Spare;

    /*
    ** Local 1Hz wake-up command packet (not related to time at tone)...
    */
    CFE_TIME_1HzCmd_t Local1HzCmd;

    /*
    ** Time at the tone command packets (sent by time servers)...
    */
    CFE_TIME_ToneDataCmd_t   ToneDataCmd;
    CFE_TIME_ToneSignalCmd_t ToneSignalCmd;

    /*
     * Normally "tone send" commands come from the scheduler based on the
     * configured action table, so it occurs at the right point between tones.
     *
     * However when "fake tone" mode is enabled, then we will locally generate the
     * "tone send" message as part of the Tone task, in addition to the regular
     * "tone signal" message above.
     */
#if (CFE_MISSION_TIME_CFG_FAKE_TONE == true)
    CFE_TIME_FakeToneCmd_t ToneSendCmd;
#endif

    /*
    ** Interrupt task semaphores...
    */
    osal_id_t LocalSemaphore;
    osal_id_t ToneSemaphore;
    /*
    ** Interrupt task ID's...
    */
    CFE_ES_TaskId_t LocalTaskID;
    CFE_ES_TaskId_t ToneTaskID;

    /*
    ** Maximum difference from expected for external time sources...
    */

    CFE_TIME_SysTime_t MaxDelta;

    /*
    ** Tone signal tolerance limits...
    */
    uint32 ToneOverLimit;
    uint32 ToneUnderLimit;

    /*
    ** Reset Area ...
    */
    uint32 DataStoreStatus;

    /*
    ** Synchronization Callback Registry
    ** One callback per app is allowed
    */
    CFE_TIME_SynchCallbackRegEntry_t SynchCallback[CFE_PLATFORM_ES_MAX_APPLICATIONS];

} CFE_TIME_Global_t;

/*
** Time task global data (from "cfe_time_task.c")...
*/
extern CFE_TIME_Global_t CFE_TIME_Global;

/*************************************************************************/
/*
** Function prototypes (get local clock)...
*/
CFE_TIME_SysTime_t CFE_TIME_LatchClock(void);

/*
** Function prototypes (Time Services utilities data)...
*/
int32 CFE_TIME_TaskInit(void);
void  CFE_TIME_TaskPipe(CFE_SB_Buffer_t *SBBufPtr);
void  CFE_TIME_InitData(void);
void  CFE_TIME_QueryResetVars(void);
void  CFE_TIME_UpdateResetVars(const CFE_TIME_Reference_t *Reference);
void  CFE_TIME_GetDiagData(void);
void  CFE_TIME_GetHkData(const CFE_TIME_Reference_t *Reference);

/*
** Function prototypes (reference)...
*/
void CFE_TIME_GetReference(CFE_TIME_Reference_t *Reference);

/*
** Function prototypes (calculate TAI/UTC)...
*/
CFE_TIME_SysTime_t CFE_TIME_CalculateTAI(const CFE_TIME_Reference_t *Reference);
CFE_TIME_SysTime_t CFE_TIME_CalculateUTC(const CFE_TIME_Reference_t *Reference);

int16 CFE_TIME_CalculateState(const CFE_TIME_Reference_t *Reference);

/*
** Function prototypes (set time globals)...
*/
void CFE_TIME_SetState(int16 NewState);
#if (CFE_PLATFORM_TIME_CFG_SOURCE == true)
void CFE_TIME_SetSource(int16 NewSource);
#endif

#if (CFE_PLATFORM_TIME_CFG_SIGNAL == true)
void CFE_TIME_SetSignal(int16 NewSignal);
#endif

#if (CFE_PLATFORM_TIME_CFG_CLIENT == true)
void CFE_TIME_SetDelay(CFE_TIME_SysTime_t NewDelay, int16 Direction);
#endif
#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
void CFE_TIME_SetTime(CFE_TIME_SysTime_t NewTime);
void CFE_TIME_SetMET(CFE_TIME_SysTime_t NewMET);
void CFE_TIME_SetSTCF(CFE_TIME_SysTime_t NewSTCF);
void CFE_TIME_SetLeapSeconds(int16 NewLeaps);
void CFE_TIME_SetAdjust(CFE_TIME_SysTime_t NewAdjust, int16 Direction);
void CFE_TIME_Set1HzAdj(CFE_TIME_SysTime_t NewAdjust, int16 Direction);
#endif

/*
** Function prototypes (send time at tone data packet -- local MET)...
*/
#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
void CFE_TIME_ToneSend(void); /* signal to send time at tone packet */
#endif

/*
** Function prototypes (send time at tone data packet -- external time)...
*/
#if (CFE_PLATFORM_TIME_CFG_SRC_MET == true)
int32 CFE_TIME_ToneSendMET(CFE_TIME_SysTime_t NewMET);
#endif

#if (CFE_PLATFORM_TIME_CFG_SRC_GPS == true)
int32 CFE_TIME_ToneSendGPS(CFE_TIME_SysTime_t NewTime, int16 NewLeaps);
#endif

#if (CFE_PLATFORM_TIME_CFG_SRC_TIME == true)
int32 CFE_TIME_ToneSendTime(CFE_TIME_SysTime_t NewTime);
#endif

/*
 * Helper function for updating the "Reference" value
 * This is the local replacement for "OS_IntLock()"
 */
volatile CFE_TIME_ReferenceState_t *CFE_TIME_StartReferenceUpdate(void);

/*
 * Helper function for updating the "Reference" value
 * This is the local replacement for "OS_IntUnlock()"
 */
static inline void CFE_TIME_FinishReferenceUpdate(volatile CFE_TIME_ReferenceState_t *NextState)
{
    CFE_TIME_Global.LastVersionCounter = NextState->StateVersion;
}

/*
 * Helper function for getting the "Reference" value
 * This is the replacement for direct memory reads of
 * the state info from the global data structure.
 */
static inline volatile CFE_TIME_ReferenceState_t *CFE_TIME_GetReferenceState(void)
{
    return &CFE_TIME_Global.ReferenceState[CFE_TIME_Global.LastVersionCounter & CFE_TIME_REFERENCE_BUF_MASK];
}

/*
** Function prototypes (process time at the tone signal and data packet)...
*/
void CFE_TIME_ToneSignal(void);
void CFE_TIME_ToneData(const CFE_TIME_ToneDataCmd_Payload_t *ToneDataCmd);
void CFE_TIME_ToneVerify(CFE_TIME_SysTime_t Time1, CFE_TIME_SysTime_t Time2);
void CFE_TIME_ToneUpdate(void);

/*
** Function prototypes (tone 1Hz interrupt)...
*/
void CFE_TIME_Tone1HzISR(void);
void CFE_TIME_Tone1HzTask(void);
void CFE_TIME_NotifyTimeSynchApps(void);

/*
** Function prototypes (local 1Hz interrupt)...
*/
void CFE_TIME_Local1HzTask(void);
void CFE_TIME_Local1HzStateMachine(void);
void CFE_TIME_Local1HzTimerCallback(osal_id_t TimerId, void *Arg);

#endif /* CFE_TIME_UTILS_H */
