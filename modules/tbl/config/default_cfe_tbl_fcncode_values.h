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
 *   Specification for the CFE Event Services (CFE_TBL) command function codes
 *
 * @note
 *   This file should be strictly limited to the command/function code (CC)
 *   macro definitions.  Other definitions such as enums, typedefs, or other
 *   macros should be placed in the msgdefs.h or msg.h files.
 */
#ifndef DEFAULT_CFE_TBL_FCNCODE_VALUES_H
#define DEFAULT_CFE_TBL_FCNCODE_VALUES_H

/************************************************************************
 * Macro Definitions
 ************************************************************************/

#define CFE_TBL_CCVAL(x) CFE_TBL_FunctionCode_##x

enum CFE_TBL_FunctionCode_
{
    CFE_TBL_FunctionCode_NOOP           = 0,
    CFE_TBL_FunctionCode_RESET_COUNTERS = 1,
    CFE_TBL_FunctionCode_LOAD           = 2,
    CFE_TBL_FunctionCode_DUMP           = 3,
    CFE_TBL_FunctionCode_VALIDATE       = 4,
    CFE_TBL_FunctionCode_ACTIVATE       = 5,
    CFE_TBL_FunctionCode_DUMP_REGISTRY  = 6,
    CFE_TBL_FunctionCode_SEND_REGISTRY  = 7,
    CFE_TBL_FunctionCode_DELETE_CDS     = 8,
    CFE_TBL_FunctionCode_ABORT_LOAD     = 9,
};

#endif
