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
 *  cFE Executive Services (TIME) Command and Telemetry packet definition file.
 *
 *  References:
 *     Flight Software Branch C Coding Standard Version 1.0a
 *     cFE Flight Software Application Developers Guide
 *
 *  Notes:
 *
 */
#ifndef CFE_TIME_MSGSTRUCT_H
#define CFE_TIME_MSGSTRUCT_H

/************************************************************************
 * Includes
 ************************************************************************/
#include "cfe_time_msgdefs.h"
#include "cfe_msg_hdr.h"

/*
 * A separate typedef for each of the commands that share this definition
 * This follows the convention for command handler prototypes and allows
 * each one to independently evolve as necessary.
 */
typedef struct CFE_TIME_NoopCmd
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command header */
} CFE_TIME_NoopCmd_t;

typedef struct CFE_TIME_ResetCountersCmd
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command header */
} CFE_TIME_ResetCountersCmd_t;

typedef struct CFE_TIME_SendDiagnosticCmd
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command header */
} CFE_TIME_SendDiagnosticCmd_t;

typedef struct CFE_TIME_OneHzCmd
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command header */
} CFE_TIME_OneHzCmd_t;

typedef struct CFE_TIME_ToneSignalCmd
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command header */
} CFE_TIME_ToneSignalCmd_t;

typedef struct CFE_TIME_FakeToneCmd
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command header */
} CFE_TIME_FakeToneCmd_t;

typedef struct CFE_TIME_SendHkCmd
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command header */
} CFE_TIME_SendHkCmd_t;

/**
 * \brief Set leap seconds command
 */
typedef struct CFE_TIME_SetLeapSecondsCmd
{
    CFE_MSG_CommandHeader_t     CommandHeader; /**< \brief Command header */
    CFE_TIME_LeapsCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_TIME_SetLeapSecondsCmd_t;

/**
 * \brief Set clock state command
 */
typedef struct CFE_TIME_SetStateCmd
{
    CFE_MSG_CommandHeader_t     CommandHeader; /**< \brief Command header */
    CFE_TIME_StateCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_TIME_SetStateCmd_t;

/**
 * \brief Set time data source command
 */
typedef struct CFE_TIME_SetSourceCmd
{
    CFE_MSG_CommandHeader_t      CommandHeader; /**< \brief Command header */
    CFE_TIME_SourceCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_TIME_SetSourceCmd_t;

/**
 * \brief Set tone signal source command
 */
typedef struct CFE_TIME_SetSignalCmd
{
    CFE_MSG_CommandHeader_t      CommandHeader; /**< \brief Command header */
    CFE_TIME_SignalCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_TIME_SetSignalCmd_t;

/*
 * A separate typedef for each of the commands that share this definition
 * This follows the convention for command handler prototypes and allows
 * each one to independently evolve as necessary.
 */
typedef struct CFE_TIME_AddDelayCmd
{
    CFE_MSG_CommandHeader_t    CommandHeader; /**< \brief Command header */
    CFE_TIME_TimeCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_TIME_AddDelayCmd_t;

typedef struct CFE_TIME_SubDelayCmd
{
    CFE_MSG_CommandHeader_t    CommandHeader; /**< \brief Command header */
    CFE_TIME_TimeCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_TIME_SubDelayCmd_t;

typedef struct CFE_TIME_SetMETCmd
{
    CFE_MSG_CommandHeader_t    CommandHeader; /**< \brief Command header */
    CFE_TIME_TimeCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_TIME_SetMETCmd_t;

typedef struct CFE_TIME_SetSTCFCmd
{
    CFE_MSG_CommandHeader_t    CommandHeader; /**< \brief Command header */
    CFE_TIME_TimeCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_TIME_SetSTCFCmd_t;

typedef struct CFE_TIME_AddAdjustCmd
{
    CFE_MSG_CommandHeader_t    CommandHeader; /**< \brief Command header */
    CFE_TIME_TimeCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_TIME_AddAdjustCmd_t;

typedef struct CFE_TIME_SubAdjustCmd
{
    CFE_MSG_CommandHeader_t    CommandHeader; /**< \brief Command header */
    CFE_TIME_TimeCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_TIME_SubAdjustCmd_t;

typedef struct CFE_TIME_SetTimeCmd
{
    CFE_MSG_CommandHeader_t    CommandHeader; /**< \brief Command header */
    CFE_TIME_TimeCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_TIME_SetTimeCmd_t;

/*
 * A separate typedef for each of the commands that share this definition
 * This follows the convention for command handler prototypes and allows
 * each one to independently evolve as necessary.
 */
typedef struct CFE_TIME_AddOneHzAdjustmentCmd
{
    CFE_MSG_CommandHeader_t               CommandHeader; /**< \brief Command header */
    CFE_TIME_OneHzAdjustmentCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_TIME_AddOneHzAdjustmentCmd_t;

typedef struct CFE_TIME_SubOneHzAdjustmentCmd
{
    CFE_MSG_CommandHeader_t               CommandHeader; /**< \brief Command header */
    CFE_TIME_OneHzAdjustmentCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_TIME_SubOneHzAdjustmentCmd_t;

/**
 * \brief Time at tone data command
 */
typedef struct CFE_TIME_ToneDataCmd
{
    CFE_MSG_CommandHeader_t        CommandHeader; /**< \brief Command header */
    CFE_TIME_ToneDataCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_TIME_ToneDataCmd_t;

/*************************************************************************/

typedef struct CFE_TIME_HousekeepingTlm
{
    CFE_MSG_TelemetryHeader_t          TelemetryHeader; /**< \brief Telemetry header */
    CFE_TIME_HousekeepingTlm_Payload_t Payload;         /**< \brief Telemetry payload */
} CFE_TIME_HousekeepingTlm_t;

typedef struct CFE_TIME_DiagnosticTlm
{
    CFE_MSG_TelemetryHeader_t        TelemetryHeader; /**< \brief Telemetry header */
    CFE_TIME_DiagnosticTlm_Payload_t Payload;         /**< \brief Telemetry payload */
} CFE_TIME_DiagnosticTlm_t;

#endif
