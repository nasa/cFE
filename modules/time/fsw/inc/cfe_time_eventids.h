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
 *  cFE Time Services Event IDs
 */

#ifndef CFE_TIME_EVENTS_H
#define CFE_TIME_EVENTS_H

/**
 * \name TIME event IDs
 */
/**\{*/

/**
 * \brief TIME Initialization Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  Time Services Task Initialization complete.
 */
#define CFE_TIME_INIT_EID 1

/**
 * \brief TIME No-op Command Success Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  \link #CFE_TIME_NOOP_CC TIME NO-OP Command \endlink success.
 */
#define CFE_TIME_NOOP_EID 4

/**
 * \brief TIME Reset Counters Command Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  \link #CFE_TIME_RESET_COUNTERS_CC TIME Reset Counters Command \endlink success.
 */
#define CFE_TIME_RESET_EID 5

/**
 * \brief TIME Request Diagnostics Command Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  \link #CFE_TIME_SEND_DIAGNOSTIC_CC TIME Request Diagnostics Command \endlink success.
 */
#define CFE_TIME_DIAG_EID 6

/**
 * \brief TIME Set Time State Command Success Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  \link #CFE_TIME_SET_STATE_CC TIME Set Time State Command \endlink success.
 */
#define CFE_TIME_STATE_EID 7

/**
 * \brief TIME Set Time Source Command Success Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  \link #CFE_TIME_SET_SOURCE_CC TIME Set Time Source Command \endlink success.
 */
#define CFE_TIME_SOURCE_EID 8

/**
 * \brief TIME Set Tone Source Command Success Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  \link #CFE_TIME_SET_SIGNAL_CC TIME Set Clock Tone Source Command \endlink success.
 */
#define CFE_TIME_SIGNAL_EID 9

/**
 * \brief TIME Add or Subtract Delay Command Success Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  \link #CFE_TIME_ADD_DELAY_CC TIME Add Time Delay Command \endlink OR a
 *  \link #CFE_TIME_SUB_DELAY_CC Subtract Time Delay Command \endlink success.
 */
#define CFE_TIME_DELAY_EID 11

/**
 * \brief TIME Set Time Command Success Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  \link #CFE_TIME_SET_TIME_CC TIME Set Time Command \endlink success.
 */
#define CFE_TIME_TIME_EID 12

/**
 * \brief TIME Set Mission Elapsed Time Command Success Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  \link #CFE_TIME_SET_MET_CC TIME Set Mission Elapsed Time Command \endlink success.
 */
#define CFE_TIME_MET_EID 13

/**
 * \brief TIME Set Spacecraft Time Correlation Factor Command Success Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  \link #CFE_TIME_SET_STCF_CC TIME Set Spacecraft Time Correlation Factor Command \endlink success.
 */
#define CFE_TIME_STCF_EID 14

/**
 * \brief TIME Add or Subtract Single STCF Adjustment Command Success Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  \link #CFE_TIME_ADD_ADJUST_CC TIME Add Single STCF Adjustment Command \endlink OR
 *  \link #CFE_TIME_SUB_ADJUST_CC TIME Subtract Single STCF Adjustment Command \endlink success.
 */
#define CFE_TIME_DELTA_EID 15

/**
 * \brief TIME Add or Subtract STCF Adjustment Each Second Command Success Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  \link #CFE_TIME_ADD_ONE_HZ_ADJUSTMENT_CC TIME Add STCF Adjustment Each Second Command \endlink OR
 *  \link #CFE_TIME_SUB_ONE_HZ_ADJUSTMENT_CC TIME Subtract STCF Adjustment Each Second Command \endlink success.
 */
#define CFE_TIME_ONEHZ_EID 16

/**
 * \brief TIME Set Leap Seconds Command Success Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  \link #CFE_TIME_SET_LEAP_SECONDS_CC TIME Set Leap Seconds Command \endlink success.
 */
#define CFE_TIME_LEAPS_EID 17

/**
 * \brief TIME Entered FLYWHEEL Mode Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  TIME Entered FLYWHEEL Mode.
 */
#define CFE_TIME_FLY_ON_EID 20

/**
 * \brief TIME Exited FLYWHEEL Mode Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  TIME Exited FLYWHEEL Mode.
 */
#define CFE_TIME_FLY_OFF_EID 21

/**
 * \brief TIME Invalid Message ID Received Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Invalid message ID received on the TIME message pipe.
 */
#define CFE_TIME_ID_ERR_EID 26

/**
 * \brief TIME Invalid Command Code Received Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Invalid command code for message ID #CFE_TIME_CMD_MID received on the TIME message pipe.
 */
#define CFE_TIME_CC_ERR_EID 27

/**
 * \brief TIME Set Clock State Command Invalid State Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_TIME_SET_STATE_CC TIME Set Clock State Command \endlink failed due to invalid state requested.
 */
#define CFE_TIME_STATE_ERR_EID 30

/**
 * \brief TIME Set Clock Source Command Invalid Source Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_TIME_SET_SOURCE_CC TIME Set Clock Source Command \endlink failed due to invalid source requested.
 */
#define CFE_TIME_SOURCE_ERR_EID 31

/**
 * \brief TIME Set Clock Tone Source Command Invalid Source Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_TIME_SET_SIGNAL_CC Set Clock Tone Source Command \endlink failed due to invalid source requested.
 */
#define CFE_TIME_SIGNAL_ERR_EID 32

/**
 * \brief TIME Add or Subtract Tone Delay Command Invalid Time Value Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_TIME_ADD_DELAY_CC TIME Add Tone Delay Command \endlink OR
 *  \link #CFE_TIME_SUB_DELAY_CC TIME Subtract Tone Delay Command \endlink failure
 *  due to an invalid time value.
 */
#define CFE_TIME_DELAY_ERR_EID 33

/**
 * \brief TIME Set Spacecraft Time Command Invalid Time Value Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_TIME_SET_TIME_CC TIME Set Spacecraft Time Command \endlink failure
 *  due to an invalid time value.
 */
#define CFE_TIME_TIME_ERR_EID 34

/**
 * \brief TIME Set Mission Elapsed Time Command Invalid Time Value Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_TIME_SET_MET_CC TIME Set Mission Elapsed Time Command \endlink failure
 *  due to an invalid time value.
 */
#define CFE_TIME_MET_ERR_EID 35

/**
 * \brief TIME Set Spacecraft Time Correlation Factor Command Invalid Time Value Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_TIME_SET_STCF_CC TIME Set Spacecraft Time Correlation Factor Command \endlink
 *  failure due to an invalid time value.
 */
#define CFE_TIME_STCF_ERR_EID 36

/**
 * \brief TIME Add or Subtract Single STCF Adjustment Command Invalid Time Value Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_TIME_ADD_ADJUST_CC TIME Add Single STCF Adjustment Command \endlink
 *  OR \link #CFE_TIME_SUB_ADJUST_CC TIME Subtract Single STCF Adjustment Command \endlink
 *  failure due to an invalid time value.
 */
#define CFE_TIME_DELTA_ERR_EID 37

/**
 * \brief TIME Set Clock Source Command Incompatible Mode Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_TIME_SET_SOURCE_CC TIME Set Clock Source Command \endlink failure
 *  due to being in an incompatible mode.
 */
#define CFE_TIME_SOURCE_CFG_EID 40

/**
 * \brief TIME Set Clock Signal Command Incompatible Mode Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_TIME_SET_SIGNAL_CC TIME Set Clock Signal Command \endlink failure
 *  due to being in an incompatible mode.
 */
#define CFE_TIME_SIGNAL_CFG_EID 41

/**
 * \brief TIME Add or Subtract Tone Delay Command Incompatible Mode Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_TIME_ADD_DELAY_CC TIME Add Tone Delay Command \endlink OR
 *  \link #CFE_TIME_SUB_DELAY_CC TIME Subtract Tone Delay Command \endlink failure
 *  due to being in an incompatible mode.
 */
#define CFE_TIME_DELAY_CFG_EID 42

/**
 * \brief TIME Set Spacecraft Time Command Incompatible Mode Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_TIME_SET_TIME_CC TIME Set Spacecraft Time Command \endlink failure
 *  due to being in an incompatible mode.
 */
#define CFE_TIME_TIME_CFG_EID 43

/**
 * \brief TIME Set Mission Elapsed Time Command Incompatible Mode Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_TIME_SET_MET_CC TIME Set Mission Elapsed Time Command \endlink failure
 *  due to being in an incompatible mode.
 */
#define CFE_TIME_MET_CFG_EID 44

/**
 * \brief TIME Set Spacecraft Time Correlation Factor Command Incompatible Mode Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_TIME_SET_STCF_CC TIME Set Spacecraft Time Correlation Factor Command \endlink failure
 *  due to being in an incompatible mode.
 */
#define CFE_TIME_STCF_CFG_EID 45

/**
 * \brief TIME Set Leap Seconds Command Incompatible Mode Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_TIME_SET_LEAP_SECONDS_CC TIME Set Leap Seconds Command \endlink failure
 *  due to being in an incompatible mode.
 */
#define CFE_TIME_LEAPS_CFG_EID 46

/**
 * \brief TIME Add or Subtract Single STCF Adjustment Command Incompatible Mode Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_TIME_ADD_ADJUST_CC TIME Add Single STCF Adjustment Command \endlink OR
 *  \link #CFE_TIME_SUB_ADJUST_CC TIME Subtract Single STCF Adjustment Command \endlink failure
 *  due to being in an incompatible mode.
 */
#define CFE_TIME_DELTA_CFG_EID 47

/**
 * \brief TIME Add or Subtract STCF Adjustment Each Second Command Incompatible Mode Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_TIME_ADD_ONE_HZ_ADJUSTMENT_CC TIME Add STCF Adjustment Each Second Command \endlink OR
 *  \link #CFE_TIME_SUB_ONE_HZ_ADJUSTMENT_CC TIME Subtract STCF Adjustment Each Second Command \endlink
 *  failure due to being in an incompatible mode.
 */
#define CFE_TIME_ONEHZ_CFG_EID 48

/**
 * \brief TIME Invalid Command Length Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Invalid length for the command code in message ID #CFE_TIME_CMD_MID received on the TIME
 *  message pipe.
 */
#define CFE_TIME_LEN_ERR_EID 49
/**\}*/

#endif /* CFE_TIME_EVENTS_H */
