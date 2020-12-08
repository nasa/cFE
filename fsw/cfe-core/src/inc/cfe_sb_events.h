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

/*
**  File: cfe_sb_events.h
**
**  Purpose:
**      cFE Software Bus (SB) Event IDs
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**     cFE Flight Software Application Developers Guide
**
**************************************************************************/
#ifndef _cfe_sb_events_
#define _cfe_sb_events_

/* **************************
** ****** Maximum EID. ******
** **************************
** The EID's below are not necessarily in order, so it can be difficult to
** determine what the next EID is to use. When you add EID's, start with MAX_EID + 1
** and when you're done adding, set this to the highest EID you used. It may
** be worthwhile to, on occasion, re-number the EID's to put them back in order.
*/
#define CFE_SB_MAX_EID                  67

/*
** SB task event message ID's.
*/
/*
** Event ID's
*/

/** \brief <tt> 'cFE SB Initialized' </tt>
**  \event <tt> 'cFE SB Initialized' </tt>
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is issued when the Software Bus Task completes its
**  initialization.
**/
#define CFE_SB_INIT_EID                 1

/** \brief <tt> 'CreatePipeErr:Bad Input Arg:app=\%s,ptr=0x\%x,depth=\%d,maxdepth=\%d' </tt>
**  \event <tt> 'CreatePipeErr:Bad Input Arg:app=\%s,ptr=0x\%x,depth=\%d,maxdepth=\%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This error event message is issued when the #CFE_SB_CreatePipe API receives a bad
**  argument. In this case, a bad argument is defined by the following:
**  A NULL PipeIdPtr, PipeDepth = 0 and PipeDepth > cfg param #CFE_PLATFORM_SB_MAX_PIPE_DEPTH
**/
#define CFE_SB_CR_PIPE_BAD_ARG_EID      2

/** \brief <tt> 'CreatePipeErr:Max Pipes(\%d)In Use.app \%s' </tt>
**  \event <tt> 'CreatePipeErr:Max Pipes(\%d)In Use.app \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This error event message is issued when the #CFE_SB_CreatePipe API is called and
**  the maximum number of pipes (defined by cfg param #CFE_PLATFORM_SB_MAX_PIPES) are in use.
**/
#define CFE_SB_MAX_PIPES_MET_EID        3


/** \brief <tt> 'CreatePipeErr:OS_QueueCreate returned \%d,app \%s' </tt>
**  \event <tt> 'CreatePipeErr:OS_QueueCreate returned \%d,app \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This error event message is issued when the #CFE_SB_CreatePipe API is called and
**  the OS returns an error when the OS returns an error from the OS_QueueCreate API.
**  The error status returned by the OS is displayed in the event. Most commonly,
**  this event is displayed as a result of trying to create pipes with the same name.
**/
#define CFE_SB_CR_PIPE_ERR_EID          4

/** \brief <tt> 'Pipe Created:name \%s,id \%d,app \%s' </tt>
**  \event <tt> 'Pipe Created:name \%s,id \%d,app \%s' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This debug event message is issued when a pipe was successfully created in the
**  #CFE_SB_CreatePipe API.
**/
#define CFE_SB_PIPE_ADDED_EID           5

/** \brief <tt> 'SetPipeOptsErr:Invalid pipe id (\%d).app \%s' </tt>
**  \event <tt> 'SetPipeOptsErr:Invalid pipe id (\%d).app \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This error event message is issued when the #CFE_SB_SetPipeOpts API is called and
**  the PipeID is invalid.
**/
#define CFE_SB_SETPIPEOPTS_ID_ERR_EID        55

/** \brief <tt> 'SetPipeOptsErr:Caller not owner (\%d).app \%s' </tt>
**  \event <tt> 'SetPipeOptsErr:Caller not owner (\%d).app \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This error event message is issued when the #CFE_SB_SetPipeOpts API is called and
**  the pipe is owned by another app ID.
**/
#define CFE_SB_SETPIPEOPTS_OWNER_ERR_EID        56

/** \brief <tt> 'SetPipeOpts: Options set (\%d). app \%s' </tt>
** \event <tt> 'SetPipeOpts: Options set (\%d). app \%s' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This debug event is generated when options are set.
**/
#define CFE_SB_SETPIPEOPTS_EID        57

/** \brief <tt> 'GetPipeOptsErr:Invalid pipe id (\%d).app \%s' </tt>
**  \event <tt> 'GetPipeOptsErr:Invalid pipe id (\%d).app \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This error event message is issued when the #CFE_SB_GetPipeOpts API is called and
**  the PipeID is invalid.
**/
#define CFE_SB_GETPIPEOPTS_ID_ERR_EID        58

/** \brief <tt> 'GetPipeOptsErr:Invalid opts ptr.app \%s' </tt>
**  \event <tt> 'GetPipeOptsErr:Invalid opts ptr.app \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This error event message is issued when the #CFE_SB_GetPipeOpts API is called and
**  the pointer is invalid.
**/
#define CFE_SB_GETPIPEOPTS_PTR_ERR_EID        59

/** \brief <tt> 'GetPipeOpts: Options retrieved. app \%s' </tt>
** \event <tt> 'GetPipeOpts: Options retrieved. app \%s' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This debug event is generated when options are retrieved.
**/
#define CFE_SB_GETPIPEOPTS_EID        60

/** \brief <tt> 'GetPipeName: Name retrieved. NameOut \%s,Id \%d, app \%s' </tt>
** \event <tt> 'GetPipeName: Name retrieved. NameOut \%s,Id \%d, app \%s' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This debug event is generated when name is retrieved by id.
**/
#define CFE_SB_GETPIPENAME_EID        62

/** \brief <tt> 'GetPipeName: Null ptr error. Id \%d, app \%s' </tt>
** \event <tt> 'GetPipeName: Null ptr error. Id \%d, app \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This debug event is generated when the name buffer ptr is null.
**/
#define CFE_SB_GETPIPENAME_NULL_PTR_EID        63

/** \brief <tt> 'GetPipeName: Id error. NameOut \%s,Id \%d, app \%s' </tt>
** \event <tt> 'GetPipeName: Id error. NameOut \%s,Id \%d, app \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This debug event is generated when name is retrieved by id.
**/
#define CFE_SB_GETPIPENAME_ID_ERR_EID        64

/** \brief <tt> 'GetPipeIdByName: ID retrieved. Name \%s,IdOut 0x\%x, app \%s' </tt>
** \event <tt> 'GetPipeIdByName: ID retrieved. Name \%s,IdOut 0x\%x, app \%s' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This debug event is generated when id is retrieved by name.
**/
#define CFE_SB_GETPIPEIDBYNAME_EID        65

/** \brief <tt> 'GetPipeIdByName Err:Bad input argument,Name 0x\%x,IdOut 0x%x,App \%s' </tt>
**  \event <tt> 'GetPipeIdByName Err:Bad input argument,Name 0x\%x,IdOut 0x%x,App \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This error event message is issued when the #CFE_SB_GetPipeIdByName API receives a
**  NULL ptr as an argument.
**/
#define CFE_SB_GETPIPEIDBYNAME_NULL_ERR_EID         66

/** \brief <tt> 'GetPipeIdByName Err:Name not found,Name \%s,IdOut 0x%x,App \%s' </tt>
**  \event <tt> 'GetPipeIdByName Err:Name not found,Name \%s,IdOut 0x%x,App \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This error event message is issued when the #CFE_SB_GetPipeIdByName API receives an
**  invalid name.
**/
#define CFE_SB_GETPIPEIDBYNAME_NAME_ERR_EID         67

/** \brief <tt> 'Subscribe Err:Bad Arg,MsgId 0x\%x,PipeId \%d,app \%s,scope \%d' </tt>
**  \event <tt> 'Subscribe Err:Bad Arg,MsgId 0x\%x,PipeId \%d,app \%s,scope \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This error event message is issued when one of the Subscribe API's are called
**  with an invalid MsgId. An invalid MsgId is defined as being greater than the
**  cfg param #CFE_PLATFORM_SB_HIGHEST_VALID_MSGID.
**
**/
#define CFE_SB_SUB_ARG_ERR_EID          6

/** \brief <tt> 'Duplicate Subscription,MsgId 0x\%x on \%s pipe,app \%s' </tt>
**  \event <tt> 'Duplicate Subscription,MsgId 0x\%x on \%s pipe,app \%s' </tt>
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This info event message is issued when a subscription request is received that
**  already exists in the routing table. A duplicate subscription is defined by a
**  matching MsgId and PipeId. No other parameters are used in detecting a duplicate
**  subscription.
**  NOTE: By default, SB filters this event. The EVS filter algorithm allows the
**  first event to pass through the filter, but all subsequent events with this
**  event id will be filtered. A command must be sent to unfilter this event if
**  the user desires to see it.
**/
#define CFE_SB_DUP_SUBSCRIP_EID         7

/** \brief <tt> 'Subscribe Err:Max Msgs(\%d)In Use,MsgId 0x\%x,pipe \%s,app \%s' </tt>
**  \event <tt> 'Subscribe Err:Max Msgs(\%d)In Use,MsgId 0x\%x,pipe \%s,app \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This error event message is issued when one of the SB subscribe APIs are called
**  with a new MsgId, and SB cannot accommodate the new MsgId because the maximum
**  number of MsgIds are in use. The maximum number of MsgIds is defined by cfg param
**  #CFE_PLATFORM_SB_MAX_MSG_IDS. This cfg param dictates the number of elements in the SB
**  routing table. There is one element per MsgId. The user may monitor the routing
**  table utilization figures (msgids currently in use, high water mark and max
**  allowed) by sending the SB cmd to dump the SB statistics data.
**/
#define CFE_SB_MAX_MSGS_MET_EID         8

/** \brief <tt> 'Subscribe Err:Max Dests(\%d)In Use For Msg 0x\%x,pipe \%s,app \%s' </tt>
**  \event <tt> 'Subscribe Err:Max Dests(\%d)In Use For Msg 0x\%x,pipe \%s,app \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This error event message is issued when a subscription request is received and
**  all destinations for that MsgId are in use. The number of destinations per msgid
**  is a configuration parameter named #CFE_PLATFORM_SB_MAX_DEST_PER_PKT. A destination is
**  defined as a pipe.
**/
#define CFE_SB_MAX_DESTS_MET_EID        9

/** \brief <tt> 'Subscription Rcvd:MsgId 0x\%x on \%s(\%d),app \%s' </tt>
**  \event <tt> 'Subscription Rcvd:MsgId 0x\%x on \%s(\%d),app \%s' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This debug event message is issued when a subscription is successfully made
**  through one of the SB Subscribe API's
**/
#define CFE_SB_SUBSCRIPTION_RCVD_EID    10

/** \brief <tt> 'UnSubscribe Err:Bad Arg,MsgId 0x\%x,PipeId \%d,app \%s,scope \%d' </tt>
**  \event <tt> 'UnSubscribe Err:Bad Arg,MsgId 0x\%x,PipeId \%d,app \%s,scope \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This error event message is issued when a request to unsubscribe fails due to
**  an invalid msgid or an invalid pipeid in one of SB's unsubscribe API's. The msgid
**  must be less than cfg param #CFE_PLATFORM_SB_HIGHEST_VALID_MSGID and the pipeid must have
**  been created and have a value less than cfg param #CFE_PLATFORM_SB_MAX_PIPES. The SB pipe
**  table may be viewed to verify its value or existence.
**/
#define CFE_SB_UNSUB_ARG_ERR_EID        11

/** \brief <tt> 'Unsubscribe Err:No subs for Msg 0x\%x on \%s,app \%s' </tt>
**  \event <tt> 'Unsubscribe Err:No subs for Msg 0x\%x on \%s,app \%s' </tt>
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This info event message is issued when a request to unsubscribe fails due to
**  a non existent msgid/pipeid combination in the SB routing table. The SB routing
**  table may be viewed to see a list of valid msgid/pipeid combinations.
**/
#define CFE_SB_UNSUB_NO_SUBS_EID        12

/** \brief <tt> 'Send Err:Bad input argument,Arg 0x\%x,App \%s' </tt>
**  \event <tt> 'Send Err:Bad input argument,Arg 0x\%x,App \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This error event message is issued when a transmit API receives an
**  invalid (possibly NULL) ptr as an argument.
**/
#define CFE_SB_SEND_BAD_ARG_EID         13

/** \brief <tt> 'No subscribers for MsgId 0x\%x,sender \%s' </tt>
**  \event <tt> 'No subscribers for MsgId 0x\%x,sender \%s' </tt>
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This info event message is issued when a transmit API is called and there
**  are no subscribers (therefore no destinations) for the message to be sent. Each
**  time the SB detects this situation, the corresponding SB telemetry point is
**  incremented..
**  NOTE: By default, SB filters this event. The EVS filter algorithm allows the
**  first event to pass through the filter, but all subsequent events with this
**  event id will be filtered. A command must be sent to unfilter this event if
**  the user desires to see it.
**/
#define CFE_SB_SEND_NO_SUBS_EID         14

/** \brief <tt> 'Send Err:Msg Too Big MsgId=0x\%x,app=\%s,size=\%d,MaxSz=\%d' </tt>
**  \event <tt> 'Send Err:Msg Too Big MsgId=0x\%x,app=\%s,size=\%d,MaxSz=\%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This error event message is issued when a transmit API is called and the
**  packet length field in the message header implies that the message size exceeds
**  the max size defined by mission cfg param #CFE_MISSION_SB_MAX_SB_MSG_SIZE. The request to
**  send the message is denied, there is no partial packet sent.
**/
#define CFE_SB_MSG_TOO_BIG_EID          15

/** \brief <tt> 'Send Err:Request for Buffer Failed. MsgId 0x\%x,app \%s,size \%d' </tt>
**  \event <tt> 'Send Err:Request for Buffer Failed. MsgId 0x\%x,app \%s,size \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This error event message is issued when a transmit API fails to receive
**  the necessary buffer memory from the ES memory pool. This could be an indication
**  that the cfg param #CFE_PLATFORM_SB_BUF_MEMORY_BYTES is set too low. To check this, send SB
**  cmd to dump the SB statistics pkt and view the buffer memory parameters.
**/
#define CFE_SB_GET_BUF_ERR_EID          16

/** \brief <tt> 'Send Err:Msg Limit Err MsgId 0x\%x,pipe \%s,sender \%s' </tt>
**  \event <tt> 'Send Err:Msg Limit Err MsgId 0x\%x,pipe \%s,sender \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This error event message is issued when a transmit API cannot route the
**  MsgId (displayed in event) to the pipe (displayed in the event) because the pipe
**  currently contains the maximum number of messages of this type (MsgId). This is
**  typically an indication that the receiver is not reading its pipe fast enough, or
**  at all. A less typical scenerio is that the sender is sending a burst of pkts of
**  this type (or MsgId) and the receiver (owner of 'pipe') cannot keep up. The
**  subscriber of the message dictates this limit count in the 'MsgLim' parameter of
**  the #CFE_SB_SubscribeEx API or uses the default value of 4 if using the
**  #CFE_SB_Subscribe API.
**/
#define CFE_SB_MSGID_LIM_ERR_EID        17

/** \brief <tt> 'Rcv Err:Bad Input Arg:BufPtr 0x\%x,pipe \%d,t/o \%d,app \%s' </tt>
**  \event <tt> 'Rcv Err:Bad Input Arg:BufPtr 0x\%x,pipe \%d,t/o \%d,app \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This error event message is issued when an invalid paramter is passed into the
**  #CFE_SB_ReceiveBuffer API. Two possibile problems would be the first parameter (*BufPtr)
**  being NULL or the third paramter (TimeOut) being less than -1.
**/
#define CFE_SB_RCV_BAD_ARG_EID          18

/** \brief <tt> 'Rcv Err:PipeId \%d does not exist,app \%s' </tt>
**  \event <tt> 'Rcv Err:PipeId \%d does not exist,app \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This error event message is issued when an invalid PipeId is passed into the
**  #CFE_SB_ReceiveBuffer API. The SB Pipe Table shows all valid PipeIds and may be viewed
**  for verification.
**/
#define CFE_SB_BAD_PIPEID_EID           19


/** \brief <tt> 'Subscribe Err:Request for Destination Blk failed for Msg 0x\%x,Pipe \%s' </tt>
**  \event <tt> 'Subscribe Err:Request for Destination Blk failed for Msg 0x\%x,Pipe \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This error event message is issued when the SB receives an error from the memory
**  pool in the attempt to obtain a new destination block. Then memory pool statistics
**  may be viewed by sending the related ES command. 
**/
#define CFE_SB_DEST_BLK_ERR_EID         20


/** \brief <tt> 'Send Err:Invalid msgid in msg,MsgId 0x\%x,App \%s' </tt>
**  \event <tt> 'Send Err:Invalid msgid in msg,MsgId 0x\%x,App \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This error event message is issued when a transmit API is called and
**  the SB discovers that the message to send has a msg id that is invalid. It may be
**  due to a msg id  that is greater than cfg parameter #CFE_PLATFORM_SB_HIGHEST_VALID_MSGID
**/
#define CFE_SB_SEND_INV_MSGID_EID       21

/** \brief <tt> 'Sending Subscription Report Msg=0x\%x,Pipe=\%d,Stat=0x\%x' </tt>
**  \event <tt> 'Sending Subscription Report Msg=0x\%x,Pipe=\%d,Stat=0x\%x' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This debug event message is issued when SB subscription reporting is enabled,
**  (which is disabled by default) and a subscription is successfully received.
**/
#define CFE_SB_SUBSCRIPTION_RPT_EID     22

/** \brief <tt> 'Msg hash collision: MsgId = 0x\%x, collisions = \%u' </tt>
**  \event <tt> 'Msg hash collision: MsgId = 0x\%x, collisions = \%u' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is generated when a message id hash collision occurs when subscribing
**  to a message.  Collisions indicate how many slots were incremented to find an opening.
**
**  Number of collisions will directly impact software bus performance.  These can be resolved
**  by adjusting MsgId values or increasing CFE_PLATFORM_SB_MAX_MSG_IDS.
**/
#define CFE_SB_HASHCOLLISION_EID        23

/** \brief <tt> 'Pipe Overflow,MsgId 0x\%x,pipe \%s,stat 0x\%x,app \%s' </tt>
**  \event <tt> 'Pipe Overflow,MsgId 0x\%x,pipe \%s,stat 0x\%x,app \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This error event message is issued when a transmit API is called and
**  encounters an error when attempting to write the msg to the destination pipe
**  (which is an underlying queue). This could indicate that the owner of the pipe is
**  not readings its messages fast enough or at all. It may also mean that the
**  pipe depth is not deep enough. The pipe depth is an input parameter to the
**  #CFE_SB_CreatePipe API.
**/
#define CFE_SB_Q_FULL_ERR_EID           25

/** \brief <tt> 'Pipe Write Err,MsgId 0x\%x,pipe \%s,stat 0x\%x,app \%s' </tt>
**  \event <tt> 'Pipe Write Err,MsgId 0x\%x,pipe \%s,stat 0x\%x,app \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This error event message is issued when a transmit API is called and
**  encounters an error when attempting to write the msg to the destination pipe
**  (which is an underlying queue). More precisely, the OS API #OS_QueuePut has
**  returned an unexpected error. The return code is displayed in the event. For
**  more information, the user may look up the return code in the OSAL documention or
**  source code.
**/
#define CFE_SB_Q_WR_ERR_EID             26

/** \brief <tt> 'Pipe Read Err,pipe \%s,app \%s,stat 0x\%x' </tt>
**  \event <tt> 'Pipe Read Err,pipe \%s,app \%s,stat 0x\%x' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This error event message is issued when a transmit API is called and
**  encounters an error when attempting to read the msg from the destination pipe
**  (which is an underlying queue). More precisely, the OS API #OS_QueueGet has
**  returned an unexpected error. The return code is displayed in the event. For
**  more information, the user may look up the return code in the OSAL documention or
**  source code.
**/
#define CFE_SB_Q_RD_ERR_EID             27

/** \brief <tt> 'No-op Cmd Rcvd' </tt>
**  \event <tt> 'No-op Cmd Rcvd' </tt>
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This info event message is issued in response an SB NO-OP command
**/
#define CFE_SB_CMD0_RCVD_EID            28

/** \brief <tt> 'Reset Counters Cmd Rcvd' </tt>
**  \event <tt> 'Reset Counters Cmd Rcvd' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This debug event message is issued in response an SB Reset Counters command
**/
#define CFE_SB_CMD1_RCVD_EID            29


/** \brief <tt> 'Software Bus Statistics packet sent' </tt>
**  \event <tt> 'Software Bus Statistics packet sent' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This debug event message is issued when SB receives a cmd to send the SB
**  statistics pkt.
**/
#define CFE_SB_SND_STATS_EID            32

/** \brief <tt> 'Enbl Route Cmd:Route does not exist.Msg 0x\%x,Pipe \%d' </tt>
**  \event <tt> 'Enbl Route Cmd:Route does not exist.Msg 0x\%x,Pipe \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This error event message is issued when SB receives a cmd to enable a route that
**  does not exist in the routing table. A route is defined by a MsgId, PipeId pair.
**/
#define CFE_SB_ENBL_RTE1_EID            33

/** \brief <tt> 'Enabling Route,Msg 0x\%x,Pipe \%d' </tt>
**  \event <tt> 'Enabling Route,Msg 0x\%x,Pipe \%d' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This debug event message is issued when SB receives a cmd to enable a route and
**  the request is successfully executed.
**/
#define CFE_SB_ENBL_RTE2_EID            34

/** \brief <tt> 'Enbl Route Cmd:Invalid Param.Msg 0x\%x,Pipe \%d' </tt>
**  \event <tt> 'Enbl Route Cmd:Invalid Param.Msg 0x\%x,Pipe \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This error event message is issued when SB receives a cmd to enable a route and
**  the MsgId or PipeId does not pass the validation checks. The MsgId must be less
**  than cfg param #CFE_PLATFORM_SB_HIGHEST_VALID_MSGID. The PipeId must exist and be less than
**  cfg param #CFE_PLATFORM_SB_MAX_PIPES. The SB pipe table may be viewed to verify the PipeId
**  existence.
**/
#define CFE_SB_ENBL_RTE3_EID            35

/** \brief <tt> 'Disable Route Cmd:Route does not exist,Msg 0x\%x,Pipe \%d' </tt>
**  \event <tt> 'Disable Route Cmd:Route does not exist,Msg 0x\%x,Pipe \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This error event message is issued when SB receives a cmd to disable a route that
**  does not exist in the routing table. A route is defined by a MsgId, PipeId pair.
**/
#define CFE_SB_DSBL_RTE1_EID            36

/** \brief <tt> 'Route Disabled,Msg 0x\%x,Pipe \%d' </tt>
**  \event <tt> 'Route Disabled,Msg 0x\%x,Pipe \%d' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This debug event message is issued when SB receives a cmd to disable a route and
**  the request is successfully executed.
**/
#define CFE_SB_DSBL_RTE2_EID            37

/** \brief <tt> 'Disable Route Cmd:Invalid Param.Msg 0x\%x,Pipe \%d' </tt>
**  \event <tt> 'Disable Route Cmd:Invalid Param.Msg 0x\%x,Pipe \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This error event message is issued when SB receives a cmd to disable a route and
**  the MsgId or PipeId does not pass the validation checks. The MsgId must be less
**  than cfg param #CFE_PLATFORM_SB_HIGHEST_VALID_MSGID. The PipeId must exist and be less than
**  cfg param #CFE_PLATFORM_SB_MAX_PIPES. The SB pipe table may be viewed to verify the PipeId
**  existence.
**/
#define CFE_SB_DSBL_RTE3_EID            38

/** \brief <tt> '\%s written:Size=\%d,Entries=\%d' </tt>
**  \event <tt> '\%s written:Size=\%d,Entries=\%d' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This debug event message is issued after the SB routing info file, pipe info 
**  file or the map info file is written and closed. This is done is response to 
**  the SB 'Send Routing Info' cmd, the SB 'Send pipe Info' cmd or the SB 'Send  
**  Map Info' cmd, respectively.
**/
#define CFE_SB_SND_RTG_EID              39


/** \brief <tt> 'Error creating file \%s, stat=0x\%x' </tt>
**  \event <tt> 'Error creating file \%s, stat=0x\%x' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This error event message is issued when the SB 'Send Routing Info' cmd is
**  received and the file create fails. The event displays the status received from
**  the OS.
**/
#define CFE_SB_SND_RTG_ERR1_EID         40

/** \brief <tt> 'Invalid Cmd, Unexpected Command Code \%d' </tt>
**  \event <tt> 'Invalid Cmd, Unexpected Command Code \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This error event message is issued when the SB receives a cmd that has an
**  unexpected cmd code.
**/
#define CFE_SB_BAD_CMD_CODE_EID         42

/** \brief <tt> 'Invalid Cmd, Unexpected Msg Id: 0x\%x' </tt>
**  \event <tt> 'Invalid Cmd, Unexpected Msg Id: 0x\%x' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This error event message is issued when the SB receives a msg that has an
**  unexpected msg id.
**/
#define CFE_SB_BAD_MSGID_EID            43


/** \brief <tt> 'Full Sub Pkt \%d Sent,Entries=\%d,Stat=0x\%x\n' </tt>
**  \event <tt> 'Full Sub Pkt \%d Sent,Entries=\%d,Stat=0x\%x\n' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This debug event message is issued in response to the
**  'Send Previous Subscriptions' command and a full pkt segment is sent.
**/
#define CFE_SB_FULL_SUB_PKT_EID         44

/** \brief <tt> 'Partial Sub Pkt \%d Sent,Entries=\%d,Stat=0x\%x' </tt>
**  \event <tt> 'Partial Sub Pkt \%d Sent,Entries=\%d,Stat=0x\%x' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This debug event message is issued in response to the
**  'Send Previous Subscriptions' command and a partial pkt segment is sent.
**/
#define CFE_SB_PART_SUB_PKT_EID         45


/** \brief <tt> 'Pipe Delete Error:Bad Argument,PipedId \%d,Requestor \%s,Idx \%d,Stat \%d' </tt>
**  \event <tt> 'Pipe Delete Error:Bad Argument,PipedId \%d,Requestor \%s,Idx \%d,Stat \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This error event message is issued from CFE_SB_DeletePipeFull when an
**  invalid pipe ID is passed in
**/
#define CFE_SB_DEL_PIPE_ERR1_EID        46

/** \brief <tt> 'Pipe Deleted:id \%d,owner \%s' </tt>
**  \event <tt> 'Pipe Deleted:id \%d,owner \%s' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This debug event message is issued when the #CFE_SB_DeletePipe API is called and
**  the request is successfully completed.
**/
#define CFE_SB_PIPE_DELETED_EID	        47

/** \brief <tt> 'Subscription Removed:Msg 0x\%x on pipe \%d,app \%s' </tt>
**  \event <tt> 'Subscription Removed:Msg 0x\%x on pipe \%d,app \%s' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This debug event message is issued when #CFE_SB_Unsubscribe API is called
**  and the request is successfully completed.
**/
#define CFE_SB_SUBSCRIPTION_REMOVED_EID 48

/** \brief <tt> 'File write,byte cnt err,file \%s,request=\%d,actual=\%d' </tt>
**  \event <tt> 'File write,byte cnt err,file \%s,request=\%d,actual=\%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This error event message is issued when one of many SB's file write operations
**  is unsuccessful. This event is a result of #CFE_FS_WriteHeader or OS_write
**  returning something other than the number of bytes requested to be written.
**  The requested value and the return value are displayed in the event.
**/
#define CFE_SB_FILEWRITE_ERR_EID        49

/** \brief <tt> 'Subscribe Err:Invalid Pipe Id,Msg=0x\%x,PipeId=\%d,App \%s' </tt>
**  \event <tt> 'Subscribe Err:Invalid Pipe Id,Msg=0x\%x,PipeId=\%d,App \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This error event message is issued when the input PipeId has a value that is not
**  listed in the pipe table. This typically means that the pipe does not exist.
**  The pipe table may be viewed for verification.
**/
#define CFE_SB_SUB_INV_PIPE_EID         50

/** \brief <tt> 'Subscribe Err:Caller(\%s) is not the owner of pipe \%d, Msg=0x\%x' </tt>
**  \event <tt> 'Subscribe Err:Caller(\%s) is not the owner of pipe \%d, Msg=0x\%x' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This error event message is issued when one of the SB subscribe API's are called
**  and the requestor is not the owner of the pipe. Only the owner of the pipe may
**  subscribe to messages on the pipe.
**/
#define CFE_SB_SUB_INV_CALLER_EID       51

/** \brief <tt> 'Unsubscribe Err:Invalid Pipe Id Msg=0x\%x,Pipe=\%d,app=\%s' </tt>
**  \event <tt> 'Unsubscribe Err:Invalid Pipe Id Msg=0x\%x,Pipe=\%d,app=\%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This error event message is issued when one of the SB unsubscribe API's are
**  called and the input parameter PipeId is not listed in the pipe table.
**  This typically means that the pipe does not exist. The pipe table may be viewed
**  for verification.
**/
#define CFE_SB_UNSUB_INV_PIPE_EID       52

/** \brief <tt> 'Unsubscribe Err:Caller(\%s) is not the owner of pipe \%d,Msg=0x\%x' </tt>
**  \event <tt> 'Unsubscribe Err:Caller(\%s) is not the owner of pipe \%d,Msg=0x\%x' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This error event message is issued when one of the SB unsubscribe API's are
**  called and the requestor is not the owner of the pipe (or ES). Only the owner of
**  the pipe(or ES for cleanup purposes)may unsubscribe messages from a pipe.
**/
#define CFE_SB_UNSUB_INV_CALLER_EID     53

/** \brief <tt> 'Pipe Delete Error:Caller(\%s) is not the owner of pipe \%d' </tt>
**  \event <tt> 'Pipe Delete Error:Caller(\%s) is not the owner of pipe \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This error event message is issued when the #CFE_SB_DeletePipe API is called by a
**  task that is not the owner of the pipe. Pipes may be deleted only by the task
**  that created the pipe or ES(for cleanup purposes).
**/
#define CFE_SB_DEL_PIPE_ERR2_EID        54

/** \brief <tt> 'Invalid cmd length: ID = 0x\%X, CC = \%d, Exp Len = \%d, Len = \%d' </tt>
**  \event <tt> 'Invalid cmd length: ID = 0x\%X, CC = \%d, Exp Len = \%d, Len = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when a message with the #CFE_SB_CMD_MID
**  message ID has arrived but whose packet length does not match the expected
**  length for the specified command code.
**
**  The \c ID field in the event message specifies the Message ID (in hex), the \c CC field
**  specifies the Command Code (in decimal), the \c Exp Len field specified the Expected
**  Length (in decimal ), and \c Len specifies the message Length (in decimal)
**  found in the message.
**/
#define CFE_SB_LEN_ERR_EID              61

/** \brief <tt> 'CreatePipeErr:Name Taken:app=\%s,ptr=0x\%x,depth=\%d,maxdepth=\%d' </tt>
**  \event <tt> 'CreatePipeErr:Name Taken:app=\%s,ptr=0x\%x,depth=\%d,maxdepth=\%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This error event message is issued when the #CFE_SB_CreatePipe API tries to create
**  a pipe with a name that is in use.
**/
#define CFE_SB_CR_PIPE_NAME_TAKEN_EID   62

/** \brief <tt> 'CreatePipeErr:No Free:app=\%s,ptr=0x\%x,depth=\%d,maxdepth=\%d' </tt>
**  \event <tt> 'CreatePipeErr:No Free:app=\%s,ptr=0x\%x,depth=\%d,maxdepth=\%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This error event message is issued when the #CFE_SB_CreatePipe API is unable to
**  create a queue because there are no queues free.
**/
#define CFE_SB_CR_PIPE_NO_FREE_EID      63


#endif /* _cfe_sb_events_ */

/************************/
/*  End of File Comment */
/************************/

