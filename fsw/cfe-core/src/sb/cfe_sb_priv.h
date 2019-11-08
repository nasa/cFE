/*
**  GSC-18128-1, "Core Flight Executive Version 6.6"
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
#include "cfe_sb.h"
#include "cfe_sb_msg.h"
#include "cfe_time.h"
#include "cfe_es.h"

/*
** Macro Definitions
*/

#define CFE_SB_INVALID_ROUTE_IDX        ((CFE_SB_MsgRouteIdx_t){ .RouteIdx = 0 })
#define CFE_SB_INVALID_MSG_KEY          ((CFE_SB_MsgKey_t){ .KeyIdx = 0 })
#define CFE_SB_UNUSED_QUEUE             0xFFFF
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

#define CFE_SB_TLM                      0
#define CFE_SB_CMD                      1

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
 * Using the default configuration where there is a 1:1 mapping between MsgID
 * and message key values, the number of keys is equal to the number of MsgIDs.
 *
 * If using an alternative key function / hash, this may change.
 */
#define CFE_SB_MAX_NUMBER_OF_MSG_KEYS   (1+CFE_PLATFORM_SB_HIGHEST_VALID_MSGID)
/*
** Type Definitions
*/


/******************************************************************************
**  Typedef:  CFE_SB_MsgKey_Atom_t
**
**  Purpose:
**           Defines the an integer type for the numeric key that is used for routing
**           table lookups.  This is the "raw value" type and typically should not
**           be used directly, except by internal table lookups.
**
*/
typedef uint16  CFE_SB_MsgKey_Atom_t;

/******************************************************************************
**  Typedef:  CFE_SB_MsgKey_t
**
**  Purpose:
**           This is a "holding structure" for the related integer CFE_SB_MsgKey_Atom_t values.
**           This defines the data type that is stored in other structures and/or passed between
**           software bus functions.
**
**           It is implemented this way to improve type safety and help ensure that "MsgKey"
**           values are not inadvertently exchanged with MsgId or Routing Index values.
**
*/
typedef struct
{
    CFE_SB_MsgKey_Atom_t KeyIdx;    /**< Holding value, do not use directly */
} CFE_SB_MsgKey_t;

/******************************************************************************/
/**
 * @brief An wrapper for holding a routing table index
 *
 * This is intended as a form of "strong typedef" where direct assignments should
 * be restricted.  Software bus uses numeric indexes into multiple tables to perform
 * its duties, and it is important that these index values are distinct and separate
 * and not mixed together.
 *
 * Using this holding structure prevents assignment directly into a different index
 * or direct usage as numeric value.
 */
typedef struct
{
    CFE_SB_MsgRouteIdx_Atom_t RouteIdx;     /**< Holding value, do not use directly in code */
} CFE_SB_MsgRouteIdx_t;


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
     uint32            Size;
     void              *Buffer;
     CFE_SB_SenderId_t Sender;
} CFE_SB_BufferD_t;


/******************************************************************************
**  Typedef:  CFE_SB_DestinationD_t
**
**  Purpose:
**     This structure defines a DESTINATION DESCRIPTOR used to specify
**     each destination pipe for a message.
**
**     Note: Changing the size of this structure may require the memory pool
**     block sizes to change.
*/

typedef struct {
     CFE_SB_PipeId_t PipeId;
     uint8           Active;
     uint16          MsgId2PipeLim;
     uint16          BuffCount;
     uint16          DestCnt;
     uint8           Scope;
     uint8           Spare[3];
     void            *Prev;
     void            *Next;
} CFE_SB_DestinationD_t;


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
     uint32            AppID;
     uint32            Size;
     void              *Buffer;
     void              *Next;
     void              *Prev;
} CFE_SB_ZeroCopyD_t;


/******************************************************************************
**  Typedef:  CFE_SB_RouteEntry_t
**
**  Purpose:
**     This structure defines an entry in the routing table
*/

typedef struct {
     CFE_SB_MsgId_t        MsgId;    /**< Original Message Id when the subscription was created */
     uint16                Destinations;
     uint32                SeqCnt;
     CFE_SB_DestinationD_t *ListHeadPtr;
} CFE_SB_RouteEntry_t;


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
     char               PipeName[OS_MAX_API_NAME];
     char               AppName[OS_MAX_API_NAME];
     uint8              Opts;
     uint8              Spare;
     uint32             AppId;
     uint32             SysQueueId;
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
typedef struct {
    uint32              SharedDataMutexId;
    uint32              SubscriptionReporting;
    uint32              SenderReporting;
    uint32              AppId;
    uint32              StopRecurseFlags[CFE_PLATFORM_ES_MAX_APPLICATIONS];
    void               *ZeroCopyTail;
    CFE_SB_PipeD_t      PipeTbl[CFE_PLATFORM_SB_MAX_PIPES];
    CFE_SB_HousekeepingTlm_t        HKTlmMsg;
    CFE_SB_StatsTlm_t               StatTlmMsg;
    CFE_SB_PipeId_t     CmdPipe;
    CFE_SB_Msg_t        *CmdPipePktPtr;
    CFE_SB_MemParams_t  Mem;
    CFE_SB_MsgRouteIdx_t      MsgMap[CFE_SB_MAX_NUMBER_OF_MSG_KEYS];
    CFE_SB_RouteEntry_t RoutingTbl[CFE_PLATFORM_SB_MAX_MSG_IDS];
    CFE_SB_AllSubscriptionsTlm_t    PrevSubMsg;
    CFE_SB_SingleSubscriptionTlm_t  SubRprtMsg;
    CFE_EVS_BinFilter_t EventFilters[CFE_SB_MAX_CFG_FILE_EVENTS_TO_FILTER];

    uint16 RouteIdxTop;
    CFE_SB_MsgRouteIdx_t RouteIdxStack[CFE_PLATFORM_SB_MAX_MSG_IDS];

}cfe_sb_t;


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
void   CFE_SB_InitMsgMap(void);
void   CFE_SB_InitRoutingTbl(void);
void   CFE_SB_InitIdxStack(void);
void   CFE_SB_ResetCounts(void);
void  CFE_SB_RouteIdxPush_Unsync(CFE_SB_MsgRouteIdx_t idx);
CFE_SB_MsgRouteIdx_t  CFE_SB_RouteIdxPop_Unsync(void);
CFE_SB_MsgKey_t CFE_SB_ConvertMsgIdtoMsgKey(CFE_SB_MsgId_t MsgId);
void   CFE_SB_LockSharedData(const char *FuncName, int32 LineNumber);
void   CFE_SB_UnlockSharedData(const char *FuncName, int32 LineNumber);
void   CFE_SB_ReleaseBuffer (CFE_SB_BufferD_t *bd, CFE_SB_DestinationD_t *dest);
int32  CFE_SB_ReadQueue(CFE_SB_PipeD_t *pd,uint32 TskId,
                        CFE_SB_TimeOut_t time_out,CFE_SB_BufferD_t **message );
int32  CFE_SB_WriteQueue(CFE_SB_PipeD_t *pd,uint32 TskId,
                         const CFE_SB_BufferD_t *bd,CFE_SB_MsgId_t MsgId );
CFE_SB_MsgRouteIdx_t CFE_SB_GetRoutingTblIdx(CFE_SB_MsgKey_t MsgKey);
uint8  CFE_SB_GetPipeIdx(CFE_SB_PipeId_t PipeId);
int32  CFE_SB_ReturnBufferToPool(CFE_SB_BufferD_t *bd);
void   CFE_SB_ProcessCmdPipePkt(void);
int32  CFE_SB_DuplicateSubscribeCheck(CFE_SB_MsgKey_t MsgKey,CFE_SB_PipeId_t PipeId);
void   CFE_SB_SetRoutingTblIdx(CFE_SB_MsgKey_t MsgKey, CFE_SB_MsgRouteIdx_t Value);
CFE_SB_RouteEntry_t* CFE_SB_GetRoutePtrFromIdx(CFE_SB_MsgRouteIdx_t RouteIdx);
void   CFE_SB_ResetCounters(void);
char   *CFE_SB_GetPipeName(CFE_SB_PipeId_t PipeId);
void   CFE_SB_SetMsgSeqCnt(CFE_SB_MsgPtr_t MsgPtr,uint32 Count);
char   *CFE_SB_GetAppTskName(uint32 TaskId, char* FullName);
CFE_SB_BufferD_t *CFE_SB_GetBufferFromPool(CFE_SB_MsgId_t MsgId, uint16 size);
CFE_SB_BufferD_t *CFE_SB_GetBufferFromCaller(CFE_SB_MsgId_t MsgId, void *Address);
CFE_SB_PipeD_t   *CFE_SB_GetPipePtr(CFE_SB_PipeId_t PipeId);
CFE_SB_PipeId_t  CFE_SB_GetAvailPipeIdx(void);
CFE_SB_DestinationD_t *CFE_SB_GetDestPtr (CFE_SB_MsgKey_t MsgKey, CFE_SB_PipeId_t PipeId);
int32 CFE_SB_DeletePipeWithAppId(CFE_SB_PipeId_t PipeId,uint32 AppId);
int32 CFE_SB_DeletePipeFull(CFE_SB_PipeId_t PipeId,uint32 AppId);
int32 CFE_SB_SubscribeFull(CFE_SB_MsgId_t   MsgId,
                           CFE_SB_PipeId_t  PipeId,
                           CFE_SB_Qos_t     Quality,
                           uint16           MsgLim,
                           uint8            Scope);

int32 CFE_SB_UnsubscribeWithAppId(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId,
                                   uint32 AppId);

int32 CFE_SB_UnsubscribeFull(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId,
                              uint8 Scope, uint32 AppId);
int32  CFE_SB_SendMsgFull(CFE_SB_Msg_t   *MsgPtr, uint32 TlmCntIncrements, uint32 CopyMode);
int32 CFE_SB_SendRtgInfo(const char *Filename);
int32 CFE_SB_SendPipeInfo(const char *Filename);
int32 CFE_SB_SendMapInfo(const char *Filename);
int32 CFE_SB_ZeroCopyReleaseDesc(CFE_SB_Msg_t *Ptr2Release, CFE_SB_ZeroCopyHandle_t BufferHandle);
int32 CFE_SB_ZeroCopyReleaseAppId(uint32         AppId);
int32 CFE_SB_DecrBufUseCnt(CFE_SB_BufferD_t *bd);
int32 CFE_SB_ValidateMsgId(CFE_SB_MsgId_t MsgId);
int32 CFE_SB_ValidatePipeId(CFE_SB_PipeId_t PipeId);
uint8 CFE_SB_GetPktType(CFE_SB_MsgId_t MsgId);
void CFE_SB_IncrCmdCtr(int32 status);
void CFE_SB_FileWriteByteCntErr(const char *Filename,uint32 Requested,uint32 Actual);
void CFE_SB_SetSubscriptionReporting(uint32 state);
uint32 CFE_SB_FindGlobalMsgIdCnt(void);
uint32 CFE_SB_RequestToSendEvent(uint32 TaskId, uint32 Bit);
void CFE_SB_FinishSendEvent(uint32 TaskId, uint32 Bit);
CFE_SB_DestinationD_t *CFE_SB_GetDestinationBlk(void);
int32 CFE_SB_PutDestinationBlk(CFE_SB_DestinationD_t *Dest);
int32 CFE_SB_AddDest(CFE_SB_RouteEntry_t *RouteEntry, CFE_SB_DestinationD_t *Dest);
int32 CFE_SB_RemoveDest(CFE_SB_RouteEntry_t *RouteEntry, CFE_SB_DestinationD_t *Dest);


/*****************************************************************************/
/** 
** \brief Get the size of a software bus message header.
**
** \par Description
**          This routine returns the number of bytes in a software bus message header.  
**          This can be used for sizing buffers that need to store SB messages.  SB 
**          message header formats can be different for each deployment of the cFE.  
**          So, applications should use this function and avoid hard coding their buffer 
**          sizes.
**
** \par Assumptions, External Events, and Notes:
**          - For statically defined messages, a function call will not work.  The 
**            macros #CFE_SB_CMD_HDR_SIZE and #CFE_SB_TLM_HDR_SIZE are available for use 
**            in static message buffer sizing or structure definitions.  
**
** \param[in]  *MsgPtr The message ID to calculate header size for.  The size of the message 
**                     header may depend on the MsgId in some implementations.  For example, 
**                     if SB messages are implemented as CCSDS packets, the size of the header 
**                     is different for command vs. telemetry packets.
**
** \returns
** \retstmt The number of bytes in the software bus message header for 
**          messages with the given \c MsgId. endstmt
** \endreturns
**
** \sa #CFE_SB_GetUserData, #CFE_SB_GetMsgId, #CFE_SB_GetUserDataLength, #CFE_SB_GetTotalMsgLength,
**     #CFE_SB_GetMsgTime, #CFE_SB_GetCmdCode, #CFE_SB_GetChecksum 
**/
uint16 CFE_SB_MsgHdrSize(const CFE_SB_Msg_t *MsgPtr);


/*
 * Software Bus Message Handler Function prototypes
 */
int32 CFE_SB_NoopCmd(const CFE_SB_Noop_t *data);
int32 CFE_SB_ResetCountersCmd(const CFE_SB_ResetCounters_t *data);
int32 CFE_SB_EnableSubReportingCmd(const CFE_SB_EnableSubReporting_t *data);
int32 CFE_SB_DisableSubReportingCmd(const CFE_SB_DisableSubReporting_t *data);
int32 CFE_SB_SendHKTlmCmd(const CCSDS_CommandPacket_t *data);
int32 CFE_SB_EnableRouteCmd(const CFE_SB_EnableRoute_t *data);
int32 CFE_SB_DisableRouteCmd(const CFE_SB_DisableRoute_t *data);
int32 CFE_SB_SendStatsCmd(const CFE_SB_SendSbStats_t *data);
int32 CFE_SB_SendRoutingInfoCmd(const CFE_SB_SendRoutingInfo_t *data);
int32 CFE_SB_SendPipeInfoCmd(const CFE_SB_SendPipeInfo_t *data);
int32 CFE_SB_SendMapInfoCmd(const CFE_SB_SendMapInfo_t *data);
int32 CFE_SB_SendPrevSubsCmd(const CFE_SB_SendPrevSubs_t *data);


/*
 * External variables private to the software bus module
 */

extern cfe_sb_t CFE_SB;



/* ---------------------------------------------------------
 * HELPER FUNCTIONS FOR TYPE-SAFE WRAPPERS / HOLDING STRUCTS
 *
 * These functions implement the type conversions between "bare numbers" and
 * the holding structures, as well as sanity tests for the holding structures.
 *
 * The data within the holding structures should never be directly in the app,
 * one of these helpers should be used once it is verified that the conversion
 * or use case is legitimate.
 * --------------------------------------------------------- */

/**
 * @brief Identifies whether a given CFE_SB_MsgId_t is valid
 *
 * Implements a basic sanity check on the value provided
 *
 * @returns true if sanity checks passed, false otherwise.
 */
static inline bool CFE_SB_IsValidMsgId(CFE_SB_MsgId_t MsgId)
{
    /* cppcheck-suppress redundantCondition */
    return (MsgId != CFE_SB_INVALID_MSG_ID && MsgId <= CFE_PLATFORM_SB_HIGHEST_VALID_MSGID);
}

/**
 * @brief Identifies whether a given CFE_SB_MsgKey_t is valid
 *
 * Implements a basic sanity check on the value provided
 *
 * @returns true if sanity checks passed, false otherwise.
 */
static inline bool CFE_SB_IsValidMsgKey(CFE_SB_MsgKey_t MsgKey)
{
    return (MsgKey.KeyIdx != 0 && MsgKey.KeyIdx <= CFE_SB_MAX_NUMBER_OF_MSG_KEYS);
}

/**
 * @brief Identifies whether a given CFE_SB_MsgRouteIdx_t is valid
 *
 * Implements a basic sanity check on the value provided
 *
 * @returns true if sanity checks passed, false otherwise.
 */
static inline bool CFE_SB_IsValidRouteIdx(CFE_SB_MsgRouteIdx_t RouteIdx)
{
    return (RouteIdx.RouteIdx != 0 && RouteIdx.RouteIdx <= CFE_PLATFORM_SB_MAX_MSG_IDS);
}

/**
 * @brief Converts between a CFE_SB_MsgKey_t and a raw value
 *
 * Converts the supplied value into a "bare number" suitable for performing
 * array lookups or other tasks for which the holding structure cannot be used directly.
 *
 * Use with caution, as this removes the type safety information from the value.
 *
 * @note It is assumed the value has already been validated using CFE_SB_IsValidMsgKey()
 *
 * @returns The underlying index value
 */
static inline CFE_SB_MsgKey_Atom_t CFE_SB_MsgKeyToValue(CFE_SB_MsgKey_t MsgKey)
{
    return (MsgKey.KeyIdx - 1);
}

/**
 * @brief Converts between a CFE_SB_MsgKey_t and a raw value
 *
 * Converts the supplied "bare number" into a type-safe CFE_SB_MsgKey_t value
 *
 * @returns A CFE_SB_MsgKey_t value
 */
static inline CFE_SB_MsgKey_t CFE_SB_ValueToMsgKey(CFE_SB_MsgKey_Atom_t KeyIdx)
{
    return ((CFE_SB_MsgKey_t){ .KeyIdx = 1 + KeyIdx });
}

/**
 * @brief Converts between a CFE_SB_MsgRouteIdx_t and a raw value
 *
 * Converts the supplied "bare number" into a type-safe CFE_SB_MsgRouteIdx_t value
 *
 * @returns A CFE_SB_MsgRouteIdx_t value
 */
static inline CFE_SB_MsgRouteIdx_t CFE_SB_ValueToRouteIdx(CFE_SB_MsgRouteIdx_Atom_t TableIdx)
{
    return ((CFE_SB_MsgRouteIdx_t){ .RouteIdx = 1 + TableIdx });
}

/**
 * @brief Converts between a CFE_SB_MsgRouteIdx_t and a raw value
 *
 * Converts the supplied value into a "bare number" suitable for performing
 * array lookups or other tasks for which the holding structure cannot be used directly.
 *
 * Use with caution, as this removes the type safety information from the value.
 *
 * @note It is assumed the value has already been validated using CFE_SB_IsValidRouteIdx()
 *
 * @returns The underlying index value
 */
static inline CFE_SB_MsgRouteIdx_Atom_t CFE_SB_RouteIdxToValue(CFE_SB_MsgRouteIdx_t RouteIdx)
{
    return (RouteIdx.RouteIdx - 1);
}


#endif /* _cfe_sb_priv_ */
/*****************************************************************************/
