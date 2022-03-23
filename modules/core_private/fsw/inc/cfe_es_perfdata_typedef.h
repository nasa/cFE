/**
 * @file
 *
 * Definition of CFE_ES_PerfData_t structure type
 */

#ifndef CFE_ES_PERFDATA_TYPEDEF_H
#define CFE_ES_PERFDATA_TYPEDEF_H

#include "common_types.h"
#include "cfe_mission_cfg.h"  /* Required for CFE_MISSION_ES_PERF_MAX_IDS */
#include "cfe_platform_cfg.h" /* Required for CFE_PLATFORM_ES_PERF_DATA_BUFFER_SIZE */

#define CFE_ES_PERF_32BIT_WORDS_IN_MASK ((CFE_MISSION_ES_PERF_MAX_IDS) / 32)

typedef struct
{
    uint32 Data;
    uint32 TimerUpper32; /* TBU - timebase register */
    uint32 TimerLower32; /* TBL - timebase register */
} CFE_ES_PerfDataEntry_t;

typedef struct
{
    uint8  Version;
    uint8  Endian;
    uint8  Spare[2];
    uint32 TimerTicksPerSecond;
    uint32 TimerLow32Rollover;
    /*
     * The "State" member is marked volatile to help
     * ensure that an optimizing compiler does not rearrange
     * or eliminate reads/writes of this value.  It is read
     * outside of any locking to determine whether or not
     * the performance log function is enabled.
     */
    volatile uint32 State;
    uint32          Mode;
    uint32          TriggerCount;
    uint32          DataStart;
    uint32          DataEnd;
    uint32          DataCount;
    uint32          InvalidMarkerReported;
    uint32          FilterTriggerMaskSize;
    uint32          FilterMask[CFE_ES_PERF_32BIT_WORDS_IN_MASK];
    uint32          TriggerMask[CFE_ES_PERF_32BIT_WORDS_IN_MASK];
} CFE_ES_PerfMetaData_t;

typedef struct
{
    CFE_ES_PerfMetaData_t  MetaData;
    CFE_ES_PerfDataEntry_t DataBuffer[CFE_PLATFORM_ES_PERF_DATA_BUFFER_SIZE];
} CFE_ES_PerfData_t;

#endif /* CFE_ES_PERFDATA_TYPEDEF_H */
