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
 *   Specification for the CFE Event Services (CFE_SB) command and telemetry
 *   message constant definitions.
 *
 *  For CFE_SB this is only the function/command code definitions
 */
#ifndef CFE_SB_MSGDEFS_H
#define CFE_SB_MSGDEFS_H

#include "common_types.h"
#include "cfe_mission_cfg.h"
#include "cfe_es_extern_typedefs.h"
#include "cfe_sb_extern_typedefs.h"
#include "cfe_sb_fcncodes.h"

/**
**  \brief Write File Info Command Payload
**
**  This structure contains a generic definition used by SB commands that write to a file
*/
typedef struct CFE_SB_WriteFileInfoCmd_Payload
{
    char Filename[CFE_MISSION_MAX_PATH_LEN]; /**< \brief Path and Filename of data to be loaded */
} CFE_SB_WriteFileInfoCmd_Payload_t;

/**
**  \brief Enable/Disable Route Command Payload
**
**  This structure contains a definition used by two SB commands,
**  'Enable Route' #CFE_SB_ENABLE_ROUTE_CC and 'Disable Route' #CFE_SB_DISABLE_ROUTE_CC.
**  A route is the destination pipe for a  particular message and is therefore defined
**  as a MsgId and PipeId combination.
*/
typedef struct CFE_SB_RouteCmd_Payload
{
    CFE_SB_MsgId_t  MsgId; /**< \brief Message ID of route to be enabled or disabled #CFE_SB_MsgId_t */
    CFE_SB_PipeId_t Pipe;  /**< \brief Pipe ID of route to be enabled or disabled #CFE_SB_PipeId_t */
    uint8           Spare; /**<\brief Spare byte to make command even number of bytes */
} CFE_SB_RouteCmd_Payload_t;

/****************************
**  SB Telemetry Formats   **
*****************************/

/**
**  \cfesbtlm Software Bus task housekeeping Packet
*/
typedef struct CFE_SB_HousekeepingTlm_Payload
{
    uint8 CommandCounter;       /**< \cfetlmmnemonic \SB_CMDPC
                                 \brief Count of valid commands received */
    uint8 CommandErrorCounter;  /**< \cfetlmmnemonic \SB_CMDEC
                           \brief Count of invalid commands received */
    uint8 NoSubscribersCounter; /**< \cfetlmmnemonic \SB_NOSUBEC
                                 \brief Count pkts sent with no subscribers */
    uint8 MsgSendErrorCounter;  /**< \cfetlmmnemonic \SB_MSGSNDEC
                               \brief Count of message send errors */

    uint8 MsgReceiveErrorCounter;        /**< \cfetlmmnemonic \SB_MSGRECEC
                                        \brief Count of message receive errors */
    uint8 InternalErrorCounter;          /**< \cfetlmmnemonic \SB_INTERNALEC
                                        \brief Count of queue read or write errors */
    uint8 CreatePipeErrorCounter;        /**< \cfetlmmnemonic \SB_NEWPIPEEC
                                        \brief Count of errors in create pipe API */
    uint8 SubscribeErrorCounter;         /**< \cfetlmmnemonic \SB_SUBSCREC
                                        \brief Count of errors in subscribe API */
    uint8 PipeOptsErrorCounter;          /**< \cfetlmmnemonic \SB_PIPEOPTSEC
                                       \brief Count of errors in set/get pipe options API */
    uint8 DuplicateSubscriptionsCounter; /**< \cfetlmmnemonic \SB_DUPSUBCNT
                                    \brief Count of duplicate subscriptions */
    uint8 GetPipeIdByNameErrorCounter;   /**< \cfetlmmnemonic \SB_GETPIPEIDBYNAMEEC
                                \brief Count of errors in get pipe id by name API */
    uint8 Spare2Align[1];                /**< \cfetlmmnemonic \SB_SPARE2ALIGN
                                              \brief Spare bytes to ensure alignment */

    uint16 PipeOverflowErrorCounter; /**< \cfetlmmnemonic \SB_PIPEOVREC
                                    \brief Count of pipe overflow errors */
    uint16 MsgLimitErrorCounter;     /**< \cfetlmmnemonic \SB_MSGLIMEC
                                       \brief Count of msg id to pipe errors */

    CFE_ES_MemHandle_t MemPoolHandle; /**< \cfetlmmnemonic \SB_MEMPOOLHANDLE
                                           \brief Handle to SB's Memory Pool */

    uint32 MemInUse; /**< \cfetlmmnemonic \SB_MEMINUSE
                                    \brief Memory in use */

    uint32 UnmarkedMem; /**< \cfetlmmnemonic \SB_UNMARKEDMEM
                             \brief cfg param CFE_PLATFORM_SB_BUF_MEMORY_BYTES minus Peak Memory in use */
} CFE_SB_HousekeepingTlm_Payload_t;

/**
** \brief SB Pipe Depth Statistics
**
** Used in SB Statistics Telemetry Packet #CFE_SB_StatsTlm_t
*/
typedef struct CFE_SB_PipeDepthStats
{
    CFE_SB_PipeId_t PipeId;   /**< \cfetlmmnemonic \SB_PDPIPEID
                                   \brief Pipe Id associated with the stats below */
    uint16 MaxQueueDepth;     /**< \cfetlmmnemonic \SB_PDDEPTH
                           \brief Number of messages the pipe can hold */
    uint16 CurrentQueueDepth; /**< \cfetlmmnemonic \SB_PDINUSE
                       \brief Number of messages currently on the pipe */
    uint16 PeakQueueDepth;    /**< \cfetlmmnemonic \SB_PDPKINUSE
                              \brief Peak number of messages that have been on the pipe */
    uint16 Spare;             /**< \cfetlmmnemonic \SB_PDSPARE
                                   \brief Spare word to ensure alignment */
} CFE_SB_PipeDepthStats_t;

/**
** \brief SB Pipe Information File Entry
**
** This statistics structure is output as part of the CFE SB
** "Send Pipe Info" command (CFE_SB_SEND_PIPE_INFO_CC).
**
** Previous versions of CFE simply wrote the internal CFE_SB_PipeD_t object
** to the file, but this also contains information such as pointers which are
** not relevant outside the running CFE process.
**
** By defining the pipe info structure separately, it also provides some
** independence, such that the internal CFE_SB_PipeD_t definition
** can evolve without changing the binary format of the information
** file.
*/
typedef struct CFE_SB_PipeInfoEntry
{
    CFE_SB_PipeId_t PipeId;                            /**< The runtime ID of the pipe */
    CFE_ES_AppId_t  AppId;                             /**< The runtime ID of the application that owns the pipe */
    char            PipeName[CFE_MISSION_MAX_API_LEN]; /**< The Name of the pipe */
    char            AppName[CFE_MISSION_MAX_API_LEN];  /**< The Name of the application that owns the pipe */
    uint16          MaxQueueDepth;                     /**< The allocated depth of the pipe (max capacity) */
    uint16          CurrentQueueDepth;                 /**< The current depth of the pipe */
    uint16          PeakQueueDepth;                    /**< The peak depth of the pipe (high watermark) */
    uint16          SendErrors;                        /**< Number of errors when writing to this pipe */
    uint8           Opts;                              /**< Pipe options set (bitmask) */
    uint8           Spare[3];                          /**< Padding to make this structure a multiple of 4 bytes */
} CFE_SB_PipeInfoEntry_t;

/**
** \cfesbtlm SB Statistics Telemetry Packet
**
** SB Statistics packet sent in response to #CFE_SB_SEND_SB_STATS_CC
*/
typedef struct CFE_SB_StatsTlm_Payload
{
    uint32 MsgIdsInUse;      /**< \cfetlmmnemonic \SB_SMMIDIU
                                  \brief Current number of MsgIds with a destination */
    uint32 PeakMsgIdsInUse;  /**< \cfetlmmnemonic \SB_SMPMIDIU
                                  \brief Peak number of MsgIds with a destination */
    uint32 MaxMsgIdsAllowed; /**< \cfetlmmnemonic \SB_SMMMIDALW
                                  \brief cFE Cfg Param \link #CFE_PLATFORM_SB_MAX_MSG_IDS \endlink */

    uint32 PipesInUse;      /**< \cfetlmmnemonic \SB_SMPIU
                                 \brief Number of pipes currently in use */
    uint32 PeakPipesInUse;  /**< \cfetlmmnemonic \SB_SMPPIU
                                 \brief Peak number of pipes since last reboot */
    uint32 MaxPipesAllowed; /**< \cfetlmmnemonic \SB_SMMPALW
                                 \brief cFE Cfg Param \link #CFE_PLATFORM_SB_MAX_PIPES \endlink */

    uint32 MemInUse;      /**< \cfetlmmnemonic \SB_SMBMIU
                                  \brief Memory bytes currently in use for SB msg transfers */
    uint32 PeakMemInUse;  /**< \cfetlmmnemonic \SB_SMPBMIU
                                  \brief Peak memory bytes in use for SB msg transfers */
    uint32 MaxMemAllowed; /**< \cfetlmmnemonic \SB_SMMBMALW
                                  \brief cFE Cfg Param \link #CFE_PLATFORM_SB_BUF_MEMORY_BYTES \endlink */

    uint32 SubscriptionsInUse;      /**< \cfetlmmnemonic \SB_SMSIU
                                         \brief Number of current subscriptions */
    uint32 PeakSubscriptionsInUse;  /**< \cfetlmmnemonic \SB_SMPSIU
                                         \brief Peak number of subscriptions */
    uint32 MaxSubscriptionsAllowed; /**< \cfetlmmnemonic \SB_SMMSALW
                                         \brief product of \link #CFE_PLATFORM_SB_MAX_MSG_IDS \endlink
                                         and \link #CFE_PLATFORM_SB_MAX_DEST_PER_PKT \endlink */

    uint32 SBBuffersInUse;     /**< \cfetlmmnemonic \SB_SMSBBIU
                                    \brief Number of SB message buffers currently in use */
    uint32 PeakSBBuffersInUse; /**< \cfetlmmnemonic \SB_SMPSBBIU
                                    \brief Max number of SB message buffers in use */

    uint32 MaxPipeDepthAllowed; /**< \cfetlmmnemonic \SB_SMMPDALW
                                     \brief Maximum allowed pipe depth */
    CFE_SB_PipeDepthStats_t
        PipeDepthStats[CFE_MISSION_SB_MAX_PIPES]; /**< \cfetlmmnemonic \SB_SMPDS
                                               \brief Pipe Depth Statistics #CFE_SB_PipeDepthStats_t*/
} CFE_SB_StatsTlm_Payload_t;

/**
** \brief SB Routing File Entry
**
** Structure of one element of the routing information in response to #CFE_SB_WRITE_ROUTING_INFO_CC
*/
typedef struct CFE_SB_RoutingFileEntry
{
    CFE_SB_MsgId_t  MsgId;                             /**< \brief Message Id portion of the route */
    CFE_SB_PipeId_t PipeId;                            /**< \brief Pipe Id portion of the route */
    uint8           State;                             /**< \brief Route Enabled or Disabled */
    uint16          MsgCnt;                            /**< \brief Number of msgs with this MsgId sent to this PipeId */
    char            AppName[CFE_MISSION_MAX_API_LEN];  /**< \brief Pipe Depth Statistics */
    char            PipeName[CFE_MISSION_MAX_API_LEN]; /**< \brief Pipe Depth Statistics */
} CFE_SB_RoutingFileEntry_t;

/**
** \brief SB Map File Entry
**
** Structure of one element of the map information in response to #CFE_SB_WRITE_MAP_INFO_CC
*/
typedef struct CFE_SB_MsgMapFileEntry
{
    CFE_SB_MsgId_t        MsgId; /**< \brief Message Id which has been subscribed to */
    CFE_SB_RouteId_Atom_t Index; /**< \brief Routing raw index value (0 based, not Route ID) */
} CFE_SB_MsgMapFileEntry_t;

/**
** \cfesbtlm SB Subscription Report Packet
**
** This structure defines the pkt sent by SB when a subscription or
** a request to unsubscribe is received while subscription reporting is enabled.
** By default subscription reporting is disabled. This feature is intended to be
** used primarily by Software Bus Networking Application (SBN)
**
** \sa #CFE_SB_ENABLE_SUB_REPORTING_CC, #CFE_SB_DISABLE_SUB_REPORTING_CC
*/
typedef struct CFE_SB_SingleSubscriptionTlm_Payload
{
    uint8           SubType; /**< \brief Subscription or Unsubscription */
    CFE_SB_MsgId_t  MsgId;   /**< \brief MsgId subscribed or unsubscribe to */
    CFE_SB_Qos_t    Qos;     /**< \brief Quality of Service, used only for interprocessor communication */
    CFE_SB_PipeId_t Pipe;    /**< \brief Destination pipe id to send above msg id  */
} CFE_SB_SingleSubscriptionTlm_Payload_t;

/**
** \brief SB Previous Subscriptions Entry
**
** This structure defines an entry used in the CFE_SB_PrevSubsPkt_t
** Intended to be used primarily by Software Bus Networking Application (SBN)
**
** Used in structure definition #CFE_SB_AllSubscriptionsTlm_t
*/
typedef struct CFE_SB_SubEntries
{
    CFE_SB_MsgId_t  MsgId; /**< \brief MsgId portion of the subscription */
    CFE_SB_Qos_t    Qos;   /**< \brief Qos portion of the subscription */
    CFE_SB_PipeId_t Pipe;  /**< \brief PipeId portion of the subscription */
} CFE_SB_SubEntries_t;

/**
** \cfesbtlm SB Previous Subscriptions Packet
**
** This structure defines the pkt(s) sent by SB that contains a list of all current
** subscriptions. This pkt is generated on cmd and intended to be used primarily by
** the Software Bus Networking Application (SBN). Typically, when the cmd is received
** there are more subscriptions than can fit in one pkt. The complete list of
** subscriptions is sent via a series of segmented pkts.
*/
typedef struct CFE_SB_AllSubscriptionsTlm_Payload
{
    uint32              PktSegment;    /**< \brief Pkt number(starts at 1) in the series */
    uint32              TotalSegments; /**< \brief Total number of pkts needed to complete the request */
    uint32              Entries;       /**< \brief Number of entries in the pkt */
    CFE_SB_SubEntries_t Entry[CFE_SB_SUB_ENTRIES_PER_PKT]; /**< \brief Array of #CFE_SB_SubEntries_t entries */
} CFE_SB_AllSubscriptionsTlm_Payload_t;

#endif
