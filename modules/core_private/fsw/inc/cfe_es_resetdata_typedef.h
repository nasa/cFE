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
 * Definition of the CFE_ES_ResetData structure type.
 * This was moved into its own header file since it is referenced by multiple CFE core apps.
 */

#ifndef CFE_ES_RESETDATA_TYPEDEF_H
#define CFE_ES_RESETDATA_TYPEDEF_H

#include "common_types.h"

#include "cfe_time_resetvars_typedef.h" /* Required for CFE_TIME_ResetVars_t definition */
#include "cfe_es_erlog_typedef.h"       /* Required for CFE_ES_ERLog_t definition */
#include "cfe_es_perfdata_typedef.h"    /* Required for CFE_ES_PerfData_t definition */
#include "cfe_evs_log_typedef.h"        /* Required for CFE_EVS_Log_t definition */
#include "cfe_platform_cfg.h"           /* CFE_PLATFORM_ES_ER_LOG_ENTRIES, CFE_PLATFORM_ES_SYSTEM_LOG_SIZE */

/*
** Reset Variables type
*/
typedef struct
{
    uint32 ResetType;
    uint32 ResetSubtype;
    uint32 BootSource;
    uint32 ES_CausedReset;
    uint32 ProcessorResetCount;
    uint32 MaxProcessorResetCount;
} CFE_ES_ResetVariables_t;

/*
** Executive Services Global Reset Data type
** This is the special memory area for ES that is preserved
** on a processor reset.
*/
typedef struct
{
    /*
    ** Exception and Reset log declaration
    */
    CFE_ES_ERLog_MetaData_t ERLog[CFE_PLATFORM_ES_ER_LOG_ENTRIES];
    uint32                  ERLogIndex;
    uint32                  ERLogEntries;
    uint32                  LastAppId;

    /*
    ** System Log declaration
    */
    char   SystemLog[CFE_PLATFORM_ES_SYSTEM_LOG_SIZE];
    size_t SystemLogWriteIdx;
    size_t SystemLogEndIdx;
    uint32 SystemLogMode;
    uint32 SystemLogEntryNum;

    /*
    ** Performance Data
    */
    CFE_ES_PerfData_t Perf;

    /*
    ** Reset Variables
    */
    CFE_ES_ResetVariables_t ResetVars;

    /*
    ** Time variables that are
    ** preserved on a processor reset
    */
    CFE_TIME_ResetVars_t TimeResetVars;

    /*
    ** EVS Log and associated variables. This needs to be preserved on a processor reset.
    */
    CFE_EVS_Log_t EVS_Log;
} CFE_ES_ResetData_t;

#endif /* CFE_ES_RESETDATA_TYPEDEF_H */
