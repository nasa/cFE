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
 * This header file contains the platform-specific base msg ID values and
 * logic to convert a topic ID to a message ID value.
 *
 * This file is just a stand-in to permit CFE to build initially.  It is
 * expected that the user will provide the preferred values in a custom
 * header.
 *
 * Conventionally, CFS binaries are compiled for a specific CPU instance,
 * with the MSGIDs for that CPU compiled directly into the binaries.  This
 * example file implements that paradigm.  There should be a separate definition
 * of this file for every CFS instance in the mission.
 *
 * The intent of this design is to permit use of a separate, single header
 * that defines all of the msgid values for all cpus, for example:
 *
 * @code
 *  #define MY_CPU1_CMD_MID_BASE 0x1A00
 *  #define MY_CPU2_CMD_MID_BASE 0x1B00
 *  #define MY_CPU3_CMD_MID_BASE 0x1C00
 *  ....
 * @endcode
 *
 * Then this file can include that combined base msgid header, and
 * define CFE_PLATFORM_BASE_MIDVAL() to cherry-pick one of the values
 * within that set.  For example on CPU2 this can be defined as:
 *
 * @code
 *  #define CFE_PLATFORM_BASE_MIDVAL(x) MY_CPU2_##x##_MID_BASE
 * @endcode
 */

#ifndef DEFAULT_CFE_CORE_API_BASE_MSGID_VALUES_H
#define DEFAULT_CFE_CORE_API_BASE_MSGID_VALUES_H

/**
 * \brief Platform-specific MSGID base value
 *
 *
 */
#define CFE_PLATFORM_BASE_MIDVAL(x) DEFAULT_CFE_PLATFORM_##x##_MID_BASE
#define CFE_GLOBAL_BASE_MIDVAL(x)   DEFAULT_GLOBAL_##x##_MID_BASE

#endif /* DEFAULT_CFE_CORE_API_BASE_MSGID_VALUES_H */
