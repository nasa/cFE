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
 *   CFE Executive Services (CFE_ES) Application Public Definitions
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
#ifndef CFE_ES_INTERFACE_CFG_H
#define CFE_ES_INTERFACE_CFG_H

/**
**  \cfeescfg Mission Max Apps in a message
**
**  \par Description:
**      Indicates the maximum number of apps in a telemetry housekeeping message
**
**      This affects the layout of command/telemetry messages but does not affect run
**      time behavior or internal allocation.
**
**  \par Limits
**       All CPUs within the same SB domain (mission) must share the same definition
**       Note this affects the size of messages, so it must not cause any message
**       to exceed the max length.
*/
#define CFE_MISSION_ES_MAX_APPLICATIONS 16

/**
**  \cfeescfg Define Max Number of Performance IDs for messages
**
**  \par Description:
**       Defines the maximum number of perf ids allowed.
**
**       Each performance id is used to identify something that needs to be
**       measured. Performance ids are limited to the range of 0 to
**       #CFE_MISSION_ES_PERF_MAX_IDS - 1. Any performance ids outside of this
**       range will be ignored and will be flagged as an error.
**
**      This affects the layout of telemetry messages but does not affect run
**      time behavior or internal allocation.
**
**  \par Limits
**       All CPUs within the same SB domain (mission) must share the same definition
**       Note this affects the size of messages, so it must not cause any message
**       to exceed the max length.
**
*/
#define CFE_MISSION_ES_PERF_MAX_IDS 128

/** \cfeescfg Maximum number of block sizes in pool structures
**
**  \par Description:
**      The upper limit for the number of block sizes supported in the generic
**      pool implementation, which in turn implements the memory pools and CDS.
**      This definition is used as the array size with the pool stats structure,
**      and therefore should be consistent across all CPUs in a mission, as well
**      as with the ground station.
**
**      There is also a platform-specific limit which may be fewer than this
**      value.
**
**  \par Limits:
**       Must be at least one.  No specific upper limit, but the number is
**       anticipated to be reasonably small (i.e. tens, not hundreds).  Large
**       values have not been tested.
**
**
*/
#define CFE_MISSION_ES_POOL_MAX_BUCKETS 17

/**
**  \cfeescfg Maximum Length of CDS Name
**
**  \par Description:
**      Indicates the maximum length (in characters) of the CDS name ('CDSName')
**      portion of a Full CDS Name of the following form:
**      "ApplicationName.CDSName"
**
**      This length does not need to include an extra character for NULL termination.
**
**  \par Limits
**       This value should be kept as a multiple of 4, to maintain alignment of
**       any possible neighboring fields without implicit padding.
**
*/
#define CFE_MISSION_ES_CDS_MAX_NAME_LENGTH 16

/**
**  \cfeescfg Mission Default CRC algorithm
**
**  \par Description:
**      Indicates the which CRC algorithm should be used as the default
**      for verifying the contents of Critical Data Stores and when calculating
**      Table Image data integrity values.
**
**  \par Limits
**      Currently only CFE_ES_CrcType_16_ARC is supported (see brief in CFE_ES_CrcType_Enum
**      definition in cfe_es_api_typedefs.h)
*/
#define CFE_MISSION_ES_DEFAULT_CRC CFE_ES_CrcType_16_ARC

/**
**  \cfeescfg Maximum Length of Full CDS Name in messages
**
**  \par Description:
**      Indicates the maximum length (in characters) of the entire CDS name
**      of the following form: "ApplicationName.CDSName"
**
**       This affects the layout of command/telemetry messages but does not affect run
**       time behavior or internal allocation.
**
**  \par Limits
**       All CPUs within the same SB domain (mission) must share the same definition
**       Note this affects the size of messages, so it must not cause any message
**       to exceed the max length.
**
**       This value should be kept as a multiple of 4, to maintain alignment of
**       any possible neighboring fields without implicit padding.
*/
#define CFE_MISSION_ES_CDS_MAX_FULL_NAME_LEN (CFE_MISSION_ES_CDS_MAX_NAME_LENGTH + CFE_MISSION_MAX_API_LEN + 4)

#ifndef CFE_OMIT_DEPRECATED_6_8
/* These names have been converted to an enum in cfe_es_api_typedefs.h */

/** \name Checksum/CRC algorithm identifiers */

#define CFE_MISSION_ES_CRC_8  CFE_ES_CrcType_CRC_8
#define CFE_MISSION_ES_CRC_16 CFE_ES_CrcType_CRC_16
#define CFE_MISSION_ES_CRC_32 CFE_ES_CrcType_CRC_32

#endif

#endif
