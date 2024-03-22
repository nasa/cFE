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
 * Purpose:
 *   This header file performs compile time checking for ES configuration
 *   parameters.
 *
 *  References:
 *     Flight Software Branch C Coding Standard Version 1.0a
 *     cFE Flight Software Application Developers Guide
 *
 *  Notes:
 *     The upper limits are somewhat arbitrary right now.
 *
 */

#ifndef CFE_ES_VERIFY_H
#define CFE_ES_VERIFY_H

#include <stdint.h>

#if CFE_PLATFORM_ES_MAX_APPLICATIONS < 6
#error CFE_PLATFORM_ES_MAX_APPLICATIONS cannot be less than 6!
#endif

#if CFE_PLATFORM_ES_MAX_LIBRARIES < 1
#error CFE_PLATFORM_ES_MAX_LIBRARIES cannot be less than 1!
#endif

#if CFE_PLATFORM_ES_ER_LOG_ENTRIES < 1
#error CFE_PLATFORM_ES_ER_LOG_ENTRIES cannot be less than 1!
#endif

#if CFE_PLATFORM_ES_SYSTEM_LOG_SIZE < 512
#error CFE_PLATFORM_ES_SYSTEM_LOG_SIZE cannot be less than 512 Bytes!
#endif

#if CFE_PLATFORM_ES_DEFAULT_STACK_SIZE < 2048
#error CFE_PLATFORM_ES_DEFAULT_STACK_SIZE cannot be less than 2048 Bytes!
#endif

/*
** Number of entries in the ES Object table ( The table that controls core cFE startup )
*/
#if CFE_PLATFORM_ES_OBJECT_TABLE_SIZE < 15
#error CFE_PLATFORM_ES_OBJECT_TABLE_SIZE cannot be less than 15!
#endif

/*
** ES Application Control Scan Rate.
*/
#if CFE_PLATFORM_ES_APP_SCAN_RATE < 100
#error CFE_PLATFORM_ES_APP_SCAN_RATE cannot be less than 100 milliseconds!
#elif CFE_PLATFORM_ES_APP_SCAN_RATE > 20000
#error CFE_PLATFORM_ES_APP_SCAN_RATE cannot be greater than 20 seconds!
#endif

/*
** ES Application Kill Timeout
*/
#if CFE_PLATFORM_ES_APP_KILL_TIMEOUT < 1
#error CFE_PLATFORM_ES_APP_KILL_TIMEOUT cannot be less than 1!
#elif CFE_PLATFORM_ES_APP_KILL_TIMEOUT > 100
#error CFE_PLATFORM_ES_APP_KILL_TIMEOUT cannot be greater than 100!
#endif

/*
** ES / cFE RAM disk parameters
*/
#if CFE_PLATFORM_ES_RAM_DISK_SECTOR_SIZE < 128
#error CFE_PLATFORM_ES_RAM_DISK_SECTOR_SIZE cannot be less than 128!
#endif

#if CFE_PLATFORM_ES_RAM_DISK_NUM_SECTORS < 128
#error CFE_PLATFORM_ES_RAM_DISK_NUM_SECTORS cannot be less than 128!
#endif

#if CFE_PLATFORM_ES_RAM_DISK_PERCENT_RESERVED < 0
#error CFE_PLATFORM_ES_RAM_DISK_PERCENT_RESERVED cannot be less than 0!
#elif CFE_PLATFORM_ES_RAM_DISK_PERCENT_RESERVED > 75
#error CFE_PLATFORM_ES_RAM_DISK_PERCENT_RESERVED cannot be greater than 75!
#endif

/*
** Critical data store size
*/
#if CFE_PLATFORM_ES_CDS_SIZE < (8 * 1024)
#error CFE_PLATFORM_ES_CDS_SIZE cannot be less than 8Kbytes!
#elif CFE_PLATFORM_ES_CDS_SIZE > UINT32_MAX
#error CFE_PLATFORM_ES_CDS_SIZE cannot be greater than UINT32_MAX (4 Gigabytes)!
#endif

/*
** User Reserved Memory Size.
*/
#if CFE_PLATFORM_ES_USER_RESERVED_SIZE < (1 * 1024)
#error CFE_PLATFORM_ES_USER_RESERVED_SIZE cannot be less than 1Kbytes!
#elif CFE_PLATFORM_ES_USER_RESERVED_SIZE > UINT32_MAX
#error CFE_PLATFORM_ES_USER_RESERVED_SIZE cannot be greater than UINT32_MAX (4 Gigabytes)!
#endif

/*
** Default System Log Mode following Power On Reset
*/
#if CFE_PLATFORM_ES_DEFAULT_POR_SYSLOG_MODE < 0
#error CFE_PLATFORM_ES_DEFAULT_POR_SYSLOG_MODE cannot be less than 0!
#elif CFE_PLATFORM_ES_DEFAULT_POR_SYSLOG_MODE > 1
#error CFE_PLATFORM_ES_DEFAULT_POR_SYSLOG_MODE cannot be greater than 1!
#endif

/*
** Default System Log Mode following Processor Reset
*/
#if CFE_PLATFORM_ES_DEFAULT_PR_SYSLOG_MODE < 0
#error CFE_PLATFORM_ES_DEFAULT_PR_SYSLOG_MODE cannot be less than 0!
#elif CFE_PLATFORM_ES_DEFAULT_PR_SYSLOG_MODE > 1
#error CFE_PLATFORM_ES_DEFAULT_PR_SYSLOG_MODE cannot be greater than 1!
#endif

/*
** Maximum number of performance IDs
*/
#if CFE_MISSION_ES_PERF_MAX_IDS < 32
#error CFE_MISSION_ES_PERF_MAX_IDS cannot be less than 32!
#endif

/*
** Performance data buffer size
*/
#if CFE_PLATFORM_ES_PERF_DATA_BUFFER_SIZE < 1025
#error CFE_PLATFORM_ES_PERF_DATA_BUFFER_SIZE cannot be less than 1025 entries!
#endif

/*
** Maximum number of Registered CDS blocks
*/
#if CFE_PLATFORM_ES_CDS_MAX_NUM_ENTRIES < 8
#error CFE_PLATFORM_ES_CDS_MAX_NUM_ENTRIES cannot be less than 8!
#endif

/*
** Maximum number of processor resets before a power-on
*/
#if CFE_PLATFORM_ES_MAX_PROCESSOR_RESETS < 0
#error CFE_PLATFORM_ES_MAX_PROCESSOR_RESETS cannot be less than 0!
#endif

/*
**  Alignment of ES memory pool
*/
#if CFE_PLATFORM_ES_MEMPOOL_ALIGN_SIZE_MIN <= 0
#error CFE_PLATFORM_ES_MEMPOOL_ALIGN_SIZE_MIN cannot be less than or equal to 0!
#elif (CFE_PLATFORM_ES_MEMPOOL_ALIGN_SIZE_MIN & (CFE_PLATFORM_ES_MEMPOOL_ALIGN_SIZE_MIN - 1)) != 0
#error CFE_PLATFORM_ES_MEMPOOL_ALIGN_SIZE_MIN must be a power of 2!
#endif

/*
**  Intermediate ES Memory Pool Block Sizes
*/
#if CFE_PLATFORM_ES_MAX_BLOCK_SIZE < CFE_MISSION_SB_MAX_SB_MSG_SIZE
#error CFE_PLATFORM_ES_MAX_BLOCK_SIZE must be equal to or larger than CFE_MISSION_SB_MAX_SB_MSG_SIZE!
#endif

#if CFE_PLATFORM_ES_MAX_BLOCK_SIZE < CFE_PLATFORM_TBL_MAX_SNGL_TABLE_SIZE
#error CFE_PLATFORM_ES_MAX_BLOCK_SIZE must be equal to or larger than CFE_PLATFORM_TBL_MAX_SNGL_TABLE_SIZE!
#endif

#if CFE_PLATFORM_ES_MAX_BLOCK_SIZE < CFE_PLATFORM_TBL_MAX_DBL_TABLE_SIZE
#error CFE_PLATFORM_ES_MAX_BLOCK_SIZE must be equal to or larger than CFE_PLATFORM_TBL_MAX_DBL_TABLE_SIZE!
#endif

#if CFE_PLATFORM_ES_MEM_BLOCK_SIZE_01 > CFE_PLATFORM_ES_MEM_BLOCK_SIZE_02
#error CFE_PLATFORM_ES_MEM_BLOCK_SIZE_01 must be less than CFE_PLATFORM_ES_MEM_BLOCK_SIZE_02
#endif

#if CFE_PLATFORM_ES_MEM_BLOCK_SIZE_02 > CFE_PLATFORM_ES_MEM_BLOCK_SIZE_03
#error CFE_PLATFORM_ES_MEM_BLOCK_SIZE_02 must be less than CFE_PLATFORM_ES_MEM_BLOCK_SIZE_03
#endif

#if CFE_PLATFORM_ES_MEM_BLOCK_SIZE_03 > CFE_PLATFORM_ES_MEM_BLOCK_SIZE_04
#error CFE_PLATFORM_ES_MEM_BLOCK_SIZE_03 must be less than CFE_PLATFORM_ES_MEM_BLOCK_SIZE_04
#endif

#if CFE_PLATFORM_ES_MEM_BLOCK_SIZE_04 > CFE_PLATFORM_ES_MEM_BLOCK_SIZE_05
#error CFE_PLATFORM_ES_MEM_BLOCK_SIZE_04 must be less than CFE_PLATFORM_ES_MEM_BLOCK_SIZE_05
#endif

#if CFE_PLATFORM_ES_MEM_BLOCK_SIZE_05 > CFE_PLATFORM_ES_MEM_BLOCK_SIZE_06
#error CFE_PLATFORM_ES_MEM_BLOCK_SIZE_05 must be less than CFE_PLATFORM_ES_MEM_BLOCK_SIZE_06
#endif

#if CFE_PLATFORM_ES_MEM_BLOCK_SIZE_06 > CFE_PLATFORM_ES_MEM_BLOCK_SIZE_07
#error CFE_PLATFORM_ES_MEM_BLOCK_SIZE_06 must be less than CFE_PLATFORM_ES_MEM_BLOCK_SIZE_07
#endif

#if CFE_PLATFORM_ES_MEM_BLOCK_SIZE_07 > CFE_PLATFORM_ES_MEM_BLOCK_SIZE_08
#error CFE_PLATFORM_ES_MEM_BLOCK_SIZE_07 must be less than CFE_PLATFORM_ES_MEM_BLOCK_SIZE_08
#endif

#if CFE_PLATFORM_ES_MEM_BLOCK_SIZE_08 > CFE_PLATFORM_ES_MEM_BLOCK_SIZE_09
#error CFE_PLATFORM_ES_MEM_BLOCK_SIZE_08 must be less than CFE_PLATFORM_ES_MEM_BLOCK_SIZE_09
#endif

#if CFE_PLATFORM_ES_MEM_BLOCK_SIZE_09 > CFE_PLATFORM_ES_MEM_BLOCK_SIZE_10
#error CFE_PLATFORM_ES_MEM_BLOCK_SIZE_09 must be less than CFE_PLATFORM_ES_MEM_BLOCK_SIZE_10
#endif

#if CFE_PLATFORM_ES_MEM_BLOCK_SIZE_10 > CFE_PLATFORM_ES_MEM_BLOCK_SIZE_11
#error CFE_PLATFORM_ES_MEM_BLOCK_SIZE_10 must be less than CFE_PLATFORM_ES_MEM_BLOCK_SIZE_11
#endif

#if CFE_PLATFORM_ES_MEM_BLOCK_SIZE_11 > CFE_PLATFORM_ES_MEM_BLOCK_SIZE_12
#error CFE_PLATFORM_ES_MEM_BLOCK_SIZE_11 must be less than CFE_PLATFORM_ES_MEM_BLOCK_SIZE_12
#endif

#if CFE_PLATFORM_ES_MEM_BLOCK_SIZE_12 > CFE_PLATFORM_ES_MEM_BLOCK_SIZE_13
#error CFE_PLATFORM_ES_MEM_BLOCK_SIZE_12 must be less than CFE_PLATFORM_ES_MEM_BLOCK_SIZE_13
#endif

#if CFE_PLATFORM_ES_MEM_BLOCK_SIZE_13 > CFE_PLATFORM_ES_MEM_BLOCK_SIZE_14
#error CFE_PLATFORM_ES_MEM_BLOCK_SIZE_13 must be less than CFE_PLATFORM_ES_MEM_BLOCK_SIZE_14
#endif

#if CFE_PLATFORM_ES_MEM_BLOCK_SIZE_14 > CFE_PLATFORM_ES_MEM_BLOCK_SIZE_15
#error CFE_PLATFORM_ES_MEM_BLOCK_SIZE_14 must be less than CFE_PLATFORM_ES_MEM_BLOCK_SIZE_15
#endif

#if CFE_PLATFORM_ES_MEM_BLOCK_SIZE_15 > CFE_PLATFORM_ES_MEM_BLOCK_SIZE_16
#error CFE_PLATFORM_ES_MEM_BLOCK_SIZE_15 must be less than CFE_PLATFORM_ES_MEM_BLOCK_SIZE_16
#endif

#if CFE_PLATFORM_ES_MEM_BLOCK_SIZE_16 > CFE_PLATFORM_ES_MAX_BLOCK_SIZE
#error CFE_PLATFORM_ES_MEM_BLOCK_SIZE_16 must be less than CFE_PLATFORM_ES_MAX_BLOCK_SIZE
#endif

/*
**  Intermediate ES Critical Data Store Memory Pool Block Sizes
*/
#if CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_01 > CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_02
#error CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_01 must be less than CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_02
#endif

#if CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_02 > CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_03
#error CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_02 must be less than CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_03
#endif

#if CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_03 > CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_04
#error CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_03 must be less than CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_04
#endif

#if CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_04 > CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_05
#error CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_04 must be less than CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_05
#endif

#if CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_05 > CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_06
#error CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_05 must be less than CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_06
#endif

#if CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_06 > CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_07
#error CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_06 must be less than CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_07
#endif

#if CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_07 > CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_08
#error CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_07 must be less than CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_08
#endif

#if CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_08 > CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_09
#error CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_08 must be less than CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_09
#endif

#if CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_09 > CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_10
#error CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_09 must be less than CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_10
#endif

#if CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_10 > CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_11
#error CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_10 must be less than CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_11
#endif

#if CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_11 > CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_12
#error CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_11 must be less than CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_12
#endif

#if CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_12 > CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_13
#error CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_12 must be less than CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_13
#endif

#if CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_13 > CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_14
#error CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_13 must be less than CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_14
#endif

#if CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_14 > CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_15
#error CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_14 must be less than CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_15
#endif

#if CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_15 > CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_16
#error CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_15 must be less than CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_16
#endif

#if CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_16 > CFE_PLATFORM_ES_CDS_MAX_BLOCK_SIZE
#error CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_16 must be less than CFE_PLATFORM_ES_CDS_MAX_BLOCK_SIZE
#endif

/*
** Validate task stack size...
*/
#if CFE_PLATFORM_ES_START_TASK_STACK_SIZE < 2048
#error CFE_PLATFORM_ES_START_TASK_STACK_SIZE must be greater than or equal to 2048
#endif

#if ((CFE_MISSION_MAX_API_LEN % 4) != 0)
#error CFE_MISSION_MAX_API_LEN must be a multiple of 4
#endif
#if ((CFE_MISSION_MAX_PATH_LEN % 4) != 0)
#error CFE_MISSION_MAX_PATH_LEN must be a multiple of 4
#endif
#if ((CFE_MISSION_MAX_FILE_LEN % 4) != 0)
#error CFE_MISSION_MAX_FILE_LEN must be a multiple of 4
#endif
#if ((CFE_MISSION_ES_CDS_MAX_NAME_LENGTH % 4) != 0)
#error CFE_MISSION_ES_CDS_MAX_NAME_LENGTH must be a multiple of 4
#endif
#if ((CFE_MISSION_ES_CDS_MAX_FULL_NAME_LEN % 4) != 0)
#error CFE_MISSION_ES_CDS_MAX_FULL_NAME_LEN must be a multiple of 4
#endif

#endif /* CFE_ES_VERIFY_H */
