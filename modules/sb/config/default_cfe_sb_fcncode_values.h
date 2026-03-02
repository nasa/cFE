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
 *   Specification for the CFE Event Services (CFE_SB) command function codes
 *
 * @note
 *   This file should be strictly limited to the command/function code (CC)
 *   macro definitions.  Other definitions such as enums, typedefs, or other
 *   macros should be placed in the msgdefs.h or msg.h files.
 */
#ifndef DEFAULT_CFE_SB_FCNCODE_VALUES_H
#define DEFAULT_CFE_SB_FCNCODE_VALUES_H

/************************************************************************
 * Macro Definitions
 ************************************************************************/

#define CFE_SB_CCVAL(x) CFE_SB_FunctionCode_##x

enum CFE_SB_FunctionCode_
{
    CFE_SB_FunctionCode_NOOP                  = 0,
    CFE_SB_FunctionCode_RESET_COUNTERS        = 1,
    CFE_SB_FunctionCode_SEND_SB_STATS         = 2,
    CFE_SB_FunctionCode_WRITE_ROUTING_INFO    = 3,
    CFE_SB_FunctionCode_ENABLE_ROUTE          = 4,
    CFE_SB_FunctionCode_DISABLE_ROUTE         = 5,
    CFE_SB_FunctionCode_WRITE_PIPE_INFO       = 7,
    CFE_SB_FunctionCode_WRITE_MAP_INFO        = 8,
    CFE_SB_FunctionCode_ENABLE_SUB_REPORTING  = 9,
    CFE_SB_FunctionCode_DISABLE_SUB_REPORTING = 10,
    CFE_SB_FunctionCode_SEND_PREV_SUBS        = 11,
};

#endif
