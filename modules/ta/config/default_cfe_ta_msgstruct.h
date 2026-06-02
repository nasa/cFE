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
 *  Purpose:
 *  Task Affinity (TA) Command and Telemetry packet definition file.
 *
 *  References:
 *     Flight Software Branch C Coding Standard Version 1.0a
 *     cFE Flight Software Application Developers Guide
 *
 *  Notes:
 *
 */
#ifndef DEFAULT_CFE_TA_MSGSTRUCT_H
#define DEFAULT_CFE_TA_MSGSTRUCT_H

#include "cfe_ta_msgdefs.h"
#include "cfe_ta_interface_cfg.h"
#include "cfe_msg_hdr.h"

/* Manual C Structs (Used ONLY when EDS is disabled) */
typedef struct CFE_TA_NoopCmd
{
    CFE_MSG_CommandHeader_t CommandHeader;
} CFE_TA_NoopCmd_t;

typedef struct CFE_TA_ResetCountersCmd
{
    CFE_MSG_CommandHeader_t CommandHeader;
} CFE_TA_ResetCountersCmd_t;

typedef struct CFE_TA_SendHkCmd
{
    CFE_MSG_CommandHeader_t CommandHeader;
} CFE_TA_SendHkCmd_t;

typedef struct CFE_TA_QueryAllTasksCmd
{
    CFE_MSG_CommandHeader_t           CommandHeader;
    CFE_TA_QueryAllTasksCmd_Payload_t Payload;
} CFE_TA_QueryAllTasksCmd_t;

typedef struct CFE_TA_SetTaskAffinityCmd
{
    CFE_MSG_CommandHeader_t             CommandHeader;
    CFE_TA_SetTaskAffinityCmd_Payload_t Payload;
} CFE_TA_SetTaskAffinityCmd_t;

typedef struct CFE_TA_GetTaskAffinityCmd
{
    CFE_MSG_CommandHeader_t             CommandHeader;
    CFE_TA_GetTaskAffinityCmd_Payload_t Payload;
} CFE_TA_GetTaskAffinityCmd_t;

typedef struct CFE_TA_HousekeepingTlm
{
    CFE_MSG_TelemetryHeader_t        TelemetryHeader;
    CFE_TA_HousekeepingTlm_Payload_t Payload;
} CFE_TA_HousekeepingTlm_t;

#endif /* DEFAULT_CFE_TA_MSGSTRUCT_H */
