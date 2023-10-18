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
 *  Purpose:
 *  cFE Executive Services (ES) Command and Telemetry packet definition file.
 *
 *  References:
 *     Flight Software Branch C Coding Standard Version 1.0a
 *     cFE Flight Software Application Developers Guide
 *
 *  Notes:
 *
 */
#ifndef CFE_ES_MSGSTRUCT_H
#define CFE_ES_MSGSTRUCT_H

/************************************************************************
 * Includes
 ************************************************************************/
#include "cfe_es_msgdefs.h"
#include "cfe_msg_hdr.h"
#include "cfe_mission_cfg.h"

/********************************/
/* Command Message Data Formats */
/********************************/

/*
 * The following commands all share the "NoArgs" format
 *
 * They are each given their own type name matching the command name, which
 * allows them to change independently in the future without changing the prototype
 * of the handler function.
 */
typedef struct CFE_ES_NoopCmd
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command header */
} CFE_ES_NoopCmd_t;

typedef struct CFE_ES_ResetCountersCmd
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command header */
} CFE_ES_ResetCountersCmd_t;

typedef struct CFE_ES_ClearSysLogCmd
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command header */
} CFE_ES_ClearSysLogCmd_t;

typedef struct CFE_ES_ClearERLogCmd
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command header */
} CFE_ES_ClearERLogCmd_t;

typedef struct CFE_ES_ResetPRCountCmd
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command header */
} CFE_ES_ResetPRCountCmd_t;

typedef struct CFE_ES_SendHkCmd
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command header */
} CFE_ES_SendHkCmd_t;

/**
 * \brief Restart cFE Command
 */
typedef struct CFE_ES_RestartCmd
{
    CFE_MSG_CommandHeader_t     CommandHeader; /**< \brief Command header */
    CFE_ES_RestartCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_ES_RestartCmd_t;

#ifndef jphfix
/**
 * \brief Generic file name command
 */
typedef struct CFE_ES_FileNameCmd
{
    CFE_MSG_CommandHeader_t      CommandHeader; /**< \brief Command header */
    CFE_ES_FileNameCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_ES_FileNameCmd_t;
#endif
/*
 * Unique typedefs for each of the commands that utilize the FileNameCmd
 * packet format
 */
typedef struct CFE_ES_QueryAllCmd
{
    CFE_MSG_CommandHeader_t      CommandHeader; /**< \brief Command header */
    CFE_ES_FileNameCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_ES_QueryAllCmd_t;

typedef struct CFE_ES_QueryAllTasksCmd
{
    CFE_MSG_CommandHeader_t      CommandHeader; /**< \brief Command header */
    CFE_ES_FileNameCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_ES_QueryAllTasksCmd_t;

typedef struct CFE_ES_WriteSysLogCmd
{
    CFE_MSG_CommandHeader_t      CommandHeader; /**< \brief Command header */
    CFE_ES_FileNameCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_ES_WriteSysLogCmd_t;

typedef struct CFE_ES_WriteERLogCmd
{
    CFE_MSG_CommandHeader_t      CommandHeader; /**< \brief Command header */
    CFE_ES_FileNameCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_ES_WriteERLogCmd_t;

/**
 * \brief Overwrite/Discard System Log Configuration Command Payload
 */
typedef struct CFE_ES_OverWriteSysLogCmd
{
    CFE_MSG_CommandHeader_t             CommandHeader; /**< \brief Command header */
    CFE_ES_OverWriteSysLogCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_ES_OverWriteSysLogCmd_t;

/**
 * \brief Start Application Command
 */
typedef struct CFE_ES_StartApp
{
    CFE_MSG_CommandHeader_t      CommandHeader; /**< \brief Command header */
    CFE_ES_StartAppCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_ES_StartAppCmd_t;

/*
 * Like NoArgsCmd, this message definition is shared by multiple commands.
 * Create a separate typedef for each one so they can all evolve independently
 * without affecting the prototype.
 */
typedef struct CFE_ES_StopAppCmd
{
    CFE_MSG_CommandHeader_t     CommandHeader; /**< \brief Command header */
    CFE_ES_AppNameCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_ES_StopAppCmd_t;

typedef struct CFE_ES_RestartAppCmd
{
    CFE_MSG_CommandHeader_t     CommandHeader; /**< \brief Command header */
    CFE_ES_AppNameCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_ES_RestartAppCmd_t;

typedef struct CFE_ES_QueryOneCmd
{
    CFE_MSG_CommandHeader_t     CommandHeader; /**< \brief Command header */
    CFE_ES_AppNameCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_ES_QueryOneCmd_t;

/**
 * \brief Reload Application Command
 */
typedef struct CFE_ES_ReloadAppCmd
{
    CFE_MSG_CommandHeader_t       CommandHeader; /**< \brief Command header */
    CFE_ES_AppReloadCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_ES_ReloadAppCmd_t;

/**
 * \brief Set Maximum Processor Reset Count Command
 */
typedef struct CFE_ES_SetMaxPRCountCmd
{
    CFE_MSG_CommandHeader_t           CommandHeader; /**< \brief Command header */
    CFE_ES_SetMaxPRCountCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_ES_SetMaxPRCountCmd_t;

/**
 * \brief Delete Critical Data Store Command
 */
typedef struct CFE_ES_DeleteCDSCmd
{
    CFE_MSG_CommandHeader_t       CommandHeader; /**< \brief Command header */
    CFE_ES_DeleteCDSCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_ES_DeleteCDSCmd_t;

/**
 * \brief Start Performance Analyzer Command
 */
typedef struct CFE_ES_StartPerfDataCmd
{
    CFE_MSG_CommandHeader_t       CommandHeader; /**< \brief Command header */
    CFE_ES_StartPerfCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_ES_StartPerfDataCmd_t;

/**
 * \brief Stop Performance Analyzer Command
 */
typedef struct CFE_ES_StopPerfDataCmd
{
    CFE_MSG_CommandHeader_t      CommandHeader; /**< \brief Command header */
    CFE_ES_StopPerfCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_ES_StopPerfDataCmd_t;

/**
 * \brief Set Performance Analyzer Filter Mask Command
 */
typedef struct CFE_ES_SetPerfFilterMaskCmd
{
    CFE_MSG_CommandHeader_t               CommandHeader; /**< \brief Command header */
    CFE_ES_SetPerfFilterMaskCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_ES_SetPerfFilterMaskCmd_t;

/**
 * \brief Set Performance Analyzer Trigger Mask Command
 */
typedef struct CFE_ES_SetPerfTriggerMaskCmd
{
    CFE_MSG_CommandHeader_t             CommandHeader; /**< \brief Command header */
    CFE_ES_SetPerfTrigMaskCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_ES_SetPerfTriggerMaskCmd_t;

/**
 * \brief Send Memory Pool Statistics Command
 */
typedef struct CFE_ES_SendMemPoolStatsCmd
{
    CFE_MSG_CommandHeader_t              CommandHeader; /**< \brief Command header */
    CFE_ES_SendMemPoolStatsCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_ES_SendMemPoolStatsCmd_t;

/**
 * \brief Dump CDS Registry Command
 */
typedef struct CFE_ES_DumpCDSRegistryCmd
{
    CFE_MSG_CommandHeader_t             CommandHeader; /**< \brief Command header */
    CFE_ES_DumpCDSRegistryCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_ES_DumpCDSRegistryCmd_t;

/*************************************************************************/

/**********************************/
/* Telemetry Message Data Formats */
/**********************************/

/**
**  \cfeestlm Single Application Information Packet
**/
typedef struct CFE_ES_OneAppTlm
{
    CFE_MSG_TelemetryHeader_t  TelemetryHeader; /**< \brief Telemetry header */
    CFE_ES_OneAppTlm_Payload_t Payload;         /**< \brief Telemetry payload */
} CFE_ES_OneAppTlm_t;

/**
**  \cfeestlm Memory Pool Statistics Packet
**/
typedef struct CFE_ES_MemStatsTlm
{
    CFE_MSG_TelemetryHeader_t     TelemetryHeader; /**< \brief Telemetry header */
    CFE_ES_PoolStatsTlm_Payload_t Payload;         /**< \brief Telemetry payload */
} CFE_ES_MemStatsTlm_t;

/**
**  \cfeestlm Executive Services Housekeeping Packet
**/
typedef struct CFE_ES_HousekeepingTlm
{
    CFE_MSG_TelemetryHeader_t        TelemetryHeader; /**< \brief Telemetry header */
    CFE_ES_HousekeepingTlm_Payload_t Payload;         /**< \brief Telemetry payload */
} CFE_ES_HousekeepingTlm_t;

#endif
