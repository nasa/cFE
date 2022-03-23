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
 *    This header file performs compile time checking for TBL configuration
 *    parameters.
 *
 */

#ifndef CFE_TBL_VERIFY_H
#define CFE_TBL_VERIFY_H

#include "cfe_platform_cfg.h"

#if (2 * CFE_PLATFORM_TBL_MAX_DBL_TABLE_SIZE) > CFE_PLATFORM_TBL_BUF_MEMORY_BYTES
#error Two buffers of size CFE_PLATFORM_TBL_MAX_DBL_TABLE_SIZE cannot be greater than memory pool size of CFE_PLATFORM_TBL_BUF_MEMORY_BYTES!
#endif

#if ((CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS + 1) * CFE_PLATFORM_TBL_MAX_SNGL_TABLE_SIZE) > \
    CFE_PLATFORM_TBL_BUF_MEMORY_BYTES
#error Shared buffers and table of size CFE_PLATFORM_TBL_MAX_SNGL_TABLE_SIZE cannot be greater than memory pool size of CFE_PLATFORM_TBL_BUF_MEMORY_BYTES!
#endif

#if CFE_PLATFORM_TBL_MAX_NUM_HANDLES < CFE_PLATFORM_TBL_MAX_NUM_TABLES
#error CFE_PLATFORM_TBL_MAX_NUM_HANDLES cannot be set less than CFE_PLATFORM_TBL_MAX_NUM_TABLES!
#endif

#if CFE_PLATFORM_TBL_MAX_CRITICAL_TABLES > CFE_PLATFORM_ES_CDS_MAX_NUM_ENTRIES
#error CFE_PLATFORM_TBL_MAX_CRITICAL_TABLES cannot be greater than CFE_PLATFORM_ES_CDS_MAX_NUM_ENTRIES!
#endif

/*
** Any modifications to the "_VALID_" limits defined below must match
** source code changes made to the function CFE_TBL_ReadHeaders() in
** the file "cfe_tbl_internal.c".
*/
#if CFE_PLATFORM_TBL_VALID_SCID_COUNT < 0
#error CFE_PLATFORM_TBL_VALID_SCID_COUNT must be greater than or equal to zero
#elif CFE_PLATFORM_TBL_VALID_SCID_COUNT > 2
#error CFE_PLATFORM_TBL_VALID_SCID_COUNT must be less than or equal to 2
#endif

#if CFE_PLATFORM_TBL_VALID_PRID_COUNT < 0
#error CFE_PLATFORM_TBL_VALID_PRID_COUNT must be greater than or equal to zero
#elif CFE_PLATFORM_TBL_VALID_PRID_COUNT > 4
#error CFE_PLATFORM_TBL_VALID_PRID_COUNT must be less than or equal to 4
#endif

/*
** Validate task stack size...
*/
#if CFE_PLATFORM_TBL_START_TASK_STACK_SIZE < 2048
#error CFE_PLATFORM_TBL_START_TASK_STACK_SIZE must be greater than or equal to 2048
#endif

/*
 * For configuration values that should be multiples of 4
 * as noted in the documentation, this confirms that they are.
 */
#if ((CFE_MISSION_TBL_MAX_NAME_LENGTH % 4) != 0)
#error CFE_MISSION_TBL_MAX_NAME_LENGTH must be a multiple of 4
#endif
#if ((CFE_MISSION_TBL_MAX_FULL_NAME_LEN % 4) != 0)
#error CFE_MISSION_TBL_MAX_FULL_NAME_LEN must be a multiple of 4
#endif

#endif /* CFE_TBL_VERIFY_H */
