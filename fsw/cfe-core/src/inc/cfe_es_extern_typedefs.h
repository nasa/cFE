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

#ifndef _CFE_ES_EXTERN_TYPEDEFS_H_
#define _CFE_ES_EXTERN_TYPEDEFS_H_

/* This header may be generated from an EDS file,
 * tools are available and the feature is enabled */
#ifdef CFE_EDS_ENABLED_BUILD

/* Use the EDS generated version of these types */
#include "cfe_es_eds_typedefs.h"

#else
/* Use the local definitions of these types */

#include "common_types.h"

/**
 * @brief Label definitions associated with CFE_ES_LogMode_Enum_t
 */
enum CFE_ES_LogMode
{

   /**
    * @brief Overwrite Log Mode
    */
   CFE_ES_LogMode_OVERWRITE                           = 0,

   /**
    * @brief Discard Log Mode
    */
   CFE_ES_LogMode_DISCARD                             = 1
};

/**
 * @brief Identifies handling of log messages after storage is filled
 *
 *
 * @sa enum CFE_ES_LogMode
 */
typedef uint8                                            CFE_ES_LogMode_Enum_t;


/**
 * @brief Label definitions associated with CFE_ES_ExceptionAction_Enum_t
 */
enum CFE_ES_ExceptionAction
{

   /**
    * @brief Restart application if exception occurs
    */
   CFE_ES_ExceptionAction_RESTART_APP                 = 0,

   /**
    * @brief Restart processor if exception occurs
    */
   CFE_ES_ExceptionAction_PROC_RESTART                = 1
};

/**
 * @brief Identifies action to take if exception occurs
 *
 *
 * @sa enum CFE_ES_ExceptionAction
 */
typedef uint8                                            CFE_ES_ExceptionAction_Enum_t;


/**
 * @brief Label definitions associated with CFE_ES_AppType_Enum_t
 */
enum CFE_ES_AppType
{

   /**
    * @brief CFE core application
    */
   CFE_ES_AppType_CORE                                = 1,

   /**
    * @brief CFE external application
    */
   CFE_ES_AppType_EXTERNAL                            = 2,

   /**
    * @brief CFE library
    */
   CFE_ES_AppType_LIBRARY                             = 3
};

/**
 * @brief Identifies type of CFE application
 *
 *
 * @sa enum CFE_ES_AppType
 */
typedef uint8                                            CFE_ES_AppType_Enum_t;


/**
 * @brief Label definitions associated with CFE_ES_RunStatus_Enum_t
 */
enum CFE_ES_RunStatus
{
    /**
     * @brief Reserved value, should not be used
     */
    CFE_ES_RunStatus_UNDEFINED                        = 0,

   /**
    * @brief Indicates that the Application should continue to run
    */
   CFE_ES_RunStatus_APP_RUN                           = 1,

   /**
    * @brief Indicates that the Application wants to exit normally
    */
   CFE_ES_RunStatus_APP_EXIT                          = 2,

   /**
    * @brief Indicates that the Application is quitting with an error
    */
   CFE_ES_RunStatus_APP_ERROR                         = 3,

   /**
    * @brief The cFE App caused an exception
    */
   CFE_ES_RunStatus_SYS_EXCEPTION                     = 4,

   /**
    * @brief The system is requesting a restart of the cFE App
    */
   CFE_ES_RunStatus_SYS_RESTART                       = 5,

   /**
    * @brief The system is requesting a reload of the cFE App
    */
   CFE_ES_RunStatus_SYS_RELOAD                        = 6,

   /**
    * @brief The system is requesting that the cFE App is stopped
    */
   CFE_ES_RunStatus_SYS_DELETE                        = 7,

   /**
    * @brief Indicates that the Core Application could not Init
    */
   CFE_ES_RunStatus_CORE_APP_INIT_ERROR               = 8,

   /**
    * @brief Indicates that the Core Application had a runtime failure
    */
   CFE_ES_RunStatus_CORE_APP_RUNTIME_ERROR            = 9,

   /**
    * @brief Reserved value, marker for the maximum state
    */
   CFE_ES_RunStatus_MAX

};

/**
 * @brief Run Status and Exit Status identifiers
 *
 *
 * @sa enum CFE_ES_RunStatus
 */
typedef uint32                                           CFE_ES_RunStatus_Enum_t;


/**
 * @brief Label definitions associated with CFE_ES_SystemState_Enum_t
 */
enum CFE_ES_SystemState
{

   /**
    * @brief reserved
    */
   CFE_ES_SystemState_UNDEFINED                       = 0,

   /**
    * @brief single threaded mode while setting up CFE itself
    */
   CFE_ES_SystemState_EARLY_INIT                      = 1,

   /**
    * @brief core apps (CFE_ES_ObjectTable) are starting (multi-threaded)
    */
   CFE_ES_SystemState_CORE_STARTUP                    = 2,

   /**
    * @brief core is ready, starting other external apps/libraries (if any)
    */
   CFE_ES_SystemState_CORE_READY                      = 3,

   /**
    * @brief startup apps have all completed their early init, but not necessarily operational yet
    */
   CFE_ES_SystemState_APPS_INIT                       = 4,

   /**
    * @brief normal operation mode; all apps are RUNNING
    */
   CFE_ES_SystemState_OPERATIONAL                     = 5,

   /**
    * @brief reserved for future use, all apps would be STOPPED
    */
   CFE_ES_SystemState_SHUTDOWN                        = 6,

   /**
    * @brief Reserved value, marker for the maximum state
    */
   CFE_ES_SystemState_MAX
};

/**
 * @brief The overall cFE System State
 *
 *
 * These values are used with the #CFE_ES_WaitForSystemState API call to synchronize application startup.
 *
 * @note These are defined in order so that relational comparisons e.g. if (STATEA < STATEB) are possible
 *
 * @sa enum CFE_ES_SystemState
 */
typedef uint32                                           CFE_ES_SystemState_Enum_t;



/**
 * @brief Label definitions associated with CFE_ES_LogEntryType_Enum_t
 */
enum CFE_ES_LogEntryType
{

   /**
    * @brief Log entry from a core subsystem
    */
   CFE_ES_LogEntryType_CORE                           = 1,

   /**
    * @brief Log entry from an application
    */
   CFE_ES_LogEntryType_APPLICATION                    = 2
};

/**
 * @brief Type of entry in the Error and Reset (ER) Log
 *
 *
 * @sa enum CFE_ES_LogEntryType
 */
typedef uint8                                            CFE_ES_LogEntryType_Enum_t;


/**
 * @brief Label definitions associated with CFE_ES_AppState_Enum_t
 */
enum CFE_ES_AppState
{

   /**
    * @brief Initial state before app thread is started
    */
   CFE_ES_AppState_UNDEFINED                          = 0,

   /**
    * @brief App thread has started, app performing early initialization of its own data
    */
   CFE_ES_AppState_EARLY_INIT                         = 1,

   /**
    * @brief Early/Local initialization is complete.  First sync point.
    */
   CFE_ES_AppState_LATE_INIT                          = 2,

   /**
    * @brief All initialization is complete.  Second sync point.
    */
   CFE_ES_AppState_RUNNING                            = 3,

   /**
    * @brief Application is waiting on a Restart/Reload/Delete request
    */
   CFE_ES_AppState_WAITING                            = 4,

   /**
    * @brief Application is stopped
    */
   CFE_ES_AppState_STOPPED                            = 5,

   /**
    * @brief Reserved entry, marker for the maximum state
    */
   CFE_ES_AppState_MAX
};

/**
 * @brief Application Run State
 *
 *
 * The normal progression of APP states:
 * UNDEFINED -> EARLY_INIT -> LATE_INIT -> RUNNING -> WAITING -> STOPPED
 *
 * @note These are defined in order so that relational comparisons e.g. if (STATEA < STATEB) are possible
 *
 * @sa enum CFE_ES_AppState
 */
typedef uint32                                           CFE_ES_AppState_Enum_t;



/**
 * @brief A type that provides a common, abstract identifier for
 * all ES managed resources (e.g. apps, tasks, counters, etc).
 *
 * Fundamentally an unsigned integer but users should treat it as
 * opaque, and only go through the ES API for introspection.
 *
 * Simple operations are provided as inline functions, which
 * should alleviate the need to do direct manipulation of the value:
 *
 *  - Check for undefined ID value
 *  - Check for equality of two ID values
 *  - Convert ID to simple integer (typically for printing/logging)
 *  - Convert simple integer to ID (inverse of above)
 */
typedef uint32 CFE_ES_ResourceID_t;

/**
 * @brief Memory Handle type
 *
 * Data type used to hold Handles of Memory Pools
 * created via CFE_ES_PoolCreate and CFE_ES_PoolCreateNoSem
 */
typedef CFE_ES_ResourceID_t CFE_ES_MemHandle_t;

/**
 * @brief CDS Handle type
 *
 * Data type used to hold Handles of Critical Data Stores. See #CFE_ES_RegisterCDS
 */
typedef CFE_ES_ResourceID_t CFE_ES_CDSHandle_t;

/**
 * @brief Type used for task priority in CFE ES as
 * including the commands/telemetry messages.
 *
 * @note the valid range is only 0-255 (same as OSAL) but
 * a wider type is used for backward compatibility
 * in binary formats of messages.
 */
typedef uint16  CFE_ES_TaskPriority_Atom_t;

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
typedef uint32  CFE_ES_MemOffset_t;

/*
 * A converter macro to use when initializing an CFE_ES_MemOffset_t
 * from an integer value of a different type.
 */
#define CFE_ES_MEMOFFSET_C(x) ((CFE_ES_MemOffset_t)(x))


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

/*
 * A converter macro to use when initializing an CFE_ES_MemAddress_t
 * from a pointer value of a different type.
 *
 * @note on a 64 bit platform, this macro will truncate the address such
 * that it will fit into a 32-bit telemetry field.  Obviously, the resulting
 * value is no longer usable as a memory address after this.
 */
#define CFE_ES_MEMADDRESS_C(x) ((CFE_ES_MemAddress_t)((cpuaddr)(x) & 0xFFFFFFFF))

#endif /* CFE_EDS_ENABLED_BUILD */

#endif /* _CFE_ES_EXTERN_TYPEDEFS_H_ */

