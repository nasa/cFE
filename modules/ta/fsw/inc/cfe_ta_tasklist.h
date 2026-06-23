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
 * Main header and specification for the CFE Task Affinity (TA) task list
 */

#ifndef CFE_TA_TASKLIST_H
#define CFE_TA_TASKLIST_H

/********************************** Include Files  ************************************/
#include <stdio.h>
#include <string.h>
#include "osapi.h"
#include "cfe.h"

/**
 * @brief Label definitions associated with CFE_TA_Mask_Type
 */
typedef enum
{
    /** @brief Use O/S Default Affinity */
    CFE_TA_OsDefault = 0,

    /** @brief Use User Specified Affinity Mask */
    CFE_TA_Mask = 1,

    /** @brief Was not found in list so use O/S Default Affinity */
    CFE_TA_Added = 2
} CFE_TA_Mask_Type;

/**
 * @brief Label definitions associated with CFE_TA_Status
 */
typedef enum
{
    /** @brief Status is Unknown */
    CFE_TA_Status_Unknown = 0,

    /** @brief Status is Ok */
    CFE_TA_Status_Ok = 1,

    /** @brief Task not found / Error */
    CFE_TA_Status_Error = 2
} CFE_TA_Status;

/**
 * \brief Tracked Task Affinity State
 */
typedef struct
{
    char             TaskName[CFE_MISSION_MAX_API_LEN];
    OS_cpuset_t      CpuMask;
    OS_cpuset_t      CpuFromOS;
    CFE_TA_Mask_Type Type;
    CFE_TA_Status    Status;
} CFE_TA_CpuTaskAffinity_t;

#endif /* CFE_TA_TASKLIST_H */
