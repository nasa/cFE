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
 *  Title:    Event Services API Log Control Interfaces.
 *
 *  Purpose:
 *            Unit specification for the event services log control interfaces.
 *
 *  Contents:
 *       I.  macro and constant type definitions
 *      II.  EVM internal structures
 *     III.  function prototypes
 *
 *  Design Notes:
 *
 *  References:
 *     Flight Software Branch C Coding Standard Version 1.0a
 *
 *  Notes:
 */

#ifndef CFE_EVS_LOG_H
#define CFE_EVS_LOG_H

/********************* Include Files  ************************/

#include "cfe_evs_msg.h" /* EVS public definitions */

/* ==============   Section I: Macro and Constant Type Definitions   =========== */

/* ==============   Section II: Internal Structures ============ */

/* ==============   Section III: Function Prototypes =========== */

/*---------------------------------------------------------------------------------------*/
/**
 * @brief This routine adds an event packet to the internal event log.
 */
void EVS_AddLog(CFE_EVS_LongEventTlm_t *EVS_PktPtr);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief This routine clears the contents of the internal event log.
 */
void EVS_ClearLog(void);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Message Handler Function
 *
 * This routine writes the contents of the internal event log to a file
 */
int32 CFE_EVS_WriteLogDataFileCmd(const CFE_EVS_WriteLogDataFileCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Message Handler Function
 *
 * This routine sets the internal event log mode.
 */
int32 CFE_EVS_SetLogModeCmd(const CFE_EVS_SetLogModeCmd_t *data);

#endif /* CFE_EVS_LOG_H */
