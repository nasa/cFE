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
 *  Purpose:
 *  cFE Executive Services (EVS) Command and Telemetry packet definition file.
 *
 *  References:
 *     Flight Software Branch C Coding Standard Version 1.0a
 *     cFE Flight Software Application Developers Guide
 *
 *  Notes:
 *
 */
#ifndef CFE_EVS_MSGSTRUCT_H
#define CFE_EVS_MSGSTRUCT_H

/************************************************************************
 * Includes
 ************************************************************************/
#include "common_types.h"
#include "cfe_evs_msgdefs.h"
#include "cfe_evs_extern_typedefs.h"
#include "cfe_msg_hdr.h"

/******************  Structure Definitions *********************/

/**
** \brief Command with no additional arguments
**/
typedef struct CFE_EVS_NoArgsCmd
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command header */
} CFE_EVS_NoArgsCmd_t;

/*
 * The NoArgsCmd is shared by several EVS command handlers.
 * Create a unique type for each one so the prototypes will follow the naming pattern,
 * allowing each command to evolve independently.
 */
typedef CFE_EVS_NoArgsCmd_t CFE_EVS_NoopCmd_t;
typedef CFE_EVS_NoArgsCmd_t CFE_EVS_ResetCountersCmd_t;
typedef CFE_EVS_NoArgsCmd_t CFE_EVS_ClearLogCmd_t;
typedef CFE_EVS_NoArgsCmd_t CFE_EVS_SendHkCmd_t;

/**
** \brief Write Event Log to File Command Payload
**
** For command details, see #CFE_EVS_WRITE_LOG_DATA_FILE_CC
**
**/
typedef struct CFE_EVS_LogFileCmd_Payload
{
    char LogFilename[CFE_MISSION_MAX_PATH_LEN]; /**< \brief Filename where log data is to be written */
} CFE_EVS_LogFileCmd_Payload_t;

/**
 * \brief Write Event Log to File Command
 */
typedef struct CFE_EVS_WriteLogDataFileCmd
{
    CFE_MSG_CommandHeader_t      CommandHeader; /**< \brief Command header */
    CFE_EVS_LogFileCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_EVS_WriteLogDataFileCmd_t;

/**
** \brief Write Event Services Application Information to File Command Payload
**
** For command details, see #CFE_EVS_WRITE_APP_DATA_FILE_CC
**
**/
typedef struct CFE_EVS_AppDataCmd_Payload
{
    char AppDataFilename[CFE_MISSION_MAX_PATH_LEN]; /**< \brief Filename where application data is to be written */
} CFE_EVS_AppDataCmd_Payload_t;

/**
 * \brief Write Event Services Application Information to File Command
 */
typedef struct CFE_EVS_WriteAppDataFileCmd
{
    CFE_MSG_CommandHeader_t      CommandHeader; /**< \brief Command header */
    CFE_EVS_AppDataCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_EVS_WriteAppDataFileCmd_t;

/**
** \brief Set Log Mode Command Payload
**
** For command details, see #CFE_EVS_SET_LOG_MODE_CC
**
**/
typedef struct CFE_EVS_SetLogMode_Payload
{
    CFE_EVS_LogMode_Enum_t LogMode; /**< \brief Mode to use in the command*/
    uint8                  Spare;   /**< \brief Pad to even byte*/
} CFE_EVS_SetLogMode_Payload_t;

/**
 * \brief Set Log Mode Command
 */
typedef struct CFE_EVS_SetLogModeCmd
{
    CFE_MSG_CommandHeader_t      CommandHeader; /**< \brief Command header */
    CFE_EVS_SetLogMode_Payload_t Payload;       /**< \brief Command payload */
} CFE_EVS_SetLogModeCmd_t;

/**
** \brief Set Event Format Mode Command Payload
**
** For command details, see #CFE_EVS_SET_EVENT_FORMAT_MODE_CC
**
**/
typedef struct CFE_EVS_SetEventFormatCode_Payload
{
    CFE_EVS_MsgFormat_Enum_t MsgFormat; /**< \brief Mode to use in the command*/
    uint8                    Spare;     /**< \brief Pad to even byte*/
} CFE_EVS_SetEventFormatMode_Payload_t;

/**
 * \brief Set Event Format Mode Command
 */
typedef struct CFE_EVS_SetEventFormatModeCmd
{
    CFE_MSG_CommandHeader_t              CommandHeader; /**< \brief Command header */
    CFE_EVS_SetEventFormatMode_Payload_t Payload;       /**< \brief Command payload */
} CFE_EVS_SetEventFormatModeCmd_t;

/**
** \brief Generic Bitmask Command Payload
**
** For command details, see #CFE_EVS_ENABLE_EVENT_TYPE_CC, #CFE_EVS_DISABLE_EVENT_TYPE_CC,
**                          #CFE_EVS_ENABLE_PORTS_CC and/or #CFE_EVS_DISABLE_PORTS_CC
**
**/
typedef struct CFE_EVS_BitMaskCmd_Payload
{
    uint8 BitMask; /**< \brief BitMask to use in the command */
    uint8 Spare;   /**< \brief Pad to even byte*/
} CFE_EVS_BitMaskCmd_Payload_t;

/**
 * \brief Generic Bitmask Command
 */
typedef struct CFE_EVS_BitMaskCmd
{
    CFE_MSG_CommandHeader_t      CommandHeader; /**< \brief Command header */
    CFE_EVS_BitMaskCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_EVS_BitMaskCmd_t;

/*
 * The CFE_EVS_BitMaskCmd_t is shared by several EVS command handlers.
 * Create a unique type for each one so the prototypes will follow the naming pattern,
 * allowing each command to evolve independently.
 */
typedef CFE_EVS_BitMaskCmd_t CFE_EVS_EnablePortsCmd_t;
typedef CFE_EVS_BitMaskCmd_t CFE_EVS_DisablePortsCmd_t;
typedef CFE_EVS_BitMaskCmd_t CFE_EVS_EnableEventTypeCmd_t;
typedef CFE_EVS_BitMaskCmd_t CFE_EVS_DisableEventTypeCmd_t;

/**
** \brief Generic App Name Command Payload
**
** For command details, see #CFE_EVS_ENABLE_APP_EVENTS_CC, #CFE_EVS_DISABLE_APP_EVENTS_CC,
**                          #CFE_EVS_RESET_APP_COUNTER_CC and/or #CFE_EVS_RESET_ALL_FILTERS_CC
**
**/
typedef struct CFE_EVS_AppNameCmd_Payload
{
    char AppName[CFE_MISSION_MAX_API_LEN]; /**< \brief Application name to use in the command*/
} CFE_EVS_AppNameCmd_Payload_t;

/**
 * \brief Generic App Name Command
 */
typedef struct CFE_EVS_AppNameCmd
{
    CFE_MSG_CommandHeader_t      CommandHeader; /**< \brief Command header */
    CFE_EVS_AppNameCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_EVS_AppNameCmd_t;

/*
 * The CFE_EVS_AppNameCmd_t is shared by several EVS command handlers.
 * Create a unique type for each one so the prototypes will follow the naming pattern,
 * allowing each command to evolve independently.
 */
typedef CFE_EVS_AppNameCmd_t CFE_EVS_EnableAppEventsCmd_t;
typedef CFE_EVS_AppNameCmd_t CFE_EVS_DisableAppEventsCmd_t;
typedef CFE_EVS_AppNameCmd_t CFE_EVS_ResetAppCounterCmd_t;
typedef CFE_EVS_AppNameCmd_t CFE_EVS_ResetAllFiltersCmd_t;

/**
** \brief Generic App Name and Event ID Command Payload
**
** For command details, see #CFE_EVS_RESET_FILTER_CC and #CFE_EVS_DELETE_EVENT_FILTER_CC
**
**/
typedef struct CFE_EVS_AppNameEventIDCmd_Payload
{
    char   AppName[CFE_MISSION_MAX_API_LEN]; /**< \brief Application name to use in the command*/
    uint16 EventID;                          /**< \brief Event ID  to use in the command*/
} CFE_EVS_AppNameEventIDCmd_Payload_t;

/**
 * \brief Generic App Name and Event ID Command
 */
typedef struct CFE_EVS_AppNameEventIDCmd
{
    CFE_MSG_CommandHeader_t             CommandHeader; /**< \brief Command header */
    CFE_EVS_AppNameEventIDCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_EVS_AppNameEventIDCmd_t;

/*
 * The CFE_EVS_AppNameEventIDCmd_t is shared by several EVS command handlers.
 * Create a unique type for each one so the prototypes will follow the naming pattern,
 * allowing each command to evolve independently.
 */
typedef CFE_EVS_AppNameEventIDCmd_t CFE_EVS_ResetFilterCmd_t;
typedef CFE_EVS_AppNameEventIDCmd_t CFE_EVS_DeleteEventFilterCmd_t;

/**
** \brief Generic App Name and Bitmask Command Payload
**
** For command details, see #CFE_EVS_ENABLE_APP_EVENT_TYPE_CC and/or #CFE_EVS_DISABLE_APP_EVENT_TYPE_CC
**
**/
typedef struct CFE_EVS_AppNameBitMaskCmd_Payload
{
    char  AppName[CFE_MISSION_MAX_API_LEN]; /**< \brief Application name to use in the command*/
    uint8 BitMask;                          /**< \brief BitMask to use in the command*/
    uint8 Spare;                            /**< \brief Pad to even byte*/
} CFE_EVS_AppNameBitMaskCmd_Payload_t;

/**
 * \brief Generic App Name and Bitmask Command
 */
typedef struct CFE_EVS_AppNameBitMaskCmd
{
    CFE_MSG_CommandHeader_t             CommandHeader; /**< \brief Command header */
    CFE_EVS_AppNameBitMaskCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_EVS_AppNameBitMaskCmd_t;

/*
 * The CFE_EVS_AppNameBitMaskCmd_t is shared by several EVS command handlers.
 * Create a unique type for each one so the prototypes will follow the naming pattern,
 * allowing each command to evolve independently.
 */
typedef CFE_EVS_AppNameBitMaskCmd_t CFE_EVS_EnableAppEventTypeCmd_t;
typedef CFE_EVS_AppNameBitMaskCmd_t CFE_EVS_DisableAppEventTypeCmd_t;

/**
** \brief Generic App Name, Event ID, Mask Command Payload
**
** For command details, see #CFE_EVS_SET_FILTER_CC, #CFE_EVS_ADD_EVENT_FILTER_CC
**                      and/or #CFE_EVS_DELETE_EVENT_FILTER_CC
**
**/
typedef struct CFE_EVS_AppNameEventIDMaskCmd_Payload
{
    char   AppName[CFE_MISSION_MAX_API_LEN]; /**< \brief Application name to use in the command*/
    uint16 EventID;                          /**< \brief Event ID  to use in the command*/
    uint16 Mask;                             /**< \brief Mask to use in the command */
} CFE_EVS_AppNameEventIDMaskCmd_Payload_t;

/**
 * \brief Generic App Name, Event ID, Mask Command
 */
typedef struct CFE_EVS_AppNameEventIDMaskCmd
{
    CFE_MSG_CommandHeader_t                 CommandHeader; /**< \brief Command header */
    CFE_EVS_AppNameEventIDMaskCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_EVS_AppNameEventIDMaskCmd_t;

/*
 * The CFE_EVS_AppNameEventIDMaskCmd_t is shared by several EVS command handlers.
 * Create a unique type for each one so the prototypes will follow the naming pattern,
 * allowing each command to evolve independently.
 */
typedef CFE_EVS_AppNameEventIDMaskCmd_t CFE_EVS_AddEventFilterCmd_t;
typedef CFE_EVS_AppNameEventIDMaskCmd_t CFE_EVS_SetFilterCmd_t;

/*************************************************************************/
/**********************************/
/* Telemetry Message Data Formats */
/**********************************/
typedef struct CFE_EVS_AppTlmData
{
    CFE_ES_AppId_t AppID;             /**< \cfetlmmnemonic \EVS_APPID
                                           \brief Numerical application identifier */
    uint16 AppMessageSentCounter;     /**< \cfetlmmnemonic \EVS_APPMSGSENTC
                                           \brief Application message sent counter */
    uint8 AppEnableStatus;            /**< \cfetlmmnemonic \EVS_APPENASTAT
                                           \brief Application event service enable status */
    uint8 AppMessageSquelchedCounter; /**< \cfetlmmnemonic \EVS_SQUELCHEDC
                                       \brief Number of events squelched */
} CFE_EVS_AppTlmData_t;

/**
**  \cfeevstlm Event Services Housekeeping Telemetry Packet
**/
typedef struct CFE_EVS_HousekeepingTlm_Payload
{
    uint8 CommandCounter;      /**< \cfetlmmnemonic \EVS_CMDPC
                                    \brief EVS Command Counter */
    uint8 CommandErrorCounter; /**< \cfetlmmnemonic \EVS_CMDEC
                                  \brief EVS Command Error Counter */
    uint8 MessageFormatMode;   /**< \cfetlmmnemonic \EVS_MSGFMTMODE
                                    \brief Event message format mode (short/long) */
    uint8 MessageTruncCounter; /**< \cfetlmmnemonic \EVS_MSGTRUNC
                                    \brief Event message truncation counter */

    uint8 UnregisteredAppCounter; /**< \cfetlmmnemonic \EVS_UNREGAPPC
                                       \brief Unregistered application message send counter */
    uint8 OutputPort;             /**< \cfetlmmnemonic \EVS_OUTPUTPORT
                                       \brief Output port mask */
    uint8 LogFullFlag;            /**< \cfetlmmnemonic \EVS_LOGFULL
                                       \brief Local event log full flag */
    uint8 LogMode;                /**< \cfetlmmnemonic \EVS_LOGMODE
                                       \brief Local event logging mode (overwrite/discard) */

    uint16 MessageSendCounter; /**< \cfetlmmnemonic \EVS_MSGSENTC
                                    \brief Event message send counter */
    uint16 LogOverflowCounter; /**< \cfetlmmnemonic \EVS_LOGOVERFLOWC
                                    \brief Local event log overflow counter */

    uint8 LogEnabled; /**< \cfetlmmnemonic \EVS_LOGENABLED
                           \brief Current event log enable/disable state */
    uint8 Spare1;     /**< \cfetlmmnemonic \EVS_HK_SPARE1
                           \brief Padding for 32 bit boundary */
    uint8 Spare2;     /**< \cfetlmmnemonic \EVS_HK_SPARE2
                           \brief Padding for 32 bit boundary */
    uint8 Spare3;     /**< \cfetlmmnemonic \EVS_HK_SPARE3
                           \brief Padding for 32 bit boundary */

    CFE_EVS_AppTlmData_t AppData[CFE_MISSION_ES_MAX_APPLICATIONS]; /**< \cfetlmmnemonic \EVS_APP
                                                                \brief Array of registered application table data */
} CFE_EVS_HousekeepingTlm_Payload_t;

typedef struct CFE_EVS_HousekeepingTlm
{
    CFE_MSG_TelemetryHeader_t         TelemetryHeader; /**< \brief Telemetry header */
    CFE_EVS_HousekeepingTlm_Payload_t Payload;         /**< \brief Telemetry payload */
} CFE_EVS_HousekeepingTlm_t;

/** Telemetry packet structures */

typedef struct CFE_EVS_PacketID
{
    char AppName[CFE_MISSION_MAX_API_LEN]; /**< \cfetlmmnemonic \EVS_APPNAME
                                        \brief Application name */
    uint16 EventID;                        /**< \cfetlmmnemonic \EVS_EVENTID
                                                \brief Numerical event identifier */
    uint16 EventType;                      /**< \cfetlmmnemonic \EVS_EVENTTYPE
                                                \brief Numerical event type identifier */
    uint32 SpacecraftID;                   /**< \cfetlmmnemonic \EVS_SCID
                                                \brief Spacecraft identifier */
    uint32 ProcessorID;                    /**< \cfetlmmnemonic \EVS_PROCESSORID
                                                \brief Numerical processor identifier */
} CFE_EVS_PacketID_t;

/**
**  \cfeevstlm Event Message Telemetry Packet (Long format)
**/
typedef struct CFE_EVS_LongEventTlm_Payload
{
    CFE_EVS_PacketID_t PacketID;                                    /**< \brief Event packet information */
    char               Message[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH]; /**< \cfetlmmnemonic \EVS_EVENT
                                                                 \brief Event message string */
    uint8 Spare1;                                                   /**< \cfetlmmnemonic \EVS_SPARE1
                                                                         \brief Structure padding */
    uint8 Spare2;                                                   /**< \cfetlmmnemonic \EVS_SPARE2
                                                                     \brief Structure padding */
} CFE_EVS_LongEventTlm_Payload_t;

/**
**  \cfeevstlm Event Message Telemetry Packet (Short format)
**/
typedef struct CFE_EVS_ShortEventTlm_Payload
{
    CFE_EVS_PacketID_t PacketID; /**< \brief Event packet information */
} CFE_EVS_ShortEventTlm_Payload_t;

typedef struct CFE_EVS_LongEventTlm
{
    CFE_MSG_TelemetryHeader_t      TelemetryHeader; /**< \brief Telemetry header */
    CFE_EVS_LongEventTlm_Payload_t Payload;         /**< \brief Telemetry payload */
} CFE_EVS_LongEventTlm_t;

typedef struct CFE_EVS_ShortEventTlm
{
    CFE_MSG_TelemetryHeader_t       TelemetryHeader; /**< \brief Telemetry header */
    CFE_EVS_ShortEventTlm_Payload_t Payload;         /**< \brief Telemetry payload */
} CFE_EVS_ShortEventTlm_t;

#endif
