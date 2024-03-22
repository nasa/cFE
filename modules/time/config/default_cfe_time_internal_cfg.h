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
 *   CFE Time Service (CFE_TIME) Application Private Config Definitions
 *
 * This provides default values for configurable items that are internal
 * to this module and do NOT affect the interface(s) of this module.  Changes
 * to items in this file only affect the local module and will be transparent
 * to external entities that are using the public interface(s).
 *
 * @note This file may be overridden/superceded by mission-provided definitions
 * either by overriding this header or by generating definitions from a command/data
 * dictionary tool.
 */
#ifndef CFE_TIME_INTERNAL_CFG_H
#define CFE_TIME_INTERNAL_CFG_H

/**
**  \cfetimecfg Time Server or Time Client Selection
**
**  \par Description:
**       This configuration parameter selects whether the Time task functions as a
**       time "server" or "client".  A time server generates the "time at the tone"
**       packet which is received by time clients.
**
**  \par Limits
**       Enable one, and only one by defining either CFE_PLATFORM_TIME_CFG_SERVER or
**       CFE_PLATFORM_TIME_CFG_CLIENT AS true.  The other must be defined as false.
*/
#define CFE_PLATFORM_TIME_CFG_SERVER true
#define CFE_PLATFORM_TIME_CFG_CLIENT false

/**
** \cfetimecfg Time Tone In Big-Endian Order
**
** \par Description:
**      If this configuration parameter is defined, the CFE time server will
**      publish time tones with payloads in big-endian order, and time clients
**      will expect the tones to be in big-endian order. This is useful for
**      mixed-endian environments. This will become obsolete once EDS is
**      available and the CFE time tone message is defined.
*/
#undef CFE_PLATFORM_TIME_CFG_BIGENDIAN

/**
**  \cfetimecfg Local MET or Virtual MET Selection for Time Servers
**
**  \par Description:
**       Depending on the specific hardware system configuration, it may be possible
**       for Time Servers to read the "local" MET from a h/w register rather than
**       having to track the MET as the count of tone signal interrupts (virtual MET)
**
**       Time Clients must be defined as using a virtual MET.  Also, a Time Server
**       cannot be defined as having both a h/w MET and an external time source (they
**       both cannot synchronize to the same tone).
**
**       Note: "disable" this define (set to false) only for Time Servers with local hardware
**       that supports a h/w MET that is synchronized to the tone signal !!!
**
**  \par Limits
**       Only applies if #CFE_PLATFORM_TIME_CFG_SERVER is set to true.
*/
#define CFE_PLATFORM_TIME_CFG_VIRTUAL true

/**
**  \cfetimecfg Include or Exclude the Primary/Redundant Tone Selection Cmd
**
**  \par Description:
**       Depending on the specific hardware system configuration, it may be possible
**       to switch between a primary and redundant tone signal.  If supported by
**       hardware, this definition will enable command interfaces to select the
**       active tone signal. Both Time Clients and Time Servers support this feature.
**       Note: Set the CFE_PLATFORM_TIME_CFG_SIGNAL define to true to enable tone signal commands.
**
**  \par Limits
**       Not Applicable
*/
#define CFE_PLATFORM_TIME_CFG_SIGNAL false

/**
**  \cfetimecfg Include or Exclude the Internal/External Time Source Selection Cmd
**
**  \par Description:
**       By default, Time Servers maintain time using an internal MET which may be a
**       h/w register or software counter, depending on available hardware. The
**       following definition enables command interfaces to switch between an
**       internal MET, or external time data received from one of several supported
**       external time sources. Only a Time Server may be configured to use external
**       time data.
**       Note: Set the CFE_PLATFORM_TIME_CFG_SOURCE define to true to include the Time Source
**             Selection Command (command allows selection between the internal
**             or external time source). Then choose the external source with the
**             CFE_TIME_CFG_SRC_??? define.
**
**  \par Limits
**       Only applies if #CFE_PLATFORM_TIME_CFG_SERVER is set to true.
*/
#define CFE_PLATFORM_TIME_CFG_SOURCE false

/**
**  \cfetimecfg Choose the External Time Source for Server only
**
**  \par Description:
**       If #CFE_PLATFORM_TIME_CFG_SOURCE is set to true, then one of the following external time
**       source types must also be set to true.  Do not set any of the external time
**       source types to true unless #CFE_PLATFORM_TIME_CFG_SOURCE is set to true.
**
**  \par Limits
**       -# If #CFE_PLATFORM_TIME_CFG_SOURCE is set to true then one and only one of the following
**       three external time sources can and must be set true:
**       #CFE_PLATFORM_TIME_CFG_SRC_MET, #CFE_PLATFORM_TIME_CFG_SRC_GPS, #CFE_PLATFORM_TIME_CFG_SRC_TIME
**       -# Only applies if #CFE_PLATFORM_TIME_CFG_SERVER is set to true.
*/
#define CFE_PLATFORM_TIME_CFG_SRC_MET  false
#define CFE_PLATFORM_TIME_CFG_SRC_GPS  false
#define CFE_PLATFORM_TIME_CFG_SRC_TIME false

/**
**  \cfetimecfg Define the Max Delta Limits for Time Servers using an Ext Time Source
**
**  \par Description:
**       If #CFE_PLATFORM_TIME_CFG_SOURCE is set to true and one of the external time sources is
**       also set to true, then the delta time limits for range checking is used.
**
**       When a new time value is received from an external source, the value is
**       compared against the "expected" time value. If the delta exceeds the
**       following defined amount, then the new time data will be ignored. This range
**       checking is only performed after the clock state has been commanded to
**       "valid". Until then, external time data is accepted unconditionally.
**
**  \par Limits
**       Applies only if both #CFE_PLATFORM_TIME_CFG_SERVER and #CFE_PLATFORM_TIME_CFG_SOURCE are set
**       to true.
*/
#define CFE_PLATFORM_TIME_MAX_DELTA_SECS 0
#define CFE_PLATFORM_TIME_MAX_DELTA_SUBS 500000

/**
**  \cfetimecfg Define the Local Clock Rollover Value in seconds and subseconds
**
**  \par Description:
**       Specifies the capability of the local clock.  Indicates the time at which
**       the local clock rolls over.
**
**  \par Limits
**       Not Applicable
*/
#define CFE_PLATFORM_TIME_MAX_LOCAL_SECS 27
#define CFE_PLATFORM_TIME_MAX_LOCAL_SUBS 0

/**
**  \cfetimecfg Define Timing Limits From One Tone To The Next
**
**  \par Description:
**       Defines limits to the timing of the 1Hz tone signal. A tone signal is valid
**       only if it arrives within one second (plus or minus the tone limit) from
**       the previous tone signal.Units are microseconds as measured with the local
**       clock.
**
**  \par Limits
**       Not Applicable
*/
#define CFE_PLATFORM_TIME_CFG_TONE_LIMIT 20000

/**
**  \cfetimecfg Define Time to Start Flywheel Since Last Tone
**
**  \par Description:
**       Define time to enter flywheel mode (in seconds since last tone data update)
**       Units are microseconds as measured with the local clock.
**
**  \par Limits
**       Not Applicable
*/
#define CFE_PLATFORM_TIME_CFG_START_FLY 2

/**
**  \cfetimecfg Define Periodic Time to Update Local Clock Tone Latch
**
**  \par Description:
**       Define Periodic Time to Update Local Clock Tone Latch. Applies only when
**       in flywheel mode. This define dictates the period at which the simulated
**       'last tone' time is updated. Units are seconds.
**
**  \par Limits
**       Not Applicable
*/
#define CFE_PLATFORM_TIME_CFG_LATCH_FLY 8

/**
**  \cfetimecfg Define TIME Task Priorities
**
**  \par Description:
**       Defines the cFE_TIME Task priority.
**       Defines the cFE_TIME Tone Task priority.
**       Defines the cFE_TIME 1HZ Task priority.
**
**  \par Limits
**       There is a lower limit of zero and an upper limit of 255 on these
**       configuration parameters.  Remember that the meaning of each task
**       priority is inverted -- a "lower" number has a "higher" priority.
*/
#define CFE_PLATFORM_TIME_START_TASK_PRIORITY 60
#define CFE_PLATFORM_TIME_TONE_TASK_PRIORITY  25
#define CFE_PLATFORM_TIME_ONEHZ_TASK_PRIORITY 25

/**
**  \cfetimecfg Define TIME Task Stack Sizes
**
**  \par Description:
**       Defines the cFE_TIME Main Task Stack Size
**       Defines the cFE_TIME Tone Task Stack Size
**       Defines the cFE_TIME 1HZ Task Stack Size
**
**  \par Limits
**       There is a lower limit of 2048 on these configuration parameters.  There
**       are no restrictions on the upper limit however, the maximum stack size
**       is system dependent and should be verified.  Most operating systems provide
**       tools for measuring the amount of stack used by a task during operation. It
**       is always a good idea to verify that no more than 1/2 of the stack is used.
*/
#define CFE_PLATFORM_TIME_START_TASK_STACK_SIZE CFE_PLATFORM_ES_DEFAULT_STACK_SIZE
#define CFE_PLATFORM_TIME_TONE_TASK_STACK_SIZE  4096
#define CFE_PLATFORM_TIME_ONEHZ_TASK_STACK_SIZE 8192

#endif
