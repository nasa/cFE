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
 * CFE Task Affinity (CFE_TA) Application Platform Configuration Header File
 *
 * This is a compatibility header for the "platform_cfg.h" file that has
 * traditionally provided both public and private config definitions
 * for each CFS app.
 *
 * These definitions are now provided in two separate files, one for
 * the public/mission scope and one for internal scope.
 *
 * @note This file may be overridden/superceded by mission-provided definitions
 * either by overriding this header or by generating definitions from a command/data
 * dictionary tool.
 */
#ifndef CFE_TA_PLATFORM_CFG_H
#define CFE_TA_PLATFORM_CFG_H

#include "cfe_ta_mission_cfg.h"
#include "cfe_ta_internal_cfg_values.h"

/**
**  \cfeescfg Define TA Task Priority
**
**  \par Description:
**       Defines the cFE_TA Task priority.
**
**  \par Limits
**       Not Applicable
*/
#define CFE_PLATFORM_TA_START_TASK_PRIORITY         CFE_PLATFORM_TA_CFGVAL(START_TASK_PRIORITY)
#define DEFAULT_CFE_PLATFORM_TA_START_TASK_PRIORITY 75

/**
**  \cfeescfg Define TA Task Stack Size
**
**  \par Description:
**       Defines the cFE_TA Task Stack Size
**
**  \par Limits
**       There is a lower limit of 2048 on this configuration parameter.  There
**       are no restrictions on the upper limit however, the maximum stack size
**       is system dependent and should be verified.  Most operating systems provide
**       tools for measuring the amount of stack used by a task during operation. It
**       is always a good idea to verify that no more than 1/2 of the stack is used.
*/
#define CFE_PLATFORM_TA_START_TASK_STACK_SIZE         CFE_PLATFORM_TA_CFGVAL(START_TASK_STACK_SIZE)
#define DEFAULT_CFE_PLATFORM_TA_START_TASK_STACK_SIZE 16384

#endif
