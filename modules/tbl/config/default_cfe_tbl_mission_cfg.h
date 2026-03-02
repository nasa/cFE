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
 * CFE Event Services (CFE_TBL) Application Mission Configuration Header File
 *
 * This is a compatibility header for the "mission_cfg.h" file that has
 * traditionally provided public config definitions for each CFS app.
 *
 * @note This file may be overridden/superceded by mission-provided definitions
 * either by overriding this header or by generating definitions from a command/data
 * dictionary tool.
 */
#ifndef DEFAULT_CFE_TBL_MISSION_CFG_H
#define DEFAULT_CFE_TBL_MISSION_CFG_H

#include "cfe_tbl_interface_cfg_values.h"

/**
**  \cfetblcfg Maximum Table Name Length
**
**  \par Description:
**      Indicates the maximum length (in characters) of the table name
**      ('TblName') portion of a Full Table Name of the following
**      form: "ApplicationName.TblName"
**
**      This length does not need to include an extra character for NULL termination.
**
**  \par Limits
**       This value should be kept as a multiple of 4, to maintain alignment of
**       any possible neighboring fields without implicit padding.
*/
#define CFE_MISSION_TBL_MAX_NAME_LENGTH         CFE_MISSION_TBL_CFGVAL(MAX_NAME_LENGTH)
#define DEFAULT_CFE_MISSION_TBL_MAX_NAME_LENGTH 16

/**
**  \cfetblcfg Maximum Length of Full Table Name in messages
**
**  \par Description:
**      Indicates the maximum length (in characters) of the entire table name
**      within software bus messages, in "AppName.TableName" notation.
**
**      This affects the layout of command/telemetry messages but does not affect run
**      time behavior or internal allocation.
**
**  \par Limits
**       All CPUs within the same SB domain (mission) must share the same definition
**       Note this affects the size of messages, so it must not cause any message
**       to exceed the max length.
**
**       This value should be kept as a multiple of 4, to maintain alignment of
**       any possible neighboring fields without implicit padding.
*/
#define CFE_MISSION_TBL_MAX_FULL_NAME_LEN CFE_MISSION_TBL_CFGVAL(MAX_FULL_NAME_LEN)

/* Default value calculated as: (CFE_MISSION_TBL_MAX_NAME_LENGTH + CFE_MISSION_MAX_API_LEN + 4) */
#define DEFAULT_CFE_MISSION_TBL_MAX_FULL_NAME_LEN 40

#endif
