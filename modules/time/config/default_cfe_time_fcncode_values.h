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
 *   Specification for the CFE Time Services (CFE_TIME) command function codes
 *
 * @note
 *   This file should be strictly limited to the command/function code (CC)
 *   macro definitions.  Other definitions such as enums, typedefs, or other
 *   macros should be placed in the msgdefs.h or msg.h files.
 */
#ifndef DEFAULT_CFE_TIME_FCNCODE_VALUES_H
#define DEFAULT_CFE_TIME_FCNCODE_VALUES_H

/************************************************************************
 * Macro Definitions
 ************************************************************************/

#define CFE_TIME_CCVAL(x) CFE_TIME_FunctionCode_##x

enum CFE_TIME_FunctionCode_
{
    CFE_TIME_FunctionCode_NOOP                  = 0,
    CFE_TIME_FunctionCode_RESET_COUNTERS        = 1,
    CFE_TIME_FunctionCode_SEND_DIAGNOSTIC       = 2,
    CFE_TIME_FunctionCode_SET_SOURCE            = 3,
    CFE_TIME_FunctionCode_SET_STATE             = 4,
    CFE_TIME_FunctionCode_ADD_DELAY             = 5,
    CFE_TIME_FunctionCode_SUB_DELAY             = 6,
    CFE_TIME_FunctionCode_SET_TIME              = 7,
    CFE_TIME_FunctionCode_SET_MET               = 8,
    CFE_TIME_FunctionCode_SET_STCF              = 9,
    CFE_TIME_FunctionCode_SET_LEAP_SECONDS      = 10,
    CFE_TIME_FunctionCode_ADD_ADJUST            = 11,
    CFE_TIME_FunctionCode_SUB_ADJUST            = 12,
    CFE_TIME_FunctionCode_ADD_ONE_HZ_ADJUSTMENT = 13,
    CFE_TIME_FunctionCode_SUB_ONE_HZ_ADJUSTMENT = 14,
    CFE_TIME_FunctionCode_SET_SIGNAL            = 15,
};

#endif
