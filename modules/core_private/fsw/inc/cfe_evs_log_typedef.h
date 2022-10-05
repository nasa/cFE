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
 * Definition of the CFE_EVS_Log structure type.
 * This was moved into its own header file since it is referenced by multiple CFE core apps.
 */

#ifndef CFE_EVS_LOG_TYPEDEF_H
#define CFE_EVS_LOG_TYPEDEF_H

#include "common_types.h"
#include "cfe_platform_cfg.h"

#include "cfe_evs_msg.h" /* Required for CFE_EVS_LongEventTlm_t definition */

/*
** \brief  EVS Log type definition. This is declared here so ES can include it
**  in the reset area structure
*/
typedef struct
{
    uint16                 Next;               /**< \brief Index of the next entry in the local event log */
    uint16                 LogCount;           /**< \brief Local Event Log counter */
    uint8                  LogFullFlag;        /**< \brief Local Event Log full flag */
    uint8                  LogMode;            /**< \brief Local Event Logging mode (overwrite/discard) */
    uint16                 LogOverflowCounter; /**< \brief Local Event Log overflow counter */
    CFE_EVS_LongEventTlm_t LogEntry[CFE_PLATFORM_EVS_LOG_MAX]; /**< \brief The actual Local Event Log entry */
} CFE_EVS_Log_t;

#endif /* CFE_EVS_LOG_TYPEDEF_H */
