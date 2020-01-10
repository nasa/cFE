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

/******************************************************************************
** File: cfe_sb_msg.h
**
** Purpose:
**      This header file contains structure definitions for all SB command and
**      telemetry packets
**
** Author:   R.McGraw/SSI
**
******************************************************************************/

#ifndef _cfe_sb_msg_
#define _cfe_sb_msg_

/*
** Includes
*/
#include "common_types.h"
#include "cfe_sb.h"
#include "cfe_es.h"


/****************************************
** SB task command packet command codes
****************************************/

/** \cfesbcmd Software Bus No-Op
**
**  \par Description
**       This command performs no other function than to increment the
**       command execution counter. The command may be used to verify
**       general aliveness of the Software Bus task.
**
**  \cfecmdmnemonic \SB_NOOP
**
**  \par Command Structure
**       #CFE_SB_CmdHdr_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the
**       following telemetry:
**       - \b \c \SB_CMDPC - command execution counter will
**         increment
**       - The #CFE_SB_CMD0_RCVD_EID informational event message will
**         be generated
**
**  \par Error Conditions
**       There are no error conditions for this command. If the Software
**       Bus receives the command, the event is sent (although it
**       may be filtered by EVS) and the counter is incremented
**       unconditionally.
**
**  \par Criticality
**       None
**
**  \sa
*/
#define CFE_SB_NOOP_CC                  0

/** \cfesbcmd Software Bus Reset Counters
**
**  \par Description
**       This command resets the following counters within the Software
**       Bus housekeeping telemetry:
**       - Command Execution Counter (\SB_CMDPC)
**       - Command Error Counter (\SB_CMDEC)
**
**  \cfecmdmnemonic \SB_RESETCTRS
**
**  \par Command Structure
**       #CFE_SB_CmdHdr_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the
**       following telemetry:
**       - \b \c \SB_CMDPC - command execution counter will
**         increment
**       - The #CFE_SB_CMD1_RCVD_EID informational event message will
**         be generated
**
**  \par Error Conditions
**       There are no error conditions for this command. If the Software
**       Bus receives the command, the event is sent (although it
**       may be filtered by EVS) and the counter is incremented
**       unconditionally.
**
**  \par Criticality
**       This command is not inherently dangerous.  However, it is
**       possible for ground systems and on-board safing procedures
**       to be designed such that they react to changes in the counter
**       values that are reset by this command.
**
**  \sa
*/
#define CFE_SB_RESET_COUNTERS_CC            1

/** \cfesbcmd Send Software Bus Statistics
**
**  \par Description
**       This command will cause the SB task to send a statistics packet
**       containing current utilization figures and high water marks which
**       may be useful for checking the margin of the SB platform configuration
**       settings.
**
**  \cfecmdmnemonic \SB_DUMPSTATS
**
**  \par Command Structure
**       #CFE_SB_CmdHdr_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the
**       following telemetry:
**       - \b \c \SB_CMDPC - command execution counter will increment
**       - Receipt of statistics packet with MsgId #CFE_SB_STATS_TLM_MID
**       - The #CFE_SB_SND_STATS_EID debug event message will be generated. All
**         debug events are filtered by default.
**
**  \par Error Conditions
**       There are no error conditions for this command. If the Software
**       Bus receives the command, the debug event is sent and the counter
**       is incremented unconditionally.
**
**  \par Criticality
**       This command is not inherently dangerous.  It will create and send
**       a message on the software bus. If performed repeatedly, it is
**       possible that receiver pipes may overflow.
**
**  \sa
*/
#define CFE_SB_SEND_SB_STATS_CC         2

/** \cfesbcmd Write Software Bus Routing Info to a File
**
**  \par Description
**       This command will create a file containing the software bus routing
**       information. The routing information contains information about every
**       subscription that has been received through the SB subscription APIs.
**       An abosulte path and filename may be specified in the command.
**       If this command field contains an empty string (NULL terminator as
**       the first character) the default file path and name is used.
**       The default file path and name is defined in the platform
**       configuration file as #CFE_PLATFORM_SB_DEFAULT_ROUTING_FILENAME.
**
**  \cfecmdmnemonic \SB_WRITEROUTING2FILE
**
**  \par Command Structure
**       #CFE_SB_WriteFileInfoCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the
**       following telemetry:
**       - \b \c \SB_CMDPC - command execution counter will increment.
**       - Specified filename created at specified location. See description.
**       - The #CFE_SB_SND_RTG_EID debug event message will be generated. All
**         debug events are filtered by default.
**
**  \par Error Conditions
**       - Errors may occur during write operations to the file. Possible
**         causes might be insufficient space in the file system or the
**         filename or file path is improperly specified.
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \SB_CMDEC - command error counter will increment
**       - A command specific error event message is issued for all error
**         cases. See #CFE_SB_SND_RTG_ERR1_EID and #CFE_SB_FILEWRITE_ERR_EID
**
**  \par Criticality
**       This command is not inherently dangerous.  It will create a new
**       file in the file system and could, if performed repeatedly without
**       sufficient file management by the operator, fill the file system.
**
**  \sa #CFE_SB_SEND_PIPE_INFO_CC, #CFE_SB_SEND_MAP_INFO_CC, #CFE_SB_WriteFileInfoCmd_t
*/
#define CFE_SB_SEND_ROUTING_INFO_CC     3

/** \cfesbcmd Enable Software Bus Route
**
**  \par Description
**       This command will enable a particular destination. The destination
**       is specified in terms of MsgID and PipeID. The MsgId and PipeID are
**       parmaters in the command. All destinations are enabled by default.
**       This command is needed only after a #CFE_SB_DISABLE_ROUTE_CC command
**       is used.
**
**  \cfecmdmnemonic \SB_ENAROUTE
**
**  \par Command Structure
**       #CFE_SB_RouteCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the
**       following telemetry:
**       - \b \c \SB_CMDPC - command execution counter will increment
**       - View routing information #CFE_SB_SEND_ROUTING_INFO_CC to verify
**         enable/disable state change
**       - The #CFE_SB_ENBL_RTE2_EID debug event message will be generated. All
**         debug events are filtered by default.
**       - Destination will begin receiving messages.
**
**  \par Error Conditions
**       An Error may occur if the MsgId or PipeId parmaters do not pass
**       validation or the destination does not exist.

**       Evidence of failure may be found in the following telemetry:
**       - \b \c \SB_CMDEC - command error counter will increment
**       - A command specific error event message is issued for all error
**         cases. See #CFE_SB_ENBL_RTE1_EID or #CFE_SB_ENBL_RTE3_EID
**
**  \par Criticality
**       This command is not inherently dangerous.
**
**  \sa #CFE_SB_SEND_ROUTING_INFO_CC, #CFE_SB_DISABLE_ROUTE_CC, #CFE_SB_RouteCmd_t
*/
#define CFE_SB_ENABLE_ROUTE_CC          4

/** \cfesbcmd Disable Software Bus Route
**
**  \par Description
**       This command will disable a particular destination. The destination
**       is specified in terms of MsgID and PipeID. The MsgId and PipeID are
**       parmaters in the command. All destinations are enabled by default.
**
**  \cfecmdmnemonic \SB_DISROUTE
**
**  \par Command Structure
**       #CFE_SB_RouteCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the
**       following telemetry:
**       - \b \c \SB_CMDPC - command execution counter will increment
**       - View routing information #CFE_SB_SEND_ROUTING_INFO_CC to verify
**         enable/disable state change
**       - The #CFE_SB_DSBL_RTE2_EID debug event message will be generated. All
**         debug events are filtered by default.
**       - Destination will stop receiving messages.
**
**  \par Error Conditions
**       An Error may occur if the MsgId or PipeId parmaters do not pass
**       validation or the destination does not exist.

**       Evidence of failure may be found in the following telemetry:
**       - \b \c \SB_CMDEC - command error counter will increment
**       - A command specific error event message is issued for all error
**         cases. See #CFE_SB_DSBL_RTE1_EID or #CFE_SB_DSBL_RTE3_EID
**
**  \par Criticality
**       This command is not intended to be used in nominal conditions. It is
**       possible to get into a state where a destination cannot be re-enabled
**       without reseting the processor. For instance, sending this command
**       with #CFE_SB_CMD_MID and the SB_Cmd_Pipe would inhibit any ground
**       commanding to the software bus until the processor was reset. There
**       are similar problems that may occur when using this command.
**
**  \sa #CFE_SB_SEND_ROUTING_INFO_CC, #CFE_SB_ENABLE_ROUTE_CC, #CFE_SB_RouteCmd_t
*/
#define CFE_SB_DISABLE_ROUTE_CC         5

/** \cfesbcmd Write Pipe Info to a File
**
**  \par Description
**       This command will create a file containing the software bus pipe
**       information. The pipe information contains information about every
**       pipe that has been created through the #CFE_SB_CreatePipe API. An
**       abosulte path and filename may be specified in the command.
**       If this command field contains an empty string (NULL terminator as
**       the first character) the default file path and name is used.
**       The default file path and name is defined in the platform
**       configuration file as #CFE_PLATFORM_SB_DEFAULT_PIPE_FILENAME.
**
**  \cfecmdmnemonic \SB_WRITEPIPE2FILE
**
**  \par Command Structure
**       #CFE_SB_WriteFileInfoCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the
**       following telemetry:
**       - \b \c \SB_CMDPC - command execution counter will increment.
**       - Specified filename created at specified location. See description.
**       - The #CFE_SB_SND_RTG_EID debug event message will be generated. All
**         debug events are filtered by default.
**
**  \par Error Conditions
**       - Errors may occur during write operations to the file. Possible
**         causes might be insufficient space in the file system or the
**         filename or file path is improperly specified.
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \SB_CMDEC - command error counter will increment
**       - A command specific error event message is issued for all error
**         cases. See #CFE_SB_SND_RTG_ERR1_EID and #CFE_SB_FILEWRITE_ERR_EID
**
**  \par Criticality
**       This command is not inherently dangerous.  It will create a new
**       file in the file system and could, if performed repeatedly without
**       sufficient file management by the operator, fill the file system.
**
**  \sa #CFE_SB_SEND_ROUTING_INFO_CC, #CFE_SB_SEND_MAP_INFO_CC
*/
#define CFE_SB_SEND_PIPE_INFO_CC        7

/** \cfesbcmd Write Map Info to a File
**
**  \par This command will create a file containing the software bus message
**       map information. The message map is a lookup table (an array of
**       uint16s)that allows fast access to the correct routing table element
**       during a softeware bus send operation. This is diasgnostic
**       information that may be needed due to the dynamic nature of the
**       cFE software bus. An abosulte path and filename may be specified
**       in the command. If this command field contains an empty string
**       (NULL terminator as the first character) the default file path and
**       name is used. The default file path and name is defined in the
**       platform configuration file as #CFE_PLATFORM_SB_DEFAULT_MAP_FILENAME.
**
**  \cfecmdmnemonic \SB_WRITEMAP2FILE
**
**  \par Command Structure
**       #CFE_SB_WriteFileInfoCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the
**       following telemetry:
**       - \b \c \SB_CMDPC - command execution counter will increment.
**       - Specified filename created at specified location. See description.
**       - The #CFE_SB_SND_RTG_EID debug event message will be generated. All
**         debug events are filtered by default.
**
**  \par Error Conditions
**       - Errors may occur during write operations to the file. Possible
**         causes might be insufficient space in the file system or the
**         filename or file path is improperly specified.
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \SB_CMDEC - command error counter will increment
**       - A command specific error event message is issued for all error
**         cases. See #CFE_SB_SND_RTG_ERR1_EID and #CFE_SB_FILEWRITE_ERR_EID
**
**  \par Criticality
**       This command is not inherently dangerous.  It will create a new
**       file in the file system and could, if performed repeatedly without
**       sufficient file management by the operator, fill the file system.
**
**  \sa #CFE_SB_SEND_ROUTING_INFO_CC, #CFE_SB_SEND_PIPE_INFO_CC
*/
#define CFE_SB_SEND_MAP_INFO_CC         8

/** \cfesbcmd Enable Subscription Reporting Command
**
**  \par This command will enable subscription reporting and is intended to
**       be used only by the CFS SBN (Software Bus Networking) Application.
**       It is not intended to be sent from the ground or used by operations.
**       When subscription reporting is enabled, SB will generate
**       and send a software bus packet for each subscription received.
**       The software bus packet that is sent contains the information
**       received in the subscription API. This subscription report is
**       neeeded by SBN if offboard routing is required.
*
**  \cfecmdmnemonic \SB_ENASUBRPTG
**
**  \par Command Structure
**       #CFE_SB_CmdHdr_t
**
**  \par Command Verification
**       Successful execution of this command will result in the sending
**       of a packet (with the #CFE_SB_ONESUB_TLM_MID MsgId) for each
**       subscription received by SB through the subscription APIs.
**
**  \par Error Conditions
**       None
**
**  \par Criticality
**       None
**
**  \sa #CFE_SB_SingleSubscriptionTlm_t, #CFE_SB_DISABLE_SUB_REPORTING_CC,
**      #CFE_SB_SEND_PREV_SUBS_CC
*/
#define CFE_SB_ENABLE_SUB_REPORTING_CC  9

/** \cfesbcmd Disable Subscription Reporting Command
**
**  \par This command will disable subscription reporting and is intended to
**       be used only by the CFS SBN (Software Bus Networking) Application.
**       It is not intended to be sent from the ground or used by operations.
**       When subscription reporting is enabled, SB will generate
**       and send a software bus packet for each subscription received.
**       The software bus packet that is sent contains the information
**       received in the subscription API. This subscription report is
**       neeeded by SBN if offboard routing is required.
**
**  \cfecmdmnemonic \SB_DISSUBRPTG
**
**  \par Command Structure
**       #CFE_SB_CmdHdr_t
**
**  \par Command Verification
**       Successful execution of this command will result in the suppression
**       of packets (with the #CFE_SB_ONESUB_TLM_MID MsgId) for each
**       subscription received by SB through the subscription APIs.
**
**  \par Error Conditions
**       None
**
**  \par Criticality
**       None
**
**  \sa #CFE_SB_SingleSubscriptionTlm_t, #CFE_SB_ENABLE_SUB_REPORTING_CC,
**      #CFE_SB_SEND_PREV_SUBS_CC
*/
#define CFE_SB_DISABLE_SUB_REPORTING_CC 10

/** \cfesbcmd Send Previous Subscriptions Command
**
**  \par This command generates a series of packets that contain information
**       regarding all subscriptions previously received by SB.
**       This command is intended to be used only by the CFS SBN(Software Bus
**       Networking) Application.
**       It is not intended to be sent from the ground or used by operations.
**       When this command is received the software bus will generate and
**       send a series of packets containing information about all subscription
**       previously received.
**
**  \cfecmdmnemonic \SB_SENDPREVSUBS
**
**  \par Command Structure
**       #CFE_SB_CmdHdr_t
**
**  \par Command Verification
**       Successful execution of this command will result in a series
**       of packets (with the #CFE_SB_ALLSUBS_TLM_MID MsgId) being sent
**       on the software bus.
**
**  \par Error Conditions
**       None
**
**  \par Criticality
**       None
**
**  \sa #CFE_SB_AllSubscriptionsTlm_t, #CFE_SB_ENABLE_SUB_REPORTING_CC,
**      #CFE_SB_DISABLE_SUB_REPORTING_CC
*/
#define CFE_SB_SEND_PREV_SUBS_CC        11


/****************************
**  SB Command Formats     **
*****************************/

/*
 * SB Messages which have no payload are each
 * given unique typedefs to follow the command handler convention
 *
 * For the SB application these is mapped to the CFE_SB_CmdHdr_t type,
 * as they contain only a primary + command header.
 */
typedef CFE_SB_CmdHdr_t  CFE_SB_Noop_t;
typedef CFE_SB_CmdHdr_t  CFE_SB_ResetCounters_t;
typedef CFE_SB_CmdHdr_t  CFE_SB_EnableSubReporting_t;
typedef CFE_SB_CmdHdr_t  CFE_SB_DisableSubReporting_t;
typedef CFE_SB_CmdHdr_t  CFE_SB_SendSbStats_t;
typedef CFE_SB_CmdHdr_t  CFE_SB_SendPrevSubs_t;


/**
**  \brief Write File Info Commands
**
**  This structure contains a generic definition used by three SB commands,
**  'Write Routing Info to File' #CFE_SB_SEND_ROUTING_INFO_CC,
**  'Write Pipe Info to File' #CFE_SB_SEND_PIPE_INFO_CC and
**  'Write Map Info to File' #CFE_SB_SEND_MAP_INFO_CC.
*/
typedef struct{
   char Filename[CFE_MISSION_MAX_PATH_LEN];/**< \brief Path and Filename of data to be loaded */
} CFE_SB_WriteFileInfoCmd_Payload_t;

typedef struct{
    CFE_SB_CmdHdr_t                     Hdr;/**< \brief cFE Software Bus Command Message Header #CFE_SB_CmdHdr_t */
    CFE_SB_WriteFileInfoCmd_Payload_t   Payload;
}CFE_SB_WriteFileInfoCmd_t;

/*
 * Create a unique typedef for each of the commands that share this format.
 */
typedef CFE_SB_WriteFileInfoCmd_t CFE_SB_SendRoutingInfo_t;
typedef CFE_SB_WriteFileInfoCmd_t CFE_SB_SendPipeInfo_t;
typedef CFE_SB_WriteFileInfoCmd_t CFE_SB_SendMapInfo_t;

/**
**  \brief Enable/Disable Route Commands
**
**  This structure contains a definition used by two SB commands,
**  'Enable Route' #CFE_SB_ENABLE_ROUTE_CC and 'Disable Route' #CFE_SB_DISABLE_ROUTE_CC.
**  A route is the destination pipe for a  particular message and is therefore defined
**  as a MsgId and PipeId combination.
*/
typedef struct {

   CFE_SB_MsgId_t       MsgId;/**< \brief Message ID of route to be enabled or disabled #CFE_SB_MsgId_t */
   CFE_SB_PipeId_t      Pipe;/**< \brief Pipe ID of route to be enabled or disabled #CFE_SB_PipeId_t */
   uint8                Spare;/**<\brief Spare byte to make command even number of bytes */
} CFE_SB_RouteCmd_Payload_t;

typedef struct{
    CFE_SB_CmdHdr_t             Hdr;/**< \brief cFE Software Bus Command Message Header #CFE_SB_CmdHdr_t */
    CFE_SB_RouteCmd_Payload_t  Payload;
} CFE_SB_RouteCmd_t;

/*
 * Create a unique typedef for each of the commands that share this format.
 */
typedef CFE_SB_RouteCmd_t CFE_SB_EnableRoute_t;
typedef CFE_SB_RouteCmd_t CFE_SB_DisableRoute_t;

/****************************
**  SB Telemetry Formats   **
*****************************/

/**
**  \cfesbtlm Software Bus task housekeeping Packet
*/
typedef struct {

    uint8           CommandCounter;/**< \cfetlmmnemonic \SB_CMDPC
                                    \brief Count of valid commands received */
    uint8           CommandErrorCounter;/**< \cfetlmmnemonic \SB_CMDEC
                                   \brief Count of invalid commands received */
    uint8           NoSubscribersCounter;/**< \cfetlmmnemonic \SB_NOSUBEC
                                          \brief Count pkts sent with no subscribers */
    uint8           MsgSendErrorCounter;/**< \cfetlmmnemonic \SB_MSGSNDEC
                                       \brief Count of message send errors */

    uint8           MsgReceiveErrorCounter;/**< \cfetlmmnemonic \SB_MSGRECEC
                                          \brief Count of message receive errors */
    uint8           InternalErrorCounter;/**< \cfetlmmnemonic \SB_INTERNALEC
                                        \brief Count of queue read or write errors */
    uint8           CreatePipeErrorCounter;/**< \cfetlmmnemonic \SB_NEWPIPEEC
                                          \brief Count of errors in create pipe API */
    uint8           SubscribeErrorCounter;/**< \cfetlmmnemonic \SB_SUBSCREC
                                         \brief Count of errors in subscribe API */
    uint8           PipeOptsErrorCounter; /**< \cfetlmmnemonic \SB_PIPEOPTSEC
                                        \brief Count of errors in set/get pipe options API */
    uint8           DuplicateSubscriptionsCounter;/**< \cfetlmmnemonic \SB_DUPSUBCNT
                                             \brief Count of duplicate subscriptions */
    uint8           Spare2Align[2];/**< \cfetlmmnemonic \SB_SPARE2ALIGN
                                        \brief Spare bytes to ensure alignment */

    uint16          PipeOverflowErrorCounter;/**< \cfetlmmnemonic \SB_PIPEOVREC
                                            \brief Count of pipe overflow errors */
    uint16          MsgLimitErrorCounter;/**< \cfetlmmnemonic \SB_MSGLIMEC
                                           \brief Count of msg id to pipe errors */

    CFE_ES_MemHandle_t MemPoolHandle;/**< \cfetlmmnemonic \SB_MEMPOOLHANDLE
                                          \brief Handle to SB's Memory Pool */

    uint32          MemInUse;/**< \cfetlmmnemonic \SB_MEMINUSE
                                            \brief Memory in use */

    uint32          UnmarkedMem;/**< \cfetlmmnemonic \SB_UNMARKEDMEM
                                     \brief cfg param CFE_PLATFORM_SB_BUF_MEMORY_BYTES minus Peak Memory in use */
} CFE_SB_HousekeepingTlm_Payload_t;

typedef struct{
    CFE_SB_TlmHdr_t         Hdr;/**< \brief cFE Software Bus Telemetry Message Header */
    CFE_SB_HousekeepingTlm_Payload_t  Payload;
} CFE_SB_HousekeepingTlm_t;


/**
** \brief SB Pipe Depth Statistics
**
** Used in SB Statistics Telemetry Packet #CFE_SB_StatsTlm_t
*/
typedef struct {

    CFE_SB_PipeId_t     PipeId;/**< \cfetlmmnemonic \SB_PDPIPEID
                                    \brief Pipe Id associated with the stats below */
    uint8               Spare;/**< \cfetlmmnemonic \SB_PDSPARE
                                   \brief Spare byte to ensure alignment */
    uint16              Depth;/**< \cfetlmmnemonic \SB_PDDEPTH
                                   \brief Number of messages the pipe can hold */
    uint16              InUse;/**< \cfetlmmnemonic \SB_PDINUSE
                                   \brief Number of messages currently on the pipe */
    uint16              PeakInUse;/**< \cfetlmmnemonic \SB_PDPKINUSE
                                       \brief Peak number of messages that have been on the pipe */

}CFE_SB_PipeDepthStats_t;

/**
** \cfesbtlm SB Statistics Telemetry Packet
**
** SB Statistics packet sent (via CFE_SB_SendMsg) in response to #CFE_SB_SEND_SB_STATS_CC
*/
typedef struct {

    uint32              MsgIdsInUse;/**< \cfetlmmnemonic \SB_SMMIDIU
                                         \brief Current number of MsgIds with a destination */
    uint32              PeakMsgIdsInUse;/**< \cfetlmmnemonic \SB_SMPMIDIU
                                             \brief Peak number of MsgIds with a destination */
    uint32              MaxMsgIdsAllowed;/**< \cfetlmmnemonic \SB_SMMMIDALW
                                              \brief cFE Cfg Param \link #CFE_PLATFORM_SB_MAX_MSG_IDS \endlink */

    uint32              PipesInUse;/**< \cfetlmmnemonic \SB_SMPIU
                                        \brief Number of pipes currently in use */
    uint32              PeakPipesInUse;/**< \cfetlmmnemonic \SB_SMPPIU
                                            \brief Peak number of pipes since last reboot */
    uint32              MaxPipesAllowed;/**< \cfetlmmnemonic \SB_SMMPALW
                                             \brief cFE Cfg Param \link #CFE_PLATFORM_SB_MAX_PIPES \endlink */

    uint32              MemInUse;/**< \cfetlmmnemonic \SB_SMBMIU
                                         \brief Memory bytes currently in use for SB msg transfers */
    uint32              PeakMemInUse;/**< \cfetlmmnemonic \SB_SMPBMIU
                                             \brief Peak memory bytes in use for SB msg transfers */
    uint32              MaxMemAllowed;/**< \cfetlmmnemonic \SB_SMMBMALW
                                              \brief cFE Cfg Param \link #CFE_PLATFORM_SB_BUF_MEMORY_BYTES \endlink */

    uint32              SubscriptionsInUse;/**< \cfetlmmnemonic \SB_SMSIU
                                                \brief Number of current subscriptions */
    uint32              PeakSubscriptionsInUse;/**< \cfetlmmnemonic \SB_SMPSIU
                                                    \brief Peak number of subscriptions */
    uint32              MaxSubscriptionsAllowed;/**< \cfetlmmnemonic \SB_SMMSALW
                                                     \brief product of \link #CFE_PLATFORM_SB_MAX_MSG_IDS \endlink
                                                     and \link #CFE_PLATFORM_SB_MAX_DEST_PER_PKT \endlink */

    uint32              SBBuffersInUse;/**< \cfetlmmnemonic \SB_SMSBBIU
                                            \brief Number of SB message buffers currently in use */
    uint32              PeakSBBuffersInUse;/**< \cfetlmmnemonic \SB_SMPSBBIU
                                                \brief Max number of SB message buffers in use */

    uint32              MaxPipeDepthAllowed;/**< \cfetlmmnemonic \SB_SMMPDALW
                                                 \brief cFE Cfg Param \link #CFE_SB_MAX_PIPE_DEPTH \endlink */
    CFE_SB_PipeDepthStats_t PipeDepthStats[CFE_MISSION_SB_MAX_PIPES];/**< \cfetlmmnemonic \SB_SMPDS
                                                                  \brief Pipe Depth Statistics #CFE_SB_PipeDepthStats_t*/
} CFE_SB_StatsTlm_Payload_t;

typedef struct{
    CFE_SB_TlmHdr_t             Hdr;/**< \brief cFE Software Bus Telemetry Message Header */
    CFE_SB_StatsTlm_Payload_t    Payload;
} CFE_SB_StatsTlm_t;


/**
** \brief SB Routing File Entry
**
** Structure of one element of the routing information in response to #CFE_SB_SEND_ROUTING_INFO_CC
*/
typedef struct{
    CFE_SB_MsgId_t      MsgId;/**< \brief Message Id portion of the route */
    CFE_SB_PipeId_t     PipeId;/**< \brief Pipe Id portion of the route */
    uint8               State;/**< \brief Route Enabled or Disabled */
    uint16              MsgCnt;/**< \brief Number of msgs with this MsgId sent to this PipeId */
    char                AppName[CFE_MISSION_MAX_API_LEN];/**< \brief Pipe Depth Statistics */
    char                PipeName[CFE_MISSION_MAX_API_LEN];/**< \brief Pipe Depth Statistics */
 }CFE_SB_RoutingFileEntry_t;


/**
** \brief SB Map File Entry
**
** Structure of one element of the map information in response to #CFE_SB_SEND_MAP_INFO_CC
*/
typedef struct{
    CFE_SB_MsgId_Atom_t        MsgId;/**< \brief Message Id which has been subscribed to */
    CFE_SB_MsgRouteIdx_Atom_t  Index;/**< \brief Routing table index where pipe destinations are found */
}CFE_SB_MsgMapFileEntry_t;


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
typedef struct {

   uint8                SubType;/**< \brief Subscription or Unsubscription */
   CFE_SB_MsgId_t       MsgId;/**< \brief MsgId subscribed or unsubscribe to */
   CFE_SB_Qos_t         Qos;/**< \brief Quality of Service, used only for interprocessor communication */
   CFE_SB_PipeId_t      Pipe;/**< \brief Destination pipe id to send above msg id  */

} CFE_SB_SingleSubscriptionTlm_Payload_t;

typedef struct{
    CFE_SB_TlmHdr_t             Hdr;/**< \brief cFE Software Bus Telemetry Message Header */
    CFE_SB_SingleSubscriptionTlm_Payload_t Payload;
} CFE_SB_SingleSubscriptionTlm_t;


/**
** \brief SB Previous Subscriptions Entry
**
** This structure defines an entry used in the CFE_SB_PrevSubsPkt_t
** Intended to be used primarily by Software Bus Networking Application (SBN)
**
** Used in structure definition #CFE_SB_AllSubscriptionsTlm_t
*/
typedef struct {

   CFE_SB_MsgId_t       MsgId;/**< \brief MsgId portion of the subscription */
   CFE_SB_Qos_t         Qos;/**< \brief Qos portion of the subscription */
   CFE_SB_PipeId_t      Pipe;/**< \brief PipeId portion of the subscription */

}CFE_SB_SubEntries_t;


/**
** \cfesbtlm SB Previous Subscriptions Packet
**
** This structure defines the pkt(s) sent by SB that contains a list of all current
** subscriptions. This pkt is generated on cmd and intended to be used primarily by
** the Software Bus Networking Application (SBN). Typically, when the cmd is received
** there are more subscriptions than can fit in one pkt. The complete list of
** subscriptions is sent via a series of segmented pkts.
*/
typedef struct {

   uint32               PktSegment;/**< \brief Pkt number(starts at 1) in the series */
   uint32               TotalSegments;/**< \brief Total number of pkts needed to complete the request */
   uint32               Entries;/**< \brief Number of entries in the pkt */
   CFE_SB_SubEntries_t  Entry[CFE_SB_SUB_ENTRIES_PER_PKT];/**< \brief Array of #CFE_SB_SubEntries_t entries */
} CFE_SB_AllSubscriptionsTlm_Payload_t;

typedef struct{
    CFE_SB_TlmHdr_t             Hdr;/**< \brief cFE Software Bus Telemetry Message Header */
    CFE_SB_AllSubscriptionsTlm_Payload_t Payload;
} CFE_SB_AllSubscriptionsTlm_t;

/*
 * COMPATIBILITY TYPEDEFS:
 * In some circumstances applications may subscribe to telemetry from this component,
 * and therefore the name changes may break existing code.  For these situations a
 * typedef is created to continue supporting the previous name.  These should be
 * removed in the next CFE release.
 */
#ifndef CFE_OMIT_DEPRECATED_6_6

typedef CFE_SB_HousekeepingTlm_t        CFE_SB_HKMsg_t;
typedef CFE_SB_StatsTlm_t               CFE_SB_StatMsg_t;
typedef CFE_SB_AllSubscriptionsTlm_t    CFE_SB_PrevSubMsg_t;
typedef CFE_SB_SingleSubscriptionTlm_t  CFE_SB_SubRprtMsg_t;

#endif /* CFE_OMIT_DEPRECATED_6_6 */


#endif /* _cfe_sb_msg_ */
/*****************************************************************************/
