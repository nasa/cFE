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
 *   CFE Executive Services (CFE_ES) Application Private Config Definitions
 *
 * This provides default values for configurable items that are internal
 * to this module and do NOT affect the interface(s) of this module.  Changes
 * to items in this file only affect the local module and will be transparent
 * to external entities that are using the public interface(s).
 *
 * @note This file may be overridden/superceded by mission-provided definitions
 * either by overriding this header or by generating definitions from a command/data
 * dictionary tool.
 */
#ifndef CFE_CORE_PRIVATE_INTERNAL_CFG_H
#define CFE_CORE_PRIVATE_INTERNAL_CFG_H

/**
**  \cfesbcfg Platform Endian Indicator
**
**  \par Description:
**       The value of this constant indicates the endianess of the target system
**
**  \par Limits
**       This parameter has a lower limit of 0 and an upper limit of 1.
*/
#define CFE_PLATFORM_ENDIAN CCSDS_LITTLE_ENDIAN

/** \cfeescfg CFE core application startup timeout
**
**  \par Description:
**      The upper limit for the amount of time that the cFE core applications
**      (ES, SB, EVS, TIME, TBL) are each allotted to reach their respective
**      "ready" states.
**
**      The CFE "main" thread starts individual tasks for each of the core applications
**      (except FS).  Each of these must perform some initialization work before the
**      next core application can be started, so the main thread waits to ensure that the
**      application has reached the "ready" state before starting the next application.
**
**      If any core application fails to start, then it indicates a major problem with
**      the system and startup is aborted.
**
**      Units are in milliseconds
**
**  \par Limits:
**       Must be defined as an integer value that is greater than
**       or equal to zero.
**
*/
#define CFE_PLATFORM_CORE_MAX_STARTUP_MSEC 30000

#endif
