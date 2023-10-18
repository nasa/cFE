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
 *  cFE Executive Services (TBL) Command and Telemetry packet definition file.
 *
 *  References:
 *     Flight Software Branch C Coding Standard Version 1.0a
 *     cFE Flight Software Application Developers Guide
 *
 *  Notes:
 *
 */
#ifndef CFE_TBL_MSGSTRUCT_H
#define CFE_TBL_MSGSTRUCT_H

/************************************************************************
 * Includes
 ************************************************************************/
#include "cfe_tbl_msgdefs.h"
#include "cfe_msg_hdr.h"

/********************************/
/* Command Message Data Formats */
/********************************/

/*
 * Typedefs for each of the messages that share the command format
 * Allows each command to evolve independently, while following the command
 * handler prototype convention
 */
typedef struct CFE_TBL_NoopCmd
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command header */
} CFE_TBL_NoopCmd_t;

typedef struct CFE_TBL_ResetCountersCmd
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command header */
} CFE_TBL_ResetCountersCmd_t;

typedef struct CFE_TBL_SendHkCmd
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command header */
} CFE_TBL_SendHkCmd_t;

/**
 * \brief Load Table Command
 */
typedef struct CFE_TBL_LoadCmd
{
    CFE_MSG_CommandHeader_t   CommandHeader; /**< \brief Command header */
    CFE_TBL_LoadCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_TBL_LoadCmd_t;

/**
 * /brief Dump Table Command
 */
typedef struct CFE_TBL_DumpCmd
{
    CFE_MSG_CommandHeader_t   CommandHeader; /**< \brief Command header */
    CFE_TBL_DumpCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_TBL_DumpCmd_t;

/**
 * \brief Validate Table Command
 */
typedef struct CFE_TBL_ValidateCmd
{
    CFE_MSG_CommandHeader_t       CommandHeader; /**< \brief Command header */
    CFE_TBL_ValidateCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_TBL_ValidateCmd_t;

/**
 * \brief Activate Table Command
 */
typedef struct CFE_TBL_ActivateCmd
{
    CFE_MSG_CommandHeader_t       CommandHeader; /**< \brief Command header */
    CFE_TBL_ActivateCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_TBL_ActivateCmd_t;

/**
 * \brief Dump Registry Command
 */
typedef struct CFE_TBL_DumpRegistryCmd
{
    CFE_MSG_CommandHeader_t           CommandHeader; /**< \brief Command header */
    CFE_TBL_DumpRegistryCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_TBL_DumpRegistryCmd_t;

/**
 * \brief Send Table Registry Command
 */
typedef struct CFE_TBL_SendRegistryCmd
{
    CFE_MSG_CommandHeader_t           CommandHeader; /**< \brief Command header */
    CFE_TBL_SendRegistryCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_TBL_SendRegistryCmd_t;

/**
 * \brief Delete Critical Table CDS Command
 */
typedef struct CFE_TBL_DeleteCDSCmd
{
    CFE_MSG_CommandHeader_t     CommandHeader; /**< \brief Command header */
    CFE_TBL_DelCDSCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_TBL_DeleteCDSCmd_t;

/**
 * \brief Abort Load Command
 */
typedef struct CFE_TBL_AbortLoadCmd
{
    CFE_MSG_CommandHeader_t        CommandHeader; /**< \brief Command header */
    CFE_TBL_AbortLoadCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_TBL_AbortLoadCmd_t;

/*************************************************************************/

/**
 * /brief Table Management Notification Command
 */
typedef struct CFE_TBL_NotifyCmd
{
    CFE_MSG_CommandHeader_t     CommandHeader; /**< \brief Command header */
    CFE_TBL_NotifyCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_TBL_NotifyCmd_t;

/*************************************************************************/

/**********************************/
/* Telemetry Message Data Formats */
/**********************************/

typedef struct CFE_TBL_HousekeepingTlm
{
    CFE_MSG_TelemetryHeader_t         TelemetryHeader; /**< \brief Telemetry header */
    CFE_TBL_HousekeepingTlm_Payload_t Payload;         /**< \brief Telemetry payload */
} CFE_TBL_HousekeepingTlm_t;

typedef struct CFE_TBL_TableRegistryTlm
{
    CFE_MSG_TelemetryHeader_t      TelemetryHeader; /**< \brief Telemetry header */
    CFE_TBL_TblRegPacket_Payload_t Payload;         /**< \brief Telemetry payload */
} CFE_TBL_TableRegistryTlm_t;

#endif
