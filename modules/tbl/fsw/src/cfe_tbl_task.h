/************************************************************************
 * NASA Docket No. GSC-19,200-1, and identified as "cFS Draco"
 *
 * Copyright (c) 2023 United States Government as represented by the
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
 * Purpose:  cFE Table Services (TBL) task header file
 *
 * Author:   David Kobe (the Hammers Company, Inc.)
 *
 * Notes:
 *
 */

#ifndef CFE_TBL_TASK_H
#define CFE_TBL_TASK_H

/*
** Required header files
*/
#include "cfe_tbl_msg.h"

#include "cfe_tbl_accdesc.h"
#include "cfe_tbl_dumpctl.h"
#include "cfe_tbl_valresult.h"
#include "cfe_tbl_regrec.h"
#include "cfe_tbl_loadbuff.h"

/*************************************************************************/

/*
** Registry mutex definitions
*/
/** \name Registry Mutex Definitions */
/**  \{ */
#define CFE_TBL_MUT_REG_NAME   "TBL_REG_MUT" /**< \brief Name of Mutex controlling Registry Access */
#define CFE_TBL_MUT_REG_VALUE  0             /**< \brief Initial Value of Registry Access Mutex */
#define CFE_TBL_MUT_WORK_NAME  "TBL_WRK_MUT" /**< \brief Name of Mutex controlling Working Buffer Assignment */
#define CFE_TBL_MUT_WORK_VALUE 0             /**< \brief Initial Value of Working Buffer Assignment Mutex */
/** \} */

/** \name Table Services Task Pipe Characteristics */
/**  \{ */
#define CFE_TBL_TASK_PIPE_NAME  "TBL_CMD_PIPE" /**< \brief Name of TBL Task Command Pipe */
#define CFE_TBL_TASK_PIPE_DEPTH 12             /**< \brief Number of Commands that can be queued */
/** \} */

/*******************************************************************************/
/**   \brief Memory Pool Data Structure
**
**     This structure defines the variables related to the TBL buffers.
*/
typedef struct
{
    CFE_ES_MemHandle_t PoolHdl;
    CFE_ES_STATIC_POOL_TYPE(CFE_PLATFORM_TBL_BUF_MEMORY_BYTES) Partition;
} CFE_TBL_BufParams_t;

/*******************************************************************************/
/**   \brief Critical Table Registry Record
**
**     Critical Table Registry Record that contains information about a Critical
**     Table that must survive the reboot and repopulation of the Table Registry.
*/
typedef struct
{
    CFE_ES_CDSHandle_t CDSHandle;        /**< \brief Handle to Critical Data Store for Critical Tables */
    CFE_TIME_SysTime_t FileTime;         /**< \brief File creation time from last file loaded into table */
    CFE_TIME_SysTime_t TimeOfLastUpdate; /**< \brief Time when Table was last updated */
    char               LastFileLoaded[OS_MAX_PATH_LEN]; /**< \brief Filename of last file loaded into table */
    char               Name[CFE_TBL_MAX_FULL_NAME_LEN]; /**< \brief Processor specific table name */
    bool               TableLoadedOnce; /**< \brief Flag indicating whether table has been loaded once or not */
} CFE_TBL_CritRegRec_t;

/*******************************************************************************/
/**   \brief Table Registry Dump Record
**
**     Shortened Table Registry Record that is used when dumping a table
**     registry entry to a file.
*/
typedef struct
{
    CFE_ES_MemOffset_t Size;             /**< \brief Size, in bytes, of Table */
    CFE_TIME_SysTime_t TimeOfLastUpdate; /**< \brief Time when Table was last updated */
    uint32             NumUsers;         /**< \brief Number of applications that are sharing the table */
    int32              LoadInProgress;   /**< \brief Flag identifies inactive buffer and whether load in progress */
    CFE_TIME_SysTime_t FileTime;         /**< \brief File creation time from last file loaded into table */
    uint32             Crc;              /**< \brief Most recent CRC computed by TBL Services on table contents */
    bool               ValidationFunc;   /**< \brief Flag indicating whether table has an associated Validation func*/
    bool               TableLoadedOnce;  /**< \brief Flag indicating whether table has been loaded once or not */
    bool               LoadPending;      /**< \brief Flag indicating an inactive buffer is ready to be copied */
    bool               DumpOnly;         /**< \brief Flag indicating Table is NOT to be loaded */
    bool               DoubleBuffered;   /**< \brief Flag indicating Table has a dedicated inactive buffer */
    char               Name[CFE_TBL_MAX_FULL_NAME_LEN]; /**< \brief Processor specific table name */
    char               LastFileLoaded[OS_MAX_PATH_LEN]; /**< \brief Filename of last file loaded into table */
    char               OwnerAppName[OS_MAX_API_NAME];   /**< \brief Application Name of App that Registered Table */
    bool               CriticalTable;                   /**< \brief Identifies whether table is Critical or Not */
} CFE_TBL_RegDumpRec_t;

/*******************************************************************************/
/**   \brief Table Registry Dump background state information
**
**    State info for background table registry dump process and one temporary data record.
*/
typedef struct
{
    CFE_FS_FileWriteMetaData_t FileWrite; /**< FS state data - must be first */

    bool                 FileExisted; /**< Set true if the file already existed at the time of request */
    CFE_TBL_RegDumpRec_t DumpRecord;  /**< Current record buffer (reused each entry) */
} CFE_TBL_RegDumpStateInfo_t;

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
    uint8 CommandCounter;      /**< \brief Counts number of valid commands received */
    uint8 CommandErrorCounter; /**< \brief Counts number of invalid commands received */

    /*
    ** Table Validation Result counters...
    */
    uint8 SuccessValCounter; /**< \brief Counts number of successful table validations */
    uint8 FailedValCounter;  /**< \brief Counts number of unsuccessful table validations */
    uint8 NumValRequests;    /**< \brief Counts number of table validation requests made */

    /*
    ** Ground Interface Information
    */
    CFE_TBL_RegId_t LastTblUpdated; /**< \brief ID of last table updated */
    CFE_TBL_RegId_t HkTlmTblRegId;  /**< \brief ID of table registry entry to be telemetered with Housekeeping */

    /*
    ** Task housekeeping and diagnostics telemetry packets...
    */
    CFE_TBL_HousekeepingTlm_t  HkPacket;     /**< \brief Housekeeping Telemetry Packet */
    CFE_TBL_TableRegistryTlm_t TblRegPacket; /**< \brief Table Registry Entry Telemetry Packet */
    CFE_TBL_NotifyCmd_t        NotifyMsg;    /**< \brief Table management notification command message */

    /*
    ** Task operational data (not reported in housekeeping)...
    */
    CFE_SB_PipeId_t CmdPipe; /**< \brief Table Task command pipe ID as obtained from Software Bus */

    /*
    ** Task initialization data (not reported in housekeeping)...
    */
    CFE_ES_AppId_t TableTaskAppId; /**< \brief Contains Table Task Application ID as assigned by OS AL */

    uint16 ValidationCounter;

    /*
    ** Registry Access Mutex and Load Buffer Semaphores
    */
    osal_id_t          RegistryMutex; /**< \brief Mutex that controls access to Table Registry */
    osal_id_t          WorkBufMutex;  /**< \brief Mutex that controls assignment of Working Buffers */
    CFE_ES_CDSHandle_t CritRegHandle; /**< \brief Handle to Critical Table Registry in CDS */
    CFE_TBL_LoadBuff_t LoadBuffs[CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS]; /**< \brief Working table buffers
                                                                              shared by single buffered tables */

    /*
    ** Registry Data
    */
    CFE_TBL_AccessDescriptor_t Handles[CFE_PLATFORM_TBL_MAX_NUM_HANDLES]; /**< \brief Array of Access Descriptors */
    CFE_TBL_RegistryRec_t      Registry[CFE_PLATFORM_TBL_MAX_NUM_TABLES]; /**< \brief Array of Table Registry Records */
    CFE_TBL_CritRegRec_t
        CritReg[CFE_PLATFORM_TBL_MAX_CRITICAL_TABLES]; /**< \brief Array of Critical Table Registry Records */
    CFE_TBL_BufParams_t Buf; /**< \brief Parameters associated with Table Task's Memory Pool */
    CFE_TBL_ValidationResult_t
        ValidationResults[CFE_PLATFORM_TBL_MAX_NUM_VALIDATIONS]; /**< \brief Array of Table Validation Requests */
    CFE_TBL_DumpControl_t DumpControlBlocks[CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS]; /**< \brief Array of Dump-Only
                                                                                         Dump Control Blocks */

    /*
     * Registry dump state info (background job)
     */
    CFE_TBL_RegDumpStateInfo_t RegDumpState;

    CFE_ResourceId_t LastHandle;
    CFE_ResourceId_t LastRegId;
    CFE_ResourceId_t LastLoadBuffId;
    CFE_ResourceId_t LastValidationResultId;
    CFE_ResourceId_t LastDumpCtrlBlockId;

} CFE_TBL_Global_t;

/*************************************************************************/
/*
 * Functions
 */

/*---------------------------------------------------------------------------------------*/
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
*/
int32 CFE_TBL_TaskInit(void);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Table Service Application Data Initialization
**
** \par Description
**          Initializes all data necessary for the Table Service Application.
**
** \par Assumptions, External Events, and Notes:
**          None
**
*/
void CFE_TBL_InitData(void);

#endif /* CFE_TBL_TASK_H */
