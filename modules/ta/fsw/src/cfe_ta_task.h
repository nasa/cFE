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
 * Main header and specification for the CFE Task Affinity (TA) command and telemetry
 * message data types.
 */

#ifndef CFE_TA_TASK_H
#define CFE_TA_TASK_H

/********************************** Include Files  ************************************/
#include "common_types.h"
#include "cfe_platform_cfg.h"
#include "cfe_mission_cfg.h"
#include "osconfig.h"
#include "cfe_ta_eventids.h"
#include "osapi-task-affinity.h"
#include "cfe_ta_tasklist.h"
#include "cfe_ta_msg.h"

/*
** Task Affinity Core Module Functions
*/
int32 CFE_TA_EarlyInit(void);
void  CFE_TA_TaskMain(void);
int32 CFE_TA_CleanUpApp(uint32 AppId);

/*********************  Macro and Constant Type Definitions   ***************************/

/**
 * \name TA Configuration Parameters
 * \{
 */

/** \brief TA Command Pipe Name */
#define CFE_TA_PIPE_NAME "TA_CMD_PIPE"

/** \brief TA Command Pipe Depth */
#define CFE_TA_PIPE_DEPTH 32

/** \brief TA Maximum Event Send Count */
#define CFE_TA_MAX_EVENT_SEND_COUNT 65535

/** \brief TA Task Log File Description */
#define CFE_TA_TASK_LOG_DESC "TA Task Affinity Info file"

/** \} */

/**
 * \name TA Error Codes
 * \{
 */
#define CFE_TA_APP_TASK_NOT_FOUND               -1
#define CFE_TA_APP_GET_TASK_AFFINITY_FAILED     -2
#define CFE_TA_APP_SET_TASK_AFFINITY_FAILED     -3
#define CFE_TA_APP_INVALID_TASK_AFFINITY_TYPE   -4
#define CFE_TA_APP_TASK_STARTUP_AFFINITY_FAILED -5
/** \} */

/************************  Internal Structure Definitions  *****************************/

/**
 * \brief TA Diagnostic Packet Structure
 */
typedef struct
{
    uint16                   CoresConfigured;               /**< \brief Number of physical cores configured */
    uint16                   TACoresMax;                    /**< \brief Maximum TA support bits*/
    CFE_TA_CpuTaskAffinity_t CpuTaskAffinity[OS_MAX_TASKS]; /**< \brief Tracked task array */
} CFE_TA_DiagInfo_t;

/**
 * \brief TA Global Data Structure
 */
typedef struct
{
    /*
    ** TA task data
    */
    /*
    ** TA Task command interface counters
    */
    uint8 CommandCounter;      /**< \brief Commands successfully processed */
    uint8 CommandErrorCounter; /**< \brief Commands that failed processing */
    uint8 StartupCounter;      /**< \brief Tasks processed during startup */
    uint8 StartupErrorCounter; /**< \brief Tasks that failed affinity assignment at startup */

    /*
    ** TA Task housekeeping telemetry
    */
    CFE_TA_HousekeepingTlm_t TA_TlmPkt; /**< \brief Housekeeping telemetry packet */

    CFE_TA_DiagInfo_t CFE_TA_DiagInfo; /**< \brief Diagnostic packet for Query All command */

    /*
    ** TA Task operational data (not reported in housekeeping)
    */
    CFE_SB_PipeId_t TA_CommandPipe; /**< \brief Command pipe ID */

    CFE_ES_AppId_t TA_AppID; /**< \brief TA Application ID */
} CFE_TA_Global_t;

/*
 *  Global variables specific to TA module
 */
extern CFE_TA_Global_t          CFE_TA_Global;
extern CFE_TA_CpuTaskAffinity_t CFE_TA_CpuTaskAffinity[OS_MAX_TASKS];

/*****************************  Function Prototypes   **********************************/

/*
 * Functions used within this module and by the unit test
 */

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Task Initialization
 *
 * This function performs any necessary TA task initialization.
 */
int32 CFE_TA_TaskInit(void);

/*
 * TA Message Handler Functions
 */

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Message Handler Function
 *
 * Request for housekeeping status telemetry packet.
 */
int32 CFE_TA_ReportHousekeepingCmd(const CFE_TA_SendHkCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Message Handler Function
 *
 * This function processes "noop" commands received on the TA command pipe.
 */
int32 CFE_TA_NoopCmd(const CFE_TA_NoopCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Message Handler Function
 *
 * This function resets all the global counter variables that are part of the task telemetry.
 */
int32 CFE_TA_ResetCountersCmd(const CFE_TA_ResetCountersCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief  Write all Task Data to a file
 */
int32 CFE_TA_QueryAllTasksCmd(const CFE_TA_QueryAllTasksCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Message Handler Function
 *
 * This function sets the task affinity using the task name and an affinity mask.
 */
int32 CFE_TA_SetTaskAffinityCmd(const CFE_TA_SetTaskAffinityCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Message Handler Function
 *
 * This function gets the task affinity using the task name.
 */
int32 CFE_TA_GetTaskAffinityCmd(const CFE_TA_GetTaskAffinityCmd_t *data);

/******************************************************************************
**
**  Purpose:
**    Linux Task creation event handler.
**    Sets the kernel task name using a glibc-specific (non-posix) function.
**    Calls the TA Startup Affinity function
**
*/
int32 CFE_TA_OS_EventHandler(OS_Event_t event, osal_id_t object_id, void *data);

#endif /* CFE_TA_TASK_H */
