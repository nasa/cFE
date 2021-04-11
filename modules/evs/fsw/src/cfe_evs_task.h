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
 * @file
 *
 *  Title:    Event Services API - Management Control Interfaces.
 *
 *  Purpose:
 *            Unit specification for the event services management control interfaces.
 *
 *  Contents:
 *       I.  macro and constant type definitions
 *      II.  EVS internal structures
 *     III.  function prototypes
 *
 *  Design Notes:
 *
 *  References:
 *     Flight Software Branch C Coding Standard Version 1.0a
 *
 */

#ifndef CFE_EVS_TASK_H
#define CFE_EVS_TASK_H

/********************************** Include Files  ************************************/
#include "common_types.h"
#include "cfe_platform_cfg.h"
#include "cfe_mission_cfg.h"
#include "osconfig.h"
#include "cfe_evs_api_typedefs.h"
#include "cfe_evs_log_typedef.h"
#include "cfe_sb_api_typedefs.h"
#include "cfe_evs_events.h"

/*********************  Macro and Constant Type Definitions   ***************************/

#define CFE_EVS_MSG_TRUNCATED        '$'
#define CFE_EVS_FREE_SLOT            (-1)
#define CFE_EVS_NO_MASK              0
#define CFE_EVS_PIPE_DEPTH           32
#define CFE_EVS_MAX_EVENT_SEND_COUNT 65535
#define CFE_EVS_MAX_FILTER_COUNT     65535
#define CFE_EVS_PIPE_NAME            "EVS_CMD_PIPE"
#define CFE_EVS_MAX_PORT_MSG_LENGTH  (CFE_MISSION_EVS_MAX_MESSAGE_LENGTH + OS_MAX_API_NAME + 30)

/* Since CFE_EVS_MAX_PORT_MSG_LENGTH is the size of the buffer that is sent to
 * print out (using OS_printf), we need to check to make sure that the buffer
 * size the OS uses is big enough. This check has to be made here because it is
 * the first spot after CFE_EVS_MAX_PORT_MSG_LENGTH is defined */
#if OS_BUFFER_SIZE < CFE_EVS_MAX_PORT_MSG_LENGTH
#error CFE_EVS_MAX_PORT_MSG_LENGTH cannot be greater than OS_BUFFER_SIZE!
#endif

/************************  Internal Structure Definitions  *****************************/

typedef struct
{
    int16  EventID; /* Numerical event identifier */
    uint16 Mask;    /* Binary filter mask */
    uint16 Count;   /* Binary filter counter */
    uint16 Padding; /* Structure padding */

} EVS_BinFilter_t;

typedef struct
{
    CFE_ES_AppId_t AppID;
    CFE_ES_AppId_t UnregAppID;

    EVS_BinFilter_t BinFilters[CFE_PLATFORM_EVS_MAX_EVENT_FILTERS]; /* Array of binary filters */

    uint8  ActiveFlag;           /* Application event service active flag */
    uint8  EventTypesActiveFlag; /* Application event types active flag */
    uint16 EventCount;           /* Application event counter */

} EVS_AppData_t;

typedef struct
{
    char            AppName[OS_MAX_API_NAME];                    /* Application name */
    uint8           ActiveFlag;                                  /* Application event service active flag */
    uint8           EventTypesActiveFlag;                        /* Application event types active flag */
    uint16          EventCount;                                  /* Application event counter */
    EVS_BinFilter_t Filters[CFE_PLATFORM_EVS_MAX_EVENT_FILTERS]; /* Application event filters */

} CFE_EVS_AppDataFile_t;

/* Global data structure */
typedef struct
{
    EVS_AppData_t AppData[CFE_PLATFORM_ES_MAX_APPLICATIONS]; /* Application state data and event filters */

    CFE_EVS_Log_t *EVS_LogPtr; /* Pointer to the EVS log in the ES Reset area*/
                               /* see cfe_es_global.h */

    /*
    ** EVS task data
    */
    CFE_EVS_HousekeepingTlm_t EVS_TlmPkt;
    CFE_SB_PipeId_t           EVS_CommandPipe;
    osal_id_t                 EVS_SharedDataMutexID;
    CFE_ES_AppId_t            EVS_AppID;

} CFE_EVS_Global_t;

/*
 *  Global variable specific to EVS module
 */
extern CFE_EVS_Global_t CFE_EVS_Global;

/*****************************  Function Prototypes   **********************************/

/*
 * Functions used within this module and by the unit test
 */
extern int32 CFE_EVS_TaskInit(void);
extern void  CFE_EVS_ProcessCommandPacket(CFE_SB_Buffer_t *SBBufPtr);

/*
 * EVS Message Handler Functions
 */
int32 CFE_EVS_ReportHousekeepingCmd(const CFE_MSG_CommandHeader_t *data);
int32 CFE_EVS_NoopCmd(const CFE_EVS_NoopCmd_t *data);
int32 CFE_EVS_ClearLogCmd(const CFE_EVS_ClearLogCmd_t *data);
int32 CFE_EVS_ResetCountersCmd(const CFE_EVS_ResetCountersCmd_t *data);
int32 CFE_EVS_SetFilterCmd(const CFE_EVS_SetFilterCmd_t *data);
int32 CFE_EVS_EnablePortsCmd(const CFE_EVS_EnablePortsCmd_t *data);
int32 CFE_EVS_DisablePortsCmd(const CFE_EVS_DisablePortsCmd_t *data);
int32 CFE_EVS_EnableEventTypeCmd(const CFE_EVS_EnableEventTypeCmd_t *data);
int32 CFE_EVS_DisableEventTypeCmd(const CFE_EVS_DisableEventTypeCmd_t *data);
int32 CFE_EVS_SetEventFormatModeCmd(const CFE_EVS_SetEventFormatModeCmd_t *data);
int32 CFE_EVS_EnableAppEventTypeCmd(const CFE_EVS_EnableAppEventTypeCmd_t *data);
int32 CFE_EVS_DisableAppEventTypeCmd(const CFE_EVS_DisableAppEventTypeCmd_t *data);
int32 CFE_EVS_EnableAppEventsCmd(const CFE_EVS_EnableAppEventsCmd_t *data);
int32 CFE_EVS_DisableAppEventsCmd(const CFE_EVS_DisableAppEventsCmd_t *data);
int32 CFE_EVS_ResetAppCounterCmd(const CFE_EVS_ResetAppCounterCmd_t *data);
int32 CFE_EVS_ResetFilterCmd(const CFE_EVS_ResetFilterCmd_t *data);
int32 CFE_EVS_AddEventFilterCmd(const CFE_EVS_AddEventFilterCmd_t *data);
int32 CFE_EVS_DeleteEventFilterCmd(const CFE_EVS_DeleteEventFilterCmd_t *data);
int32 CFE_EVS_WriteAppDataFileCmd(const CFE_EVS_WriteAppDataFileCmd_t *data);
int32 CFE_EVS_ResetAllFiltersCmd(const CFE_EVS_ResetAllFiltersCmd_t *data);

#endif /* CFE_EVS_TASK_H */
