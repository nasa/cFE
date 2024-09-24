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
 *  cFE Event Services Event IDs
 */

#ifndef CFE_EVS_EVENTS_H
#define CFE_EVS_EVENTS_H

/**
 * \name EVS event IDs
 */
/**\{*/

/**
 * \brief EVS No-op Command Success Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  \link #CFE_EVS_NOOP_CC EVS NO-OP command \endlink success.
 */
#define CFE_EVS_NOOP_EID 0

/**
 * \brief EVS Initialization Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  Event Services Task initialization complete.
 */
#define CFE_EVS_STARTUP_EID 1

/**
 * \brief EVS Write Event Log Command File Write Entry Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_EVS_WRITE_LOG_DATA_FILE_CC EVS Write Event Log Command \endlink failure
 *  writing data to the file.
 */
#define CFE_EVS_ERR_WRLOGFILE_EID 2

/**
 * \brief EVS Write Event Log Command Filename Parse or File Create Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_EVS_WRITE_LOG_DATA_FILE_CC EVS Write Event Log Command \endlink failure
 *  parsing the file name or during open/creation of the file.  OVERLOADED
 */
#define CFE_EVS_ERR_CRLOGFILE_EID 3

/**
 * \brief EVS Invalid Message ID Received Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Invalid message ID received on the EVS message pipe.
 */
#define CFE_EVS_ERR_MSGID_EID 5

/**
 * \brief EVS Command Event Not Registered For Filtering Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  An EVS command handler failure due to the event not being registered for filtering. OVERLOADED
 */
#define CFE_EVS_ERR_EVTIDNOREGS_EID 6

/**
 * \brief EVS Command Application Not Registered With EVS Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  An EVS command handler failure due to the referenced application not being registered with EVS. OVERLOADED
 */
#define CFE_EVS_ERR_APPNOREGS_EID 7

/**
 * \brief EVS Command Get Application Data Failure Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  An EVS command handler failure retrieving the application data. OVERLOADED
 */
#define CFE_EVS_ERR_ILLAPPIDRANGE_EID 8

/**
 * \brief EVS Command Get Application ID Failure Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  An EVS command handler failure retrieving the application ID. OVERLOADED
 */
#define CFE_EVS_ERR_NOAPPIDFOUND_EID 9

/**
 * \brief EVS Set Event Format Command Invalid Format Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_EVS_SET_EVENT_FORMAT_MODE_CC EVS Set Event Format Command \endlink failure
 *  due to invalid format argument.
 */
#define CFE_EVS_ERR_ILLEGALFMTMOD_EID 10

/**
 * \brief EVS Add Filter Command Max Filters Exceeded Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_EVS_ADD_EVENT_FILTER_CC EVS Add Filter Command \endlink failure
 *  due to exceeding the maximum number of filters.
 */
#define CFE_EVS_ERR_MAXREGSFILTER_EID 11

/**
 * \brief EVS Write Application Data Command Write Data Failure Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_EVS_WRITE_APP_DATA_FILE_CC Write Application Data Command \endlink failure
 *  to write application EVS data.
 */
#define CFE_EVS_ERR_WRDATFILE_EID 12

/**
 * \brief EVS Write Application Data Command Filename Parse or File Create Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_EVS_WRITE_APP_DATA_FILE_CC Write Application Data Command \endlink failed
 *  to parse the filename or open/create the file. OVERLOADED
 */
#define CFE_EVS_ERR_CRDATFILE_EID 13

/**
 * \brief EVS Write File Header to Log File Failure Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Bytes written during Write File Header to Log File was not equal to the expected header size.
 */
#define CFE_EVS_WRITE_HEADER_ERR_EID 14

/**
 * \brief EVS Invalid Command Code Received Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Invalid command code for message ID #CFE_EVS_CMD_MID received on the EVS message pipe.
 */
#define CFE_EVS_ERR_CC_EID 15

/**
 * \brief EVS Reset Counters Command Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  \link #CFE_EVS_RESET_COUNTERS_CC EVS Reset Counters Command \endlink success.
 */
#define CFE_EVS_RSTCNT_EID 16

/**
 * \brief EVS Set Filter Command Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  \link #CFE_EVS_SET_FILTER_CC EVS Set Filter Command \endlink success.
 */
#define CFE_EVS_SETFILTERMSK_EID 17

/**
 * \brief EVS Enable Ports Command Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  \link #CFE_EVS_ENABLE_PORTS_CC EVS Enable Ports Command \endlink success.
 */
#define CFE_EVS_ENAPORT_EID 18

/**
 * \brief EVS Disable Ports Command Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  \link #CFE_EVS_DISABLE_PORTS_CC EVS Disable Ports Command \endlink success.
 */
#define CFE_EVS_DISPORT_EID 19

/**
 * \brief EVS Enable Event Type Command Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  \link #CFE_EVS_ENABLE_EVENT_TYPE_CC EVS Enable Event Type Command \endlink success.
 */
#define CFE_EVS_ENAEVTTYPE_EID 20

/**
 * \brief EVS Disable Event Type Command Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  \link #CFE_EVS_DISABLE_EVENT_TYPE_CC EVS Disable Event Type Command \endlink success.
 */
#define CFE_EVS_DISEVTTYPE_EID 21

/**
 * \brief EVS Set Event Format Mode Command Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  \link #CFE_EVS_SET_EVENT_FORMAT_MODE_CC EVS Set Event Format Mode Command \endlink success.
 */
#define CFE_EVS_SETEVTFMTMOD_EID 22

/**
 * \brief EVS Enable App Event Type Command Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  \link #CFE_EVS_ENABLE_APP_EVENT_TYPE_CC EVS Enable App Event Type Command \endlink success.
 */
#define CFE_EVS_ENAAPPEVTTYPE_EID 23

/**
 * \brief EVS Disable App Event Type Command Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  \link #CFE_EVS_DISABLE_APP_EVENT_TYPE_CC EVS Disable App Event Type Command \endlink success.
 */
#define CFE_EVS_DISAPPENTTYPE_EID 24

/**
 * \brief EVS Enable App Events Command Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  \link #CFE_EVS_ENABLE_APP_EVENTS_CC EVS Enable App Events Command \endlink success.
 */
#define CFE_EVS_ENAAPPEVT_EID 25

/**
 * \brief EVS Disable App Events Command Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  \link #CFE_EVS_DISABLE_APP_EVENTS_CC EVS Disable App Events Command \endlink success.
 */
#define CFE_EVS_DISAPPEVT_EID 26

/**
 * \brief EVS Reset App Event Counter Command Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  \link #CFE_EVS_RESET_APP_COUNTER_CC EVS Reset App Event Counter Command \endlink success.
 */
#define CFE_EVS_RSTEVTCNT_EID 27

/**
 * \brief EVS Reset App Event Filter Command Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  \link #CFE_EVS_RESET_FILTER_CC EVS Reset App Event Filter Command \endlink success.
 */
#define CFE_EVS_RSTFILTER_EID 28

/**
 * \brief EVS Reset All Filters Command Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  \link #CFE_EVS_RESET_ALL_FILTERS_CC EVS Reset All FIlters Command \endlink success.
 */
#define CFE_EVS_RSTALLFILTER_EID 29

/**
 * \brief EVS Add Event Filter Command Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  \link #CFE_EVS_ADD_EVENT_FILTER_CC EVS Add Event Filter Command \endlink success.
 */
#define CFE_EVS_ADDFILTER_EID 30

/**
 * \brief EVS Delete Event Filter Command Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  \link #CFE_EVS_DELETE_EVENT_FILTER_CC EVS Delete Event Filter Command \endlink success.
 */
#define CFE_EVS_DELFILTER_EID 31

/**
 * \brief EVS Write Application Data Command Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  \link #CFE_EVS_WRITE_APP_DATA_FILE_CC EVS Write Application Data Command \endlink success.
 */
#define CFE_EVS_WRDAT_EID 32

/**
 * \brief EVS Write Event Log Command Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  \link #CFE_EVS_WRITE_LOG_DATA_FILE_CC EVS Write Event Log Command \endlink success.
 */
#define CFE_EVS_WRLOG_EID 33

/**
 * \brief EVS Add Filter Command Duplicate Registration Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_EVS_ADD_EVENT_FILTER_CC EVS Add Filter Command \endlink failure due
 *  to event already being registered for filtering.
 */
#define CFE_EVS_EVT_FILTERED_EID 37

/**
 * \brief EVS Set Log Mode Command Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  \link #CFE_EVS_SET_LOG_MODE_CC EVS Set Log Mode Command \endlink success.
 */
#define CFE_EVS_LOGMODE_EID 38

/**
 * \brief EVS Set Log Mode Command Invalid Mode Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_EVS_SET_LOG_MODE_CC EVS Set Log Mode Command \endlink failure
 *  due to invalid log mode.
 */
#define CFE_EVS_ERR_LOGMODE_EID 39

/**
 * \brief EVS Port Or Event Type Bitmask Invalid Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Invalid bitmask for EVS port or event type.  OVERLOADED
 */
#define CFE_EVS_ERR_INVALID_BITMASK_EID 40

/**
 * \brief EVS Send Event API App Not Registered With EVS Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  An EVS Send Event API called for application not registered with EVS.
 */
#define CFE_EVS_ERR_UNREGISTERED_EVS_APP 41

/**
 * \brief EVS Filter Max Count Reached Event ID
 *
 *  \par Type: INFORMATIONAL
 *
 *  \par Cause:
 *
 *  Filter count for the event reached CFE_EVS_MAX_FILTER_COUNT and is latched until filter is reset.
 */
#define CFE_EVS_FILTER_MAX_EID 42

/**
 * \brief EVS Invalid Command Length Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Invalid length for the command code in message ID #CFE_EVS_CMD_MID received on the EVS message pipe.
 */
#define CFE_EVS_LEN_ERR_EID 43

/**
 * \brief EVS Events Squelched Error Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Events generated in app at a rate in excess of
 *  #CFE_PLATFORM_EVS_MAX_APP_EVENT_BURST in one moment or
 *  #CFE_PLATFORM_EVS_APP_EVENTS_PER_SEC sustained
 */
#define CFE_EVS_SQUELCHED_ERR_EID 44
/**\}*/

#endif /* CFE_EVS_EVENTS_H */
