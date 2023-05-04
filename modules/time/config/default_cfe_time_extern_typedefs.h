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
 * Declarations and prototypes for cfe_time_extern_typedefs module
 */

#ifndef CFE_TIME_EXTERN_TYPEDEFS_H
#define CFE_TIME_EXTERN_TYPEDEFS_H

#include "common_types.h"

/**
**  \brief Data structure used to hold system time values
**
**  \par Description
**       The #CFE_TIME_SysTime_t data structure is used to hold time
**       values.  Time is referred to as the elapsed time (in seconds
**       and subseconds) since a specified epoch time.  The subseconds
**       field contains the number of 2^(-32) second intervals that have
**       elapsed since the epoch.
**
*/
typedef struct CFE_TIME_SysTime
{
    uint32 Seconds;    /**< \brief Number of seconds since epoch */
    uint32 Subseconds; /**< \brief Number of subseconds since epoch (LSB = 2^(-32) seconds) */
} CFE_TIME_SysTime_t;

/**
 * @brief Label definitions associated with CFE_TIME_FlagBit_Enum_t
 */
enum CFE_TIME_FlagBit
{
    /**
     * @brief The spacecraft time has been set
     */
    CFE_TIME_FlagBit_CLKSET = 0,

    /**
     * @brief This instance of Time Services is flywheeling
     */
    CFE_TIME_FlagBit_FLYING = 1,

    /**
     * @brief The clock source is set to internal
     */
    CFE_TIME_FlagBit_SRCINT = 2,

    /**
     * @brief The clock signal is set to primary
     */
    CFE_TIME_FlagBit_SIGPRI = 3,

    /**
     * @brief The Time Server is in flywheel mode
     */
    CFE_TIME_FlagBit_SRVFLY = 4,

    /**
     * @brief This instance of Time Services was commanded into flywheel mode
     */
    CFE_TIME_FlagBit_CMDFLY = 5,

    /**
     * @brief One time STCF Adjustment is to be done in positive direction
     */
    CFE_TIME_FlagBit_ADDADJ = 6,

    /**
     * @brief 1 Hz STCF Adjustment is to be done in a positive direction
     */
    CFE_TIME_FlagBit_ADD1HZ = 7,

    /**
     * @brief Time Client Latency is applied in a positive direction
     */
    CFE_TIME_FlagBit_ADDTCL = 8,

    /**
     * @brief This instance of Time Services is a Time Server
     */
    CFE_TIME_FlagBit_SERVER = 9,

    /**
     * @brief The tone received is good compared to the last tone received
     */
    CFE_TIME_FlagBit_GDTONE = 10
};

/**
 * @brief Bit positions of the various clock state flags
 *
 * @sa enum CFE_TIME_FlagBit
 */
typedef uint8 CFE_TIME_FlagBit_Enum_t;

/**
 * @brief Label definitions associated with CFE_TIME_ClockState_Enum_t
 */
enum CFE_TIME_ClockState
{
    /**
     *
     * The spacecraft time has not been set since the last clock
     * reset.  Times returned by clock routines have no relationship
     * to any ground-based time reference.
     *
     */
    CFE_TIME_ClockState_INVALID = -1,

    /**
     *
     * The spacecraft time has been set at least once since the last
     * clock reset, and it is synchronized with the primary on-board
     * time base.  Times returned by clock routines can be trusted.
     *
     */
    CFE_TIME_ClockState_VALID = 0,

    /**
     *
     * The spacecraft time has been set at least once since the last
     * clock reset, but it is not currently synchronized with the
     * primary on-board time base.  Times returned by clock routines
     * are a "best guess" based on a non-optimal oscillator.
     *
     */
    CFE_TIME_ClockState_FLYWHEEL = 1
};

/**
 * @brief Enumerated types identifying the quality of the current time
 *
 * \par Description
 * The #CFE_TIME_ClockState_Enum_t enumerations identify the three recognized states of the current time.
 * If the clock has never been successfully synchronized with the primary onboard clock source, the
 * time is considered to be #CFE_TIME_ClockState_INVALID.  If the time is currently synchronized (i.e. - the
 * primary synchronization mechanism has not been dropped for any significant amount of time), then
 * the current time is considered to be #CFE_TIME_ClockState_VALID.  If the time had, at some point in the past,
 * been synchronized, but the synchronization with the primary onboard clock has since been lost, then
 * the time is considered to be #CFE_TIME_ClockState_FLYWHEEL.  Since different clocks drift at different rates
 * from one another, the accuracy of the time while in #CFE_TIME_ClockState_FLYWHEEL is dependent upon the time
 * spent in that state.
 *
 * @sa enum CFE_TIME_ClockState
 */
typedef int16 CFE_TIME_ClockState_Enum_t;

/**
 * @brief Label definitions associated with CFE_TIME_SourceSelect_Enum_t
 */
enum CFE_TIME_SourceSelect
{
    /**
     * @brief Use Internal Source
     */
    CFE_TIME_SourceSelect_INTERNAL = 1,

    /**
     * @brief Use External Source
     */
    CFE_TIME_SourceSelect_EXTERNAL = 2
};

/**
 * @brief Clock Source Selection Parameters
 *
 * @sa enum CFE_TIME_SourceSelect
 */
typedef uint8 CFE_TIME_SourceSelect_Enum_t;

/**
 * @brief Label definitions associated with CFE_TIME_ToneSignalSelect_Enum_t
 */
enum CFE_TIME_ToneSignalSelect
{
    /**
     * @brief Primary Source
     */
    CFE_TIME_ToneSignalSelect_PRIMARY = 1,

    /**
     * @brief Redundant Source
     */
    CFE_TIME_ToneSignalSelect_REDUNDANT = 2
};

/**
 * @brief Tone Signal Selection Parameters
 *
 * @sa enum CFE_TIME_ToneSignalSelect
 */
typedef uint8 CFE_TIME_ToneSignalSelect_Enum_t;

/**
 * @brief Label definitions associated with CFE_TIME_AdjustDirection_Enum_t
 */
enum CFE_TIME_AdjustDirection
{
    /**
     * @brief Add time adjustment
     */
    CFE_TIME_AdjustDirection_ADD = 1,

    /**
     * @brief Subtract time adjustment
     */
    CFE_TIME_AdjustDirection_SUBTRACT = 2
};

/**
 * @brief STCF adjustment direction (for both one-time and 1Hz adjustments)
 *
 * @sa enum CFE_TIME_AdjustDirection
 */
typedef uint8 CFE_TIME_AdjustDirection_Enum_t;

/**
 * @brief Label definitions associated with CFE_TIME_FlywheelState_Enum_t
 */
enum CFE_TIME_FlywheelState
{
    /**
     * @brief Not in flywheel state
     */
    CFE_TIME_FlywheelState_NO_FLY = 0,

    /**
     * @brief In flywheel state
     */
    CFE_TIME_FlywheelState_IS_FLY = 1
};

/**
 * @brief Fly-wheel status values
 *
 * @sa enum CFE_TIME_FlywheelState
 */
typedef uint8 CFE_TIME_FlywheelState_Enum_t;

/**
 * @brief Label definitions associated with CFE_TIME_SetState_Enum_t
 */
enum CFE_TIME_SetState
{
    /**
     * @brief Spacecraft time has not been set
     */
    CFE_TIME_SetState_NOT_SET = 0,

    /**
     * @brief Spacecraft time has been set
     */
    CFE_TIME_SetState_WAS_SET = 1
};

/**
 * @brief Clock status values (has the clock been set to correct time)
 *
 * @sa enum CFE_TIME_SetState
 */
typedef uint8 CFE_TIME_SetState_Enum_t;

#endif /* CFE_TIME_EXTERN_TYPEDEFS_H */
