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

/*
**  File: cfe_ta_cpuaffinity.c
**
**
** Purpose:
**   Default task affinity list
**
*/

/*
**  Include Files
*/
#include <stdio.h>
#include <string.h>

/*
** cFE includes
*/
#include "cfe_ta_tasklist.h"
#include "osapi.h"

/*
** TA Core Affinity Helper Macros
*/
#define CFE_TA_CPU(n)                (1ULL << ((n) - 1))
#define CFE_TA_CPU_RANGE(start, end) (((1ULL << ((end) - (start) + 1)) - 1) << ((start) - 1))
#define CFE_TA_CPU_ALL               (~0ULL)
/*
** Helper macro to unpack a 8-bit integer into an OS_cpuset_t struct
** Note this assumes OS_MAX_CPUS<=8. If it's more this needs to be adjusted for the other bits
*/
#define CFE_TA_BUILD_MASK(val)                    \
    {                                             \
        .affinity_mask = {(uint8)((val) & 0xFF) } \
    }

/*
** Core affinity assignment macro (1 indexed)
** The Type field needs to be set to CFE_TA_Mask for assigning manual core affinity
**     Ex: assign affinity to core1 only
**         Method 1: CFE_TA_BUILD_MASK(0x1)
**         Method 2: CFE_TA_BUILD_MASK(CFE_TA_CPU(1))
**     Ex: assign affinity to core1 and core2
**         Method 1: CFE_TA_BUILD_MASK(0x3)
**         Method 2: CFE_TA_BUILD_MASK(CFE_TA_CPU(1) | CFE_TA_CPU(2))
**         Method 3: CFE_TA_BUILD_MASK(CFE_TA_CPU_RANGE(1,2))
**     Ex: assign affinity to core1 and core3 and core4 (avoids core2)
**         Method 1: CFE_TA_BUILD_MASK(0xD)
**         Method 2: CFE_TA_BUILD_MASK(CFE_TA_CPU(1) | CFE_TA_CPU(3) | CFE_TA_CPU(4))
*/

/*
**
** LOCAL FUNCTION PROTOTYPES
**
*/

/***************************************************************************
 **                        FUNCTIONS DEFINITIONS
 ***************************************************************************/

/*
** Initial Task Affinity configuration
*/

/* clang-format off */
CFE_TA_CpuTaskAffinity_t  CFE_TA_TargetConfigList[OS_MAX_TASKS] =
{
/*        Task Name, CpuMask(User), CpuFromOS, Type(User),          Status */
/* 01 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },  /* { "CFE_ES",          CFE_TA_BUILD_MASK(CFE_TA_CPU(1)), {{0}}, CFE_TA_Mask, CFE_TA_Status_Unknown }, */
/* 02 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },  /* { "ES_BG_TASK",      CFE_TA_BUILD_MASK(CFE_TA_CPU(1)), {{0}}, CFE_TA_Mask, CFE_TA_Status_Unknown }, */
/* 03 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },  /* { "CFE_EVS",         CFE_TA_BUILD_MASK(CFE_TA_CPU(1)), {{0}}, CFE_TA_Mask, CFE_TA_Status_Unknown }, */
/* 04 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },  /* { "CFE_SB",          CFE_TA_BUILD_MASK(CFE_TA_CPU(1)), {{0}}, CFE_TA_Mask, CFE_TA_Status_Unknown }, */
/* 05 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },  /* { "CFE_TIME",        CFE_TA_BUILD_MASK(CFE_TA_CPU(1)), {{0}}, CFE_TA_Mask, CFE_TA_Status_Unknown }, */
/* 06 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },  /* { "TIME_TONE_TASK",  CFE_TA_BUILD_MASK(CFE_TA_CPU(1)), {{0}}, CFE_TA_Mask, CFE_TA_Status_Unknown }, */
/* 07 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },  /* { "TIME_ONEHZ_TASK", CFE_TA_BUILD_MASK(CFE_TA_CPU(1)), {{0}}, CFE_TA_Mask, CFE_TA_Status_Unknown }, */
/* 08 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },  /* { "CFE_TBL",         CFE_TA_BUILD_MASK(CFE_TA_CPU(1)), {{0}}, CFE_TA_Mask, CFE_TA_Status_Unknown }, */
/* 09 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },  /* { "CFE_TA",          CFE_TA_BUILD_MASK(CFE_TA_CPU(1)), {{0}}, CFE_TA_Mask, CFE_TA_Status_Unknown }, */
/* 10 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },  /* { "SAMPLE_APP",      CFE_TA_BUILD_MASK(CFE_TA_CPU(1)), {{0}}, CFE_TA_Mask, CFE_TA_Status_Unknown }, */
/* 11 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },  /* { "TO_LAB",          CFE_TA_BUILD_MASK(CFE_TA_CPU(1)), {{0}}, CFE_TA_Mask, CFE_TA_Status_Unknown }, */
/* 12 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },  /* { "CI_LAB",          CFE_TA_BUILD_MASK(CFE_TA_CPU(1)), {{0}}, CFE_TA_Mask, CFE_TA_Status_Unknown }, */
/* 13 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },  /* { "SCH_LAB",         CFE_TA_BUILD_MASK(CFE_TA_CPU(1)), {{0}}, CFE_TA_Mask, CFE_TA_Status_Unknown }, */
/* 14 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },  /* { "CF",              CFE_TA_BUILD_MASK(CFE_TA_CPU(1)), {{0}}, CFE_TA_Mask, CFE_TA_Status_Unknown }, */
/* 15 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },  /* { "FM",              CFE_TA_BUILD_MASK(CFE_TA_CPU(1)), {{0}}, CFE_TA_Mask, CFE_TA_Status_Unknown }, */
/* 16 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },  /* { "FM_CHILD_TASK",   CFE_TA_BUILD_MASK(CFE_TA_CPU(1)), {{0}}, CFE_TA_Mask, CFE_TA_Status_Unknown }, */
/* 17 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },
/* 18 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },
/* 19 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },
/* 20 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },
/* 21 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },
/* 22 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },
/* 23 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },
/* 24 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },
/* 25 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },
/* 26 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },
/* 27 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },
/* 28 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },
/* 29 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },
/* 30 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },
/* 31 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },
/* 32 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },
/* 33 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },
/* 34 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },
/* 35 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },
/* 36 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },
/* 37 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },
/* 38 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },
/* 39 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },
/* 40 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },
/* 41 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },
/* 42 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },
/* 43 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },
/* 44 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },
/* 45 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },
/* 46 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },
/* 47 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },
/* 48 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },
/* 49 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },
/* 50 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },
/* 51 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },
/* 52 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },
/* 53 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },
/* 54 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },
/* 55 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },
/* 56 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },
/* 57 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },
/* 58 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },
/* 59 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },
/* 60 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },
/* 61 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },
/* 62 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },
/* 63 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown },
/* 64 */ { "",        {{0}},       {{0}},     CFE_TA_OsDefault, CFE_TA_Status_Unknown }
};
/* clang-format on */
