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
 *   Specification for the CFE Executive Services (CFE_ES) command function codes
 *
 * @note
 *   This file should be strictly limited to the command/function code (CC)
 *   macro definitions.  Other definitions such as enums, typedefs, or other
 *   macros should be placed in the msgdefs.h or msg.h files.
 */
#ifndef DEFAULT_CFE_ES_FCNCODE_VALUES_H
#define DEFAULT_CFE_ES_FCNCODE_VALUES_H

/************************************************************************
 * Macro Definitions
 ************************************************************************/

#define CFE_ES_CCVAL(x) CFE_ES_FunctionCode_##x

enum CFE_ES_FunctionCode_
{
    CFE_ES_FunctionCode_NOOP                  = 0,
    CFE_ES_FunctionCode_RESET_COUNTERS        = 1,
    CFE_ES_FunctionCode_RESTART               = 2,
    CFE_ES_FunctionCode_START_APP             = 4,
    CFE_ES_FunctionCode_STOP_APP              = 5,
    CFE_ES_FunctionCode_RESTART_APP           = 6,
    CFE_ES_FunctionCode_RELOAD_APP            = 7,
    CFE_ES_FunctionCode_QUERY_ONE             = 8,
    CFE_ES_FunctionCode_QUERY_ALL             = 9,
    CFE_ES_FunctionCode_CLEAR_SYS_LOG         = 10,
    CFE_ES_FunctionCode_WRITE_SYS_LOG         = 11,
    CFE_ES_FunctionCode_CLEAR_ER_LOG          = 12,
    CFE_ES_FunctionCode_WRITE_ER_LOG          = 13,
    CFE_ES_FunctionCode_START_PERF_DATA       = 14,
    CFE_ES_FunctionCode_STOP_PERF_DATA        = 15,
    CFE_ES_FunctionCode_SET_PERF_FILTER_MASK  = 16,
    CFE_ES_FunctionCode_SET_PERF_TRIGGER_MASK = 17,
    CFE_ES_FunctionCode_OVER_WRITE_SYS_LOG    = 18,
    CFE_ES_FunctionCode_RESET_PR_COUNT        = 19,
    CFE_ES_FunctionCode_SET_MAX_PR_COUNT      = 20,
    CFE_ES_FunctionCode_DELETE_CDS            = 21,
    CFE_ES_FunctionCode_SEND_MEM_POOL_STATS   = 22,
    CFE_ES_FunctionCode_DUMP_CDS_REGISTRY     = 23,
    CFE_ES_FunctionCode_QUERY_ALL_TASKS       = 24,
};

#endif
