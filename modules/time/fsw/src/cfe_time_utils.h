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
#define CFE_TIME_TASK_TONE_NAME  "TIME_TONE_TASK"
#define CFE_TIME_TASK_ONEHZ_NAME "TIME_ONEHZ_TASK"
#define CFE_TIME_TASK_STACK_PTR  CFE_ES_TASK_STACK_ALLOCATE
#define CFE_TIME_TASK_FLAGS      0

/*
** Interrupt semaphore definitions...
*/
#define CFE_TIME_SEM_TONE_NAME  "TIME_TONE_SEM"
#define CFE_TIME_SEM_ONEHZ_NAME "TIME_ONEHZ_SEM"
#define CFE_TIME_SEM_VALUE      0
#define CFE_TIME_SEM_OPTIONS    0

/*
** Main Task Pipe definitions...
*/

#define CFE_TIME_TASK_PIPE_NAME  "TIME_CMD_PIPE"
#define CFE_TIME_TASK_PIPE_DEPTH 12

/*
** Reset Area state at startup...
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
    int16              DelayDirection;    /* Whether "AtToneDelay" is add or subtract */
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
    CFE_TIME_SysTime_t         PendingMET;
    CFE_TIME_SysTime_t         PendingSTCF;
    int16                      PendingLeaps;
    CFE_TIME_ClockState_Enum_t PendingState;

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
    ** Flag that indicates if "CFE_TIME_GetReference()" ever failed to get a valid time
    */
    bool GetReferenceFail;

    /*
    ** Local 1Hz wake-up command packet (not related to time at tone)...
    */
    CFE_TIME_OneHzCmd_t LocalOneHzCmd;

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

/*---------------------------------------------------------------------------------------*/
/**
 * @brief query local clock
 */
CFE_TIME_SysTime_t CFE_TIME_LatchClock(void);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Time task initialization
 */
int32 CFE_TIME_TaskInit(void);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Initialize global time task data
 */
void CFE_TIME_InitData(void);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief query contents of Reset Variables
 */
void CFE_TIME_QueryResetVars(void);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief update contents of Reset Variables
 */
void CFE_TIME_UpdateResetVars(const CFE_TIME_Reference_t *Reference);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Report diagnostics data
 */
void CFE_TIME_GetDiagData(void);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Report local housekeeping data
 */
void CFE_TIME_GetHkData(const CFE_TIME_Reference_t *Reference);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief get reference data (time at "tone")
 */
void CFE_TIME_GetReference(CFE_TIME_Reference_t *Reference);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief calculate TAI from reference data
 */
CFE_TIME_SysTime_t CFE_TIME_CalculateTAI(const CFE_TIME_Reference_t *Reference);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief calculate UTC from reference data
 */
CFE_TIME_SysTime_t CFE_TIME_CalculateUTC(const CFE_TIME_Reference_t *Reference);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief determine current time state (per API)
 */
CFE_TIME_ClockState_Enum_t CFE_TIME_CalculateState(const CFE_TIME_Reference_t *Reference);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief set clock state
 */
void CFE_TIME_SetState(CFE_TIME_ClockState_Enum_t NewState);

#if (CFE_PLATFORM_TIME_CFG_SOURCE == true)

/*---------------------------------------------------------------------------------------*/
/**
 * @brief  set clock source
 */
void CFE_TIME_SetSource(int16 NewSource);
#endif

#if (CFE_PLATFORM_TIME_CFG_SIGNAL == true)

/*---------------------------------------------------------------------------------------*/
/**
 * @brief set tone signal (pri vs red)
 */
void CFE_TIME_SetSignal(int16 NewSignal);
#endif

#if (CFE_PLATFORM_TIME_CFG_CLIENT == true)

/*---------------------------------------------------------------------------------------*/
/**
 * @brief set tone delay (time client only)
 */
void CFE_TIME_SetDelay(CFE_TIME_SysTime_t NewDelay, int16 Direction);
#endif

#if (CFE_PLATFORM_TIME_CFG_SERVER == true)

/*---------------------------------------------------------------------------------------*/
/**
 * @brief set time (time server only)
 */
void CFE_TIME_SetTime(CFE_TIME_SysTime_t NewTime);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief set MET (time server only)
 */
void CFE_TIME_SetMET(CFE_TIME_SysTime_t NewMET);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief set STCF (time server only)
 */
void CFE_TIME_SetSTCF(CFE_TIME_SysTime_t NewSTCF);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief set leap seconds (time server only)
 */
void CFE_TIME_SetLeapSeconds(int16 NewLeaps);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief one time STCF adjustment (server only)
 */
void CFE_TIME_SetAdjust(CFE_TIME_SysTime_t NewAdjust, int16 Direction);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief 1Hz STCF adjustment (time server only)
 */
void CFE_TIME_Set1HzAdj(CFE_TIME_SysTime_t NewAdjust, int16 Direction);
#endif

/*
** Function prototypes (send time at tone data packet -- local MET)...
*/
#if (CFE_PLATFORM_TIME_CFG_SERVER == true)

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Send "time at the tone" (local time)
 *
 * the appropriate time (relative to the tone) such that the
 * "time at the tone" data command will arrive within the
 * specified window for tone and data packet verification.
 */
void CFE_TIME_ToneSend(void); /* signal to send time at tone packet */
#endif

/*
** Function prototypes (send time at tone data packet -- external time)...
*/
#if (CFE_PLATFORM_TIME_CFG_SRC_MET == true)

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Send "time at tone" (external MET)
 *
 * the appropriate time (relative to the tone) such that the
 * "time at the tone" data command will arrive within the
 * specified window for tone and data packet verification.
 */
int32 CFE_TIME_ToneSendMET(CFE_TIME_SysTime_t NewMET);
#endif

#if (CFE_PLATFORM_TIME_CFG_SRC_GPS == true)

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Send "time at tone" (external GPS)
 *
 * the appropriate time (relative to the tone) such that the
 * "time at the tone" data command will arrive within the
 * specified window for tone and data packet verification.
 */
int32 CFE_TIME_ToneSendGPS(CFE_TIME_SysTime_t NewTime, int16 NewLeaps);
#endif

#if (CFE_PLATFORM_TIME_CFG_SRC_TIME == true)

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Send "time at tone" (external time)
 *
 * the appropriate time (relative to the tone) such that the
 * "time at the tone" data command will arrive within the
 * specified window for tone and data packet verification.
 */
int32 CFE_TIME_ToneSendTime(CFE_TIME_SysTime_t NewTime);
#endif

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Initiate an update to the global time reference data
 *
 * Helper function for updating the "Reference" value
 * This is the local replacement for "OS_IntLock()"
 */
volatile CFE_TIME_ReferenceState_t *CFE_TIME_StartReferenceUpdate(void);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Complete an update to the global time reference data
 *
 * Helper function for updating the "Reference" value
 * This is the local replacement for "OS_IntUnlock()"
 */
static inline void CFE_TIME_FinishReferenceUpdate(volatile CFE_TIME_ReferenceState_t *NextState)
{
    CFE_TIME_Global.LastVersionCounter = NextState->StateVersion;
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Get a pointer to the global time reference data
 *
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

/*---------------------------------------------------------------------------------------*/
/**
 * @brief "tone signal" occurred recently
 *
 * This function is called upon receipt of a command indicating
 * that a time at the tone signal was detected.  The mission
 * dependent h/w or s/w that detected the tone signal latched
 * the local clock and generated this command.  The use of a
 * command announcing the tone signal ensures that this code
 * is not called from within an interrupt handler.
 *
 * It is not a concern that some amount of time has elapsed since
 * the tone actually occurred.  We are currently computing
 * time as a delta (as measured on our local clock) from a
 * previously latched tone.  It just doesn't matter if the
 * size of the delta slightly exceeds a second.  The quality
 * of our local clock will always be sufficient to measure
 * time for a couple of seconds.
 */
void CFE_TIME_ToneSignal(void);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief process "time at tone" data packet
 */
void CFE_TIME_ToneData(const CFE_TIME_ToneDataCmd_Payload_t *ToneDataCmd);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief validate tone and data packet
 *
 * If the data packet is designed to arrive after the tone, then
 *
 *    Time1 = local clock latched at the detection of the tone
 *    Time2 = local clock latched at the arrival of the packet
 *
 *
 * If the data packet is designed to arrive before the tone, then
 *
 *    Time1 = local clock latched at the arrival of the packet
 *    Time2 = local clock latched at the detection of the tone
 *
 * In either case, Time1 occurred before Time2
 */
void CFE_TIME_ToneVerify(CFE_TIME_SysTime_t Time1, CFE_TIME_SysTime_t Time2);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief process "matching" tone & data packet
 */
void CFE_TIME_ToneUpdate(void);

/*
** Function prototypes (tone 1Hz interrupt)...
*/

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Tone signal ISR
 */
void CFE_TIME_Tone1HzISR(void);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Tone 1Hz task
 *
 * This task exists solely to generate the tone signal command.
 */
void CFE_TIME_Tone1HzTask(void);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Call App Synch Callback Funcs
 */
void CFE_TIME_NotifyTimeSynchApps(void);

/*
** Function prototypes (local 1Hz interrupt)...
*/

/*---------------------------------------------------------------------------------------*/
/**
 * @brief  Local 1Hz task (not the tone)
 *
 * This task exists solely to generate the 1Hz wakeup command.
 *
 * This is a temporary solution until a scheduler is implemented.
 */
void CFE_TIME_Local1HzTask(void);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Update the TIME state, should be called at 1Hz
 */
void CFE_TIME_Local1HzStateMachine(void);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief  1Hz callback routine
 *
 * This is a wrapper around CFE_TIME_Local1HzISR that conforms to
 * the prototype of an OSAL Timer callback routine.
 */
void CFE_TIME_Local1HzTimerCallback(osal_id_t TimerId, void *Arg);

/*
** Command handler for "HK request"...
*/

/*---------------------------------------------------------------------------------------*/
/**
 * @brief  Onboard command (HK request)
 */
int32 CFE_TIME_SendHkCmd(const CFE_TIME_SendHkCmd_t *data);

/*
** Command handler for "tone signal detected"...
*/

/*---------------------------------------------------------------------------------------*/
/**
 * @brief  Time at tone command (signal)
 */
int32 CFE_TIME_ToneSignalCmd(const CFE_TIME_ToneSignalCmd_t *data);

/*
** Command handler for "time at the tone"...
*/

/*---------------------------------------------------------------------------------------*/
/**
 * @brief  Time at tone command (data)
 */
int32 CFE_TIME_ToneDataCmd(const CFE_TIME_ToneDataCmd_t *data);

/*
** Command handler for 1Hz signal...
*/

/*---------------------------------------------------------------------------------------*/
/**
 * @brief  Execute state machine tasks required at 1Hz
 *
 * Service the "1Hz" notification message, and perform any state machine
 * tasks that are intended to be executed at local 1Hz intervals.
 *
 * This also implements the "fake tone" functionality when that is enabled,
 * as we do not need a separate MID for this job.
 */
int32 CFE_TIME_OneHzCmd(const CFE_TIME_OneHzCmd_t *data);

#if (CFE_PLATFORM_TIME_CFG_SERVER == true)

/*---------------------------------------------------------------------------------------*/
/**
 * @brief  Time at tone command (send data)
 *
 * Command handler for "request time at the tone"...
 *
 * @note This command (sent by the scheduler) is used to
 *       signal that now is the right time (in relation
 *       to the "real" tone signal) for a Time Server to
 *       send the "time at the tone" data packet.  We do
 *       not need (or want) this command if we are not a
 *       Time Server.
 *
 *       In "fake tone" mode this command is locally generated
 *       however it is still sent via the software bus, thereby
 *       utilizing (mostly) the same code path as the
 *       non-fake tone mode.
 */
int32 CFE_TIME_ToneSendCmd(const CFE_TIME_FakeToneCmd_t *data);
#endif

/*
 * Ground command helper functions
 */

/*---------------------------------------------------------------------------------------*/
/**
 * @brief  Time task ground command (tone delay)
 */
void CFE_TIME_SetDelayImpl(const CFE_TIME_TimeCmd_Payload_t *CommandPtr, CFE_TIME_AdjustDirection_Enum_t Direction);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief  Time task ground command (1Hz adjust)
 */
void CFE_TIME_1HzAdjImpl(const CFE_TIME_OneHzAdjustmentCmd_Payload_t *CommandPtr,
                         CFE_TIME_AdjustDirection_Enum_t              Direction);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief  Time task ground command (adjust STCF)
 */
void CFE_TIME_AdjustImpl(const CFE_TIME_TimeCmd_Payload_t *CommandPtr, CFE_TIME_AdjustDirection_Enum_t Direction);

/*
** Ground command handlers...
*/

/*---------------------------------------------------------------------------------------*/
/**
 * @brief  Time task ground command (1Hz adjust)
 *
 * This is a wrapper around CFE_TIME_1HzAdjImpl()
 */
int32 CFE_TIME_AddOneHzAdjustmentCmd(const CFE_TIME_AddOneHzAdjustmentCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief  Time task ground command (1Hz adjust)
 *
 * This is a wrapper around CFE_TIME_AdjustImpl()
 */
int32 CFE_TIME_AddAdjustCmd(const CFE_TIME_AddAdjustCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief  Time task ground command (tone delay)
 *
 * Wrapper around CFE_TIME_SetDelayImpl() for add/subtract operations
 */
int32 CFE_TIME_AddDelayCmd(const CFE_TIME_AddDelayCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief  Time task ground command (tone delay)
 *
 * Wrapper around CFE_TIME_SetDelayImpl() for add/subtract operations
 */
int32 CFE_TIME_SubDelayCmd(const CFE_TIME_SubDelayCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief  Time task ground command (diagnostics)
 */
int32 CFE_TIME_SendDiagnosticTlm(const CFE_TIME_SendDiagnosticCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief  Time task ground command (NOOP)
 */
int32 CFE_TIME_NoopCmd(const CFE_TIME_NoopCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief  Time task ground command (reset counters)
 */
int32 CFE_TIME_ResetCountersCmd(const CFE_TIME_ResetCountersCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief  Time task ground command (set leaps)
 */
int32 CFE_TIME_SetLeapSecondsCmd(const CFE_TIME_SetLeapSecondsCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief  Time task ground command (set MET)
 *
 * @note This command will not have lasting effect if configured
 * to get external time of type MET.  Also, there cannot
 * be a local h/w MET and an external MET since both would
 * need to be synchronized to the same tone signal.
 */
int32 CFE_TIME_SetMETCmd(const CFE_TIME_SetMETCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief  Time task command (set tone source)
 */
int32 CFE_TIME_SetSignalCmd(const CFE_TIME_SetSignalCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief  Time task command (set time source)
 */
int32 CFE_TIME_SetSourceCmd(const CFE_TIME_SetSourceCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief  Time task command (set clock state)
 */
int32 CFE_TIME_SetStateCmd(const CFE_TIME_SetStateCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief  Time task ground command (set STCF)
 */
int32 CFE_TIME_SetSTCFCmd(const CFE_TIME_SetSTCFCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief  Time task ground command (calc STCF)
 */
int32 CFE_TIME_SetTimeCmd(const CFE_TIME_SetTimeCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief  Time task ground command (1Hz adjust)
 *
 * This is a wrapper around CFE_TIME_1HzAdjImpl()
 */
int32 CFE_TIME_SubOneHzAdjustmentCmd(const CFE_TIME_SubOneHzAdjustmentCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief  Time task ground command (1Hz adjust)
 *
 * This is a wrapper around CFE_TIME_AdjustImpl()
 */
int32 CFE_TIME_SubAdjustCmd(const CFE_TIME_SubAdjustCmd_t *data);

#endif /* CFE_TIME_UTILS_H */
