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

/*
**  Filename: cfe_time_events.h
**
**  Purpose:
**	           cFE Time Services (Time) Event IDs
**
**  Design Notes:
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**
*/

#ifndef _cfe_time_events_
#define _cfe_time_events_

/* **************************
** ****** Maximum EID. ******
** **************************
** The EID's below may not necessarily be in order, so it can be difficult to
** determine what the next EID is to use. When you add EID's, start with MAX_EID + 1
** and when you're done adding, set this to the highest EID you used. It may
** be worthwhile to, on occasion, re-number the EID's to put them back in order.
*/
#define CFE_TIME_MAX_EID         49

/*
** Event message ID's...
*/
/** \brief <tt> 'cFE TIME Initialized' </tt>
**  \event <tt> 'cFE TIME Initialized' </tt> 
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is always automatically issued when the Time Services
**  Task completes its Initialization.
**/
#define CFE_TIME_INIT_EID        1    /* start up message "informational" */

/** \brief <tt> 'No-op command' </tt>
**  \event <tt> 'No-op command' </tt> 
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is always automatically issued in response 
**  to a cFE Time Services \link #CFE_TIME_NOOP_CC NO-OP command \endlink
**/
#define CFE_TIME_NOOP_EID        4    /* processed command "informational" */

/** \brief <tt> 'Reset Counters command' </tt>
**  \event <tt> 'Reset Counters command' </tt> 
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is always automatically issued in response 
**  to a cFE Time Services \link #CFE_TIME_RESET_COUNTERS_CC Reset Counters command \endlink
**/
#define CFE_TIME_RESET_EID       5

/** \brief <tt> 'Request diagnostics command' </tt>
**  \event <tt> 'Request diagnostics command' </tt> 
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is always automatically issued in response 
**  to a cFE Time Services \link #CFE_TIME_SEND_DIAGNOSTIC_TLM_CC Request Diagnostics command \endlink
**/
#define CFE_TIME_DIAG_EID        6

/** \brief <tt> 'Set Clock State = \%s' </tt>
**  \event <tt> 'Set Clock State = \%s' </tt> 
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is generated upon successful completion 
**  of a cFE Time Services \link #CFE_TIME_SET_STATE_CC Set Time State command \endlink
**
**  The \c '\%s' field will identify whether the command specified
**  \c VALID, \c INVALID, or \c FLYWHEEL.
**/
#define CFE_TIME_STATE_EID       7

/** \brief <tt> 'Set Time Source = \%s' </tt>
**  \event <tt> 'Set Time Source = \%s' </tt> 
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is generated upon successful completion 
**  of a cFE Time Services \link #CFE_TIME_SET_SOURCE_CC Set Time Source command \endlink
**
**  The \c '\%s' field will identify whether the command specified
**  \c INTERNAL, or \c EXTERNAL.
**/
#define CFE_TIME_SOURCE_EID      8

/** \brief <tt> 'Set Tone Source = \%s' </tt>
**  \event <tt> 'Set Tone Source = \%s' </tt> 
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is generated upon successful completion 
**  of a cFE Time Services \link #CFE_TIME_SET_SIGNAL_CC Set Clock Signal command \endlink
**
**  The \c '\%s' field will identify whether the command specified
**  \c PRIMARY, or \c REDUNDANT.
**/
#define CFE_TIME_SIGNAL_EID      9


/** \brief <tt> 'Set Tone Delay -- secs = \%d, usecs = \%d, ssecs = 0x\%X, dir = \%d' </tt>
**  \event <tt> 'Set Tone Delay -- secs = \%d, usecs = \%d, ssecs = 0x\%X, dir = \%d' </tt> 
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is generated upon successful completion 
**  of either a cFE Time Services 
**  \link #CFE_TIME_ADD_DELAY_CC Add Time Delay \endlink OR a
**  \link #CFE_TIME_SUB_DELAY_CC Subtract Time Delay command \endlink
**
**  The \c secs field specifies the new delay (in seconds), the \c usecs
**  field specifies the delay in micro-seconds, the \c ssecs field is
**  the micro-seconds field converted to Spacecraft Time sub-seconds and
**  the \c dir field identifies the direction of the delay.  The direction
**  can be either #CFE_TIME_AdjustDirection_ADD or #CFE_TIME_AdjustDirection_SUBTRACT.
**/
#define CFE_TIME_DELAY_EID       11

/** \brief <tt> 'Set Time -- secs = \%d, usecs = \%d, ssecs = 0x\%X' </tt>
**  \event <tt> 'Set Time -- secs = \%d, usecs = \%d, ssecs = 0x\%X' </tt> 
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is generated upon successful completion 
**  of a cFE Time Services \link #CFE_TIME_SET_TIME_CC Set Time command \endlink
**
**  The \c secs field specifies the new spacecraft time (in seconds), the \c usecs
**  field specifies the spacecraft time micro-seconds, the \c ssecs field is
**  the micro-seconds field converted to Spacecraft Time sub-seconds
**/
#define CFE_TIME_TIME_EID        12

/** \brief <tt> 'Set MET -- secs = \%d, usecs = \%d, ssecs = 0x\%X' </tt>
**  \event <tt> 'Set MET -- secs = \%d, usecs = \%d, ssecs = 0x\%X' </tt> 
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is generated upon successful completion 
**  of a cFE Time Services \link #CFE_TIME_SET_MET_CC Set Mission Elapsed Time command \endlink
**
**  The \c secs field specifies the new MET (in seconds), the \c usecs
**  field specifies the MET micro-seconds, the \c ssecs field is
**  the micro-seconds field converted to Spacecraft Time sub-seconds
**/
#define CFE_TIME_MET_EID         13

/** \brief <tt> 'Set STCF -- secs = \%d, usecs = \%d, ssecs = 0x\%X' </tt>
**  \event <tt> 'Set STCF -- secs = \%d, usecs = \%d, ssecs = 0x\%X' </tt> 
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is generated upon successful completion 
**  of a cFE Time Services \link #CFE_TIME_SET_STCF_CC Set Spacecraft Time 
**  Correlation Factor command \endlink
**
**  The \c secs field specifies the new STCF (in seconds), the \c usecs
**  field specifies the STCF micro-seconds, the \c ssecs field is
**  the micro-seconds field converted to Spacecraft Time sub-seconds.
**/
#define CFE_TIME_STCF_EID        14

/** \brief <tt> 'STCF Adjust -- secs = \%d, usecs = \%d, ssecs = 0x\%X, dir[1=Positive, 2=Negative] = \%d' </tt>
**  \event <tt> 'STCF Adjust -- secs = \%d, usecs = \%d, ssecs = 0x\%X, dir[1=Positive, 2=Negative] = \%d' </tt> 
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is generated upon successful completion 
**  of any of the following cFE Time Services STCF Adjustment Commands:
**   - \link #CFE_TIME_ADD_ADJUST_CC Add Single STCF Adjustment command \endlink
**   - \link #CFE_TIME_SUB_ADJUST_CC Subtract Single STCF Adjustment command \endlink
**
**  The \c secs field specifies the number of seconds the STCF is to be adjusted by,
**  the \c usecs field specifies the number of micro-seconds, the \c ssecs field is
**  the micro-seconds field converted to Spacecraft Time sub-seconds and the \c dir
**  field identifies whether the adjustment was added or subtracted. The direction
**  can be either #CFE_TIME_AdjustDirection_ADD or #CFE_TIME_AdjustDirection_SUBTRACT.
**/
#define CFE_TIME_DELTA_EID       15

/** \brief <tt> 'STCF 1Hz Adjust -- secs = \%d, ssecs = 0x\%X, dir = \%d' </tt>
**  \event <tt> 'STCF 1Hz Adjust -- secs = \%d, ssecs = 0x\%X, dir = \%d' </tt> 
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is generated upon successful completion 
**  of any of the following cFE Time Services STCF Adjustment Commands:
**   - \link #CFE_TIME_ADD_1HZ_ADJUSTMENT_CC Add STCF Adjustment each second command \endlink
**   - \link #CFE_TIME_SUB_1HZ_ADJUSTMENT_CC Subtract STCF Adjustment each second command \endlink
**
**  The \c secs field specifies the number of seconds the STCF is to be adjusted by,
**  the \c ssecs field specifies the number of sub-seconds (1/2^32 seconds) the STCF
**  is to be adjusted by and the \c dir field identifies whether the adjustment was
**  added or subtracted. The direction value can be either #CFE_TIME_AdjustDirection_ADD or
**  #CFE_TIME_AdjustDirection_SUBTRACT.
**/
#define CFE_TIME_1HZ_EID         16

/** \brief <tt> 'Set Leap Seconds = \%d' </tt>
**  \event <tt> 'Set Leap Seconds = \%d' </tt> 
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is generated upon successful completion 
**  of the \link #CFE_TIME_SET_LEAP_SECONDS_CC Set Leap Seconds command \endlink
**
**  The \c \%d field contains the number of seconds the Spacecraft's Leap Seconds
**  has been set to.
**/
#define CFE_TIME_LEAPS_EID       17

/** \brief <tt> 'Start FLYWHEEL' </tt>
**  \event <tt> 'Start FLYWHEEL' </tt> 
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is generated whenever the Time Services enters FLYWHEEL mode.
**/
#define CFE_TIME_FLY_ON_EID      20   /* flywheel state "informational" */

/** \brief <tt> 'Stop FLYWHEEL' </tt>
**  \event <tt> 'Stop FLYWHEEL' </tt> 
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is generated whenever the Time Services exits FLYWHEEL mode.
**/
#define CFE_TIME_FLY_OFF_EID     21

#define CFE_TIME_EXIT_ERR_EID    25   /* task termination "error" */

/** \brief <tt> 'Invalid message ID -- ID = 0x\%X' </tt>
**  \event <tt> 'Invalid message ID -- ID = 0x\%X' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated whenever Time Services receives
**  a message from the software bus that is not one of Time Services
**  recognized messages.
**
**  The \c ID field specifies, in hex, the message ID of the inappropriately
**  received message.
**/
#define CFE_TIME_ID_ERR_EID      26   /* invalid command packet "error" */

/** \brief <tt> 'Invalid command code -- ID = 0x\%X, CC = \%d' </tt>
**  \event <tt> 'Invalid command code -- ID = 0x\%X, CC = \%d' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated whenever Time Services receives
**  a message from the software bus that contains a unrecognized command
**  code in its header..
**
**  The \c ID field specifies, in hex, the message ID of the message containing
**  the unrecognized command code, identified, in decimal, by the \c CC field.
**/
#define CFE_TIME_CC_ERR_EID      27

/** \brief <tt> 'Invalid Clock State = 0x\%X' </tt>
**  \event <tt> 'Invalid Clock State = 0x\%X' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated whenever Time Services receives
**  a \link #CFE_TIME_SET_STATE_CC Set Clock State Command \endlink
**  that contains a desired clock state that is none of the following:
**  - #CFE_TIME_ClockState_INVALID
**  - #CFE_TIME_ClockState_VALID
**  - #CFE_TIME_ClockState_FLYWHEEL
**
**  The \c State field specifies, in hex, the state value received 
**  in the command message.
**/
#define CFE_TIME_STATE_ERR_EID   30   /* processed command "error" */

/** \brief <tt> 'Invalid Time Source = 0x\%X' </tt>
**  \event <tt> 'Invalid Time Source = 0x\%X' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated whenever Time Services receives
**  a \link #CFE_TIME_SET_SOURCE_CC Set Clock Source Command \endlink
**  that contains a desired clock source that is none of the following:
**  - #CFE_TIME_SourceSelect_INTERNAL
**  - #CFE_TIME_SourceSelect_EXTERNAL
**
**  The \c Source field specifies, in hex, the source value received 
**  in the command message.
**/
#define CFE_TIME_SOURCE_ERR_EID  31

/** \brief <tt> 'Invalid Tone Source = 0x\%X' </tt>
**  \event <tt> 'Invalid Tone Source = 0x\%X' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated whenever Time Services receives
**  a \link #CFE_TIME_SET_SIGNAL_CC Set Clock Signal Command \endlink
**  that contains a desired clock source that is none of the following:
**  - #CFE_TIME_ToneSignalSelect_PRIMARY
**  - #CFE_TIME_ToneSignalSelect_REDUNDANT
**
**  The \c Source field specifies, in hex, the signal source value received 
**  in the command message.
**/
#define CFE_TIME_SIGNAL_ERR_EID  32

/** \brief <tt> 'Invalid Tone Delay -- secs = \%d, usecs = \%d' </tt>
**  \event <tt> 'Invalid Tone Delay -- secs = \%d, usecs = \%d' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated whenever Time Services receives
**  either a \link #CFE_TIME_ADD_DELAY_CC Add Tone Delay Command \endlink
**  OR a \link #CFE_TIME_SUB_DELAY_CC Subtract Tone Delay Command \endlink
**  that contains a microsecond field that is greater than or equal to
**  1000000.
**
**  The \c secs field specifies, in decimal, the tone signal delay in seconds
**  and the \c usecs field specifies, in decimal, the micro-second delay that
**  was in error.
**/
#define CFE_TIME_DELAY_ERR_EID   33

/** \brief <tt> 'Invalid Time -- secs = \%d, usecs = \%d' </tt>
**  \event <tt> 'Invalid Time -- secs = \%d, usecs = \%d' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated whenever Time Services receives
**  a \link #CFE_TIME_SET_TIME_CC Set Spacecraft Time Command \endlink
**  that contains a microsecond field that is greater than or equal to
**  1,000,000.
**
**  The \c secs field specifies, in decimal, the spacecraft time in seconds
**  and the \c usecs field specifies, in decimal, the micro-second field of
**  the spacecraft time that was in error.
**/
#define CFE_TIME_TIME_ERR_EID    34

/** \brief <tt> 'Invalid MET -- secs = \%d, usecs = \%d' </tt>
**  \event <tt> 'Invalid MET -- secs = \%d, usecs = \%d' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated whenever Time Services receives
**  a \link #CFE_TIME_SET_MET_CC Set Mission Elapsed Time Command \endlink
**  that contains a microsecond field that is greater than or equal to
**  1,000,000.
**
**  The \c secs field specifies, in decimal, the MET in seconds
**  and the \c usecs field specifies, in decimal, the micro-second field of
**  the MET that was in error.
**/
#define CFE_TIME_MET_ERR_EID     35

/** \brief <tt> 'Invalid STCF -- secs = \%d, usecs = \%d' </tt>
**  \event <tt> 'Invalid STCF -- secs = \%d, usecs = \%d' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated whenever Time Services receives
**  a \link #CFE_TIME_SET_STCF_CC Set Spacecraft Time Correlation Factor Command \endlink
**  that contains a microsecond field that is greater than or equal to
**  1,000,000.
**
**  The \c secs field specifies, in decimal, the STCF in seconds
**  and the \c usecs field specifies, in decimal, the micro-second field of
**  the STCF that was in error.
**/
#define CFE_TIME_STCF_ERR_EID    36

/** \brief <tt> 'Invalid STCF Adjust -- secs = \%d, usecs = \%d, dir[1=Positive, 2=Negative] = \%d' </tt>
**  \event <tt> 'Invalid STCF Adjust -- secs = \%d, usecs = \%d, dir[1=Positive, 2=Negative] = \%d' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated whenever Time Services receives
**  either a \link #CFE_TIME_ADD_ADJUST_CC Add Single STCF Adjustment Command \endlink
**  OR a \link #CFE_TIME_SUB_ADJUST_CC Subtract Single STCF Adjustment command \endlink
**  that contains a microsecond field that is greater than or equal to
**  1,000,000.
**
**  The \c secs field specifies the number of seconds the STCF is to be adjusted by,
**  the \c usecs field specifies the number of micro-seconds that was in error, the \c dir
**  field identifies whether the adjustment was to be added or subtracted. The direction
**  can be either #CFE_TIME_AdjustDirection_ADD or #CFE_TIME_AdjustDirection_SUBTRACT.
**/
#define CFE_TIME_DELTA_ERR_EID   37

/** (obsolete - unused)
**/
#define CFE_TIME_1HZ_ERR_EID     38

/** \brief <tt> 'Set Source commands invalid without CFE_PLATFORM_TIME_CFG_SOURCE set to true' </tt>
**  \event <tt> 'Set Source commands invalid without CFE_PLATFORM_TIME_CFG_SOURCE set to true' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated whenever Time Services receives
**  a \link #CFE_TIME_SET_SOURCE_CC Set Clock Source Command \endlink
**  and the Time Services configuration parameter #CFE_PLATFORM_TIME_CFG_SOURCE has
**  not been set to true in the cfe_platform_cfg.h file.
**/
#define CFE_TIME_SOURCE_CFG_EID  40   /* cmd disabled per cfg "error" */

/** \brief <tt> 'Set Signal commands invalid without CFE_PLATFORM_TIME_CFG_SIGNAL set to true' </tt>
**  \event <tt> 'Set Signal commands invalid without CFE_PLATFORM_TIME_CFG_SIGNAL set to true' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated whenever Time Services receives
**  a \link #CFE_TIME_SET_SIGNAL_CC Set Clock Signal Command \endlink
**  and the Time Services configuration parameter #CFE_PLATFORM_TIME_CFG_SIGNAL has
**  not been set to true in the cfe_platform_cfg.h file.
**/
#define CFE_TIME_SIGNAL_CFG_EID  41

/** \brief <tt> 'Set Delay commands invalid without CFE_PLATFORM_TIME_CFG_CLIENT set to true' </tt>
**  \event <tt> 'Set Delay commands invalid without CFE_PLATFORM_TIME_CFG_CLIENT set to true' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated whenever Time Services receives
**  either a \link #CFE_TIME_ADD_DELAY_CC Add Tone Delay Command \endlink
**  OR a \link #CFE_TIME_SUB_DELAY_CC Subtract Tone Delay Command \endlink
**  and the Time Services configuration parameter #CFE_PLATFORM_TIME_CFG_CLIENT has
**  not been set to true in the cfe_platform_cfg.h file.
**/
#define CFE_TIME_DELAY_CFG_EID   42

/** \brief <tt> 'Set Time commands invalid without CFE_PLATFORM_TIME_CFG_SERVER set to true' </tt>
**  \event <tt> 'Set Time commands invalid without CFE_PLATFORM_TIME_CFG_SERVER set to true' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated whenever Time Services receives
**  a \link #CFE_TIME_SET_TIME_CC Set Spacecraft Time Command \endlink
**  and the Time Services configuration parameter #CFE_PLATFORM_TIME_CFG_SERVER has
**  not been set to true in the cfe_platform_cfg.h file.
**/
#define CFE_TIME_TIME_CFG_EID    43

/** \brief <tt> 'Set MET commands invalid without CFE_PLATFORM_TIME_CFG_SERVER set to true' </tt>
**  \event <tt> 'Set MET commands invalid without CFE_PLATFORM_TIME_CFG_SERVER set to true' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated whenever Time Services receives
**  a \link #CFE_TIME_SET_MET_CC Set Mission Elapsed Time Command \endlink
**  and the Time Services configuration parameter #CFE_PLATFORM_TIME_CFG_SERVER has
**  not been set to true in the cfe_platform_cfg.h file.
**/
#define CFE_TIME_MET_CFG_EID     44

/** \brief <tt> 'Set STCF commands invalid without CFE_PLATFORM_TIME_CFG_SERVER set to true' </tt>
**  \event <tt> 'Set STCF commands invalid without CFE_PLATFORM_TIME_CFG_SERVER set to true' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated whenever Time Services receives
**  a \link #CFE_TIME_SET_STCF_CC Set Spacecraft Time Correlation Factor Command \endlink
**  and the Time Services configuration parameter #CFE_PLATFORM_TIME_CFG_SERVER has
**  not been set to true in the cfe_platform_cfg.h file.
**/
#define CFE_TIME_STCF_CFG_EID    45

/** \brief <tt> 'Set Leaps commands invalid without CFE_PLATFORM_TIME_CFG_SERVER set to true' </tt>
**  \event <tt> 'Set Leaps commands invalid without CFE_PLATFORM_TIME_CFG_SERVER set to true' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated whenever Time Services receives
**  a \link #CFE_TIME_SET_LEAP_SECONDS_CC Set Leap Seconds Command \endlink
**  and the Time Services configuration parameter #CFE_PLATFORM_TIME_CFG_SERVER has
**  not been set to true in the cfe_platform_cfg.h file.
**/
#define CFE_TIME_LEAPS_CFG_EID   46

/** \brief <tt> 'STCF Adjust commands invalid without CFE_PLATFORM_TIME_CFG_SERVER set to true' </tt>
**  \event <tt> 'STCF Adjust commands invalid without CFE_PLATFORM_TIME_CFG_SERVER set to true' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated whenever Time Services receives
**  either a \link #CFE_TIME_ADD_ADJUST_CC Add Single STCF Adjustment Command \endlink
**  OR a \link #CFE_TIME_SUB_ADJUST_CC Subtract Single STCF Adjustment command \endlink
**  and the Time Services configuration parameter #CFE_PLATFORM_TIME_CFG_SERVER has
**  not been set to true in the cfe_platform_cfg.h file.
**/
#define CFE_TIME_DELTA_CFG_EID   47

/** \brief <tt> '1Hz Adjust commands invalid without CFE_PLATFORM_TIME_CFG_SERVER set to true' </tt>
**  \event <tt> '1Hz Adjust commands invalid without CFE_PLATFORM_TIME_CFG_SERVER set to true' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated whenever Time Services receives
**  either a \link #CFE_TIME_ADD_1HZ_ADJUSTMENT_CC Add STCF Adjustment each second Command \endlink
**  OR a \link #CFE_TIME_SUB_1HZ_ADJUSTMENT_CC Subtract STCF Adjustment each second command \endlink
**  and the Time Services configuration parameter #CFE_PLATFORM_TIME_CFG_SERVER has
**  not been set to true in the cfe_platform_cfg.h file.
**/
#define CFE_TIME_1HZ_CFG_EID     48

/** \brief <tt> 'Invalid cmd length: ID = 0x\%X, CC = \%d, Exp Len = \%d, Len = \%d' </tt>
**  \event <tt> 'Invalid cmd length: ID = 0x\%X, CC = \%d, Exp Len = \%d, Len = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when a message with the #CFE_TIME_CMD_MID
**  message ID has arrived but whose packet length does not match the expected
**  length for the specified command code.
**
**  The \c ID field in the event message specifies the Message ID (in hex), the \c CC field
**  specifies the Command Code (in decimal), the \c Exp Len field specified the Expected
**  Length (in decimal ), and \c Len specifies the message Length (in decimal)
**  found in the message.
**/
#define CFE_TIME_LEN_ERR_EID       49

#endif  /* _cfe_time_events_ */
