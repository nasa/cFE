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
** File: cfe_sb_priv.h
**
** Purpose:
**      This header file contains prototypes for private functions and type
**      definitions for SB internal use.
**
** Author:   R.McGraw/SSI
**
******************************************************************************/

#ifndef _cfe_sb_priv_
#define _cfe_sb_priv_

/*
** Includes
*/
#include "common_types.h"
#include "private/cfe_private.h"
#include "private/cfe_sb_destination_typedef.h"
#include "cfe_sb.h"
#include "cfe_sb_msg.h"
#include "cfe_time.h"
#include "cfe_es.h"
#include "private/cfe_sbr.h"

/*
** Macro Definitions
*/

#define CFE_SB_UNUSED_QUEUE             OS_OBJECT_ID_UNDEFINED
#define CFE_SB_INVALID_PIPE             0xFF
#define CFE_SB_NO_DESTINATION           0xFF
#define CFE_SB_FAILED                   1
#define SB_DONT_CARE                    0

#define CFE_SB_NO_DUPLICATE             0
#define CFE_SB_DUPLICATE                1

#define CFE_SB_INACTIVE                 0
#define CFE_SB_ACTIVE                   1

#define CFE_SB_GLOBAL                   0
#define CFE_SB_LOCAL                    1

#define CFE_SB_SEND_ZEROCOPY            0
#define CFE_SB_SEND_ONECOPY             1

#define CFE_SB_NOT_IN_USE               0
#define CFE_SB_IN_USE                   1

#define CFE_SB_DISABLE                  0
#define CFE_SB_ENABLE                   1

#define CFE_SB_DENIED                   0
#define CFE_SB_GRANTED                  1

#define CFE_SB_DO_NOT_INCREMENT         0
#define CFE_SB_INCREMENT_TLM            1

#define CFE_SB_MAIN_LOOP_ERR_DLY        1000
#define CFE_SB_CMD_PIPE_DEPTH           32
#define CFE_SB_CMD_PIPE_NAME            "SB_CMD_PIPE"
#define CFE_SB_MAX_CFG_FILE_EVENTS_TO_FILTER     8

#define CFE_SB_QOS_LOW_PRIORITY         0
#define CFE_SB_QOS_LOW_RELIABILITY      0

#define CFE_SB_PIPE_OVERFLOW            (-1)
#define CFE_SB_PIPE_WR_ERR              (-2)
#define CFE_SB_USECNT_ERR               (-3)
#define CFE_SB_FILE_IO_ERR              (-5)

/* bit map for stopping recursive event problem */
#define CFE_SB_SEND_NO_SUBS_EID_BIT     0
#define CFE_SB_GET_BUF_ERR_EID_BIT      1
#define CFE_SB_MSGID_LIM_ERR_EID_BIT    2
#define CFE_SB_Q_FULL_ERR_EID_BIT       3
#define CFE_SB_Q_WR_ERR_EID_BIT         4

/*
** Type Definitions
*/

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

typedef struct {
     CFE_SB_MsgId_t    MsgId;
     uint16            UseCount;
     size_t            Size;
     void              *Buffer;
} CFE_SB_BufferD_t;

/******************************************************************************
**  Typedef:  CFE_SB_ZeroCopyD_t
**
**  Purpose:
**     This structure defines a ZERO COPY BUFFER DESCRIPTOR used to specify
**     the buffer provided to a requestor.
**
**     Note: Changing the size of this structure may require the memory pool
**     block sizes to change.
*/

typedef struct {
     CFE_ES_ResourceID_t AppID;
     size_t              Size;
     void              *Buffer;
     void              *Next;
     void              *Prev;
} CFE_SB_ZeroCopyD_t;

/******************************************************************************
**  Typedef:  CFE_SB_PipeD_t
**
**  Purpose:
**     This structure defines a pipe descriptor used to specify the
**     characteristics and status of a pipe.
*/

typedef struct {
     uint8              InUse;
     CFE_SB_PipeId_t    PipeId;
     char               AppName[OS_MAX_API_NAME];
     uint8              Opts;
     uint8              Spare;
     CFE_ES_ResourceID_t AppId;
     osal_id_t          SysQueueId;
     uint32             LastSender;
     uint16             QueueDepth;
     uint16             SendErrors;
     CFE_SB_BufferD_t  *CurrentBuff;
     CFE_SB_BufferD_t  *ToTrashBuff;
} CFE_SB_PipeD_t;

/******************************************************************************
**  Typedef:  CFE_SB_BufParams_t
**
**  Purpose:
**     This structure defines the variables related to the SB routing buffers.
*/
typedef struct {

   CFE_ES_MemHandle_t PoolHdl;
   CFE_ES_STATIC_POOL_TYPE(CFE_PLATFORM_SB_BUF_MEMORY_BYTES) Partition;

} CFE_SB_MemParams_t;


/******************************************************************************
**  Typedef:  cfe_sb_t
**
**  Purpose:
**     This structure contains the SB global variables.
*/
typedef struct
{
    osal_id_t                      SharedDataMutexId;
    uint32                         SubscriptionReporting;
    uint32                         SenderReporting;
    CFE_ES_ResourceID_t            AppId;
    uint32                         StopRecurseFlags[OS_MAX_TASKS];
    void                          *ZeroCopyTail;
    CFE_SB_PipeD_t                 PipeTbl[CFE_PLATFORM_SB_MAX_PIPES];
    CFE_SB_HousekeepingTlm_t       HKTlmMsg;
    CFE_SB_StatsTlm_t              StatTlmMsg;
    CFE_SB_PipeId_t                CmdPipe;
    CFE_SB_MemParams_t             Mem;
    CFE_SB_AllSubscriptionsTlm_t   PrevSubMsg;
    CFE_SB_SingleSubscriptionTlm_t SubRprtMsg;
    CFE_EVS_BinFilter_t            EventFilters[CFE_SB_MAX_CFG_FILE_EVENTS_TO_FILTER];
} cfe_sb_t;


/******************************************************************************
**  Typedef:  CFE_SB_SendErrEventBuf_t
**
**  Purpose:
**     This structure is used to store event information during a send.
*/
typedef struct{
  uint32            EventId;
  int32             ErrStat;
  CFE_SB_PipeId_t   PipeId;
}CFE_SB_SendErrEventBuf_t;


/******************************************************************************
**  Typedef:  CFE_SB_EventBuf_t
**
**  Purpose:
**     This structure is used to store event information during a send.
*/
typedef struct{
  uint32    EvtsToSnd;
  CFE_SB_SendErrEventBuf_t  EvtBuf[CFE_PLATFORM_SB_MAX_DEST_PER_PKT];
}CFE_SB_EventBuf_t;


/*
** Software Bus Function Prototypes
*/

int32  CFE_SB_AppInit(void);
int32  CFE_SB_InitBuffers(void);
void   CFE_SB_InitPipeTbl(void);
void   CFE_SB_InitIdxStack(void);
void   CFE_SB_ResetCounts(void);
void   CFE_SB_LockSharedData(const char *FuncName, int32 LineNumber);
void   CFE_SB_UnlockSharedData(const char *FuncName, int32 LineNumber);
void   CFE_SB_ReleaseBuffer (CFE_SB_BufferD_t *bd, CFE_SB_DestinationD_t *dest);
int32  CFE_SB_ReadQueue(CFE_SB_PipeD_t *PipeDscPtr,CFE_ES_ResourceID_t TskId,
                        CFE_SB_TimeOut_t Time_Out,CFE_SB_BufferD_t **Message );
int32  CFE_SB_WriteQueue(CFE_SB_PipeD_t *pd,uint32 TskId,
                         const CFE_SB_BufferD_t *bd,CFE_SB_MsgId_t MsgId );
uint8  CFE_SB_GetPipeIdx(CFE_SB_PipeId_t PipeId);
int32  CFE_SB_ReturnBufferToPool(CFE_SB_BufferD_t *bd);
void   CFE_SB_ProcessCmdPipePkt(CFE_SB_Buffer_t *SBBufPtr);
void   CFE_SB_ResetCounters(void);
void   CFE_SB_SetMsgSeqCnt(CFE_MSG_Message_t *MsgPtr,uint32 Count);
char   *CFE_SB_GetAppTskName(CFE_ES_ResourceID_t TaskId, char* FullName);
CFE_SB_BufferD_t *CFE_SB_GetBufferFromPool(CFE_SB_MsgId_t MsgId, size_t Size);
CFE_SB_BufferD_t *CFE_SB_GetBufferFromCaller(CFE_SB_MsgId_t MsgId, void *Address);
CFE_SB_PipeD_t   *CFE_SB_GetPipePtr(CFE_SB_PipeId_t PipeId);
CFE_SB_PipeId_t  CFE_SB_GetAvailPipeIdx(void);
int32 CFE_SB_DeletePipeWithAppId(CFE_SB_PipeId_t PipeId,CFE_ES_ResourceID_t AppId);
int32 CFE_SB_DeletePipeFull(CFE_SB_PipeId_t PipeId,CFE_ES_ResourceID_t AppId);
int32 CFE_SB_SubscribeFull(CFE_SB_MsgId_t   MsgId,
                           CFE_SB_PipeId_t  PipeId,
                           CFE_SB_Qos_t     Quality,
                           uint16           MsgLim,
                           uint8            Scope);

int32 CFE_SB_UnsubscribeWithAppId(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId,
        CFE_ES_ResourceID_t AppId);

int32 CFE_SB_UnsubscribeFull(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId,
                              uint8 Scope, CFE_ES_ResourceID_t AppId);
int32  CFE_SB_TransmitBufferFull(CFE_SB_BufferD_t *BufDscPtr,
                                 CFE_SBR_RouteId_t RouteId,
                                 CFE_SB_MsgId_t    MsgId);
int32 CFE_SB_TransmitMsgValidate(CFE_MSG_Message_t *MsgPtr,
                                 CFE_SB_MsgId_t    *MsgIdPtr,
                                 CFE_MSG_Size_t    *SizePtr,
                                 CFE_SBR_RouteId_t *RouteIdPtr);
int32 CFE_SB_SendRtgInfo(const char *Filename);
int32 CFE_SB_SendPipeInfo(const char *Filename);
int32 CFE_SB_SendMapInfo(const char *Filename);
int32 CFE_SB_ZeroCopyReleaseDesc(CFE_SB_Buffer_t *Ptr2Release, CFE_SB_ZeroCopyHandle_t BufferHandle);
int32 CFE_SB_ZeroCopyReleaseAppId(CFE_ES_ResourceID_t         AppId);
int32 CFE_SB_DecrBufUseCnt(CFE_SB_BufferD_t *bd);
int32 CFE_SB_ValidateMsgId(CFE_SB_MsgId_t MsgId);
int32 CFE_SB_ValidatePipeId(CFE_SB_PipeId_t PipeId);
void CFE_SB_IncrCmdCtr(int32 status);
void CFE_SB_FileWriteByteCntErr(const char *Filename,uint32 Requested,uint32 Actual);
void CFE_SB_SetSubscriptionReporting(uint32 state);
uint32 CFE_SB_RequestToSendEvent(CFE_ES_ResourceID_t TaskId, uint32 Bit);
void CFE_SB_FinishSendEvent(CFE_ES_ResourceID_t TaskId, uint32 Bit);
CFE_SB_DestinationD_t *CFE_SB_GetDestinationBlk(void);
int32 CFE_SB_PutDestinationBlk(CFE_SB_DestinationD_t *Dest);

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
int32 CFE_SB_SendRoutingInfoCmd(const CFE_SB_SendRoutingInfoCmd_t *data);
int32 CFE_SB_SendPipeInfoCmd(const CFE_SB_SendPipeInfoCmd_t *data);
int32 CFE_SB_SendMapInfoCmd(const CFE_SB_SendMapInfoCmd_t *data);
int32 CFE_SB_SendPrevSubsCmd(const CFE_SB_SendPrevSubsCmd_t *data);


/*
 * External variables private to the software bus module
 */

extern cfe_sb_t CFE_SB;

#endif /* _cfe_sb_priv_ */
/*****************************************************************************/
