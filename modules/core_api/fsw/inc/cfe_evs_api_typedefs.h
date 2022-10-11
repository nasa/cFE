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
 *  Title:    Event Services API Application Library Header File
 *
 *  Purpose:
 *	           Unit specification for Event services library functions and macros.
 *
 *  Design Notes:
 *
 *  References:
 *     Flight Software Branch C Coding Standard Version 1.0a
 */

#ifndef CFE_EVS_API_TYPEDEFS_H
#define CFE_EVS_API_TYPEDEFS_H

/********************************** Include Files  ************************************/
#include "common_types.h" /* Basic data types */
#include "cfe_evs_extern_typedefs.h"

/** \name Common Event Filter Mask Values
 * Message is sent if (previous event count) & MASK == 0
 */
/** \{ */
#define CFE_EVS_NO_FILTER        0x0000 /**< \brief Stops any filtering.  All messages are sent. */
#define CFE_EVS_FIRST_ONE_STOP   0xFFFF /**< \brief Sends the first event.  All remaining messages are filtered. */
#define CFE_EVS_FIRST_TWO_STOP   0xFFFE /**< \brief Sends the first 2 events.  All remaining messages are filtered. */
#define CFE_EVS_FIRST_4_STOP     0xFFFC /**< \brief Sends the first 4 events.  All remaining messages are filtered. */
#define CFE_EVS_FIRST_8_STOP     0xFFF8 /**< \brief Sends the first 8 events.  All remaining messages are filtered. */
#define CFE_EVS_FIRST_16_STOP    0xFFF0 /**< \brief Sends the first 16 events.  All remaining messages are filtered. */
#define CFE_EVS_FIRST_32_STOP    0xFFE0 /**< \brief Sends the first 32 events.  All remaining messages are filtered. */
#define CFE_EVS_FIRST_64_STOP    0xFFC0 /**< \brief Sends the first 64 events.  All remaining messages are filtered. */
#define CFE_EVS_EVERY_OTHER_ONE  0x0001 /**< \brief Sends every other event. */
#define CFE_EVS_EVERY_OTHER_TWO  0x0002 /**< \brief Sends two, filters one, sends two, filters one, etc */
#define CFE_EVS_EVERY_FOURTH_ONE 0x0003 /**< \brief Sends every fourth event message.  All others are filtered. */
/** \} */

/******************  Structure Definitions *********************/

/** \brief Event message filter definition structure */
typedef struct CFE_EVS_BinFilter
{
    uint16 EventID; /**< \brief Numerical event identifier */
    uint16 Mask;    /**< \brief Binary filter mask value */
} CFE_EVS_BinFilter_t;

#endif /* CFE_EVS_API_TYPEDEFS_H */
