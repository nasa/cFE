/*

** $Id: cfe_time_utils.h 1.8 2014/07/07 10:22:52GMT-05:00 acudmore Exp  $
**
**      GSC-18128-1, "Core Flight Executive Version 6.6"
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
** Purpose:  cFE Time Services (TIME) library utilities header file
**
** Author:   S.Walling/Microtel
**
** Notes:
**
*/

/*
** Ensure that header is included only once...
*/
#ifndef _cfe_time_utils_
#define _cfe_time_utils_

/*
** Required header files...
*/
#include "cfe_platform_cfg.h"
#include "cfe.h"
#include "cfe_time_verify.h"
#include "cfe_time_msg.h"
#include "cfe_time_events.h"
#include "cfe_psp.h"

/*************************************************************************/

#define CFE_TIME_NEGATIVE        0x80000000  /* ~ 68 * 31,536,000 seconds */

/*************************************************************************/

/*
** Main task definitions...
*/
#define CFE_TIME_TASK_NAME       "CFE_TIME"
#define CFE_TIME_RESET_SIGNATURE   0xA5A55A5A

/*
** Interrupt task definitions...
*/
#define CFE_TIME_TASK_TONE_NAME  "TIME_TONE_TASK"
#define CFE_TIME_TASK_1HZ_NAME   "TIME_1HZ_TASK"
#define CFE_TIME_TASK_STACK_PTR  0
#define CFE_TIME_TASK_FLAGS      0

/*
** Interrupt semaphore definitions...
*/
#define CFE_TIME_SEM_TONE_NAME   "TIME_TONE_SEM"
#define CFE_TIME_SEM_1HZ_NAME    "TIME_1HZ_SEM"
#define CFE_TIME_SEM_VALUE       0
#define CFE_TIME_SEM_OPTIONS     0

/*
** Main Task Pipe definitions...
*/

#define CFE_TIME_TASK_PIPE_NAME  "TIME_CMD_PIPE"
#define CFE_TIME_TASK_PIPE_DEPTH 12

/*
** Reset Area state state at startup...
*/

#define CFE_TIME_RESET_AREA_ERROR       1   /* no mem available */
#define CFE_TIME_RESET_AREA_BAD         2   /* had invalid data */
#define CFE_TIME_RESET_AREA_NEW         3   /* new memory block */
#define CFE_TIME_RESET_AREA_EXISTING    4   /* had valid data   */

/*************************************************************************/

/*
** Type definition (time reference data)...
*/
typedef struct
{

  CFE_TIME_SysTime_t    AtToneMET;      /* MET at time of tone */
  CFE_TIME_SysTime_t    AtToneSTCF;     /* STCF at time of tone */
  int16                 AtToneLeapSeconds;    /* Leap Seconds at time of tone */
  int16                 ClockSetState;  /* Time has been "set" */
  int16                 ClockFlyState;  /* Current fly-wheel state */
  CFE_TIME_SysTime_t    AtToneDelay;    /* Adjustment for slow tone detection */
  CFE_TIME_SysTime_t    AtToneLatch;    /* Local clock latched at time of tone */
  CFE_TIME_SysTime_t    CurrentLatch;   /* Local clock latched just "now" */
  CFE_TIME_SysTime_t    TimeSinceTone;  /* Time elapsed since the tone */
  CFE_TIME_SysTime_t    CurrentMET;     /* MET at this instant */

} CFE_TIME_Reference_t;

/*
** Time Synchronization Callback Registry Information
*/
typedef struct
{
  volatile CFE_TIME_SynchCallbackPtr_t    Ptr;  /**< \brief Pointer to Callback function */
} CFE_TIME_SynchCallbackRegEntry_t;

/*************************************************************************/

/*
** Type definition (time task global data)...
*/
typedef struct
{
  /*
  ** Task command interface counters...
  */
  uint8                 CommandCounter;
  uint8                 CommandErrorCounter;

  /*
  ** Task housekeeping and diagnostics telemetry packets...
  */
  CFE_TIME_HousekeepingTlm_t    HkPacket;
  CFE_TIME_DiagnosticTlm_t      DiagPacket;

  /*
  ** Task operational data (not reported in housekeeping)...
  */
  CFE_SB_MsgPtr_t       MsgPtr;
  CFE_SB_PipeId_t       CmdPipe;

  /*
  ** Task initialization data (not reported in housekeeping)...
  */
  char                  PipeName[16];
  uint16                PipeDepth;

  /*
  ** Data values used to compute time (in reference to "tone")...
  ** NOTE: All values that are thread shared but accessed in a lock-less
  ** design must be marked "volatile" to ensure that the memory reads
  ** and writes are not inadvertently reordered or removed by an optimizer.
  ** This is most important for those values read during CFE_TIME_GetReference()
  */
  volatile CFE_TIME_SysTime_t    AtToneMET;
  volatile CFE_TIME_SysTime_t    AtToneSTCF;
  volatile int16                 AtToneLeapSeconds;
  volatile CFE_TIME_SysTime_t    AtToneDelay;
  volatile CFE_TIME_SysTime_t    AtToneLatch;
  /*
  ** Data values used to define the current clock state...
  */
  volatile int16                 ClockSetState;
  volatile int16                 ClockFlyState;
  int16                 ClockSource;
  int16                 ClockSignal;
  int16                 ServerFlyState;

  /*
  ** Pending data values (from "time at tone" command data)...
  */
  CFE_TIME_SysTime_t    PendingMET;
  CFE_TIME_SysTime_t    PendingSTCF;
  int16                 PendingLeaps;
  int16                 PendingState;

  /*
  ** STCF adjustment values...
  */
  CFE_TIME_SysTime_t    OneTimeAdjust;
  CFE_TIME_SysTime_t    OneHzAdjust;

  int16                 OneTimeDirection; /* Add = TRUE */
  int16                 OneHzDirection;
  int16                 DelayDirection;

  /*
  ** Most recent local clock latch values...
  */
  CFE_TIME_SysTime_t    ToneSignalLatch;  /* Latched at tone */
  CFE_TIME_SysTime_t    ToneDataLatch;    /* Latched at packet */

  /*
  ** Miscellaneous counters...
  */
  uint32                ToneMatchCounter;   /* Tone and data match */
  uint32                ToneMatchErrorCounter;  /* Tone and data mismatch */
  uint32                ToneSignalCounter;  /* Tone signal commands */
  uint32                ToneDataCounter;    /* Tone data commands */
  uint32                ToneIntCounter;     /* Tone interrupts (valid) */
  uint32                ToneIntErrorCounter;    /* Tone interrupts (invalid) */
  uint32                ToneTaskCounter;    /* Tone task wake-ups */
  uint32                VirtualMET;       /* Software MET */
  uint32                LocalIntCounter;    /* Local 1Hz interrupts */
  uint32                LocalTaskCounter;   /* Local 1Hz task wake-ups */
  uint32                InternalCount;    /* Time from internal data */
  uint32                ExternalCount;    /* Time from external data */

  volatile uint32       PendingVersionCounter;     /* Pending Updates to "AtTone" values */
  volatile uint32       CompleteVersionCounter;    /* Completed Updates to "AtTone" values */

  /*
  ** Time window verification values (converted from micro-secs)...
  **
  ** Regardless of whether the tone follows the time packet, or vice
  **    versa, these values define the acceptable window of time for
  **    the second event to follow the first.  The minimum value may
  **    be as little as zero, and the maximum must be something less
  **    than a second.
  */
  uint32                MinElapsed;
  uint32                MaxElapsed;

  /*
  ** Maximum local clock value (before roll-over)...
  */
  CFE_TIME_SysTime_t    MaxLocalClock;

  /*
  ** Clock state has been commanded into (CFE_TIME_ClockState_FLYWHEEL)...
  */
  boolean               Forced2Fly;

  /*
  ** Clock state has just transitioned into (CFE_TIME_ClockState_FLYWHEEL)...
  **   (not in HK since it won't be true long enough to detect)
  */

  boolean               AutoStartFly;
  boolean               IsToneGood;

  /*
  ** Spare byte for alignment 
  */
  boolean               Spare;

  /*
  ** Local 1Hz wake-up command packet (not related to time at tone)...
  */
  CCSDS_CommandPacket_t Local1HzCmd;

  /*
  ** Time at the tone command packets (sent by time servers)...
  */
  CFE_TIME_ToneDataCmd_t ToneDataCmd;
  CFE_TIME_ToneSignalCmd_t ToneSignalCmd;

  /*
   * Normally "tone send" commands come from the scheduler based on the
   * configured action table, so it occurs at the right point between tones.
   *
   * However when "fake tone" mode is enabled, then we will locally generate the
   * "tone send" message as part of the Tone task, in addition to the regular
   * "tone signal" message above.
   */
#if (CFE_MISSION_TIME_CFG_FAKE_TONE == TRUE)
  CCSDS_CommandPacket_t  ToneSendCmd;
#endif

  /*
  ** Interrupt task semaphores...
  */
  uint32                LocalSemaphore;
  uint32                ToneSemaphore;
  /*
  ** Interrupt task ID's...
  */
  uint32                LocalTaskID;
  uint32                ToneTaskID;

  /*
  ** Maximum difference from expected for external time sources...
  */

  CFE_TIME_SysTime_t    MaxDelta;

  /*
  ** Tone signal tolerance limits...
  */
  uint32                ToneOverLimit;
  uint32                ToneUnderLimit;

  /*
  ** Reset Area ...
  */
  uint32                DataStoreStatus;
  
  /*
  ** Synchronization Callback Registry
  ** One callback per app is allowed
  */
  CFE_TIME_SynchCallbackRegEntry_t SynchCallback[CFE_PLATFORM_ES_MAX_APPLICATIONS];

} CFE_TIME_TaskData_t;

/*
** Time task global data (from "cfe_time_task.c")...
*/
extern CFE_TIME_TaskData_t CFE_TIME_TaskData;


/*************************************************************************/
/*
** Function prototypes (get local clock)...
*/
CFE_TIME_SysTime_t CFE_TIME_LatchClock(void);

/*
** Function prototypes (Time Services utilities data)...
*/
int32 CFE_TIME_TaskInit (void);
void  CFE_TIME_TaskPipe(CFE_SB_MsgPtr_t MessagePtr);
void CFE_TIME_InitData(void);
void CFE_TIME_QueryResetVars(void);
void CFE_TIME_UpdateResetVars(const CFE_TIME_Reference_t *Reference);
uint16 CFE_TIME_GetStateFlags(void);
void CFE_TIME_GetDiagData(void);
void CFE_TIME_GetHkData(const CFE_TIME_Reference_t *Reference);

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
#if (CFE_PLATFORM_TIME_CFG_SOURCE == TRUE)
void CFE_TIME_SetSource(int16 NewSource);
#endif

#if (CFE_PLATFORM_TIME_CFG_SIGNAL == TRUE)
void CFE_TIME_SetSignal(int16 NewSignal);
#endif

#if (CFE_PLATFORM_TIME_CFG_CLIENT == TRUE)
void CFE_TIME_SetDelay(CFE_TIME_SysTime_t NewDelay, int16 Direction);
#endif
#if (CFE_PLATFORM_TIME_CFG_SERVER == TRUE)
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
#if (CFE_PLATFORM_TIME_CFG_SERVER == TRUE)
void CFE_TIME_ToneSend(void); /* signal to send time at tone packet */
#endif

/*
** Function prototypes (send time at tone data packet -- external time)...
*/
#if (CFE_PLATFORM_TIME_CFG_SRC_MET == TRUE)
int32 CFE_TIME_ToneSendMET(CFE_TIME_SysTime_t NewMET);
#endif

#if (CFE_PLATFORM_TIME_CFG_SRC_GPS == TRUE)
int32 CFE_TIME_ToneSendGPS(CFE_TIME_SysTime_t NewTime, int16 NewLeaps);
#endif

#if (CFE_PLATFORM_TIME_CFG_SRC_TIME == TRUE)
int32 CFE_TIME_ToneSendTime(CFE_TIME_SysTime_t NewTime);
#endif


/*
 * Helper function for updating the "Reference" value
 * This is the local replacement for "OS_IntLock()"
 */
static inline uint32 CFE_TIME_StartReferenceUpdate(void)
{
    uint32 NextVersion = 1 + CFE_TIME_TaskData.CompleteVersionCounter;
    CFE_TIME_TaskData.PendingVersionCounter = NextVersion;
    return NextVersion;
}

/*
 * Helper function for updating the "Reference" value
 * This is the local replacement for "OS_IntUnlock()"
 */
static inline void CFE_TIME_FinishReferenceUpdate(uint32 NextVersion)
{
    CFE_TIME_TaskData.CompleteVersionCounter = NextVersion;
}



/*
** Function prototypes (process time at the tone signal and data packet)...
*/
void CFE_TIME_ToneSignal(void);
void CFE_TIME_ToneData(const CFE_TIME_ToneDataCmd_Payload_t *Packet);
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
void CFE_TIME_Local1HzISR(void);
void CFE_TIME_Local1HzTask(void);
void CFE_TIME_Local1HzStateMachine(void);
void CFE_TIME_Local1HzTimerCallback(uint32 TimerId, void *Arg);


#endif /* _cfe_time_utils_ */

/************************/
/*  End of File Comment */
/************************/
