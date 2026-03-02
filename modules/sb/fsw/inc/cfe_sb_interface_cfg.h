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
 *   CFE Software Bus (CFE_SB) Application Public Definitions
 *
 * This provides default values for configurable items that affect
 * the interface(s) of this module.  This includes the CMD/TLM message
 * interface, tables definitions, and any other data products that
 * serve to exchange information with other entities.
 *
 * @note This file may be overridden/superceded by mission-provided definitions
 * either by overriding this header or by generating definitions from a command/data
 * dictionary tool.
 */
#ifndef CFE_SB_INTERFACE_CFG_H
#define CFE_SB_INTERFACE_CFG_H

#include "cfe_sb_interface_cfg_values.h"

/**
**  \cfesbcfg Maximum SB Message Size
**
**  \par Description:
**      The following definition dictates the maximum message size allowed on
**      the software bus. SB checks the pkt length field in the header of all
**      messages sent. If the pkt length field indicates the message is larger
**      than this define, SB sends an event and rejects the send.
**
**  \par Limits
**      This parameter has a lower limit of 6 (CCSDS primary header size).  There
**      are no restrictions on the upper limit however, the maximum message size is
**      system dependent and should be verified.  Total message size values that are
**      checked against this configuration are defined by a 16 bit data word.
*/
#define CFE_MISSION_SB_MAX_SB_MSG_SIZE         CFE_MISSION_SB_CFGVAL(MAX_SB_MSG_SIZE)
#define DEFAULT_CFE_MISSION_SB_MAX_SB_MSG_SIZE 32768

/**
**  \cfesbcfg Maximum Number of pipes that SB command/telemetry messages may hold
**
**  \par Description:
**       Dictates the maximum number of unique Pipes the SB message definitions will hold.
**
**       This affects the layout of command/telemetry messages but does not affect run
**       time behavior or internal allocation.
**
**  \par Limits
**       All CPUs within the same SB domain (mission) must share the same definition
**       Note this affects the size of messages, so it must not cause any message
**       to exceed the max length.
**
*/
#define CFE_MISSION_SB_MAX_PIPES         CFE_MISSION_SB_CFGVAL(MAX_PIPES)
#define DEFAULT_CFE_MISSION_SB_MAX_PIPES 32

/**
**  \cfesbcfg Maximum Number of subscription entries per subscription report packet
**
**  \par Description:
**       The subscription report will group up to this number of entries into each TLM packet
**
**  \par Limits
**       Must not cause the size of the subscription report telemetry packet to exceed mission limits
*/
#define CFE_MISSION_SB_SUB_ENTRIES_PER_PKT         CFE_MISSION_SB_CFGVAL(SUB_ENTRIES_PER_PKT)
#define DEFAULT_CFE_MISSION_SB_SUB_ENTRIES_PER_PKT 20

#endif
