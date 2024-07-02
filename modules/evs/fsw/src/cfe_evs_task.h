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
#include "cfe_time.h"
#include "cfe_evs_api_typedefs.h"
#include "cfe_evs_log_typedef.h"
#include "cfe_sb_api_typedefs.h"
#include "cfe_evs_eventids.h"

/*********************  Macro and Constant Type Definitions   ***************************/

#define CFE_EVS_MSG_TRUNCATED        '$'
#define CFE_EVS_FREE_SLOT            (-1)
#define CFE_EVS_NO_MASK              0
#define CFE_EVS_PIPE_DEPTH           32
#define CFE_EVS_MAX_EVENT_SEND_COUNT 65535
#define CFE_EVS_MAX_FILTER_COUNT     65535
#define CFE_EVS_MAX_SQUELCH_COUNT    255
#define CFE_EVS_PIPE_NAME            "EVS_CMD_PIPE"
#define CFE_EVS_MAX_PORT_MSG_LENGTH  (CFE_MISSION_EVS_MAX_MESSAGE_LENGTH + OS_MAX_API_NAME + 19)

/* Since CFE_EVS_MAX_PORT_MSG_LENGTH is the size of the buffer that is sent to
 * print out (using OS_printf), we need to check to make sure that the buffer
 * size the OS uses is big enough. The buffer needs to have at least 11 extra
 * characters to accommodate the format string "EVS Port%u %s\n" used in
 * downstream processing for sending messages via ports. This check has to be
 * made here because it is the first spot after CFE_EVS_MAX_PORT_MSG_LENGTH
 * is defined. */
#if OS_BUFFER_SIZE < CFE_EVS_MAX_PORT_MSG_LENGTH + 11
#error CFE_EVS_MAX_PORT_MSG_LENGTH cannot be greater than OS_BUFFER_SIZE - 11!
#endif

/************************  Internal Structure Definitions  *****************************/

typedef struct
{
    uint16 EventID; /* Numerical event identifier */
    uint16 Mask;    /* Binary filter mask */
    uint16 Count;   /* Binary filter counter */
    uint16 Padding; /* Structure padding */
} EVS_BinFilter_t;

typedef struct
{
    CFE_ES_AppId_t AppID;
    CFE_ES_AppId_t UnregAppID;

    EVS_BinFilter_t BinFilters[CFE_PLATFORM_EVS_MAX_EVENT_FILTERS]; /* Array of binary filters */

    uint8     ActiveFlag;                /* Application event service active flag */
    uint8     EventTypesActiveFlag;      /* Application event types active flag */
    uint16    EventCount;                /* Application event counter */
    OS_time_t LastSquelchCreditableTime; /* Time of last squelch token return */
    int32     SquelchTokens;             /* Application event squelch token counter */
    uint8     SquelchedCount;            /* Application events squelched counter */
} EVS_AppData_t;

typedef struct
{
    char            AppName[OS_MAX_API_NAME]; /* Application name */
    uint8           ActiveFlag;               /* Application event service active flag */
    uint8           EventTypesActiveFlag;     /* Application event types active flag */
    uint16          EventCount;               /* Application event counter */
    uint8           SquelchedCount;           /* Application events squelched counter */
    uint8           Spare[3];
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
    uint32                    EVS_EventBurstMax;
} CFE_EVS_Global_t;

/*
 *  Global variable specific to EVS module
 */
extern CFE_EVS_Global_t CFE_EVS_Global;

/*****************************  Function Prototypes   **********************************/

/*
 * Functions used within this module and by the unit test
 */

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Task Initialization
 *
 * This function performs any necessary EVS task initialization.
 */
int32 CFE_EVS_TaskInit(void);

/*
 * EVS Message Handler Functions
 */

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Message Handler Function
 *
 * Request for housekeeping status telemetry packet.
 */
int32 CFE_EVS_SendHkCmd(const CFE_EVS_SendHkCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Message Handler Function
 *
 * This function processes "noop" commands received on the EVS command pipe.
 */
int32 CFE_EVS_NoopCmd(const CFE_EVS_NoopCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Message Handler Function
 *
 * This function processes "clear log" commands received on the EVS command pipe.
 */
int32 CFE_EVS_ClearLogCmd(const CFE_EVS_ClearLogCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Message Handler Function
 *
 * This function resets all the global counter variables that are part of the task telemetry.
 */
int32 CFE_EVS_ResetCountersCmd(const CFE_EVS_ResetCountersCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Message Handler Function
 *
 * This routine sets the filter mask for the given event_id in the
 * calling task's filter array
 */
int32 CFE_EVS_SetFilterCmd(const CFE_EVS_SetFilterCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Message Handler Function
 *
 * This routine sets the command given ports to an enabled state
 * @note Shifting is done so the value not masked off is placed in the ones spot:
 * necessary for comparing with true.
 */
int32 CFE_EVS_EnablePortsCmd(const CFE_EVS_EnablePortsCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Message Handler Function
 *
 * This routine sets the command given ports to a disabled state
 * @note Shifting is done so the value not masked off is placed in the ones spot:
 * necessary for comparing with true.
 */
int32 CFE_EVS_DisablePortsCmd(const CFE_EVS_DisablePortsCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Message Handler Function
 *
 * This routine sets the given event types to an enabled state across all
 * registered applications
 */
int32 CFE_EVS_EnableEventTypeCmd(const CFE_EVS_EnableEventTypeCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Message Handler Function
 *
 * This routine sets the given event types to a disabled state across all
 * registered applications
 */
int32 CFE_EVS_DisableEventTypeCmd(const CFE_EVS_DisableEventTypeCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Message Handler Function
 *
 * This routine sets the Event Format Mode
 */
int32 CFE_EVS_SetEventFormatModeCmd(const CFE_EVS_SetEventFormatModeCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Message Handler Function
 *
 * This routine sets the given event type for the given application identifier to an
 * enabled state
 */
int32 CFE_EVS_EnableAppEventTypeCmd(const CFE_EVS_EnableAppEventTypeCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Message Handler Function
 *
 * This routine sets the given event type for the given application identifier to a
 * disabled state
 */
int32 CFE_EVS_DisableAppEventTypeCmd(const CFE_EVS_DisableAppEventTypeCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Message Handler Function
 *
 * This routine enables application events for the given application identifier
 */
int32 CFE_EVS_EnableAppEventsCmd(const CFE_EVS_EnableAppEventsCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Message Handler Function
 *
 * This routine disables application events for the given application identifier
 */
int32 CFE_EVS_DisableAppEventsCmd(const CFE_EVS_DisableAppEventsCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Message Handler Function
 *
 * This routine sets the application event counter to zero for the given
 * application identifier
 */
int32 CFE_EVS_ResetAppCounterCmd(const CFE_EVS_ResetAppCounterCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Message Handler Function
 *
 * This routine sets the application event filter counter to zero for the given
 * application identifier and event identifier
 */
int32 CFE_EVS_ResetFilterCmd(const CFE_EVS_ResetFilterCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Message Handler Function
 *
 * This routine adds the given event filter for the given application
 * identifier and event identifier.
 */
int32 CFE_EVS_AddEventFilterCmd(const CFE_EVS_AddEventFilterCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Message Handler Function
 *
 * This routine deletes the event filter for the given application
 * identifier and event identifier
 */
int32 CFE_EVS_DeleteEventFilterCmd(const CFE_EVS_DeleteEventFilterCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Message Handler Function
 *
 * This routine writes all application data to a file for all applications that
 * have registered with the EVS.  The application data includes the Application ID,
 * Active Flag, Event Count, Event Types Active Flag, and Filter Data.
 */
int32 CFE_EVS_WriteAppDataFileCmd(const CFE_EVS_WriteAppDataFileCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Message Handler Function
 *
 * This routine sets all application event filter counters to zero for the given
 * application identifier
 */
int32 CFE_EVS_ResetAllFiltersCmd(const CFE_EVS_ResetAllFiltersCmd_t *data);

#endif /* CFE_EVS_TASK_H */
