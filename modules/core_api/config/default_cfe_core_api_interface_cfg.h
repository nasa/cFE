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
 *   This header file contains the mission configuration parameters and
 *   typedefs with mission scope.
 */

#ifndef CFE_CORE_API_INTERFACE_CFG_H
#define CFE_CORE_API_INTERFACE_CFG_H

/**
**  \cfemissioncfg cFE Maximum length for pathnames within data exchange structures
**
**  \par Description:
**       The value of this constant dictates the size of pathnames within all structures
**       used for external data exchange, such as Software bus messages and table definitions.
**       This is typically the same as OS_MAX_PATH_LEN but that is OSAL dependent --
**       and as such it definable on a per-processor/OS basis and hence may be different
**       across multiple processors.  It is recommended to set this to the value of the
**       largest OS_MAX_PATH_LEN in use on any CPU on the mission.
**
**       This affects only the layout of command/telemetry messages and table definitions;
**       internal allocation may use the platform-specific OS_MAX_PATH_LEN value.
**
**       This length must include an extra character for NULL termination.
**
**  \par Limits
**       All CPUs within the same SB domain (mission) and ground tools must share the
**       same definition.
**       Note this affects the size of messages, so it must not cause any message
**       to exceed the max length.
**
**       This value should be kept as a multiple of 4, to maintain alignment of
**       any possible neighboring fields without implicit padding.
*/
#define CFE_MISSION_MAX_PATH_LEN 64

/**
**  \cfemissioncfg cFE Maximum length for filenames within data exchange structures
**
**  \par Description:
**       The value of this constant dictates the size of filenames within all structures
**       used for external data exchange, such as Software bus messages and table definitions.
**       This is typically the same as OS_MAX_FILE_LEN but that is OSAL dependent --
**       and as such it definable on a per-processor/OS basis and hence may be different
**       across multiple processors.  It is recommended to set this to the value of the
**       largest OS_MAX_FILE_LEN in use on any CPU on the mission.
**
**       This affects only the layout of command/telemetry messages and table definitions;
**       internal allocation may use the platform-specific OS_MAX_FILE_LEN value.
**
**       This length must include an extra character for NULL termination.
**
**  \par Limits
**       All CPUs within the same SB domain (mission) and ground tools must share the
**       same definition.
**       Note this affects the size of messages, so it must not cause any message
**       to exceed the max length.
**
**       This value should be kept as a multiple of 4, to maintain alignment of
**       any possible neighboring fields without implicit padding.
*/
#define CFE_MISSION_MAX_FILE_LEN 20

/**
**  \cfemissioncfg cFE Maximum length for API names within data exchange structures
**
**  \par Description:
**       The value of this constant dictates the size of filenames within all structures
**       used for external data exchange, such as Software bus messages and table definitions.
**       This is typically the same as OS_MAX_API_LEN but that is OSAL dependent --
**       and as such it definable on a per-processor/OS basis and hence may be different
**       across multiple processors.  It is recommended to set this to the value of the
**       largest OS_MAX_API_LEN in use on any CPU on the mission.
**
**       This affects only the layout of command/telemetry messages and table definitions;
**       internal allocation may use the platform-specific OS_MAX_API_LEN value.
**
**       This length must include an extra character for NULL termination.
**
**  \par Limits
**       All CPUs within the same SB domain (mission) must share the same definition
**       Note this affects the size of messages, so it must not cause any message
**       to exceed the max length.
**
**       This value should be kept as a multiple of 4, to maintain alignment of
**       any possible neighboring fields without implicit padding.
*/
#define CFE_MISSION_MAX_API_LEN 20

/**
**  \cfemissioncfg cFE Maximum number of files in a message/data exchange
**
**  \par Description:
**       The value of this constant dictates the maximum number of files within all structures
**       used for external data exchange, such as Software bus messages and table definitions.
**       This is typically the same as OS_MAX_NUM_OPEN_FILES but that is OSAL dependent --
**       and as such it definable on a per-processor/OS basis and hence may be different
**       across multiple processors.  It is recommended to set this to the value of the
**       largest OS_MAX_NUM_OPEN_FILES in use on any CPU on the mission.
**
**       This affects only the layout of command/telemetry messages and table definitions;
**       internal allocation may use the platform-specific OS_MAX_NUM_OPEN_FILES value.
**
**  \par Limits
**       All CPUs within the same SB domain (mission) must share the same definition
**       Note this affects the size of messages, so it must not cause any message
**       to exceed the max length.
**
*/
#define CFE_MISSION_MAX_NUM_FILES 50

#endif /* SAMPLE_MISSION_CFG_H */
