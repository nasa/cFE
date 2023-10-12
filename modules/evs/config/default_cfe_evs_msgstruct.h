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
 *  cFE Executive Services (EVS) Command and Telemetry packet definition file.
 *
 *  References:
 *     Flight Software Branch C Coding Standard Version 1.0a
 *     cFE Flight Software Application Developers Guide
 *
 *  Notes:
 *
 */
#ifndef CFE_EVS_MSGSTRUCT_H
#define CFE_EVS_MSGSTRUCT_H

/************************************************************************
 * Includes
 ************************************************************************/
#include "common_types.h"
#include "cfe_evs_msgdefs.h"
#include "cfe_evs_extern_typedefs.h"
#include "cfe_msg_hdr.h"

/******************  Structure Definitions *********************/

/*
 * The NoArgsCmd is shared by several EVS command handlers.
 * Create a unique type for each one so the prototypes will follow the naming pattern,
 * allowing each command to evolve independently.
 */
typedef struct CFE_EVS_NoopCmd
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command header */
} CFE_EVS_NoopCmd_t;

typedef struct CFE_EVS_ResetCountersCmd
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command header */
} CFE_EVS_ResetCountersCmd_t;

typedef struct CFE_EVS_ClearLogCmd
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command header */
} CFE_EVS_ClearLogCmd_t;

typedef struct CFE_EVS_SendHkCmd
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command header */
} CFE_EVS_SendHkCmd_t;

/**
 * \brief Write Event Log to File Command
 */
typedef struct CFE_EVS_WriteLogDataFileCmd
{
    CFE_MSG_CommandHeader_t      CommandHeader; /**< \brief Command header */
    CFE_EVS_LogFileCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_EVS_WriteLogDataFileCmd_t;

/**
 * \brief Write Event Services Application Information to File Command
 */
typedef struct CFE_EVS_WriteAppDataFileCmd
{
    CFE_MSG_CommandHeader_t      CommandHeader; /**< \brief Command header */
    CFE_EVS_AppDataCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_EVS_WriteAppDataFileCmd_t;

/**
 * \brief Set Log Mode Command
 */
typedef struct CFE_EVS_SetLogModeCmd
{
    CFE_MSG_CommandHeader_t      CommandHeader; /**< \brief Command header */
    CFE_EVS_SetLogMode_Payload_t Payload;       /**< \brief Command payload */
} CFE_EVS_SetLogModeCmd_t;

/**
 * \brief Set Event Format Mode Command
 */
typedef struct CFE_EVS_SetEventFormatModeCmd
{
    CFE_MSG_CommandHeader_t              CommandHeader; /**< \brief Command header */
    CFE_EVS_SetEventFormatMode_Payload_t Payload;       /**< \brief Command payload */
} CFE_EVS_SetEventFormatModeCmd_t;

/*
 * The CFE_EVS_BitMaskCmd_t is shared by several EVS command handlers.
 * Create a unique type for each one so the prototypes will follow the naming pattern,
 * allowing each command to evolve independently.
 */
typedef struct CFE_EVS_EnablePortsCmd
{
    CFE_MSG_CommandHeader_t      CommandHeader; /**< \brief Command header */
    CFE_EVS_BitMaskCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_EVS_EnablePortsCmd_t;

typedef struct CFE_EVS_DisablePortsCmd
{
    CFE_MSG_CommandHeader_t      CommandHeader; /**< \brief Command header */
    CFE_EVS_BitMaskCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_EVS_DisablePortsCmd_t;

typedef struct CFE_EVS_EnableEventTypeCmd
{
    CFE_MSG_CommandHeader_t      CommandHeader; /**< \brief Command header */
    CFE_EVS_BitMaskCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_EVS_EnableEventTypeCmd_t;

typedef struct CFE_EVS_DisableEventTypeCmd
{
    CFE_MSG_CommandHeader_t      CommandHeader; /**< \brief Command header */
    CFE_EVS_BitMaskCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_EVS_DisableEventTypeCmd_t;

/*
 * The CFE_EVS_AppNameCmd_t is shared by several EVS command handlers.
 * Create a unique type for each one so the prototypes will follow the naming pattern,
 * allowing each command to evolve independently.
 */
typedef struct CFE_EVS_EnableAppEventsCmd
{
    CFE_MSG_CommandHeader_t      CommandHeader; /**< \brief Command header */
    CFE_EVS_AppNameCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_EVS_EnableAppEventsCmd_t;

typedef struct CFE_EVS_DisableAppEventsCmd
{
    CFE_MSG_CommandHeader_t      CommandHeader; /**< \brief Command header */
    CFE_EVS_AppNameCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_EVS_DisableAppEventsCmd_t;

typedef struct CFE_EVS_ResetAppCounterCmd
{
    CFE_MSG_CommandHeader_t      CommandHeader; /**< \brief Command header */
    CFE_EVS_AppNameCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_EVS_ResetAppCounterCmd_t;

typedef struct CFE_EVS_ResetAllFiltersCmd
{
    CFE_MSG_CommandHeader_t      CommandHeader; /**< \brief Command header */
    CFE_EVS_AppNameCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_EVS_ResetAllFiltersCmd_t;

/*
 * The CFE_EVS_AppNameEventIDCmd_t is shared by several EVS command handlers.
 * Create a unique type for each one so the prototypes will follow the naming pattern,
 * allowing each command to evolve independently.
 */
typedef struct CFE_EVS_ResetFilterCmd
{
    CFE_MSG_CommandHeader_t             CommandHeader; /**< \brief Command header */
    CFE_EVS_AppNameEventIDCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_EVS_ResetFilterCmd_t;

typedef struct CFE_EVS_DeleteEventFilterCmd
{
    CFE_MSG_CommandHeader_t             CommandHeader; /**< \brief Command header */
    CFE_EVS_AppNameEventIDCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_EVS_DeleteEventFilterCmd_t;

/*
 * The CFE_EVS_AppNameBitMaskCmd_t is shared by several EVS command handlers.
 * Create a unique type for each one so the prototypes will follow the naming pattern,
 * allowing each command to evolve independently.
 */
typedef struct CFE_EVS_EnableAppEventTypeCmd
{
    CFE_MSG_CommandHeader_t             CommandHeader; /**< \brief Command header */
    CFE_EVS_AppNameBitMaskCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_EVS_EnableAppEventTypeCmd_t;

typedef struct CFE_EVS_DisableAppEventTypeCmd
{
    CFE_MSG_CommandHeader_t             CommandHeader; /**< \brief Command header */
    CFE_EVS_AppNameBitMaskCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_EVS_DisableAppEventTypeCmd_t;

/*
 * The CFE_EVS_AppNameEventIDMaskCmd_t is shared by several EVS command handlers.
 * Create a unique type for each one so the prototypes will follow the naming pattern,
 * allowing each command to evolve independently.
 */
typedef struct CFE_EVS_AddEventFilterCmd
{
    CFE_MSG_CommandHeader_t                 CommandHeader; /**< \brief Command header */
    CFE_EVS_AppNameEventIDMaskCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_EVS_AddEventFilterCmd_t;

typedef struct CFE_EVS_SetFilterCmd
{
    CFE_MSG_CommandHeader_t                 CommandHeader; /**< \brief Command header */
    CFE_EVS_AppNameEventIDMaskCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_EVS_SetFilterCmd_t;

/*************************************************************************/

/*******************************************/
/* Telemetry Message Encapsulation Formats */
/*******************************************/

typedef struct CFE_EVS_HousekeepingTlm
{
    CFE_MSG_TelemetryHeader_t         TelemetryHeader; /**< \brief Telemetry header */
    CFE_EVS_HousekeepingTlm_Payload_t Payload;         /**< \brief Telemetry payload */
} CFE_EVS_HousekeepingTlm_t;

typedef struct CFE_EVS_LongEventTlm
{
    CFE_MSG_TelemetryHeader_t      TelemetryHeader; /**< \brief Telemetry header */
    CFE_EVS_LongEventTlm_Payload_t Payload;         /**< \brief Telemetry payload */
} CFE_EVS_LongEventTlm_t;

typedef struct CFE_EVS_ShortEventTlm
{
    CFE_MSG_TelemetryHeader_t       TelemetryHeader; /**< \brief Telemetry header */
    CFE_EVS_ShortEventTlm_Payload_t Payload;         /**< \brief Telemetry payload */
} CFE_EVS_ShortEventTlm_t;

#endif
