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
 *   Specification for the CFE Event Services (CFE_SB) command function codes
 *
 * @note
 *   This file should be strictly limited to the command/function code (CC)
 *   macro definitions.  Other definitions such as enums, typedefs, or other
 *   macros should be placed in the msgdefs.h or msg.h files.
 */
#ifndef CFE_SB_FCNCODES_H
#define CFE_SB_FCNCODES_H

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
**       #CFE_SB_NoopCmd_t
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
#define CFE_SB_NOOP_CC 0

/** \cfesbcmd Software Bus Reset Counters
**
**  \par Description
**       This command resets the following counters within the Software
**       Bus housekeeping telemetry:
**       - Command Execution Counter (\SB_CMDPC)
**       - Command Error Counter (\SB_CMDEC)
**       - No Subscribers Counter (\SB_NOSUBEC)
**       - Duplicate Subscriptions Counter (\SB_DUPSUBCNT)
**       - Msg Send Error Counter (\SB_MSGSNDEC)
**       - Msg Receive Error Counter (\SB_MSGRECEC)
**       - Internal Error Counter (\SB_INTERNALEC)
**       - Create Pipe Error Counter (\SB_NEWPIPEEC)
**       - Subscribe Error Counter (\SB_SUBSCREC)
**       - Pipe Overflow Error Counter (\SB_PIPEOVREC)
**       - Msg Limit Error Counter (\SB_MSGLIMEC)
**
**  \cfecmdmnemonic \SB_RESETCTRS
**
**  \par Command Structure
**       #CFE_SB_ResetCountersCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the
**       following telemetry:
**       - \b \c \SB_CMDPC - command execution counter will
**         be reset to 0
**       - All other counters listed in description will be reset to 0
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
#define CFE_SB_RESET_COUNTERS_CC 1

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
**       #CFE_SB_SendSbStatsCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the
**       following telemetry:
**       - \b \c \SB_CMDPC - command execution counter will increment
**       - Receipt of statistics packet with MsgId #CFE_SB_STATS_TLM_MID
**       - The #CFE_SB_SND_STATS_EID debug event message will be generated
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
#define CFE_SB_SEND_SB_STATS_CC 2

/** \cfesbcmd Write Software Bus Routing Info to a File
**
**  \par Description
**       This command will create a file containing the software bus routing
**       information. The routing information contains information about every
**       subscription that has been received through the SB subscription APIs.
**       An absolute path and filename may be specified in the command.
**       If this command field contains an empty string (NULL terminator as
**       the first character) the default file path and name is used.
**       The default file path and name is defined in the platform
**       configuration file as #CFE_PLATFORM_SB_DEFAULT_ROUTING_FILENAME.
**
**  \cfecmdmnemonic \SB_WRITEROUTING2FILE
**
**  \par Command Structure
**       #CFE_SB_WriteRoutingInfoCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the
**       following telemetry:
**       - \b \c \SB_CMDPC - command execution counter will increment.
**         NOTE: the command counter is incremented when the request is accepted,
**         before writing the file, which is performed as a background task.
**       - The file specified in the command (or the default specified
**         by the #CFE_PLATFORM_SB_DEFAULT_ROUTING_FILENAME configuration parameter) will be
**         updated with the latest information.
**       - The #CFE_SB_SND_RTG_EID debug event message will be generated
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - A previous request to write a software bus information file has not yet completed
**       - The specified FileName cannot be parsed
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
*/
#define CFE_SB_WRITE_ROUTING_INFO_CC 3

/** \cfesbcmd Enable Software Bus Route
**
**  \par Description
**       This command will enable a particular destination. The destination
**       is specified in terms of MsgID and PipeID. The MsgId and PipeID are
**       parameters in the command. All destinations are enabled by default.
**       This command is needed only after a #CFE_SB_DISABLE_ROUTE_CC command
**       is used.
**
**  \cfecmdmnemonic \SB_ENAROUTE
**
**  \par Command Structure
**       #CFE_SB_EnableRouteCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the
**       following telemetry:
**       - \b \c \SB_CMDPC - command execution counter will increment
**       - View routing information #CFE_SB_WRITE_ROUTING_INFO_CC to verify
**         enable/disable state change
**       - The #CFE_SB_ENBL_RTE2_EID debug event message will be generated
**       - Destination will begin receiving messages
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - the MsgId or PipeId parameters do not pass validation
**       - the destination does not exist.
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \SB_CMDEC - command error counter will increment
**       - A command specific error event message is issued for all error
**         cases. See #CFE_SB_ENBL_RTE1_EID or #CFE_SB_ENBL_RTE3_EID
**
**  \par Criticality
**       This command is not inherently dangerous.
*/
#define CFE_SB_ENABLE_ROUTE_CC 4

/** \cfesbcmd Disable Software Bus Route
**
**  \par Description
**       This command will disable a particular destination. The destination
**       is specified in terms of MsgID and PipeID. The MsgId and PipeID are
**       parameters in the command. All destinations are enabled by default.
**
**  \cfecmdmnemonic \SB_DISROUTE
**
**  \par Command Structure
**       #CFE_SB_DisableRouteCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the
**       following telemetry:
**       - \b \c \SB_CMDPC - command execution counter will increment
**       - View routing information #CFE_SB_WRITE_ROUTING_INFO_CC to verify
**         enable/disable state change
**       - The #CFE_SB_DSBL_RTE2_EID debug event message will be generated
**       - Destination will stop receiving messages
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - the MsgId or PipeId parameters do not pass validation
**       - the destination does not exist.
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \SB_CMDEC - command error counter will increment
**       - A command specific error event message is issued for all error
**         cases. See #CFE_SB_DSBL_RTE1_EID or #CFE_SB_DSBL_RTE3_EID
**
**  \par Criticality
**       This command is not intended to be used in nominal conditions. It is
**       possible to get into a state where a destination cannot be re-enabled
**       without resetting the processor. For instance, sending this command
**       with #CFE_SB_CMD_MID and the SB_Cmd_Pipe would inhibit any ground
**       commanding to the software bus until the processor was reset. There
**       are similar problems that may occur when using this command.
*/
#define CFE_SB_DISABLE_ROUTE_CC 5

/** \cfesbcmd Write Pipe Info to a File
**
**  \par Description
**       This command will create a file containing the software bus pipe
**       information. The pipe information contains information about every
**       pipe that has been created through the #CFE_SB_CreatePipe API. An
**       absolute path and filename may be specified in the command.
**       If this command field contains an empty string (NULL terminator as
**       the first character) the default file path and name is used.
**       The default file path and name is defined in the platform
**       configuration file as #CFE_PLATFORM_SB_DEFAULT_PIPE_FILENAME.
**
**  \cfecmdmnemonic \SB_WRITEPIPE2FILE
**
**  \par Command Structure
**       #CFE_SB_WritePipeInfoCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the
**       following telemetry:
**       - \b \c \SB_CMDPC - command execution counter will increment.
**         NOTE: the command counter is incremented when the request is accepted,
**         before writing the file, which is performed as a background task.
**       - The file specified in the command (or the default specified
**         by the #CFE_PLATFORM_SB_DEFAULT_PIPE_FILENAME configuration parameter) will be
**         updated with the latest information.
**       - The #CFE_SB_SND_RTG_EID debug event message will be generated
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - A previous request to write a software bus information file has not yet completed
**       - The specified FileName cannot be parsed
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
*/
#define CFE_SB_WRITE_PIPE_INFO_CC 7

/** \cfesbcmd Write Map Info to a File
**
**  \par This command will create a file containing the software bus message
**       map information. The message map is a lookup table (an array of
**       uint16s)that allows fast access to the correct routing table element
**       during a software bus send operation. This is diagnostic
**       information that may be needed due to the dynamic nature of the
**       cFE software bus. An absolute path and filename may be specified
**       in the command. If this command field contains an empty string
**       (NULL terminator as the first character) the default file path and
**       name is used. The default file path and name is defined in the
**       platform configuration file as #CFE_PLATFORM_SB_DEFAULT_MAP_FILENAME.
**
**  \cfecmdmnemonic \SB_WRITEMAP2FILE
**
**  \par Command Structure
**       #CFE_SB_WriteMapInfoCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the
**       following telemetry:
**       - \b \c \SB_CMDPC - command execution counter will increment.
**         NOTE: the command counter is incremented when the request is accepted,
**         before writing the file, which is performed as a background task.
**       - The file specified in the command (or the default specified
**         by the #CFE_PLATFORM_SB_DEFAULT_MAP_FILENAME configuration parameter) will be
**         updated with the latest information.
**       - The #CFE_SB_SND_RTG_EID debug event message will be generated
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - A previous request to write a software bus information file has not yet completed
**       - The specified FileName cannot be parsed
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
*/
#define CFE_SB_WRITE_MAP_INFO_CC 8

/** \cfesbcmd Enable Subscription Reporting Command
**
**  \par Description
**       This command will enable subscription reporting and is intended to
**       be used only by the CFS SBN (Software Bus Networking) Application.
**       It is not intended to be sent from the ground or used by operations.
**       When subscription reporting is enabled, SB will generate
**       and send a software bus packet for each subscription received.
**       The software bus packet that is sent contains the information
**       received in the subscription API. This subscription report is
**       needed by SBN if offboard routing is required.
*
**  \cfecmdmnemonic \SB_ENASUBRPTG
**
**  \par Command Structure
**       #CFE_SB_EnableSubReportingCmd_t
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
#define CFE_SB_ENABLE_SUB_REPORTING_CC 9

/** \cfesbcmd Disable Subscription Reporting Command
**
**  \par Description
**       This command will disable subscription reporting and is intended to
**       be used only by the CFS SBN (Software Bus Networking) Application.
**       It is not intended to be sent from the ground or used by operations.
**       When subscription reporting is enabled, SB will generate
**       and send a software bus packet for each subscription received.
**       The software bus packet that is sent contains the information
**       received in the subscription API. This subscription report is
**       needed by SBN if offboard routing is required.
**
**  \cfecmdmnemonic \SB_DISSUBRPTG
**
**  \par Command Structure
**       #CFE_SB_DisableSubReportingCmd_t
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
**       #CFE_SB_SendPrevSubsCmd_t
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
#define CFE_SB_SEND_PREV_SUBS_CC 11

#endif
