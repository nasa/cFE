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

/*
**  File: cfe_es.h
**
**  Purpose:
**	Unit specification for Executive Services library functions and macros.
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**     cFE Flight Software Application Developers Guide
**
**	Notes:
**
*/

#ifndef _cfe_es_
#define _cfe_es_

/*
** Includes 
*/
#include "cfe_es_extern_typedefs.h"
#include "cfe_mission_cfg.h"
#include "cfe_perfids.h"

/*****************************************************************************/

/*
** Macro Definitions
*/

/* 
** The OS_PRINTF macro may be defined by OSAL to enable
** printf-style argument checking.  If using a version of OSAL
** that does not define this then define it as a no-op.
*/
#ifndef OS_PRINTF
#define OS_PRINTF(m,n)
#endif

#define CFE_ES_DBIT(x) (1L << (x))                                    /* Places a one at bit positions 0 thru 31 */
#define CFE_ES_DTEST(i,x) (((i) & CFE_ES_DBIT(x)) != 0)               /* true iff bit x of i is set */
#define CFE_ES_TEST_LONG_MASK(m,s)  (CFE_ES_DTEST(m[(s)/32],(s)%32))  /* Test a bit within an array of 32-bit integers. */
#define CFE_ES_MAX_MEMPOOL_BLOCK_SIZES     17    /**< Max number of size divisions allowed in a memory pool */

/*
** Note about reset type and subtypes:
**
** These values come from the PSP so the actual definition of these enumerations
** was moved to the PSP header file <cfe_psp.h>.
**
** In the future the Electronic Data sheets (EDS) for PSP/ES
** will define the exact values to use in telemetry messages.
*/

/*
**  Reset types
*/
/** \name Reset Type extensions */
/** \{ */
#define CFE_ES_APP_RESTART       CFE_PSP_RST_TYPE_MAX       /**< Application only was reset (extend the PSP enumeration here) */
/** \} */


/** \name Critical Data Store Macros */
/** \{ */
/** Maximum length allowed for CDS name. <BR>
** NOTE: "+2" is for NULL Character and "." (i.e. - "AppName.CDSName") */
#define CFE_ES_CDS_MAX_FULL_NAME_LEN (CFE_MISSION_ES_CDS_MAX_NAME_LENGTH + OS_MAX_API_NAME + 2)

#define CFE_ES_CDS_BAD_HANDLE  (CFE_ES_CDSHandle_t) 0xFFFF
/** \} */

#define CFE_ES_NO_MUTEX                 0 /**< \brief Indicates that the memory pool selection will not use a semaphore */
#define CFE_ES_USE_MUTEX                1 /**< \brief Indicates that the memory pool selection will use a semaphore */

/*
 * To preserve source-code compatibility with existing code,
 * this allows the old enum names to still work.  This should
 * be turned off after the new names are established.
 *  (sed -i -e 's/<old-name>/<new-name>/g' should take care of it)
 *
 * Note about why this is a good idea to do --
 * In the list below there are two values with similar names:
 *   CFE_ES_EXCEPTION, CFE_ES_SYS_EXCEPTION
 *
 * But these map to different values for two different purposes,
 * one is a app status and the other is a reset subtype.  Using the
 * new names makes it much clearer as to which is which, will
 * greatly reduce the chance of getting them mixed up, and make it
 * much more obvious to a code reviewer if the ARE mixed up somewhere.
 */
#ifndef CFE_OMIT_DEPRECATED_6_6

#define CFE_ES_PROCESSOR_RESET            CFE_PSP_RST_TYPE_PROCESSOR
#define CFE_ES_POWERON_RESET              CFE_PSP_RST_TYPE_POWERON

#define CFE_ES_POWER_CYCLE                CFE_PSP_RST_SUBTYPE_POWER_CYCLE
#define CFE_ES_PUSH_BUTTON                CFE_PSP_RST_SUBTYPE_PUSH_BUTTON
#define CFE_ES_HW_SPECIAL_COMMAND         CFE_PSP_RST_SUBTYPE_HW_SPECIAL_COMMAND
#define CFE_ES_HW_WATCHDOG                CFE_PSP_RST_SUBTYPE_HW_WATCHDOG
#define CFE_ES_RESET_COMMAND              CFE_PSP_RST_SUBTYPE_RESET_COMMAND
#define CFE_ES_EXCEPTION                  CFE_PSP_RST_SUBTYPE_EXCEPTION
#define CFE_ES_UNDEFINED_RESET            CFE_PSP_RST_SUBTYPE_UNDEFINED_RESET
#define CFE_ES_HWDEBUG_RESET              CFE_PSP_RST_SUBTYPE_HWDEBUG_RESET
#define CFE_ES_BANKSWITCH_RESET           CFE_PSP_RST_SUBTYPE_BANKSWITCH_RESET

/*
 * Compatibility Macros for the SystemState enumeration
 */
#define CFE_ES_SYSTEM_STATE_UNDEFINED       CFE_ES_SystemState_UNDEFINED
#define CFE_ES_SYSTEM_STATE_EARLY_INIT      CFE_ES_SystemState_EARLY_INIT
#define CFE_ES_SYSTEM_STATE_CORE_STARTUP    CFE_ES_SystemState_CORE_STARTUP
#define CFE_ES_SYSTEM_STATE_CORE_READY      CFE_ES_SystemState_CORE_READY
#define CFE_ES_SYSTEM_STATE_APPS_INIT       CFE_ES_SystemState_APPS_INIT
#define CFE_ES_SYSTEM_STATE_OPERATIONAL     CFE_ES_SystemState_OPERATIONAL
#define CFE_ES_SYSTEM_STATE_SHUTDOWN        CFE_ES_SystemState_SHUTDOWN

/*
 * Compatibility Macros for the RunStatus enumeration
 */
#define CFE_ES_APP_RUN                      CFE_ES_RunStatus_APP_RUN
#define CFE_ES_APP_EXIT                     CFE_ES_RunStatus_APP_EXIT
#define CFE_ES_APP_ERROR                    CFE_ES_RunStatus_APP_ERROR
#define CFE_ES_SYS_EXCEPTION                CFE_ES_RunStatus_SYS_EXCEPTION
#define CFE_ES_SYS_RESTART                  CFE_ES_RunStatus_SYS_RESTART
#define CFE_ES_SYS_RELOAD                   CFE_ES_RunStatus_SYS_RELOAD
#define CFE_ES_SYS_DELETE                   CFE_ES_RunStatus_SYS_DELETE
#define CFE_ES_CORE_APP_INIT_ERROR          CFE_ES_RunStatus_CORE_APP_INIT_ERROR
#define CFE_ES_CORE_APP_RUNTIME_ERROR       CFE_ES_RunStatus_CORE_APP_RUNTIME_ERROR

/*
 * Compatibility Macros for the AppState enumeration
 */
#define CFE_ES_APP_STATE_UNDEFINED          CFE_ES_AppState_UNDEFINED
#define CFE_ES_APP_STATE_EARLY_INIT         CFE_ES_AppState_EARLY_INIT
#define CFE_ES_APP_STATE_LATE_INIT          CFE_ES_AppState_LATE_INIT
#define CFE_ES_APP_STATE_RUNNING            CFE_ES_AppState_RUNNING
#define CFE_ES_APP_STATE_WAITING            CFE_ES_AppState_WAITING
#define CFE_ES_APP_STATE_STOPPED            CFE_ES_AppState_STOPPED

/*
 * Compatibility Macros for the AppType enumeration
 */
#define CFE_ES_APP_TYPE_CORE                CFE_ES_AppType_CORE
#define CFE_ES_APP_TYPE_EXTERNAL            CFE_ES_AppType_EXTERNAL

/*
 * Compatibility Macros for the LogMode enumeration
 */
#define CFE_ES_LOG_DISCARD                  CFE_ES_LogMode_DISCARD
#define CFE_ES_LOG_OVERWRITE                CFE_ES_LogMode_OVERWRITE

/*
 * Compatibility Macros for the ExceptionAction enumeration
 */
#define CFE_ES_APP_EXCEPTION_RESTART_APP    CFE_ES_ExceptionAction_RESTART_APP
#define CFE_ES_APP_EXCEPTION_PROC_RESTART   CFE_ES_ExceptionAction_PROC_RESTART

/*
 * Compatibility Macros for the LogEntryType enumeration
 */
#define CFE_ES_CORE_LOG_ENTRY               CFE_ES_LogEntryType_CORE
#define CFE_ES_APPLICATION_LOG_ENTRY        CFE_ES_LogEntryType_APPLICATION


#endif

/*****************************************************************************/
/*
** Type Definitions
*/

/*
** Memory Handle type
*/
typedef cpuaddr CFE_ES_MemHandle_t;  /**< \brief Data type used to hold Handles of Memory Pools created via CFE_ES_PoolCreate and CFE_ES_PoolCreateNoSem */

/*
** 
** CFE_ES_AppInfo_t is a structure that is used to provide
** information about an app. It is primarily used for the QueryOne and
** QueryAll Commands.
**
*/
typedef struct
{
   uint32   AppId;                          /**< \cfetlmmnemonic \ES_APP_ID
                                                 \brief Application ID for this Application */
   uint32   Type;                           /**< \cfetlmmnemonic \ES_APPTYPE
                                                 \brief The type of App: CORE or EXTERNAL */

   char     Name[OS_MAX_API_NAME];          /**< \cfetlmmnemonic \ES_APPNAME
                                                 \brief The Registered Name of the Application */
   char     EntryPoint[OS_MAX_API_NAME];    /**< \cfetlmmnemonic \ES_APPENTRYPT
                                                 \brief The Entry Point label for the Application */
   char     FileName[OS_MAX_PATH_LEN];      /**< \cfetlmmnemonic \ES_APPFILENAME
                                                 \brief The Filename of the file containing the Application */

   uint32   StackSize;                      /**< \cfetlmmnemonic \ES_STACKSIZE
                                                 \brief The Stack Size of the Application */
   uint32   ModuleId;                       /**< \cfetlmmnemonic \ES_MODULEID
                                                 \brief The ID of the Loadable Module for the Application */
   uint32   AddressesAreValid;              /**< \cfetlmmnemonic \ES_ADDRVALID
                                                 \brief Indicates that the Code, Data, and BSS addresses/sizes are valid */
   uint32   CodeAddress;                    /**< \cfetlmmnemonic \ES_CODEADDR
                                                 \brief The Address of the Application Code Segment*/
   uint32   CodeSize;                       /**< \cfetlmmnemonic \ES_CODESIZE
                                                 \brief The Code Size of the Application */
   uint32   DataAddress;                    /**< \cfetlmmnemonic \ES_DATAADDR
                                                 \brief The Address of the Application Data Segment*/
   uint32   DataSize;                       /**< \cfetlmmnemonic \ES_DATASIZE
                                                 \brief The Data Size of the Application */
   uint32   BSSAddress;                     /**< \cfetlmmnemonic \ES_BSSADDR
                                                 \brief The Address of the Application BSS Segment*/
   uint32   BSSSize;                        /**< \cfetlmmnemonic \ES_BSSSIZE
                                                  \brief The BSS Size of the Application */
   uint32   StartAddress;                   /**< \cfetlmmnemonic \ES_STARTADDR
                                                 \brief The Start Address of the Application */
   uint16   ExceptionAction;                /**< \cfetlmmnemonic \ES_EXCEPTNACTN
                                                 \brief What should occur if Application has an exception
                                                 (Restart Application OR Restart Processor) */
   uint16   Priority;                       /**< \cfetlmmnemonic \ES_PRIORITY
                                                 \brief The Priority of the Application */
   uint32   MainTaskId;                     /**< \cfetlmmnemonic \ES_MAINTASKID
                                                 \brief The Application's Main Task ID */
   uint32   ExecutionCounter;               /**< \cfetlmmnemonic \ES_MAINTASKEXECNT
                                                 \brief The Application's Main Task Execution Counter */
   char     MainTaskName[OS_MAX_API_NAME];  /**< \cfetlmmnemonic \ES_MAINTASKNAME
                                                 \brief The Application's Main Task ID */
   uint32   NumOfChildTasks;                /**< \cfetlmmnemonic \ES_CHILDTASKS
                                                 \brief Number of Child tasks for an App */

} CFE_ES_AppInfo_t;

/*
** Task Info Type
*/
typedef struct
{
   uint32   TaskId;                    /**< \brief Task Id */
   uint32   ExecutionCounter;          /**K \brief Task Execution Counter */
   uint8    TaskName[OS_MAX_API_NAME]; /**< \brief Task Name */
   uint32   AppId;                     /**< \brief Parent Application ID */
   uint8    AppName[OS_MAX_API_NAME];  /**< \brief Parent Application Name */

} CFE_ES_TaskInfo_t;

/*
** Memory Pool Statistics data type
*/
typedef struct
{
    uint32  BlockSize;               /**< \brief Number of bytes in each of these blocks */
    uint32  NumCreated;              /**< \brief Number of Memory Blocks of this size created */
    uint32  NumFree;                 /**< \brief Number of Memory Blocks of this size that are free */
} CFE_ES_BlockStats_t;

typedef struct
{
    uint32                PoolSize;                /**< \cfetlmmnemonic \ES_POOLSIZE
                                                        \brief  Size of Memory Pool (in bytes) */
    uint32                NumBlocksRequested;      /**< \cfetlmmnemonic \ES_BLKSREQ
                                                        \brief Number of times a memory block has been allocated */
    uint32                CheckErrCtr;             /**< \cfetlmmnemonic \ES_BLKERRCTR
                                                        \brief Number of errors detected when freeing a memory block */
    uint32                NumFreeBytes;            /**< \cfetlmmnemonic \ES_FREEBYTES
                                                        \brief Number of bytes never allocated to a block */
    CFE_ES_BlockStats_t   BlockStats[CFE_ES_MAX_MEMPOOL_BLOCK_SIZES]; /**< \cfetlmmnemonic \ES_BLKSTATS
                                                                           \brief Contains stats on each block size */
} CFE_ES_MemPoolStats_t;

/*
** CDS Handle type
*/
typedef cpuaddr CFE_ES_CDSHandle_t;    /**< \brief Data type used to hold Handles of Critical Data Stores. See CFE_ES_RegisterCDS */

typedef struct
{
    CFE_ES_CDSHandle_t    Handle;          /**< \brief Handle of CDS */
    uint32                Size;            /**< \brief Size, in bytes, of the CDS memory block */
    bool                  Table;           /**< \brief Flag that indicates whether CDS contains a Critical Table */
    char                  Name[CFE_ES_CDS_MAX_FULL_NAME_LEN]; /**< \brief Processor Unique Name of CDS */
    uint8                 ByteAlignSpare1; /**< \brief Spare byte to insure structure size is multiple of 4 bytes */
} CFE_ES_CDSRegDumpRec_t;

/*
** Child Task Main Function Prototype
*/
typedef void (*CFE_ES_ChildTaskMainFuncPtr_t)(void); /**< \brief Required Prototype of Child Task Main Functions */
typedef int32 (*CFE_ES_LibraryEntryFuncPtr_t)(uint32 LibId); /**< \brief Required Prototype of Library Initialization Functions */

/**
 * Union that can be used for minimum memory alignment of ES memory pools on the target.
 * It contains the longest native data types such that the alignment of this structure
 * should reflect the largest possible alignment requirements for any data on this processor.
 */
typedef union
{
    void *Ptr;
    /* note -- native types (int/double) are intentional here */
    long long int LongInt;
    long double LongDouble;
} CFE_ES_PoolAlign_t;

/**
 * A macro to help instantiate static memory pools that are correctly aligned.
 * This resolves to a union type that contains a member called "Data" that will
 * be correctly aligned to be a memory pool and sized according to the argument.
 */
#define CFE_ES_STATIC_POOL_TYPE(size)    union { CFE_ES_PoolAlign_t Align; uint8 Data[size]; }

/*****************************************************************************/
/*
** Exported Functions
*/

/*****************************************************************************/


/*****************************************************************************/
/**
** \brief cFE Main Entry Point used by Board Support Package to start cFE
**
** \par Description
**        cFE main entry point. This is the entry point into the cFE software.
**        It is called only by the Board Support Package software.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  StartType     Identifies whether this was a #CFE_PSP_RST_TYPE_POWERON or #CFE_PSP_RST_TYPE_PROCESSOR.
**
** \param[in]  StartSubtype  Specifies, in more detail, what caused the \c StartType identified above.
**                           See #CFE_PSP_RST_SUBTYPE_POWER_CYCLE for possible examples.
**
** \param[in]  ModeId        Identifies the source of the Boot as determined by the BSP.
**
** \param[in]  StartFilePath Identifies the startup file to use to initialize the cFE apps.
**
** \sa #CFE_ES_ResetCFE
**
******************************************************************************/
void CFE_ES_Main(uint32 StartType, uint32 StartSubtype, uint32 ModeId , const char *StartFilePath );

/*****************************************************************************/
/**
** \brief Return the most recent Reset Type
**
** \par Description
**        Provides the caller with codes that identifies the type of Reset
**        the processor most recently underwent.  The caller can also obtain
**        information on what caused the reset by supplying a pointer to a
**        variable that will be filled with the Reset Sub-Type.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]   ResetSubtypePtr    Pointer to \c uint32 type variable in which the Reset Sub-Type will be stored.
**                                 The caller can set this pointer to NULL if the Sub-Type is of no interest.
**
** \param[out]  *ResetSubtypePtr   If the provided pointer was not \c NULL, the Reset Sub-Type is stored at the given address.
**                                 For a list of possible Sub-Type values, see \link #CFE_PSP_RST_SUBTYPE_POWER_CYCLE "Reset Sub-Types" \endlink.
**
** \returns
** \retcode #CFE_PSP_RST_TYPE_POWERON   \retdesc \copydoc CFE_PSP_RST_TYPE_POWERON    \endcode
** \retcode #CFE_PSP_RST_TYPE_PROCESSOR \retdesc \copydoc CFE_PSP_RST_TYPE_PROCESSOR  \endcode
** \endreturns
**
** \sa #CFE_ES_GetAppID, #CFE_ES_GetAppIDByName, #CFE_ES_GetAppName, #CFE_ES_GetTaskInfo
**
******************************************************************************/
int32 CFE_ES_GetResetType(uint32 *ResetSubtypePtr);

/*****************************************************************************/
/**
** \brief Reset the cFE Core and all cFE Applications
**
** \par Description
**        This API causes an immediate reset of the cFE Kernel and all cFE Applications.
**        The caller can specify whether the reset should clear all memory (#CFE_PSP_RST_TYPE_POWERON)
**        or try to retain volatile memory areas (#CFE_PSP_RST_TYPE_PROCESSOR).
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  ResetType    Identifies the type of reset desired.  Allowable settings are:
**                          \arg #CFE_PSP_RST_TYPE_POWERON     - Causes all memory to be cleared 
**                          \arg #CFE_PSP_RST_TYPE_PROCESSOR   - Attempts to retain volatile disk, critical data store and user reserved memory. 
**
** \returns
** \retcode #CFE_ES_BAD_ARGUMENT    \retdesc \copydoc CFE_ES_BAD_ARGUMENT     \endcode
** \retcode #CFE_ES_NOT_IMPLEMENTED \retdesc \copydoc CFE_ES_NOT_IMPLEMENTED  \endcode
** \endreturns
**
** \sa #CFE_ES_Main
**
******************************************************************************/
int32  CFE_ES_ResetCFE(uint32 ResetType);

/*****************************************************************************/
/**
** \brief Restart a single cFE Application
**
** \par Description
**        This API causes a cFE Application to be stopped and restarted.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  AppID       Identifies the application to be reset.
**
** \returns
** \retcode #CFE_ES_NOT_IMPLEMENTED \retdesc \copydoc CFE_ES_NOT_IMPLEMENTED  \endcode
** \endreturns
**
** \sa #CFE_ES_ReloadApp, #CFE_ES_DeleteApp
**
******************************************************************************/
int32 CFE_ES_RestartApp(uint32 AppID);

/*****************************************************************************/
/**
** \brief Reload a single cFE Application
**
** \par Description
**        This API causes a cFE Application to be stopped and restarted from
**        the specified file.
**
** \par Assumptions, External Events, and Notes:
**        The specified application will be deleted before it is reloaded from the
**        specified file.  In the event that an application cannot be reloaded due to
**        a corrupt file, the application may no longer be reloaded when given a valid
**        load file (it has been deleted and no longer exists).  To recover, the
**        application may be restarted by loading the application via the ES_STARTAPP
**        command (#CFE_ES_START_APP_CC).
**
** \param[in]  AppID       Identifies the application to be reset.
**
** \param[in]  AppFileName Identifies the new file to start.
**
** \returns
** \retcode #CFE_ES_NOT_IMPLEMENTED \retdesc \copydoc CFE_ES_NOT_IMPLEMENTED  \endcode
** \endreturns
**
** \sa #CFE_ES_RestartApp, #CFE_ES_DeleteApp, #CFE_ES_START_APP_CC
**
******************************************************************************/
int32 CFE_ES_ReloadApp(uint32 AppID, const char *AppFileName);

/*****************************************************************************/
/**
** \brief Delete a cFE Application
**
** \par Description
**        This API causes a cFE Application to be stopped deleted.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  AppID       Identifies the application to be reset.
**
** \returns
** \retcode #CFE_ES_NOT_IMPLEMENTED \retdesc \copydoc CFE_ES_NOT_IMPLEMENTED  \endcode
** \endreturns
**
** \sa #CFE_ES_RestartApp, #CFE_ES_ReloadApp
**
******************************************************************************/
int32 CFE_ES_DeleteApp(uint32 AppID);

/*****************************************************************************/
/**
** \brief Exit a cFE Application
**
** \par Description
**        This API is the "Exit Point" for the cFE application
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  ExitStatus     .
**
** \returns
** \retcode #CFE_ES_NOT_IMPLEMENTED \retdesc \copydoc CFE_ES_NOT_IMPLEMENTED  \endcode
** \endreturns
**
** \sa #CFE_ES_RunLoop, #CFE_ES_RegisterApp
**
******************************************************************************/
void CFE_ES_ExitApp(uint32 ExitStatus);


/*****************************************************************************/
/**
** \brief Check for Exit, Restart, or Reload commands
**
** \par Description
**        This is the API that allows an app to check for exit requests from
**         the system.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  ExitStatus   A pointer to a variable containing the Application's
**                          desired run status.  Acceptable values are:
**                          \arg #CFE_ES_RunStatus_APP_RUN - \copydoc CFE_ES_RunStatus_APP_RUN
**                          \arg #CFE_ES_RunStatus_APP_EXIT - \copydoc CFE_ES_RunStatus_APP_EXIT
**                          \arg #CFE_ES_RunStatus_APP_ERROR - \copydoc CFE_ES_RunStatus_APP_ERROR
**
** \returns
** \retcode true  \retdesc The application should continue executing \endcode
** \retcode false \retdesc The application should terminate itself \endcode
** \endreturns
**
** \sa #CFE_ES_ExitApp, #CFE_ES_RegisterApp
**
******************************************************************************/
bool CFE_ES_RunLoop(uint32 *ExitStatus);

/*****************************************************************************/
/**
** \brief Allow an Application to Wait for a minimum global system state
**
** \par Description
**        This is the API that allows an app to wait for the rest of the apps
**        to complete a given stage of initialization before continuing.
**
**        This gives finer grained control than the "CFE_ES_WaitForStartupSync()" call.
**
** \par Assumptions, External Events, and Notes:
**        This API assumes that the caller has also been initialized sufficiently
**        to satisfy the global system state it is waiting for, and the apps own
**        state will be updated accordingly.
**
** \param[in]  TimeOutMilliseconds   The timeout value in Milliseconds.
**                                   This parameter must be at least 1000. Lower values
**                                   will be rounded up. There is not an option to
**                                   wait indefinitely to avoid hanging a critical
**                                   application because a non-critical app did not start.
**
** \param[in]  MinSystemState        Determine the state of the App
**
** \returns
**      CFE_SUCCESS if state was successfully achieved
**      CFE_ES_OPERATION_TIMED_OUT if the timeout was reached
**      (or other defined error code in case of error)
**
** \sa #CFE_ES_RunLoop
**
******************************************************************************/
int32 CFE_ES_WaitForSystemState(uint32 MinSystemState, uint32 TimeOutMilliseconds);

/*****************************************************************************/
/**
** \brief Allow an Application to Wait for the "OPERATIONAL" global system state
**
** \par Description
**        This is the API that allows an app to wait for the rest of the apps
**        to complete their entire initialization before continuing. It is most
**        useful for applications such as Health and Safety or the Scheduler that need
**        to wait until applications exist and are running before sending out
**        packets to them.
**
**        This is a specialized wrapper for CFE_ES_WaitForSystemState() for compatibility
**        with applications using this API.
**
** \par Assumptions, External Events, and Notes:
**          This API should only be called as the last item of an Apps initialization.
**          In addition, this API should only be called by an App that is started
**          from the ES Startup file. It should not be used by an App that is 
**          started after the system is running. ( Although it will cause no harm )
**
** \param[in]  TimeOutMilliseconds   The timeout value in Milliseconds.
**                                   This parameter must be at least 1000. Lower values
**                                   will be rounded up. There is not an option to 
**                                   wait indefinitely to avoid hanging a critical
**                                   application because a non-critical app did not start. 
**
**
** \sa #CFE_ES_RunLoop
**
******************************************************************************/
void CFE_ES_WaitForStartupSync(uint32 TimeOutMilliseconds);


/*
** Application Management functions
*/

/*****************************************************************************/
/**
** \brief Registers a cFE Application with the Executive Services
**
** \par Description
**        This API registers the calling Application with the cFE.
**
** \par Assumptions, External Events, and Notes:
**        NOTE: This function \b MUST be called before any other cFE API functions are called.
**
** \returns
** \retstmt Return codes from #OS_TaskRegister          \endcode
** \retstmt Return codes from #OS_BinSemTake            \endcode
** \retcode #CFE_SUCCESS \retdesc \copydoc CFE_SUCCESS  \endcode
** \endreturns
**
** \sa #CFE_ES_ExitApp, #CFE_ES_RunLoop
**
******************************************************************************/
int32 CFE_ES_RegisterApp(void);

/*****************************************************************************/
/**
** \brief Get an Application ID for the calling Application
**
** \par Description
**        This routine retrieves the cFE Application ID for the calling Application.  
**
** \par Assumptions, External Events, and Notes:
**        NOTE: \b All tasks associated with the Application would return the same Application ID.
**
** \param[in]   AppIdPtr       Pointer to variable that is to receive the Application's ID.
**
** \param[out]  *AppIdPtr      Application ID of the calling Application.
**
** \returns
** \retcode #CFE_SUCCESS       \retdesc \copydoc CFE_SUCCESS        \endcode
** \retcode #CFE_ES_ERR_APPID  \retdesc \copydoc CFE_ES_ERR_APPID   \endcode
** \retcode #CFE_ES_ERR_BUFFER \retdesc \copydoc CFE_ES_ERR_BUFFER  \endcode
** \endreturns
**
** \sa #CFE_ES_GetResetType, #CFE_ES_GetAppIDByName, #CFE_ES_GetAppName, #CFE_ES_GetTaskInfo
**
******************************************************************************/
int32 CFE_ES_GetAppID(uint32 *AppIdPtr);

/*****************************************************************************/
/**
** \brief Get an Application ID associated with a specified Application name
**
** \par Description
**        This routine retrieves the cFE Application ID associated with a
**        specified Application name.  
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in]   AppIdPtr       Pointer to variable that is to receive the Application's ID.
**
** \param[in]   AppName        Pointer to null terminated character string containing an Application name.
**
** \param[out]  *AppIdPtr      Application ID of the calling Application.
**
** \returns
** \retcode #CFE_SUCCESS         \retdesc \copydoc CFE_SUCCESS         \endcode
** \retcode #CFE_ES_ERR_APPNAME  \retdesc \copydoc CFE_ES_ERR_APPNAME  \endcode
** \retcode #CFE_ES_ERR_BUFFER   \retdesc \copydoc CFE_ES_ERR_BUFFER   \endcode
** \endreturns
**
** \sa #CFE_ES_GetResetType, #CFE_ES_GetAppID, #CFE_ES_GetAppName, #CFE_ES_GetTaskInfo
**
******************************************************************************/
int32 CFE_ES_GetAppIDByName(uint32 *AppIdPtr, const char *AppName);

/*****************************************************************************/
/**
** \brief Get an Application name for a specified Application ID
**
** \par Description
**        This routine retrieves the cFE Application name associated with a
**        specified Application ID.
**
** \par Assumptions, External Events, and Notes:
**        In the case of a failure (#CFE_ES_ERR_APPID), an empty string is returned.  #CFE_ES_ERR_APPID
**        will be returned if the specified Application ID (AppId) is invalid or not in use.
**
** \param[in]   AppName       Pointer to a character array of at least \c BufferLength in size that will
**                            be filled with the appropriate Application name.
**
** \param[in]   AppId         Application ID of Application whose name is being requested.
**
** \param[in]   BufferLength  The maximum number of characters, including the null terminator, that can be put
**                            into the \c AppName buffer.  This routine will truncate the name to this length,
**                            if necessary.
**
** \param[out]  *AppName      Null terminated Application name of the Application associated with the 
**                            specified Application ID.
**
** \returns
** \retcode #CFE_SUCCESS         \retdesc \copydoc CFE_SUCCESS        \endcode
** \retcode #CFE_ES_ERR_APPID    \retdesc \copydoc CFE_ES_ERR_APPID   \endcode
** \endreturns
**
** \sa #CFE_ES_GetResetType, #CFE_ES_GetAppID, #CFE_ES_GetAppIDByName, #CFE_ES_GetTaskInfo
**
******************************************************************************/
int32 CFE_ES_GetAppName(char *AppName, uint32 AppId, uint32 BufferLength);

/*****************************************************************************/
/**
** \brief Get Application Information given a specified App ID
**
** \par Description
**        This routine retrieves the information about an App associated with a
**        specified App ID. The information includes all of the information ES 
**        maintains for an application ( documented in the CFE_ES_AppInfo_t type ) 
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in]   AppInfo      Pointer to a \c CFE_ES_AppInfo_t structure that holds the specific 
**                            Application information.
**
** \param[in]   AppId        Application ID of Application whose name is being requested.
**
** \param[out]  *AppInfo     Filled out \c CFE_ES_AppInfo_t structure containing the 
**                            App Name, and application memory addresses among other fields.
**
** \returns
** \retcode #CFE_SUCCESS         \retdesc \copydoc CFE_SUCCESS         \endcode
** \retcode #CFE_ES_ERR_APPID    \retdesc \copydoc CFE_ES_ERR_APPID    \endcode
** \retcode #CFE_ES_ERR_BUFFER   \retdesc \copydoc CFE_ES_ERR_BUFFER   \endcode
** \endreturns
**
** \sa #CFE_ES_GetResetType, #CFE_ES_GetAppID, #CFE_ES_GetAppIDByName, #CFE_ES_GetAppName
**
******************************************************************************/
int32 CFE_ES_GetAppInfo(CFE_ES_AppInfo_t *AppInfo, uint32 AppId);

/*****************************************************************************/
/**
** \brief Get Task Information given a specified Task ID
**
** \par Description
**        This routine retrieves the information about a Task associated with a
**        specified Task ID. The information includes Task Name, and Parent/Creator 
**        Application ID.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in]   TaskInfo      Pointer to a \c CFE_ES_TaskInfo_t structure that holds the specific 
**                            task information.
**
** \param[in]   TaskId        Application ID of Application whose name is being requested.
**
** \param[out]  *TaskInfo     Filled out \c CFE_ES_TaskInfo_t structure containing the 
**                            Task Name, Parent App Name, Parent App ID among other fields.
**
** \returns
** \retcode #CFE_SUCCESS         \retdesc \copydoc CFE_SUCCESS         \endcode
** \retcode #CFE_ES_ERR_TASKID   \retdesc \copydoc CFE_ES_ERR_TASKID   \endcode
** \retcode #CFE_ES_ERR_BUFFER   \retdesc \copydoc CFE_ES_ERR_BUFFER   \endcode
** \endreturns
**
** \sa #CFE_ES_GetResetType, #CFE_ES_GetAppID, #CFE_ES_GetAppIDByName, #CFE_ES_GetAppName
**
******************************************************************************/
int32 CFE_ES_GetTaskInfo(CFE_ES_TaskInfo_t *TaskInfo, uint32 TaskId);

/*
** Task management functions
*/
/*****************************************************************************/
/**
** \brief Registers a cFE Child task associated with a cFE Application
**
** \par Description
**        This routine registers a cFE Child task and associates it with its parent
**        cFE Application.  
**
** \par Assumptions, External Events, and Notes:
**        NOTE: This API \b MUST be called by the Child Task before any other cFE API calls are made.
**
** \returns
** \retcode #CFE_SUCCESS                       \retdesc \copydoc CFE_SUCCESS                     \endcode
** \retcode #CFE_ES_ERR_CHILD_TASK_REGISTER    \retdesc \copydoc CFE_ES_ERR_CHILD_TASK_REGISTER  \endcode
** \endreturns
**
** \sa #CFE_ES_CreateChildTask, #CFE_ES_DeleteChildTask, #CFE_ES_ExitChildTask
**
******************************************************************************/
int32  CFE_ES_RegisterChildTask(void);

/*****************************************************************************/
/**
** \brief Creates a new task under an existing Application
**
** \par Description
**        This routine creates a new task (a separate execution thread) owned by the calling Application.  
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in]   TaskIdPtr     A pointer to a variable that will be filled in with the new task's ID.
**
** \param[in]   TaskName      A pointer to a string containing the desired name of the new task.  
**                            This can be up to #OS_MAX_API_NAME characters, including the trailing null.
**
** \param[in]   FunctionPtr   A pointer to the function that will be spawned as a new task.  This function 
**                            must have the following signature: uint32 function(void).  Input parameters 
**                            for the new task are not supported.
**
** \param[in]   StackPtr      A pointer to the location where the child task's stack pointer should start.   
**                            NOTE: Not all underlying operating systems support this parameter.
**
** \param[in]   StackSize     The number of bytes to allocate for the new task's stack.
**
** \param[in]   Priority      The priority for the new task.  Lower numbers are higher priority, with 0 being 
**                            the highest priority.  Applications cannot create tasks with a higher priority 
**                            (lower number) than their own priority.
**
** \param[in]   Flags         Reserved for future expansion.
**
** \param[out]  *TaskIdPtr    The Task ID of the newly created child task.
**
** \returns
** \retcode #CFE_SUCCESS                     \retdesc \copydoc CFE_SUCCESS                   \endcode
** \retcode #CFE_ES_ERR_CHILD_TASK_CREATE    \retdesc \copydoc CFE_ES_ERR_CHILD_TASK_CREATE  \endcode
** \endreturns
**
** \sa #CFE_ES_RegisterChildTask, #CFE_ES_DeleteChildTask, #CFE_ES_ExitChildTask
**
******************************************************************************/
int32  CFE_ES_CreateChildTask(uint32                          *TaskIdPtr,
                              const char                      *TaskName,
                              CFE_ES_ChildTaskMainFuncPtr_t    FunctionPtr,
                              uint32                          *StackPtr,
                              uint32                           StackSize,
                              uint32                           Priority,
                              uint32                           Flags);

/*****************************************************************************/
/**
** \brief Deletes a task under an existing Application
**
** \par Description
**        This routine deletes a task under an Application specified by the \c TaskId obtained
**        when the child task was created using the #CFE_ES_CreateChildTask API.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in]   TaskId     The task ID previously obtained when the Child Task was created with the #CFE_ES_CreateChildTask API.
**
** \returns
** \retcode #CFE_ES_NOT_IMPLEMENTED \retdesc \copydoc CFE_ES_NOT_IMPLEMENTED  \endcode
** \endreturns
**
** \sa #CFE_ES_RegisterChildTask, #CFE_ES_CreateChildTask, #CFE_ES_ExitChildTask
**
******************************************************************************/
int32 CFE_ES_DeleteChildTask(uint32 TaskId);

/*****************************************************************************/
/**
** \brief Exits a child task
**
** \par Description
**        This routine allows the current executing child task to exit and 
**        be deleted by ES.
**
** \par Assumptions, External Events, and Notes:
**        This function cannot be called from an Application's Main Task.
**
** \returns
** \retstmt
**        This function does not return a value, but if it does return
**        at all, it is assumed that the Task was either unregistered or
**        this function was called from a cFE Application's main task.   \endcode
** \endreturns
**
** \sa #CFE_ES_RegisterChildTask, #CFE_ES_CreateChildTask, #CFE_ES_DeleteChildTask
**
******************************************************************************/
void CFE_ES_ExitChildTask(void);

/*****************************************************************************/
/**
** \brief Increments the execution counter for the calling task
**
** \par Description
**        This routine increments the execution counter that is stored for
**        the calling task. It can be called from cFE Application main tasks, child
**        tasks, or cFE Core application main tasks. Normally, the call is not 
**        necessary from a cFE Application, since the CFE_ES_RunLoop call increments
**        the counter for the Application. 
**
** \par Assumptions, External Events, and Notes:
**        NOTE: This API is not needed for Appplications that call the CFE_ES_RunLoop call.
**
** \returns
** \retstmt
**        This function does not return a value.   \endcode
** \endreturns
**
** \sa #CFE_ES_RunLoop
**
******************************************************************************/
void  CFE_ES_IncrementTaskCounter(void);

/*
** Log management functions
*/

/*****************************************************************************/
/**
** \brief Write a string to the cFE System Log
**
** \par Description
**        This routine writes a formatted string to the cFE system log.  This 
**        can be used to record very low-level errors that can't be reported 
**        using the Event Services. This function is used in place of printf 
**        for flight software. It should be used for significant startup events, 
**        critical errors, and conditionally compiled debug software. 
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in]   SpecStringPtr     The format string for the log message.  
**                                This is similar to the format string for a printf() call.
**
** \returns
** \retcode #CFE_SUCCESS             \retdesc \copydoc CFE_SUCCESS             \endcode
** \retcode #CFE_ES_ERR_SYS_LOG_FULL \retdesc \copydoc CFE_ES_ERR_SYS_LOG_FULL \endcode
** \endreturns
**
** \sa
**
******************************************************************************/
int32 CFE_ES_WriteToSysLog(const char *SpecStringPtr, ...) OS_PRINTF(1,2);

/*****************************************************************************/
/**
** \brief Calculate a CRC on a block of memory
**
** \par Description
**        This routine calculates a cyclic redundancy check (CRC) on a block of memory.  The CRC algorithm
**        used is determined by the last parameter. 
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in]   DataPtr     Pointer to the base of the memory block.
**
** \param[in]   DataLength  The number of bytes in the memory block.
**
** \param[in]   InputCRC    A starting value for use in the CRC calculation.  This parameter
**                          allows the user to calculate the CRC of non-contiguous blocks as
**                          a single value.  Nominally, the user should set this value to zero.
**
** \param[in]   TypeCRC     One of the following CRC algorithm selections:
**
**                          \arg \c CFE_MISSION_ES_CRC_8 - (Not currently implemented)
**                          \arg \c CFE_MISSION_ES_CRC_16 - a CRC-16 algorithm
**                          \arg \c CFE_MISSION_ES_CRC_32 - (not currently implemented)
**
** \returns
** \retstmt The result of the CRC calculation on the specified memory block. \endcode
** \endreturns
**
******************************************************************************/
uint32 CFE_ES_CalculateCRC(const void *DataPtr, uint32 DataLength, uint32 InputCRC, uint32 TypeCRC);

/*
** Critical Data Store API
*/

/*****************************************************************************/
/**
** \brief Reserve space (or re-obtain previously reserved space) in the Critical Data Store (CDS)
**
** \par Description
**        This routine allocates a block of memory in the Critical Data Store and associates it with
**        the calling Application.  The memory can survive an Application restart as well as a Processor Reset. 
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in]   HandlePtr   Pointer Application's variable that will contain the CDS Memory Block Handle.
**
** \param[in]   BlockSize   The number of bytes needed in the CDS.
**
** \param[in]   Name        A pointer to a character string containing an application 
**                          unique name of #CFE_MISSION_ES_CDS_MAX_NAME_LENGTH characters or less.
**
** \param[out]  *HandlePtr  The handle of the CDS block that can be used in 
**                          #CFE_ES_CopyToCDS and #CFE_ES_RestoreFromCDS.
**
** \returns
** \retcode #CFE_SUCCESS  \retdesc The memory block was successfully created in the CDS.           \endcode
** \retcode #CFE_ES_NOT_IMPLEMENTED \retdesc The processor does not support a Critical Data Store. \endcode
** \retcode #CFE_ES_CDS_ALREADY_EXISTS \retdesc \copydoc CFE_ES_CDS_ALREADY_EXISTS                 \endcode
** \retcode #CFE_ES_CDS_INVALID_SIZE   \retdesc \copydoc CFE_ES_CDS_INVALID_SIZE                   \endcode
** \retcode #CFE_ES_CDS_INVALID_NAME   \retdesc \copydoc CFE_ES_CDS_INVALID_NAME                   \endcode
** \retcode #CFE_ES_CDS_REGISTRY_FULL  \retdesc \copydoc CFE_ES_CDS_REGISTRY_FULL                  \endcode
** \endreturns
**
** \sa #CFE_ES_CopyToCDS, #CFE_ES_RestoreFromCDS
**
******************************************************************************/
int32 CFE_ES_RegisterCDS(CFE_ES_CDSHandle_t *HandlePtr, int32 BlockSize, const char *Name);

/*****************************************************************************/
/**
** \brief Save a block of data in the Critical Data Store (CDS)
**
** \par Description
**        This routine copies a specified block of memory into the Critical Data Store that
**        had been previously registered via #CFE_ES_RegisterCDS.  The block of memory to be
**        copied must be at least as big as the size specified when registering the CDS.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in]   Handle       The handle of the CDS block that was previously obtained from #CFE_ES_RegisterCDS.
**
** \param[in]   DataToCopy   A Pointer to the block of memory to be copied into the CDS.
**
** \returns
** \retcode #OS_SUCCESS              \retdesc \copydoc OS_SUCCESS              \endcode
** \retcode #CFE_ES_ERR_MEM_HANDLE   \retdesc \copydoc CFE_ES_ERR_MEM_HANDLE   \endcode
** \retcode #OS_ERROR                \retdesc Problem with handle or a size mismatch   \endcode
** \endreturns
**
** \sa #CFE_ES_RegisterCDS, #CFE_ES_RestoreFromCDS
**
*/
int32 CFE_ES_CopyToCDS(CFE_ES_CDSHandle_t Handle, void *DataToCopy);

/*****************************************************************************/
/**
** \brief Recover a block of data from the Critical Data Store (CDS)
**
** \par Description
**        This routine copies data from the Critical Data Store identified with the \c Handle into
**        the area of memory pointed to by the \c RestoreToMemory pointer.  The area of memory to
**        be copied into must be at least as big as the size specified when registering the CDS.
**        The recovery will indicate an error if the data integrity check maintained by the CDS 
**        indicates the contents of the CDS have changed.  However, the contents will still be
**        copied into the specified area of memory.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in]   Handle             The handle of the CDS block that was previously obtained from #CFE_ES_RegisterCDS.
**
** \param[in]   RestoreToMemory    A Pointer to the block of memory that is to be restored with the contents of the CDS.
**
** \param[out]  *RestoreToMemory   The contents of the specified CDS.
**
** \returns
** \retcode #CFE_SUCCESS                \retdesc \copydoc CFE_SUCCESS                     \endcode
** \retcode #CFE_ES_CDS_BLOCK_CRC_ERR   \retdesc \copydoc CFE_ES_CDS_BLOCK_CRC_ERR        \endcode
** \retcode #OS_ERROR                   \retdesc Problem with handle or a size mismatch   \endcode
** \endreturns
**
** \sa #CFE_ES_RegisterCDS, #CFE_ES_CopyToCDS
**
*/
int32 CFE_ES_RestoreFromCDS(void *RestoreToMemory, CFE_ES_CDSHandle_t Handle);

/*
** Memory Allocation/Deallocation API
*/

/*****************************************************************************/
/**
** \brief Initializes a memory pool created by an application without using a semaphore during processing.
**
** \par Description
**        This routine initializes a pool of memory supplied by the calling application.  When a memory pool
**        created by this routine is processed, no mutex handling is performed.  
**
** \par Assumptions, External Events, and Notes:
**        -# The size of the pool must be an integral number of 32-bit words
**        -# The start address of the pool must be 32-bit aligned
**        -# 168 bytes are used for internal bookkeeping, therefore, they will not be available for allocation.
**
** \param[in]   HandlePtr   A pointer to the variable the caller wishes to have the memory pool handle kept in.
**
** \param[in]   MemPtr      A Pointer to the pool of memory created by the calling application. This address must
**                          be on a 32-bit boundary.
**
** \param[in]   Size        The size of the pool of memory.  Note that this must be an integral number of 32 bit words.
**
** \param[out]  *HandlePtr  The memory pool handle.
**
** \returns
** \retcode #CFE_SUCCESS           \retdesc \copydoc CFE_SUCCESS           \endcode
** \retcode #CFE_ES_BAD_ARGUMENT   \retdesc \copydoc CFE_ES_BAD_ARGUMENT   \endcode
** \endreturns
**
** \sa #CFE_ES_PoolCreate, #CFE_ES_PoolCreateEx, #CFE_ES_GetPoolBuf, #CFE_ES_PutPoolBuf, #CFE_ES_GetMemPoolStats
**
******************************************************************************/
int32 CFE_ES_PoolCreateNoSem(CFE_ES_MemHandle_t *HandlePtr, uint8 *MemPtr, uint32 Size);

/*****************************************************************************/
/**
** \brief Initializes a memory pool created by an application while using a semaphore during processing.
**
** \par Description
**        This routine initializes a pool of memory supplied by the calling application.  When a memory pool
**        created by this routine is processed, mutex handling will be performed.  
**
** \par Assumptions, External Events, and Notes:
**        -# The size of the pool must be an integral number of 32-bit words
**        -# The start address of the pool must be 32-bit aligned
**        -# 168 bytes are used for internal bookkeeping, therefore, they will not be available for allocation.
**
** \param[in]   HandlePtr   A pointer to the variable the caller wishes to have the memory pool handle kept in.
**
** \param[in]   MemPtr      A Pointer to the pool of memory created by the calling application. This address must
**                          be on a 32-bit boundary.
**
** \param[in]   Size        The size of the pool of memory.  Note that this must be an integral number of 32 bit words.
**
** \param[out]  *HandlePtr  The memory pool handle.
**
** \returns
** \retcode #CFE_SUCCESS           \retdesc \copydoc CFE_SUCCESS           \endcode
** \retcode #CFE_ES_BAD_ARGUMENT   \retdesc \copydoc CFE_ES_BAD_ARGUMENT   \endcode
** \endreturns
**
** \sa #CFE_ES_PoolCreateNoSem, #CFE_ES_PoolCreateEx, #CFE_ES_GetPoolBuf, #CFE_ES_PutPoolBuf, #CFE_ES_GetMemPoolStats
**
******************************************************************************/
int32 CFE_ES_PoolCreate(CFE_ES_MemHandle_t *HandlePtr, uint8 *MemPtr, uint32 Size);

/*****************************************************************************/
/**
** \brief Initializes a memory pool created by an application with application specified block sizes.
**
** \par Description
**        This routine initializes a pool of memory supplied by the calling application.  
**
** \par Assumptions, External Events, and Notes:
**        -# The size of the pool must be an integral number of 32-bit words
**        -# The start address of the pool must be 32-bit aligned
**        -# 168 bytes are used for internal bookkeeping, therefore, they will not be available for allocation.
**
** \param[in]   HandlePtr      A pointer to the variable the caller wishes to have the memory pool handle kept in.
**
** \param[in]   MemPtr         A Pointer to the pool of memory created by the calling application. This address must
**                             be on a 32-bit boundary.
**
** \param[in]   Size           The size of the pool of memory.  Note that this must be an integral number of 32 bit words.
**
** \param[in]   NumBlockSizes  The number of different block sizes specified in the \c BlockSizes array. If set equal to
**                             zero or if greater than 17, then default block sizes are used.
**
** \param[in]   BlockSizes     Pointer to an array of sizes to be used instead of the default block sizes specified by
**                             #CFE_PLATFORM_ES_MEM_BLOCK_SIZE_01 through #CFE_PLATFORM_ES_MAX_BLOCK_SIZE.  If the pointer is equal to NULL,
**                             the default block sizes are used.
**
** \param[in]   UseMutex       Flag indicating whether the new memory pool will be processing with mutex handling or not.
**                             Valid parameter values are #CFE_ES_USE_MUTEX and #CFE_ES_NO_MUTEX
**
** \param[out]  *HandlePtr     The memory pool handle.
**
** \returns
** \retcode #CFE_SUCCESS           \retdesc \copydoc CFE_SUCCESS          \endcode
** \retcode #CFE_ES_BAD_ARGUMENT   \retdesc \copydoc CFE_ES_BAD_ARGUMENT  \endcode
** \endreturns
**
** \sa #CFE_ES_PoolCreate, #CFE_ES_PoolCreateNoSem, #CFE_ES_GetPoolBuf, #CFE_ES_PutPoolBuf, #CFE_ES_GetMemPoolStats
**
******************************************************************************/
int32 CFE_ES_PoolCreateEx(CFE_ES_MemHandle_t *HandlePtr, uint8 *MemPtr, uint32 Size, uint32 NumBlockSizes, uint32 *BlockSizes, uint16 UseMutex);

/*****************************************************************************/
/**
** \brief Gets a buffer from the memory pool created by #CFE_ES_PoolCreate or #CFE_ES_PoolCreateNoSem
**
** \par Description
**        This routine obtains a block of memory from the memory pool supplied by the calling application.    
**
** \par Assumptions, External Events, and Notes:
**        -# The size allocated from the memory pool is, at a minimum, 12 bytes more than requested.
**
** \param[in]   BufPtr      A pointer to the Application's pointer in which will be stored the address of the allocated memory buffer.
**
** \param[in]   HandlePtr   The handle to the memory pool as returned by #CFE_ES_PoolCreate or #CFE_ES_PoolCreateNoSem.
**
** \param[in]   Size        The size of the buffer requested.  NOTE: The size allocated may be larger.
**
** \param[out]  *BufPtr     The address of the requested buffer.
**
** \returns
** \retstmt When successful, the return value is a positive number and is the number of bytes 
**          actually allocated for the buffer. \endcode
** \retcode #CFE_ES_ERR_MEM_HANDLE      \retdesc \copydoc CFE_ES_ERR_MEM_HANDLE       \endcode
** \retcode #CFE_ES_ERR_MEM_BLOCK_SIZE  \retdesc \copydoc CFE_ES_ERR_MEM_BLOCK_SIZE   \endcode
** \endreturns
**
** \sa #CFE_ES_PoolCreate, #CFE_ES_PoolCreateNoSem, #CFE_ES_PoolCreateEx, #CFE_ES_PutPoolBuf, #CFE_ES_GetMemPoolStats, #CFE_ES_GetPoolBufInfo
**
******************************************************************************/
int32 CFE_ES_GetPoolBuf(uint32 **BufPtr, CFE_ES_MemHandle_t HandlePtr, uint32 Size);

/*****************************************************************************/
/**
** \brief Gets info on a buffer previously allocated via #CFE_ES_GetPoolBuf
**
** \par Description
**        This routine gets info on a buffer in the memory pool.    
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in]   HandlePtr   The handle to the memory pool as returned by #CFE_ES_PoolCreate or #CFE_ES_PoolCreateNoSem.
**
** \param[in]   BufPtr      A pointer to the memory buffer to provide status for.
**
** \returns
** \retstmt When successful, the return value is a positive number and is the number of bytes actually allocated. \endcode
** \retcode #CFE_ES_ERR_MEM_HANDLE      \retdesc \copydoc CFE_ES_ERR_MEM_HANDLE   \endcode
** \retcode #CFE_ES_BUFFER_NOT_IN_POOL  \retdesc \copydoc CFE_ES_BUFFER_NOT_IN_POOL   \endcode
** \endreturns
**
** \sa #CFE_ES_PoolCreate, #CFE_ES_PoolCreateNoSem, #CFE_ES_PoolCreateEx, #CFE_ES_GetPoolBuf, #CFE_ES_GetMemPoolStats, #CFE_ES_PutPoolBuf
**
******************************************************************************/
int32 CFE_ES_GetPoolBufInfo(CFE_ES_MemHandle_t HandlePtr, uint32 *BufPtr);

/*****************************************************************************/
/**
** \brief Releases a buffer from the memory pool that was previously allocated via #CFE_ES_GetPoolBuf
**
** \par Description
**        This routine releases a buffer back into the memory pool.    
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in]   HandlePtr   The handle to the memory pool as returned by #CFE_ES_PoolCreate or #CFE_ES_PoolCreateNoSem.
**
** \param[in]   BufPtr      A pointer to the memory buffer to be released.
**
** \returns
** \retstmt When successful, the return value is a positive number and is the number of bytes actually released. \endcode
** \retcode #CFE_ES_ERR_MEM_HANDLE      \retdesc \copydoc CFE_ES_ERR_MEM_HANDLE   \endcode
** \endreturns
**
** \sa #CFE_ES_PoolCreate, #CFE_ES_PoolCreateNoSem, #CFE_ES_PoolCreateEx, #CFE_ES_GetPoolBuf, #CFE_ES_GetMemPoolStats, #CFE_ES_GetPoolBufInfo
**
******************************************************************************/
int32 CFE_ES_PutPoolBuf(CFE_ES_MemHandle_t HandlePtr, uint32 *BufPtr);

/*****************************************************************************/
/**
** \brief Extracts the statistics maintained by the memory pool software
**
** \par Description
**        This routine fills the #CFE_ES_MemPoolStats_t data structure with the statistics
**        maintained by the memory pool software.  These statistics can then be telemetered
**        by the calling Application.    
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in]   BufPtr      Pointer to #CFE_ES_MemPoolStats_t data structure to be 
**                          filled with memory statistics.
**
** \param[in]   Handle      The handle to the memory pool whose statistics are desired.
**
** \param[out] *BufPtr      Memory Pool Statistics stored in given data structure.
**
** \returns
** \retcode #CFE_SUCCESS                \retdesc \copydoc CFE_SUCCESS            \endcode
** \retcode #CFE_ES_ERR_MEM_HANDLE      \retdesc \copydoc CFE_ES_ERR_MEM_HANDLE  \endcode
** \endreturns
**
** \sa #CFE_ES_PoolCreate, #CFE_ES_PoolCreateNoSem, #CFE_ES_PoolCreateEx, #CFE_ES_GetPoolBuf, #CFE_ES_PutPoolBuf
**
******************************************************************************/
int32 CFE_ES_GetMemPoolStats(CFE_ES_MemPoolStats_t *BufPtr, CFE_ES_MemHandle_t  Handle);

/*
** Performance Log API
*/

/*****************************************************************************/
/**
** \brief Entry marker for use with Software Performance Analysis Tool.
**
** \par Description
**        This macro logs the entry or start event/marker for the specified 
**        entry \c id. This macro, in conjunction with the #CFE_ES_PerfLogExit, 
**        is used by the Software Performance Analysis tool (see section 5.15).    
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in]   id          Identifier of the specific event or marker.
**
** \sa #CFE_ES_PerfLogExit, #CFE_ES_PerfLogAdd
**
******************************************************************************/
#define CFE_ES_PerfLogEntry(id) (CFE_ES_PerfLogAdd(id, 0))

/*****************************************************************************/
/**
** \brief Exit marker for use with Software Performance Analysis Tool.
**
** \par Description
**        This macro logs the exit or end event/marker for the specified 
**        entry \c id. This macro, in conjunction with the #CFE_ES_PerfLogEntry, 
**        is used by the Software Performance Analysis tool (see section 5.15).    
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in]   id          Identifier of the specific event or marker.
**
** \sa #CFE_ES_PerfLogEntry, #CFE_ES_PerfLogAdd
**
******************************************************************************/
#define CFE_ES_PerfLogExit(id) (CFE_ES_PerfLogAdd(id, 1))


/*****************************************************************************/
/**
** \brief Function called by CFE_ES_PerfLogEntry and CFE_ES_PerfLogExit macros
**
** \par Description
**        This function logs the entry and exit marker for the specified 
**        \c id. This function is used by the Software Performance Analysis 
**        tool (see section 5.15).    
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in]   Marker          Identifier of the specific event or marker.
** \param[in]   EntryExit       Used to specify Entry(0) or Exit(1)
**
** \sa #CFE_ES_PerfLogEntry, #CFE_ES_PerfLogExit
**
******************************************************************************/
void CFE_ES_PerfLogAdd(uint32 Marker, uint32 EntryExit);

/*
** Generic Counter API
*/

/*****************************************************************************/
/**
** \brief Register a generic counter
**
** \par Description
**        This routine registers a generic counter. 
**
** \par Assumptions, External Events, and Notes:
**        None.
**
** \param[in]  *CounterName   The Name of the generic counter.
**
** \param[out] *CounterIdPtr  The Counter Id of the newly created counter.
**
** \returns
** \retcode #CFE_SUCCESS                \retdesc \copydoc CFE_SUCCESS            \endcode
** \retcode #CFE_ES_BAD_ARGUMENT      \retdesc \copydoc CFE_ES_BAD_ARGUMENT  \endcode
** \endreturns
**
** \sa #CFE_ES_IncrementGenCounter, #CFE_ES_DeleteGenCounter, #CFE_ES_SetGenCount, #CFE_ES_GetGenCount, #CFE_ES_GetGenCounterIDByName
**
******************************************************************************/
int32 CFE_ES_RegisterGenCounter(uint32 *CounterIdPtr, const char *CounterName);

/*****************************************************************************/
/**
** \brief Delete a generic counter
**
** \par Description
**        This routine deletes a previously registered generic counter. 
**
** \par Assumptions, External Events, and Notes:
**        None.
**
** \param[in]  CounterId     The Counter Id of the newly created counter.
**
** \returns
** \retcode #CFE_SUCCESS                \retdesc \copydoc CFE_SUCCESS            \endcode
** \retcode #CFE_ES_BAD_ARGUMENT      \retdesc \copydoc CFE_ES_BAD_ARGUMENT  \endcode
** \endreturns
**
** \sa #CFE_ES_IncrementGenCounter, #CFE_ES_RegisterGenCounter, #CFE_ES_SetGenCount, #CFE_ES_GetGenCount, #CFE_ES_GetGenCounterIDByName
**
******************************************************************************/
int32 CFE_ES_DeleteGenCounter(uint32 CounterId);

/*****************************************************************************/
/**
** \brief Increments the specified generic counter
**
** \par Description
**        This routine increments the specified generic counter. 
**
** \par Assumptions, External Events, and Notes:
**        None.
**
** \param[in]   CounterId    The Counter to be incremented.
**
** \returns
** \retcode #CFE_SUCCESS                \retdesc \copydoc CFE_SUCCESS            \endcode
** \retcode #CFE_ES_BAD_ARGUMENT      \retdesc \copydoc CFE_ES_BAD_ARGUMENT  \endcode
** \endreturns
**
** \sa #CFE_ES_RegisterGenCounter, #CFE_ES_DeleteGenCounter, #CFE_ES_SetGenCount, #CFE_ES_GetGenCount, #CFE_ES_GetGenCounterIDByName
**
******************************************************************************/
int32 CFE_ES_IncrementGenCounter(uint32 CounterId);

/*****************************************************************************/
/**
** \brief Set the specified generic counter
**
** \par Description
**        This routine sets the specified generic counter to the specified value. 
**
** \par Assumptions, External Events, and Notes:
**        None.
**
** \param[in]   CounterId    The Counter to be set.
**
** \param[in]   Count        The new value of the Counter.
**
** \returns
** \retcode #CFE_SUCCESS                \retdesc \copydoc CFE_SUCCESS            \endcode
** \retcode #CFE_ES_BAD_ARGUMENT      \retdesc \copydoc CFE_ES_BAD_ARGUMENT  \endcode
** \endreturns
**
** \sa #CFE_ES_RegisterGenCounter, #CFE_ES_DeleteGenCounter, #CFE_ES_IncrementGenCounter, #CFE_ES_GetGenCount, #CFE_ES_GetGenCounterIDByName
**
******************************************************************************/
int32 CFE_ES_SetGenCount(uint32 CounterId, uint32 Count);

/*****************************************************************************/
/**
** \brief Get the specified generic counter count
**
** \par Description
**        This routine gets the value of a generic counter. 
**
** \par Assumptions, External Events, and Notes:
**        None.
**
** \param[in]   CounterId    The Counter to get the value from.
**
** \param[in]   *Count     The value of the Counter.
**
** \returns
** \retcode #CFE_SUCCESS                \retdesc \copydoc CFE_SUCCESS            \endcode
** \retcode #CFE_ES_BAD_ARGUMENT      \retdesc \copydoc CFE_ES_BAD_ARGUMENT  \endcode
** \endreturns
**
** \sa #CFE_ES_RegisterGenCounter, #CFE_ES_DeleteGenCounter, #CFE_ES_SetGenCount, #CFE_ES_IncrementGenCounter, #CFE_ES_GetGenCounterIDByName
**
******************************************************************************/
int32 CFE_ES_GetGenCount(uint32 CounterId, uint32 *Count);


/*****************************************************************************/
/**
** \brief Get the Id associated with a generic counter name
**
** \par Description
**        This routine gets the Counter Id for a generic counter specified by name.
**
** \par Assumptions, External Events, and Notes:
**        None.
**
** \param[in]   *CounterName     The name of the Counter.
**
** \param[out]  *CounterIdPtr    The Counter Id for the given name.
**
** \returns
** \retcode #CFE_SUCCESS                \retdesc \copydoc CFE_SUCCESS            \endcode
** \retcode #CFE_ES_BAD_ARGUMENT      \retdesc \copydoc CFE_ES_BAD_ARGUMENT  \endcode
** \endreturns
**
** \sa #CFE_ES_RegisterGenCounter, #CFE_ES_DeleteGenCounter, #CFE_ES_SetGenCount, #CFE_ES_IncrementGenCounter, #CFE_ES_GetGenCount
******************************************************************************/
int32 CFE_ES_GetGenCounterIDByName(uint32 *CounterIdPtr, const char *CounterName);

/*****************************************************************************/
/**
** \brief Process an exception detected by the underlying OS/PSP
**
** \par Description
**        This hook routine is called from the PSP when an exception occurs
**
** \par Assumptions, External Events, and Notes:
**        None.
**
** \param[in]   HostTaskId       The OS (not OSAL) task ID
** \param[in]   ReasonString     Identifier from PSP
** \param[in]   ContextPointer   Context data from PSP
** \param[in]   ContextSize      Size of context data from PSP
**
******************************************************************************/
void CFE_ES_ProcessCoreException(uint32  HostTaskId,     const char *ReasonString,
                                 const uint32 *ContextPointer, uint32 ContextSize);



#endif  /* _cfe_es_ */
