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
 * Purpose:  cFE Time Services (TIME) SB message definitions header file
 *
 * Author:   S.Walling/Microtel
 *
 * Notes:
 *
 */

#ifndef CFE_TIME_MSG_H
#define CFE_TIME_MSG_H

/*
** Required header files...
*/
#include "common_types.h" /* Basic data types */
#include "cfe_msg_hdr.h"  /* for header definitions */
#include "cfe_time_extern_typedefs.h"

/*************************************************************************/

/*
** Time task command packet command codes...
*/
/** \name Time Services Command Codes */
/** \{ */

/** \cfetimecmd Time No-Op
**
**  \par Description
**       This command performs no other function than to increment the
**       command execution counter. The command may be used to verify
**       general aliveness of the Time Services task.
**
**  \cfecmdmnemonic \TIME_NOOP
**
**  \par Command Structure
**       #CFE_TIME_NoopCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the
**       following telemetry:
**       - \b \c \TIME_CMDPC - command execution counter will increment
**       - The #CFE_TIME_NOOP_EID informational event message will be generated
**
**  \par Error Conditions
**       There are no error conditions for this command. If the Time
**       Services receives the command, the event is sent (although it
**       may be filtered by EVS) and the counter is incremented
**       unconditionally.
**
**  \par Criticality
**       None
**
**  \sa
*/
#define CFE_TIME_NOOP_CC 0 /* no-op command */

/** \cfetimecmd Time Reset Counters
**
**  \par Description
**       This command resets the following counters within the Time
**       Services \link CFE_TIME_HousekeepingTlm_t Housekeeping Telemetry \endlink:
**       - Command Execution Counter (\TIME_CMDPC)
**       - Command Error Counter (\TIME_CMDEC)
**       This command also resets the following counters within the
**       Time Services \link CFE_TIME_DiagnosticTlm_t Diagnostic Telemetry \endlink:
**       - Tone Signal Detected Software Bus Message Counter (\TIME_TSDETCNT)
**       - Time at the Tone Data Software Bus Message Counter (\TIME_TATTCNT)
**       - Tone Signal/Data Verify Counter (\TIME_VERIFYCNT)
**       - Tone Signal/Data Error Counter (\TIME_VERIFYER)
**       - Tone Signal Interrupt Counter (\TIME_TSISRCNT)
**       - Tone Signal Interrupt Error Counter (\TIME_TSISRERR)
**       - Tone Signal Task Counter (\TIME_TSTASKCNT)
**       - Local 1 Hz Interrupt Counter (\TIME_1HZISRCNT)
**       - Local 1 Hz Task Counter (\TIME_1HZTASKCNT)
**       - Reference Time Version Counter (\TIME_VERSIONCNT)
**
**  \cfecmdmnemonic \TIME_RESETCTRS
**
**  \par Command Structure
**       #CFE_TIME_ResetCountersCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the
**       following telemetry:
**       - \b \c \TIME_CMDPC - command execution counter will reset to 0
**       - \b \c \TIME_CMDEC - command error counter will reset to 0
**       - The #CFE_TIME_RESET_EID informational event message will be generated
**
**  \par Error Conditions
**       There are no error conditions for this command. If the Time
**       Services receives the command, the event is sent (although it
**       may be filtered by EVS) and the counter is reset
**       unconditionally.
**
**  \par Criticality
**       None
**
**  \sa
*/
#define CFE_TIME_RESET_COUNTERS_CC 1 /* reset counters */

/** \cfetimecmd Request TIME Diagnostic Telemetry
**
**  \par Description
**       This command requests that the Time Service generate a message
**       containing various data values not included in the normal Time
**       Service housekeeping message.  The command requests only a single
**       copy of the diagnostic message.  Refer to #CFE_TIME_DiagnosticTlm_t for
**       a description of the Time Service diagnostic message contents.
**
**  \cfecmdmnemonic \TIME_REQUESTDIAG
**
**  \par Command Structure
**       #CFE_TIME_SendDiagnosticCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the
**       following telemetry:
**       - \b \c \TIME_CMDPC - command execution counter will increment
**       - Sequence Counter for #CFE_TIME_DiagnosticTlm_t will increment
**       - The #CFE_TIME_DIAG_EID debug event message will be generated
**
**  \par Error Conditions
**       There are no error conditions for this command. If the Time
**       Services receives the command, the event and telemetry is sent
**       (although one or both may be filtered by EVS and TO) and the
**       counter is incremented unconditionally.
**
**  \par Criticality
**       None
**
**  \sa
*/
#define CFE_TIME_SEND_DIAGNOSTIC_TLM_CC 2 /* request diagnostic hk telemetry */

/** \cfetimecmd Set Time Source
**
**  \par Description
**       This command selects the Time Service clock source.  Although the
**       list of potential clock sources is mission specific and defined
**       via configuration parameters, this command provides a common method
**       for switching between the local processor clock and an external
**       source for time data.<BR><BR>
**       When commanded to accept external time data (GPS, MET, spacecraft
**       time, etc.), the Time Server will enable input via an API function
**       specific to the configuration definitions for the particular source.
**       When commanded to use internal time data, the Time Server will ignore
**       the external data.  However, the Time Server will continue to use the
**       API function as the trigger to generate a "time at the tone" command
**       packet regardless of the internal/external command selection.<BR><BR>
**       Notes:
**       - Operating in FLYWHEEL mode is not considered a choice related
**         to clock source, but rather an element of the clock state.  See below
**         for a description of the #CFE_TIME_SET_STATE_CC command.
**       - This command is only valid when the #CFE_PLATFORM_TIME_CFG_SOURCE configuration
**         parameter in the cfe_platform_cfg.h file has been set to true.
**
**  \cfecmdmnemonic \TIME_SETSOURCE
**
**  \par Command Structure
**       #CFE_TIME_SetSourceCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the
**       following telemetry:
**       - \b \c \TIME_CMDPC - command execution counter will increment
**       - \b \c \TIME_SOURCE - Diagnostic Telemetry point will indicate the
**         command specified value
**       - The #CFE_TIME_SOURCE_EID informational event message will be generated
**
**  \par Error Conditions
**       - Invalid Source selection
**         (a value other than #CFE_TIME_SourceSelect_INTERNAL or #CFE_TIME_SourceSelect_EXTERNAL was specified)
**       - Time source selection not allowed on this platform
**       <BR><BR>Evidence of failure may be found in the following telemetry:
**       - \b \c \TIME_CMDEC - Command Error counter will increment
**       - Error specific event message (either #CFE_TIME_SOURCE_CFG_EID or #CFE_TIME_SOURCE_ERR_EID)
**
**  \par Criticality
**       Although clock source selection is important, this command is not critical.
**
**  \sa #CFE_TIME_SET_STATE_CC, #CFE_TIME_SET_SIGNAL_CC
*/
#define CFE_TIME_SET_SOURCE_CC 3 /* set clock source (int vs ext) */

/** \cfetimecmd Set Time State
**
**  \par Description
**       This command indirectly affects the Time Service on-board determination
**       of clock state.  Clock state is a combination of factors, most significantly
**       whether the spacecraft time has been accurately set, and whether Time Service
**       is operating in FLYWHEEL mode.<BR><BR>
**       This command may be used to notify the Time Server that spacecraft time is
**       now correct, or that time is no longer correct.  This information will be
**       distributed to Time Clients, and in turn, to any interested sub-systems.<BR><BR>
**       Also, this command may be used to force a Time Server or Time Client into
**       FLYWHEEL mode.  Use of FLYWHEEL mode is mainly for debug purposes although
**       in extreme circumstances, it may be of value to force Time Service not to rely
**       on normal time updates.  Note that when commanded into FLYWHEEL mode, the Time
**       Service will remain so until receipt of another "set state" command setting the
**       state into a mode other than FLYWHEEL.<BR><BR>
**       Note also that setting the clock state to VALID or INVALID on a Time Client that
**       is currently getting time updates from the Time Server will have very limited
**       effect.  As soon as the Time Client receives the next time update, the VALID/INVALID
**       selection will be set to that of the Time Server.  However, setting a Time Client
**       to FLYWHEEL cannot be overridden by the Time Server since the Time Client will
**       ignore time updates from the Time Server while in FLYWHEEL mode.
**
**  \cfecmdmnemonic \TIME_SETSTATE
**
**  \par Command Structure
**       #CFE_TIME_SetStateCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the
**       following telemetry:
**       - \b \c \TIME_CMDPC - command execution counter will increment
**       - \b \c \TIME_STATEFLG - Housekeeping Telemetry point "may" indicate the
**         command specified value (see above)
**       - The #CFE_TIME_STATE_EID informational event message will be generated
**
**  \par Error Conditions
**       - Invalid State selection
**         (a value other than #CFE_TIME_ClockState_INVALID, #CFE_TIME_ClockState_VALID or
**         #CFE_TIME_ClockState_FLYWHEEL was specified)
**       - Time source selection not allowed on this platform
**       <BR><BR>Evidence of failure may be found in the following telemetry:
**       - \b \c \TIME_CMDEC - Command Error counter will increment
**       - Error specific event message (#CFE_TIME_STATE_ERR_EID)
**
**  \par Criticality
**       Setting Time Service into FLYWHEEL mode is not particularly hazardous, as
**       the result may be that the calculation of spacecraft time is done using a
**       less than optimal timer.  However, inappropriately setting the clock state
**       to VALID (indicating that spacecraft time is accurate) may result in other
**       sub-systems performing incorrect time based calculations.  The specific risk
**       is dependent upon the behavior of those sub-systems.
**
**  \sa #CFE_TIME_SET_SOURCE_CC, #CFE_TIME_SET_SIGNAL_CC
*/
#define CFE_TIME_SET_STATE_CC 4 /* set clock state */

/** \cfetimecmd Add Time to Tone Time Delay
**
**  \par Description
**       This command is used to factor out a known, predictable latency between the
**       Time Server and a particular Time Client.  The correction is applied (added)
**       to the current time calculation for Time Clients, so this command has no
**       meaning for Time Servers.  Each Time Client can have a unique latency setting.
**       The latency value is a positive number of seconds and microseconds that represent
**       the deviation from the time maintained by the Time Server.
**
**  \cfecmdmnemonic \TIME_ADDCLOCKLAT
**
**  \par Command Structure
**       #CFE_TIME_AddDelayCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the
**       following telemetry:
**       - \b \c \TIME_CMDPC - command execution counter will increment
**       - \b \c \TIME_LATENTS - Housekeeping Telemetry point indicating command specified values
**       - \b \c \TIME_LATENTDIR - Diagnostic Telemetry point indicating commanded latency direction
**       - The #CFE_TIME_DELAY_EID informational event message will be generated
**
**  \par Error Conditions
**       - An invalid number of microseconds was specified (must be less than 1 million)
**       - Platform receiving the command is not a Time Client
**       <BR><BR>Evidence of Failure may be found in the following telemetry:
**       - \b \c \TIME_CMDEC - command error counter will increment
**       - Error specific event messages will be issued (#CFE_TIME_DELAY_CFG_EID or #CFE_TIME_DELAY_ERR_EID)
**
**  \par Criticality
**       Inappropriately setting the clock may result in other sub-systems performing incorrect
**       time based calculations.  The specific risk is dependent upon the behavior of those sub-systems.
**
**  \sa #CFE_TIME_SUB_DELAY_CC
*/
#define CFE_TIME_ADD_DELAY_CC 5 /* add tone delay value */

/** \cfetimecmd Subtract Time from Tone Time Delay
**
**  \par Description
**       This command is used to factor out a known, predictable latency between the Time Server
**       and a particular Time Client.  The correction is applied (subtracted) to the current time
**       calculation for Time Clients, so this command has no meaning for Time Servers.  Each Time
**       Client can have a unique latency setting.  The latency value is a positive number of seconds
**       and microseconds that represent the deviation from the time maintained by the Time Server.<BR><BR>
**       Note that it is unimaginable that the seconds value will ever be anything but zero.
**
**  \cfecmdmnemonic \TIME_SUBCLOCKLAT
**
**  \par Command Structure
**       #CFE_TIME_SubDelayCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the
**       following telemetry:
**       - \b \c \TIME_CMDPC - command execution counter will increment
**       - \b \c \TIME_LATENTS - Housekeeping Telemetry point indicating command specified values
**       - \b \c \TIME_LATENTDIR - Diagnostic Telemetry point indicating commanded latency direction
**       - The #CFE_TIME_DELAY_EID informational event message will be generated
**
**  \par Error Conditions
**       - An invalid number of microseconds was specified (must be less than 1 million)
**       - Platform receiving the command is not a Time Client
**       <BR><BR>Evidence of Failure may be found in the following telemetry:
**       - \b \c \TIME_CMDEC - command error counter will increment
**       - Error specific event messages will be issued (#CFE_TIME_DELAY_CFG_EID or #CFE_TIME_DELAY_ERR_EID)
**
**  \par Criticality
**       Inappropriately setting the clock may result in other sub-systems performing incorrect
**       time based calculations.  The specific risk is dependent upon the behavior of those sub-systems.
**
**  \sa #CFE_TIME_ADD_DELAY_CC
*/
#define CFE_TIME_SUB_DELAY_CC 6 /* sub tone delay value */

/** \cfetimecmd Set Spacecraft Time
**
**  \par Description
**       This command sets the spacecraft clock to a new value,
**       regardless of the current setting (time jam). The new time
**       value represents the desired offset from the mission-defined
**       time epoch and takes effect immediately upon execution of
**       this command.  Time Service will calculate a new STCF value
**       based on the current MET and the desired new time using one
**       of the following: <BR><BR>
**       If Time Service is configured to compute current time as TAI<BR>
**       - <B> STCF = (new time) - (current MET) </B>
**       - <B> (current time) = (current MET) + STCF </B>
**       <BR><BR>If Time Service is configured to compute current time as UTC
**       - <B> STCF = ((new time) - (current MET)) + (Leap Seconds) </B>
**       - <B> (current time) = ((current MET) + STCF) - (Leap Seconds) </B>
**
**  \cfecmdmnemonic \TIME_SETCLOCK
**
**  \par Command Structure
**       #CFE_TIME_SetTimeCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the
**       following telemetry:
**       - \b \c \TIME_CMDPC - command execution counter will increment
**       - \b \c \TIME_STCFSECS - Housekeeping Telemetry point indicating newly calculated STCF seconds value
**       - \b \c \TIME_STCFSUBSECS - Housekeeping Telemetry point indicating newly calculated STCF subseconds value
**       - The #CFE_TIME_TIME_EID informational event message will be generated
**
**  \par Error Conditions
**       - An invalid number of microseconds was specified (must be less than 1 million)
**       - Platform receiving the command is not a Time Server
**       <BR><BR>Evidence of Failure may be found in the following telemetry:
**       - \b \c \TIME_CMDEC - command error counter will increment
**       - Error specific event messages will be issued (#CFE_TIME_TIME_CFG_EID or #CFE_TIME_TIME_ERR_EID)
**
**  \par Criticality
**       Inappropriately setting the clock may result in other sub-systems performing incorrect
**       time based calculations.  The specific risk is dependent upon the behavior of those sub-systems.
**
**  \sa #CFE_TIME_SET_MET_CC, #CFE_TIME_SET_STCF_CC, #CFE_TIME_SET_LEAP_SECONDS_CC
*/
#define CFE_TIME_SET_TIME_CC 7 /* set time */

/** \cfetimecmd Set Mission Elapsed Time
**
**  \par Description
**       This command sets the Mission Elapsed Timer (MET) to the specified value.<BR><BR>
**       Note that the MET (as implemented for cFE Time Service) is a logical representation
**       and not a physical timer.  Thus, setting the MET is not dependent on whether the
**       hardware supports a MET register that can be written to.<BR><BR>
**       Note also that Time Service "assumes" that during normal operation, the MET is
**       synchronized to the tone signal.  Therefore, unless operating in FLYWHEEL mode,
**       the sub-seconds portion of the MET will be set to zero at the next tone signal interrupt.<BR><BR>
**       The new MET takes effect immediately upon execution of this command.
**
**  \cfecmdmnemonic \TIME_SETCLOCKMET
**
**  \par Command Structure
**       #CFE_TIME_SetMETCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the
**       following telemetry:
**       - \b \c \TIME_CMDPC - command execution counter will increment
**       - \b \c \TIME_METSECS - Housekeeping Telemetry point indicating new MET seconds value
**       - \b \c \TIME_METSUBSECS - Housekeeping Telemetry point indicating new MET subseconds value
**       - The #CFE_TIME_MET_EID informational event message will be generated
**
**  \par Error Conditions
**       - An invalid number of microseconds was specified (must be less than 1 million)
**       - Platform receiving the command is not a Time Server
**       <BR><BR>Evidence of Failure may be found in the following telemetry:
**       - \b \c \TIME_CMDEC - command error counter will increment
**       - Error specific event messages will be issued (#CFE_TIME_MET_CFG_EID or #CFE_TIME_MET_ERR_EID)
**
**  \par Criticality
**       Inappropriately setting the clock may result in other sub-systems performing incorrect
**       time based calculations.  The specific risk is dependent upon the behavior of those sub-systems.
**
**  \sa #CFE_TIME_SET_TIME_CC, #CFE_TIME_SET_STCF_CC, #CFE_TIME_SET_LEAP_SECONDS_CC
*/
#define CFE_TIME_SET_MET_CC 8 /* set MET */

/** \cfetimecmd Set Spacecraft Time Correlation Factor
**
**  \par Description
**       This command sets the Spacecraft Time Correlation Factor (STCF) to the specified value.
**       This command differs from the previously described SET CLOCK in the nature of the command
**       argument.  This command sets the STCF value directly, rather than extracting the STCF
**       from a value representing the total of MET, STCF and optionally, Leap Seconds.  The new
**       STCF takes effect immediately upon execution of this command.
**
**  \cfecmdmnemonic \TIME_SETCLOCKSTCF
**
**  \par Command Structure
**       #CFE_TIME_SetSTCFCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the
**       following telemetry:
**       - \b \c \TIME_CMDPC - command execution counter will increment
**       - \b \c \TIME_STCFSECS - Housekeeping Telemetry point indicating new  STCF seconds value
**       - \b \c \TIME_STCFSUBSECS - Housekeeping Telemetry point indicating new  STCF subseconds value
**       - The #CFE_TIME_STCF_EID informational event message will be generated
**
**  \par Error Conditions
**       - An invalid number of microseconds was specified (must be less than 1 million)
**       - Platform receiving the command is not a Time Server
**       <BR><BR>Evidence of Failure may be found in the following telemetry:
**       - \b \c \TIME_CMDEC - command error counter will increment
**       - Error specific event messages will be issued (#CFE_TIME_STCF_CFG_EID or #CFE_TIME_STCF_ERR_EID)
**
**  \par Criticality
**       Inappropriately setting the clock may result in other sub-systems performing incorrect
**       time based calculations.  The specific risk is dependent upon the behavior of those sub-systems.
**
**  \sa #CFE_TIME_SET_TIME_CC, #CFE_TIME_SET_MET_CC, #CFE_TIME_SET_LEAP_SECONDS_CC
*/
#define CFE_TIME_SET_STCF_CC 9 /* set STCF */

/** \cfetimecmd Set Leap Seconds
**
**  \par Description
**       This command sets the spacecraft Leap Seconds to the specified value.
**       Leap Seconds may be positive or negative, and there is no limit to the
**       value except, of course, the limit imposed by the 16 bit signed integer
**       data type.  The new Leap Seconds value takes effect immediately upon
**       execution of this command.
**
**  \cfecmdmnemonic \TIME_SETCLOCKLEAP
**
**  \par Command Structure
**       #CFE_TIME_SetLeapSecondsCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the
**       following telemetry:
**       - \b \c \TIME_CMDPC - command execution counter will increment
**       - \b \c \TIME_LEAPSECS - Housekeeping Telemetry point indicating new Leap seconds value
**       - The #CFE_TIME_LEAPS_EID informational event message will be generated
**
**  \par Error Conditions
**       - Platform receiving the command is not a Time Server
**       <BR><BR>Evidence of Failure may be found in the following telemetry:
**       - \b \c \TIME_CMDEC - command error counter will increment
**       - Error specific event messages will be issued (#CFE_TIME_LEAPS_CFG_EID)
**
**  \par Criticality
**       Inappropriately setting the clock may result in other sub-systems performing incorrect
**       time based calculations.  The specific risk is dependent upon the behavior of those sub-systems.
**
**  \sa #CFE_TIME_SET_TIME_CC, #CFE_TIME_SET_MET_CC, #CFE_TIME_SET_STCF_CC
*/
#define CFE_TIME_SET_LEAP_SECONDS_CC 10 /* set Leap Seconds */

/** \cfetimecmd Add Delta to Spacecraft Time Correlation Factor
**
**  \par Description
**       This command adjusts the Spacecraft Time Correlation Factor (STCF) by
**       adding the specified value.  The new STCF takes effect immediately upon
**       execution of this command.
**
**  \cfecmdmnemonic \TIME_ADDSTCFADJ
**
**  \par Command Structure
**       #CFE_TIME_AddAdjustCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the
**       following telemetry:
**       - \b \c \TIME_CMDPC - command execution counter will increment
**       - \b \c \TIME_STCFSECS - Housekeeping Telemetry point indicating new STCF seconds value
**       - \b \c \TIME_STCFSUBSECS - Housekeeping Telemetry point indicating new STCF subseconds value
**       - The #CFE_TIME_DELTA_EID informational event message will be generated
**
**  \par Error Conditions
**       - An invalid number of microseconds was specified (must be less than 1 million)
**       - Platform receiving the command is not a Time Server
**       <BR><BR>Evidence of Failure may be found in the following telemetry:
**       - \b \c \TIME_CMDEC - command error counter will increment
**       - Error specific event messages will be issued (#CFE_TIME_DELTA_ERR_EID or #CFE_TIME_DELTA_CFG_EID)
**
**  \par Criticality
**       Inappropriately setting the clock may result in other sub-systems performing incorrect
**       time based calculations.  The specific risk is dependent upon the behavior of those sub-systems.
**
**  \sa #CFE_TIME_ADD_ADJUST_CC, #CFE_TIME_SUB_ADJUST_CC, #CFE_TIME_ADD_1HZ_ADJUSTMENT_CC,
*#CFE_TIME_SUB_1HZ_ADJUSTMENT_CC
*/
#define CFE_TIME_ADD_ADJUST_CC 11 /* add one time STCF adjustment */

/** \cfetimecmd Subtract Delta from Spacecraft Time Correlation Factor
**
**  \par Description
**       This command adjusts the Spacecraft Time Correlation Factor (STCF) by subtracting the specified
**       value.  The new STCF takes effect immediately upon execution of this command.
**
**  \cfecmdmnemonic \TIME_SUBSTCFADJ
**
**  \par Command Structure
**       #CFE_TIME_SubAdjustCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the
**       following telemetry:
**       - \b \c \TIME_CMDPC - command execution counter will increment
**       - \b \c \TIME_STCFSECS - Housekeeping Telemetry point indicating new STCF seconds value
**       - \b \c \TIME_STCFSUBSECS - Housekeeping Telemetry point indicating new STCF subseconds value
**       - The #CFE_TIME_DELTA_EID informational event message will be generated
**
**  \par Error Conditions
**       - An invalid number of microseconds was specified (must be less than 1 million)
**       - Platform receiving the command is not a Time Server
**       <BR><BR>Evidence of Failure may be found in the following telemetry:
**       - \b \c \TIME_CMDEC - command error counter will increment
**       - Error specific event messages will be issued (#CFE_TIME_DELTA_ERR_EID or #CFE_TIME_DELTA_CFG_EID)
**
**  \par Criticality
**       Inappropriately setting the clock may result in other sub-systems performing incorrect
**       time based calculations.  The specific risk is dependent upon the behavior of those sub-systems.
**
**  \sa #CFE_TIME_ADD_ADJUST_CC, #CFE_TIME_ADD_1HZ_ADJUSTMENT_CC, #CFE_TIME_SUB_1HZ_ADJUSTMENT_CC
*/
#define CFE_TIME_SUB_ADJUST_CC 12 /* subtract one time STCF adjustment */

/** \cfetimecmd Add Delta to Spacecraft Time Correlation Factor each 1Hz
**
**  \par Description
**       This command has been updated to take actual sub-seconds (1/2^32 seconds)
**       rather than micro-seconds as an input argument.  This change occurred
**       after the determination was made that one micro-second is too large an
**       increment for a constant 1Hz adjustment.<BR><BR>
**       This command continuously adjusts the Spacecraft Time Correlation Factor (STCF)
**       every second, by adding the specified value.  The adjustment to the STCF is
**       applied in the Time Service local 1Hz interrupt handler.  As the local 1Hz
**       interrupt is not synchronized to the tone signal, one cannot say when the
**       adjustment will occur, other than once a second, at about the same time
**       relative to the tone.<BR><BR>
**       There was some debate about whether the maximum 1Hz clock drift correction
**       factor would ever need to exceed some small fraction of a second.  But, the
**       decision was made to provide the capability to make 1Hz adjustments greater
**       than one second and leave it to the ground system to provide mission specific
**       limits.
**
**  \cfecmdmnemonic \TIME_ADD1HZSTCF
**
**  \par Command Structure
**       #CFE_TIME_Add1HZAdjustmentCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the
**       following telemetry:
**       - \b \c \TIME_CMDPC - command execution counter will increment
**       - \b \c \TIME_STCFSECS - Housekeeping Telemetry point indicating new STCF seconds value
**       - \b \c \TIME_STCFSUBSECS - Housekeeping Telemetry point indicating new STCF subseconds value
**       - The #CFE_TIME_1HZ_EID informational event message will be generated
**
**  \par Error Conditions
**       - Platform receiving the command is not a Time Server
**       <BR><BR>Evidence of Failure may be found in the following telemetry:
**       - \b \c \TIME_CMDEC - command error counter will increment
**       - Error specific event message will be issued (#CFE_TIME_1HZ_CFG_EID)
**
**  \par Criticality
**       Inappropriately setting the clock may result in other sub-systems performing incorrect
**       time based calculations.  The specific risk is dependent upon the behavior of those sub-systems.
**
**  \sa #CFE_TIME_ADD_ADJUST_CC, #CFE_TIME_SUB_ADJUST_CC, #CFE_TIME_SUB_1HZ_ADJUSTMENT_CC
*/
#define CFE_TIME_ADD_1HZ_ADJUSTMENT_CC 13 /* add 1Hz STCF adjustment */

/** \cfetimecmd Subtract Delta from Spacecraft Time Correlation Factor each 1Hz
**
**  \par Description
**       This command has been updated to take actual sub-seconds (1/2^32 seconds)
**       rather than micro-seconds as an input argument.  This change occurred
**       after the determination was made that one micro-second is too large an
**       increment for a constant 1Hz adjustment.<BR><BR>
**       This command continuously adjusts the Spacecraft Time Correlation Factor (STCF)
**       every second, by subtracting the specified value.  The adjustment to the STCF
**       is applied in the Time Service local 1Hz interrupt handler.  As the local 1Hz
**       interrupt is not synchronized to the tone signal, one cannot say when the
**       adjustment will occur, other than once a second, at about the same time
**       relative to the tone.<BR><BR>
**       There was some debate about whether the maximum 1Hz clock drift correction
**       factor would ever need to exceed some small fraction of a second.  But, the
**       decision was made to provide the capability to make 1Hz adjustments greater
**       than one second and leave it to the ground system to provide mission specific
**       limits.
**
**  \cfecmdmnemonic \TIME_SUB1HZSTCF
**
**  \par Command Structure
**       #CFE_TIME_Sub1HZAdjustmentCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the
**       following telemetry:
**       Successful execution of this command may be verified with the
**       following telemetry:
**       - \b \c \TIME_CMDPC - command execution counter will increment
**       - \b \c \TIME_STCFSECS - Housekeeping Telemetry point indicating new STCF seconds value
**       - \b \c \TIME_STCFSUBSECS - Housekeeping Telemetry point indicating new STCF subseconds value
**       - The #CFE_TIME_1HZ_EID informational event message will be generated
**
**  \par Error Conditions
**       - Platform receiving the command is not a Time Server
**       <BR><BR>Evidence of Failure may be found in the following telemetry:
**       - \b \c \TIME_CMDEC - command error counter will increment
**       - Error specific event message will be issued (#CFE_TIME_1HZ_CFG_EID)
**
**  \par Criticality
**       Inappropriately setting the clock may result in other sub-systems performing incorrect
**       time based calculations.  The specific risk is dependent upon the behavior of those sub-systems.
**
**  \sa #CFE_TIME_ADD_ADJUST_CC, #CFE_TIME_SUB_ADJUST_CC, #CFE_TIME_ADD_1HZ_ADJUSTMENT_CC
*/
#define CFE_TIME_SUB_1HZ_ADJUSTMENT_CC 14 /* subtract 1Hz STCF adjustment */

/** \cfetimecmd Set Tone Signal Source
**
**  \par Description
**       This command selects the Time Service tone signal source.  Although the
**       list of potential tone signal sources is mission specific, a common
**       choice is the selection of primary or redundant tone signal.  The selection
**       may be available to both the Time Server and Time Clients, depending on
**       hardware configuration.<BR><BR>
**       Notes:
**       - This command is only valid when the #CFE_PLATFORM_TIME_CFG_SIGNAL configuration
**         parameter in the cfe_platform_cfg.h file has been set to true.
**
**  \cfecmdmnemonic \TIME_SETSIGNAL
**
**  \par Command Structure
**       #CFE_TIME_SetSignalCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the
**       following telemetry:
**       - \b \c \TIME_CMDPC - command execution counter will
**         increment
**       - \b \c \TIME_SIGNAL - Diagnostic Telemetry point will indicate the
**         command specified value
**       - The #CFE_TIME_SIGNAL_EID informational event message will
**         be generated
**
**  \par Error Conditions
**       - Invalid Signal selection
**         (a value other than #CFE_TIME_ToneSignalSelect_PRIMARY or #CFE_TIME_ToneSignalSelect_REDUNDANT was specified)
**       - Multiple Tone Signal Sources not available on this platform
**       <BR><BR>Evidence of failure may be found in the following telemetry:
**       - \b \c \TIME_CMDEC - Command Error counter will increment
**       - Error specific event message (either #CFE_TIME_SIGNAL_CFG_EID or #CFE_TIME_SIGNAL_ERR_EID)
**
**  \par Criticality
**       Although tone signal source selection is important, this command is not critical
**
**  \sa #CFE_TIME_SET_STATE_CC, #CFE_TIME_SET_SOURCE_CC
*/
#define CFE_TIME_SET_SIGNAL_CC 15 /* set clock signal (pri vs red) */
/** \} */

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

/*************************************************************************/

/**
 * \brief Generic no argument command
 */
typedef struct CFE_TIME_NoArgsCmd
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command header */
} CFE_TIME_NoArgsCmd_t;

/*
 * A separate typedef for each of the commands that share this definition
 * This follows the convention for command handler prototypes and allows
 * each one to independently evolve as necessary.
 */
typedef CFE_TIME_NoArgsCmd_t CFE_TIME_NoopCmd_t;
typedef CFE_TIME_NoArgsCmd_t CFE_TIME_ResetCountersCmd_t;
typedef CFE_TIME_NoArgsCmd_t CFE_TIME_SendDiagnosticCmd_t;
typedef CFE_TIME_NoArgsCmd_t CFE_TIME_1HzCmd_t;
typedef CFE_TIME_NoArgsCmd_t CFE_TIME_ToneSignalCmd_t;
typedef CFE_TIME_NoArgsCmd_t CFE_TIME_FakeToneCmd_t;
typedef CFE_TIME_NoArgsCmd_t CFE_TIME_SendHkCmd_t;

/**
 * \brief Set leap seconds command payload
 */
typedef struct CFE_TIME_LeapsCmd_Payload
{
    int16 LeapSeconds;
} CFE_TIME_LeapsCmd_Payload_t;

/**
 * \brief Set leap seconds command
 */
typedef struct CFE_TIME_SetLeapSecondsCmd
{
    CFE_MSG_CommandHeader_t     CommandHeader; /**< \brief Command header */
    CFE_TIME_LeapsCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_TIME_SetLeapSecondsCmd_t;

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
 * \brief Set clock state command
 */
typedef struct CFE_TIME_SetStateCmd
{
    CFE_MSG_CommandHeader_t     CommandHeader; /**< \brief Command header */
    CFE_TIME_StateCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_TIME_SetStateCmd_t;

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
 * \brief Set time data source command
 */
typedef struct CFE_TIME_SetSourceCmd
{
    CFE_MSG_CommandHeader_t      CommandHeader; /**< \brief Command header */
    CFE_TIME_SourceCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_TIME_SetSourceCmd_t;

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
 * \brief Set tone signal source command
 */
typedef struct CFE_TIME_SetSignalCmd
{
    CFE_MSG_CommandHeader_t      CommandHeader; /**< \brief Command header */
    CFE_TIME_SignalCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_TIME_SetSignalCmd_t;

/**
 * \brief Generic seconds, microseconds command payload
 */
typedef struct CFE_TIME_TimeCmd_Payload
{
    uint32 Seconds;
    uint32 MicroSeconds;
} CFE_TIME_TimeCmd_Payload_t;

/**
 * \brief Generic seconds, microseconds argument command
 */
typedef struct CFE_TIME_TimeCmd
{
    CFE_MSG_CommandHeader_t    CommandHeader; /**< \brief Command header */
    CFE_TIME_TimeCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_TIME_TimeCmd_t;

/*
 * A separate typedef for each of the commands that share this definition
 * This follows the convention for command handler prototypes and allows
 * each one to independently evolve as necessary.
 */
typedef CFE_TIME_TimeCmd_t CFE_TIME_AddDelayCmd_t;
typedef CFE_TIME_TimeCmd_t CFE_TIME_SubDelayCmd_t;
typedef CFE_TIME_TimeCmd_t CFE_TIME_SetMETCmd_t;
typedef CFE_TIME_TimeCmd_t CFE_TIME_SetSTCFCmd_t;
typedef CFE_TIME_TimeCmd_t CFE_TIME_AddAdjustCmd_t;
typedef CFE_TIME_TimeCmd_t CFE_TIME_SubAdjustCmd_t;
typedef CFE_TIME_TimeCmd_t CFE_TIME_SetTimeCmd_t;

/**
 * \brief Generic seconds, subseconds command payload
 */
typedef struct CFE_TIME_OneHzAdjustmentCmd_Payload
{
    uint32 Seconds;
    uint32 Subseconds;
} CFE_TIME_OneHzAdjustmentCmd_Payload_t;

/**
 * \brief Generic seconds, subseconds adjustment command
 */
typedef struct CFE_TIME_OneHzAdjustmentCmd
{
    CFE_MSG_CommandHeader_t               CommandHeader; /**< \brief Command header */
    CFE_TIME_OneHzAdjustmentCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_TIME_OneHzAdjustmentCmd_t;

/*
 * A separate typedef for each of the commands that share this definition
 * This follows the convention for command handler prototypes and allows
 * each one to independently evolve as necessary.
 */
typedef CFE_TIME_OneHzAdjustmentCmd_t CFE_TIME_Add1HZAdjustmentCmd_t;
typedef CFE_TIME_OneHzAdjustmentCmd_t CFE_TIME_Sub1HZAdjustmentCmd_t;

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

/**
 * \brief Time at tone data command
 */
typedef struct CFE_TIME_ToneDataCmd
{
    CFE_MSG_CommandHeader_t        CommandHeader; /**< \brief Command header */
    CFE_TIME_ToneDataCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_TIME_ToneDataCmd_t;

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

typedef struct CFE_TIME_HousekeepingTlm
{
    CFE_MSG_TelemetryHeader_t          TelemetryHeader; /**< \brief Telemetry header */
    CFE_TIME_HousekeepingTlm_Payload_t Payload;         /**< \brief Telemetry payload */
} CFE_TIME_HousekeepingTlm_t;

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

typedef struct CFE_TIME_DiagnosticTlm
{
    CFE_MSG_TelemetryHeader_t        TelemetryHeader; /**< \brief Telemetry header */
    CFE_TIME_DiagnosticTlm_Payload_t Payload;         /**< \brief Telemetry payload */
} CFE_TIME_DiagnosticTlm_t;

#endif /* CFE_TIME_MSG_H */
