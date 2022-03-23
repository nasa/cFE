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
 *      This header file performs compile time checking for SB configuration
 *      parameters.
 *
 * Author:   R.McGraw/SSI
 *
 */

#ifndef CFE_SB_VERIFY_H
#define CFE_SB_VERIFY_H

#include <stdint.h>

#if CFE_PLATFORM_SB_MAX_MSG_IDS < 1
#error CFE_PLATFORM_SB_MAX_MSG_IDS cannot be less than 1!
#endif

#if CFE_PLATFORM_SB_MAX_PIPES < 1
#error CFE_PLATFORM_SB_MAX_PIPES cannot be less than 1!
#endif

#if CFE_PLATFORM_SB_MAX_PIPES > OS_MAX_QUEUES
#error CFE_PLATFORM_SB_MAX_PIPES cannot be greater than OS_MAX_QUEUES!
#endif

#if CFE_PLATFORM_SB_MAX_DEST_PER_PKT < 1
#error CFE_PLATFORM_SB_MAX_DEST_PER_PKT cannot be less than 1!
#endif

#if CFE_PLATFORM_SB_HIGHEST_VALID_MSGID < 1
#error CFE_PLATFORM_SB_HIGHEST_VALID_MSGID cannot be less than 1!
#endif

#if CFE_PLATFORM_SB_HIGHEST_VALID_MSGID > 0xFFFFFFFE
#error CFE_PLATFORM_SB_HIGHEST_VALID_MSGID cannot be > 0xFFFFFFFE
#endif

#if CFE_PLATFORM_SB_BUF_MEMORY_BYTES < 512
#error CFE_PLATFORM_SB_BUF_MEMORY_BYTES cannot be less than 512 bytes!
#endif

#if CFE_PLATFORM_SB_BUF_MEMORY_BYTES > UINT32_MAX
#error CFE_PLATFORM_SB_BUF_MEMORY_BYTES cannot be greater than UINT32_MAX (4 Gigabytes)!
#endif

/*
 * Legacy time formats no longer supported in core cFE, this will pass
 * if default is selected or if both defines are removed
 */
#if (CFE_MISSION_SB_PACKET_TIME_FORMAT != CFE_MISSION_SB_TIME_32_16_SUBS)
#error Legacy CFE_MISSION_SB_PACKET_TIME_FORMAT implementations no longer supported in core
#endif

#if CFE_MISSION_SB_MAX_SB_MSG_SIZE < 6
#error CFE_MISSION_SB_MAX_SB_MSG_SIZE cannot be less than 6 (CCSDS Primary Hdr Size)!
#endif

/*
**  SB Memory Pool Block Sizes
*/
#if CFE_PLATFORM_SB_MAX_BLOCK_SIZE < CFE_MISSION_SB_MAX_SB_MSG_SIZE
#error CFE_PLATFORM_SB_MAX_BLOCK_SIZE must be > or = to CFE_MISSION_SB_MAX_SB_MSG_SIZE!
#endif

#if CFE_PLATFORM_SB_MEM_BLOCK_SIZE_01 > CFE_PLATFORM_SB_MEM_BLOCK_SIZE_02
#error CFE_PLATFORM_SB_MEM_BLOCK_SIZE_01 must be less than CFE_PLATFORM_SB_MEM_BLOCK_SIZE_02
#endif

#if CFE_PLATFORM_SB_MEM_BLOCK_SIZE_02 > CFE_PLATFORM_SB_MEM_BLOCK_SIZE_03
#error CFE_PLATFORM_SB_MEM_BLOCK_SIZE_02 must be less than CFE_PLATFORM_SB_MEM_BLOCK_SIZE_03
#endif

#if CFE_PLATFORM_SB_MEM_BLOCK_SIZE_03 > CFE_PLATFORM_SB_MEM_BLOCK_SIZE_04
#error CFE_PLATFORM_SB_MEM_BLOCK_SIZE_03 must be less than CFE_PLATFORM_SB_MEM_BLOCK_SIZE_04
#endif

#if CFE_PLATFORM_SB_MEM_BLOCK_SIZE_04 > CFE_PLATFORM_SB_MEM_BLOCK_SIZE_05
#error CFE_PLATFORM_SB_MEM_BLOCK_SIZE_04 must be less than CFE_PLATFORM_SB_MEM_BLOCK_SIZE_05
#endif

#if CFE_PLATFORM_SB_MEM_BLOCK_SIZE_05 > CFE_PLATFORM_SB_MEM_BLOCK_SIZE_06
#error CFE_PLATFORM_SB_MEM_BLOCK_SIZE_05 must be less than CFE_PLATFORM_SB_MEM_BLOCK_SIZE_06
#endif

#if CFE_PLATFORM_SB_MEM_BLOCK_SIZE_06 > CFE_PLATFORM_SB_MEM_BLOCK_SIZE_07
#error CFE_PLATFORM_SB_MEM_BLOCK_SIZE_06 must be less than CFE_PLATFORM_SB_MEM_BLOCK_SIZE_07
#endif

#if CFE_PLATFORM_SB_MEM_BLOCK_SIZE_07 > CFE_PLATFORM_SB_MEM_BLOCK_SIZE_08
#error CFE_PLATFORM_SB_MEM_BLOCK_SIZE_07 must be less than CFE_PLATFORM_SB_MEM_BLOCK_SIZE_08
#endif

#if CFE_PLATFORM_SB_MEM_BLOCK_SIZE_08 > CFE_PLATFORM_SB_MEM_BLOCK_SIZE_09
#error CFE_PLATFORM_SB_MEM_BLOCK_SIZE_08 must be less than CFE_PLATFORM_SB_MEM_BLOCK_SIZE_09
#endif

#if CFE_PLATFORM_SB_MEM_BLOCK_SIZE_09 > CFE_PLATFORM_SB_MEM_BLOCK_SIZE_10
#error CFE_PLATFORM_SB_MEM_BLOCK_SIZE_09 must be less than CFE_PLATFORM_SB_MEM_BLOCK_SIZE_10
#endif

#if CFE_PLATFORM_SB_MEM_BLOCK_SIZE_10 > CFE_PLATFORM_SB_MEM_BLOCK_SIZE_11
#error CFE_PLATFORM_SB_MEM_BLOCK_SIZE_10 must be less than CFE_PLATFORM_SB_MEM_BLOCK_SIZE_11
#endif

#if CFE_PLATFORM_SB_MEM_BLOCK_SIZE_11 > CFE_PLATFORM_SB_MEM_BLOCK_SIZE_12
#error CFE_PLATFORM_SB_MEM_BLOCK_SIZE_11 must be less than CFE_PLATFORM_SB_MEM_BLOCK_SIZE_12
#endif

#if CFE_PLATFORM_SB_MEM_BLOCK_SIZE_12 > CFE_PLATFORM_SB_MEM_BLOCK_SIZE_13
#error CFE_PLATFORM_SB_MEM_BLOCK_SIZE_12 must be less than CFE_PLATFORM_SB_MEM_BLOCK_SIZE_13
#endif

#if CFE_PLATFORM_SB_MEM_BLOCK_SIZE_13 > CFE_PLATFORM_SB_MEM_BLOCK_SIZE_14
#error CFE_PLATFORM_SB_MEM_BLOCK_SIZE_13 must be less than CFE_PLATFORM_SB_MEM_BLOCK_SIZE_14
#endif

#if CFE_PLATFORM_SB_MEM_BLOCK_SIZE_14 > CFE_PLATFORM_SB_MEM_BLOCK_SIZE_15
#error CFE_PLATFORM_SB_MEM_BLOCK_SIZE_14 must be less than CFE_PLATFORM_SB_MEM_BLOCK_SIZE_15
#endif

#if CFE_PLATFORM_SB_MEM_BLOCK_SIZE_15 > CFE_PLATFORM_SB_MEM_BLOCK_SIZE_16
#error CFE_PLATFORM_SB_MEM_BLOCK_SIZE_15 must be less than CFE_PLATFORM_SB_MEM_BLOCK_SIZE_16
#endif

#if CFE_PLATFORM_SB_MEM_BLOCK_SIZE_16 >= CFE_PLATFORM_SB_MAX_BLOCK_SIZE
#error CFE_PLATFORM_SB_MEM_BLOCK_SIZE_16 must be less than CFE_PLATFORM_SB_MAX_BLOCK_SIZE
#endif

#if CFE_PLATFORM_SB_DEFAULT_MSG_LIMIT < 4
#error CFE_PLATFORM_SB_DEFAULT_MSG_LIMIT cannot be less than 4!
#endif

#if CFE_PLATFORM_SB_DEFAULT_MSG_LIMIT > 65535
#error CFE_PLATFORM_SB_DEFAULT_MSG_LIMIT cannot be greater than 65535!
#endif

/*
** Validate task stack size...
*/
#if CFE_PLATFORM_SB_START_TASK_STACK_SIZE < 2048
#error CFE_PLATFORM_SB_START_TASK_STACK_SIZE must be greater than or equal to 2048
#endif

#endif /* CFE_SB_VERIFY_H */
