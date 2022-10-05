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
 * Definition of the CFE_TIME_ResetVars_t structure type
 */

#ifndef CFE_TIME_RESETVARS_TYPEDEF_H
#define CFE_TIME_RESETVARS_TYPEDEF_H

/*
** Includes
*/
#include "common_types.h"
#include "cfe_time_extern_typedefs.h"

#define CFE_TIME_RESET_SIGNATURE 0xA5A55A5A

/**
**  \brief Time related variables that are maintained through a Processor Reset
**
**  \par Description
**       The #CFE_TIME_ResetVars_t data structure contains those variables that are maintained
**       in an area of memory that is not cleared during a Processor Reset.  This allows the
**       cFE Time Service to maintain time to the best of its ability after a Processor Reset.
*/
typedef struct CFE_TIME_ResetVars
{
    uint32             Signature;    /**< \brief Data validation signature used to verify data structure contents*/
    int16              LeapSeconds;  /**< \brief Leap seconds value */
    uint16             ClockSignal;  /**< \brief Current clock signal selection */
    CFE_TIME_SysTime_t CurrentMET;   /**< \brief Current Mission Elapsed Time (MET) */
    CFE_TIME_SysTime_t CurrentSTCF;  /**< \brief Current Spacecraft Time Correlation Factor (STCF) */
    CFE_TIME_SysTime_t CurrentDelay; /**< \brief Current time client delay value */
} CFE_TIME_ResetVars_t;

#endif /* CFE_TIME_RESETVARS_TYPEDEF_H */
