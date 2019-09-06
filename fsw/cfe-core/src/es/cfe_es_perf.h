/*
**  GSC-18128-1, "Core Flight Executive Version 6.6"
**
**  Copyright (c) 2006-2019 United States Government as represented by
**  the Administrator of the National Aeronautics and Space Administration.
**  All Rights Reserved.
**
**  Licensed under the Apache License, Version 2.0 (the "License");
**  you may not use this file except in compliance with the License.
**  You may obtain a copy of the License at
**
**    http://www.apache.org/licenses/LICENSE-2.0
**
**  Unless required by applicable law or agreed to in writing, software
**  distributed under the License is distributed on an "AS IS" BASIS,
**  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**  See the License for the specific language governing permissions and
**  limitations under the License.
*/

/*
** File:  cfe_es_perf.h
**
** Purpose: Performance Analyzer data structures
**
** Design Notes:
**
** References:
**
*/

#ifndef _cfe_es_perf_
#define _cfe_es_perf_


/*
** Include Files
*/
#include "common_types.h"
#include "osapi.h"
#include "cfe_es.h"
#include "cfe_es_msg.h"
#include "cfe_es_events.h"
#include "cfe_sb.h"
#include "cfe_evs.h"
#include "cfe_perfids.h"
#include "cfe_psp.h"

/*
**  Defines
*/
#define CFE_ES_PERF_CHILD_NAME           "ES_PerfFileWriter"
#define CFE_ES_PERF_CHILD_STACK_PTR      0
#define CFE_ES_PERF_CHILD_FLAGS          0


enum CFE_ES_PerfState_t {
    CFE_ES_PERF_IDLE = 0,
    CFE_ES_PERF_WAITING_FOR_TRIGGER,
    CFE_ES_PERF_TRIGGERED,
    CFE_ES_PERF_MAX_STATES
};

enum CFE_ES_PerfMode_t {
    CFE_ES_PERF_TRIGGER_START = 0,
    CFE_ES_PERF_TRIGGER_CENTER,
    CFE_ES_PERF_TRIGGER_END,
    CFE_ES_PERF_MAX_MODES
};

typedef struct {
    uint32                         DataToWrite;
    uint32                         ChildID;
    char                           DataFileName[OS_MAX_PATH_LEN];
    int32                          DataFileDescriptor;
} CFE_ES_PerfLogDump_t;

extern CFE_ES_PerfLogDump_t    CFE_ES_PerfLogDumpStatus;

#endif /* _cfe_es_perf_ */

