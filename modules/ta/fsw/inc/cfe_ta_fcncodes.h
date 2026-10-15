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
 *   Specification for the CFE Task Affinity (CFE_TA) command function codes
 *
 * @note
 *   This file should be strictly limited to the command/function code (CC)
 *   macro definitions.  Other definitions such as enums, typedefs, or other
 *   macros should be placed in the msgdefs.h or msg.h files.
 */
#ifndef CFE_TA_FCNCODES_H
#define CFE_TA_FCNCODES_H

#include "cfe_ta_fcncode_values.h"

/****************************************
** TA task command packet command codes
****************************************/

/** \brief Task Affinity No-Op
**
**  \par Description
**       This command performs no other function than to increment the
**       command execution counter. The command may be used to verify
**       general aliveness of the Task Affinity task.
**
**  \cfecmdmnemonic \TA_NOOP
**
**  \par Command Structure
**       #CFE_TA_NoopCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the
**       following telemetry:
**       - \b \c \TA_CMDPC - command execution counter will
**         increment
**       - The #CFE_TA_NOOP_INF_EID informational event message will
**         be generated
**
**  \par Error Conditions
**       There are no error conditions for this command. If Task
**       Affinity receives the command, the event is sent (although it
**       may be filtered by EVS) and the counter is incremented
**       unconditionally.
**
**  \par Criticality
**       None
**
**  \sa
*/
#define CFE_TA_NOOP_CC CFE_TA_CCVAL(NOOP)

/** \brief Task Affinity Reset Counters
**
**  \par Description
**       This command resets the following counters within Task
**       Affinity housekeeping telemetry:
**       - Command Execution Counter (\TA_CMDPC)
**       - Command Error Counter (\TA_CMDEC)
**
**  \cfecmdmnemonic \TA_RESETCTRS
**
**  \par Command Structure
**       #CFE_TA_ResetCountersCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the
**       following telemetry:
**       - \b \c \TA_CMDPC - command execution counter will
**         be reset to 0
**       - All other counters listed in description will be reset to 0
**       - The #CFE_TA_RESET_COUNTERS_INF_EID informational event message will
**         be generated
**
**  \par Error Conditions
**       There are no error conditions for this command. If Task
**       Affinity receives the command, the event is sent (although it
**       may be filtered by EVS) and the counter is reset unconditionally.
**
**  \par Criticality
**       This command is not inherently dangerous.  However, it is
**       possible for ground systems and on-board safing procedures
**       to be designed such that they react to changes in the counter
**       values that are reset by this command.
**
**  \sa
*/
#define CFE_TA_RESET_COUNTERS_CC CFE_TA_CCVAL(RESET_COUNTERS)

/** \brief Writes a list of All Task Affinity Tasks to a File
**
**  \par Description
**       This command takes the information kept by Task Affinity on all of the
**       registered tasks and writes it to the specified file.
**
**  \cfecmdmnemonic \TA_QUERY_ALL
**
**  \par Command Structure
**       #CFE_TA_QueryAllTasksCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \TA_CMDPC - command execution counter will
**         increment
**       - The #CFE_TA_TASKINFO_EID debug event message will be
**         generated.
**       - The file specified in the command (or the default specified
**         by the configuration parameter) will be
**         updated with the latest information.
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The file name specified could not be parsed
**       - An Error occurs while trying to write to the file
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \TA_CMDEC - command error counter will increment
**       - A command specific error event message is issued for all error
**         cases
**
**  \par Criticality
**       This command is not inherently dangerous.  It will create a new
**       file in the file system (or overwrite an existing one) and could,
**       if performed repeatedly without sufficient file management by the
**       operator, fill the file system.
**
**  \sa
*/
#define CFE_TA_QUERY_ALL_TASKS_CC CFE_TA_CCVAL(QUERY_ALL_TASKS)

/** \brief Updates the Task Affinity of a task
**
**  \par Description
**       This command sets a new CPU core affinity mask for a specific
**       running OS task using the task's name.
**
**  \cfecmdmnemonic \TA_SET_AFFINITY
**
**  \par Command Structure
**       #CFE_TA_SetTaskAffinityCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \TA_CMDPC - command execution counter will
**         increment
**       - The #CFE_TA_SET_TASK_AFFINITY_CMD_INF_EID information event message will be
**         generated.
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The task name is not currently tracked by TA
**       - The OS cannot find a task with the given name
**       - The OS rejects the affinity mask
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \TA_CMDEC - command error counter will increment
**       - A command specific error event message is issued for all error
**         cases
**
**  \par Criticality
**       Improperly restricting high-priority tasks to overloaded or
**       invalid cores can result in task starvation and system instability.
**
**  \sa
*/
#define CFE_TA_SET_TASK_AFFINITY_CC CFE_TA_CCVAL(SET_TASK_AFFINITY)

/** \brief Reads the Task Affinity of a task
**
**  \par Description
**       This command requests the current CPU core affinity mask for a
**       specific running OS task from the operating system and updates
**       TA's internal records.
**
**  \cfecmdmnemonic \TA_GET_AFFINITY
**
**  \par Command Structure
**       #CFE_TA_GetTaskAffinityCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \TA_CMDPC - command execution counter will
**         increment
**       - The #CFE_TA_GET_TASK_AFFINITY_CMD_INF_EID information event message will be
**         generated.
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The task name is not currently tracked by TA
**       - The OS cannot find a task with the given name
**       - The OS fails to return the affinity mask
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \TA_CMDEC - command error counter will increment
**       - A command specific error event message is issued for all error
**         cases
**
**  \par Criticality
**       This command is not inherently dangerous as it only reads state.
**
**  \sa
*/
#define CFE_TA_GET_TASK_AFFINITY_CC CFE_TA_CCVAL(GET_TASK_AFFINITY)

#endif
