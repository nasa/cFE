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
 * Purpose:  cFE Time Services (TIME) library API header file
 *
 * Author:   S.Walling/Microtel
 *
 * Notes:
 *
 */

#ifndef CFE_TIME_API_TYPEDEFS_H
#define CFE_TIME_API_TYPEDEFS_H

/*
** Includes
*/
#include "common_types.h"
#include "cfe_time_extern_typedefs.h"

/*****************************************************************************/
/*
** Macro Definitions
*/

#define CFE_TIME_PRINTED_STRING_SIZE \
    24 /**< \brief Required size of buffer to be passed into #CFE_TIME_Print (includes null terminator) */

/**
 * A general-purpose initializer for CFE_TIME_SysTime_t values.
 *
 * Represents "time zero" in the CFE_TIME_SysTime_t domain.  This
 * can be used as a general purpose initializer for instantiations
 * of the CFE_TIME_SysTime_t type.
 */
#define CFE_TIME_ZERO_VALUE ((CFE_TIME_SysTime_t) {0, 0})

/*****************************************************************************/
/*
** Type Definitions
*/

/**
**  \brief Enumerated types identifying the relative relationships of two times
**
**  \par Description
**       Since time fields contain numbers that are relative to an epoch time, then it is possible for a time value
**       to be "negative".  This can lead to some confusion about what relationship exists between two time values.
**       To resolve this confusion, the cFE provides the API #CFE_TIME_Compare which returns these enumerated values.
*/
typedef enum CFE_TIME_Compare
{
    CFE_TIME_A_LT_B = -1, /**< \brief The first specified time is considered to be before the second specified time */
    CFE_TIME_EQUAL  = 0,  /**< \brief The two specified times are considered to be equal */
    CFE_TIME_A_GT_B = 1   /**< \brief The first specified time is considered to be after the second specified time */
} CFE_TIME_Compare_t;

/**
**   \brief Time Synchronization Callback Function Ptr Type
**
**   \par Description
**        Applications that wish to get direct notification of the receipt of the cFE Time Synchronization signal
**        (typically a 1 Hz signal), must register a callback function with the following prototype via the
**        #CFE_TIME_RegisterSynchCallback API.
*/
typedef int32 (*CFE_TIME_SynchCallbackPtr_t)(void);

#endif /* CFE_TIME_API_TYPEDEFS_H */
