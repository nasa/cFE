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
#define CFE_SB_SEND_NO_SUBS_EID_BIT  0
#define CFE_SB_GET_BUF_ERR_EID_BIT   1
#define CFE_SB_MSGID_LIM_ERR_EID_BIT 2
#define CFE_SB_Q_FULL_ERR_EID_BIT    3
#define CFE_SB_Q_WR_ERR_EID_BIT      4

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
     * Actual MsgId of the content, cached here to avoid repeat
     * calls into CFE_MSG API during traversal/delivery of the message.
     *
     * MsgId is set for buffers which contain actual data in transit.  AppId is unset
     * while in transit, as it may be sent to multiple apps.
     *
     * During zero copy buffer initial allocation, the MsgId is not known at this time
     * and should be set to the invalid msg ID.
     */
    CFE_SB_MsgId_t MsgId;

    /**
     * Current owner of the buffer, if owned by a single app.
     *
     * This is used to track "zero copy" buffer allocations - this will be set to
     * the AppID that initally allocated it, before it is used to transmit a message.
     *
     * When the message is in transit, it may be queued to multiple applictions,
     * so this is unset.
     */
    CFE_ES_AppId_t AppId;

    size_t         AllocatedSize; /**< Total size of this descriptor (including descriptor itself) */
    size_t         ContentSize;   /**< Actual size of message content currently stored in the buffer */
    CFE_MSG_Type_t ContentType;   /**< Type of message content currently stored in the buffer */

    bool AutoSequence; /**< If message should get its sequence number assigned from the route */

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
    uint8             Opts;
    uint8             Spare;
    CFE_ES_AppId_t    AppId;
    osal_id_t         SysQueueId;
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
** This tracks the number of desinations along with destination data for 1 route.
** Each route may contain zero or more desinations (variable length).
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
**  Typedef:  CFE_SB_SendErrEventBuf_t
**
**  Purpose:
**     This structure is used to store event information during a send.
*/
typedef struct
{
    uint32          EventId;
    int32           ErrStat;
    CFE_SB_PipeId_t PipeId;
} CFE_SB_SendErrEventBuf_t;

/******************************************************************************
**  Typedef:  CFE_SB_EventBuf_t
**
**  Purpose:
**     This structure is used to store event information during a send.
*/
typedef struct
{
    uint32                   EvtsToSnd;
    CFE_SB_SendErrEventBuf_t EvtBuf[CFE_PLATFORM_SB_MAX_DEST_PER_PKT];
} CFE_SB_EventBuf_t;

/*
** Software Bus Function Prototypes
*/

int32 CFE_SB_AppInit(void);
int32 CFE_SB_InitBuffers(void);
void  CFE_SB_InitPipeTbl(void);
void  CFE_SB_InitIdxStack(void);
void  CFE_SB_ResetCounts(void);
void  CFE_SB_LockSharedData(const char *FuncName, int32 LineNumber);
void  CFE_SB_UnlockSharedData(const char *FuncName, int32 LineNumber);
void  CFE_SB_ReleaseBuffer(CFE_SB_BufferD_t *bd, CFE_SB_DestinationD_t *dest);
int32 CFE_SB_WriteQueue(CFE_SB_PipeD_t *pd, uint32 TskId, const CFE_SB_BufferD_t *bd, CFE_SB_MsgId_t MsgId);
void  CFE_SB_ProcessCmdPipePkt(CFE_SB_Buffer_t *SBBufPtr);
void  CFE_SB_ResetCounters(void);
void  CFE_SB_SetMsgSeqCnt(CFE_MSG_Message_t *MsgPtr, uint32 Count);
char *CFE_SB_GetAppTskName(CFE_ES_TaskId_t TaskId, char *FullName);
int32 CFE_SB_DeletePipeWithAppId(CFE_SB_PipeId_t PipeId, CFE_ES_AppId_t AppId);
int32 CFE_SB_DeletePipeFull(CFE_SB_PipeId_t PipeId, CFE_ES_AppId_t AppId);
int32 CFE_SB_SubscribeFull(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId, CFE_SB_Qos_t Quality, uint16 MsgLim,
                           uint8 Scope);

int32 CFE_SB_UnsubscribeWithAppId(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId, CFE_ES_AppId_t AppId);

int32  CFE_SB_UnsubscribeFull(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId, uint8 Scope, CFE_ES_AppId_t AppId);
int32  CFE_SB_TransmitMsgValidate(CFE_MSG_Message_t *MsgPtr, CFE_SB_MsgId_t *MsgIdPtr, CFE_MSG_Size_t *SizePtr,
                                  CFE_SBR_RouteId_t *RouteIdPtr);
int32  CFE_SB_ZeroCopyReleaseAppId(CFE_ES_AppId_t AppId);
void   CFE_SB_IncrBufUseCnt(CFE_SB_BufferD_t *bd);
void   CFE_SB_DecrBufUseCnt(CFE_SB_BufferD_t *bd);
int32  CFE_SB_ValidateMsgId(CFE_SB_MsgId_t MsgId);
int32  CFE_SB_ValidatePipeId(CFE_SB_PipeId_t PipeId);
void   CFE_SB_IncrCmdCtr(int32 status);
void   CFE_SB_SetSubscriptionReporting(uint32 state);
int32  CFE_SB_SendSubscriptionReport(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId, CFE_SB_Qos_t Quality);
uint32 CFE_SB_RequestToSendEvent(CFE_ES_TaskId_t TaskId, uint32 Bit);
void   CFE_SB_FinishSendEvent(CFE_ES_TaskId_t TaskId, uint32 Bit);
CFE_SB_DestinationD_t *CFE_SB_GetDestinationBlk(void);
int32                  CFE_SB_PutDestinationBlk(CFE_SB_DestinationD_t *Dest);

/**
 * \brief For SB buffer tracking, get first/next position in a list
 */
static inline CFE_SB_BufferLink_t *CFE_SB_TrackingListGetNext(CFE_SB_BufferLink_t *Node)
{
    return Node->Next;
}

/**
 * \brief For SB buffer tracking, checks if this current position represents the end of the list
 */
static inline bool CFE_SB_TrackingListIsEnd(CFE_SB_BufferLink_t *List, CFE_SB_BufferLink_t *Node)
{
    /* Normally list nodes should never have NULL, buf if they do, do not follow it */
    return (Node == NULL || Node == List);
}

/**
 * \brief For SB buffer tracking, reset link state to default
 *
 * This turns the node into a singleton/lone object (not in a list)
 * or resets the head link to be empty.
 */
void CFE_SB_TrackingListReset(CFE_SB_BufferLink_t *Link);

/**
 * \brief For SB buffer tracking, removes a node from a tracking list
 *
 * Extracts a single node from whatever list it is in.  After this the
 * node becomes a singleton owned by the caller.  It may be put into
 * another list or freed.
 */
void CFE_SB_TrackingListRemove(CFE_SB_BufferLink_t *Node);

/**
 * \brief For SB buffer tracking, adds a node to a tracking list
 *
 * Extracts a single node from the list its in.  After this the
 * node becomes a singleton owned by the caller.  It must put it
 * in another list or free it.
 */
void CFE_SB_TrackingListAdd(CFE_SB_BufferLink_t *List, CFE_SB_BufferLink_t *Node);

/**
 * \brief Allocates a new buffer descriptor from the SB memory pool.
 *
 * \param[in] MaxMsgSize Maximum message content size that the buffer must be capable of holding
 * \returns Pointer to buffer descriptor, or NULL on failure.
 */
CFE_SB_BufferD_t *CFE_SB_GetBufferFromPool(size_t MaxMsgSize);

/**
 * \brief Returns a buffer to SB memory pool
 *
 * \param[in] Pointer to descriptor to return
 */
void CFE_SB_ReturnBufferToPool(CFE_SB_BufferD_t *bd);

/**
 * \brief Broadcast a SB buffer descriptor to all destinations in route
 *
 * Internal routine that implements the logic of transmitting a message buffer
 * to all destinations subscribed in the SB route.
 *
 * As this function will broadcast the message to any number of destinations (0-many),
 * and some may be successful and some may fail, the status cannot be expressed
 * in any single error code, so this does not return any status.
 *
 * Instead, this routine handles all potential outcomes on its own, and does
 * not expect the caller to handle any delivery issues.  Also note that the general
 * design pattern of the software bus is a "send and forget" model where the sender does
 * not know (or care) what entities are subscribed to the data being generated.
 *
 *  - For any undeliverable destination (limit, OSAL error, etc), a proper event is generated.
 *  - For any successful queueing, the buffer use count is incremented
 *
 * The caller is expected to hold a reference (use count) of the buffer prior to invoking
 * this routine, representing itself, which is then consumed by this routine.
 *
 * \note  _This call will "consume" the buffer by decrementing the buffer use count_ after
 *        broadcasting the message to all subscribed pipes.
 *
 * The caller should not access the buffer again after calling this function, as it may
 * be deallocated at any time.  If the caller wishes to continue accessing the buffer,
 * it should explicitly increment the use count before calling this, which will prevent
 * deallocation.
 *
 * \param[in] BufDscPtr Pointer to the buffer descriptor to broadcast
 * \param[in] RouteId   Route to send to
 */
void CFE_SB_BroadcastBufferToRoute(CFE_SB_BufferD_t *BufDscPtr, CFE_SBR_RouteId_t RouteId);

/**
 * \brief Perform basic sanity check on the Zero Copy handle
 *
 * \param[in]  BufPtr pointer to the content buffer
 * \param[out] BufDscPtr Will be set to actual buffer descriptor
 *
 * \returns CFE_SUCCESS if validation passed, or error code.
 */
int32 CFE_SB_ZeroCopyBufferValidate(CFE_SB_Buffer_t *BufPtr, CFE_SB_BufferD_t **BufDscPtr);

/**
 * \brief Add a destination node
 *
 * Private function that will add a destination node to the linked list
 *
 * \note Assumes destination pointer is valid
 *
 * \param[in] RouteId The route ID to add destination node to
 * \param[in] DestPtr Pointer to the destination to add
 */
int32 CFE_SB_AddDestNode(CFE_SBR_RouteId_t RouteId, CFE_SB_DestinationD_t *NewNode);

/**
 * \brief Remove a destination node
 *
 * Private function that will remove a destination node from the linked list
 *
 * \note Assumes destination pointer is valid and in route
 *
 * \param[in] RouteId The route ID to remove destination node from
 * \param[in] DestPtr Pointer to the destination to remove
 */
void CFE_SB_RemoveDestNode(CFE_SBR_RouteId_t RouteId, CFE_SB_DestinationD_t *NodeToRemove);

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

/*****************************************************************************/
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
** \returns Estimated number of bytes in the message header for the given message
**/
size_t CFE_SB_MsgHdrSize(const CFE_MSG_Message_t *MsgPtr);

/*
 * Software Bus Message Handler Function prototypes
 */
int32 CFE_SB_NoopCmd(const CFE_SB_NoopCmd_t *data);
int32 CFE_SB_ResetCountersCmd(const CFE_SB_ResetCountersCmd_t *data);
int32 CFE_SB_EnableSubReportingCmd(const CFE_SB_EnableSubReportingCmd_t *data);
int32 CFE_SB_DisableSubReportingCmd(const CFE_SB_DisableSubReportingCmd_t *data);
int32 CFE_SB_SendHKTlmCmd(const CFE_MSG_CommandHeader_t *data);
int32 CFE_SB_EnableRouteCmd(const CFE_SB_EnableRouteCmd_t *data);
int32 CFE_SB_DisableRouteCmd(const CFE_SB_DisableRouteCmd_t *data);
int32 CFE_SB_SendStatsCmd(const CFE_SB_SendSbStatsCmd_t *data);
int32 CFE_SB_WriteRoutingInfoCmd(const CFE_SB_WriteRoutingInfoCmd_t *data);
int32 CFE_SB_WritePipeInfoCmd(const CFE_SB_WritePipeInfoCmd_t *data);
int32 CFE_SB_WriteMapInfoCmd(const CFE_SB_WriteMapInfoCmd_t *data);
int32 CFE_SB_SendPrevSubsCmd(const CFE_SB_SendPrevSubsCmd_t *data);

/**
 * @brief Locate the Pipe table entry correlating with a given Pipe ID.
 *
 * This only returns a pointer to the table entry and does _not_
 * otherwise check/validate the entry.
 *
 * @param[in]   PipeId   the Pipe ID to locate
 * @return pointer to Pipe Table entry for the given Pipe ID
 */
extern CFE_SB_PipeD_t *CFE_SB_LocatePipeDescByID(CFE_SB_PipeId_t PipeId);

/**
 * @brief Check if an Pipe descriptor is in use or free/empty
 *
 * This routine checks if the Pipe table entry is in use or if it is free
 *
 * As this dereferences fields within the descriptor, global data must be
 * locked prior to invoking this function.
 *
 * @param[in]   PipeDscPtr   pointer to Pipe table entry
 * @returns true if the entry is in use/configured, or false if it is free/empty
 */
static inline bool CFE_SB_PipeDescIsUsed(const CFE_SB_PipeD_t *PipeDscPtr)
{
    return CFE_RESOURCEID_TEST_DEFINED(PipeDscPtr->PipeId);
}

/**
 * @brief Get the ID value from an Pipe table entry
 *
 * This routine converts the table entry back to an abstract ID.
 *
 * @param[in]   PipeDscPtr   pointer to Pipe table entry
 * @returns PipeID of entry
 */
static inline CFE_SB_PipeId_t CFE_SB_PipeDescGetID(const CFE_SB_PipeD_t *PipeDscPtr)
{
    return PipeDscPtr->PipeId;
}

/**
 * @brief Marks an Pipe table entry as used (not free)
 *
 * This sets the internal field(s) within this entry, and marks
 * it as being associated with the given Pipe ID.
 *
 * As this dereferences fields within the descriptor, global data must be
 * locked prior to invoking this function.
 *
 * @param[in]   PipeDscPtr   pointer to Pipe table entry
 * @param[in]   PipeID       the Pipe ID of this entry
 */
static inline void CFE_SB_PipeDescSetUsed(CFE_SB_PipeD_t *PipeDscPtr, CFE_ResourceId_t PendingID)
{
    PipeDscPtr->PipeId = CFE_SB_PIPEID_C(PendingID);
}

/**
 * @brief Set an Pipe descriptor table entry free (not used)
 *
 * This clears the internal field(s) within this entry, and allows the
 * memory to be re-used in the future.
 *
 * As this dereferences fields within the descriptor, global data must be
 * locked prior to invoking this function.
 *
 * @param[in]   PipeDscPtr   pointer to Pipe table entry
 */
static inline void CFE_SB_PipeDescSetFree(CFE_SB_PipeD_t *PipeDscPtr)
{
    PipeDscPtr->PipeId = CFE_SB_INVALID_PIPE;
}

/**
 * @brief Check if an Pipe descriptor is a match for the given PipeID
 *
 * This routine confirms that the previously-located descriptor is valid
 * and matches the expected Pipe ID.
 *
 * As this dereferences fields within the descriptor, global data must be
 * locked prior to invoking this function.
 *
 * @param[in]   PipeDscPtr   pointer to Pipe table entry
 * @param[in]   PipeID       expected Pipe ID
 * @returns true if the entry matches the given Pipe ID
 */
static inline bool CFE_SB_PipeDescIsMatch(const CFE_SB_PipeD_t *PipeDscPtr, CFE_SB_PipeId_t PipeID)
{
    return (PipeDscPtr != NULL && CFE_RESOURCEID_TEST_EQUAL(PipeDscPtr->PipeId, PipeID));
}

/* Availability check functions used in conjunction with CFE_ResourceId_FindNext() */
bool CFE_SB_CheckPipeDescSlotUsed(CFE_ResourceId_t CheckId);

/*
 * Helper functions for background file write requests (callbacks)
 */
void CFE_SB_CollectMsgMapInfo(CFE_SBR_RouteId_t RouteId, void *ArgPtr);
bool CFE_SB_WriteMsgMapInfoDataGetter(void *Meta, uint32 RecordNum, void **Buffer, size_t *BufSize);
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
