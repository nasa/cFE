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

/**
 * @file
 *
 *  Purpose:
 *  This file contains the Internal interface for the cFE Application control functions of ES.
 *  These functions and data structures manage the Applications and Child tasks in the cFE.
 *
 *  References:
 *     Flight Software Branch C Coding Standard Version 1.0a
 *     cFE Flight Software Application Developers Guide
 *
 */

#ifndef CFE_ES_APPCTRL_H
#define CFE_ES_APPCTRL_H

/*
** Include Files
*/
#include "common_types.h"

#include "cfe_es_api_typedefs.h"

/*
** Type Definitions
*/

/*
** CFE_ES_AppTableScanState_t is an internal structure used to keep state of
** the background app table scan/cleanup process
*/
typedef struct
{
    uint32 PendingAppStateChanges;
    uint32 BackgroundScanTimer;
    uint8  LastScanCommandCount;
} CFE_ES_AppTableScanState_t;

/*****************************************************************************/
/*
** Function prototypes
*/

/*---------------------------------------------------------------------------------------*/
/**
 * Scan the Application Table for actions to take
 *
 * This function scans the ES Application table and acts on the changes
 * in application states. This is where the external cFE Applications are
 * restarted, reloaded, or deleted.
 */
bool CFE_ES_RunAppTableScan(uint32 ElapsedTime, void *Arg);

/*---------------------------------------------------------------------------------------*/
/**
 * Perform the requested control action for an application
 */
void CFE_ES_ProcessControlRequest(CFE_ES_AppId_t AppId);

#endif /* CFE_ES_APPS_H */
