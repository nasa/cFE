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
 *   CFE Software Bus (CFE_SB) Application Private Config Definitions
 *
 * This provides default values for configurable items that are internal
 * to this module and do NOT affect the interface(s) of this module.  Changes
 * to items in this file only affect the local module and will be transparent
 * to external entities that are using the public interface(s).
 *
 * @note This file may be overridden/superceded by mission-provided definitions
 * either by overriding this header or by generating definitions from a command/data
 * dictionary tool.
 */
#ifndef CFE_SB_INTERNAL_CFG_H
#define CFE_SB_INTERNAL_CFG_H

/**
**  \cfesbcfg Maximum Number of Unique Message IDs SB Routing Table can hold
**
**  \par Description:
**       Dictates the maximum number of unique MsgIds the SB routing table will hold.
**       This constant has a direct effect on the size of SB's tables and arrays.
**       Keeping this count as low as possible will save memory.
**       To see the run-time, high-water mark and the current utilization figures
**       regarding this parameter, send an SB command to 'Send Statistics Pkt'.
**
**  \par Limits
**       This must be a power of two if software bus message routing hash implementation
**       is being used.  Lower than 64 will cause unit test failures, and
**       telemetry reporting is impacted below 32.  There is no hard
**       upper limit, but impacts memory footprint.  For software bus message routing
**       search implementation the number of msg ids subscribed to impacts performance.
**
*/
#define CFE_PLATFORM_SB_MAX_MSG_IDS 256

/**
**  \cfesbcfg Maximum Number of Unique Pipes SB Routing Table can hold
**
**  \par Description:
**       Dictates the maximum number of unique Pipes the SB routing table will hold.
**       This constant has a direct effect on the size of SB's tables and arrays.
**       Keeping this count as low as possible will save memory.
**       To see the run-time, high-water mark and the current utilization figures
**       regarding this parameter, send an SB command to 'Send Statistics Pkt'.
**
**  \par Limits
**       This parameter has a lower limit of 1.  This parameter must also be less than
**       or equal to OS_MAX_QUEUES.
**
*/
#define CFE_PLATFORM_SB_MAX_PIPES 64

/**
**  \cfesbcfg Maximum Number of unique local destinations a single MsgId can have
**
**  \par Description:
**       Dictates the maximum number of unique local destinations a single MsgId can
**       have.
**
**  \par Limits
**       This parameter has a lower limit of 1.  There are no restrictions on the upper
**       limit however, the maximum number of destinations per packet is system dependent
**       and should be verified.  Destination number values that are checked against this
**       configuration are defined by a 16 bit data word.
**
*/
#define CFE_PLATFORM_SB_MAX_DEST_PER_PKT 16

/**
**  \cfesbcfg Default Subscription Message Limit
**
**  \par Description:
**       Dictates the default Message Limit when using the #CFE_SB_Subscribe API. This will
**       limit the number of messages with a specific message ID that can be received through
**       a subscription. This only changes the default; other message limits can be set on a per
**       subscription basis using #CFE_SB_SubscribeEx .
**
**  \par Limits
**       This parameter has a lower limit of 4 and an upper limit of 65535.
**
*/
#define CFE_PLATFORM_SB_DEFAULT_MSG_LIMIT 4

/**
**  \cfesbcfg Size of the SB buffer memory pool
**
**  \par Description:
**       Dictates the size of the SB memory pool. For each message the SB
**       sends, the SB dynamically allocates from this memory pool, the memory needed
**       to process the message. The memory needed to process each message is msg
**       size + msg descriptor(CFE_SB_BufferD_t). This memory pool is also used
**       to allocate destination descriptors (CFE_SB_DestinationD_t) during the
**       subscription process.
**       To see the run-time, high-water mark and the current utilization figures
**       regarding this parameter, send an SB command to 'Send Statistics Pkt'.
**       Some memory statistics have been added to the SB housekeeping packet.
**       NOTE: It is important to monitor these statistics to ensure the desired
**       memory margin is met.
**
**  \par Limits
**       This parameter has a lower limit of 512 and an upper limit of UINT_MAX (4 Gigabytes).
**
*/
#define CFE_PLATFORM_SB_BUF_MEMORY_BYTES 524288

/**
**  \cfesbcfg Highest Valid Message Id
**
**  \par Description:
**       The value of this constant dictates the range of valid message ID's, from 0
**       to CFE_PLATFORM_SB_HIGHEST_VALID_MSGID (inclusive).
**
**       Although this can be defined differently across platforms, each platform can
**       only publish/subscribe to message ids within their allowable range. Typically
**       this value is set the same across all mission platforms to avoid this complexity.
**
**  \par Limits
**       This parameter has a lower limit is 1, and an upper limit of 0xFFFFFFFE.
**
**       When using the direct message map implementation for software bus routing, this
**       value is used to size the map where a value of 0x1FFF results in a 16 KBytes map
**       and 0xFFFF is 128 KBytes.
**
**       When using the hash implementation for software bus routing, a multiple of the
**       CFE_PLATFORM_SB_MAX_MSG_IDS is used to size the message map.  In that case
**       the range selected here does not impact message map memory use, so it's
**       reasonable to use up to the full range supported by the message ID implementation.
*/
#define CFE_PLATFORM_SB_HIGHEST_VALID_MSGID 0x1FFF

/**
**  \cfesbcfg Default Routing Information Filename
**
**  \par Description:
**       The value of this constant defines the filename used to store the software
**       bus routing information.  This filename is used only when no filename is
**       specified in the command.
**
**  \par Limits
**       The length of each string, including the NULL terminator cannot exceed the
**       #OS_MAX_PATH_LEN value.
*/
#define CFE_PLATFORM_SB_DEFAULT_ROUTING_FILENAME "/ram/cfe_sb_route.dat"

/**
**  \cfesbcfg Default Pipe Information Filename
**
**  \par Description:
**       The value of this constant defines the filename used to store the software
**       bus pipe information. This filename is used only when no filename is
**       specified in the command.
**
**  \par Limits
**       The length of each string, including the NULL terminator cannot exceed the
**       #OS_MAX_PATH_LEN value.
*/
#define CFE_PLATFORM_SB_DEFAULT_PIPE_FILENAME "/ram/cfe_sb_pipe.dat"

/**
**  \cfesbcfg Default Message Map Filename
**
**  \par Description:
**       The value of this constant defines the filename used to store the software
**       bus message map information. This filename is used only when no filename is
**       specified in the command. The message map is a lookup table (array of 16bit
**       words) that has an element for each possible MsgId value and holds the
**       routing table index for that MsgId. The Msg Map provides fast access to the
**       destinations of a message.
**
**  \par Limits
**       The length of each string, including the NULL terminator cannot exceed the
**       #OS_MAX_PATH_LEN value.
*/
#define CFE_PLATFORM_SB_DEFAULT_MAP_FILENAME "/ram/cfe_sb_msgmap.dat"

/**
**  \cfesbcfg SB Event Filtering
**
**  \par Description:
**       This group of configuration parameters dictates what SB events will be
**       filtered through SB. The filtering will begin after the SB task initializes
**       and stay in effect until a cmd to SB changes it.
**       This allows the operator to set limits on the number of event messages that
**       are sent during system initialization.
**       NOTE: Set all unused event values and mask values to zero
**
**  \par Limits
**       This filtering applies only to SB events.
**       These parameters have a lower limit of 0 and an upper limit of 65535.
*/
#define CFE_PLATFORM_SB_FILTERED_EVENT1 CFE_SB_SEND_NO_SUBS_EID
#define CFE_PLATFORM_SB_FILTER_MASK1    CFE_EVS_FIRST_4_STOP

#define CFE_PLATFORM_SB_FILTERED_EVENT2 CFE_SB_DUP_SUBSCRIP_EID
#define CFE_PLATFORM_SB_FILTER_MASK2    CFE_EVS_FIRST_4_STOP

#define CFE_PLATFORM_SB_FILTERED_EVENT3 CFE_SB_MSGID_LIM_ERR_EID
#define CFE_PLATFORM_SB_FILTER_MASK3    CFE_EVS_FIRST_16_STOP

#define CFE_PLATFORM_SB_FILTERED_EVENT4 CFE_SB_Q_FULL_ERR_EID
#define CFE_PLATFORM_SB_FILTER_MASK4    CFE_EVS_FIRST_16_STOP

#define CFE_PLATFORM_SB_FILTERED_EVENT5 0
#define CFE_PLATFORM_SB_FILTER_MASK5    CFE_EVS_NO_FILTER

#define CFE_PLATFORM_SB_FILTERED_EVENT6 0
#define CFE_PLATFORM_SB_FILTER_MASK6    CFE_EVS_NO_FILTER

#define CFE_PLATFORM_SB_FILTERED_EVENT7 0
#define CFE_PLATFORM_SB_FILTER_MASK7    CFE_EVS_NO_FILTER

#define CFE_PLATFORM_SB_FILTERED_EVENT8 0
#define CFE_PLATFORM_SB_FILTER_MASK8    CFE_EVS_NO_FILTER

/**
**  \cfeescfg Define SB Memory Pool Block Sizes
**
**  \par Description:
**       Software Bus Memory Pool Block Sizes
**
**  \par Limits
**       These sizes MUST be increasing and MUST be an integral multiple of 4.
**       The number of block sizes defined cannot exceed
**       #CFE_PLATFORM_ES_POOL_MAX_BUCKETS
*/
#define CFE_PLATFORM_SB_MEM_BLOCK_SIZE_01 8
#define CFE_PLATFORM_SB_MEM_BLOCK_SIZE_02 16
#define CFE_PLATFORM_SB_MEM_BLOCK_SIZE_03 20
#define CFE_PLATFORM_SB_MEM_BLOCK_SIZE_04 36
#define CFE_PLATFORM_SB_MEM_BLOCK_SIZE_05 64
#define CFE_PLATFORM_SB_MEM_BLOCK_SIZE_06 96
#define CFE_PLATFORM_SB_MEM_BLOCK_SIZE_07 128
#define CFE_PLATFORM_SB_MEM_BLOCK_SIZE_08 160
#define CFE_PLATFORM_SB_MEM_BLOCK_SIZE_09 256
#define CFE_PLATFORM_SB_MEM_BLOCK_SIZE_10 512
#define CFE_PLATFORM_SB_MEM_BLOCK_SIZE_11 1024
#define CFE_PLATFORM_SB_MEM_BLOCK_SIZE_12 2048
#define CFE_PLATFORM_SB_MEM_BLOCK_SIZE_13 4096
#define CFE_PLATFORM_SB_MEM_BLOCK_SIZE_14 8192
#define CFE_PLATFORM_SB_MEM_BLOCK_SIZE_15 16384
#define CFE_PLATFORM_SB_MEM_BLOCK_SIZE_16 32768
#define CFE_PLATFORM_SB_MAX_BLOCK_SIZE    (CFE_MISSION_SB_MAX_SB_MSG_SIZE + 128)

/**
**  \cfeescfg Define SB Task Priority
**
**  \par Description:
**       Defines the cFE_SB Task priority.
**
**  \par Limits
**       Not Applicable
*/
#define CFE_PLATFORM_SB_START_TASK_PRIORITY 64

/**
**  \cfeescfg Define SB Task Stack Size
**
**  \par Description:
**       Defines the cFE_SB Task Stack Size
**
**  \par Limits
**       There is a lower limit of 2048 on this configuration parameter.  There
**       are no restrictions on the upper limit however, the maximum stack size
**       is system dependent and should be verified.  Most operating systems provide
**       tools for measuring the amount of stack used by a task during operation. It
**       is always a good idea to verify that no more than 1/2 of the stack is used.
*/
#define CFE_PLATFORM_SB_START_TASK_STACK_SIZE CFE_PLATFORM_ES_DEFAULT_STACK_SIZE

#endif
