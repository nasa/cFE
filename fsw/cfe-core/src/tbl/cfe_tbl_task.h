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
** File: cfe_tbl_task.h
**
** Purpose:  cFE Table Services (TBL) task header file
**
** Author:   David Kobe (the Hammers Company, Inc.)
**
** Notes:
**
*/

/*
** Ensure that header is included only once...
*/
#ifndef _cfe_tbl_task_
#define _cfe_tbl_task_


/*
** Required header files
*/
#include "private/cfe_private.h"
#include "cfe_tbl_events.h"
#include "cfe_tbl_msg.h"


/*************************************************************************/

/*
** Registry mutex definitions
*/
/** \name Registry Mutex Definitions */
/**  \{ */
#define CFE_TBL_MUT_REG_NAME       "TBL_REG_MUT"  /**< \brief Name of Mutex controlling Registry Access */
#define CFE_TBL_MUT_REG_VALUE       0             /**< \brief Initial Value of Registry Access Mutex */
#define CFE_TBL_MUT_WORK_NAME      "TBL_WRK_MUT"  /**< \brief Name of Mutex controlling Working Buffer Assignment */ 
#define CFE_TBL_MUT_WORK_VALUE      0             /**< \brief Initial Value of Working Buffer Assignment Mutex */
/** \} */

/** \name Table Services Task Pipe Characteristics */
/**  \{ */
#define CFE_TBL_TASK_PIPE_NAME     "TBL_CMD_PIPE"  /**< \brief Name of TBL Task Command Pipe */
#define CFE_TBL_TASK_PIPE_DEPTH    12              /**< \brief Number of Commands that can be queued */
/** \} */

/** \brief Value indicating when no load is in progress */
/**
**  This macro is used to indicate no Load is in Progress by assigning it to
**  #CFE_TBL_RegistryRec_t::LoadInProgress 
*/ 
#define CFE_TBL_NO_LOAD_IN_PROGRESS   (-1)

/** \brief Value indicating when no Validation is Pending */
/**
**  This macro is used to indicate no Validation is Pending by assigning it to
**  #CFE_TBL_RegistryRec_t::ValidateActiveIndex or #CFE_TBL_RegistryRec_t::ValidateInactiveIndex 
*/ 
#define CFE_TBL_NO_VALIDATION_PENDING (-1) 

/** \brief Value indicating when no Dump is Pending on a Dump-Only Table */
/**
**  This macro is used to indicate no Dump is Pending by assigning it to
**  #CFE_TBL_RegistryRec_t::DumpControlIndex 
*/ 
#define CFE_TBL_NO_DUMP_PENDING (-1) 

/************************  Internal Structure Definitions  *****************************/

/*******************************************************************************/
/**  \brief Identifies the current state of a validation sequence.
*/
typedef enum 
{
    CFE_TBL_VALIDATION_FREE=0,     /**< \brief Validation Result Block is Free */
    CFE_TBL_VALIDATION_PENDING,    /**< \brief Validation Result Block waiting for Application */
    CFE_TBL_VALIDATION_PERFORMED   /**< \brief Validation Result Block contains Validation Results */
} CFE_TBL_ValidationState_t;


/*******************************************************************************/
/**  \brief Identifies the current state of a dump request.
*/
typedef enum 
{
    CFE_TBL_DUMP_FREE=0,     /**< \brief Dump Request Block is Free */
    CFE_TBL_DUMP_PENDING,    /**< \brief Dump Request Block waiting for Application */
    CFE_TBL_DUMP_PERFORMED   /**< \brief Dump Request Block processed by Application */
} CFE_TBL_DumpState_t;


/*******************************************************************************/
/**   \brief Validation Result Block
**
**    This structure holds the data to be returned to the Operator via telemetry
**    on the results of a Validation request.
*/
typedef struct 
{
    CFE_TBL_ValidationState_t State;          /**< \brief Current state of this block of data */
    int32                     Result;         /**< \brief Result returned by Application's Validation function */
    uint32                    CrcOfTable;     /**< \brief Data Integrity Value computed on Table Buffer */
    bool                      ActiveBuffer;   /**< \brief Flag indicating whether Validation is on Active/Inactive Buffer */
    char                      TableName[CFE_TBL_MAX_FULL_NAME_LEN]; /**< \brief Name of Table being Validated */
} CFE_TBL_ValidationResult_t;


/*******************************************************************************/
/**   \brief Memory Pool Data Structure
**
**     This structure defines the variables related to the TBL buffers.
*/
typedef struct 
{
   CFE_ES_MemHandle_t    PoolHdl;
   CFE_ES_STATIC_POOL_TYPE(CFE_PLATFORM_TBL_BUF_MEMORY_BYTES) Partition;
} CFE_TBL_BufParams_t;


/*******************************************************************************/
/**   \brief Load Buffer Description Data
**
**     This structure holds a pointer to a table buffer along with its associated
**     data such as the time from the file that was loaded into the buffer, whether
**     the buffer has been allocated and a string describing the source of the data.
*/
typedef struct 
{
    void          *BufferPtr;                   /**< \brief Pointer to Load Buffer */
    uint32         FileCreateTimeSecs;          /**< \brief File creation time from last file loaded into table */
    uint32         FileCreateTimeSubSecs;       /**< \brief File creation time from last file loaded into table */
    uint32         Crc;                         /**< \brief Last calculated CRC for this buffer's contents */
    bool           Taken;                       /**< \brief Flag indicating whether buffer is in use */
    bool           Validated;                   /**< \brief Flag indicating whether the buffer has been successfully validated */
    char           DataSource[OS_MAX_PATH_LEN]; /**< \brief Source of data put into buffer (filename or memory address) */
} CFE_TBL_LoadBuff_t;


/*******************************************************************************/
/**   \brief Application to Table Access Descriptor
**
**     Table Access Descriptor data structure that contains information necessary
**     to access the table without interfering with other threads.  TblHandles are
**     an index into an array of Access Descriptors, thus identifying a specific 
**     AccessDescriptor for a particular Application for a table.
*/
typedef struct 
{
    CFE_ES_ResourceID_t   AppId;            /**< \brief Application ID to verify access */
    int16                 RegIndex;         /**< \brief Index into Table Registry (a.k.a. - Global Table #) */
    CFE_TBL_Handle_t      PrevLink;         /**< \brief Index of previous access descriptor in linked list */
    CFE_TBL_Handle_t      NextLink;         /**< \brief Index of next access descriptor in linked list */
    bool                  UsedFlag;         /**< \brief Indicates whether this descriptor is being used or not  */
    bool                  LockFlag;         /**< \brief Indicates whether thread is currently accessing table data */
    bool                  Updated;          /**< \brief Indicates table has been updated since last GetAddress call */
    uint8                 BufferIndex;      /**< \brief Index of buffer currently being used */
} CFE_TBL_AccessDescriptor_t;


/*******************************************************************************/
/**   \brief Table Registry Record
**
**     Table Registry Record that contains all information associated
**     with a particular table.
*/
typedef struct 
{
    CFE_ES_ResourceID_t         OwnerAppId;         /**< \brief Application ID of App that Registered Table */
    size_t                      Size;               /**< \brief Size, in bytes, of Table */
    CFE_SB_MsgId_t              NotificationMsgId;  /**< \brief Message ID of an associated management notification message */
    uint32                      NotificationParam;  /**< \brief Parameter of an associated management notification message */
    CFE_TBL_LoadBuff_t          Buffers[2];         /**< \brief Active and Inactive Buffer Pointers */
    CFE_TBL_CallbackFuncPtr_t   ValidationFuncPtr;  /**< \brief Ptr to Owner App's function that validates tbl contents */
    CFE_TIME_SysTime_t          TimeOfLastUpdate;   /**< \brief Time when Table was last updated */
    CFE_TBL_Handle_t            HeadOfAccessList;   /**< \brief Index into Handles Array that starts Access Linked List */
    int32                       LoadInProgress;     /**< \brief Flag identifies inactive buffer and whether load in progress */
    int32                       ValidateActiveIndex;    /**< \brief Index to Validation Request on Active Table Result data */
    int32                       ValidateInactiveIndex;  /**< \brief Index to Validation Request on Inactive Table Result data */
    int32                       DumpControlIndex;       /**< \brief Index to Dump Control Block */
    CFE_ES_CDSHandle_t          CDSHandle;          /**< \brief Handle to Critical Data Store for Critical Tables */
    CFE_MSG_FcnCode_t           NotificationCC;     /**< \brief Command Code of an associated management notification message */
    bool                        CriticalTable;      /**< \brief Flag indicating whether table is a Critical Table */
    bool                        TableLoadedOnce;    /**< \brief Flag indicating whether table has been loaded once or not */
    bool                        LoadPending;        /**< \brief Flag indicating an inactive buffer is ready to be copied */
    bool                        DumpOnly;           /**< \brief Flag indicating Table is NOT to be loaded */
    bool                        DoubleBuffered;        /**< \brief Flag indicating Table has a dedicated inactive buffer */
    bool                        UserDefAddr;        /**< \brief Flag indicating Table address was defined by Owner Application */
    bool                        NotifyByMsg;        /**< \brief Flag indicating Table Services should notify owning App via message
                                                                when table requires management */ 
    uint8                       ActiveBufferIndex;  /**< \brief Index identifying which buffer is the active buffer */
    char                        Name[CFE_TBL_MAX_FULL_NAME_LEN];   /**< \brief Processor specific table name */
    char                        LastFileLoaded[OS_MAX_PATH_LEN];   /**< \brief Filename of last file loaded into table */
} CFE_TBL_RegistryRec_t;


/*******************************************************************************/
/**   \brief Critical Table Registry Record
**
**     Critical Table Registry Record that contains information about a Critical
**     Table that must survive the reboot and repopulation of the Table Registry.
*/
typedef struct 
{
    CFE_ES_CDSHandle_t          CDSHandle;              /**< \brief Handle to Critical Data Store for Critical Tables */
    uint32                      FileCreateTimeSecs;     /**< \brief File creation time from last file loaded into table */
    uint32                      FileCreateTimeSubSecs;  /**< \brief File creation time from last file loaded into table */
    CFE_TIME_SysTime_t          TimeOfLastUpdate;       /**< \brief Time when Table was last updated */
    char                        LastFileLoaded[OS_MAX_PATH_LEN];   /**< \brief Filename of last file loaded into table */
    char                        Name[CFE_TBL_MAX_FULL_NAME_LEN];   /**< \brief Processor specific table name */
    bool                        TableLoadedOnce;        /**< \brief Flag indicating whether table has been loaded once or not */
} CFE_TBL_CritRegRec_t;


/*******************************************************************************/
/**   \brief Dump Control Block
**
**    This structure holds the data associated with a dump request.
*/
typedef struct 
{
    CFE_TBL_DumpState_t       State;          /**< \brief Current state of this block of data */
    size_t                    Size;           /**< \brief Number of bytes to be dumped */
    CFE_TBL_LoadBuff_t       *DumpBufferPtr;  /**< \brief Address where dumped data is to be stored temporarily */
    CFE_TBL_RegistryRec_t    *RegRecPtr;      /**< \brief Ptr to dumped table's registry record */
    char                      TableName[CFE_TBL_MAX_FULL_NAME_LEN]; /**< \brief Name of Table being Dumped */
} CFE_TBL_DumpControl_t;


/*******************************************************************************/
/**   \brief Table Registry Dump Record
**
**     Shortened Table Registry Record that is used when dumping a table
**     registry entry to a file.
*/
typedef struct 
{
    CFE_ES_MemOffset_t          Size;               /**< \brief Size, in bytes, of Table */
    CFE_TIME_SysTime_t          TimeOfLastUpdate;   /**< \brief Time when Table was last updated */
    uint32                      NumUsers;           /**< \brief Number of applications that are sharing the table */
    int32                       LoadInProgress;     /**< \brief Flag identifies inactive buffer and whether load in progress */
    uint32                      FileCreateTimeSecs;    /**< \brief File creation time from last file loaded into table */
    uint32                      FileCreateTimeSubSecs; /**< \brief File creation time from last file loaded into table */
    uint32                      Crc;                /**< \brief Most recent CRC computed by TBL Services on table contents */
    bool                        ValidationFunc;     /**< \brief Flag indicating whether table has an associated Validation func*/
    bool                        TableLoadedOnce;    /**< \brief Flag indicating whether table has been loaded once or not */
    bool                        LoadPending;        /**< \brief Flag indicating an inactive buffer is ready to be copied */
    bool                        DumpOnly;           /**< \brief Flag indicating Table is NOT to be loaded */
    bool                        DoubleBuffered;        /**< \brief Flag indicating Table has a dedicated inactive buffer */
    char                        Name[CFE_TBL_MAX_FULL_NAME_LEN];   /**< \brief Processor specific table name */
    char                        LastFileLoaded[OS_MAX_PATH_LEN];   /**< \brief Filename of last file loaded into table */
    char                        OwnerAppName[OS_MAX_API_NAME];     /**< \brief Application Name of App that Registered Table */
    bool                     CriticalTable;                     /**< \brief Identifies whether table is Critical or Not */
} CFE_TBL_RegDumpRec_t;

/*******************************************************************************/
/**   \brief Table Task Global Data
**
**     Structure used to ensure Table Task Global Data is maintained as a single
**     block of memory.  This improves Table Maintenance by simplifying the memory
**     map and helps to keep the code in an "object oriented" style.
*/
typedef struct
{
  /*
  ** Task command interface counters...
  */
  uint8                  CommandCounter;                      /**< \brief Counts number of valid commands received */
  uint8                  CommandErrorCounter;                      /**< \brief Counts number of invalid commands received */

  /*
  ** Table Validation Result counters...
  */
  uint8                  SuccessValCounter;                   /**< \brief Counts number of successful table validations */
  uint8                  FailedValCounter;                    /**< \brief Counts number of unsuccessful table validations */
  uint8                  NumValRequests;                  /**< \brief Counts number of table validation requests made */

  /*
  ** Ground Interface Information
  */
  int16                  LastTblUpdated;                  /**< \brief Index into Registry of last table updated */

  /*
  ** Task housekeeping and diagnostics telemetry packets...
  */
  CFE_TBL_HousekeepingTlm_t       HkPacket;               /**< \brief Housekeping Telemetry Packet */
  CFE_TBL_TableRegistryTlm_t      TblRegPacket;           /**< \brief Table Registry Entry Telemetry Packet */
  CFE_TBL_NotifyCmd_t    NotifyMsg;                       /**< \brief Table management notification command message */

  /*
  ** Task operational data (not reported in housekeeping)...
  */
  CFE_SB_PipeId_t        CmdPipe;                         /**< \brief Table Task command pipe ID as obtained from Software Bus */

  /*
  ** Task initialization data (not reported in housekeeping)...
  */
  char                   PipeName[16];                    /**< \brief Contains name of Table Task command pipe */
  uint16                 PipeDepth;                       /**< \brief Contains depth of Table Task command pipe */
  CFE_ES_ResourceID_t    TableTaskAppId;                  /**< \brief Contains Table Task Application ID as assigned by OS AL */

  int16                  HkTlmTblRegIndex;                /**< \brief Index of table registry entry to be telemetered with Housekeeping */
  uint16                 ValidationCounter;

  /*
  ** Registry Access Mutex and Load Buffer Semaphores
  */
  osal_id_t              RegistryMutex;                   /**< \brief Mutex that controls access to Table Registry */
  osal_id_t              WorkBufMutex;                    /**< \brief Mutex that controls assignment of Working Buffers */
  CFE_ES_CDSHandle_t     CritRegHandle;                   /**< \brief Handle to Critical Table Registry in CDS */
  CFE_TBL_LoadBuff_t     LoadBuffs[CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS];  /**< \brief Working table buffers shared by single buffered tables */

  /*
  ** Registry Data
  */
  CFE_TBL_AccessDescriptor_t  Handles[CFE_PLATFORM_TBL_MAX_NUM_HANDLES];  /**< \brief Array of Access Descriptors */
  CFE_TBL_RegistryRec_t       Registry[CFE_PLATFORM_TBL_MAX_NUM_TABLES];  /**< \brief Array of Table Registry Records */
  CFE_TBL_CritRegRec_t        CritReg[CFE_PLATFORM_TBL_MAX_CRITICAL_TABLES]; /**< \brief Array of Critical Table Registry Records */
  CFE_TBL_BufParams_t         Buf;                               /**< \brief Parameters associated with Table Task's Memory Pool */
  CFE_TBL_ValidationResult_t  ValidationResults[CFE_PLATFORM_TBL_MAX_NUM_VALIDATIONS]; /**< \brief Array of Table Validation Requests */
  CFE_TBL_DumpControl_t       DumpControlBlocks[CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS]; /**< \brief Array of Dump-Only Dump Control Blocks */

} CFE_TBL_TaskData_t;


/*************************************************************************/
/*
 * Functions
 */

/*****************************************************************************/
/**
** \brief Compares message with #CFE_TBL_CmdHandlerTbl to identify the message
**
** \par Description
**          Searches the Command Handler Table for an entry matching the
**          message ID and, if necessary, the Command Code.  If an entry
**          is not located, an error code is returned.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in] MessageID message ID of command message received on command pipe
**
** \param[in] CommandCode command code from command message received on command pipe
**
** \retval #CFE_SUCCESS          \copydoc CFE_SUCCESS
** \retval #CFE_TBL_BAD_CMD_CODE \copydoc CFE_TBL_BAD_CMD_CODE
** \retval #CFE_TBL_BAD_MSG_ID   \copydoc CFE_TBL_BAD_MSG_ID
**
******************************************************************************/
extern int16 CFE_TBL_SearchCmdHndlrTbl(CFE_SB_MsgId_t MessageID, uint16 CommandCode);

/*****************************************************************************/
/**
** \brief cFE Table Services Core Application Initialization
**
** \par Description
**        This function initializes all data associated with the cFE Table
**        Services Core Application.  It is only called when the Application
**        is first started.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \return #CFE_SUCCESS  \copydoc CFE_SUCCESS
** \return Any of the return values from #CFE_EVS_Register
** \return Any of the return values from #CFE_SB_CreatePipe
** \return Any of the return values from #CFE_SB_Subscribe
** \return Any of the return values from #CFE_EVS_SendEvent
******************************************************************************/
int32 CFE_TBL_TaskInit(void);

/*****************************************************************************/
/**
** \brief Processes command pipe messages
**
** \par Description
**          Processes messages obtained from the command pipe.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in] MessagePtr a pointer to the message received from the command pipe
**
** 
******************************************************************************/
void  CFE_TBL_TaskPipe(CFE_SB_Buffer_t *SBBufPtr);

/*****************************************************************************/
/**
** \brief Table Service Application Data Initialization
**
** \par Description
**          Initializes all data necessary for the Table Service Application.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** 
******************************************************************************/
void  CFE_TBL_InitData(void);


#endif /* _cfe_tbl_task_ */

/************************/
/*  End of File Comment */
/************************/
