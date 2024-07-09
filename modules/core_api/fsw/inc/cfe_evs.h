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

#ifndef CFE_EVS_H
#define CFE_EVS_H

/********************************** Include Files  ************************************/
#include "common_types.h" /* Basic data types */
#include "cfe_error.h"
#include "cfe_evs_api_typedefs.h"
#include "cfe_es_api_typedefs.h"
#include "cfe_time_api_typedefs.h"

/*
** Utility macros to make for simpler/more compact/readable code.
*/
#define CFE_EVS_Send(E, T, ...)  CFE_EVS_SendEvent((E), CFE_EVS_EventType_##T, __VA_ARGS__)
#define CFE_EVS_SendDbg(E, ...)  CFE_EVS_Send(E, DEBUG, __VA_ARGS__)
#define CFE_EVS_SendInfo(E, ...) CFE_EVS_Send(E, INFORMATION, __VA_ARGS__)
#define CFE_EVS_SendErr(E, ...)  CFE_EVS_Send(E, ERROR, __VA_ARGS__)
#define CFE_EVS_SendCrit(E, ...) CFE_EVS_Send(E, CRITICAL, __VA_ARGS__)

/****************** Function Prototypes **********************/

/** @defgroup CFEAPIEVSReg cFE Registration APIs
 * @{
 */

/**
** \brief Register an application for receiving event services
**
** \par Description
**          This routine registers an application with event services and allocates/initializes
**          the internal data structures used to support this application's events.  An application
**          may not send events unless it has called this routine.  The routine also accepts a filter
**          array structure for applications requiring event filtering.  In the current implementation
**          of the EVS, only the binary filtering scheme is supported.  See section TBD of the cFE
**          Application Programmer's Guide for a description of the behavior of binary filters.
**          Applications may call #CFE_EVS_Register more than once, but each call will wipe out all
**          filters registered by previous calls (filter registration is NOT cumulative).
**
** \par Assumptions, External Events, and Notes:
**          Note: Event filters can be added, deleted or modified by ground commands.  All filtering
**                schemes include a default setting that results in no filtering (such as #CFE_EVS_NO_FILTER
**                for binary filters).
**
**          <b> Filter Scheme: </b> Binary <BR>
**          <b> Code: </b> CFE_EVS_EventFilter_BINARY <BR>
**          <b> Filter Structure: </b>
**          \code
**                     typedef struct CFE_EVS_BinFilter {
**                           uint16  EventID,
**                           uint16  Mask ;
**                     } CFE_EVS_BinFilter_t;
**          \endcode
**
** \param[in] Filters            Pointer to an array of event message filters, or NULL if no filtering is desired.
**                               The structure of an event message filter depends on the FilterScheme selected.
**                               (see Filter Schemes mentioned above)
**
** \param[in] NumEventFilters    The number of event message filters included in this call.  This must be less than
**                               or equal to the maximum number of events allowed per application
**                               (#CFE_PLATFORM_EVS_MAX_EVENT_FILTERS).
**
** \param[in] FilterScheme       The event filtering scheme that this application will use.  For the first
**                               implementation of the event services, only filter type #CFE_EVS_EventFilter_BINARY
**                               will be supported.
**
** \return Execution status below or from #CFE_ES_GetAppID, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS                 \copybrief CFE_SUCCESS
** \retval #CFE_EVS_APP_FILTER_OVERLOAD \copybrief CFE_EVS_APP_FILTER_OVERLOAD
** \retval #CFE_EVS_UNKNOWN_FILTER      \copybrief CFE_EVS_UNKNOWN_FILTER
** \retval #CFE_EVS_APP_ILLEGAL_APP_ID  \copybrief CFE_EVS_APP_ILLEGAL_APP_ID
** \retval #CFE_ES_BAD_ARGUMENT         \copybrief CFE_ES_BAD_ARGUMENT
**/
CFE_Status_t CFE_EVS_Register(const void *Filters, uint16 NumEventFilters, uint16 FilterScheme);
/**@}*/

/** @defgroup CFEAPIEVSSend cFE Send Event APIs
 * @{
 */

/**
** \brief Generate a software event.
**
** \par Description
**          This routine generates a software event message.  If the EventID is not filtered,
**          the event will be sent as a software bus message, optionally logged in the local
**          event log, and optionally sent as an ASCII text string out the enabled output port(s).
**
** \par Assumptions, External Events, and Notes:
**          This API only works within the context of a registered application or core service.
**          For messages outside the context of a registered application (for example early
**          in app initialization or if registration fails) #CFE_ES_WriteToSysLog can be used
**          for reporting.
**
** \param[in] EventID            A numeric literal used to uniquely identify an application event.
**                               The \c EventID is defined and supplied by the application sending the event.
**
** \param[in] EventType          A numeric literal used to classify an event, one of:
**                                   \arg #CFE_EVS_EventType_DEBUG
**                                   \arg #CFE_EVS_EventType_INFORMATION
**                                   \arg #CFE_EVS_EventType_ERROR
**                                   \arg #CFE_EVS_EventType_CRITICAL
**
** \param[in] Spec               A pointer to a null terminated text string @nonnull describing the output format
**                               for the event.  This is the same type of format string used for the ANSI
**                               \c printf function.  Nominally the post-conversion string is limited to 80
**                               characters, but this limit is configurable through the parameter
**                               #CFE_MISSION_EVS_MAX_MESSAGE_LENGTH.  Characters beyond the limit will be truncated.
**                               Do not use floating point conversions (%f, %e, %E, %g, and %G) in the format
**                               string unless your application will be running in a system that supports
**                               floating point arithmetic.  Do not use non-printable characters (\\t, \\n, etc.)
**                               in the format string; they will mess up the formatting when the events are
**                               displayed on the ground system.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS                \copybrief CFE_SUCCESS
** \retval #CFE_EVS_APP_NOT_REGISTERED \copybrief CFE_EVS_APP_NOT_REGISTERED
** \retval #CFE_EVS_APP_ILLEGAL_APP_ID \copybrief CFE_EVS_APP_ILLEGAL_APP_ID
** \retval #CFE_EVS_INVALID_PARAMETER  \copybrief CFE_EVS_INVALID_PARAMETER
**
** \sa #CFE_EVS_SendEventWithAppID, #CFE_EVS_SendTimedEvent
**
**/
CFE_Status_t CFE_EVS_SendEvent(uint16 EventID, CFE_EVS_EventType_Enum_t EventType, const char *Spec, ...)
    OS_PRINTF(3, 4);

/**
** \brief Generate a software event given the specified Application ID.
**
** \par Description
**          This routine generates a software event message.  If the EventID is not filtered,
**          the event will be sent as a software bus message, optionally logged in the local
**          event log, and optionally sent as an ASCII text string out the enabled output port(s).
**          Note that this function should really only be used from within an API in order to
**          preserve the context of an Application's event.  In general, #CFE_EVS_SendEvent should be used.
**
** \par Assumptions, External Events, and Notes:
**          The Application ID must correspond to a registered application or core service.
**          For messages outside the context of a registered application (for example early
**          in app initialization or if registration fails) #CFE_ES_WriteToSysLog can be used
**          for reporting.
**
** \param[in] EventID            A numeric literal used to uniquely identify an application event.
**                               The \c EventID is defined and supplied by the application sending the event.
**
** \param[in] EventType          A numeric literal used to classify an event, one of:
**                                   \arg #CFE_EVS_EventType_DEBUG
**                                   \arg #CFE_EVS_EventType_INFORMATION
**                                   \arg #CFE_EVS_EventType_ERROR
**                                   \arg #CFE_EVS_EventType_CRITICAL
**
** \param[in] AppID              The Application ID from which the event message should appear.
**
** \param[in] Spec               A pointer to a null terminated text string @nonnull describing the output format
**                               for the event.  This is the same type of format string used for the ANSI
**                               \c printf function.  Nominally the post-conversion string is limited to 80
**                               characters, but this limit is configurable through the parameter
**                               #CFE_MISSION_EVS_MAX_MESSAGE_LENGTH.  Characters beyond the limit will be truncated.
**                               Do not use floating point conversions (%f, %e, %E, %g, and %G) in the format
**                               string unless your application will be running in a system that supports
**                               floating point arithmetic.  Do not use non-printable characters (\\t, \\n, etc.)
**                               in the format string; they will mess up the formatting when the events are
**                               displayed on the ground system.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS                \copybrief CFE_SUCCESS
** \retval #CFE_EVS_APP_NOT_REGISTERED \copybrief CFE_EVS_APP_NOT_REGISTERED
** \retval #CFE_EVS_APP_ILLEGAL_APP_ID \copybrief CFE_EVS_APP_ILLEGAL_APP_ID
** \retval #CFE_EVS_INVALID_PARAMETER  \copybrief CFE_EVS_INVALID_PARAMETER
**
** \sa #CFE_EVS_SendEvent, #CFE_EVS_SendTimedEvent
**
**/
CFE_Status_t CFE_EVS_SendEventWithAppID(uint16 EventID, CFE_EVS_EventType_Enum_t EventType, CFE_ES_AppId_t AppID,
                                        const char *Spec, ...) OS_PRINTF(4, 5);

/**
** \brief Generate a software event with a specific time tag.
**
** \par Description
**          This routine is the same as #CFE_EVS_SendEvent except that the caller specifies the event time
**          instead of having the EVS use the current spacecraft time.  This routine should be used in
**          situations where an error condition is detected at one time, but the event message is reported
**          at a later time.
**
** \par Assumptions, External Events, and Notes:
**          This API only works within the context of a registered application or core service.
**          For messages outside the context of a registered application (for example early
**          in app initialization or if registration fails) #CFE_ES_WriteToSysLog can be used
**          for reporting.
**
** \param[in] Time               The time to include in the event.  This will usually be a time returned
**                               by the function #CFE_TIME_GetTime.
**
** \param[in] EventID            A numeric literal used to uniquely identify an application event.
**                               The \c EventID is defined and supplied by the application sending the event.
**
** \param[in] EventType          A numeric literal used to classify an event, one of:
**                                   \arg #CFE_EVS_EventType_DEBUG
**                                   \arg #CFE_EVS_EventType_INFORMATION
**                                   \arg #CFE_EVS_EventType_ERROR
**                                   \arg #CFE_EVS_EventType_CRITICAL
**
** \param[in] Spec               A pointer to a null terminated text string @nonnull describing the output format
**                               for the event.  This is the same type of format string used for the ANSI
**                               \c printf function.  Nominally the post-conversion string is limited to 80
**                               characters, but this limit is configurable through the parameter
**                               #CFE_MISSION_EVS_MAX_MESSAGE_LENGTH.  Characters beyond the limit will be truncated.
**                               Do not use floating point conversions (%f, %e, %E, %g, and %G) in the format
**                               string unless your application will be running in a system that supports
**                               floating point arithmetic.  Do not use non-printable characters (\\t, \\n, etc.)
**                               in the format string; they will mess up the formatting when the events are
**                               displayed on the ground system.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS                \copybrief CFE_SUCCESS
** \retval #CFE_EVS_APP_NOT_REGISTERED \copybrief CFE_EVS_APP_NOT_REGISTERED
** \retval #CFE_EVS_APP_ILLEGAL_APP_ID \copybrief CFE_EVS_APP_ILLEGAL_APP_ID
** \retval #CFE_EVS_INVALID_PARAMETER  \copybrief CFE_EVS_INVALID_PARAMETER
**
** \sa #CFE_EVS_SendEvent, #CFE_EVS_SendEventWithAppID
**
**/
CFE_Status_t CFE_EVS_SendTimedEvent(CFE_TIME_SysTime_t Time, uint16 EventID, CFE_EVS_EventType_Enum_t EventType,
                                    const char *Spec, ...) OS_PRINTF(4, 5);
/**@}*/

/** @defgroup CFEAPIEVSResetFilter cFE Reset Event Filter APIs
 * @{
 */

/**
** \brief Resets the calling application's event filter for a single event ID.
**
** \par Description
**          Resets the filter such that the next event is treated like the first.
**          For example, if the filter was set to only send the first event, the
**          next event following the reset would be sent.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in] EventID            A numeric literal used to uniquely identify an application event.
**                               The \c EventID is defined and supplied by the application sending the event.
**
** \return Execution status below or from #CFE_ES_GetAppID, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS                \copybrief CFE_SUCCESS
** \retval #CFE_EVS_APP_NOT_REGISTERED \copybrief CFE_EVS_APP_NOT_REGISTERED
** \retval #CFE_EVS_APP_ILLEGAL_APP_ID \copybrief CFE_EVS_APP_ILLEGAL_APP_ID
** \retval #CFE_EVS_EVT_NOT_REGISTERED \copybrief CFE_EVS_EVT_NOT_REGISTERED
**
** \sa #CFE_EVS_ResetAllFilters
**
**/
CFE_Status_t CFE_EVS_ResetFilter(uint16 EventID);

/**
** \brief Resets all of the calling application's event filters.
**
** \par Description
**          This routine resets all the calling application's event filter counters to zero, providing a
**          quick and convenient method for resetting event filters.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \return Execution status below or from #CFE_ES_GetAppID, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS                \copybrief CFE_SUCCESS
** \retval #CFE_EVS_APP_NOT_REGISTERED \copybrief CFE_EVS_APP_NOT_REGISTERED
** \retval #CFE_EVS_APP_ILLEGAL_APP_ID \copybrief CFE_EVS_APP_ILLEGAL_APP_ID
**
** \sa #CFE_EVS_ResetFilter
**
**/
CFE_Status_t CFE_EVS_ResetAllFilters(void);
/**@}*/

#endif /* CFE_EVS_H */
