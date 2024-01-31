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
 *  cFE Executive Services Event IDs
 */

#ifndef CFE_ES_EVENTS_H
#define CFE_ES_EVENTS_H

/**
 * \name ES event IDs
 */
/**\{*/

/**
 * \brief ES Initialization Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  Executive Services Task initialization complete.
 */
#define CFE_ES_INIT_INF_EID 1

/**
 * \brief ES Initialization Statistics Information Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  Executive Services Task initialization complete.
 */
#define CFE_ES_INITSTATS_INF_EID 2

/**
 * \brief ES No-op Command Success Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_NOOP_CC ES No-op Command \endlink success.
 */
#define CFE_ES_NOOP_INF_EID 3

/**
 * \brief ES Reset Counters Command Success Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_RESET_COUNTERS_CC ES Reset Counters Command \endlink success.
 */
#define CFE_ES_RESET_INF_EID 4

/**
 * \brief ES Start Application Command Success Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_START_APP_CC ES Start Application Command \endlink success.
 */
#define CFE_ES_START_INF_EID 6

/**
 * \brief ES Stop Application Command Request Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_STOP_APP_CC ES Stop Application Command \endlink success.
 *  Note this event signifies the request to delete the application has been successfully
 *  submitted.  The successful completion will generate a #CFE_ES_STOP_INF_EID event.
 */
#define CFE_ES_STOP_DBG_EID 7

/**
 * \brief ES Stop Application Completed Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  Request to delete an application successfully completed.
 */
#define CFE_ES_STOP_INF_EID 8

/**
 * \brief ES Restart Application Command Request Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_RESTART_APP_CC ES Restart Application Command \endlink success.
 *  Note this event signifies the request to restart the application has been successfully
 *  submitted.  The successful completion will generate a #CFE_ES_RESTART_APP_INF_EID event.
 */
#define CFE_ES_RESTART_APP_DBG_EID 9

/**
 * \brief ES Restart Application Completed Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  Request to restart an application successfully completed.
 */
#define CFE_ES_RESTART_APP_INF_EID 10

/**
 * \brief ES Reload Application Command Request Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_RELOAD_APP_CC ES Reload Application Command \endlink success.
 *  Note this event signifies the request to reload the application has been successfully
 *  submitted.  The successful completion will generate a #CFE_ES_RELOAD_APP_INF_EID event.
 */
#define CFE_ES_RELOAD_APP_DBG_EID 11

/**
 * \brief ES Reload Application Complete Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  Request to reload an application successfully completed.
 */
#define CFE_ES_RELOAD_APP_INF_EID 12

/**
 * \brief ES Nominal Exit Application Complete Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  Nominal request to exit an application successfully completed.
 *  This event indicates the Application exited due to a nominal exit condition.
 */
#define CFE_ES_EXIT_APP_INF_EID 13

/**
 * \brief ES Error Exit Application Complete Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  Error request to exit an application successfully completed.
 *  This event indicates the Application exited due to an error condition. The details of the
 *  error that occurred should be given by the Application through an event message,
 *  System Log entry, or both.
 */
#define CFE_ES_ERREXIT_APP_INF_EID 14

/**
 * \brief ES Query One Application Command Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_QUERY_ONE_CC ES Query One Application Command \endlink success.
 */
#define CFE_ES_ONE_APP_EID 15

/**
 * \brief ES Query All Applications Command Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_QUERY_ALL_CC ES Query All Applications Command \endlink success.
 */
#define CFE_ES_ALL_APPS_EID 16

/**
 * \brief ES Clear System Log Command Success Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_CLEAR_SYS_LOG_CC ES Clear System Log Command \endlink success.
 */
#define CFE_ES_SYSLOG1_INF_EID 17

/**
 * \brief ES Write System Log Command Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_CLEAR_SYS_LOG_CC ES Write System Log Command \endlink success.
 */
#define CFE_ES_SYSLOG2_EID 18

/**
 * \brief ES Clear Exception Reset Log Command Success Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_CLEAR_ER_LOG_CC ES Clear Exception Reset Log Command \endlink success.
 */
#define CFE_ES_ERLOG1_INF_EID 19

/**
 * \brief ES Write Exception Reset Log Complete Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  Request to write the Exception Reset log successfully completed.
 */
#define CFE_ES_ERLOG2_EID 20

/**
 * \brief ES Invalid Message ID Received Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Invalid message ID received on the ES message pipe.
 */
#define CFE_ES_MID_ERR_EID 21

/**
 * \brief ES Invalid Command Code Received Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Invalid command code for message ID #CFE_ES_CMD_MID received on the ES message pipe.
 */
#define CFE_ES_CC1_ERR_EID 22

/**
 * \brief ES Invalid Command Length Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Invalid length for the command code in message ID #CFE_ES_CMD_MID received on the ES message pipe.
 */
#define CFE_ES_LEN_ERR_EID 23

/**
 * \brief ES Restart Command Invalid Restart Type Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_RESTART_CC ES cFE Restart Command \endlink failure due to invalid restart type.
 */
#define CFE_ES_BOOT_ERR_EID 24

/**
 * \brief ES Start Application Command Application Creation Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_START_APP_CC ES Start Application Command \endlink failure during
 *  application creation after successful parameter validation.
 */
#define CFE_ES_START_ERR_EID 26

/**
 * \brief ES Start Application Command Invalid Filename Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_START_APP_CC ES Start Application Command \endlink failure due to
 *  invalid filename.
 */
#define CFE_ES_START_INVALID_FILENAME_ERR_EID 27

/**
 * \brief ES Start Application Command Entry Point NULL Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_START_APP_CC ES Start Application Command \endlink failure due to
 *  a NULL Application Entry Point.
 */
#define CFE_ES_START_INVALID_ENTRY_POINT_ERR_EID 28

/**
 * \brief ES Start Application Command App Name NULL Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_START_APP_CC ES Start Application Command \endlink failure due to
 *  NULL Application Name.
 */
#define CFE_ES_START_NULL_APP_NAME_ERR_EID 29

/**
 * \brief ES Start Application Command Priority Too Large Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_START_APP_CC ES Start Application Command \endlink failure due to
 *  a requested application priority greater than the maximum priority allowed for tasks as
 *  defined by the OS Abstraction Layer (OS_MAX_PRIORITY).
 */
#define CFE_ES_START_PRIORITY_ERR_EID 31

/**
 * \brief ES Start Application Command Exception Action Invalid Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_START_APP_CC ES Start Application Command \endlink failure due to
 *  invalid application exception action.
 */
#define CFE_ES_START_EXC_ACTION_ERR_EID 32

/**
 * \brief ES Error Exit Application Cleanup Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Error request to exit an application failed during application cleanup.
 *  Application and related resources will be in undefined state.
 */
#define CFE_ES_ERREXIT_APP_ERR_EID 33

/**
 * \brief ES Stop Application Command Request Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_STOP_APP_CC ES Stop Application Command \endlink request failed.
 */
#define CFE_ES_STOP_ERR1_EID 35

/**
 * \brief ES Stop Application Command Get AppID By Name Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_STOP_APP_CC ES Stop Application Command \endlink failed to get application
 *  ID from application name.  The application will not be deleted.
 */
#define CFE_ES_STOP_ERR2_EID 36

/**
 * \brief ES Stop Application Cleanup Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Request to delete an application failed during application cleanup.
 *  Application and related resources will be in undefined state.
 */
#define CFE_ES_STOP_ERR3_EID 37

/**
 * \brief ES Restart Application Command Request Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_RESTART_APP_CC ES Restart Application Command \endlink request failed.
 */
#define CFE_ES_RESTART_APP_ERR1_EID 38

/**
 * \brief ES Restart Application Command Get AppID By Name Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_RESTART_APP_CC ES Restart Application Command \endlink failed to get application
 *  ID from application name.  The application will not be restarted.
 */
#define CFE_ES_RESTART_APP_ERR2_EID 39

/**
 * \brief ES Restart Application Startup Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Request to restart an application failed during application startup.  The application
 *  will not be restarted.
 */
#define CFE_ES_RESTART_APP_ERR3_EID 40

/**
 * \brief ES Restart Application Cleanup Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Request to restart an application failed during application cleanup.  The application
 *  will not be restarted and will be in an undefined state along with it's associated
 *  resources.
 */
#define CFE_ES_RESTART_APP_ERR4_EID 41

/**
 * \brief ES Reload Application Command Request Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_RELOAD_APP_CC ES Reload Application Command \endlink request failed.
 */
#define CFE_ES_RELOAD_APP_ERR1_EID 42

/**
 * \brief ES Reload Application Command Get AppID By Name Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_RELOAD_APP_CC ES Reload Application Command \endlink failed to get application
 *  ID from application name.  The application will not be reloaded.
 */
#define CFE_ES_RELOAD_APP_ERR2_EID 43

/**
 * \brief ES Reload Application Startup Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Request to reload an application failed during application startup.  The application
 *  will not be reloaded.
 */
#define CFE_ES_RELOAD_APP_ERR3_EID 44

/**
 * \brief ES Reload Application Cleanup Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Request to reload an application failed during application cleanup.  The application
 *  will not be reloaded and will be in an undefined state along with it's associated
 *  resources.
 */
#define CFE_ES_RELOAD_APP_ERR4_EID 45

/**
 * \brief ES Exit Application Cleanup Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Nominal request to exit an application failed during application cleanup.
 *  Application and related resources will be in undefined state.
 */
#define CFE_ES_EXIT_APP_ERR_EID 46

/**
 * \brief ES Process Control Invalid Exception State Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Invalid Exception state encountered when processing requests for application state changes.
 *  Exceptions are processed immediately, so this state should never occur during routine processing.
 */
#define CFE_ES_PCR_ERR1_EID 47

/**
 * \brief ES Process Control Unknown State Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Unknown state encountered when processing requests for application state changes.
 */
#define CFE_ES_PCR_ERR2_EID 48

/**
 * \brief ES Query One Application Data Command Transmit Message Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_QUERY_ONE_CC ES Query One Application Data Command \endlink failed during
 *  message transmission.
 */
#define CFE_ES_ONE_ERR_EID 49

/**
 * \brief ES Query One Application Data Command Get AppID By Name Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_QUERY_ONE_CC ES Query One Application Data Command \endlink failed to get application
 *  ID from application name. Message will not be sent.
 */
#define CFE_ES_ONE_APPID_ERR_EID 50

/**
 * \brief ES Query All Application Data Command File Creation Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_QUERY_ALL_CC ES Query All Application Data Command \endlink failed to create file.
 */
#define CFE_ES_OSCREATE_ERR_EID 51

/**
 * \brief ES Query All Application Data Command File Write Header Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_QUERY_ALL_CC ES Query All Application Data Command \endlink failed to write file header.
 */
#define CFE_ES_WRHDR_ERR_EID 52

/**
 * \brief ES Query All Application Data Command File Write App Data Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_QUERY_ALL_CC ES Query All Application Data Command \endlink failed to write file application data.
 */
#define CFE_ES_TASKWR_ERR_EID 53

/**
 * \brief ES Write System Log Command Filename Parse or File Creation Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_WRITE_SYS_LOG_CC ES Write System Log Command \endlink failed parsing file
 *  name or creating the file. OVERLOADED
 */
#define CFE_ES_SYSLOG2_ERR_EID 55

/**
 * \brief ES Write Exception Reset Log Command Request or File Creation Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_WRITE_ER_LOG_CC ES Write Exception Reset Log Command \endlink request failed or
 *  file creation failed. OVERLOADED
 */
#define CFE_ES_ERLOG2_ERR_EID 56

/**
 * \brief ES Start Performance Analyzer Data Collection Command Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_START_PERF_DATA_CC ES Start Performance Analyzer Data Collection Command \endlink success.
 */
#define CFE_ES_PERF_STARTCMD_EID 57

/**
 * \brief ES Start Performance Analyzer Data Collection Command Idle Check Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_START_PERF_DATA_CC ES Start Performance Analyzer Data Collection Command \endlink failed
 *  due to already being started.
 */
#define CFE_ES_PERF_STARTCMD_ERR_EID 58

/**
 * \brief ES Start Performance Analyzer Data Collection Command Invalid Trigger Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_START_PERF_DATA_CC ES Start Performance Analyzer Data Collection Command \endlink failed
 *  due to invalid trigger mode.
 */
#define CFE_ES_PERF_STARTCMD_TRIG_ERR_EID 59

/**
 * \brief ES Stop Performance Analyzer Data Collection Command Request Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_STOP_PERF_DATA_CC ES Stop Performance Analyzer Data Collection Command \endlink success.
 *  Note this event signifies the request to stop and write the performance data has been successfully
 *  submitted.  The successful completion will generate a #CFE_ES_PERF_DATAWRITTEN_EID event.
 */
#define CFE_ES_PERF_STOPCMD_EID 60

/**
 * \brief ES Stop Performance Analyzer Data Collection Command Request Idle Check Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_STOP_PERF_DATA_CC ES Stop Performance Analyzer Data Collection Command \endlink failed
 *  due to a write already in progress.
 */
#define CFE_ES_PERF_STOPCMD_ERR2_EID 62

/**
 * \brief ES Set Performance Analyzer Filter Mask Command Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_SET_PERF_FILTER_MASK_CC ES Set Performance Analyzer Filter Mask Command \endlink success.
 */
#define CFE_ES_PERF_FILTMSKCMD_EID 63

/**
 * \brief ES Set Performance Analyzer Filter Mask Command Invalid Index Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_SET_PERF_FILTER_MASK_CC ES Set Performance Analyzer Filter Mask Command \endlink failed
 *  filter index range check.
 */
#define CFE_ES_PERF_FILTMSKERR_EID 64

/**
 * \brief ES Set Performance Analyzer Trigger Mask Command Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_SET_PERF_TRIGGER_MASK_CC ES Set Performance Analyzer Trigger Mask Command \endlink success.
 */
#define CFE_ES_PERF_TRIGMSKCMD_EID 65

/**
 * \brief ES Set Performance Analyzer Trigger Mask Command Invalid Mask Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_SET_PERF_TRIGGER_MASK_CC ES Set Performance Analyzer Trigger Mask Command \endlink failed
 *  the mask range check.
 */
#define CFE_ES_PERF_TRIGMSKERR_EID 66

/**
 * \brief ES Stop Performance Analyzer Data Collection Command Filename Parse or File Create Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_STOP_PERF_DATA_CC ES Stop Performance Analyzer Data Collection Command \endlink failed
 *  either parsing the file name or during open/creation of the file. OVERLOADED
 */
#define CFE_ES_PERF_LOG_ERR_EID 67

/**
 * \brief Performance Log Write Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  Request to write the performance log successfully completed.
 */
#define CFE_ES_PERF_DATAWRITTEN_EID 68

/**
 * \brief ES Register CDS API Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  #CFE_ES_RegisterCDS API failure, see reported status code or system log for details.
 */
#define CFE_ES_CDS_REGISTER_ERR_EID 69

/**
 * \brief ES Set System Log Overwrite Mode Command Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_OVER_WRITE_SYS_LOG_CC ES Set System Log Overwrite Mode Command \endlink success.
 */
#define CFE_ES_SYSLOGMODE_EID 70

/**
 * \brief ES Set System Log Overwrite Mode Command Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_OVER_WRITE_SYS_LOG_CC ES Set System Log Overwrite Mode Command \endlink failed
 *  due to invalid mode requested.
 */
#define CFE_ES_ERR_SYSLOGMODE_EID 71

/**
 * \brief ES Set Processor Reset Counter to Zero Command Success Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_RESET_PR_COUNT_CC ES Set Processor Reset Counter to Zero Command \endlink success.
 */
#define CFE_ES_RESET_PR_COUNT_EID 72

/**
 * \brief ES Set Maximum Processor Reset Limit Command Success Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_RESET_PR_COUNT_CC ES Set Maximum Processor Reset Limit Command \endlink success.
 */
#define CFE_ES_SET_MAX_PR_COUNT_EID 73

/**
 * \brief ES File Write Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  ES File Write failure writing data to file. OVERLOADED
 */
#define CFE_ES_FILEWRITE_ERR_EID 74

/**
 * \brief ES Delete CDS Command Delete Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_DELETE_CDS_CC ES Delete CDS Command \endlink failed while deleting,
 *  see reported status code or system log for details.
 */
#define CFE_ES_CDS_DELETE_ERR_EID 76

/**
 * \brief ES Delete CDS Command Lookup CDS Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_DELETE_CDS_CC ES Delete CDS Command \endlink failed due to the
 *  specified CDS name not found in the CDS Registry.
 */
#define CFE_ES_CDS_NAME_ERR_EID 77

/**
 * \brief ES Delete CDS Command Success Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_DELETE_CDS_CC ES Delete CDS Command \endlink success.
 */
#define CFE_ES_CDS_DELETED_INFO_EID 78

/**
 * \brief ES Delete CDS Command For Critical Table Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_DELETE_CDS_CC Delete CDS Command \endlink failure due to the
 *  specified CDS name being a critical table.
 *  Critical Table images can only be deleted via a Table Services
 *  command, #CFE_TBL_DELETE_CDS_CC.
 */
#define CFE_ES_CDS_DELETE_TBL_ERR_EID 79

/**
 * \brief ES Delete CDS Command With Active Owner Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_DELETE_CDS_CC ES Delete CDS Command \endlink failure due to the
 *  specifies CDS name is registered to an active application.
 */
#define CFE_ES_CDS_OWNER_ACTIVE_EID 80

/**
 * \brief ES Telemeter Memory Statistics Command Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_SEND_MEM_POOL_STATS_CC ES Telemeter Memory Statistics Command \endlink success.
 */
#define CFE_ES_TLM_POOL_STATS_INFO_EID 81

/**
 * \brief ES Telemeter Memory Statistics Command Invalid Handle Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_SEND_MEM_POOL_STATS_CC ES Telemeter Memory Statistics Command \endlink failure
 *  due to an invalid memory handle.
 */
#define CFE_ES_INVALID_POOL_HANDLE_ERR_EID 82

/**
 * \brief ES Write Critical Data Store Registry Command Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_DUMP_CDS_REGISTRY_CC ES Write Critical Data Store Registry Command \endlink success.
 */
#define CFE_ES_CDS_REG_DUMP_INF_EID 83

/**
 * \brief ES Write Critical Data Store Registry Command Record Write Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_DUMP_CDS_REGISTRY_CC ES Write Critical Data Store Registry Command \endlink failed
 *  to write CDS record.
 */
#define CFE_ES_CDS_DUMP_ERR_EID 84

/**
 * \brief ES Write Critical Data Store Registry Command Header Write Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_DUMP_CDS_REGISTRY_CC ES Write Critical Data Store Registry Command \endlink failed
 *  to write header.
 */
#define CFE_ES_WRITE_CFE_HDR_ERR_EID 85

/**
 * \brief ES Write Critical Data Store Registry Command Filename Parse or File Create Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_DUMP_CDS_REGISTRY_CC ES Write Critical Data Store Registry Command \endlink failed
 *  to parse filename or open/create the file. OVERLOADED
 */
#define CFE_ES_CREATING_CDS_DUMP_ERR_EID 86

/**
 * \brief ES Write All Task Data Command Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_QUERY_ALL_TASKS_CC ES Write All Task Data Command \endlink success.
 */
#define CFE_ES_TASKINFO_EID 87

/**
 * \brief ES Write All Task Data Command Filename Parse or File Create Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_QUERY_ALL_TASKS_CC ES Write All Task Data Command \endlink failed
 *  to parse the filename or open/create the file.
 */
#define CFE_ES_TASKINFO_OSCREATE_ERR_EID 88

/**
 * \brief ES Write All Task Data Command Write Header Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_QUERY_ALL_TASKS_CC ES Write All Task Data Command \endlink failed
 *  to write file header.
 */
#define CFE_ES_TASKINFO_WRHDR_ERR_EID 89

/**
 * \brief ES Write All Task Data Command Write Data Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_QUERY_ALL_TASKS_CC ES Write All Task Data Command \endlink failed
 *  to write task data to file.
 */
#define CFE_ES_TASKINFO_WR_ERR_EID 90

/**
 * \brief cFS Version Information Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  ES Initialization complete and response to \link #CFE_ES_NOOP_CC ES NO-OP Command \endlink.
 *
 *  A separate version info event will be generated for every module which is statically
 *  linked into the CFE core executable (e.g. OSAL, PSP, MSG, SBR, etc).
 *
 *  The version information reported in this event is derived from the source revision
 *  control system at build time, as opposed to manually-assigned semantic version numbers.
 *  It is intended to uniquely identify the actual source code that is currently running,
 *  to the extent this is possible.
 *
 *  The \c Mission version information also identifies the build configuration name, if available.
 */
#define CFE_ES_VERSION_INF_EID 91

/**
 * \brief cFS Build Information Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  ES Initialization complete and response to \link #CFE_ES_NOOP_CC ES NO-OP Command \endlink.
 *
 *  The \c Build field identifies the build date, time, hostname and user identifier of
 *  the build host machine for the current running binary.  The first string is the
 *  build date/time, and the second string is formatted as "user@hostname"
 *
 *  This additionally reports the configuration name that was selected by the user,
 *  which may affect various platform/mission limits.
 *
 *  By default, if not specified/overridden, the default values of these variables will be:
 *    BUILDDATE ==> the output of "date +%Y%m%d%H%M"
 *    HOSTNAME ==> the output of "hostname"
 *    USER ==> the output of "whoami"
 *
 *  The values can be overridden by setting an environment variable with the names above
 *  to the value desired for the field when running "make".
 */
#define CFE_ES_BUILD_INF_EID 92

/**
 * \brief ES Write Exception Reset Log Command Already In Progress Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_ES_WRITE_ER_LOG_CC ES Write Exception Reset Log Command \endlink failure due to
 *  a write already being in progress.
 */
#define CFE_ES_ERLOG_PENDING_ERR_EID 93
/**\}*/

#endif /* CFE_ES_EVENTS_H */
