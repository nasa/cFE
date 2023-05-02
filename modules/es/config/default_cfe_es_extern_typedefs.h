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
 * Declarations and prototypes for cfe_es_extern_typedefs module
 */

#ifndef CFE_ES_EXTERN_TYPEDEFS_H
#define CFE_ES_EXTERN_TYPEDEFS_H

#include "common_types.h"
#include "cfe_resourceid_typedef.h"
#include "cfe_mission_cfg.h"

/**
 * @brief Label definitions associated with CFE_ES_LogMode_Enum_t
 */
enum CFE_ES_LogMode
{
    /**
     * @brief Overwrite Log Mode
     */
    CFE_ES_LogMode_OVERWRITE = 0,

    /**
     * @brief Discard Log Mode
     */
    CFE_ES_LogMode_DISCARD = 1
};

/**
 * @brief Identifies handling of log messages after storage is filled
 *
 * @sa enum CFE_ES_LogMode
 */
typedef uint8 CFE_ES_LogMode_Enum_t;

/**
 * @brief Label definitions associated with CFE_ES_ExceptionAction_Enum_t
 */
enum CFE_ES_ExceptionAction
{
    /**
     * @brief Restart application if exception occurs
     */
    CFE_ES_ExceptionAction_RESTART_APP = 0,

    /**
     * @brief Restart processor if exception occurs
     */
    CFE_ES_ExceptionAction_PROC_RESTART = 1
};

/**
 * @brief Identifies action to take if exception occurs
 *
 * @sa enum CFE_ES_ExceptionAction
 */
typedef uint8 CFE_ES_ExceptionAction_Enum_t;

/**
 * @brief Label definitions associated with CFE_ES_AppType_Enum_t
 */
enum CFE_ES_AppType
{
    /**
     * @brief CFE core application
     */
    CFE_ES_AppType_CORE = 1,

    /**
     * @brief CFE external application
     */
    CFE_ES_AppType_EXTERNAL = 2,

    /**
     * @brief CFE library
     */
    CFE_ES_AppType_LIBRARY = 3
};

/**
 * @brief Identifies type of CFE application
 *
 * @sa enum CFE_ES_AppType
 */
typedef uint8 CFE_ES_AppType_Enum_t;

/**
 * @brief Label definitions associated with CFE_ES_RunStatus_Enum_t
 */
enum CFE_ES_RunStatus
{
    /**
     * @brief Reserved value, should not be used
     */
    CFE_ES_RunStatus_UNDEFINED = 0,

    /**
     * @brief Indicates that the Application should continue to run
     */
    CFE_ES_RunStatus_APP_RUN = 1,

    /**
     * @brief Indicates that the Application wants to exit normally
     */
    CFE_ES_RunStatus_APP_EXIT = 2,

    /**
     * @brief Indicates that the Application is quitting with an error
     */
    CFE_ES_RunStatus_APP_ERROR = 3,

    /**
     * @brief The cFE App caused an exception
     */
    CFE_ES_RunStatus_SYS_EXCEPTION = 4,

    /**
     * @brief The system is requesting a restart of the cFE App
     */
    CFE_ES_RunStatus_SYS_RESTART = 5,

    /**
     * @brief The system is requesting a reload of the cFE App
     */
    CFE_ES_RunStatus_SYS_RELOAD = 6,

    /**
     * @brief The system is requesting that the cFE App is stopped
     */
    CFE_ES_RunStatus_SYS_DELETE = 7,

    /**
     * @brief Indicates that the Core Application could not Init
     */
    CFE_ES_RunStatus_CORE_APP_INIT_ERROR = 8,

    /**
     * @brief Indicates that the Core Application had a runtime failure
     */
    CFE_ES_RunStatus_CORE_APP_RUNTIME_ERROR = 9,

    /**
     * @brief Reserved value, marker for the maximum state
     */
    CFE_ES_RunStatus_MAX
};

/**
 * @brief Run Status and Exit Status identifiers
 *
 * @sa enum CFE_ES_RunStatus
 */
typedef uint32 CFE_ES_RunStatus_Enum_t;

/**
 * @brief Label definitions associated with CFE_ES_SystemState_Enum_t
 */
enum CFE_ES_SystemState
{
    /**
     * @brief reserved
     */
    CFE_ES_SystemState_UNDEFINED = 0,

    /**
     * @brief single threaded mode while setting up CFE itself
     */
    CFE_ES_SystemState_EARLY_INIT = 1,

    /**
     * @brief core apps (CFE_ES_ObjectTable) are starting (multi-threaded)
     */
    CFE_ES_SystemState_CORE_STARTUP = 2,

    /**
     * @brief core is ready, starting other external apps/libraries (if any)
     */
    CFE_ES_SystemState_CORE_READY = 3,

    /**
     * @brief startup apps have all completed their early init, but not necessarily operational yet
     */
    CFE_ES_SystemState_APPS_INIT = 4,

    /**
     * @brief normal operation mode; all apps are RUNNING
     */
    CFE_ES_SystemState_OPERATIONAL = 5,

    /**
     * @brief reserved for future use, all apps would be STOPPED
     */
    CFE_ES_SystemState_SHUTDOWN = 6,

    /**
     * @brief Reserved value, marker for the maximum state
     */
    CFE_ES_SystemState_MAX
};

/**
 * @brief The overall cFE System State
 *
 * These values are used with the #CFE_ES_WaitForSystemState API call to synchronize application startup.
 *
 * @note These are defined in order so that relational comparisons e.g. if (STATEA < STATEB) are possible
 *
 * @sa enum CFE_ES_SystemState
 */
typedef uint32 CFE_ES_SystemState_Enum_t;

/**
 * @brief Label definitions associated with CFE_ES_LogEntryType_Enum_t
 */
enum CFE_ES_LogEntryType
{
    /**
     * @brief Log entry from a core subsystem
     */
    CFE_ES_LogEntryType_CORE = 1,

    /**
     * @brief Log entry from an application
     */
    CFE_ES_LogEntryType_APPLICATION = 2
};

/**
 * @brief Type of entry in the Error and Reset (ER) Log
 *
 * @sa enum CFE_ES_LogEntryType
 */
typedef uint8 CFE_ES_LogEntryType_Enum_t;

/**
 * @brief Label definitions associated with CFE_ES_AppState_Enum_t
 */
enum CFE_ES_AppState
{
    /**
     * @brief Initial state before app thread is started
     */
    CFE_ES_AppState_UNDEFINED = 0,

    /**
     * @brief App thread has started, app performing early initialization of its own data
     */
    CFE_ES_AppState_EARLY_INIT = 1,

    /**
     * @brief Early/Local initialization is complete.  First sync point.
     */
    CFE_ES_AppState_LATE_INIT = 2,

    /**
     * @brief All initialization is complete.  Second sync point.
     */
    CFE_ES_AppState_RUNNING = 3,

    /**
     * @brief Application is waiting on a Restart/Reload/Delete request
     */
    CFE_ES_AppState_WAITING = 4,

    /**
     * @brief Application is stopped
     */
    CFE_ES_AppState_STOPPED = 5,

    /**
     * @brief Reserved entry, marker for the maximum state
     */
    CFE_ES_AppState_MAX
};

/**
 * @brief Application Run State
 *
 * The normal progression of APP states:
 * UNDEFINED -> EARLY_INIT -> LATE_INIT -> RUNNING -> WAITING -> STOPPED
 *
 * @note These are defined in order so that relational comparisons e.g. if (STATEA < STATEB) are possible
 *
 * @sa enum CFE_ES_AppState
 */
typedef uint32 CFE_ES_AppState_Enum_t;

/**
 * @brief A type for Application IDs
 *
 * This is the type that is used for any API accepting or returning an App ID
 */
typedef CFE_RESOURCEID_BASE_TYPE CFE_ES_AppId_t;

/**
 * @brief A type for Task IDs
 *
 * This is the type that is used for any API accepting or returning a Task ID
 */
typedef CFE_RESOURCEID_BASE_TYPE CFE_ES_TaskId_t;

/**
 * @brief A type for Library IDs
 *
 * This is the type that is used for any API accepting or returning a Lib ID
 */
typedef CFE_RESOURCEID_BASE_TYPE CFE_ES_LibId_t;

/**
 * @brief A type for Counter IDs
 *
 * This is the type that is used for any API accepting or returning a Counter ID
 */
typedef CFE_RESOURCEID_BASE_TYPE CFE_ES_CounterId_t;

/**
 * @brief Memory Handle type
 *
 * Data type used to hold Handles of Memory Pools
 * created via CFE_ES_PoolCreate and CFE_ES_PoolCreateNoSem
 */
typedef CFE_RESOURCEID_BASE_TYPE CFE_ES_MemHandle_t;

/**
 * @brief CDS Handle type
 *
 * Data type used to hold Handles of Critical Data Stores. See #CFE_ES_RegisterCDS
 */
typedef CFE_RESOURCEID_BASE_TYPE CFE_ES_CDSHandle_t;

/**
 * @brief Type used for task priority in CFE ES as
 * including the commands/telemetry messages.
 *
 * @note the valid range is only 0-255 (same as OSAL) but
 * a wider type is used for backward compatibility
 * in binary formats of messages.
 */
typedef uint16 CFE_ES_TaskPriority_Atom_t;

/**
 * @brief Type used for memory sizes and offsets in commands and telemetry
 *
 * For backward compatibility with existing CFE code this should be uint32,
 * but all telemetry information will be limited to 4GB in size as a result.
 *
 * On 64-bit platforms this can be a 64-bit value which will allow larger
 * memory objects, but this will break compatibility with existing control
 * systems, and may also change the alignment/padding of messages.
 *
 * In either case this must be an unsigned type.
 */
typedef uint32 CFE_ES_MemOffset_t;

/**
 * @brief Memory Offset initializer wrapper
 *
 * A converter macro to use when initializing a CFE_ES_MemOffset_t
 * from an integer value of a different type.
 */
#define CFE_ES_MEMOFFSET_C(x) ((CFE_ES_MemOffset_t)(x))

/**
 * @brief Memory Offset to integer value (size_t) wrapper
 *
 * A converter macro to use when interpreting a CFE_ES_MemOffset_t
 * value as a "size_t" type
 */
#define CFE_ES_MEMOFFSET_TO_SIZET(x) ((size_t)(x))

/**
 * @brief Type used for memory addresses in command and telemetry messages
 *
 * For backward compatibility with existing CFE code this should be uint32,
 * but if running on a 64-bit platform, addresses in telemetry will be
 * truncated to 32 bits and therefore will not be valid.
 *
 * On 64-bit platforms this can be a 64-bit address which will allow the
 * full memory address in commands and telemetry, but this will break
 * compatibility with existing control systems, and may also change
 * the alignment/padding of messages.
 *
 * In either case this must be an unsigned type.
 *
 * FSW code should access this value via the macros provided, which
 * converts to the native "cpuaddr" type provided by OSAL.  This macro
 * provides independence between the message representation and local
 * representation of a memory address.
 */
typedef uint32 CFE_ES_MemAddress_t;

/**
 * @brief Memory Address initializer wrapper
 *
 * A converter macro to use when initializing a CFE_ES_MemAddress_t
 * from a pointer value of a different type.
 */
#define CFE_ES_MEMADDRESS_C(x) ((CFE_ES_MemAddress_t)((cpuaddr)(x)&0xFFFFFFFF))

/**
 * @brief Memory Address to pointer wrapper
 *
 * A converter macro to use when interpreting a CFE_ES_MemAddress_t
 * as a pointer value.
 */
#define CFE_ES_MEMADDRESS_TO_PTR(x) ((void *)(cpuaddr)(x))

/*
 * Data Structures shared between API and Message (CMD/TLM) interfaces
 */

/**
 * \brief Application Information
 *
 * Structure that is used to provide information about an app.
 * It is primarily used for the QueryOne and QueryAll Commands.
 *
 * While this structure is primarily intended for Application info,
 * it can also represent Library information where only a subset of
 * the information applies.
 */
typedef struct CFE_ES_AppInfo
{
    CFE_ResourceId_t ResourceId; /**< \cfetlmmnemonic \ES_APP_ID
                                      \brief Application or Library ID for this resource */
    uint32 Type;                 /**< \cfetlmmnemonic \ES_APPTYPE
                                      \brief The type of App: CORE or EXTERNAL */

    char Name[CFE_MISSION_MAX_API_LEN];       /**< \cfetlmmnemonic \ES_APPNAME
                                                   \brief The Registered Name of the Application */
    char EntryPoint[CFE_MISSION_MAX_API_LEN]; /**< \cfetlmmnemonic \ES_APPENTRYPT
                                                   \brief The Entry Point label for the Application */
    char FileName[CFE_MISSION_MAX_PATH_LEN];  /**< \cfetlmmnemonic \ES_APPFILENAME
                                                   \brief The Filename of the file containing the Application */

    CFE_ES_MemOffset_t StackSize;                  /**< \cfetlmmnemonic \ES_STACKSIZE
                                                        \brief The Stack Size of the Application */
    uint32 AddressesAreValid;                      /**< \cfetlmmnemonic \ES_ADDRVALID
                                                        \brief Indicates that the Code, Data, and BSS addresses/sizes are valid */
    CFE_ES_MemAddress_t CodeAddress;               /**< \cfetlmmnemonic \ES_CODEADDR
                                                        \brief The Address of the Application Code Segment*/
    CFE_ES_MemOffset_t CodeSize;                   /**< \cfetlmmnemonic \ES_CODESIZE
                                                        \brief The Code Size of the Application */
    CFE_ES_MemAddress_t DataAddress;               /**< \cfetlmmnemonic \ES_DATAADDR
                                                        \brief The Address of the Application Data Segment*/
    CFE_ES_MemOffset_t DataSize;                   /**< \cfetlmmnemonic \ES_DATASIZE
                                                        \brief The Data Size of the Application */
    CFE_ES_MemAddress_t BSSAddress;                /**< \cfetlmmnemonic \ES_BSSADDR
                                                        \brief The Address of the Application BSS Segment*/
    CFE_ES_MemOffset_t BSSSize;                    /**< \cfetlmmnemonic \ES_BSSSIZE
                                                        \brief The BSS Size of the Application */
    CFE_ES_MemAddress_t StartAddress;              /**< \cfetlmmnemonic \ES_STARTADDR
                                                        \brief The Start Address of the Application */
    CFE_ES_ExceptionAction_Enum_t ExceptionAction; /**< \cfetlmmnemonic \ES_EXCEPTNACTN
                                                   \brief What should occur if Application has an exception
                                                   (Restart Application OR Restart Processor) */
    CFE_ES_TaskPriority_Atom_t Priority;           /**< \cfetlmmnemonic \ES_PRIORITY
                                                      \brief The Priority of the Application */
    CFE_ES_TaskId_t MainTaskId;                    /**< \cfetlmmnemonic \ES_MAINTASKID
                                                        \brief The Application's Main Task ID */
    uint32 ExecutionCounter;                       /**< \cfetlmmnemonic \ES_MAINTASKEXECNT
                                                        \brief The Application's Main Task Execution Counter */
    char MainTaskName[CFE_MISSION_MAX_API_LEN];    /**< \cfetlmmnemonic \ES_MAINTASKNAME
                                                        \brief The Application's Main Task ID */
    uint32 NumOfChildTasks;                        /**< \cfetlmmnemonic \ES_CHILDTASKS
                                                        \brief Number of Child tasks for an App */
} CFE_ES_AppInfo_t;

/**
 * \brief Task Information
 *
 * Structure that is used to provide information about a task. It is primarily
 * used for the Query All Tasks (#CFE_ES_QUERY_ALL_TASKS_CC) command.
 *
 * \note There is not currently a telemetry message directly containing this
 * data structure, but it does define the format of the data file generated
 * by the Query All Tasks command.  Therefore it should be considered
 * part of the overall telemetry interface.
 */
typedef struct CFE_ES_TaskInfo
{
    CFE_ES_TaskId_t            TaskId;                            /**< \brief Task Id */
    uint32                     ExecutionCounter;                  /**< \brief Task Execution Counter */
    char                       TaskName[CFE_MISSION_MAX_API_LEN]; /**< \brief Task Name */
    CFE_ES_AppId_t             AppId;                             /**< \brief Parent Application ID */
    char                       AppName[CFE_MISSION_MAX_API_LEN];  /**< \brief Parent Application Name */
    CFE_ES_MemOffset_t         StackSize;                         /**< Size of task stack */
    CFE_ES_TaskPriority_Atom_t Priority;                          /**< Priority of task */
    uint8                      Spare[2];                          /**< Spare bytes for alignment */
} CFE_ES_TaskInfo_t;

/**
 * \brief CDS Register Dump Record
 *
 * Structure that is used to provide information about a critical data store.
 * It is primarily used for the Dump CDS registry (#CFE_ES_DUMP_CDS_REGISTRY_CC)
 * command.
 *
 * \note There is not currently a telemetry message directly containing this
 * data structure, but it does define the format of the data file generated
 * by the Dump CDS registry command.  Therefore it should be considered
 * part of the overall telemetry interface.
 */
typedef struct CFE_ES_CDSRegDumpRec
{
    CFE_ES_CDSHandle_t Handle; /**< \brief Handle of CDS */
    CFE_ES_MemOffset_t Size;   /**< \brief Size, in bytes, of the CDS memory block */
    bool               Table;  /**< \brief Flag that indicates whether CDS contains a Critical Table */
    char               Name[CFE_MISSION_ES_CDS_MAX_FULL_NAME_LEN]; /**< \brief Processor Unique Name of CDS */
    uint8              ByteAlignSpare[3]; /**< \brief Spare bytes to ensure structure size is multiple of 4 bytes */
} CFE_ES_CDSRegDumpRec_t;

/**
 * \brief Block statistics
 *
 * Sub-Structure that is used to provide information about a specific
 * block size/bucket within a memory pool.
 */
typedef struct CFE_ES_BlockStats
{
    CFE_ES_MemOffset_t BlockSize;  /**< \brief Number of bytes in each of these blocks */
    uint32             NumCreated; /**< \brief Number of Memory Blocks of this size created */
    uint32             NumFree;    /**< \brief Number of Memory Blocks of this size that are free */
} CFE_ES_BlockStats_t;

/**
 * \brief Memory Pool Statistics
 *
 * Structure that is used to provide information about a memory
 * pool.  Used by the Memory Pool Stats telemetry message.
 *
 * \sa #CFE_ES_SEND_MEM_POOL_STATS_CC
 */
typedef struct CFE_ES_MemPoolStats
{
    CFE_ES_MemOffset_t PoolSize;                                     /**< \cfetlmmnemonic \ES_POOLSIZE
                                                                          \brief  Size of Memory Pool (in bytes) */
    uint32 NumBlocksRequested;                                       /**< \cfetlmmnemonic \ES_BLKSREQ
                                                                          \brief Number of times a memory block has been allocated */
    uint32 CheckErrCtr;                                              /**< \cfetlmmnemonic \ES_BLKERRCTR
                                                                          \brief Number of errors detected when freeing a memory block */
    CFE_ES_MemOffset_t NumFreeBytes;                                 /**< \cfetlmmnemonic \ES_FREEBYTES
                                                                          \brief Number of bytes never allocated to a block */
    CFE_ES_BlockStats_t BlockStats[CFE_MISSION_ES_POOL_MAX_BUCKETS]; /**< \cfetlmmnemonic \ES_BLKSTATS
                                                                          \brief Contains stats on each block size */
} CFE_ES_MemPoolStats_t;

#endif /* CFE_ES_EXTERN_TYPEDEFS_H */
