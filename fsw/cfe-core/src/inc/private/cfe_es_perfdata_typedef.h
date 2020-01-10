/*
**  GSC-18128-1, "Core Flight Executive Version 6.7"
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

/**
 * \file cfe_es_perfdata_typedef.h
 *
 *  Created on: Jan 22, 2015
 *      Author: joseph.p.hickey@nasa.gov
 *
 * Placeholder for file content description
 */

#ifndef CFE_ES_PERFDATA_TYPEDEF_H_
#define CFE_ES_PERFDATA_TYPEDEF_H_

#include <common_types.h>
#include "cfe_mission_cfg.h"  /* Required for CFE_MISSION_ES_PERF_MAX_IDS */
#include "cfe_platform_cfg.h"  /* Required for CFE_PLATFORM_ES_PERF_DATA_BUFFER_SIZE */

#define CFE_ES_PERF_32BIT_WORDS_IN_MASK ((CFE_MISSION_ES_PERF_MAX_IDS) / 32)


typedef struct {
    uint32         Data;
    uint32         TimerUpper32;   /* TBU - timebase register */
    uint32         TimerLower32;   /* TBL - timebase register */
} CFE_ES_PerfDataEntry_t;

typedef struct {
    uint8                          Version;
    uint8                          Endian;
    uint8                          Spare[2];
    uint32                         TimerTicksPerSecond;
    uint32                         TimerLow32Rollover;
    uint32                         State;
    uint32                         Mode;
    uint32                         TriggerCount;
    uint32                         DataStart;
    uint32                         DataEnd;
    uint32                         DataCount;
    uint32                         InvalidMarkerReported;
    uint32                         FilterTriggerMaskSize;
    uint32                         FilterMask[CFE_ES_PERF_32BIT_WORDS_IN_MASK];
    uint32                         TriggerMask[CFE_ES_PERF_32BIT_WORDS_IN_MASK];
} CFE_ES_PerfMetaData_t;

typedef struct {
    CFE_ES_PerfMetaData_t          MetaData;
    CFE_ES_PerfDataEntry_t         DataBuffer[CFE_PLATFORM_ES_PERF_DATA_BUFFER_SIZE];
} CFE_ES_PerfData_t;



#endif /* CFE_ES_PERFDATA_TYPEDEF_H_ */
