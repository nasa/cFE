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
 *  cFE Software Bus Services Event IDs
 */

#ifndef CFE_SB_EVENTS_H
#define CFE_SB_EVENTS_H

/**
 * \name SB event IDs
 */
/**\{*/

/**
 * \brief SB Initialization Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  Software Bus Services Task initialization complete.
 */
#define CFE_SB_INIT_EID 1

/**
 * \brief SB Create Pipe API Bad Argument Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  #CFE_SB_CreatePipe API failure due to a bad input argument.
 */
#define CFE_SB_CR_PIPE_BAD_ARG_EID 2

/**
 * \brief SB Create Pipe API Max Pipes Exceeded Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  #CFE_SB_CreatePipe API failure to do maximum number of pipes being exceeded.
 */
#define CFE_SB_MAX_PIPES_MET_EID 3

/**
 * \brief SB Create Pipe API Queue Create Failure Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  #CFE_SB_CreatePipe API failure creating the queue.
 */
#define CFE_SB_CR_PIPE_ERR_EID 4

/**
 * \brief SB Create Pipe API Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  #CFE_SB_CreatePipe API successfully completed.
 */
#define CFE_SB_PIPE_ADDED_EID 5

/**
 * \brief SB Subscribe API Bad Argument Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  An SB Subscribe API failed due to an invalid input argument.
 */
#define CFE_SB_SUB_ARG_ERR_EID 6

/**
 * \brief SB Subscribe API Duplicate MsgId Subscription Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  An SB Subscribe API was called with a Message ID that was already subscribed on the pipe
 *  on the pipe.
 */
#define CFE_SB_DUP_SUBSCRIP_EID 7

/**
 * \brief SB Subscribe API Max Subscriptions Exceeded Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  An SB Subscribe API was called on a pipe that already has the maximum allowed
 *  number of subscriptions.
 */
#define CFE_SB_MAX_MSGS_MET_EID 8

/**
 * \brief SB Subscribe API Max Destinations Exceeded Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  An SB Subscribe API was called with a message id that already has the
 *  maximum allowed number of destinations.
 */
#define CFE_SB_MAX_DESTS_MET_EID 9

/**
 * \brief SB Subscribe API Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  An SB Subscribe API completed successfully.
 */
#define CFE_SB_SUBSCRIPTION_RCVD_EID 10

/**
 * \brief SB Unsubscribe API Bad Argument Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  An SB Unsubscribe API failed due to an invalid input argument.
 */
#define CFE_SB_UNSUB_ARG_ERR_EID 11

/**
 * \brief SB Unsubscribe API No MsgId Subscription Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  An SB Unsubscribe API was called with a Message ID that wasn't subscribed on the pipe
 */
#define CFE_SB_UNSUB_NO_SUBS_EID 12

/**
 * \brief SB Transmit API Bad Argument Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  An SB Transmit API failed due to an invalid input argument.
 */
#define CFE_SB_SEND_BAD_ARG_EID 13

/**
 * \brief SB Transmit API No MsgId Subscribers Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  An SB Transmit API was called with a Message ID with no subscriptions.
 */
#define CFE_SB_SEND_NO_SUBS_EID 14

/**
 * \brief SB Transmit API Message Size Limit Exceeded Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  An SB Transmit API was called with a message that is too big.
 */
#define CFE_SB_MSG_TOO_BIG_EID 15

/**
 * \brief SB Transmit API Buffer Request Failure Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  An SB Transmit API call buffer request failed.
 */
#define CFE_SB_GET_BUF_ERR_EID 16

/**
 * \brief SB Transmit API MsgId Pipe Limit Exceeded Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  An SB Transmit API call failed to deliver the MsgId to a pipe due to the
 *  limit for the number of messages with that MsgId for that pipe being exceeded.
 */
#define CFE_SB_MSGID_LIM_ERR_EID 17

/**
 * \brief SB Receive Buffer API Bad Argument Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  #CFE_SB_ReceiveBuffer API failure due to a bad input argument.
 */
#define CFE_SB_RCV_BAD_ARG_EID 18

/**
 * \brief SB Receive Buffer API Invalid Pipe Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  #CFE_SB_ReceiveBuffer API failure due to an invalid Pipe ID.
 */
#define CFE_SB_BAD_PIPEID_EID 19

/**
 * \brief SB Subscribe API Get Destination Block Failure Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  An SB Subscribe API call failed to get a destination block.
 */
#define CFE_SB_DEST_BLK_ERR_EID 20

/**
 * \brief SB Transmit API Invalid MsgId Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  An SB Transmit API was called with an invalid message ID.
 */
#define CFE_SB_SEND_INV_MSGID_EID 21

/**
 * \brief SB Subscription Report Sent Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  SB Subscription Report sent in response to a successful subscription.
 */
#define CFE_SB_SUBSCRIPTION_RPT_EID 22

/**
 * \brief SB Subscribe API Message Table Hash Collision Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  An SB Subscribe API call caused a message table hash collision, which will impact
 *  message transmission performance. This can be resolved by deconflicting MsgId values
 *  or increasing #CFE_PLATFORM_SB_MAX_MSG_IDS.
 */
#define CFE_SB_HASHCOLLISION_EID 23

/**
 * \brief SB Transmit API Pipe Overflow Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  An SB Transmit API call failed to deliver the Message ID to a pipe due to the
 *  pipe queue being full.
 */
#define CFE_SB_Q_FULL_ERR_EID 25

/**
 * \brief SB Transmit API Queue Write Failure Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  An SB Transmit API call failed due to a pipe queue write failure.
 */
#define CFE_SB_Q_WR_ERR_EID 26

/**
 * \brief SB Transmit API Queue Read Failure Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  An SB Transmit API called failed due to a pipe queue read failure.
 */
#define CFE_SB_Q_RD_ERR_EID 27

/**
 * \brief SB No-op Command Success Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  \link #CFE_SB_NOOP_CC SB NO-OP Command \endlink success.
 */
#define CFE_SB_CMD0_RCVD_EID 28

/**
 * \brief SB Reset Counters Command Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  \link #CFE_SB_RESET_COUNTERS_CC SB Reset Counters Command \endlink success.
 */
#define CFE_SB_CMD1_RCVD_EID 29

/**
 * \brief SB Send Statistics Command Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  \link #CFE_SB_SEND_SB_STATS_CC SB Send Statistics Command \endlink success.
 */
#define CFE_SB_SND_STATS_EID 32

/**
 * \brief SB Enable Route Command Invalid MsgId/PipeID Pair Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_SB_ENABLE_ROUTE_CC SB Enable Route Command \endlink failure due
 *  to the Message ID not being subscribed to the pipe.
 */
#define CFE_SB_ENBL_RTE1_EID 33

/**
 * \brief SB Enable Route Command Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  \link #CFE_SB_ENABLE_ROUTE_CC SB Enable Route Command \endlink success.
 */
#define CFE_SB_ENBL_RTE2_EID 34

/**
 * \brief SB Enable Route Command Invalid MsgId or Pipe Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_SB_ENABLE_ROUTE_CC SB Enable Route Command \endlink failure due
 *  to an invalid MsgId or Pipe.
 */
#define CFE_SB_ENBL_RTE3_EID 35

/**
 * \brief SB Disable Route Command Invalid MsgId/PipeId Pair Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_SB_DISABLE_ROUTE_CC SB Disable Route Command \endlink failure due
 *  to the Message ID not being subscribed to the pipe.
 */
#define CFE_SB_DSBL_RTE1_EID 36

/**
 * \brief SB Disable Route Command Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  \link #CFE_SB_DISABLE_ROUTE_CC SB Disable Route Command \endlink success.
 */
#define CFE_SB_DSBL_RTE2_EID 37

/**
 * \brief SB Disable Route Command Invalid MsgId or Pipe Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_SB_DISABLE_ROUTE_CC SB Disable Route Command \endlink failure due
 *  to an invalid MsgId or Pipe.
 */
#define CFE_SB_DSBL_RTE3_EID 38

/**
 * \brief SB File Write Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  An SB file write successfully completed. OVERLOADED
 */
#define CFE_SB_SND_RTG_EID 39

/**
 * \brief SB File Write Create File Failure Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  An SB file write failure due to file creation error. OVERLOADED
 */
#define CFE_SB_SND_RTG_ERR1_EID 40

/**
 * \brief SB Invalid Command Code Received Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Invalid command code for message ID #CFE_SB_CMD_MID or #CFE_SB_SUB_RPT_CTRL_MID received
 *  on the SB message pipe. OVERLOADED
 */
#define CFE_SB_BAD_CMD_CODE_EID 42

/**
 * \brief SB Invalid Message ID Received Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Invalid message ID received on the SB message pipe.
 */
#define CFE_SB_BAD_MSGID_EID 43

/**
 * \brief SB Send Previous Subscriptions Command Full Packet Sent Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  \link #CFE_SB_SEND_PREV_SUBS_CC SB Send Previous Subscriptions Command \endlink processing
 *  sent a full subscription packet.
 */
#define CFE_SB_FULL_SUB_PKT_EID 44

/**
 * \brief SB Send Previous Subscriptions Command Partial Packet Sent Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  \link #CFE_SB_SEND_PREV_SUBS_CC SB Send Previous Subscriptions Command \endlink processing
 *  sent a partial subscription packet.
 */
#define CFE_SB_PART_SUB_PKT_EID 45

/**
 * \brief SB Pipe Delete API Bad Argument Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  An SB Delete Pipe API failed due to an invalid input argument.
 */
#define CFE_SB_DEL_PIPE_ERR1_EID 46

/**
 * \brief SB Pipe Delete API Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  An SB Delete Pipe API successfully completed.
 */
#define CFE_SB_PIPE_DELETED_EID 47

/**
 * \brief SB Unsubscribe API Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  An SB Unsubscribe API successfully completed.
 */
#define CFE_SB_SUBSCRIPTION_REMOVED_EID 48

/**
 * \brief SB File Write Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  An SB file write failure encountered when writing to the file.
 */
#define CFE_SB_FILEWRITE_ERR_EID 49

/**
 * \brief SB Subscribe API Invalid Pipe Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  An SB Subscribe API failed due to an invalid pipe ID.
 */
#define CFE_SB_SUB_INV_PIPE_EID 50

/**
 * \brief SB Subscribe API Not Owner Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  An SB Subscribe API failed due to not being the pipe owner.
 */
#define CFE_SB_SUB_INV_CALLER_EID 51

/**
 * \brief SB Unsubscribe API Invalid Pipe Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  An SB Unsubscribe API failed due to an invalid pipe ID.
 */
#define CFE_SB_UNSUB_INV_PIPE_EID 52

/**
 * \brief SB Unsubscribe API Not Owner Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  An SB Unsubscribe API failed due to not being the pipe owner.
 */
#define CFE_SB_UNSUB_INV_CALLER_EID 53

/**
 * \brief SB Delete Pipe API Not Owner Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 * An SB Delete Pipe API failed due to not being the pipe owner.
 */
#define CFE_SB_DEL_PIPE_ERR2_EID 54

/**
 * \brief SB Set Pipe Opts API Invalid Pipe Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  #CFE_SB_SetPipeOpts API failure due to an invalid pipe ID
 */
#define CFE_SB_SETPIPEOPTS_ID_ERR_EID 55

/**
 * \brief SB Set Pipe Opts API Not Owner Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  #CFE_SB_SetPipeOpts API failure due to not being the pipe owner.
 */
#define CFE_SB_SETPIPEOPTS_OWNER_ERR_EID 56

/**
 * \brief SB Set Pipe Opts API Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  #CFE_SB_SetPipeOpts success.
 */
#define CFE_SB_SETPIPEOPTS_EID 57

/**
 * \brief SB Get Pipe Opts API Invalid Pipe Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  #CFE_SB_GetPipeOpts failure due to invalid pipe ID.
 */
#define CFE_SB_GETPIPEOPTS_ID_ERR_EID 58

/**
 * \brief SB Get Pipe Opts API Invalid Pointer Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  #CFE_SB_GetPipeOpts failure due to invalid pointer.
 */
#define CFE_SB_GETPIPEOPTS_PTR_ERR_EID 59

/**
 * \brief SB Get Pipe Opts API Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  #CFE_SB_GetPipeOpts success.
 */
#define CFE_SB_GETPIPEOPTS_EID 60

/**
 * \brief SB Get Pipe Name API Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  #CFE_SB_GetPipeName success.
 */
#define CFE_SB_GETPIPENAME_EID 62

/**
 * \brief SB Get Pipe Name API Invalid Pointer Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  #CFE_SB_GetPipeName failure due to invalid pointer.
 */
#define CFE_SB_GETPIPENAME_NULL_PTR_EID 63

/**
 * \brief SB Get Pipe Name API Invalid Pipe or Resource Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  #CFE_SB_GetPipeName failure due to invalid pipe ID or failure in retrieving resource name. OVERLOADED
 */
#define CFE_SB_GETPIPENAME_ID_ERR_EID 64

/**
 * \brief SB Get Pipe ID By Name API Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  #CFE_SB_GetPipeIdByName success.
 */
#define CFE_SB_GETPIPEIDBYNAME_EID 65

/**
 * \brief SB Get Pipe ID By Name API Invalid Pointer Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  #CFE_SB_GetPipeIdByName failure due to invalid pointer.
 */
#define CFE_SB_GETPIPEIDBYNAME_NULL_ERR_EID 66

/**
 * \brief SB Get Pipe ID By Name API Name Not Found Or ID Not Matched Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  #CFE_SB_GetPipeIdByName failure due to name not found or ID mismatch. OVERLOADED
 */
#define CFE_SB_GETPIPEIDBYNAME_NAME_ERR_EID 67

/**
 * \brief SB Invalid Command Length Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Invalid length for the command code in message ID #CFE_SB_CMD_MID or #CFE_SB_SUB_RPT_CTRL_MID
 *  received on the SB message pipe.
 */
#define CFE_SB_LEN_ERR_EID 68

/**
 * \brief SB Create Pipe API Name Taken Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  #CFE_SB_CreatePipe API failure due to pipe name taken.
 */
#define CFE_SB_CR_PIPE_NAME_TAKEN_EID 69

/**
 * \brief SB Create Pipe API Queues Exhausted Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  #CFE_SB_CreatePipe API failure due to no free queues.
 */
#define CFE_SB_CR_PIPE_NO_FREE_EID 70

/**
 * \brief SB integrity actions on transmit message failure event
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  A CFE SB transmit transaction has rejected a message due
 *  to failure of the associated message integrity action(s).
 */
#define CFE_SB_SEND_MESSAGE_INTEGRITY_FAIL_EID 71

/**
 * \brief SB validation of received message failure event
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  A CFE SB receive transaction has rejected a message due
 *  to failure of the associated message integrity action(s).
 */
#define CFE_SB_RCV_MESSAGE_INTEGRITY_FAIL_EID 72

/**\}*/

#endif /* CFE_SB_EVENTS_H */
