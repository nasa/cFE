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
 *   Specification for the CFE Event Services (CFE_EVS) command function codes
 *
 * @note
 *   This file should be strictly limited to the command/function code (CC)
 *   macro definitions.  Other definitions such as enums, typedefs, or other
 *   macros should be placed in the msgdefs.h or msg.h files.
 */
#ifndef DEFAULT_CFE_EVS_FCNCODE_VALUES_H
#define DEFAULT_CFE_EVS_FCNCODE_VALUES_H

/************************************************************************
 * Macro Definitions
 ************************************************************************/

#define CFE_EVS_CCVAL(x) CFE_EVS_FunctionCode_##x

enum CFE_EVS_FunctionCode_
{
    CFE_EVS_FunctionCode_NOOP                   = 0,
    CFE_EVS_FunctionCode_RESET_COUNTERS         = 1,
    CFE_EVS_FunctionCode_ENABLE_EVENT_TYPE      = 2,
    CFE_EVS_FunctionCode_DISABLE_EVENT_TYPE     = 3,
    CFE_EVS_FunctionCode_SET_EVENT_FORMAT_MODE  = 4,
    CFE_EVS_FunctionCode_ENABLE_APP_EVENT_TYPE  = 5,
    CFE_EVS_FunctionCode_DISABLE_APP_EVENT_TYPE = 6,
    CFE_EVS_FunctionCode_ENABLE_APP_EVENTS      = 7,
    CFE_EVS_FunctionCode_DISABLE_APP_EVENTS     = 8,
    CFE_EVS_FunctionCode_RESET_APP_COUNTER      = 9,
    CFE_EVS_FunctionCode_SET_FILTER             = 10,
    CFE_EVS_FunctionCode_ENABLE_PORTS           = 11,
    CFE_EVS_FunctionCode_DISABLE_PORTS          = 12,
    CFE_EVS_FunctionCode_RESET_FILTER           = 13,
    CFE_EVS_FunctionCode_RESET_ALL_FILTERS      = 14,
    CFE_EVS_FunctionCode_ADD_EVENT_FILTER       = 15,
    CFE_EVS_FunctionCode_DELETE_EVENT_FILTER    = 16,
    CFE_EVS_FunctionCode_WRITE_APP_DATA_FILE    = 17,
    CFE_EVS_FunctionCode_WRITE_LOG_DATA_FILE    = 18,
    CFE_EVS_FunctionCode_SET_LOG_MODE           = 19,
    CFE_EVS_FunctionCode_CLEAR_LOG              = 20,
};

#endif
