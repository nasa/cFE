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
 * Declarations and prototypes for cfe_evs_extern_typedefs module
 */

#ifndef CFE_EVS_EXTERN_TYPEDEFS_H
#define CFE_EVS_EXTERN_TYPEDEFS_H

#include "common_types.h"

/**
 * @brief Label definitions associated with CFE_EVS_MsgFormat_Enum_t
 */
enum CFE_EVS_MsgFormat
{
    /**
     * @brief Short Format Messages
     */
    CFE_EVS_MsgFormat_SHORT = 0,

    /**
     * @brief Long Format Messages
     */
    CFE_EVS_MsgFormat_LONG = 1
};

/**
 * @brief Identifies format of log messages
 *
 * @sa enum CFE_EVS_MsgFormat
 */
typedef uint8 CFE_EVS_MsgFormat_Enum_t;

/**
 * @brief Label definitions associated with CFE_EVS_LogMode_Enum_t
 */
enum CFE_EVS_LogMode
{
    /**
     * @brief Overwrite Log Mode
     */
    CFE_EVS_LogMode_OVERWRITE = 0,

    /**
     * @brief Discard Log Mode
     */
    CFE_EVS_LogMode_DISCARD = 1
};

/**
 * @brief Identifies handling of log messages after storage is filled
 *
 * @sa enum CFE_EVS_LogMode
 */
typedef uint8 CFE_EVS_LogMode_Enum_t;

/**
 * @brief Label definitions associated with CFE_EVS_EventType_Enum_t
 */
enum CFE_EVS_EventType
{
    /**
     * @brief Events that are intended only for debugging, not nominal operations
     */
    CFE_EVS_EventType_DEBUG = 1,

    /**
     * @brief Events that identify a state change or action that is not an error
     */
    CFE_EVS_EventType_INFORMATION = 2,

    /**
     * @brief Events that identify an error but are not catastrophic (e.g. - bad command
     */
    CFE_EVS_EventType_ERROR = 3,

    /**
     * @brief Events that identify errors that are unrecoverable autonomously
     */
    CFE_EVS_EventType_CRITICAL = 4
};

/**
 * @brief Identifies type of event message
 *
 * @sa enum CFE_EVS_EventType
 */
typedef uint16 CFE_EVS_EventType_Enum_t;

/**
 * @brief Label definitions associated with CFE_EVS_EventFilter_Enum_t
 */
enum CFE_EVS_EventFilter
{
    /**
     * @brief Binary event filter
     */
    CFE_EVS_EventFilter_BINARY = 0
};

/**
 * @brief Identifies event filter schemes
 *
 * @sa enum CFE_EVS_EventFilter
 */
typedef uint8 CFE_EVS_EventFilter_Enum_t;

/**
 * @brief Label definitions associated with CFE_EVS_EventOutput_Enum_t
 */
enum CFE_EVS_EventOutput
{
    /**
     * @brief Output Port 1
     */
    CFE_EVS_EventOutput_PORT1 = 1,

    /**
     * @brief Output Port 2
     */
    CFE_EVS_EventOutput_PORT2 = 2,

    /**
     * @brief Output Port 3
     */
    CFE_EVS_EventOutput_PORT3 = 3,

    /**
     * @brief Output Port 4
     */
    CFE_EVS_EventOutput_PORT4 = 4
};

/**
 * @brief Identifies event output port
 *
 * @sa enum CFE_EVS_EventOutput
 */
typedef uint8 CFE_EVS_EventOutput_Enum_t;

#endif /* CFE_EVS_EXTERN_TYPEDEFS_H */
