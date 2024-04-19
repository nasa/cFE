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
 * Purpose:
 *      This header file contains prototypes for private functions and type
 *      definitions for SB internal use.
 *
 * Author:   R.McGraw/SSI
 *
 */

#ifndef CFE_SB_PRIV_H
#define CFE_SB_PRIV_H

/*
** Includes
*/
#include "cfe_platform_cfg.h"
#include "common_types.h"
#include "cfe_sb_api_typedefs.h"
#include "cfe_es_api_typedefs.h"
#include "cfe_sbr_api_typedefs.h"
#include "cfe_msg_api_typedefs.h"
#include "cfe_fs_api_typedefs.h"
#include "cfe_resourceid_api_typedefs.h"
#include "cfe_sb_destination_typedef.h"
#include "cfe_sb_msg.h"

/*
** Macro Definitions
*/

#define CFE_SB_UNUSED_QUEUE   OS_OBJECT_ID_UNDEFINED
#define CFE_SB_NO_DESTINATION 0xFF
#define CFE_SB_FAILED         1
#define SB_DONT_CARE          0

#define CFE_SB_NO_DUPLICATE 0
#define CFE_SB_DUPLICATE    1

#define CFE_SB_INACTIVE 0
#define CFE_SB_ACTIVE   1

#define CFE_SB_MSG_GLOBAL 0
#define CFE_SB_MSG_LOCAL  1

#define CFE_SB_SEND_ZEROCOPY 0
#define CFE_SB_SEND_ONECOPY  1

#define CFE_SB_NOT_IN_USE 0
#define CFE_SB_IN_USE     1

#define CFE_SB_DISABLE 0
#define CFE_SB_ENABLE  1

#define CFE_SB_DENIED  0
#define CFE_SB_GRANTED 1

#define CFE_SB_DO_NOT_INCREMENT 0
#define CFE_SB_INCREMENT_TLM    1

#define CFE_SB_MAIN_LOOP_ERR_DLY             1000
#define CFE_SB_CMD_PIPE_DEPTH                32
#define CFE_SB_CMD_PIPE_NAME                 "SB_CMD_PIPE"
#define CFE_SB_MAX_CFG_FILE_EVENTS_TO_FILTER 8

#define CFE_SB_PIPE_OVERFLOW (-1)
#define CFE_SB_PIPE_WR_ERR   (-2)
#define CFE_SB_USECNT_ERR    (-3)
#define CFE_SB_FILE_IO_ERR   (-5)

/* bit map for stopping recursive event problem */
#define CFE_SB_SEND_NO_SUBS_EID_BIT   0
#define CFE_SB_GET_BUF_ERR_EID_BIT    1
#define CFE_SB_MSGID_LIM_ERR_EID_BIT  2
#define CFE_SB_Q_FULL_ERR_EID_BIT     3
#define CFE_SB_Q_WR_ERR_EID_BIT       4
#define CFE_SB_SEND_BAD_ARG_EID_BIT   5
#define CFE_SB_SEND_INV_MSGID_EID_BIT 6
#define CFE_SB_MSG_TOO_BIG_EID_BIT    7
/*
** Type Definitions
*/

/**
 * \brief Basic linked list structure allowing all buffer descriptors to be tracked.
 */
typedef struct CFE_SB_BufferLink
{
    struct CFE_SB_BufferLink *Next;
    struct CFE_SB_BufferLink *Prev;
} CFE_SB_BufferLink_t;

/******************************************************************************
**  Typedef:  CFE_SB_BufferD_t
**
**  Purpose:
**     This structure defines a BUFFER DESCRIPTOR used to specify the MsgId
**     and address of each packet buffer.
**
**     Note: Changing the size of this structure may require the memory pool
**     block sizes to change.
*/
typedef struct CFE_SB_BufferD
{
    CFE_SB_BufferLink_t Link; /**< Links for inclusion in the tracking lists */

    /**
     * MsgId that should be used for routing/delivering the message.  Traditionally,
     * this is the same as the MsgId inside the content, but cached for easier access.
     * However, the user may also choose to route messages explicitly and thus this
     * message ID could differ from the one inside the message content.
     *
     * MsgId is set for buffers in transit.  AppId is unset while in transit, as it
     * may be sent to multiple apps.
     *
     * During zero copy buffer initial allocation, the MsgId is not known at this time
     * and should be set to the invalid msg ID.
     */
    CFE_SBR_RouteId_t DestRouteId;

    /**
     * Current owner of the buffer, if owned by a single app.
     *
     * This is used to track "zero copy" buffer allocations - this will be set to
     * the AppID that initially allocated it, before it is used to transmit a message.
     *
     * When the message is in transit, it may be queued to multiple applications,
     * so this is unset.
     */
    CFE_ES_AppId_t AppId;

    CFE_SB_MsgId_t MsgId;

    size_t AllocatedSize; /**< Total size of this descriptor (including descriptor itself) */
    size_t ContentSize;   /**< Actual size of message content currently stored in the buffer */

    uint16 UseCount; /**< Number of active references to this buffer in the system */

    CFE_SB_Buffer_t Content; /* Variably sized content field, Keep last */
} CFE_SB_BufferD_t;

/******************************************************************************
**  Typedef:  CFE_SB_PipeD_t
**
**  Purpose:
**     This structure defines a pipe descriptor used to specify the
**     characteristics and status of a pipe.
*/

typedef struct
{
    CFE_SB_PipeId_t   PipeId;
    CFE_ES_AppId_t    AppId;
    osal_id_t         SysQueueId;
    uint8             Opts;
    uint8             Spare;
    uint16            SendErrors;
    uint16            MaxQueueDepth;
    uint16            CurrentQueueDepth;
    uint16            PeakQueueDepth;
    CFE_SB_BufferD_t *LastBuffer;
} CFE_SB_PipeD_t;

/******************************************************************************
**  Typedef:  CFE_SB_BufParams_t
**
**  Purpose:
**     This structure defines the variables related to the SB routing buffers.
*/
typedef struct
{
    CFE_ES_MemHandle_t PoolHdl;
    CFE_ES_STATIC_POOL_TYPE(CFE_PLATFORM_SB_BUF_MEMORY_BYTES) Partition;
} CFE_SB_MemParams_t;

/*******************************************************************************/
/**
** \brief SB route info temporary structure
**
** This tracks the number of destinations along with destination data for 1 route.
** Each route may contain zero or more destinations (variable length).
*/
typedef struct
{
    uint32                    NumDestinations;
    CFE_SB_RoutingFileEntry_t DestEntries[CFE_PLATFORM_SB_MAX_DEST_PER_PKT]; /**< Actual data written to file */
} CFE_SB_BackgroundRouteInfoBuffer_t;

/**
 * \brief Temporary holding buffer for records being written to a file.
 *
 * This is shared/reused between all file types (msg map, route info, pipe info).
 */
typedef union
{
    CFE_SB_BackgroundRouteInfoBuffer_t RouteInfo;
    CFE_SB_PipeInfoEntry_t             PipeInfo;
    CFE_SB_MsgMapFileEntry_t           MsgMapInfo;
} CFE_SB_BackgroundFileBuffer_t;

/**
 * \brief SB Background file write state information
 *
 * Must be stored in persistent memory (e.g. global).
 */
typedef struct
{
    CFE_FS_FileWriteMetaData_t    FileWrite; /**< FS state data - must be first */
    CFE_SB_BackgroundFileBuffer_t Buffer;    /**< Temporary holding area for file record */
} CFE_SB_BackgroundFileStateInfo_t;

/******************************************************************************
**  Typedef:  CFE_SB_Global_t
**
**  Purpose:
**     This structure contains the SB global variables.
*/
typedef struct
{
    osal_id_t                    SharedDataMutexId;
    uint32                       SubscriptionReporting;
    CFE_ES_AppId_t               AppId;
    uint32                       StopRecurseFlags[OS_MAX_TASKS];
    CFE_SB_PipeD_t               PipeTbl[CFE_PLATFORM_SB_MAX_PIPES];
    CFE_SB_HousekeepingTlm_t     HKTlmMsg;
    CFE_SB_StatsTlm_t            StatTlmMsg;
    CFE_SB_PipeId_t              CmdPipe;
    CFE_SB_MemParams_t           Mem;
    CFE_SB_AllSubscriptionsTlm_t PrevSubMsg;
    CFE_EVS_BinFilter_t          EventFilters[CFE_SB_MAX_CFG_FILE_EVENTS_TO_FILTER];
    CFE_SB_Qos_t                 Default_Qos;
    CFE_ResourceId_t             LastPipeId;

    CFE_SB_BackgroundFileStateInfo_t BackgroundFile;

    /* A list of buffers currently in-transit, owned by SB */
    CFE_SB_BufferLink_t InTransitList;

    /* A list of buffers currently issued to apps for zero-copy */
    CFE_SB_BufferLink_t ZeroCopyList;
} CFE_SB_Global_t;

/******************************************************************************
 *  Typedef:  CFE_SB_PipeSetEntry_t
 */
typedef struct
{
    CFE_SB_PipeId_t PipeId;
    osal_id_t       SysQueueId;
    uint16          PendingEventId;
    int32           OsStatus;
} CFE_SB_PipeSetEntry_t;

typedef enum
{
    CFE_SB_MessageTxn_TimeoutMode_POLL, /**< Transaction should not block */
    CFE_SB_MessageTxn_TimeoutMode_PEND, /**< Transaction should block indefinitely */
    CFE_SB_MessageTxn_TimeoutMode_TIMED /**< Transaction should block for a limited amount of time */
} CFE_SB_MessageTxn_TimeoutMode_t;

/**
 * \brief Tracks the status of a message/buffer transmit transaction
 *
 * This includes the list of destinations to send to, and the status of each,
 * as well as any pending event IDs from the transaction setup.
 *
 */
typedef struct
{
    bool IsTransmit;
    bool IsEndpoint;

    uint16 NumPipes;
    uint16 MaxPipes;
    uint16 NumPipeErrs;
    uint16 TransactionEventId;

    CFE_SB_MessageTxn_TimeoutMode_t TimeoutMode;
    int32_t                         UserTimeoutParam;
    OS_time_t                       AbsTimeout;

    CFE_Status_t   Status;
    size_t         ContentSize;
    CFE_SB_MsgId_t RoutingMsgId;

    const void *RefMemPtr;

    CFE_SB_PipeSetEntry_t *PipeSet;
} CFE_SB_MessageTxn_State_t;

/**
 * \brief Tracks the status of a message/buffer transmit transaction
 *
 * This includes the list of destinations to send to, and the status of each,
 * as well as any pending event IDs from the transaction setup.
 *
 */
typedef struct
{
    CFE_SB_MessageTxn_State_t MessageTxn_State;

    CFE_SB_PipeSetEntry_t DestSet[CFE_PLATFORM_SB_MAX_DEST_PER_PKT];
} CFE_SB_TransmitTxn_State_t;

/**
 * \brief Tracks the status of a message/buffer transmit transaction
 *
 * This includes the list of destinations to send to, and the status of each,
 * as well as any pending event IDs from the transaction setup.
 *
 */
typedef struct
{
    CFE_SB_MessageTxn_State_t MessageTxn_State;

    CFE_SB_PipeSetEntry_t Source;
} CFE_SB_ReceiveTxn_State_t;

typedef bool (*CFE_SB_MessageTxn_PipeHandler_t)(CFE_SB_MessageTxn_State_t *, CFE_SB_PipeSetEntry_t *, void *);

/*
** Software Bus Function Prototypes
*/

/*---------------------------------------------------------------------------------------*/
/**
 * Initialization routine for SB application.
 *
 * This routine is executed when  * the SB application is started by Executive Services.
 *
 * \return Execution status, see \ref CFEReturnCodes
 */
int32 CFE_SB_AppInit(void);

/*---------------------------------------------------------------------------------------*/
/**
 * Initialize the Software Bus Buffer Pool.
 *
 * \return Execution status, see \ref CFEReturnCodes
 */
int32 CFE_SB_InitBuffers(void);

/*---------------------------------------------------------------------------------------*/
/**
 * Initialize the Software Bus Pipe Table.
 *
 * @note This function MUST be called before any SB API's are called.
 */
void CFE_SB_InitPipeTbl(void);

/*---------------------------------------------------------------------------------------*/
/**
 * SB internal function to obtain exclusive access to SB global data structures
 *
 * @param FuncName    the function name containing the code
 * @param LineNumber  the line number of the calling code
 */
void CFE_SB_LockSharedData(const char *FuncName, int32 LineNumber);

/*---------------------------------------------------------------------------------------*/
/**
 * SB internal function to release exclusive access to SB global data structures
 *
 * @param FuncName    the function name containing the code
 * @param LineNumber  the line number of the calling code
 */
void CFE_SB_UnlockSharedData(const char *FuncName, int32 LineNumber);

/*---------------------------------------------------------------------------------------*/
/**
 * Function to reset the SB housekeeping counters.
 * @note Command counter not incremented for this command
 */
void CFE_SB_ResetCounters(void);

/*---------------------------------------------------------------------------------------*/
/**
 * This function returns a pointer to the app.tsk name string
 *
 * @note With taskId, Parent App name and Child Task name can be queried from ES
 *
 * @param TaskId  the task id of the app.task name desired
 * @param FullName  string buffer to store name
 * @return Pointer to App.Tsk Name
 */
char *CFE_SB_GetAppTskName(CFE_ES_TaskId_t TaskId, char *FullName);

/*---------------------------------------------------------------------------------------*/
/**
 * Deletes a pipe from SB owned by a specific app
 *
 * @param PipeId  The ID of the pipe to delete.
 * @param AppId   The application that owns the pipe
 * \return Execution status, see \ref CFEReturnCodes
 */
int32 CFE_SB_DeletePipeWithAppId(CFE_SB_PipeId_t PipeId, CFE_ES_AppId_t AppId);

/*---------------------------------------------------------------------------------------*/
/**
 * @copydoc CFE_SB_DeletePipeWithAppId
 *
 * Internal implementation of the pipe delete operation
 *
 * \return Execution status, see \ref CFEReturnCodes
 */
int32 CFE_SB_DeletePipeFull(CFE_SB_PipeId_t PipeId, CFE_ES_AppId_t AppId);

/*---------------------------------------------------------------------------------------*/
/**
 * CFE Internal API used to subscribe to a message
 *
 * This internal API exposes all subscription choices/parameters.  This function is
 * called by CFE_SB_SubscribeEx, CFE_SB_Subscribe and CFE_SB_SubscribeLocal.
 *
 * @param MsgId    Mission unique identifier for the message being requested
 * @param PipeId   The Pipe ID to send the message to
 * @param Quality  Quality of Service (Qos)  priority and reliability
 * @param MsgLim   Max number of messages, with this MsgId, allowed on the
 *                 pipe at any time.
 * @param Scope    Local subscription or broadcasted to peers
 *
 * \return Execution status, see \ref CFEReturnCodes
 */
int32 CFE_SB_SubscribeFull(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId, CFE_SB_Qos_t Quality, uint16 MsgLim,
                           uint8 Scope);

/*---------------------------------------------------------------------------------------*/
/**
 * Unsubscribe a Message ID from a pipe
 *
 * This internal API can be used to force unsubscribe when the calling context
 * is not the owner of the pipe.
 *
 * \return Execution status, see \ref CFEReturnCodes
 */
int32 CFE_SB_UnsubscribeWithAppId(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId, CFE_ES_AppId_t AppId);

/*---------------------------------------------------------------------------------------*/
/**
 * CFE Internal API used to unsubscribe to a message.
 *
 * This internal API exposes all available unsubscribe choices/parameters.
 *
 * \return Execution status, see \ref CFEReturnCodes
 */
int32 CFE_SB_UnsubscribeFull(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId, uint8 Scope, CFE_ES_AppId_t AppId);

/*---------------------------------------------------------------------------------------*/
/**
 * Release all zero-copy buffers associated with the given app ID.
 *
 * API used for releasing all pointers to a buffers (for zero copy mode
 * only) for a specific Application. This function is used for cleaning
 * up when an application crashes.
 *
 * @param AppId  Application ID to clean up
 *
 * \return Execution status, see \ref CFEReturnCodes
 */
int32 CFE_SB_ZeroCopyReleaseAppId(CFE_ES_AppId_t AppId);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Increment the UseCount of a buffer
 *
 * UseCount is a variable in the CFE_SB_BufferD_t and is used to
 * determine when a buffer may be returned to the memory pool.
 *
 * @note This must only be invoked while holding the SB global lock
 *
 * @param bd  Pointer to the buffer descriptor.
 */
void CFE_SB_IncrBufUseCnt(CFE_SB_BufferD_t *bd);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Decrement the UseCount of a buffer
 *
 * UseCount is a variable in the CFE_SB_BufferD_t and is used to
 * determine when a buffer may be returned to the memory pool.
 *
 * If the UseCount is decremented to zero, it will return the buffer to
 * the memory pool.
 *
 * @note This must only be invoked while holding the SB global lock
 *
 * @param bd  Pointer to the buffer descriptor.
 */
void CFE_SB_DecrBufUseCnt(CFE_SB_BufferD_t *bd);

/*---------------------------------------------------------------------------------------*/
/**
 * SB internal function to validate a given MsgId.
 * \return Execution status, see \ref CFEReturnCodes
 */
int32 CFE_SB_ValidateMsgId(CFE_SB_MsgId_t MsgId);

/*---------------------------------------------------------------------------------------*/
/**
 * Increment the command counter based on the status input.
 *
 * This small utility was written to eliminate duplicate code.
 *
 * @param status  typically #CFE_SUCCESS or an SB error code
 */
void CFE_SB_IncrCmdCtr(int32 status);

/*---------------------------------------------------------------------------------------*/
/**
 * SB internal function to enable and disable subscription reporting.
 */
void CFE_SB_SetSubscriptionReporting(uint32 state);

/*---------------------------------------------------------------------------------------*/
/**
 * SB internal function to generate the "ONESUB_TLM" message after a subscription.
 *
 * Arguments reflect the Payload of notification message - MsgId, PipeId, QOS
 *
 * @note this is a no-op when subscription reporting is disabled.
 *
 * \return Execution status, see \ref CFEReturnCodes
 */
int32 CFE_SB_SendSubscriptionReport(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId, CFE_SB_Qos_t Quality);

/*---------------------------------------------------------------------------------------*/
/**
 * This function will test the given bit for the given task.
 *
 * This prevents recursive events from occurring.
 *
 * If the bit is set this function will return #CFE_SB_DENIED. If bit is not set, this
 * function will set  the bit and return #CFE_SB_GRANTED.
 *
 * @returns grant/deny status
 */
uint32 CFE_SB_RequestToSendEvent(CFE_ES_TaskId_t TaskId, int32 Bit);

/*---------------------------------------------------------------------------------------*/
/**
 * This function will clear the given bit for the given task.
 *
 * This should be called after a successful CFE_SB_RequestToSendEvent()
 */
void CFE_SB_FinishSendEvent(CFE_ES_TaskId_t TaskId, int32 Bit);

/*---------------------------------------------------------------------------------------*/
/**
 * This function gets a destination descriptor from the SB memory pool.
 *
 * @note This must only be invoked while holding the SB global lock
 * @return Pointer to the destination descriptor
 */
CFE_SB_DestinationD_t *CFE_SB_GetDestinationBlk(void);

/*---------------------------------------------------------------------------------------*/
/**
 * This function returns a destination descriptor to the SB memory pool.
 * @note This must only be invoked while holding the SB global lock
 *
 * @param Dest Pointer to the destination descriptor
 *
 * \return Execution status, see \ref CFEReturnCodes
 */
int32 CFE_SB_PutDestinationBlk(CFE_SB_DestinationD_t *Dest);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief For SB buffer tracking, get first/next position in a list
 */
static inline CFE_SB_BufferLink_t *CFE_SB_TrackingListGetNext(CFE_SB_BufferLink_t *Node)
{
    return Node->Next;
}

/*---------------------------------------------------------------------------------------*/
/**
 * \brief For SB buffer tracking, checks if this current position represents the end of the list
 */
static inline bool CFE_SB_TrackingListIsEnd(const CFE_SB_BufferLink_t *List, const CFE_SB_BufferLink_t *Node)
{
    /* Normally list nodes should never have NULL, buf if they do, do not follow it */
    return (Node == NULL || Node == List);
}

/*---------------------------------------------------------------------------------------*/
/**
 * \brief For SB buffer tracking, reset link state to default
 *
 * This turns the node into a singleton/lone object (not in a list)
 * or resets the head link to be empty.
 */
void CFE_SB_TrackingListReset(CFE_SB_BufferLink_t *Link);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief For SB buffer tracking, removes a node from a tracking list
 *
 * Extracts a single node from whatever list it is in.  After this the
 * node becomes a singleton owned by the caller.  It may be put into
 * another list or freed.
 */
void CFE_SB_TrackingListRemove(CFE_SB_BufferLink_t *Node);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief For SB buffer tracking, adds a node to a tracking list
 *
 * Extracts a single node from the list its in.  After this the
 * node becomes a singleton owned by the caller.  It must put it
 * in another list or free it.
 */
void CFE_SB_TrackingListAdd(CFE_SB_BufferLink_t *List, CFE_SB_BufferLink_t *Node);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Allocates a new buffer descriptor from the SB memory pool.
 *
 * Requests a buffer from the SB buffer pool. The SB buffer pool is a
 * preallocated block of memory of size #CFE_PLATFORM_SB_BUF_MEMORY_BYTES. It is used
 * by the SB to dynamically allocate memory to hold the message and a buffer
 * descriptor associated with the message during the sending of a message.
 *
 * @note This must only be invoked while holding the SB global lock
 *
 * \param[in] MaxMsgSize Maximum message content size that the buffer must be capable of holding
 * \returns Pointer to buffer descriptor, or NULL on failure.
 */
CFE_SB_BufferD_t *CFE_SB_GetBufferFromPool(size_t MaxMsgSize);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Returns a buffer to SB memory pool
 *
 * This function will return a block of memory back to the SB memory pool,
 * so it can be re-used for a future message
 *
 * @note This must only be invoked while holding the SB global lock
 * \param[in] bd Pointer to descriptor to return
 */
void CFE_SB_ReturnBufferToPool(CFE_SB_BufferD_t *bd);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Perform basic sanity check on the Zero Copy handle
 *
 * \param[in]  BufPtr pointer to the content buffer
 * \param[out] BufDscPtr Will be set to actual buffer descriptor
 *
 * \returns CFE_SUCCESS if validation passed, or error code.
 */
int32 CFE_SB_ZeroCopyBufferValidate(CFE_SB_Buffer_t *BufPtr, CFE_SB_BufferD_t **BufDscPtr);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Add a destination node
 *
 * Private function that will add a destination node to the linked list
 *
 * \note Assumes destination pointer is valid
 *
 * \param[in] RouteId The route ID to add destination node to
 * \param[in] NewNode Pointer to the destination to add
 */
int32 CFE_SB_AddDestNode(CFE_SBR_RouteId_t RouteId, CFE_SB_DestinationD_t *NewNode);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Remove a destination node
 *
 * Private function that will remove a destination node from the linked list
 *
 * \note Assumes destination pointer is valid and in route
 *
 * \param[in] RouteId      The route ID to remove destination node from
 * \param[in] NodeToRemove Pointer to the destination to remove
 */
void CFE_SB_RemoveDestNode(CFE_SBR_RouteId_t RouteId, CFE_SB_DestinationD_t *NodeToRemove);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Remove a destination
 *
 * Private function that will remove a destination by removing the node,
 * returning the block, and decrementing counters
 *
 * \note Assumes destination pointer is valid and in route
 *
 * \param[in] RouteId The route ID to remove destination from
 * \param[in] DestPtr Pointer to the destination to remove
 */
void CFE_SB_RemoveDest(CFE_SBR_RouteId_t RouteId, CFE_SB_DestinationD_t *DestPtr);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Get destination pointer for PipeId from RouteId
 *
 * Private function that will return the destination pointer related to the
 * given PipeId and RouteId if it exists
 *
 * \param[in] RouteId The route ID to search
 * \param[in] PipeId  The pipe ID to search for
 *
 * \returns Then destination pointer for a match, NULL otherwise
 */
CFE_SB_DestinationD_t *CFE_SB_GetDestPtr(CFE_SBR_RouteId_t RouteId, CFE_SB_PipeId_t PipeId);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Get the size of a message header.
**
** \par Description
**          This routine is a best guess of the message header size based off type
**          information and the local message implementation.
**          If a different header implementation was used to generate the message
**          the returned size may not be correct.  Critical functionality should
**          use the real message structure or otherwise confirm header implementation
**          matches expectations prior to using this API.
**
** \par Assumptions, External Events, and Notes:
**          - Utilize CFE_MSG_CommandHeader_t and CFE_MSG_TelemetryHeader_t for
**            defining message structures.
**
** \param[in]  *MsgPtr The message ID to calculate header size for.  The size of the message
**                     header may depend on the MsgId in some implementations.  For example,
**                     if SB messages are implemented as CCSDS packets, the size of the header
**                     is different for command vs. telemetry packets.
**
** \returns Estimated number of bytes in the message header for the given message.
** \retval 0 if an error occurs, such as if the MsgPtr argument is not valid or header type cannot be identified.
**/
size_t CFE_SB_MsgHdrSize(const CFE_MSG_Message_t *MsgPtr);

/*
 * Message Transmit/Receive Transaction implementation functions
 */

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Set up transaction initial state
 *
 * Helper function to initialize the common parts of a transmit/receive transaction
 *
 * \param[out]   TxnPtr     Transaction object to initialize
 * \param[inout] PipeSet    Pointer to pipe entry buffer
 * \param[in]    MaxPipes   Number of entries in PipeSet
 * \param[in]    RefMemPtr  User-supplied Opaque object pointer for event logging
 */
void CFE_SB_MessageTxn_Init(CFE_SB_MessageTxn_State_t *TxnPtr, CFE_SB_PipeSetEntry_t *PipeSet, uint16 MaxPipes,
                            const void *RefMemPtr);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Check transaction current state
 *
 * Checks if the transaction is in a good state.
 *
 * \param[in] TxnPtr    Transaction object to check
 * \returns   Boolean indicating transaction condition
 * \retval    true if the transaction is OK
 * \retval    false if the transaction has an error
 */
static inline bool CFE_SB_MessageTxn_IsOK(const CFE_SB_MessageTxn_State_t *TxnPtr)
{
    return (TxnPtr->Status == CFE_SUCCESS && TxnPtr->TransactionEventId == 0);
}

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Get transaction content size
 *
 * Obtains the size of the object/message being passed in this transaction
 *
 * \param[in]   TxnPtr     Transaction object
 * \returns Size of message/object being passed
 */
static inline size_t CFE_SB_MessageTxn_GetContentSize(const CFE_SB_MessageTxn_State_t *TxnPtr)
{
    return TxnPtr->ContentSize;
}

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Get transaction MsgId used for routing
 *
 * Obtains the MsgId used for routing of the object/message being passed in this transaction
 *
 * \note This MsgId may be different than the one embedded within the message
 *
 * \param[in]   TxnPtr     Transaction object
 * \returns MsgId used for routing of message/object being passed
 */
static inline CFE_SB_MsgId_t CFE_SB_MessageTxn_GetRoutingMsgId(CFE_SB_MessageTxn_State_t *TxnPtr)
{
    return TxnPtr->RoutingMsgId;
}

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Get transaction status code
 *
 * Obtains the CFE status code associated with the transaction
 *
 * \param[in]   TxnPtr     Transaction object
 * \returns CFE Status code
 */
static inline CFE_Status_t CFE_SB_MessageTxn_GetStatus(CFE_SB_MessageTxn_State_t *TxnPtr)
{
    return TxnPtr->Status;
}

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Set transaction status code and event
 *
 * If an off-nominal condition occurs, this routine sets the specified status and/or event code
 * that should be returned to the caller to indicate that condition.
 *
 * A value of 0 (which is an alias to #CFE_SUCCESS for status codes) can be passed for either
 * parameter if there is no defined code for the particular condition.
 *
 * The transaction object stores the first non-zero value for event ID or status code.
 *
 * \param[inout]   TxnPtr     Transaction object
 * \param[in]      EventId    The event ID to set
 * \param[in]      Status     The status code to set
 */
void CFE_SB_MessageTxn_SetEventAndStatus(CFE_SB_MessageTxn_State_t *TxnPtr, uint16 EventId, CFE_Status_t Status);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Set transaction MsgId used for routing
 *
 * Configures the MsgId used for routing of the object/message being passed in this transaction
 *
 * \note This MsgId may be different than the one embedded within the message
 *
 * \param[inout]  TxnPtr         Transaction object
 * \param[in]     RoutingMsgId   MsgId used for routing of message/object being passed
 */
void CFE_SB_MessageTxn_SetRoutingMsgId(CFE_SB_MessageTxn_State_t *TxnPtr, CFE_SB_MsgId_t RoutingMsgId);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Set transaction content size
 *
 * Configures the size of the object/message being passed in this transaction
 *
 * \param[inout] TxnPtr        Transaction object
 * \param[in]    ContentSize   Size of message/object being passed
 */
void CFE_SB_MessageTxn_SetContentSize(CFE_SB_MessageTxn_State_t *TxnPtr, size_t ContentSize);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Configure transaction time out
 *
 * Configures the relative timeout (in milliseconds) for this transaction.  This is based
 * on sampling the PSP clock at the time this function is invoked, and adding the relative
 * time to it.
 *
 * This should be called early in the transaction configuration process.
 *
 * \param[inout]  TxnPtr   Transaction object
 * \param[in]     Timeout  Timeout for message/object transaction
 */
void CFE_SB_MessageTxn_SetTimeout(CFE_SB_MessageTxn_State_t *TxnPtr, int32 Timeout);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Configure message endpoint (origination/termination) processing
 *
 * For a transmit transaction this enables origination processing.  If configured as
 * true, then the CFE_MSG_OriginationAction() function is invoked prior to sending the message.
 * The specific action(s) depend on the MSG module implementation, and may include:
 *   - Updating the sequence number
 *   - Updating the message timestamp
 *   - Calculating/Appending any applicable CRC, checksum, or other error control field
 *   - Any other system-specific message header updates
 *
 * For a receive transaction, this enables message verification.  If configured as
 * true, then the CFE_MSG_VerificationAction() function will be invoked before the message buffer
 * is returned to the caller.  The verification criteria depends on the MSG implementation, but
 * is typically expected to be the inverse of the origination processing.  That is, items that
 * were calculated or updated as part of the origination process can be verified during this step.
 * If verification fails, then an event will be logged, the message will be dropped, and the
 * implementation will get the next message in the queue (if any).
 *
 * \note The receive verification function in the default MSG implementation always returns
 * success, thus should be backward compatible with previous CFE versions that did not perform
 * any verification at the SB level, leaving it up to the individual app to check.  To make use
 * of this feature, the implementer must also provide an alternate implemenation of
 * CFE_MSG_VerificationAction() that performs the desired checks for their particular use-case.
 *
 * \sa CFE_MSG_OriginationAction()
 * \sa CFE_MSG_VerificationAction()
 *
 * \param[inout] TxnPtr      Transaction object
 * \param[in]    IsEndpoint  Pass as "true" to enable endpoint (origination/termination) message processing
 */
void CFE_SB_MessageTxn_SetEndpoint(CFE_SB_MessageTxn_State_t *TxnPtr, bool IsEndpoint);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Helper function to get the OSAL timeout to use
 *
 * Returns the relative timeout (in milliseconds) for any pending queue operation(s).
 * This reflects the amount of time left from the initial call to CFE_SB_MessageTxn_SetTimeout().
 *
 * If the original timeout was to PEND, this always returns -1 (OS_PEND).
 * If the original timeout was to POLL, this always returns 0 (OS_CHECK).
 * If the original timeout was some nonzero value, this returns the time left, in milliseconds,
 * based on the current time as obtained from the PSP.
 *
 * \param[in]   TxnPtr   Transaction object
 * \returns Timeout to use for OSAL routines that may block
 */
int32 CFE_SB_MessageTxn_GetOsTimeout(const CFE_SB_MessageTxn_State_t *TxnPtr);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Facilitates reporting of a single event related to a transmit transaction
 *
 * Internal routine that assembles the following information about how an event should
 * be reported:
 *
 *  - Complete message text (human-readable)
 *  - The type of event (information, error, etc)
 *  - The loop-detection bit to use (request to send/clear to send)
 *
 * \param[inout] TxnPtr     Transaction object
 * \param[in]    TskId      Calling task identifier
 * \param[in]    EventId    Event to report
 * \param[out]   EvtMsg     Message buffer
 * \param[in]    EvtMsgSize Size of EvtMsg buffer
 * \param[in]    ContextPtr Context information (may be NULL for general transaction events)
 * \param[out]   EventType  Buffer to store event type
 * \param[out]   ReqBit     Buffer to store request/loop detect bit
 */
void CFE_SB_MessageTxn_GetEventDetails(const CFE_SB_MessageTxn_State_t *TxnPtr, const CFE_SB_PipeSetEntry_t *ContextPtr,
                                       uint16 EventId, CFE_ES_TaskId_t TskId, char *EvtMsg, size_t EvtMsgSize,
                                       CFE_EVS_EventType_Enum_t *EventType, int32 *ReqBit);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Reports a single event related to a transmit transaction
 *
 * Internal routine that performs the basic flow of sending a single event ID related
 * to a transmit transaction.  This uses CFE_SB_TransmitTxn_GetEventDetails() to determine
 * how the event should be reported, then checks/sets the Request bit (loop detect), sends the
 * event, then clears the request bit.
 *
 * \param[in]   TxnPtr     Transaction object
 * \param[in]   ContextPtr Context information (may be NULL for general transaction events)
 * \param[in]   EventId    Event to report
 *
 * \returns Boolean indicating if the event constitutes a transaction failure
 * \retval  true   if the transmit transaction failed as a result (event is an error)
 * \retval  false  if this was informational (event is benign, e.g. no subscribers)
 */
bool CFE_SB_MessageTxn_ReportSingleEvent(const CFE_SB_MessageTxn_State_t *TxnPtr,
                                         const CFE_SB_PipeSetEntry_t *ContextPtr, uint16 EventId);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Implements all reporting and accouting associated with a transmit transaction
 *
 * Internal routine that sends all events and increments any telemetry counters associated
 * with the transaction.  This should be called as the last step of a transmit operation.
 *
 * \param[in]   TxnPtr     Transaction object
 */
void CFE_SB_MessageTxn_ReportEvents(const CFE_SB_MessageTxn_State_t *TxnPtr);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Process all pipes identified in the transaction object
 *
 * The provided HandlerFunc will be invoked for every item in the PipeSet within the transaction
 * object.  The handler function can stop the loop by returning "false".
 *
 * \param[in]    HandlerFunc Handler function to call
 * \param[inout] TxnPtr      Transaction object, passed through to handler
 * \param[inout] Arg         Opaque argument to pass to handler
 */
void CFE_SB_MessageTxn_ProcessPipes(CFE_SB_MessageTxn_PipeHandler_t HandlerFunc, CFE_SB_MessageTxn_State_t *TxnPtr,
                                    void *Arg);

/*
 * Receive Transaction implementation/helper functions
 * These functions are specific to the receive-side operation
 */

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Initialize a receive transaction
 *
 * This should be the first function called to initialize a transaction state object
 * that has been newly allocated.  The object will be put into a safe initial state.
 *
 * The RefMemPtr should refer to the object/buffer that the user intends to receive from the
 * software bus.  This is kept as part of the transaction mainly for logging/event purposes,
 * the address may be reported in events if the transaction is unsuccessful.
 *
 * \param[out] TxnPtr    Transaction object to initialize
 * \param[in]  RefMemPtr Pointer to user object/buffer being received (opaque)
 */
CFE_SB_MessageTxn_State_t *CFE_SB_ReceiveTxn_Init(CFE_SB_ReceiveTxn_State_t *TxnPtr, const void *RefMemPtr);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Sets the Pipe ID to read for a receive transaction
 *
 * Currently, receive transactions only deal with a single pipe.  This sets the pipe ID to
 * read to receive the object.
 *
 * \param[inout] TxnPtr  Transaction object
 * \param[in]    PipeId  Pipe ID to read from
 */
void CFE_SB_ReceiveTxn_SetPipeId(CFE_SB_MessageTxn_State_t *TxnPtr, CFE_SB_PipeId_t PipeId);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Pipe handler function for receive transactions
 *
 * Helper function to implement reading of a pipe during a receive transaction.  This
 * is only used via CFE_SB_MessageTxn_ProcessPipes(), but declared here so it can be unit
 * tested.
 *
 * This implements the logic of transmitting a message buffer to all destinations that were
 * identified via CFE_SB_TransmitTxn_FindDestinations().
 *
 * As this function will broadcast the message to any number of destinations (0-many),
 * and some may be successful and some may fail, this function always returns true
 * to continue the loop inside CFE_SB_MessageTxn_ProcessPipes().
 *
 * The status/result of every queue write operation is kept as part of the transaction
 * status object.  Event reporting related those operations is deferred to the
 * CFE_SB_TransmitTxn_ReportEvents() function, which should be invoked at the end
 * of the transaction to report any delivery issues.
 *
 * The caller is expected to hold a reference (use count) of the buffer prior to invoking
 * this routine, representing itself, plus the number of active destinations.
 *
 * \note If any queue write operation is _not_ successful, the use count will be decremented.
 * \sa CFE_SB_MessageTxn_ProcessPipes
 *
 * \param[inout] TxnPtr     Transaction object
 * \param[in]    ContextPtr Pointer to pipe entry within transaction
 * \param[inout] Arg        Opaque argument for API, should be a CFE_SB_BufferD_t**
 * \returns always false to stop the parent loop (receive transactions only read a single pipe)
 */
bool CFE_SB_ReceiveTxn_PipeHandler(CFE_SB_MessageTxn_State_t *TxnPtr, CFE_SB_PipeSetEntry_t *ContextPtr, void *Arg);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Executes a receive transaction
 *
 * Implements reading of the pipe(s) and exporting information such as the content pointer, content size,
 * and message ID.
 *
 * \param[inout] TxnPtr     Transaction object
 * \returns Pointer to buffer that was read
 * \retval  NULL if no message was read (e.g. if a timeout occurred or polling an empty queue)
 */
const CFE_SB_Buffer_t *CFE_SB_ReceiveTxn_Execute(CFE_SB_MessageTxn_State_t *TxnPtr);

/*
 * Transmit Transaction implementation/helper functions
 * These functions are specific to the transmit-side operation
 */

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Initialize a transmit transaction
 *
 * This should be the first function called to initialize a transaction state object
 * that has been newly allocated.  The object will be put into a safe initial state.
 *
 * The RefMemPtr should refer to the object/buffer that the user intends to send on the
 * software bus.  This is kept as part of the transaction mainly for logging/event purposes,
 * the address may be reported in events if the transaction is unsuccessful.
 *
 * \param[out] TxnPtr    Transaction object to initialize
 * \param[in]  RefMemPtr Pointer to user object/buffer being transmitted (opaque)
 */
CFE_SB_MessageTxn_State_t *CFE_SB_TransmitTxn_Init(CFE_SB_TransmitTxn_State_t *TxnPtr, const void *RefMemPtr);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Set up a transmit transaction from a CFE MSG object
 *
 * Extracts the routing information - specifically MsgId and Size - from the message content.
 *
 * Software Bus transmit operations typically operate based on the MsgId and Size information
 * that the application stores the message header prior to transmission.  This function implements
 * that traditional behavior by pulling this information out of the message and caching it in
 * the transaction state object for use during routing and delivery of the object.
 *
 * The information is extracted from the message using the CFE MSG API calls.
 *
 * \param[inout] TxnPtr   Transaction object
 * \param[in]    MsgPtr   Pointer to message being transmitted
 */
void CFE_SB_TransmitTxn_SetupFromMsg(CFE_SB_MessageTxn_State_t *TxnPtr, const CFE_MSG_Message_t *MsgPtr);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Find the complete set of destination pipes for the given transaction
 *
 * Looks up the routing information (via the SBR subsystem/module) and collects the set of
 * active destinations (Pipe IDs) that the message will need to be broadcast to.
 *
 * If no destinations are found, then this sets the transaction status to generate a
 * NO SUBSCRIBERS event, but it does not actually send the event from here.
 *
 * \sa CFE_SB_TransmitTxn_ReportEvents()
 *
 * \note This also increments the buffer use count for every destination found, in anticipation
 * of writing the buffer address into the underlying queue.  If the subsequent write is not
 * actually successful, then the count must be decremented accordingly, to keep the reference
 * counts correct.
 *
 * \param[inout] TxnPtr    Transaction object
 * \param[inout] BufDscPtr Buffer descriptor that is pending broadcast
 */
void CFE_SB_TransmitTxn_FindDestinations(CFE_SB_MessageTxn_State_t *TxnPtr, CFE_SB_BufferD_t *BufDscPtr);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Pipe handler function for transmit transactions
 *
 * Helper function to implement reading of a pipe during a transmit transaction.  This
 * is only used via CFE_SB_MessageTxn_ProcessPipes(), but declared here so it can be unit
 * tested.
 *
 * \sa CFE_SB_MessageTxn_ProcessPipes
 *
 * \param[inout] TxnPtr     Transaction object
 * \param[in]    ContextPtr Pointer to pipe entry within transaction
 * \param[inout] Arg        Opaque argument for API, should be a CFE_SB_BufferD_t*
 * \returns always true to continue the parent loop (transmit transactions process all pipes)
 */
bool CFE_SB_TransmitTxn_PipeHandler(CFE_SB_MessageTxn_State_t *TxnPtr, CFE_SB_PipeSetEntry_t *ContextPtr, void *Arg);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Executes the transmit transaction
 *
 * Internal routine that finds all active destinations for the route  and broadcasts the
 * buffer to those destinations. Any errors or off-nominal events that occur will be stored
 * in the transaction object for deferred reporting via CFE_SB_TransmitTxn_ReportEvents().
 *
 * \note Upon successful operation, this function decrements the use count of the buffer.
 * The calling routine is expected to hold one reference to the buffer being transmitted, which
 * is then consumed by this call.  If the caller intends to retain a reference to the buffer,
 * the use count must be incremented prior to invoking this function.
 *
 * However, if this returns a non-successful status code then the use count will _not_ be
 * decremented - but this can generally only fail if the passed-in buffer does not validate.
 *
 * \param[inout] TxnPtr    Transaction object
 * \param[inout] BufPtr    Buffer object that is pending to be broadcast
 */
void CFE_SB_TransmitTxn_Execute(CFE_SB_MessageTxn_State_t *TxnPtr, CFE_SB_Buffer_t *BufPtr);

/*
 * Software Bus Message Handler Function prototypes
 */

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Command Message Handler function
 *
 * \param[in] data Pointer to command structure
 * \return Execution status, see \ref CFEReturnCodes
 */
int32 CFE_SB_NoopCmd(const CFE_SB_NoopCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Command Message Handler function
 *
 * \param[in] data Pointer to command structure
 * \return Execution status, see \ref CFEReturnCodes
 */
int32 CFE_SB_ResetCountersCmd(const CFE_SB_ResetCountersCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Command Message Handler function
 *
 * \param[in] data Pointer to command structure
 * \return Execution status, see \ref CFEReturnCodes
 */
int32 CFE_SB_EnableSubReportingCmd(const CFE_SB_EnableSubReportingCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Command Message Handler function
 *
 * \param[in] data Pointer to command structure
 * \return Execution status, see \ref CFEReturnCodes
 */
int32 CFE_SB_DisableSubReportingCmd(const CFE_SB_DisableSubReportingCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Function to send the SB housekeeping packet.
 *
 * @note Command counter not incremented for this command
 *
 * \param[in] data Pointer to command structure
 * \return Execution status, see \ref CFEReturnCodes
 */
int32 CFE_SB_SendHKTlmCmd(const CFE_SB_SendHkCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Command Message Handler function
 *
 * SB internal function to enable a specific route.
 * A route is defined as a MsgId/PipeId combination.
 *
 * \param[in] data Pointer to command structure
 * \return Execution status, see \ref CFEReturnCodes
 */
int32 CFE_SB_EnableRouteCmd(const CFE_SB_EnableRouteCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Command Message Handler function
 *
 * SB internal function to disable a specific route.
 * A route is defined as a MsgId/PipeId combination.
 *
 * \param[in] data Pointer to command structure
 * \return Execution status, see \ref CFEReturnCodes
 */
int32 CFE_SB_DisableRouteCmd(const CFE_SB_DisableRouteCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Command Message Handler function
 *
 * SB internal function to send a Software Bus statistics packet
 *
 * \param[in] data Pointer to command structure
 * \return Execution status, see \ref CFEReturnCodes
 */
int32 CFE_SB_SendStatsCmd(const CFE_SB_SendSbStatsCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Command Message Handler function
 *
 * SB internal function to handle processing of 'Write Routing Info' Cmd
 *
 * \param[in] data Pointer to command structure
 * \return Execution status, see \ref CFEReturnCodes
 */
int32 CFE_SB_WriteRoutingInfoCmd(const CFE_SB_WriteRoutingInfoCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Command Message Handler function
 *
 * SB internal function to handle processing of 'Write Pipe Info' Cmd
 *
 * \param[in] data Pointer to command structure
 * \return Execution status, see \ref CFEReturnCodes
 */
int32 CFE_SB_WritePipeInfoCmd(const CFE_SB_WritePipeInfoCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Command Message Handler function
 *
 * SB internal function to handle processing of 'Write Map Info' Cmd
 *
 * \param[in] data Pointer to command structure
 * \return Execution status, see \ref CFEReturnCodes
 */
int32 CFE_SB_WriteMapInfoCmd(const CFE_SB_WriteMapInfoCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Command Message Handler function
 *
 * SB function to build and send an SB packet containing a complete list of
 * current subscriptions.Intended to be used primarily for the Software Bus
 * Networking Application (SBN).
 *
 * \param[in] data Pointer to command structure
 * \return Execution status, see \ref CFEReturnCodes
 */
int32 CFE_SB_SendPrevSubsCmd(const CFE_SB_SendPrevSubsCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Locate the Pipe table entry correlating with a given Pipe ID.
 *
 * This only returns a pointer to the table entry where the record
 * should reside, but does _not_ actually check/validate the entry.
 *
 * If the passed-in ID parameter is not within the acceptable range of ID
 * values for pipe IDs, such that it could never be valid under
 * any circumstances, then NULL is returned.  Otherwise, a pointer to the
 * corresponding table entry is returned, indicating the location where
 * that ID _should_ reside, if it is currently in use.
 *
 * @note This only returns where the ID should reside, not that it actually
 * resides there.  If looking up an existing ID, then caller must additionally
 * confirm that the returned record is a match to the expected ID before using
 * or modifying the data within the returned record pointer.
 *
 * The CFE_SB_PipeDescIsMatch() function can be used to check/confirm
 * if the returned table entry is a positive match for the given ID.
 *
 * @sa CFE_SB_PipeDescIsMatch()
 *
 * @param[in]   PipeId   the Pipe ID to locate
 * @return pointer to Pipe Table entry for the given Pipe ID
 */
CFE_SB_PipeD_t *CFE_SB_LocatePipeDescByID(CFE_SB_PipeId_t PipeId);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Check if a Pipe descriptor is in use or free/empty
 *
 * This routine checks if the Pipe table entry is in use or if it is free
 *
 * As this dereferences fields within the descriptor, global data must be
 * locked prior to invoking this function.
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   PipeDscPtr   pointer to Pipe table entry
 * @returns true if the entry is in use/configured, or false if it is free/empty
 */
static inline bool CFE_SB_PipeDescIsUsed(const CFE_SB_PipeD_t *PipeDscPtr)
{
    return CFE_RESOURCEID_TEST_DEFINED(PipeDscPtr->PipeId);
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Get the ID value from a Pipe table entry
 *
 * This routine converts the table entry back to an abstract ID.
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   PipeDscPtr   pointer to Pipe table entry
 * @returns PipeID of entry
 */
static inline CFE_SB_PipeId_t CFE_SB_PipeDescGetID(const CFE_SB_PipeD_t *PipeDscPtr)
{
    return PipeDscPtr->PipeId;
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Marks a Pipe table entry as used (not free)
 *
 * This sets the internal field(s) within this entry, and marks
 * it as being associated with the given Pipe ID.
 *
 * As this dereferences fields within the descriptor, global data must be
 * locked prior to invoking this function.
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   PipeDscPtr   pointer to Pipe table entry
 * @param[in]   PendingID    the Pipe ID of this entry
 */
static inline void CFE_SB_PipeDescSetUsed(CFE_SB_PipeD_t *PipeDscPtr, CFE_ResourceId_t PendingID)
{
    PipeDscPtr->PipeId = CFE_SB_PIPEID_C(PendingID);
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Set a Pipe descriptor table entry free (not used)
 *
 * This clears the internal field(s) within this entry, and allows the
 * memory to be re-used in the future.
 *
 * As this dereferences fields within the descriptor, global data must be
 * locked prior to invoking this function.
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   PipeDscPtr   pointer to Pipe table entry
 */
static inline void CFE_SB_PipeDescSetFree(CFE_SB_PipeD_t *PipeDscPtr)
{
    PipeDscPtr->PipeId = CFE_SB_INVALID_PIPE;
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Check if a Pipe descriptor is a match for the given PipeID
 *
 * This routine confirms that the previously-located descriptor is valid
 * and matches the expected Pipe ID.
 *
 * As this dereferences fields within the descriptor, global data must be
 * locked prior to invoking this function.
 *
 * This function may be used in conjunction with CFE_SB_LocatePipeDescByID()
 * to confirm that the located record is a positive match to the expected ID.
 * As such, the record pointer is also permitted to be NULL, to alleviate the
 * need for the caller to handle this possibility explicitly.
 *
 * Once a record pointer has been successfully validated using this routine,
 * it may be safely passed to all other internal functions.
 *
 * @sa CFE_SB_LocatePipeDescByID
 *
 * @param[in]   PipeDscPtr   pointer to Pipe table entry
 * @param[in]   PipeID       expected Pipe ID
 * @returns true if the entry matches the given Pipe ID
 */
static inline bool CFE_SB_PipeDescIsMatch(const CFE_SB_PipeD_t *PipeDscPtr, CFE_SB_PipeId_t PipeID)
{
    return (PipeDscPtr != NULL && CFE_RESOURCEID_TEST_EQUAL(PipeDscPtr->PipeId, PipeID));
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Checks if a table slot is used or not
 *
 * Helper for allocating IDs,
 * Used in conjunction with CFE_ResourceId_FindNext().
 *
 * @param CheckId generic slot ID to test
 * @returns true if slot is currently in use/unavailable
 */
bool CFE_SB_CheckPipeDescSlotUsed(CFE_ResourceId_t CheckId);

/*
 * Helper functions for background file write requests (callbacks)
 */

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Local callback helper for writing map info to a file
 * This retrieves a single record of information from the SB global state object(s)
 */
void CFE_SB_CollectMsgMapInfo(CFE_SBR_RouteId_t RouteId, void *ArgPtr);
bool CFE_SB_WriteMsgMapInfoDataGetter(void *Meta, uint32 RecordNum, void **Buffer, size_t *BufSize);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Local callback helper for writing routing info to a file
 * This retrieves a single record of information from the SB global state object(s)
 */
void CFE_SB_CollectRouteInfo(CFE_SBR_RouteId_t RouteId, void *ArgPtr);
bool CFE_SB_WriteRouteInfoDataGetter(void *Meta, uint32 RecordNum, void **Buffer, size_t *BufSize);
bool CFE_SB_WritePipeInfoDataGetter(void *Meta, uint32 RecordNum, void **Buffer, size_t *BufSize);
void CFE_SB_BackgroundFileEventHandler(void *Meta, CFE_FS_FileWriteEvent_t Event, int32 Status, uint32 RecordNum,
                                       size_t BlockSize, size_t Position);

/*
 * External variables private to the software bus module
 */

extern CFE_SB_Global_t CFE_SB_Global;

#endif /* CFE_SB_PRIV_H */
