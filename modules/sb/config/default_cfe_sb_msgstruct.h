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
 *  cFE Executive Services (SB) Command and Telemetry packet definition file.
 *
 *  References:
 *     Flight Software Branch C Coding Standard Version 1.0a
 *     cFE Flight Software Application Developers Guide
 *
 *  Notes:
 *
 */
#ifndef CFE_SB_MSGSTRUCT_H
#define CFE_SB_MSGSTRUCT_H

/************************************************************************
 * Includes
 ************************************************************************/
#include "cfe_sb_msgdefs.h"
#include "cfe_msg_hdr.h"

/****************************
**  SB Command Formats     **
*****************************/

/*
 * SB Messages which have no payload are each
 * given unique typedefs to follow the command handler convention
 *
 * For the SB application these are mapped to the CFE_MSG_CommandHeader_t type,
 * as they contain only a primary + command header.
 */
typedef struct CFE_SB_NoopCmd
{
    CFE_MSG_CommandHeader_t CommandHeader;
} CFE_SB_NoopCmd_t;

typedef struct CFE_SB_ResetCountersCmd
{
    CFE_MSG_CommandHeader_t CommandHeader;
} CFE_SB_ResetCountersCmd_t;

typedef struct CFE_SB_EnableSubReportingCmd
{
    CFE_MSG_CommandHeader_t CommandHeader;
} CFE_SB_EnableSubReportingCmd_t;

typedef struct CFE_SB_DisableSubReportingCmd
{
    CFE_MSG_CommandHeader_t CommandHeader;
} CFE_SB_DisableSubReportingCmd_t;

typedef struct CFE_SB_SendSbStatsCmd
{
    CFE_MSG_CommandHeader_t CommandHeader;
} CFE_SB_SendSbStatsCmd_t;

typedef struct CFE_SB_SendPrevSubsCmd
{
    CFE_MSG_CommandHeader_t CommandHeader;
} CFE_SB_SendPrevSubsCmd_t;

typedef struct CFE_SB_SendHkCmd
{
    CFE_MSG_CommandHeader_t CommandHeader;
} CFE_SB_SendHkCmd_t;

/*
 * Create a unique typedef for each of the commands that share this format.
 */
typedef struct CFE_SB_WriteRoutingInfoCmd
{
    CFE_MSG_CommandHeader_t           CommandHeader; /**< \brief Command header */
    CFE_SB_WriteFileInfoCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_SB_WriteRoutingInfoCmd_t;

typedef struct CFE_SB_WritePipeInfoCmd
{
    CFE_MSG_CommandHeader_t           CommandHeader; /**< \brief Command header */
    CFE_SB_WriteFileInfoCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_SB_WritePipeInfoCmd_t;

typedef struct CFE_SB_WriteMapInfoCmd
{
    CFE_MSG_CommandHeader_t           CommandHeader; /**< \brief Command header */
    CFE_SB_WriteFileInfoCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_SB_WriteMapInfoCmd_t;

/*
 * Create a unique typedef for each of the commands that share this format.
 */
typedef struct CFE_SB_EnableRouteCmd
{
    CFE_MSG_CommandHeader_t   CommandHeader; /**< \brief Command header */
    CFE_SB_RouteCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_SB_EnableRouteCmd_t;

typedef struct CFE_SB_DisableRouteCmd
{
    CFE_MSG_CommandHeader_t   CommandHeader; /**< \brief Command header */
    CFE_SB_RouteCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_SB_DisableRouteCmd_t;

/****************************
**  SB Telemetry Formats   **
*****************************/

typedef struct CFE_SB_HousekeepingTlm
{
    CFE_MSG_TelemetryHeader_t        TelemetryHeader; /**< \brief Telemetry header */
    CFE_SB_HousekeepingTlm_Payload_t Payload;         /**< \brief Telemetry payload */
} CFE_SB_HousekeepingTlm_t;

typedef struct CFE_SB_StatsTlm
{
    CFE_MSG_TelemetryHeader_t TelemetryHeader; /**< \brief Telemetry header */
    CFE_SB_StatsTlm_Payload_t Payload;         /**< \brief Telemetry payload */
} CFE_SB_StatsTlm_t;

typedef struct CFE_SB_SingleSubscriptionTlm
{
    CFE_MSG_TelemetryHeader_t              TelemetryHeader; /**< \brief Telemetry header */
    CFE_SB_SingleSubscriptionTlm_Payload_t Payload;         /**< \brief Telemetry payload */
} CFE_SB_SingleSubscriptionTlm_t;

typedef struct CFE_SB_AllSubscriptionsTlm
{
    CFE_MSG_TelemetryHeader_t            TelemetryHeader; /**< \brief Telemetry header */
    CFE_SB_AllSubscriptionsTlm_Payload_t Payload;         /**< \brief Telemetry payload */
} CFE_SB_AllSubscriptionsTlm_t;

#endif
