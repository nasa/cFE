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

/*
**  File:
**    cfe_es_apps.c
**
**  Purpose:
**    This file contains functions for starting cFE applications from a filesystem.
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**     cFE Flight Software Application Developers Guide
**
**  Notes:
**
*/

/*
** Includes
*/
#include "cfe_es_module_all.h"

#include <stdio.h>
#include <string.h> /* memset() */
#include <fcntl.h>
#include <ctype.h>

#include "target_config.h"

/*
 * Simple state structure used when cleaning up objects associated with tasks
 *
 * This is used locally by CFE_ES_CleanupTaskResources
 */
typedef struct
{
    uint32 ErrorFlag;
    uint32 FoundObjects;
    uint32 PrevFoundObjects;
    uint32 DeletedObjects;
    int32  OverallStatus;
} CFE_ES_CleanupState_t;

/*
****************************************************************************
** Functions
***************************************************************************
*/

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_ES_AppRecord_t *CFE_ES_LockUserAppRecord(CFE_ES_AppId_t AppID, const char *LogMsgAction)
{
    CFE_ES_AppRecord_t *AppRecPtr;
    bool                IsValid;

    IsValid   = false;
    AppRecPtr = CFE_ES_LocateAppRecordByID(AppID);
    CFE_ES_LockSharedData(__func__, __LINE__);
    if (!CFE_ES_AppRecordIsMatch(AppRecPtr, AppID))
    {
        CFE_ES_WriteToSysLog("%s: Invalid Application ID received for %s, AppID = %lu\n",
                             __func__,
                             LogMsgAction,
                             CFE_RESOURCEID_TO_ULONG(AppID));
    }
    else if (AppRecPtr->Type == CFE_ES_AppType_CORE)
    {
        CFE_ES_SysLogWrite_Unsync("%s: Cannot %s a CORE Application: %s.\n",
                                  __func__,
                                  LogMsgAction,
                                  CFE_ES_AppRecordGetName(AppRecPtr));
    }
    else if (AppRecPtr->AppState != CFE_ES_AppState_RUNNING)
    {
        CFE_ES_SysLogWrite_Unsync("%s: Cannot %s Application %s, It is not running.\n",
                                  __func__,
                                  LogMsgAction,
                                  CFE_ES_AppRecordGetName(AppRecPtr));
    }
    else
    {
        IsValid = true;
    }

    if (!IsValid)
    {
        CFE_ES_UnlockSharedData(__func__, __LINE__);
        AppRecPtr = NULL;
    }

    return AppRecPtr;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_LoadModule(CFE_ResourceId_t                 ParentResourceId,
                        const char                      *ModuleName,
                        const CFE_ES_ModuleLoadParams_t *LoadParams,
                        CFE_ES_ModuleLoadStatus_t       *LoadStatus)
{
    osal_id_t ModuleId = OS_OBJECT_ID_UNDEFINED;
    cpuaddr   InitSymbolAddress;
    int32     ReturnCode;
    int32     OsStatus;
    uint32    LoadFlags;

    LoadFlags         = 0;
    InitSymbolAddress = 0;
    ReturnCode        = CFE_SUCCESS;

    if (LoadParams->FileName[0] != 0)
    {
        switch (CFE_ResourceId_GetBase(ParentResourceId))
        {
            case CFE_ES_APPID_BASE:
                /*
                 * Apps should not typically have symbols exposed to other apps.
                 *
                 * Keeping symbols local/private may help ensure this module is unloadable
                 * in the future, depending on underlying OS/loader implementation.
                 */
                LoadFlags |= OS_MODULE_FLAG_LOCAL_SYMBOLS;
                break;
            case CFE_ES_LIBID_BASE:
                /*
                 * Libraries need to have their symbols exposed to other apps.
                 *
                 * Note on some OS/loader implementations this may make it so the module
                 * cannot be unloaded, if there is no way to ensure that symbols
                 * are not being referenced.  CFE does not currently support unloading
                 * of libraries for this reason, among others.
                 */
                LoadFlags |= OS_MODULE_FLAG_GLOBAL_SYMBOLS;
                break;
            default:
                break;
        }

        /*
         * Load the module via OSAL.
         */
        OsStatus = OS_ModuleLoad(&ModuleId, ModuleName, LoadParams->FileName, LoadFlags);

        if (OsStatus != OS_SUCCESS)
        {
            CFE_ES_WriteToSysLog("%s: Could not load file:%s. EC = %ld\n",
                                 __func__,
                                 LoadParams->FileName,
                                 (long)OsStatus);
            ModuleId   = OS_OBJECT_ID_UNDEFINED;
            ReturnCode = CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
        }
    }
    else
    {
        ModuleId = OS_OBJECT_ID_UNDEFINED;
    }

    /*
     * If the Load was OK, then lookup the address of the entry point
     */
    if (ReturnCode == CFE_SUCCESS && LoadParams->InitSymbolName[0] != 0
        && strcmp(LoadParams->InitSymbolName, "NULL") != 0)
    {
        OsStatus = OS_ModuleSymbolLookup(ModuleId, &InitSymbolAddress, LoadParams->InitSymbolName);
        if (OsStatus != OS_SUCCESS)
        {
            CFE_ES_WriteToSysLog("%s: Could not find symbol:%s. EC = %ld\n",
                                 __func__,
                                 LoadParams->InitSymbolName,
                                 (long)OsStatus);
            ReturnCode = CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
        }
    }

    if (ReturnCode == CFE_SUCCESS)
    {
        /* store the data in the app record after successful load+lookup */
        LoadStatus->ModuleId          = ModuleId;
        LoadStatus->InitSymbolAddress = InitSymbolAddress;
    }
    else if (OS_ObjectIdDefined(ModuleId))
    {
        /* If the module had been successfully loaded, then unload it,
         * so that it does not consume resources */
        OsStatus = OS_ModuleUnload(ModuleId);
        if (OsStatus != OS_SUCCESS) /* There's not much we can do except notify */
        {
            CFE_ES_WriteToSysLog("%s: Failed to unload: %s. EC = %ld\n", __func__, ModuleName, (long)OsStatus);
        }
    }

    return ReturnCode;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_GetTaskFunction(CFE_ES_TaskEntryFuncPtr_t *FuncPtr)
{
    CFE_ES_TaskRecord_t      *TaskRecPtr;
    CFE_ES_TaskEntryFuncPtr_t EntryFunc;
    int32                     ReturnCode;
    int32                     Timeout;

    /*
     * Use the same timeout as was used for the startup script itself.
     */
    ReturnCode = CFE_ES_ERR_APP_REGISTER;
    Timeout    = CFE_PLATFORM_ES_STARTUP_SCRIPT_TIMEOUT_MSEC;
    EntryFunc  = NULL;

    while (true)
    {
        OS_TaskDelay(CFE_PLATFORM_ES_STARTUP_SYNC_POLL_MSEC);

        CFE_ES_LockSharedData(__func__, __LINE__);
        TaskRecPtr = CFE_ES_GetTaskRecordByContext();
        if (TaskRecPtr != NULL)
        {
            EntryFunc = TaskRecPtr->EntryFunc;
            if (CFE_RESOURCEID_TEST_DEFINED(TaskRecPtr->AppId) && EntryFunc != NULL)
            {
                ReturnCode = CFE_SUCCESS;
            }
        }
        CFE_ES_UnlockSharedData(__func__, __LINE__);

        if (ReturnCode == CFE_SUCCESS || Timeout <= 0)
        {
            /* end of loop condition */
            break;
        }

        Timeout -= CFE_PLATFORM_ES_STARTUP_SYNC_POLL_MSEC;
    }

    /* output function address to caller */
    if (FuncPtr != NULL)
    {
        *FuncPtr = EntryFunc;
    }

    return ReturnCode;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_ES_TaskEntryPoint(void)
{
    CFE_ES_TaskEntryFuncPtr_t RealEntryFunc;

    /* When CFE_ES_GetTaskFunction returns CFE_SUCCESS, RealEntryFunc != NULL
     * is implicit and therefore does not need to be checked here. This is
     * because in CFE_ES_GetTaskFunction, ReturnCode is set to CFE_SUCCESS only
     * if EntryFunc != 0, and *FuncPtr which is RealEntryFunc is set to
     * EntryFunc. Since FuncPtr != NULL is also implicitly true since FuncPtr =
     * &RealEntryFunc, FuncPtr != NULL will always evaluate to true. The
     * condition RealEntryFunc != NUL was therefore removed for coverage
     * purposes.*/
    if (CFE_ES_GetTaskFunction(&RealEntryFunc) == CFE_SUCCESS)
    {
        /*
         * Set the default exception environment, which should
         * be done serialized (i.e. only one task at a time should
         * call into CFE_PSP_SetDefaultExceptionEnvironment).
         */
        CFE_ES_LockSharedData(__func__, __LINE__);
        CFE_PSP_SetDefaultExceptionEnvironment();
        CFE_ES_UnlockSharedData(__func__, __LINE__);

        /*
         * Call the actual task entry function
         */
        (*RealEntryFunc)();
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_StartAppTask(CFE_ES_TaskId_t                *TaskIdPtr,
                          const char                     *TaskName,
                          CFE_ES_TaskEntryFuncPtr_t       EntryFunc,
                          const CFE_ES_TaskStartParams_t *Params,
                          CFE_ES_AppId_t                  ParentAppId)
{
    CFE_ES_TaskRecord_t *TaskRecPtr;
    osal_id_t            OsalTaskId = OS_OBJECT_ID_UNDEFINED;
    CFE_ES_TaskId_t      LocalTaskId;
    int32                OsStatus;
    int32                ReturnCode;

    /*
     * Create the primary task for the newly loaded task
     */
    OsStatus = OS_TaskCreate(&OsalTaskId,           /* task id */
                             TaskName,              /* task name matches app name for main task */
                             CFE_ES_TaskEntryPoint, /* task function pointer */
                             Params->StackPtr,      /* stack pointer (allocate if NULL) */
                             Params->StackSize,     /* stack size */
                             Params->Priority,      /* task priority */
                             OS_FP_ENABLED);        /* task options */

    CFE_ES_LockSharedData(__func__, __LINE__);

    if (OsStatus == OS_SUCCESS)
    {
        /*
         * As this is a newly-created task, this shouldn't fail.
         * The entry is not (yet) matching the task ID - it will be
         * initialized here.
         */
        LocalTaskId = CFE_ES_TaskId_FromOSAL(OsalTaskId);
        TaskRecPtr  = CFE_ES_LocateTaskRecordByID(LocalTaskId);
        if (CFE_ES_TaskRecordIsUsed(TaskRecPtr))
        {
            CFE_ES_SysLogWrite_Unsync("%s: Error: ES_TaskTable slot for ID %lx in use at task creation!\n",
                                      __func__,
                                      OS_ObjectIdToInteger(OsalTaskId));
        }

        /*
         * Clear any other/stale data that might be in the entry,
         * and reset all fields to the correct value.
         */
        memset(TaskRecPtr, 0, sizeof(*TaskRecPtr));

        TaskRecPtr->AppId       = ParentAppId;
        TaskRecPtr->EntryFunc   = EntryFunc;
        TaskRecPtr->StartParams = *Params;

        strncpy(TaskRecPtr->TaskName, TaskName, sizeof(TaskRecPtr->TaskName) - 1);
        TaskRecPtr->TaskName[sizeof(TaskRecPtr->TaskName) - 1] = 0;

        CFE_ES_TaskRecordSetUsed(TaskRecPtr, CFE_RESOURCEID_UNWRAP(LocalTaskId));

        /*
         * Increment the registered Task count.
         */
        CFE_ES_Global.RegisteredTasks++;
        ReturnCode = CFE_SUCCESS;
        *TaskIdPtr = CFE_ES_TaskRecordGetID(TaskRecPtr);
    }
    else
    {
        CFE_ES_SysLogWrite_Unsync("%s: AppCreate Error: TaskCreate %s Failed. EC = %ld!\n",
                                  __func__,
                                  TaskName,
                                  (long)OsStatus);
        ReturnCode = CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
        *TaskIdPtr = CFE_ES_TASKID_UNDEFINED;
    }

    CFE_ES_UnlockSharedData(__func__, __LINE__);

    return ReturnCode;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_AppCreate(CFE_ES_AppId_t *ApplicationIdPtr, const char *AppName, const CFE_ES_AppStartParams_t *Params)
{
    CFE_Status_t        Status;
    CFE_Status_t        CleanupStatus;
    CFE_ES_AppRecord_t *AppRecPtr;
    CFE_ResourceId_t    PendingResourceId = CFE_RESOURCEID_UNDEFINED;

    /*
     * The AppName must not be NULL
     */
    if (AppName == NULL || Params == NULL)
    {
        return CFE_ES_BAD_ARGUMENT;
    }

    /* Confirm name will fit inside the record */
    if (memchr(AppName, 0, sizeof(AppRecPtr->AppName)) == NULL)
    {
        return CFE_ES_BAD_ARGUMENT;
    }

    /*
    ** Allocate an ES_AppTable entry
    */
    CFE_ES_LockSharedData(__func__, __LINE__);

    /*
    ** Find an ES AppTable entry, and set to RESERVED
    **
    ** In this state, the entry is no longer free, but also will not pass the
    ** validation test.  So this function effectively has exclusive access
    ** without holding the global lock.
    **
    ** IMPORTANT: it must set the ID to something else before leaving
    ** this function or else the resource will be leaked.  After this
    ** point, execution must proceed to the end of the function to
    ** guarantee that the entry is either completed or freed.
    */

    /*
     * Check for an existing entry with the same name.
     * Also check for a matching Library name.
     * (Apps and libraries should be uniquely named)
     */
    AppRecPtr = CFE_ES_LocateAppRecordByName(AppName);
    if (AppRecPtr != NULL)
    {
        CFE_ES_SysLogWrite_Unsync("%s: Duplicate app name '%s'\n", __func__, AppName);
        Status = CFE_ES_ERR_DUPLICATE_NAME;
    }
    else
    {
        /* scan for a free slot */
        PendingResourceId = CFE_ResourceId_FindNext(CFE_ES_Global.LastAppId,
                                                    CFE_PLATFORM_ES_MAX_APPLICATIONS,
                                                    CFE_ES_CheckAppIdSlotUsed);
        AppRecPtr         = CFE_ES_LocateAppRecordByID(CFE_ES_APPID_C(PendingResourceId));

        if (AppRecPtr == NULL)
        {
            CFE_ES_SysLogWrite_Unsync("%s: No free application slots available\n", __func__);
            Status = CFE_ES_NO_RESOURCE_IDS_AVAILABLE;
        }
        else
        {
            /* Fully clear the entry, just in case of stale data */
            memset(AppRecPtr, 0, sizeof(*AppRecPtr));

            /* Store the app name from passed-in value */
            strncpy(AppRecPtr->AppName, AppName, sizeof(AppRecPtr->AppName) - 1);

            AppRecPtr->Type = CFE_ES_AppType_EXTERNAL;

            /*
             * Fill out the parameters in the StartParams sub-structure
             *
             * This contains all relevant info, including file name, entry point,
             * main task info, etc. which is required to start the app now
             * or in a future restart/reload request.
             */
            AppRecPtr->StartParams = *Params;

            /*
             * Fill out the Task State info
             */
            AppRecPtr->ControlReq.AppControlRequest = CFE_ES_RunStatus_APP_RUN;
            AppRecPtr->ControlReq.AppTimerMsec      = 0;

            CFE_ES_AppRecordSetUsed(AppRecPtr, CFE_RESOURCEID_RESERVED);
            CFE_ES_Global.LastAppId = PendingResourceId;
            Status                  = CFE_SUCCESS;
        }
    }

    CFE_ES_UnlockSharedData(__func__, __LINE__);

    /*
     * If ID allocation was not successful, return now.
     * A message regarding the issue should have already been logged
     */
    if (Status != CFE_SUCCESS)
    {
        return Status;
    }

    /*
     * Load the module based on StartParams configured above.
     */
    Status = CFE_ES_LoadModule(PendingResourceId, AppName, &AppRecPtr->StartParams.BasicInfo, &AppRecPtr->LoadStatus);

    /*
     * If the Load was OK, then complete the initialization
     */
    if (Status == CFE_SUCCESS)
    {
        Status =
            CFE_ES_StartAppTask(&AppRecPtr->MainTaskId, /* Task ID (output) stored in App Record as main task */
                                AppName,                /* Main Task name matches app name */
                                (CFE_ES_TaskEntryFuncPtr_t)
                                    AppRecPtr->LoadStatus.InitSymbolAddress, /* Init Symbol is main task entry point */
                                &AppRecPtr->StartParams.MainTaskInfo,        /* Main task parameters */
                                CFE_ES_APPID_C(PendingResourceId));          /* Parent App ID */
    }

    /*
     * Finalize data in the app table entry, which must be done under lock.
     * This transitions the entry from being RESERVED to the real ID.
     */
    CFE_ES_LockSharedData(__func__, __LINE__);

    if (Status == CFE_SUCCESS)
    {
        /*
         * important - set the ID to its proper value
         * which turns this into a real/valid table entry
         */
        CFE_ES_AppRecordSetUsed(AppRecPtr, PendingResourceId);

        /*
         ** Increment the registered App counter.
         */
        CFE_ES_Global.RegisteredExternalApps++;
    }
    else
    {
        /*
         * Set the table entry back to free
         */
        if (OS_ObjectIdDefined(AppRecPtr->LoadStatus.ModuleId))
        {
            CleanupStatus = OS_ModuleUnload(AppRecPtr->LoadStatus.ModuleId);
            if (CleanupStatus != OS_SUCCESS)
            {
                CFE_ES_WriteToSysLog("%s: Module (ID:0x%08lX) Unload failed. RC=%ld\n",
                                     __func__,
                                     OS_ObjectIdToInteger(AppRecPtr->LoadStatus.ModuleId),
                                     (long)CleanupStatus);
            }
        }

        CFE_ES_AppRecordSetFree(AppRecPtr);
        PendingResourceId = CFE_RESOURCEID_UNDEFINED;
    }

    CFE_ES_UnlockSharedData(__func__, __LINE__);

    *ApplicationIdPtr = CFE_ES_APPID_C(PendingResourceId);

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_LoadLibrary(CFE_ES_LibId_t *LibraryIdPtr, const char *LibName, const CFE_ES_ModuleLoadParams_t *Params)
{
    CFE_ES_LibraryEntryFuncPtr_t FunctionPointer;
    CFE_ES_LibRecord_t          *LibSlotPtr;
    int32                        Status;
    CFE_ResourceId_t             PendingResourceId;

    /*
     * The LibName must not be NULL
     */
    if (LibName == NULL || Params == NULL)
    {
        return CFE_ES_BAD_ARGUMENT;
    }

    /* Confirm name will fit inside the record */
    if (memchr(LibName, 0, sizeof(LibSlotPtr->LibName)) == NULL)
    {
        return CFE_ES_BAD_ARGUMENT;
    }

    /*
    ** Allocate an ES_LibTable entry
    */
    PendingResourceId = CFE_RESOURCEID_UNDEFINED;

    /*
    ** Find an ES AppTable entry, and set to RESERVED
    **
    ** In this state, the entry is no longer free, but also will not pass the
    ** validation test.  So this function effectively has exclusive access
    ** without holding the global lock.
    **
    ** IMPORTANT: it must set the ID to something else before leaving
    ** this function or else the resource will be leaked.  After this
    ** point, execution must proceed to the end of the function to
    ** guarantee that the entry is either completed or freed.
    */
    CFE_ES_LockSharedData(__func__, __LINE__);

    /*
     * Check for an existing entry with the same name.
     * Also check for a matching Library name.
     * (Libs and libraries should be uniquely named)
     */
    LibSlotPtr = CFE_ES_LocateLibRecordByName(LibName);
    if (LibSlotPtr != NULL || CFE_ES_LocateAppRecordByName(LibName) != NULL)
    {
        CFE_ES_SysLogWrite_Unsync("%s: Duplicate Lib name '%s'\n", __func__, LibName);
        if (LibSlotPtr != NULL)
        {
            PendingResourceId = CFE_RESOURCEID_UNWRAP(CFE_ES_LibRecordGetID(LibSlotPtr));
        }
        Status = CFE_ES_ERR_DUPLICATE_NAME;
    }
    else
    {
        /* scan for a free slot */
        PendingResourceId =
            CFE_ResourceId_FindNext(CFE_ES_Global.LastLibId, CFE_PLATFORM_ES_MAX_LIBRARIES, CFE_ES_CheckLibIdSlotUsed);
        LibSlotPtr = CFE_ES_LocateLibRecordByID(CFE_ES_LIBID_C(PendingResourceId));

        if (LibSlotPtr == NULL)
        {
            CFE_ES_SysLogWrite_Unsync("%s: No free library slots available\n", __func__);
            Status = CFE_ES_NO_RESOURCE_IDS_AVAILABLE;
        }
        else
        {
            /* Fully clear the entry, just in case of stale data */
            memset(LibSlotPtr, 0, sizeof(*LibSlotPtr));

            /*
             * Fill out the parameters in the AppStartParams sub-structure
             */
            strncpy(LibSlotPtr->LibName, LibName, sizeof(LibSlotPtr->LibName) - 1);
            LibSlotPtr->LibName[sizeof(LibSlotPtr->LibName) - 1] = '\0';
            LibSlotPtr->LoadParams                               = *Params;

            CFE_ES_LibRecordSetUsed(LibSlotPtr, CFE_RESOURCEID_RESERVED);
            CFE_ES_Global.LastLibId = PendingResourceId;
            Status                  = CFE_SUCCESS;
        }
    }

    CFE_ES_UnlockSharedData(__func__, __LINE__);

    /*
     * If any off-nominal condition exists, skip the rest of this logic.
     * (Log message already written)
     */
    if (Status != CFE_SUCCESS)
    {
        *LibraryIdPtr = CFE_ES_LIBID_C(PendingResourceId);
        return Status;
    }

    /*
     * Load the module based on StartParams configured above.
     */
    Status = CFE_ES_LoadModule(PendingResourceId, LibName, &LibSlotPtr->LoadParams, &LibSlotPtr->LoadStatus);
    if (Status == CFE_SUCCESS)
    {
        FunctionPointer = (CFE_ES_LibraryEntryFuncPtr_t)LibSlotPtr->LoadStatus.InitSymbolAddress;
        if (FunctionPointer != NULL)
        {
            Status = (*FunctionPointer)(CFE_ES_LIBID_C(PendingResourceId));
            if (Status != CFE_SUCCESS)
            {
                CFE_ES_WriteToSysLog("%s: Load Shared Library Init Error = 0x%08x\n", __func__, (unsigned int)Status);
            }
        }
    }

    /*
     * Finalize data in the app table entry, which must be done under lock.
     * This transitions the entry from being RESERVED to the real type,
     * either MAIN_TASK (success) or returning to INVALID (failure).
     */
    CFE_ES_LockSharedData(__func__, __LINE__);

    if (Status == CFE_SUCCESS)
    {
        /*
         * important - set the ID to its proper value
         * which turns this into a real/valid table entry
         */
        CFE_ES_LibRecordSetUsed(LibSlotPtr, PendingResourceId);

        /*
         * Increment the registered Lib counter.
         */
        CFE_ES_Global.RegisteredLibs++;
    }
    else
    {
        CFE_ES_LibRecordSetFree(LibSlotPtr);
        PendingResourceId = CFE_RESOURCEID_UNDEFINED;
    }

    CFE_ES_UnlockSharedData(__func__, __LINE__);

    *LibraryIdPtr = CFE_ES_LIBID_C(PendingResourceId);

    return Status;
}

/*----------------------------------------------------------------
 *
 * Local helper function
 * Collects the set of resources that are associated with the app being cleaned up
 * This only marks them for removal, it does not fully remove yet.
 *
 *-----------------------------------------------------------------*/
static CFE_Status_t CFE_ES_CleanUpApp_GetResources(CFE_ES_AppId_t      AppId,
                                                   osal_id_t          *ModuleIdPtr,
                                                   CFE_ES_TaskId_t    *TaskList,
                                                   CFE_ES_MemHandle_t *PoolList)
{
    int32                   ReturnCode;
    CFE_ES_AppRecord_t     *AppRecPtr;
    CFE_ES_TaskRecord_t    *TaskRecPtr;
    CFE_ES_MemPoolRecord_t *MemPoolRecPtr;
    size_t                  i;
    size_t                  NumTasks;
    size_t                  NumPools;

    NumTasks   = 0;
    NumPools   = 0;
    ReturnCode = CFE_SUCCESS;

    AppRecPtr = CFE_ES_LocateAppRecordByID(AppId);

    /*
     * Collect a list of resources previously owned by this app, which
     * must be done while the global data is locked.
     */
    CFE_ES_LockSharedData(__func__, __LINE__);

    if (CFE_ES_AppRecordIsMatch(AppRecPtr, AppId))
    {
        if (AppRecPtr->Type == CFE_ES_AppType_EXTERNAL)
        {
            CFE_ES_Global.RegisteredExternalApps--;

            /*
             * Get the Module ID, if it was an external app
             *
             * (this will be OS_OBJECT_ID_UNDEFINED if it was not loaded dynamically)
             */
            *ModuleIdPtr = AppRecPtr->LoadStatus.ModuleId;
        }

        /*
         * Collect all tasks associated with this app
         */
        TaskRecPtr = CFE_ES_Global.TaskTable;
        for (i = 0; i < OS_MAX_TASKS; i++)
        {
            if (CFE_ES_TaskRecordIsUsed(TaskRecPtr) && CFE_RESOURCEID_TEST_EQUAL(TaskRecPtr->AppId, AppId))
            {
                TaskList[NumTasks] = CFE_ES_TaskRecordGetID(TaskRecPtr);

                /* Store the main task ID at index 0 (swap with whatever was there) */
                if (CFE_RESOURCEID_TEST_EQUAL(TaskList[NumTasks], AppRecPtr->MainTaskId) && NumTasks != 0)
                {
                    TaskList[NumTasks] = TaskList[0];
                    TaskList[0]        = AppRecPtr->MainTaskId;
                }

                /* Mark record for removal */
                CFE_ES_TaskRecordSetUsed(TaskRecPtr, CFE_RESOURCEID_RESERVED);
                ++NumTasks;
            }

            ++TaskRecPtr;
        }

        CFE_ES_Global.RegisteredTasks -= NumTasks;

        /*
         * Collect memory pools associated with this app
         */
        MemPoolRecPtr = CFE_ES_Global.MemPoolTable;
        for (i = 0; i < CFE_PLATFORM_ES_MAX_MEMORY_POOLS; i++)
        {
            if (CFE_ES_MemPoolRecordIsUsed(MemPoolRecPtr)
                && CFE_RESOURCEID_TEST_EQUAL(MemPoolRecPtr->OwnerAppID, AppId))
            {
                PoolList[NumPools] = CFE_ES_MemPoolRecordGetID(MemPoolRecPtr);
                ++NumPools;
            }

            ++MemPoolRecPtr;
        }

        /*
         * Set the record to RESERVED.
         *
         * This prevents reallocation of this slot while the remainder
         * of resources are freed.
         */
        CFE_ES_AppRecordSetUsed(AppRecPtr, CFE_RESOURCEID_RESERVED);
    }
    else
    {
        CFE_ES_SysLogWrite_Unsync("%s: AppID %lu is not valid for deletion\n",
                                  __func__,
                                  CFE_RESOURCEID_TO_ULONG(AppId));
        ReturnCode = CFE_ES_APP_CLEANUP_ERR;
    }

    CFE_ES_UnlockSharedData(__func__, __LINE__);

    /* clear out any remaining entries in the lists */
    for (i = NumTasks; i < OS_MAX_TASKS; ++i)
    {
        TaskList[i] = CFE_ES_TASKID_UNDEFINED;
    }

    for (i = NumPools; i < CFE_PLATFORM_ES_MAX_MEMORY_POOLS; ++i)
    {
        PoolList[i] = CFE_ES_MEMHANDLE_UNDEFINED;
    }

    return ReturnCode;
}

/*----------------------------------------------------------------
 *
 * Local helper function
 * Frees resources associated with this app in _other_ core modules
 * This is done by invoking the notification callback for that module
 *
 * Returns the number of callbacks that returned non-success.  Should be 0.
 *
 *-----------------------------------------------------------------*/
static uint32 CFE_ES_CleanUpApp_FreeCoreResources(CFE_ES_AppId_t AppId)
{
    /*
     * Now actually delete all the resources associated with the task.
     *
     * Most of this involves calling into other subsystems, so it is
     * done while the ES global data is UNLOCKED to avoid holding more
     * than one lock at a time.
     */

    CFE_Status_t                Status;
    uint32                      i;
    uint32                      FailCount;
    const Target_ObjectTable_t *Entry;

    /* Cleaning up resource is "best effort" -- the app is gone, so
     * we reclaim everything assocated with that app if possible.  If
     * failures occur, that should not stop the rest of cleanup. */
    FailCount = 0;

    i = 0;
    while (true)
    {
        Entry = GLOBAL_CONFIGDATA.CoreObjectTable[i];
        if (Entry == NULL)
        {
            /* this list is always terminated with a NULL entry */
            break;
        }

        if (Entry->AppCleanupCb != NULL)
        {
            /* Notify the core module that the app is going away.  This is so those
             * modules can also reclaim any resources associated with this app */
            Status = Entry->AppCleanupCb(CFE_RESOURCEID_TO_ULONG(AppId));
            if (Status != CFE_SUCCESS)
            {
                CFE_ES_WriteToSysLog("%s: Error returned from app Cleanup for %s: EC = 0x%08X\n",
                                     __func__,
                                     Entry->Name,
                                     (unsigned int)Status);
                ++FailCount;
            }
        }

        ++i;
    }

    return FailCount;
}

/*----------------------------------------------------------------
 *
 * Local helper function
 * Frees task resources associated with this app, for every task
 * that was identified for removal
 *
 * Returns the number of task cleanups that returned non-success.  Should be 0.
 *
 *-----------------------------------------------------------------*/
static uint32 CFE_ES_CleanUpApp_FreeTasks(CFE_ES_AppId_t AppId, const CFE_ES_TaskId_t *TaskList)
{
    size_t       i;
    uint32       FailCount;
    CFE_Status_t Status;

    /*
     * Delete all tasks.
     *
     * Note that the main task is always positioned at index 0 in this list.
     *
     * This iterates the list in reverse order, such that the child
     * tasks are deleted first (in any order) and main task is deleted last.
     */
    FailCount = 0;
    i         = OS_MAX_TASKS;
    while (i > 0)
    {
        --i;
        if (CFE_RESOURCEID_TEST_DEFINED(TaskList[i]))
        {
            Status = CFE_ES_CleanupTaskResources(TaskList[i]);
        }
        else
        {
            Status = CFE_SUCCESS;
        }
        if (Status != CFE_SUCCESS)
        {
            CFE_ES_WriteToSysLog("%s: CleanUpTaskResources for Task ID:%lu returned Error: 0x%08X\n",
                                 __func__,
                                 CFE_RESOURCEID_TO_ULONG(TaskList[i]),
                                 (unsigned int)Status);
            ++FailCount;
        }
    }
    return FailCount;
}

/*----------------------------------------------------------------
 *
 * Local helper function
 * Frees memory pool resources associated with this app, for every mempool
 * that was identified for removal
 *
 * Returns the number of cleanups that returned non-success.  Should be 0.
 *
 *-----------------------------------------------------------------*/
static uint32 CFE_ES_CleanUpApp_FreePools(CFE_ES_AppId_t AppId, const CFE_ES_MemHandle_t *PoolList)
{
    size_t       i;
    uint32       FailCount;
    CFE_Status_t Status;

    /*
     * Delete all mem pools.
     */
    FailCount = 0;
    for (i = 0; i < CFE_PLATFORM_ES_MAX_MEMORY_POOLS; ++i)
    {
        if (CFE_RESOURCEID_TEST_DEFINED(PoolList[i]))
        {
            Status = CFE_ES_PoolDelete(PoolList[i]);
        }
        else
        {
            Status = CFE_SUCCESS;
        }

        if (Status != CFE_SUCCESS)
        {
            CFE_ES_WriteToSysLog("%s: delete pool %lu returned Error: 0x%08X\n",
                                 __func__,
                                 CFE_RESOURCEID_TO_ULONG(PoolList[i]),
                                 (unsigned int)Status);
            ++FailCount;
        }
    }

    return FailCount;
}

/*----------------------------------------------------------------
 *
 * Local helper function
 * All task/app resources that were previously marked for removal are
 * fully removed here.  This cannot fail.
 *
 *-----------------------------------------------------------------*/
static void CFE_ES_CleanUpApp_Finish(CFE_ES_AppId_t AppId, const CFE_ES_TaskId_t *TaskList)
{
    CFE_ES_AppRecord_t  *AppRecPtr;
    CFE_ES_TaskRecord_t *TaskRecPtr;
    size_t               i;

    /*
     * Finally, re-acquire the ES lock and set all
     * table entries free for re-use.
     */
    CFE_ES_LockSharedData(__func__, __LINE__);

    /*
     * Free all task records.
     */
    for (i = 0; i < OS_MAX_TASKS; ++i)
    {
        /* note - if this returns NULL then CFE_ES_TaskRecordIsMatch will be false */
        TaskRecPtr = CFE_ES_LocateTaskRecordByID(TaskList[i]);
        if (CFE_ES_TaskRecordIsMatch(TaskRecPtr, CFE_ES_TASKID_C(CFE_RESOURCEID_RESERVED)))
        {
            CFE_ES_TaskRecordSetFree(TaskRecPtr);
        }
    }

    /*
     * Now finally delete the record and allow re-use of the slot.
     */
    AppRecPtr = CFE_ES_LocateAppRecordByID(AppId);
    if (CFE_ES_AppRecordIsMatch(AppRecPtr, CFE_ES_APPID_C(CFE_RESOURCEID_RESERVED)))
    {
        CFE_ES_AppRecordSetFree(AppRecPtr);
    }

    CFE_ES_UnlockSharedData(__func__, __LINE__);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_CleanUpApp(CFE_ES_AppId_t AppId)
{
    int32              ReturnCode;
    CFE_ES_TaskId_t    TaskList[OS_MAX_TASKS];
    CFE_ES_MemHandle_t PoolList[CFE_PLATFORM_ES_MAX_MEMORY_POOLS];
    osal_id_t          ModuleId;
    int32              OsStatus;
    uint32             FailCount;

    ModuleId  = OS_OBJECT_ID_UNDEFINED;
    FailCount = 0;
    memset(TaskList, 0, sizeof(TaskList));
    memset(PoolList, 0, sizeof(PoolList));

    /*
     * Collect a list of resources previously owned by this app, which
     * must be done while the global data is locked.
     */
    ReturnCode = CFE_ES_CleanUpApp_GetResources(AppId, &ModuleId, TaskList, PoolList);

    /* if that failed, do not do anything else */
    if (ReturnCode == CFE_SUCCESS)
    {
        /*
         * Now actually delete all the resources associated with the task.
         *
         * Most of this involves calling into other subsystems, so it is
         * done while the ES global data is UNLOCKED to avoid holding more
         * than one lock at a time.
         */
        FailCount += CFE_ES_CleanUpApp_FreeCoreResources(AppId);
        FailCount += CFE_ES_CleanUpApp_FreeTasks(AppId, TaskList);
        FailCount += CFE_ES_CleanUpApp_FreePools(AppId, PoolList);

        /*
        ** Unload the module, if applicable
        */
        if (OS_ObjectIdDefined(ModuleId))
        {
            /*
            ** Unload the module only if it is an external app
            */
            OsStatus = OS_ModuleUnload(ModuleId);
            if (OsStatus != OS_SUCCESS)
            {
                CFE_ES_WriteToSysLog("%s: Module (ID:0x%08lX) Unload failed. RC=%ld\n",
                                     __func__,
                                     OS_ObjectIdToInteger(ModuleId),
                                     (long)OsStatus);
                ++FailCount;
            }
        }

        /*
         * Finally, re-acquire the ES lock and set all
         * table entries free for re-use.
         */
        CFE_ES_CleanUpApp_Finish(AppId, TaskList);

        /*
         * The FailCount is the number of resources that were _not_ successfully reclaimed.
         * It should normally be 0.  This return value is for informational purposes only,
         * the caller should not really do anything differently - the app is gone as much
         * as it can be, even if this is nonzero.
         */
        if (FailCount != 0)
        {
            ReturnCode = CFE_ES_APP_CLEANUP_ERR;
        }
    }

    return ReturnCode;
}

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 * Purpose: clean up all objects.
 * NOTE: This is called while holding the ES global lock
 *
 *-----------------------------------------------------------------*/
void CFE_ES_CleanupObjectCallback(osal_id_t ObjectId, void *arg)
{
    CFE_ES_CleanupState_t *CleanState;
    int32                  OsStatus;
    osal_objtype_t         ObjType;
    bool                   ObjIsValid;
    CFE_Status_t           ErrorStatus = CFE_ES_APP_CLEANUP_ERR; // Set default cFE error status

    CleanState = (CFE_ES_CleanupState_t *)arg;
    ObjIsValid = true;

    ObjType = OS_IdentifyObject(ObjectId);
    switch (ObjType)
    {
        case OS_OBJECT_TYPE_OS_TASK:
            OsStatus    = OS_TaskDelete(ObjectId);
            ErrorStatus = CFE_ES_ERR_CHILD_TASK_DELETE;
            break;
        case OS_OBJECT_TYPE_OS_QUEUE:
            OsStatus    = OS_QueueDelete(ObjectId);
            ErrorStatus = CFE_ES_QUEUE_DELETE_ERR;
            break;
        case OS_OBJECT_TYPE_OS_BINSEM:
            OsStatus    = OS_BinSemDelete(ObjectId);
            ErrorStatus = CFE_ES_BIN_SEM_DELETE_ERR;
            break;
        case OS_OBJECT_TYPE_OS_COUNTSEM:
            OsStatus    = OS_CountSemDelete(ObjectId);
            ErrorStatus = CFE_ES_COUNT_SEM_DELETE_ERR;
            break;
        case OS_OBJECT_TYPE_OS_MUTEX:
            OsStatus    = OS_MutSemDelete(ObjectId);
            ErrorStatus = CFE_ES_MUT_SEM_DELETE_ERR;
            break;
        case OS_OBJECT_TYPE_OS_TIMECB:
            OsStatus    = OS_TimerDelete(ObjectId);
            ErrorStatus = CFE_ES_TIMER_DELETE_ERR;
            break;
        case OS_OBJECT_TYPE_OS_STREAM:
            OsStatus = OS_close(ObjectId);
            break;
        case OS_OBJECT_TYPE_OS_MODULE:
            OsStatus = OS_ModuleUnload(ObjectId);
            break;
        default:
            ObjIsValid = false;
            OsStatus   = OS_ERROR;
            break;
    }

    if (ObjIsValid)
    {
        ++CleanState->FoundObjects;
        if (OsStatus == OS_SUCCESS)
        {
            ++CleanState->DeletedObjects;
        }
        else
        {
            CFE_ES_SysLogWrite_Unsync("%s: Call to OSAL Delete Object (ID:%lu) failed. RC=%ld\n",
                                      __func__,
                                      OS_ObjectIdToInteger(ObjectId),
                                      (long)OsStatus);
            if (CleanState->OverallStatus == CFE_SUCCESS)
            {
                // Save the object-type-specific error status that was set earlier in the switch statement
                CleanState->OverallStatus = ErrorStatus;
            }
        }
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_CleanupTaskResources(CFE_ES_TaskId_t TaskId)
{
    CFE_ES_CleanupState_t CleanState;
    int32                 OsStatus;
    int32                 Result;
    osal_id_t             OsalId;

    /* Get the Task ID for calling OSAL APIs (convert type) */
    OsalId = CFE_ES_TaskId_ToOSAL(TaskId);

    /*
    ** Delete all OSAL resources that belong to this task
    */
    memset(&CleanState, 0, sizeof(CleanState));
    --CleanState.PrevFoundObjects;
    while (1)
    {
        OS_ForEachObject(OsalId, CFE_ES_CleanupObjectCallback, &CleanState);
        if (CleanState.FoundObjects == 0 || CleanState.ErrorFlag != 0)
        {
            break;
        }
        /*
         * The number of found objects should show a downward trend,
         * if not, then stop and do not loop here forever.  (This can
         * happen when using the UT stub functions, or if an object
         * cannot be fully deleted successfully).
         */
        CleanState.ErrorFlag =
            (CleanState.DeletedObjects == 0 || CleanState.FoundObjects >= CleanState.PrevFoundObjects);
        CleanState.PrevFoundObjects = CleanState.FoundObjects;
        CleanState.FoundObjects     = 0;
        CleanState.DeletedObjects   = 0;
    }

    /*
    ** Delete the task itself
    **
    ** Note, if the task self exited, then the ID becomes invalid.
    ** In this case the OS_ERR_INVALID_ID status is returned, but
    ** that is OK, there is nothing else needed to do.
    */
    OsStatus = OS_TaskDelete(OsalId);
    if (OsStatus == OS_SUCCESS || OsStatus == OS_ERR_INVALID_ID)
    {
        Result = CleanState.OverallStatus;
        if (Result == CFE_SUCCESS && CleanState.FoundObjects > 0)
        {
            /* Objects leftover after cleanup -- resource leak */
            Result = CFE_ES_APP_CLEANUP_ERR;
        }
    }
    else
    {
        Result = CFE_ES_TASK_DELETE_ERR;
    }

    return Result;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_ES_CopyModuleBasicInfo(const CFE_ES_ModuleLoadParams_t *ParamsPtr, CFE_ES_AppInfo_t *AppInfoPtr)
{
    strncpy(AppInfoPtr->EntryPoint, ParamsPtr->InitSymbolName, sizeof(AppInfoPtr->EntryPoint) - 1);
    AppInfoPtr->EntryPoint[sizeof(AppInfoPtr->EntryPoint) - 1] = '\0';

    strncpy(AppInfoPtr->FileName, ParamsPtr->FileName, sizeof(AppInfoPtr->FileName) - 1);
    AppInfoPtr->FileName[sizeof(AppInfoPtr->FileName) - 1] = '\0';
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_ES_CopyModuleStatusInfo(const CFE_ES_ModuleLoadStatus_t *StatusPtr, CFE_ES_AppInfo_t *AppInfoPtr)
{
    AppInfoPtr->StartAddress = CFE_ES_MEMADDRESS_C(StatusPtr->InitSymbolAddress);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_ES_CopyModuleAddressInfo(osal_id_t ModuleId, CFE_ES_AppInfo_t *AppInfoPtr)
{
    OS_module_prop_t ModuleInfo;
    int32            OsStatus;

    memset(&ModuleInfo, 0, sizeof(ModuleInfo));

    OsStatus = OS_ModuleInfo(ModuleId, &ModuleInfo);
    if (OsStatus == OS_SUCCESS)
    {
        AppInfoPtr->AddressesAreValid =
            (sizeof(ModuleInfo.addr.code_address) <= sizeof(AppInfoPtr->CodeAddress)) && ModuleInfo.addr.valid;
    }
    else
    {
        AppInfoPtr->AddressesAreValid = false;
    }

    /*
     * Convert the internal size and address to the telemetry format.
     * (The telemetry format may be a different bitwidth than the native processor)
     */
    AppInfoPtr->CodeAddress = CFE_ES_MEMADDRESS_C(ModuleInfo.addr.code_address);
    AppInfoPtr->CodeSize    = CFE_ES_MEMOFFSET_C(ModuleInfo.addr.code_size);
    AppInfoPtr->DataAddress = CFE_ES_MEMADDRESS_C(ModuleInfo.addr.data_address);
    AppInfoPtr->DataSize    = CFE_ES_MEMOFFSET_C(ModuleInfo.addr.data_size);
    AppInfoPtr->BSSAddress  = CFE_ES_MEMADDRESS_C(ModuleInfo.addr.bss_address);
    AppInfoPtr->BSSSize     = CFE_ES_MEMOFFSET_C(ModuleInfo.addr.bss_size);
}
