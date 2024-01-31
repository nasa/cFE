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
 *   Specification for the CFE Time Services (CFE_TIME) command function codes
 *
 * @note
 *   This file should be strictly limited to the command/function code (CC)
 *   macro definitions.  Other definitions such as enums, typedefs, or other
 *   macros should be placed in the msgdefs.h or msg.h files.
 */
#ifndef CFE_TIME_FCNCODES_H
#define CFE_TIME_FCNCODES_H

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
#define CFE_TIME_SEND_DIAGNOSTIC_CC 2 /* request diagnostic hk telemetry */

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
**  \sa #CFE_TIME_ADD_ADJUST_CC, #CFE_TIME_SUB_ADJUST_CC, #CFE_TIME_ADD_ONE_HZ_ADJUSTMENT_CC,
*#CFE_TIME_SUB_ONE_HZ_ADJUSTMENT_CC
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
**  \sa #CFE_TIME_ADD_ADJUST_CC, #CFE_TIME_ADD_ONE_HZ_ADJUSTMENT_CC, #CFE_TIME_SUB_ONE_HZ_ADJUSTMENT_CC
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
**       #CFE_TIME_AddOneHzAdjustmentCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the
**       following telemetry:
**       - \b \c \TIME_CMDPC - command execution counter will increment
**       - \b \c \TIME_STCFSECS - Housekeeping Telemetry point indicating new STCF seconds value
**       - \b \c \TIME_STCFSUBSECS - Housekeeping Telemetry point indicating new STCF subseconds value
**       - The #CFE_TIME_ONEHZ_EID informational event message will be generated
**
**  \par Error Conditions
**       - Platform receiving the command is not a Time Server
**       <BR><BR>Evidence of Failure may be found in the following telemetry:
**       - \b \c \TIME_CMDEC - command error counter will increment
**       - Error specific event message will be issued (#CFE_TIME_ONEHZ_CFG_EID)
**
**  \par Criticality
**       Inappropriately setting the clock may result in other sub-systems performing incorrect
**       time based calculations.  The specific risk is dependent upon the behavior of those sub-systems.
**
**  \sa #CFE_TIME_ADD_ADJUST_CC, #CFE_TIME_SUB_ADJUST_CC, #CFE_TIME_SUB_ONE_HZ_ADJUSTMENT_CC
*/
#define CFE_TIME_ADD_ONE_HZ_ADJUSTMENT_CC 13 /* add 1Hz STCF adjustment */

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
**       #CFE_TIME_SubOneHzAdjustmentCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the
**       following telemetry:
**       Successful execution of this command may be verified with the
**       following telemetry:
**       - \b \c \TIME_CMDPC - command execution counter will increment
**       - \b \c \TIME_STCFSECS - Housekeeping Telemetry point indicating new STCF seconds value
**       - \b \c \TIME_STCFSUBSECS - Housekeeping Telemetry point indicating new STCF subseconds value
**       - The #CFE_TIME_ONEHZ_EID informational event message will be generated
**
**  \par Error Conditions
**       - Platform receiving the command is not a Time Server
**       <BR><BR>Evidence of Failure may be found in the following telemetry:
**       - \b \c \TIME_CMDEC - command error counter will increment
**       - Error specific event message will be issued (#CFE_TIME_ONEHZ_CFG_EID)
**
**  \par Criticality
**       Inappropriately setting the clock may result in other sub-systems performing incorrect
**       time based calculations.  The specific risk is dependent upon the behavior of those sub-systems.
**
**  \sa #CFE_TIME_ADD_ADJUST_CC, #CFE_TIME_SUB_ADJUST_CC, #CFE_TIME_ADD_ONE_HZ_ADJUSTMENT_CC
*/
#define CFE_TIME_SUB_ONE_HZ_ADJUSTMENT_CC 14 /* subtract 1Hz STCF adjustment */

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

#endif
