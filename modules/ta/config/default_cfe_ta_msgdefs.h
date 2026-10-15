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
 *   Specification for the CFE Task Affinity (CFE_TA) command and telemetry
 *   message constant definitions.
 *
 *  For CFE_TA this is only the function/command code definitions
 */

#ifndef CFE_TA_MSGDEFS_H
#define CFE_TA_MSGDEFS_H

#include "cfe_mission_cfg.h"
#include "cfe_ta_mission_cfg.h"
#include "common_types.h"

/**
** \brief Query All Tasks Command Payload
**
** For command details, see #CFE_TA_QUERY_ALL_TASKS_CC
**/
typedef struct CFE_TA_QueryAllTasksCmd_Payload
{
    char FileName[CFE_MISSION_MAX_PATH_LEN]; /**< \brief Full path and filename
                                                where data is to be dumped */
} CFE_TA_QueryAllTasksCmd_Payload_t;

/**
** \brief Set Task Affinity Command Payload
**
** For command details, see #CFE_TA_SET_TASK_AFFINITY_CC
**/
typedef struct CFE_TA_SetTaskAffinityCmd_Payload
{
    char  TaskName[CFE_MISSION_MAX_API_LEN];             /**< \brief Name of the task */
    uint8 AffinityMask[CFE_MISSION_TA_MAX_NUM_CPUS / 8]; /**< \brief CPU core affinity mask (octets) */
} CFE_TA_SetTaskAffinityCmd_Payload_t;

/**
** \brief Get Task Affinity Command Payload
**
** For command details, see #CFE_TA_GET_TASK_AFFINITY_CC
**/
typedef struct CFE_TA_GetTaskAffinityCmd_Payload
{
    char TaskName[CFE_MISSION_MAX_API_LEN]; /**< \brief Name of the task to query
                                             */
} CFE_TA_GetTaskAffinityCmd_Payload_t;

/**
** \brief Housekeeping Telemetry Payload
**/
typedef struct CFE_TA_HousekeepingTlm_Payload
{
    uint8 CommandCounter;      /**< \brief Valid command execution counter */
    uint8 CommandErrorCounter; /**< \brief Command execution error counter */
    uint8 StartupCounter;      /**< \brief Tasks successfully assigned affinity at
                                  startup */
    uint8 StartupErrorCounter; /**< \brief Tasks failed to assign affinity at
                                  startup */
} CFE_TA_HousekeepingTlm_Payload_t;

#endif /* CFE_TA_MSGDEFS_H */
