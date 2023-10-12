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
 *   Specification for the CFE Time Services (CFE_TIME) command and telemetry
 *   message constant definitions.
 *
 *  For CFE_TIME this is only the function/command code definitions
 */
#ifndef CFE_TIME_MSGDEFS_H
#define CFE_TIME_MSGDEFS_H

#include "common_types.h"
#include "cfe_mission_cfg.h"
#include "cfe_time_extern_typedefs.h"
#include "cfe_time_fcncodes.h"

/** \defgroup CFETIMEClkStates cFE Clock State Flag Defines
 * \{
 */
#define CFE_TIME_FLAG_CLKSET 0x8000 /**< \brief The spacecraft time has been set */
#define CFE_TIME_FLAG_FLYING 0x4000 /**< \brief This instance of Time Services is flywheeling */
#define CFE_TIME_FLAG_SRCINT 0x2000 /**< \brief The clock source is set to "internal" */
#define CFE_TIME_FLAG_SIGPRI 0x1000 /**< \brief The clock signal is set to "primary" */
#define CFE_TIME_FLAG_SRVFLY 0x0800 /**< \brief The Time Server is in flywheel mode */
#define CFE_TIME_FLAG_CMDFLY 0x0400 /**< \brief This instance of Time Services was commanded into flywheel mode */
#define CFE_TIME_FLAG_ADDADJ 0x0200 /**< \brief One time STCF Adjustment is to be done in positive direction */
#define CFE_TIME_FLAG_ADD1HZ 0x0100 /**< \brief 1 Hz STCF Adjustment is to be done in a positive direction */
#define CFE_TIME_FLAG_ADDTCL 0x0080 /**< \brief Time Client Latency is applied in a positive direction */
#define CFE_TIME_FLAG_SERVER 0x0040 /**< \brief This instance of Time Services is a Time Server */
#define CFE_TIME_FLAG_GDTONE 0x0020 /**< \brief The tone received is good compared to the last tone received */
#define CFE_TIME_FLAG_REFERR \
    0x0010 /**< \brief GetReference read error, will be set if unable to get a consistent ref value */
#define CFE_TIME_FLAG_UNUSED 0x000F /**< \brief Reserved flags - should be zero */
/** \} */

/**
 * \brief Set leap seconds command payload
 */
typedef struct CFE_TIME_LeapsCmd_Payload
{
    int16 LeapSeconds;
} CFE_TIME_LeapsCmd_Payload_t;

/**
 * \brief Set clock state command payload
 */
typedef struct CFE_TIME_StateCmd_Payload
{
    CFE_TIME_ClockState_Enum_t ClockState; /**< \brief #CFE_TIME_ClockState_INVALID=Spacecraft time has
                                  not been accurately set, #CFE_TIME_ClockState_VALID=Spacecraft clock
                                  has been accurately set, #CFE_TIME_ClockState_FLYWHEEL=Force into
                                  FLYWHEEL mode   */
                                           /**< Selects the current clock state */
} CFE_TIME_StateCmd_Payload_t;

/**
 * \brief Set time data source command payload
 */
typedef struct CFE_TIME_SourceCmd_Payload
{
    int16 TimeSource; /**< \brief #CFE_TIME_SourceSelect_INTERNAL=Internal Source,
                                  #CFE_TIME_SourceSelect_EXTERNAL=External Source   */
                      /**< Selects either the "Internal" and "External" clock source */
} CFE_TIME_SourceCmd_Payload_t;

/**
 * \brief Set tone signal source command payload
 */
typedef struct CFE_TIME_SignalCmd_Payload
{
    int16 ToneSource; /**< \brief #CFE_TIME_ToneSignalSelect_PRIMARY=Primary Source,
                                  #CFE_TIME_ToneSignalSelect_REDUNDANT=Redundant Source   */
                      /**< Selects either the "Primary" or "Redundant" tone signal source */
} CFE_TIME_SignalCmd_Payload_t;

/**
 * \brief Generic seconds, microseconds command payload
 */
typedef struct CFE_TIME_TimeCmd_Payload
{
    uint32 Seconds;
    uint32 MicroSeconds;
} CFE_TIME_TimeCmd_Payload_t;

/**
 * \brief Generic seconds, subseconds command payload
 */
typedef struct CFE_TIME_OneHzAdjustmentCmd_Payload
{
    uint32 Seconds;
    uint32 Subseconds;
} CFE_TIME_OneHzAdjustmentCmd_Payload_t;

/**
 * \brief Time at tone data command payload
 */
typedef struct CFE_TIME_ToneDataCmd_Payload
{
    CFE_TIME_SysTime_t         AtToneMET;         /**< \brief MET at time of tone */
    CFE_TIME_SysTime_t         AtToneSTCF;        /**< \brief STCF at time of tone */
    int16                      AtToneLeapSeconds; /**< \brief Leap Seconds at time of tone */
    CFE_TIME_ClockState_Enum_t AtToneState;       /**< \brief Clock state at time of tone */
} CFE_TIME_ToneDataCmd_Payload_t;

/*************************************************************************/

/**
**  \cfetimetlm Time Services Housekeeping Packet
**/
typedef struct CFE_TIME_HousekeepingTlm_Payload
{
    /*
    ** Task command interface counters...
    */
    uint8 CommandCounter;      /**< \cfetlmmnemonic \TIME_CMDPC
                                \brief Time Command Execution Counter */
    uint8 CommandErrorCounter; /**< \cfetlmmnemonic \TIME_CMDEC
                           \brief Time Command Error Counter */

    /*
    ** Clock state flags and "as calculated" clock state...
    */
    uint16 ClockStateFlags;                   /**< \cfetlmmnemonic \TIME_STATEFLG
                                                   \brief State Flags */
    CFE_TIME_ClockState_Enum_t ClockStateAPI; /**< \cfetlmmnemonic
                              \TIME_APISTATE \brief API State */

    /*
    ** Leap Seconds...
    */
    int16 LeapSeconds; /**< \cfetlmmnemonic \TIME_LEAPSECS
                            \brief Current Leaps Seconds */

    /*
    ** Current MET and STCF time values...
    */
    uint32 SecondsMET; /**< \cfetlmmnemonic \TIME_METSECS
                            \brief Current MET (seconds) */
    uint32 SubsecsMET; /**< \cfetlmmnemonic \TIME_METSUBSECS
                            \brief Current MET (sub-seconds) */

    uint32 SecondsSTCF; /**< \cfetlmmnemonic \TIME_STCFSECS
                             \brief Current STCF (seconds) */
    uint32 SubsecsSTCF; /**< \cfetlmmnemonic \TIME_STCFSUBSECS
                             \brief Current STCF (sub-seconds) */

/*
** 1Hz STCF adjustment values (server only)...
*/
#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
    uint32 Seconds1HzAdj; /**< \cfetlmmnemonic \TIME_1HZADJSECS
                               \brief Current 1 Hz SCTF adjustment (seconds) */
    uint32 Subsecs1HzAdj; /**< \cfetlmmnemonic \TIME_1HZADJSSECS
                               \brief Current 1 Hz SCTF adjustment (sub-seconds) */
#endif

/*
** Time at tone delay values (client only)...
*/
#if (CFE_PLATFORM_TIME_CFG_CLIENT == true)
    uint32 SecondsDelay; /**< \cfetlmmnemonic \TIME_1HZDLYSECS
                              \brief Current 1 Hz SCTF Delay (seconds) */
    uint32 SubsecsDelay; /**< \cfetlmmnemonic \TIME_1HZDLYSSECS
                              \brief Current 1 Hz SCTF Delay (sub-seconds) */
#endif
} CFE_TIME_HousekeepingTlm_Payload_t;

/*************************************************************************/

/**
**  \cfetimetlm Time Services Diagnostics Packet
**/
typedef struct CFE_TIME_DiagnosticTlm_Payload
{
    /*
     ** Data values used to compute time (in reference to "tone")...
     */
    CFE_TIME_SysTime_t AtToneMET;   /**< \cfetlmmnemonic \TIME_TMETS
                                               \brief MET at time of tone */
    CFE_TIME_SysTime_t AtToneSTCF;  /**< \cfetlmmnemonic \TIME_STCFS
                                               \brief STCF at time of tone */
    CFE_TIME_SysTime_t AtToneDelay; /**< \cfetlmmnemonic \TIME_LATENTS
                                               \brief Adjustment for slow tone detection */
    CFE_TIME_SysTime_t AtToneLatch; /**< \cfetlmmnemonic \TIME_TVALIDS
                                               \brief Local clock latched at time of tone */

    int16 AtToneLeapSeconds;                  /**< \cfetlmmnemonic \TIME_LEAPS
                                                   \brief Leap Seconds at time of tone */
    CFE_TIME_ClockState_Enum_t ClockStateAPI; /**< \cfetlmmnemonic \TIME_APISTATE
                                    \brief Clock state as per API */

    /*
     ** Data values that reflect the time (right now)...
     */
    CFE_TIME_SysTime_t TimeSinceTone; /**< \cfetlmmnemonic \TIME_ELAPSEDS
                                                 \brief Time elapsed since the tone */
    CFE_TIME_SysTime_t CurrentLatch;  /**< \cfetlmmnemonic \TIME_LOCALS
                                                 \brief Local clock latched just "now" */
    CFE_TIME_SysTime_t CurrentMET;    /**< \cfetlmmnemonic \TIME_METS
                                                 \brief MET at this instant */
    CFE_TIME_SysTime_t CurrentTAI;    /**< \cfetlmmnemonic \TIME_TAIS
                                                 \brief TAI at this instant */
    CFE_TIME_SysTime_t CurrentUTC;    /**< \cfetlmmnemonic \TIME_UTCS
                                                 \brief UTC at this instant */

    /*
     ** Data values used to define the current clock state...
     */
    int16 ClockSetState;  /**< \cfetlmmnemonic \TIME_VALID
                                     \brief Time has been "set" */
    int16 ClockFlyState;  /**< \cfetlmmnemonic \TIME_FLYWHEEL
                                     \brief Current fly-wheel state */
    int16 ClockSource;    /**< \cfetlmmnemonic \TIME_SOURCE
                                     \brief Internal vs external, etc. */
    int16 ClockSignal;    /**< \cfetlmmnemonic \TIME_SIGNAL
                                     \brief Primary vs redundant, etc. */
    int16 ServerFlyState; /**< \cfetlmmnemonic \TIME_SRVFLY
                                     \brief Used by clients only */
    int16 Forced2Fly;     /**< \cfetlmmnemonic \TIME_CMD2FLY
                                     \brief Commanded into fly-wheel */

    /*
     ** Clock state flags...
     */
    uint16 ClockStateFlags; /**< \cfetlmmnemonic \TIME_STATEFLAGS
                                       \brief Clock State Flags */

    /*
     ** STCF adjustment direction values...
     */
    int16 OneTimeDirection; /**< \cfetlmmnemonic \TIME_ADJUSTDIR
                                       \brief One time STCF adjustment direction (Add = 1,  Sub = 2) */
    int16 OneHzDirection;   /**< \cfetlmmnemonic \TIME_1HZADJDIR
                                       \brief 1Hz STCF adjustment direction */
    int16 DelayDirection;   /**< \cfetlmmnemonic \TIME_LATENTDIR
                                       \brief Client latency adjustment direction */

    /*
     ** STCF adjustment values...
     */
    CFE_TIME_SysTime_t OneTimeAdjust; /**< \cfetlmmnemonic \TIME_ADJUSTS
                                                 \brief Previous one-time STCF adjustment */
    CFE_TIME_SysTime_t OneHzAdjust;   /**< \cfetlmmnemonic \TIME_1HZADJS
                                                 \brief Current 1Hz STCF adjustment */

    /*
     ** Most recent local clock latch values...
     */
    CFE_TIME_SysTime_t ToneSignalLatch; /**< \cfetlmmnemonic \TIME_TTS
                                                   \brief  Local Clock latched at most recent tone signal */
    CFE_TIME_SysTime_t ToneDataLatch;   /**< \cfetlmmnemonic \TIME_TDS
                                                   \brief  Local Clock latched at arrival of tone data */

    /*
     ** Miscellaneous counters (subject to reset command)...
     */
    uint32 ToneMatchCounter;      /**< \cfetlmmnemonic \TIME_VERIFYCNT
                                           \brief  Tone signal / data verification count */
    uint32 ToneMatchErrorCounter; /**< \cfetlmmnemonic \TIME_VERIFYER
                                       \brief  Tone signal / data verification error count */
    uint32 ToneSignalCounter;     /**< \cfetlmmnemonic \TIME_TSDETCNT
                                           \brief  Tone signal detected SB message count */
    uint32 ToneDataCounter;       /**< \cfetlmmnemonic \TIME_TATTCNT
                                           \brief  Time at the tone data SB message count */
    uint32 ToneIntCounter;        /**< \cfetlmmnemonic \TIME_TSISRCNT
                                           \brief  Tone signal ISR execution count */
    uint32 ToneIntErrorCounter;   /**< \cfetlmmnemonic \TIME_TSISRERR
                                       \brief  Tone signal ISR error count */
    uint32 ToneTaskCounter;       /**< \cfetlmmnemonic \TIME_TSTASKCNT
                                           \brief  Tone task execution count */
    uint32 VersionCounter;        /**< \cfetlmmnemonic \TIME_VERSIONCNT
                                           \brief  Count of mods to time at tone reference data (version) */
    uint32 LocalIntCounter;       /**< \cfetlmmnemonic \TIME_1HZISRCNT
                                           \brief  Local 1Hz ISR execution count */
    uint32 LocalTaskCounter;      /**< \cfetlmmnemonic \TIME_1HZTASKCNT
                                           \brief  Local 1Hz task execution count */

    /*
     ** Miscellaneous counters (not subject to reset command)...
     */
    uint32 VirtualMET; /**< \cfetlmmnemonic \TIME_LOGICALMET
                                  \brief  Software MET */

    /*
     ** Time window verification values (converted from micro-secs)...
     **
     ** Regardless of whether the tone follows the time packet, or vice
     **    versa, these values define the acceptable window of time for
     **    the second event to follow the first.  The minimum value may
     **    be as little as zero, and the maximum must be something less
     **    than a second.
     */
    uint32 MinElapsed; /**< \cfetlmmnemonic \TIME_MINWINDOW
                                  \brief Min tone signal / data pkt arrival window (Sub-seconds) */
    uint32 MaxElapsed; /**< \cfetlmmnemonic \TIME_MAXWINDOW
                                  \brief Max tone signal / data pkt arrival window (Sub-seconds) */

    /*
     ** Maximum local clock value (before roll-over)...
     */
    CFE_TIME_SysTime_t MaxLocalClock; /**< \cfetlmmnemonic \TIME_WRAPS
                                                 \brief Max local clock value before rollover */

    /*
     ** Tone signal tolerance limits...
     */
    uint32 ToneOverLimit;  /**< \cfetlmmnemonic \TIME_MAXSS
                                      \brief Max between tone signal interrupts */
    uint32 ToneUnderLimit; /**< \cfetlmmnemonic \TIME_MINSS
                                      \brief Min between tone signal interrupts */

    /*
     ** Reset Area...
     */
    uint32 DataStoreStatus; /**< \cfetlmmnemonic \TIME_ATASTSTAT
                                       \brief Data Store status (preserved across processor reset) */
} CFE_TIME_DiagnosticTlm_Payload_t;

#endif
