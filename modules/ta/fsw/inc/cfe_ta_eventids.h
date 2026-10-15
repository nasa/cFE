/************************************************************************
 * NASA Docket No. GSC-19,200-1, and identified as "cFS Draco"
 *
 * Copyright (c) 2023 United States Government as represented by the
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
 *  cFE Task Affinity Event IDs
 */

#ifndef CFE_TA_EVENTS_H
#define CFE_TA_EVENTS_H

/**
 * \name TA event IDs
 * \{
 */

/**
 * \brief TA Initialization Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *  Task Affinity Task initialization complete.
 */
#define CFE_TA_INIT_INF_EID 1

/**
 * \brief TA Invalid Message Id Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *  Task Affinity Task received an unknown Message Id.
 */
#define CFE_TA_ERR_MSGID_EID 2

/**
 * \brief TA No-op Command Success Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *  \link #CFE_TA_NOOP_CC NO-OP TA No-op Command \endlink success.
 */
#define CFE_TA_NOOP_INF_EID 3

/**
 * \brief TA Reset Counters Command Success Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *  \link #CFE_TA_RESET_COUNTERS_CC TA Reset Counters Command \endlink success.
 */
#define CFE_TA_RESET_COUNTERS_INF_EID 4

/**
 * \brief TA Invalid Command Code Received Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *  Invalid command code for message ID received on the TA message pipe.
 */
#define CFE_TA_ERR_CC_EID 5

/**
 * \brief TA Invalid Command Length Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *  Invalid length for the command code in message ID received on the TA message pipe.
 */
#define CFE_TA_LEN_ERR_EID 6

/**
 * \brief TA Write All Task Affinity Data Command Filename Parse or File Create Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *  \link #CFE_TA_QUERY_ALL_TASKS_CC TA Write All Task Affinity Data Command \endlink failed
 *  to parse the filename or open/create the file.
 */
#define CFE_TA_TASKINFO_OSCREATE_ERR_EID 7

/**
 * \brief TA Write All Task Affinity Data Command Filename Parse or File Create Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *  \link #CFE_TA_QUERY_ALL_TASKS_CC TA Write All Task Affinity Data Command \endlink failed
 *  to parse the filename or open/create the file.
 */
#define CFE_TA_TASKINFO_FILENAME_ERR_EID 8

/**
 * \brief TA Write All Task Data Command Write Header Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *  \link #CFE_TA_QUERY_ALL_TASKS_CC TA Write All Task Affinity Data Command \endlink failed
 *  to write file header.
 */
#define CFE_TA_TASKINFO_WRHDR_ERR_EID 9

/**
 * \brief TA Write All Task Affinity Data Command Write Data Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *  \link #CFE_TA_QUERY_ALL_TASKS_CC TA Write All Task Affinity Data Command \endlink failed
 *  to write task data to file.
 */
#define CFE_TA_TASKINFO_WR_ERR_EID 10

/**
 * \brief TA Write All Task Data Command Success Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *  \link #CFE_TA_QUERY_ALL_TASKS_CC TA Write All Task Affinity Data Command \endlink success.
 */
#define CFE_TA_TASKINFO_EID 11

/**
 * \brief TA Set Task Affinity at Startup Success Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *  TA Set Startup Affinity success.
 */
#define CFE_TA_SET_STARTUP_AFFINITY_INF_EID 12

/**
 * \brief TA Set Task Affinity at Startup Added Success Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *  TA Set Startup Affinity success (Task Added).
 */
#define CFE_TA_SET_STARTUP_AFFINITY_ADDED_INF_EID 13

/**
 * \brief TA Set Task Affinity at Startup Failure Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *  TA Set Startup Affinity failure.
 */
#define CFE_TA_SET_STARTUP_AFFINITY_ERR_EID 14

/**
 * \brief TA Set Task Affinity by command task not found Failure Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *  \link #CFE_TA_SET_TASK_AFFINITY_CC TA Set Affinity Command \endlink failed.
 */
#define CFE_TA_SET_AFFINITY_CMD_NOTFOUND_ERR_EID 15

/**
 * \brief TA Set Task Affinity by command get affinity Failure Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *  \link #CFE_TA_SET_TASK_AFFINITY_CC TA Set Affinity Command \endlink failed.
 */
#define CFE_TA_SET_AFFINITY_CMD_GET_ERR_EID 16

/**
 * \brief TA Set Task Affinity by command set affinity Failure Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *  \link #CFE_TA_SET_TASK_AFFINITY_CC TA Set Affinity Command \endlink failed.
 */
#define CFE_TA_SET_AFFINITY_CMD_SET_ERR_EID 17

/**
 * \brief TA Set Task Affinity by command Success Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *  \link #CFE_TA_SET_TASK_AFFINITY_CC TA Set Affinity Command \endlink success.
 */
#define CFE_TA_SET_TASK_AFFINITY_CMD_INF_EID 18

/**
 * \brief TA Get Task Affinity by command task not found Failure Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *  \link #CFE_TA_GET_TASK_AFFINITY_CC TA Get Affinity Command \endlink failed.
 */
#define CFE_TA_GET_AFFINITY_CMD_NOTFOUND_ERR_EID 19

/**
 * \brief TA Get Task Affinity by command get affinity Failure Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *  \link #CFE_TA_GET_TASK_AFFINITY_CC TA Get Affinity Command \endlink failed.
 */
#define CFE_TA_GET_AFFINITY_CMD_GET_ERR_EID 20

/**
 * \brief TA Get Task Affinity by command Success Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *  \link #CFE_TA_GET_TASK_AFFINITY_CC TA Get Affinity Command \endlink success.
 */
#define CFE_TA_GET_TASK_AFFINITY_CMD_INF_EID 21

/**
 * \brief TA Get Task Affinity different value affinity between stored in TA and OS
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *  \link #CFE_TA_GET_TASK_AFFINITY_CC TA Get Affinity Command \endlink affinity change not recognized by TA.
 */
#define CFE_TA_GET_TASK_AFFINITY_DISCREPANCY_INF_EID 22

/**
 * \brief TA Core information Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *  Task Affinity Core information message
 */
#define CFE_TA_CORE_INF_EID 23

/** \} */

#endif /* CFE_TA_EVENTS_H */
