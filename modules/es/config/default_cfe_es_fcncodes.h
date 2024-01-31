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
 *   Specification for the CFE Executive Services (CFE_ES) command function codes
 *
 * @note
 *   This file should be strictly limited to the command/function code (CC)
 *   macro definitions.  Other definitions such as enums, typedefs, or other
 *   macros should be placed in the msgdefs.h or msg.h files.
 */
#ifndef CFE_ES_FCNCODES_H
#define CFE_ES_FCNCODES_H

/************************************************************************
 * Macro Definitions
 ************************************************************************/

/** \name Executive Services Command Codes */
/** \{ */

/** \cfeescmd Executive Services No-Op
**
**  \par Description
**       This command performs no other function than to increment the
**       command execution counter. The command may be used to verify
**       general aliveness of the Executive Services task.
**
**  \cfecmdmnemonic \ES_NOOP
**
**  \par Command Structure
**       #CFE_ES_NoopCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the
**       following telemetry:
**       - \b \c \ES_CMDPC - command execution counter will
**         increment
**       - The #CFE_ES_BUILD_INF_EID informational event message will
**         be generated
**       - The #CFE_ES_NOOP_INF_EID informational event message will
**         be generated
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The command packet length is incorrect
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \ES_CMDEC - command error counter will increment
**       - the #CFE_ES_LEN_ERR_EID error event message will be generated
**
**  \par Criticality
**       None
**
**  \sa
*/
#define CFE_ES_NOOP_CC 0

/** \cfeescmd Executive Services Reset Counters
**
**  \par Description
**       This command resets the following counters within the Executive
**       Services housekeeping telemetry:
**       - Command Execution Counter
**       - Command Error Counter
**
**  \cfecmdmnemonic \ES_RESETCTRS
**
**  \par Command Structure
**       #CFE_ES_ResetCountersCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \ES_CMDPC - command execution counter and error counter will
**         be reset to zero
**       - The #CFE_ES_RESET_INF_EID informational event message will be
**         generated
**
**  \par Error Conditions
**       There are no error conditions for this command. If the Executive
**       Services receives the command, the event is sent (although it
**       may be filtered by EVS) and the counter is incremented
**       unconditionally.
**
**  \par Criticality
**       This command is not inherently dangerous.  However, it is
**       possible for ground systems and on-board safing procedures
**       to be designed such that they react to changes in the counter
**       values that are reset by this command.
**
**  \sa  #CFE_ES_RESET_PR_COUNT_CC
*/
#define CFE_ES_RESET_COUNTERS_CC 1

/** \cfeescmd Executive Services Processor / Power-On Reset
**
**  \par Description
**       This command restarts the cFE in one of two modes.  The Power-On Reset
**       will cause the cFE to restart as though the power were first applied
**       to the processor.  The Processor Reset will attempt to retain the contents
**       of the volatile disk and the contents of the Critical Data Store.
**       NOTE: If a requested Processor Reset should cause the
**       Processor Reset Counter (\b \c \ES_PROCRESETCNT)
**       to exceed OR EQUAL the limit #CFE_PLATFORM_ES_MAX_PROCESSOR_RESETS (which is reported in
**       housekeeping telemetry as \b \c \ES_MAXPROCRESETS),
**       the command is \b AUTOMATICALLY upgraded to a Power-On Reset.
**
**  \cfecmdmnemonic \ES_RESET
**
**  \par Command Structure
**       #CFE_ES_RestartCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the
**       following telemetry:
**       - \b \c \ES_PROCRESETCNT - processor reset counter will increment (processor
**               reset) or reset to zero (power-on reset)
**       - \b \c \ES_RESETTYPE - processor reset type will be updated
**       - \b \c \ES_RESETSUBTYPE - processor reset subtype will be updated
**       - New entries in the Exception Reset Log and System Log can be found<BR>
**       NOTE: Verification of a Power-On Reset is shown through the loss of
**       data nominally retained through a Processor Reset<BR>
**       NOTE: Since the reset of the processor resets the command execution
**       counter (\b \c \ES_CMDPC), this counter \b CANNOT be used to verify
**       command execution.
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The \link #CFE_ES_RestartCmd_Payload_t Restart Type \endlink was
**         not a recognized value.
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \ES_CMDEC - command error counter will increment
**       - the #CFE_ES_BOOT_ERR_EID error event message will be generated
**
**  \par Criticality
**       This command is, by definition, dangerous.  Significant loss of
**       data will occur.  All processes and the cFE itself will be stopped
**       and restarted.  With the Power-On reset option, all data on the
**       volatile disk and the contents of the Critical Data Store will be
**       lost.
**
**  \sa  #CFE_ES_RESET_PR_COUNT_CC, #CFE_ES_SET_MAX_PR_COUNT_CC
*/
#define CFE_ES_RESTART_CC 2

/** \cfeescmd Load and Start an Application
**
**  \par Description
**       This command starts the specified application with the
**       specified start address, stack size, etc options.
**
**  \cfecmdmnemonic \ES_STARTAPP
**
**  \par Command Structure
**       #CFE_ES_StartAppCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \ES_CMDPC - command execution counter will
**         increment
**       - The #CFE_ES_START_INF_EID informational event message will be
**         generated
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The specified application filename string cannot be parsed
**       - The specified application entry point is an empty string
**       - The specified application name is an empty string
**       - The specified priority is greater than 255
**       - The specified exception action is neither #CFE_ES_ExceptionAction_RESTART_APP (0) or
**         #CFE_ES_ExceptionAction_PROC_RESTART (1)
**       - The Operating System was unable to load the specified application file
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \ES_CMDEC - command error counter will increment
**       - A command specific error event message is issued for all error
**         cases
**
**  \par Criticality
**       This command is not inherently dangerous although system resources
**       could be taxed beyond their limits with the starting of erroneous
**       or invalid applications.
**
**  \sa #CFE_ES_STOP_APP_CC, #CFE_ES_RESTART_APP_CC, #CFE_ES_RELOAD_APP_CC
*/
#define CFE_ES_START_APP_CC 4

/** \cfeescmd Stop and Unload Application
**
**  \par Description
**       This command halts and removes the specified Application
**       from the system. \b NOTE: This command should never be used
**       on the Command Ingest application.  This would prevent further
**       commands from entering the system.  If Command Ingest needs to
**       be stopped and restarted, use #CFE_ES_RESTART_APP_CC or
**       #CFE_ES_RELOAD_APP_CC.
**
**  \cfecmdmnemonic \ES_STOPAPP
**
**  \par Command Structure
**       #CFE_ES_StopAppCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \ES_CMDPC - command execution counter will
**         increment
**       - The #CFE_ES_STOP_DBG_EID debug event message will be
**         generated. NOTE: This event message only identifies that the
**         stop request has been initiated, not that it has completed.
**       - Once the stop has successfully completed, the list of Applications
**         and Tasks created in response to the \b \c \ES_WRITEAPPINFO2FILE,
**         \b \c \ES_WRITETASKINFO2FILE should no longer contain the
**         specified application.
**       - \b \c \ES_REGTASKS - number of tasks will decrease after tasks
**         associated with app (main task and any child tasks) are stopped
**       - \b \c \ES_REGEXTAPPS - external application counter will decrement
**         after app is cleaned up
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The specified application name is not recognized as an active application
**       - The specified application is one of the cFE's Core applications (ES, EVS, SB, TBL, TIME)
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \ES_CMDEC - command error counter will increment
**       - A command specific error event message is issued for all error
**         cases
**       - Additional information on the reason for command failure may be found
**         in the System Log
**
**  \par Criticality
**       This command is not inherently dangerous, however the removal of certain
**       applications (e.g. - Spacecraft Attitude and Control) may have a detrimental effect
**       on the spacecraft.
**
**  \sa #CFE_ES_START_APP_CC, #CFE_ES_RESTART_APP_CC, #CFE_ES_RELOAD_APP_CC
*/
#define CFE_ES_STOP_APP_CC 5

/** \cfeescmd Stops, Unloads, Loads using the previous File name, and Restarts an Application
**
**  \par Description
**       This command halts and removes the specified Application
**       from the system.  Then it immediately loads the Application from
**       the same filename last used to start.  This command is
**       especially useful for restarting a Command Ingest Application
**       since once it has been stopped, no further commands can come in
**       to restart it.
**
**  \cfecmdmnemonic \ES_RESTARTAPP
**
**  \par Command Structure
**       #CFE_ES_RestartAppCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \ES_CMDPC - command execution counter will
**         increment
**       - The #CFE_ES_RESTART_APP_DBG_EID debug event message will be
**         generated. NOTE: This event message only identifies that the
**         restart process has been initiated, not that it has completed.
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The original file is missing
**       - The specified application name is not recognized as an active application
**       - The specified application is one of the cFE's Core applications (ES, EVS, SB, TBL, TIME)
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \ES_CMDEC - command error counter will increment
**       - A command specific error event message is issued for all error
**         cases
**       - Additional information on the reason for command failure may be found
**         in the System Log
**
**  \par Criticality
**       This command is not inherently dangerous, however the restarting of certain
**       applications (e.g. - Spacecraft Attitude and Control) may have a detrimental effect
**       on the spacecraft.
**
**  \sa #CFE_ES_START_APP_CC, #CFE_ES_STOP_APP_CC, #CFE_ES_RELOAD_APP_CC
*/
#define CFE_ES_RESTART_APP_CC 6

/** \cfeescmd Stops, Unloads, Loads from the command specified File and Restarts an Application
**
**  \par Description
**       This command halts and removes the specified Application
**       from the system.  Then it immediately loads the Application from
**       the command specified file and restarts it.  This command is
**       especially useful for restarting a Command Ingest Application
**       since once it has been stopped, no further commands can come in
**       to restart it.
**
**  \cfecmdmnemonic \ES_RELOADAPP
**
**  \par Command Structure
**       #CFE_ES_ReloadAppCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \ES_CMDPC - command execution counter will
**         increment
**       - The #CFE_ES_RELOAD_APP_DBG_EID debug event message will be
**         generated. NOTE: This event message only identifies that the
**         reload process has been initiated, not that it has completed.
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The specified application filename string cannot be parsed
**       - The specified application name is not recognized as an active application
**       - The specified application is one of the cFE's Core applications (ES, EVS, SB, TBL, TIME)
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \ES_CMDEC - command error counter will increment
**       - A command specific error event message is issued for all error
**         cases
**       - Additional information on the reason for command failure may be found
**         in the System Log
**
**  \par Criticality
**       This command is not inherently dangerous, however the restarting of certain
**       applications (e.g. - Spacecraft Attitude and Control) may have a detrimental effect
**       on the spacecraft.
**
**  \sa #CFE_ES_START_APP_CC, #CFE_ES_STOP_APP_CC, #CFE_ES_RESTART_APP_CC
*/
#define CFE_ES_RELOAD_APP_CC 7

/** \cfeescmd Request Executive Services Information on a specified module
**
**  \par Description
**       This command takes the information kept by Executive Services on the
**       specified application or library and telemeters it to the ground.
**
**  \cfecmdmnemonic \ES_QUERYAPP
**
**  \par Command Structure
**       #CFE_ES_QueryOneCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \ES_CMDPC - command execution counter will
**         increment
**       - The #CFE_ES_ONE_APP_EID debug event message will be
**         generated.
**       - Receipt of the #CFE_ES_OneAppTlm_t telemetry packet
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The specified name is not recognized as an active application or library
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \ES_CMDEC - command error counter will increment
**       - A command specific error event message is issued for all error
**         cases
**
**  \par Criticality
**       None
**
**  \sa #CFE_ES_QUERY_ALL_CC, #CFE_ES_QUERY_ALL_TASKS_CC
*/
#define CFE_ES_QUERY_ONE_CC 8

/** \cfeescmd Writes all Executive Services Information on all loaded modules to a File
**
**  \par Description
**       This command takes the information kept by Executive Services on all of the
**       registered applications and libraries and writes it to the specified file.
**
**  \cfecmdmnemonic \ES_WRITEAPPINFO2FILE
**
**  \par Command Structure
**       #CFE_ES_QueryAllCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \ES_CMDPC - command execution counter will
**         increment
**       - The #CFE_ES_ALL_APPS_EID debug event message will be
**         generated.
**       - The file specified in the command (or the default specified
**         by the #CFE_PLATFORM_ES_DEFAULT_APP_LOG_FILE configuration parameter) will be
**         updated with the latest information.
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The specified FileName cannot be parsed
**       - An Error occurs while trying to write to the file
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \ES_CMDEC - command error counter will increment
**       - A command specific error event message is issued for all error
**         cases
**
**  \par Criticality
**       This command is not inherently dangerous.  It will create a new
**       file in the file system (or overwrite an existing one) and could,
**       if performed repeatedly without sufficient file management by the
**       operator, fill the file system.
**
**  \sa #CFE_ES_QUERY_ONE_CC, #CFE_ES_QUERY_ALL_TASKS_CC
*/
#define CFE_ES_QUERY_ALL_CC 9

/** \cfeescmd Clear Executive Services System Log
**
**  \par Description
**       This command clears the contents of the Executive Services System Log.
**
**  \cfecmdmnemonic \ES_CLEARSYSLOG
**
**  \par Command Structure
**       #CFE_ES_ClearSysLogCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \ES_CMDPC - command execution counter will
**         increment
**       - The #CFE_ES_SYSLOG1_INF_EID informational event message will be
**         generated.
**       - \b \c \ES_SYSLOGBYTEUSED - System Log Bytes Used will go to zero
**       - \b \c \ES_SYSLOGENTRIES - Number of System Log Entries will go to zero
**
**  \par Error Conditions
**       There are no error conditions for this command. If the Executive
**       Services receives the command, the event is sent (although it
**       may be filtered by EVS) and the counter is incremented
**       unconditionally.
**
**  \par Criticality
**       This command is not dangerous.  However, any previously logged data
**       will be lost.
**
**  \sa #CFE_ES_WRITE_SYS_LOG_CC, #CFE_ES_CLEAR_ER_LOG_CC, #CFE_ES_WRITE_ER_LOG_CC,
**      #CFE_ES_OVER_WRITE_SYS_LOG_CC
*/
#define CFE_ES_CLEAR_SYS_LOG_CC 10

/** \cfeescmd Writes contents of Executive Services System Log to a File
**
**  \par Description
**       This command causes the contents of the Executive Services System Log
**       to be written to a log file.
**
**  \cfecmdmnemonic \ES_WRITESYSLOG2FILE
**
**  \par Command Structure
**       #CFE_ES_WriteSysLogCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \ES_CMDPC - command execution counter will
**         increment
**       - The #CFE_ES_SYSLOG2_EID debug event message will be
**         generated.
**       - The file specified in the command (or the default specified
**         by the #CFE_PLATFORM_ES_DEFAULT_SYSLOG_FILE configuration parameter) will be
**         updated with the latest information.
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The specified FileName cannot be parsed
**       - An Error occurs while trying to write to the file
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \ES_CMDEC - command error counter will increment
**       - A command specific error event message is issued for all error
**         cases
**
**  \par Criticality
**       This command is not inherently dangerous.  It will create a new
**       file in the file system (or overwrite an existing one) and could,
**       if performed repeatedly without sufficient file management by the
**       operator, fill the file system.
**
**  \sa #CFE_ES_CLEAR_SYS_LOG_CC, #CFE_ES_CLEAR_ER_LOG_CC, #CFE_ES_WRITE_ER_LOG_CC,
**      #CFE_ES_OVER_WRITE_SYS_LOG_CC
*/
#define CFE_ES_WRITE_SYS_LOG_CC 11

/** \cfeescmd Clears the contents of the Exception and Reset Log
**
**  \par Description
**       This command causes the contents of the Executive Services Exception
**       and Reset Log to be cleared.
**
**  \cfecmdmnemonic \ES_CLEARERLOG
**
**  \par Command Structure
**       #CFE_ES_ClearERLogCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \ES_CMDPC - command execution counter will
**         increment
**       - The #CFE_ES_ERLOG1_INF_EID informational event message will be
**         generated.
**       - \b \c \ES_ERLOGINDEX - Index into Exception Reset Log goes to zero
**
**  \par Error Conditions
**       There are no error conditions for this command. If the Executive
**       Services receives the command, the event is sent (although it
**       may be filtered by EVS) and the counter is incremented
**       unconditionally.
**
**  \par Criticality
**       This command is not dangerous.  However, any previously logged data
**       will be lost.
**
**  \sa #CFE_ES_CLEAR_SYS_LOG_CC, #CFE_ES_WRITE_SYS_LOG_CC, #CFE_ES_WRITE_ER_LOG_CC
*/
#define CFE_ES_CLEAR_ER_LOG_CC 12

/** \cfeescmd Writes Exception and Reset Log to a File
**
**  \par Description
**       This command causes the contents of the Executive Services Exception
**       and Reset Log to be written to the specified file.
**
**  \cfecmdmnemonic \ES_WRITEERLOG2FILE
**
**  \par Command Structure
**       #CFE_ES_WriteERLogCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \ES_CMDPC - command execution counter will
**         increment
**       - The #CFE_ES_ERLOG2_EID debug event message will be
**         generated.
**       - The file specified in the command (or the default specified
**         by the #CFE_PLATFORM_ES_DEFAULT_ER_LOG_FILE configuration parameter) will be
**         updated with the latest information.
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - A previous request to write the ER log has not yet completed
**       - The specified FileName cannot be parsed
**       - An Error occurs while trying to write to the file
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \ES_CMDEC - command error counter will increment
**       - A command specific error event message is issued for all error
**         cases
**
**  \par Criticality
**       This command is not inherently dangerous.  It will create a new
**       file in the file system (or overwrite an existing one) and could,
**       if performed repeatedly without sufficient file management by the
**       operator, fill the file system.
**
**  \sa #CFE_ES_CLEAR_SYS_LOG_CC, #CFE_ES_WRITE_SYS_LOG_CC, #CFE_ES_CLEAR_ER_LOG_CC
*/
#define CFE_ES_WRITE_ER_LOG_CC 13

/** \cfeescmd Start Performance Analyzer
**
**  \par Description
**       This command causes the Performance Analyzer to begin collecting data using the specified trigger mode.
**
**  \cfecmdmnemonic \ES_STARTLADATA
**
**  \par Command Structure
**       #CFE_ES_StartPerfDataCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \ES_CMDPC - command execution counter will
**         increment
**       - \b \c \ES_PERFSTATE - Current performance analyzer state will change to
**         either WAITING FOR TRIGGER or, if conditions are appropriate fast enough,
**         TRIGGERED.
**       - \b \c \ES_PERFMODE - Performance Analyzer Mode will change to the commanded trigger mode (TRIGGER START,
**         TRIGGER CENTER, or TRIGGER END).
**       - \b \c \ES_PERFTRIGCNT - Performance Trigger Count will go to zero
**       - \b \c \ES_PERFDATASTART - Data Start Index will go to zero
**       - \b \c \ES_PERFDATAEND - Data End Index will go to zero
**       - \b \c \ES_PERFDATACNT - Performance Data Counter will go to zero
**       - The #CFE_ES_PERF_STARTCMD_EID debug event message will be
**         generated.
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - A previous #CFE_ES_STOP_PERF_DATA_CC command has not completely finished.
**       - An invalid trigger mode is requested.
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \ES_CMDEC - command error counter will increment
**       - A command specific error event message is issued for all error
**         cases
**
**  \par Criticality
**       This command is not inherently dangerous but may cause a small increase in CPU
**       utilization as the performance analyzer data is collected.
**
**  \sa #CFE_ES_STOP_PERF_DATA_CC, #CFE_ES_SET_PERF_FILTER_MASK_CC, #CFE_ES_SET_PERF_TRIGGER_MASK_CC
*/
#define CFE_ES_START_PERF_DATA_CC 14

/** \cfeescmd Stop Performance Analyzer and write data file
**
**  \par Description
**       This command stops the Performance Analyzer from collecting any more data,
**       and writes all previously collected performance data to a log file.
**
**  \cfecmdmnemonic \ES_STOPLADATA
**
**  \par Command Structure
**       #CFE_ES_StopPerfDataCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \ES_CMDPC - command execution counter will
**         increment
**       - \b \c \ES_PERFSTATE - Current performance analyzer state will change to
**         IDLE.
**       - The #CFE_ES_PERF_STOPCMD_EID debug event message will be
**         generated to indicate that data collection has been stopped.
**         NOTE: Performance log data is written to the file as a background job.
**         This event indicates that the file write process is initiated, not that
**         it has completed.
**       - The file specified in the command (or the default specified
**         by the #CFE_PLATFORM_ES_DEFAULT_PERF_DUMP_FILENAME configuration parameter) will be
**         updated with the latest information.
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The file name specified could not be parsed
**       - Log data from a previous Stop Performance Analyzer command is still
**         being written to a file.
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \ES_CMDEC - command error counter will increment
**       - A command specific error event message is issued for all error
**         cases
**
**       NOTE: The performance analyzer data collection will still be stopped
**       in the event of an error parsing the log file name or writing the log file.
**
**  \par Criticality
**       This command is not inherently dangerous.  However, depending on configuration,
**       performance data log files may be large in size and thus may fill the available
**       storage.
**
**  \sa #CFE_ES_START_PERF_DATA_CC, #CFE_ES_SET_PERF_FILTER_MASK_CC, #CFE_ES_SET_PERF_TRIGGER_MASK_CC
*/
#define CFE_ES_STOP_PERF_DATA_CC 15

/** \cfeescmd Set Performance Analyzer's Filter Masks
**
**  \par Description
**       This command sets the Performance Analyzer's Filter Masks.
**
**  \cfecmdmnemonic \ES_LAFILTERMASK
**
**  \par Command Structure
**       #CFE_ES_SetPerfFilterMaskCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \ES_CMDPC - command execution counter will
**         increment
**       - \b \c \ES_PERFFLTRMASK - the current performance filter mask
**         value(s) should reflect the commanded value
**       - The #CFE_ES_PERF_FILTMSKCMD_EID debug event message will be
**         generated.
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The Filter Mask ID number is out of range
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \ES_CMDEC - command error counter will increment
**       - A command specific error event message is issued for all error
**         cases
**
**  \par Criticality
**       Changing the filter masks may cause a small change in the Performance
**       Analyzer's CPU utilization.
**
**  \sa #CFE_ES_START_PERF_DATA_CC, #CFE_ES_STOP_PERF_DATA_CC, #CFE_ES_SET_PERF_TRIGGER_MASK_CC
*/
#define CFE_ES_SET_PERF_FILTER_MASK_CC 16

/** \cfeescmd Set Performance Analyzer's Trigger Masks
**
**  \par Description
**       This command sets the Performance Analyzer's Trigger Masks.
**
**  \cfecmdmnemonic \ES_LATRIGGERMASK
**
**  \par Command Structure
**       #CFE_ES_SetPerfTriggerMaskCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \ES_CMDPC - command execution counter will
**         increment
**       - \b \c \ES_PERFTRIGMASK - the current performance trigger mask
**         value(s) should reflect the commanded value
**       - The #CFE_ES_PERF_TRIGMSKCMD_EID debug event message will be
**         generated.
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The Trigger Mask ID number is out of range
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \ES_CMDEC - command error counter will increment
**       - A command specific error event message is issued for all error
**         cases
**
**  \par Criticality
**       Changing the trigger masks may cause a small change in the Performance
**       Analyzer's CPU utilization.
**
**  \sa #CFE_ES_START_PERF_DATA_CC, #CFE_ES_STOP_PERF_DATA_CC, #CFE_ES_SET_PERF_FILTER_MASK_CC
*/
#define CFE_ES_SET_PERF_TRIGGER_MASK_CC 17

/** \cfeescmd Set Executive Services System Log Mode to Discard/Overwrite
**
**  \par Description
**       This command allows the user to configure the Executive Services
**       to either discard new System Log messages when it is full or to
**       overwrite the oldest messages.
**
**  \cfecmdmnemonic \ES_OVERWRITESYSLOGMODE
**
**  \par Command Structure
**       #CFE_ES_OverWriteSysLogCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \ES_CMDPC - command execution counter will
**         increment
**       - \b \c \ES_SYSLOGMODE - Current System Log Mode should reflect
**         the commanded value
**       - The #CFE_ES_SYSLOGMODE_EID debug event message will be
**         generated.
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The desired mode is neither #CFE_ES_LogMode_OVERWRITE or #CFE_ES_LogMode_DISCARD
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \ES_CMDEC - command error counter will increment
**       - A command specific error event message is issued for all error
**         cases
**
**  \par Criticality
**       None. (It should be noted that "Overwrite" mode would allow a message
**       identifying the cause of a problem to be lost by a subsequent flood of
**       additional messages).
**
**  \sa #CFE_ES_CLEAR_SYS_LOG_CC, #CFE_ES_WRITE_SYS_LOG_CC
*/
#define CFE_ES_OVER_WRITE_SYS_LOG_CC 18

/** \cfeescmd Resets the Processor Reset Counter to Zero
**
**  \par Description
**       This command allows the user to reset the Processor Reset Counter to zero.
**       The Processor Reset Counter counts the number of Processor Resets that
**       have occurred so as to identify when a Processor Reset should automatically
**       be upgraded to a full Power-On Reset.
**
**  \cfecmdmnemonic \ES_RESETPRCNT
**
**  \par Command Structure
**       #CFE_ES_ResetPRCountCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \ES_CMDPC - command execution counter will
**         increment
**       - \b \c \ES_PROCRESETCNT - Current number of processor resets will go to zero
**       - The #CFE_ES_RESET_PR_COUNT_EID informational event message will be
**         generated.
**
**  \par Error Conditions
**       There are no error conditions for this command. If the Executive
**       Services receives the command, the event is sent (although it
**       may be filtered by EVS) and the counter is incremented
**       unconditionally.
**
**  \par Criticality
**       This command is not critical.  The only impact would be that the system
**       would have to have more processor resets before an automatic power-on reset
**       occurred.
**
**  \sa #CFE_ES_SET_MAX_PR_COUNT_CC, #CFE_ES_RESET_COUNTERS_CC
*/
#define CFE_ES_RESET_PR_COUNT_CC 19

/** \cfeescmd Configure the Maximum Number of Processor Resets before a Power-On Reset
**
**  \par Description
**       This command allows the user to specify the number of Processor Resets that
**       are allowed before the next Processor Reset is upgraded to a Power-On Reset.
**
**  \cfecmdmnemonic \ES_SETMAXPRCNT
**
**  \par Command Structure
**       #CFE_ES_SetMaxPRCountCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \ES_CMDPC - command execution counter will
**         increment
**       - \b \c \ES_MAXPROCRESETS - Current maximum number of processor resets
**         before an automatic power-on reset will go to the command specified value.
**       - The #CFE_ES_SET_MAX_PR_COUNT_EID informational event message will be
**         generated.
**
**  \par Error Conditions
**       There are no error conditions for this command. If the Executive
**       Services receives the command, the event is sent (although it
**       may be filtered by EVS) and the counter is incremented
**       unconditionally.
**
**  \par Criticality
**       If the operator were to set the Maximum Processor Reset Count to too high a value,
**       the processor would require an inordinate number of consecutive processor resets
**       before an automatic power-on reset would occur.  This could potentially leave the
**       spacecraft without any control for a significant amount of time if a processor reset
**       fails to clear a problem.
**
**  \sa #CFE_ES_RESET_PR_COUNT_CC
*/
#define CFE_ES_SET_MAX_PR_COUNT_CC 20

/** \cfeescmd Delete Critical Data Store
**
**  \par Description
**       This command allows the user to delete a Critical Data Store that was created
**       by an Application that is now no longer executing.
**
**  \cfecmdmnemonic \ES_DELETECDS
**
**  \par Command Structure
**       #CFE_ES_DeleteCDSCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \ES_CMDPC - command execution counter will
**         increment
**       - The #CFE_ES_CDS_DELETED_INFO_EID informational event message will be
**         generated.
**       - The specified CDS should no longer appear in a CDS Registry dump generated
**         upon receipt of the #CFE_ES_DUMP_CDS_REGISTRY_CC command
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The specified CDS is the CDS portion of a Critical Table
**       - The specified CDS is not found in the CDS Registry
**       - The specified CDS is associated with an Application that is still active
**       - An error occurred while accessing the CDS memory (see the System Log for more details)
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \ES_CMDEC - command error counter will increment
**       - A command specific error event message is issued for all error
**         cases
**
**  \par Criticality
**       This command is not critical because it is not possible to delete a CDS that is
**       associated with an active application.  However, deleting a CDS does eliminate
**       any "history" that an application may be wishing to keep.
**
**  \sa #CFE_ES_DUMP_CDS_REGISTRY_CC, #CFE_TBL_DELETE_CDS_CC
*/
#define CFE_ES_DELETE_CDS_CC 21

/** \cfeescmd Telemeter Memory Pool Statistics
**
**  \par Description
**       This command allows the user to obtain a snapshot of the statistics maintained
**       for a specified memory pool.
**
**  \cfecmdmnemonic \ES_TLMPOOLSTATS
**
**  \par Command Structure
**       #CFE_ES_SendMemPoolStatsCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \ES_CMDPC - command execution counter will
**         increment
**       - The #CFE_ES_TLM_POOL_STATS_INFO_EID debug event message will be
**         generated.
**       - The \link #CFE_ES_MemStatsTlm_t Memory Pool Statistics Telemetry Packet \endlink
**         is produced
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The specified handle is not associated with a known memory pool
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \ES_CMDEC - command error counter will increment
**       - A command specific error event message is issued for all error
**         cases
**
**  \par Criticality
**       <B>An incorrect Memory Pool Handle value can cause a system crash.</B>
**       Extreme care should be taken to ensure the memory handle value
**       used in the command is correct.
**
**  \sa
*/
#define CFE_ES_SEND_MEM_POOL_STATS_CC 22

/** \cfeescmd Dump Critical Data Store Registry to a File
**
**  \par Description
**       This command allows the user to dump the Critical Data Store
**       Registry to an onboard file.
**
**  \cfecmdmnemonic \ES_DUMPCDSREG
**
**  \par Command Structure
**       #CFE_ES_DumpCDSRegistryCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \ES_CMDPC - command execution counter will
**         increment
**       - The #CFE_ES_CDS_REG_DUMP_INF_EID debug event message will be
**         generated.
**       - The file specified in the command (or the default specified
**         by the #CFE_PLATFORM_ES_DEFAULT_CDS_REG_DUMP_FILE configuration parameter) will be
**         updated with the latest information.
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The file name specified could not be parsed
**       - Error occurred while creating or writing to the dump file
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \ES_CMDEC - command error counter will increment
**       - A command specific error event message is issued for all error
**         cases
**
**  \par Criticality
**       This command is not inherently dangerous.  It will create a new
**       file in the file system (or overwrite an existing one) and could,
**       if performed repeatedly without sufficient file management by the
**       operator, fill the file system.
**
**  \sa  #CFE_ES_DELETE_CDS_CC, #CFE_TBL_DELETE_CDS_CC
*/
#define CFE_ES_DUMP_CDS_REGISTRY_CC 23

/** \cfeescmd Writes a list of All Executive Services Tasks to a File
**
**  \par Description
**       This command takes the information kept by Executive Services on all of the
**       registered tasks and writes it to the specified file.
**
**  \cfecmdmnemonic \ES_WRITETASKINFO2FILE
**
**  \par Command Structure
**       #CFE_ES_QueryAllTasksCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \ES_CMDPC - command execution counter will
**         increment
**       - The #CFE_ES_TASKINFO_EID debug event message will be
**         generated.
**       - The file specified in the command (or the default specified
**         by the #CFE_PLATFORM_ES_DEFAULT_TASK_LOG_FILE configuration parameter) will be
**         updated with the latest information.
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The file name specified could not be parsed
**       - An Error occurs while trying to write to the file
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \ES_CMDEC - command error counter will increment
**       - A command specific error event message is issued for all error
**         cases
**
**  \par Criticality
**       This command is not inherently dangerous.  It will create a new
**       file in the file system (or overwrite an existing one) and could,
**       if performed repeatedly without sufficient file management by the
**       operator, fill the file system.
**
**  \sa #CFE_ES_QUERY_ALL_CC, CFE_ES_QUERY_ONE_CC
*/
#define CFE_ES_QUERY_ALL_TASKS_CC 24

/** \} */

#endif
