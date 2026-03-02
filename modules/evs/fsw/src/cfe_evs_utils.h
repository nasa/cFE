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
 *  Title:    Event Services Task and API - Utility functions.
 *
 *  Purpose:
 *            Unit specification for the event services utility functions.
 *
 *  Contents:
 *       I.  macro and constant type definitions
 *      II.  EVS utility internal structures
 *     III.  function prototypes
 *
 *  Design Notes:
 *
 *  References:
 *     Flight Software Branch C Coding Standard Version 1.0a
 *
 */

#ifndef CFE_EVS_UTILS_H
#define CFE_EVS_UTILS_H

/********************* Include Files  ************************/

#include "cfe_evs_task.h" /* EVS internal definitions */
#include "cfe_resourceid.h"
#include "cfe_es_api_typedefs.h"
#include "cfe_time_api_typedefs.h"

/* ==============   Section I: Macro and Constant Type Definitions   =========== */

/* Macro representing all event types turned on */
#define CFE_EVS_ALL_EVENT_TYPES_MASK \
    (CFE_EVS_DEBUG_BIT | CFE_EVS_INFORMATION_BIT | CFE_EVS_ERROR_BIT | CFE_EVS_CRITICAL_BIT)

/* ==============   Section II: Internal Structures ============ */

/* ==============   Section III: Function Prototypes =========== */

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Obtain the EVS app record for the given ID
 *
 * This only obtains a pointer to where the record should be, it does
 * not check/confirm that the record actually is for the given AppID.
 * Use EVS_AppDataIsMatch() to determine if the record is valid.
 *
 * @sa EVS_AppDataIsMatch()
 *
 * @param[in]   AppID   AppID to find
 * @returns Pointer to app table entry, or NULL if ID is invalid.
 */
EVS_AppData_t *EVS_GetAppDataByID(CFE_ES_AppId_t AppID);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Obtain the context information for the currently running app
 *
 * Obtains both the AppData record (pointer) and AppID for the current context.
 *
 * @param[out]   AppDataOut     Location to store App Data record pointer
 * @param[out]   AppIDOut       Location to store AppID
 * @returns CFE_SUCCESS if successful, or relevant error code.
 */
int32 EVS_GetCurrentContext(EVS_AppData_t **AppDataOut, CFE_ES_AppId_t *AppIDOut);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Check if an EVS app record is in use or free/empty
 *
 * This routine checks if the EVS app entry is in use or if it is free
 *
 * As this dereferences fields within the record, global data must be
 * locked prior to invoking this function.
 *
 * @param[in]   AppDataPtr   pointer to app table entry
 * @returns true if the entry is in use/configured, or false if it is free/empty
 */
static inline bool EVS_AppDataIsUsed(EVS_AppData_t *AppDataPtr)
{
    return CFE_RESOURCEID_TEST_DEFINED(AppDataPtr->AppID);
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Get the ID value from an EVS table entry
 *
 * This routine converts the table entry back to an abstract ID.
 *
 * @param[in]   AppDataPtr   pointer to app table entry
 * @returns AppID of entry
 */
static inline CFE_ES_AppId_t EVS_AppDataGetID(EVS_AppData_t *AppDataPtr)
{
    /*
     * The initial implementation does not store the ID in the entry;
     * the ID is simply the zero-based index into the table.
     */
    return AppDataPtr->AppID;
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Marks an EVS table entry as used (not free)
 *
 * This sets the internal field(s) within this entry, and marks
 * it as being associated with the given app ID.
 *
 * As this dereferences fields within the record, global data must be
 * locked prior to invoking this function.
 *
 * @param[in]   AppDataPtr   pointer to app table entry
 * @param[in]   AppID       the app ID of this entry
 */
static inline void EVS_AppDataSetUsed(EVS_AppData_t *AppDataPtr, CFE_ES_AppId_t AppID)
{
    AppDataPtr->AppID = AppID;
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Set an EVS table entry free (not used)
 *
 * This clears the internal field(s) within this entry, and allows the
 * memory to be re-used in the future.
 *
 * @param[in]   AppDataPtr   pointer to app table entry
 */
static inline void EVS_AppDataSetFree(EVS_AppData_t *AppDataPtr)
{
    AppDataPtr->AppID = CFE_ES_APPID_UNDEFINED;
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Check if an EVS record is a match for the given AppID
 *
 * This routine confirms that the previously-located record is valid
 * and matches the expected app ID.
 *
 * @param[in]   AppDataPtr   pointer to app table entry
 * @param[in]   AppID       expected app ID
 * @returns true if the entry matches the given app ID
 */
static inline bool EVS_AppDataIsMatch(EVS_AppData_t *AppDataPtr, CFE_ES_AppId_t AppID)
{
    return (AppDataPtr != NULL && CFE_RESOURCEID_TEST_EQUAL(AppDataPtr->AppID, AppID));
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Retrieve app details by app name
 *
 * This routine returns the application ID and
 * status specifying the validity of the ID
 */
int32 EVS_GetApplicationInfo(EVS_AppData_t **AppDataOut, const char *pAppName);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Generate "not registered" error event
 *
 * This routine sends one "not registered" event per application
 * Assumptions and Notes:
 */
int32 EVS_NotRegistered(EVS_AppData_t *AppDataPtr, CFE_ES_AppId_t CallerID);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Check if event is filtered
 *
 * This routine returns true if the given event identifier and event type
 * is filtered for the given application identifier.  Otherwise a value of
 * false is returned.
 */
bool EVS_IsFiltered(EVS_AppData_t *AppDataPtr, uint16 EventID, CFE_EVS_EventType_Enum_t EventType);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Check if event is squelched
 *
 * This routine returns false if the squelch token counter has become negative.
 * Otherwise a value of true is returned. In addition, it updates the squelch
 * token counter based on time, and emits an event message if squelched.
 *
 * If #CFE_PLATFORM_EVS_MAX_APP_EVENT_BURST == 0, this function returns true and is otherwise a no-op
 */
bool EVS_CheckAndIncrementSquelchTokens(EVS_AppData_t *AppDataPtr);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Find the filter record corresponding to the given event ID
 *
 * This routine searches and returns an index to the given Event ID with the
 * given application filter array.
 */
EVS_BinFilter_t *EVS_FindEventID(uint16 EventID, EVS_BinFilter_t *FilterArray);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Set event types using bitmask and bool
 *
 * This routine sets event types selected in a BitMask, true for on, false for off
 */
void EVS_SetTypes(EVS_AppData_t *AppDataPtr, uint8 BitMask, bool State);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Converts event array to a BitMask
 *
 * This routine converts the EventTypesActive array in an AppData Struct to a BitMask
 *
 * @returns uint8 BitMask for active event types in an app
 */
uint8 EVS_EventArrayToBitMask(const EVS_AppData_t *AppDataPtr);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Send all configured telemetry for an event
 *
 * This routine sends an EVS event message out the software bus and all
 * enabled output ports
 * @note This always generates a "long" style message for logging purposes.
 * If configured for long events the same message is sent on the software bus as well.
 * If configured for short events, a separate short message is generated using a subset
 * of the information from the long message.
 */
void EVS_GenerateEventTelemetry(EVS_AppData_t *AppDataPtr, uint16 EventID, CFE_EVS_EventType_Enum_t EventType,
                                const CFE_TIME_SysTime_t *Time, const char *MsgSpec, va_list ArgPtr);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Internal function to send an event
 *
 * This routine allows EVS to send events without having to verify
 * that the caller has a valid AppID and has registered with EVS.
 * This routine also does not need to acquire the mutex semaphore,
 * which can be time consuming on some platforms.
 */
int32 EVS_SendEvent(uint16 EventID, CFE_EVS_EventType_Enum_t EventType, const char *Spec, ...);

/**
 * @brief Checks if the provided BitMask is invalid.
 *
 * This function evaluates whether the given BitMask is either zero or exceeds the maximum allowed
 * value defined by CFE_EVS_ALL_EVENT_TYPES_MASK (which represents all events types turned on).
 * If the BitMask is invalid, an error event is sent and the function returns true.
 *
 * @param BitMask The bitmask to be checked.
 * @param CommandCode The command code associated with the bitmask.
 *
 * @return true if the BitMask is invalid, false otherwise.
 */
bool EVS_IsInvalidBitMask(uint32 BitMask, uint16 CommandCode);

#endif /* CFE_EVS_UTILS_H */
