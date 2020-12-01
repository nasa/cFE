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
#include "private/cfe_private.h"
#include "cfe_es.h"
#include "cfe_psp.h"
#include "cfe_es_global.h"
#include "cfe_es_task.h"
#include "cfe_es_apps.h"
#include "cfe_es_log.h"
#include "cfe_es_resource.h"

#include <stdio.h>
#include <string.h> /* memset() */
#include <fcntl.h>

/*
** Defines
*/
#define ES_START_BUFF_SIZE 128

/*
**
**  Global Variables
**
*/

/*
****************************************************************************
** Functions
***************************************************************************
*/

/*
** Name:
**   CFE_ES_StartApplications
**
** Purpose:
**   This routine loads/starts cFE applications.
**
*/
void CFE_ES_StartApplications(uint32 ResetType, const char *StartFilePath )
{
   char ES_AppLoadBuffer[ES_START_BUFF_SIZE];  /* A buffer of for a line in a file */
   const char *TokenList[CFE_ES_STARTSCRIPT_MAX_TOKENS_PER_LINE];
   uint32      NumTokens;
   uint32      BuffLen;                            /* Length of the current buffer */
   osal_id_t   AppFile;
   int32       Status;
   char        c;
   bool        LineTooLong = false;
   bool        FileOpened = false;

   /*
   ** Get the ES startup script filename.
   ** If this is a Processor Reset, try to open the file in the volatile disk first.
   */
   if ( ResetType == CFE_PSP_RST_TYPE_PROCESSOR )
   {
      /*
      ** Open the file in the volatile disk.
      */
      Status = OS_OpenCreate(&AppFile, CFE_PLATFORM_ES_VOLATILE_STARTUP_FILE, OS_FILE_FLAG_NONE, OS_READ_ONLY);

      if ( Status >= 0 )
      {
         CFE_ES_WriteToSysLog ("ES Startup: Opened ES App Startup file: %s\n",
                                CFE_PLATFORM_ES_VOLATILE_STARTUP_FILE);
         FileOpened = true;
      }
      else
      {
         CFE_ES_WriteToSysLog ("ES Startup: Cannot Open Volatile Startup file, Trying Nonvolatile.\n");
         FileOpened = false;
      }

   } /* end if */

   /*
   ** This if block covers two cases: A Power on reset, and a Processor reset when
   ** the startup file on the volatile file system could not be opened.
   */
   if ( FileOpened == false )
   {
      /*
      ** Try to Open the file passed in to the cFE start.
      */
      Status = OS_OpenCreate(&AppFile, StartFilePath, OS_FILE_FLAG_NONE, OS_READ_ONLY);

      if ( Status >= 0 )
      {
         CFE_ES_WriteToSysLog ("ES Startup: Opened ES App Startup file: %s\n",StartFilePath);
         FileOpened = true;
      }
      else
      {
         CFE_ES_WriteToSysLog ("ES Startup: Error, Can't Open ES App Startup file: %s EC = 0x%08X\n",
                              StartFilePath, (unsigned int)Status );
         FileOpened = false;
      }

   }

   /*
   ** If the file is opened in either the Nonvolatile or the Volatile disk, process it.
   */
   if ( FileOpened == true)
   {
      memset(ES_AppLoadBuffer,0x0,ES_START_BUFF_SIZE);
      BuffLen = 0;
      NumTokens = 0;
      TokenList[0] = ES_AppLoadBuffer;

      /*
      ** Parse the lines from the file. If it has an error
      ** or reaches EOF, then abort the loop.
      */
      while(1)
      {
         Status = OS_read(AppFile, &c, 1);
         if ( Status < 0 )
         {
            CFE_ES_WriteToSysLog ("ES Startup: Error Reading Startup file. EC = 0x%08X\n",(unsigned int)Status);
            break;
         }
         else if ( Status == 0 )
         {
            /*
            ** EOF Reached
            */
            break;
         }
         else if(c != '!')
         {
             if ( c <= ' ')
             {
                /*
                ** Skip all white space in the file
                */
                ;
             }
             else if ( c == ',' )
             {
                /*
                ** replace the field delimiter with a null
                ** This is used to separate the tokens
                */
                if ( BuffLen < ES_START_BUFF_SIZE )
                {
                   ES_AppLoadBuffer[BuffLen] = 0;
                }
                else
                {
                   LineTooLong = true;
                }
                BuffLen++;

                if ( NumTokens < (CFE_ES_STARTSCRIPT_MAX_TOKENS_PER_LINE-1))
                {
                    /*
                     * NOTE: pointer never deferenced unless "LineTooLong" is false.
                     */
                    ++NumTokens;
                    TokenList[NumTokens] = &ES_AppLoadBuffer[BuffLen];
                }
             }
             else if ( c != ';' )
             {
                /*
                ** Regular data gets copied in
                */
                if ( BuffLen < ES_START_BUFF_SIZE )
                {
                   ES_AppLoadBuffer[BuffLen] = c;
                }
                else
                {
                   LineTooLong = true;
                }
                BuffLen++;
             }
             else
             {
                if ( LineTooLong == true )
                {
                   /*
                   ** The was too big for the buffer
                   */
                   CFE_ES_WriteToSysLog ("ES Startup: ES Startup File Line is too long: %u bytes.\n",(unsigned int)BuffLen);
                   LineTooLong = false;
                }
                else
                {
                   /*
                   ** Send the line to the file parser
                   ** Ensure termination of the last token and send it along
                   */
                   ES_AppLoadBuffer[BuffLen] = 0;
                   CFE_ES_ParseFileEntry(TokenList, 1 + NumTokens);
                }
                BuffLen = 0;
                NumTokens = 0;
             }
         }
         else
         {
           /*
           ** break when EOF character '!' is reached
           */
           break;
         }
      }
      /*
      ** close the file
      */
      OS_close(AppFile);

   }
}

/*
**---------------------------------------------------------------------------------------
** Name: CFE_ES_ParseFileEntry
**
**   Purpose: This function parses the startup file line for an individual
**            cFE application.
**---------------------------------------------------------------------------------------
*/
int32 CFE_ES_ParseFileEntry(const char **TokenList, uint32 NumTokens)
{
   const char   *FileName;
   const char   *AppName;
   const char   *EntryPoint;
   const char   *EntryType;
   unsigned long PriorityIn;
   unsigned long StackSizeIn;
   unsigned long ExceptionActionIn;
   CFE_ES_ResourceID_t ApplicationId;
   int32  CreateStatus = CFE_ES_ERR_APP_CREATE;

   /*
   ** Check to see if the correct number of items were parsed
   */
   if ( NumTokens < 8 )
   {
      CFE_ES_WriteToSysLog("ES Startup: Invalid ES Startup file entry: %u\n",(unsigned int)NumTokens);
      return (CreateStatus);
   }

   EntryType = TokenList[0];
   FileName = TokenList[1];
   EntryPoint = TokenList[2];
   AppName = TokenList[3];

   /*
    * NOTE: In previous CFE versions the sscanf() function was used to convert
    * these string values into integers.  This approach of using the pre-tokenized strings
    * and strtoul() is safer but the side effect is that it will also be more "permissive" in
    * what is accepted vs. rejected by this function.
    *
    * For instance if the startup script contains "123xyz", this will be converted to the value
    * 123 instead of triggering a validation failure as it would have in CFE <= 6.5.0.
    *
    * This permissive parsing should not be relied upon, as it may become more strict again in
    * future CFE revisions.
    *
    * Also note that this uses "unsigned long" as that is the defined output type of strtoul().
    * It will be converted to the correct type later.
    */
   PriorityIn = strtoul(TokenList[4], NULL, 0);
   StackSizeIn = strtoul(TokenList[5], NULL, 0);
   ExceptionActionIn = strtoul(TokenList[7], NULL, 0);

   if(strcmp(EntryType,"CFE_APP")==0)
   {
      CFE_ES_WriteToSysLog("ES Startup: Loading file: %s, APP: %s\n",
                            FileName, AppName);

      /*
      ** Validate Some parameters
      ** Exception action should be 0 ( Restart App ) or
      ** 1 ( Processor reset ). If it's non-zero, assume it means
      ** reset CPU.
      */
      if ( ExceptionActionIn > CFE_ES_ExceptionAction_RESTART_APP )
      {
          ExceptionActionIn = CFE_ES_ExceptionAction_PROC_RESTART;
      }

      /*
       * Task priority cannot be bigger than OS_MAX_TASK_PRIORITY
       */
      if ( PriorityIn > OS_MAX_TASK_PRIORITY )
      {
          PriorityIn = OS_MAX_TASK_PRIORITY;
      }

      /*
      ** Now create the application
      */
      CreateStatus = CFE_ES_AppCreate(&ApplicationId, FileName,
                               EntryPoint, AppName,
                               PriorityIn,
                               StackSizeIn,
                               ExceptionActionIn);
   }
   else if(strcmp(EntryType,"CFE_LIB")==0)
   {
      CFE_ES_WriteToSysLog("ES Startup: Loading shared library: %s\n",FileName);

      /*
      ** Now load the library
      */
      CreateStatus = CFE_ES_LoadLibrary(&ApplicationId, FileName,
                               EntryPoint, AppName);

   }
   else
   {
      CFE_ES_WriteToSysLog("ES Startup: Unexpected EntryType %s in startup file.\n",EntryType);
   }

   return (CreateStatus);

}

/*
**-------------------------------------------------------------------------------------
** Name: CFE_ES_LoadModule
**
** Helper function to load + configure (but not start) a new app/lib module
**
** Loads the module file via OSAL and stores all relevant info in the table entry as necessary.
**
**-------------------------------------------------------------------------------------
*/
int32 CFE_ES_LoadModule(const CFE_ES_ModuleLoadParams_t* LoadParams, CFE_ES_ModuleLoadStatus_t *LoadStatus)
{
    osal_id_t ModuleId;
    cpuaddr StartAddr;
    int32 ReturnCode;
    int32 StatusCode;

    StartAddr = 0;
    ReturnCode = CFE_SUCCESS;

    if (LoadParams->FileName[0] != 0)
    {
        /*
         ** Load the module via OSAL.
         */
        StatusCode = OS_ModuleLoad ( &ModuleId,
                LoadParams->Name,
                LoadParams->FileName,
                OS_MODULE_FLAG_GLOBAL_SYMBOLS );

        if (StatusCode != OS_SUCCESS)
        {
            CFE_ES_WriteToSysLog("ES Startup: Could not load file:%s. EC = 0x%08X\n",
                    LoadParams->FileName, (unsigned int)StatusCode);
            ModuleId = OS_OBJECT_ID_UNDEFINED;
            ReturnCode = CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
        }
    }
    else
    {
        ModuleId = OS_OBJECT_ID_UNDEFINED;
    }

    /*
     ** If the Load was OK, then lookup the address of the entry point
     */
    if (ReturnCode == CFE_SUCCESS && LoadParams->EntryPoint[0] != 0)
    {
        StatusCode = OS_SymbolLookup(&StartAddr, LoadParams->EntryPoint);
        if (StatusCode != OS_SUCCESS)
        {
            CFE_ES_WriteToSysLog("ES Startup: Could not find symbol:%s. EC = 0x%08X\n",
                    LoadParams->EntryPoint, (unsigned int)StatusCode);
            ReturnCode = CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
        }
    }

    if ( ReturnCode == CFE_SUCCESS )
    {
        /* store the data in the app record after successful load+lookup */
        LoadStatus->ModuleId = ModuleId;
        LoadStatus->EntryAddress = StartAddr;
    }
    else if (OS_ObjectIdDefined(ModuleId))
    {
        /* If the module had been successfully loaded, then unload it,
         * so that it does not consume resources */
        StatusCode = OS_ModuleUnload(ModuleId);
        if ( StatusCode != OS_SUCCESS ) /* There's not much we can do except notify */
        {
            CFE_ES_WriteToSysLog("ES Startup: Failed to unload: %s. EC = 0x%08X\n",
                    LoadParams->Name, (unsigned int)StatusCode);
        }
    }

    return ReturnCode;
}

/*
**-------------------------------------------------------------------------------------
** Name: CFE_ES_GetAppEntryPoint
**
** Helper function to act as the intermediate entry point of an app
** This is to support starting apps before having a fully completed entry in the
** global app table.  The app startup will delay until the app creation is completed
** and verified, then the actual entry point will be determined.
**
**-------------------------------------------------------------------------------------
*/
int32 CFE_ES_GetAppEntryPoint(osal_task_entry *FuncPtr)
{
    CFE_ES_AppRecord_t *AppRecPtr;
    int32 ReturnCode;
    int32 Timeout;

    /*
     * Use the same timeout as was used for the startup script itself.
     */
    ReturnCode = CFE_ES_ERR_APP_REGISTER;
    Timeout = CFE_PLATFORM_ES_STARTUP_SCRIPT_TIMEOUT_MSEC;

    while(true)
    {
        OS_TaskDelay(CFE_PLATFORM_ES_STARTUP_SYNC_POLL_MSEC);

        CFE_ES_LockSharedData(__func__,__LINE__);
        AppRecPtr = CFE_ES_GetAppRecordByContext();
        if (AppRecPtr != NULL)
        {
            AppRecPtr->AppState = CFE_ES_AppState_EARLY_INIT;
            *FuncPtr = (osal_task_entry)AppRecPtr->ModuleInfo.EntryAddress;
            ReturnCode = CFE_SUCCESS;
        }
        CFE_ES_UnlockSharedData(__func__,__LINE__);

        if (ReturnCode == CFE_SUCCESS || Timeout <= 0)
        {
            /* end of loop condition */
            break;
        }

        Timeout -= CFE_PLATFORM_ES_STARTUP_SYNC_POLL_MSEC;
    }

    return (ReturnCode);
}

/*
**-------------------------------------------------------------------------------------
** Name: CFE_ES_AppEntryPoint
**
** Helper function to act as the intermediate entry point of an app
** This is to support starting apps before having a fully completed entry in the
** global app table.  The app startup will delay until the app creation is completed
** and verified, then the actual entry point will be determined.
**
**-------------------------------------------------------------------------------------
*/
void CFE_ES_AppEntryPoint(void)
{
    osal_task_entry RealEntryFunc;

    if (CFE_ES_GetAppEntryPoint(&RealEntryFunc) == CFE_SUCCESS &&
            RealEntryFunc != NULL)
    {
        (*RealEntryFunc)();
    }
}

/*
**-------------------------------------------------------------------------------------
** Name: CFE_ES_StartMainTask
**
** Helper function to start (but not load) a new app/lib module
**
** Note that OSAL does not separate the action of creating and start a task, providing
** only OS_TaskCreate which does both.  But there is a potential race condition if
** the real task code starts and calls e.g. CFE_ES_RegisterApp() or any other function
** that depends on having an AppID context, before its fully registered in the global app table.
**
** Therefore this calls a dedicated CFE_ES_AppEntryPoint which then will wait until
** the task is fully registered in the global, before calling the actual app entry point.
**
**-------------------------------------------------------------------------------------
*/
int32 CFE_ES_StartAppTask(const CFE_ES_AppStartParams_t* StartParams, CFE_ES_ResourceID_t RefAppId, CFE_ES_ResourceID_t *TaskIdPtr)
{
    CFE_ES_TaskRecord_t *TaskRecPtr;
    osal_id_t OsalTaskId;
    CFE_ES_ResourceID_t TaskId;
    int32 StatusCode;
    int32 ReturnCode;

    /*
     ** Create the primary task for the newly loaded task
     */
    StatusCode = OS_TaskCreate(&OsalTaskId,   /* task id */
            StartParams->BasicInfo.Name,      /* task name */
            CFE_ES_AppEntryPoint,             /* task function pointer */
            OSAL_TASK_STACK_ALLOCATE,         /* stack pointer (allocate) */
            StartParams->StackSize,           /* stack size */
            StartParams->Priority,            /* task priority */
            OS_FP_ENABLED);                   /* task options */

    CFE_ES_LockSharedData(__func__,__LINE__);

    if ( StatusCode == OS_SUCCESS )
    {
        /*
         * As this is a newly-created task, this shouldn't fail.
         * The entry is not (yet) matching the task ID - it will be
         * initialized here.
         */
        TaskId = CFE_ES_ResourceID_FromOSAL(OsalTaskId);
        TaskRecPtr = CFE_ES_LocateTaskRecordByID(TaskId);
        if ( CFE_ES_TaskRecordIsUsed(TaskRecPtr) )
        {
            CFE_ES_SysLogWrite_Unsync("ES Startup: Error: ES_TaskTable slot for ID %lx in use at task creation!\n",
                    OS_ObjectIdToInteger(OsalTaskId));
        }

        /*
         * Clear any other/stale data that might be in the entry,
         * and reset all fields to the correct value.
         */
        memset(TaskRecPtr, 0, sizeof(*TaskRecPtr));

        TaskRecPtr->AppId = RefAppId;
        strncpy(TaskRecPtr->TaskName, StartParams->BasicInfo.Name, sizeof(TaskRecPtr->TaskName)-1);
        TaskRecPtr->TaskName[sizeof(TaskRecPtr->TaskName)-1] = 0;
        CFE_ES_TaskRecordSetUsed(TaskRecPtr, TaskId);

        /*
         ** Increment the registered Task count.
         */
        CFE_ES_Global.RegisteredTasks++;
        ReturnCode = CFE_SUCCESS;
        *TaskIdPtr = TaskId;
    }
    else
    {
        CFE_ES_SysLogWrite_Unsync("ES Startup: AppCreate Error: TaskCreate %s Failed. EC = 0x%08X!\n",
                    StartParams->BasicInfo.Name,(unsigned int)StatusCode);
        ReturnCode = CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
        *TaskIdPtr = CFE_ES_RESOURCEID_UNDEFINED;
    }

    CFE_ES_UnlockSharedData(__func__,__LINE__);

    return ReturnCode;
}

/*
**---------------------------------------------------------------------------------------
** Name: ES_AppCreate
**
**   Purpose: This function loads and creates a cFE Application.
**            This function can be called from the ES startup code when it
**            loads the cFE Applications from the disk using the startup script, or it
**            can be called when the ES Start Application command is executed.
**
**---------------------------------------------------------------------------------------
*/
int32 CFE_ES_AppCreate(CFE_ES_ResourceID_t *ApplicationIdPtr,
                       const char   *FileName,
                       const char   *EntryPointName,
                       const char   *AppName,
                       CFE_ES_TaskPriority_Atom_t  Priority,
                       size_t  StackSize,
                       CFE_ES_ExceptionAction_Enum_t  ExceptionAction)
{
   CFE_Status_t Status;
   CFE_ES_ResourceID_t MainTaskId;
   CFE_ES_AppRecord_t *AppRecPtr;
   CFE_ES_ResourceID_t PendingAppId;

   /*
    * The FileName must not be NULL
    */
   if (FileName == NULL || AppName == NULL)
   {
       return CFE_ES_BAD_ARGUMENT;
   }

   if (strlen(AppName) >= OS_MAX_API_NAME)
   {
       return CFE_ES_BAD_ARGUMENT;
   }

   /*
   ** Allocate an ES_AppTable entry
   */
   CFE_ES_LockSharedData(__func__,__LINE__);

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
       CFE_ES_SysLogWrite_Unsync("ES Startup: Duplicate app name '%s'\n", AppName);
       Status = CFE_ES_ERR_DUPLICATE_NAME;
   }
   else
   {
       /* scan for a free slot */
       PendingAppId = CFE_ES_FindNextAvailableId(CFE_ES_Global.LastAppId, CFE_PLATFORM_ES_MAX_APPLICATIONS);
       AppRecPtr = CFE_ES_LocateAppRecordByID(PendingAppId);

       if (AppRecPtr == NULL)
       {
            CFE_ES_SysLogWrite_Unsync("ES Startup: No free application slots available\n");
            Status = CFE_ES_NO_RESOURCE_IDS_AVAILABLE;
       }
       else
       {
           /* Fully clear the entry, just in case of stale data */
           memset(AppRecPtr, 0, sizeof(*AppRecPtr));

           /*
            * Fill out the parameters in the StartParams sub-structure
            */
           AppRecPtr->Type = CFE_ES_AppType_EXTERNAL;
           strncpy(AppRecPtr->StartParams.BasicInfo.Name, AppName,
                   sizeof(AppRecPtr->StartParams.BasicInfo.Name)-1);
           strncpy(AppRecPtr->StartParams.BasicInfo.FileName, FileName,
                   sizeof(AppRecPtr->StartParams.BasicInfo.FileName)-1);
           if (EntryPointName != NULL && strcmp(EntryPointName, "NULL") != 0)
           {
               strncpy(AppRecPtr->StartParams.BasicInfo.EntryPoint, EntryPointName,
                       sizeof(AppRecPtr->StartParams.BasicInfo.EntryPoint)-1);
           }

           AppRecPtr->StartParams.StackSize = StackSize;
           AppRecPtr->StartParams.ExceptionAction = ExceptionAction;
           AppRecPtr->StartParams.Priority = Priority;

           /*
            * Fill out the Task State info
            */
           AppRecPtr->ControlReq.AppControlRequest = CFE_ES_RunStatus_APP_RUN;
           AppRecPtr->ControlReq.AppTimerMsec = 0;

           CFE_ES_AppRecordSetUsed(AppRecPtr, CFE_ES_RESOURCEID_RESERVED);
           CFE_ES_Global.LastAppId = PendingAppId;
           Status = CFE_SUCCESS;
       }
   }

   CFE_ES_UnlockSharedData(__func__,__LINE__);

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
   Status = CFE_ES_LoadModule(&AppRecPtr->StartParams.BasicInfo, &AppRecPtr->ModuleInfo);

   /*
    * If the Load was OK, then complete the initialization
    */
   if (Status == CFE_SUCCESS)
   {
       Status = CFE_ES_StartAppTask(&AppRecPtr->StartParams, PendingAppId, &MainTaskId);
   }
   else
   {
       MainTaskId = CFE_ES_RESOURCEID_UNDEFINED;
   }

   /*
    * Finalize data in the app table entry, which must be done under lock.
    * This transitions the entry from being RESERVED to the real ID.
    */
   CFE_ES_LockSharedData(__func__,__LINE__);

   if ( Status == CFE_SUCCESS )
   {
       /*
        * important - set the ID to its proper value
        * which turns this into a real/valid table entry
        */
       AppRecPtr->MainTaskId = MainTaskId;
       CFE_ES_AppRecordSetUsed(AppRecPtr, PendingAppId);

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
       CFE_ES_AppRecordSetFree(AppRecPtr);
       PendingAppId = CFE_ES_RESOURCEID_UNDEFINED;
   }

   CFE_ES_UnlockSharedData(__func__,__LINE__);

   *ApplicationIdPtr = PendingAppId;

   return Status;

} /* End Function */
/*
**---------------------------------------------------------------------------------------
** Name: CFE_ES_LoadLibrary
**
**   Purpose: This function loads and initializes a cFE Shared Library.
**
**---------------------------------------------------------------------------------------
*/
int32 CFE_ES_LoadLibrary(CFE_ES_ResourceID_t       *LibraryIdPtr,
                         const char   *FileName,
                         const char   *EntryPointName,
                         const char   *LibName)
{
   CFE_ES_LibraryEntryFuncPtr_t FunctionPointer;
   CFE_ES_LibRecord_t *         LibSlotPtr;
   int32                        Status;
   CFE_ES_ResourceID_t          PendingLibId;

   /*
    * The FileName must not be NULL
    */
   if (FileName == NULL || LibName == NULL)
   {
       return CFE_ES_BAD_ARGUMENT;
   }

   if (strlen(LibName) >= OS_MAX_API_NAME)
   {
       return CFE_ES_BAD_ARGUMENT;
   }

   /*
   ** Allocate an ES_LibTable entry
   */
   FunctionPointer = NULL;
   PendingLibId = CFE_ES_RESOURCEID_UNDEFINED;

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
   CFE_ES_LockSharedData(__func__,__LINE__);

   /*
    * Check for an existing entry with the same name.
    * Also check for a matching Library name.
    * (Libs and libraries should be uniquely named)
    */
   LibSlotPtr = CFE_ES_LocateLibRecordByName(LibName);
   if (LibSlotPtr != NULL || CFE_ES_LocateAppRecordByName(LibName) != NULL)
   {
       CFE_ES_SysLogWrite_Unsync("ES Startup: Duplicate Lib name '%s'\n", LibName);
       if (LibSlotPtr != NULL)
       {
           PendingLibId = CFE_ES_LibRecordGetID(LibSlotPtr);
       }
       Status = CFE_ES_ERR_DUPLICATE_NAME;
   }
   else
   {
       /* scan for a free slot */
       PendingLibId = CFE_ES_FindNextAvailableId(CFE_ES_Global.LastLibId, CFE_PLATFORM_ES_MAX_LIBRARIES);
       LibSlotPtr = CFE_ES_LocateLibRecordByID(PendingLibId);

       if (LibSlotPtr == NULL)
       {
           CFE_ES_SysLogWrite_Unsync("ES Startup: No free library slots available\n");
           Status = CFE_ES_NO_RESOURCE_IDS_AVAILABLE;
       }
       else
       {
           /* Fully clear the entry, just in case of stale data */
           memset(LibSlotPtr, 0, sizeof(*LibSlotPtr));

           /*
            * Fill out the parameters in the AppStartParams sub-structure
            */
           strncpy(LibSlotPtr->BasicInfo.Name, LibName,
                   sizeof(LibSlotPtr->BasicInfo.Name)-1);
           strncpy(LibSlotPtr->BasicInfo.FileName, FileName,
                   sizeof(LibSlotPtr->BasicInfo.FileName)-1);
           if (EntryPointName != NULL && strcmp(EntryPointName, "NULL") != 0)
           {
              strncpy(LibSlotPtr->BasicInfo.EntryPoint, EntryPointName,
                      sizeof(LibSlotPtr->BasicInfo.EntryPoint)-1);
           }

           CFE_ES_LibRecordSetUsed(LibSlotPtr, CFE_ES_RESOURCEID_RESERVED);
           CFE_ES_Global.LastLibId = PendingLibId;
           Status = CFE_SUCCESS;
       }
   }

   CFE_ES_UnlockSharedData(__func__,__LINE__);

   /*
    * If any off-nominal condition exists, skip the rest of this logic.
    * (Log message already written)
    */
   if (Status != CFE_SUCCESS)
   {
       *LibraryIdPtr = PendingLibId;
       return Status;
   }

   /*
    * Load the module based on StartParams configured above.
    */
   Status = CFE_ES_LoadModule(&LibSlotPtr->BasicInfo, &LibSlotPtr->ModuleInfo);
   if (Status == CFE_SUCCESS)
   {
      FunctionPointer = (CFE_ES_LibraryEntryFuncPtr_t)LibSlotPtr->ModuleInfo.EntryAddress;
      if (FunctionPointer != NULL)
      {
         Status = (*FunctionPointer)(PendingLibId);
         if (Status != CFE_SUCCESS)
         {
             CFE_ES_WriteToSysLog("ES Startup: Load Shared Library Init Error = 0x%08x\n",
                     (unsigned int)Status);
         }
      }
   }

   /*
    * Finalize data in the app table entry, which must be done under lock.
    * This transitions the entry from being RESERVED to the real type,
    * either MAIN_TASK (success) or returning to INVALID (failure).
    */
   CFE_ES_LockSharedData(__func__,__LINE__);

   if ( Status == CFE_SUCCESS )
   {
      /*
       * important - set the ID to its proper value
       * which turns this into a real/valid table entry
       */
      CFE_ES_LibRecordSetUsed(LibSlotPtr, PendingLibId);

       /*
       * Increment the registered Lib counter.
       */
      CFE_ES_Global.RegisteredLibs++;
   }
   else
   {
      CFE_ES_LibRecordSetFree(LibSlotPtr);
      PendingLibId = CFE_ES_RESOURCEID_UNDEFINED;
   }

   CFE_ES_UnlockSharedData(__func__,__LINE__);

   *LibraryIdPtr = PendingLibId;

   return(Status);

} /* End Function */

/*
**---------------------------------------------------------------------------------------
** Name: CFE_ES_RunAppTableScan
**
**   Purpose: This function scans the ES Application table and acts on the changes
**             in application states. This is where the external cFE Applications are
**             restarted, reloaded, or deleted.
**---------------------------------------------------------------------------------------
*/
bool CFE_ES_RunAppTableScan(uint32 ElapsedTime, void *Arg)
{
   CFE_ES_AppTableScanState_t *State = (CFE_ES_AppTableScanState_t *)Arg;
   uint32                      i;
   CFE_ES_AppRecord_t         *AppPtr;
   CFE_ES_ResourceID_t         AppTimeoutList[CFE_PLATFORM_ES_MAX_APPLICATIONS];
   uint32                      NumAppTimeouts;

   if (State->PendingAppStateChanges == 0)
   {
       /*
        * If the command count changes, then a scan becomes due immediately.
        */
       if (State->LastScanCommandCount == CFE_ES_TaskData.CommandCounter &&
               State->BackgroundScanTimer > ElapsedTime)
       {
           /* no action at this time, background scan is not due yet */
           State->BackgroundScanTimer -= ElapsedTime;
           return false;
       }
   }

   /*
    * Every time a scan is initiated (for any reason)
    * reset the background scan timer to the full value,
    * and take a snapshot of the the command counter.
    */
   NumAppTimeouts = 0;
   State->BackgroundScanTimer = CFE_PLATFORM_ES_APP_SCAN_RATE;
   State->LastScanCommandCount = CFE_ES_TaskData.CommandCounter;
   State->PendingAppStateChanges = 0;

   /*
    * Scan needs to be done with the table locked,
    * as these state changes need to be done atomically
    * with respect to other tasks that also access/update
    * the state.
    */
   CFE_ES_LockSharedData(__func__,__LINE__);

   /*
   ** Scan the ES Application table. Skip entries that are:
   **  - Not in use, or
   **  - cFE Core apps, or
   **  - Currently running
   */
   AppPtr = CFE_ES_Global.AppTable;
   for ( i = 0; i < CFE_PLATFORM_ES_MAX_APPLICATIONS; i++ )
   {
       if ( CFE_ES_AppRecordIsUsed(AppPtr) && AppPtr->Type == CFE_ES_AppType_EXTERNAL)
       {
           if (AppPtr->AppState > CFE_ES_AppState_RUNNING)
           {
               /*
                * Increment the "pending" counter which reflects
                * the number of apps that are in some phase of clean up.
                */
               ++State->PendingAppStateChanges;

               /*
                * Decrement the wait timer, if active.
                * When the timeout value becomes zero, take the action to delete/restart/reload the app
                */
               if ( AppPtr->ControlReq.AppTimerMsec > ElapsedTime )
               {
                   AppPtr->ControlReq.AppTimerMsec -= ElapsedTime;
               }
               else
               {
                   AppPtr->ControlReq.AppTimerMsec = 0;

                   /* Add it to the list to be processed later */
                   AppTimeoutList[NumAppTimeouts] = CFE_ES_AppRecordGetID(AppPtr);
                   ++NumAppTimeouts;
               }
           }
           else if (AppPtr->AppState == CFE_ES_AppState_RUNNING &&
                       AppPtr->ControlReq.AppControlRequest > CFE_ES_RunStatus_APP_RUN)
           {
               /* this happens after a command arrives to restart/reload/delete an app */
               /* switch to WAITING state, and set the timer for transition */
               AppPtr->AppState = CFE_ES_AppState_WAITING;
               AppPtr->ControlReq.AppTimerMsec = CFE_PLATFORM_ES_APP_KILL_TIMEOUT * CFE_PLATFORM_ES_APP_SCAN_RATE;
           }


       } /* end if */

       ++AppPtr;

   } /* end for loop */

   CFE_ES_UnlockSharedData(__func__,__LINE__);


   /*
    * Now invoke the CFE_ES_ProcessControlRequest() routine for any app
    * which has reached that point.
    */
   for ( i = 0; i < NumAppTimeouts; i++ )
   {
       /*
        * Call CFE_ES_ProcessControlRequest() with a reference to
        * the _copies_ of the app record details.  (This avoids
        * needing to access the global records outside of the lock).
        */
       CFE_ES_ProcessControlRequest(AppTimeoutList[i]);
   }


   /*
    * This state machine is considered active if there are any
    * pending app state changes.  Returning "true" will cause this job
    * to be called from the background task at a faster interval.
    */
   return (State->PendingAppStateChanges != 0);

} /* End Function */


/*
**---------------------------------------------------------------------------------------
**   Name: CFE_ES_ProcessControlRequest
**
**   Purpose: This function will perform the requested control action for an application.
**---------------------------------------------------------------------------------------
*/
void CFE_ES_ProcessControlRequest(CFE_ES_ResourceID_t AppId)
{
    CFE_ES_AppRecord_t      *AppRecPtr;
    uint32                   PendingControlReq;
    CFE_ES_AppStartParams_t  OrigStartParams;
    CFE_Status_t             CleanupStatus;
    CFE_Status_t             StartupStatus;
    CFE_ES_ResourceID_t      NewAppId;
    const char              *ReqName;
    char                     MessageDetail[48];
    uint16                   EventID;
    CFE_EVS_EventType_Enum_t EventType;

    /* Init/clear all local state variables */
    ReqName = NULL;
    MessageDetail[0] = 0;
    EventID = 0;
    EventType = 0;
    StartupStatus = CFE_SUCCESS;
    PendingControlReq = 0;
    NewAppId = CFE_ES_RESOURCEID_UNDEFINED;
    AppRecPtr = CFE_ES_LocateAppRecordByID(AppId);
    memset(&OrigStartParams, 0, sizeof(OrigStartParams));


    /*
     * Take a local snapshot of the important app record data
     * This way it becomes private and can be accessed without
     * concerns about other threads/tasks, even after the global
     * data records are eventually cleared.
     */
    CFE_ES_LockSharedData(__func__,__LINE__);

    if (CFE_ES_AppRecordIsMatch(AppRecPtr, AppId))
    {
        PendingControlReq = AppRecPtr->ControlReq.AppControlRequest;
        OrigStartParams = AppRecPtr->StartParams;
    }

    CFE_ES_UnlockSharedData(__func__,__LINE__);

    /*
     * All control requests start by deleting the app/task and
     * all associated resources.
     *
     * The reload/restart requests will start it again, and it gets
     * a new appID.  For other requests it just leaves it deleted.
     *
     * Note that Cleanup can fail for a variety of reasons, including
     * situations where e.g. a task ID had become stale because the task
     * already exited itself.  In most cases these are minor errors and
     * reflect problems with the consistency of the old app record.
     *
     * Even when this happens the cleanup should still do its best effort
     * to release all relevant global data entries.  So it should not
     * prevent starting the new app, if a restart/reload is indicated.
     */
    CleanupStatus = CFE_ES_CleanUpApp(AppId);

    /*
     * Attempt to restart the app if the request indicated to do so,
     * regardless of the CleanupStatus.
     */
    if ( PendingControlReq == CFE_ES_RunStatus_SYS_RESTART ||
            PendingControlReq == CFE_ES_RunStatus_SYS_RELOAD )
    {
        StartupStatus = CFE_ES_AppCreate(&NewAppId,
                OrigStartParams.BasicInfo.FileName,
                OrigStartParams.BasicInfo.EntryPoint,
                OrigStartParams.BasicInfo.Name,
                OrigStartParams.Priority,
                OrigStartParams.StackSize,
                OrigStartParams.ExceptionAction);
    }

    /*
     * Determine the event ID associated with the control request,
     * which indicates the success/failure of the operation and
     * any other relevant detail.
     *
     * Note that the specific event ID that gets generated is the only
     * other difference between all these control request types.
     */
    switch ( PendingControlReq )
    {
    case CFE_ES_RunStatus_APP_EXIT:
        ReqName = "Exit";
        if (CleanupStatus != CFE_SUCCESS)
        {
            /* error event for this request */
            EventID = CFE_ES_EXIT_APP_ERR_EID;
        }
        else
        {
            /* success event for this request */
            EventID = CFE_ES_EXIT_APP_INF_EID;
        }
        break;

    case CFE_ES_RunStatus_APP_ERROR:
        ReqName = "Exit";
        if ( CleanupStatus != CFE_SUCCESS )
        {
            /* error event for this request */
            EventID = CFE_ES_ERREXIT_APP_ERR_EID;
        }
        else
        {
            /* success event for this request */
            EventID = CFE_ES_ERREXIT_APP_INF_EID;
        }
        break;

    case CFE_ES_RunStatus_SYS_DELETE:
        ReqName = "Stop";
        if ( CleanupStatus != CFE_SUCCESS )
        {
            /* error event for this request */
            EventID = CFE_ES_STOP_ERR3_EID;
        }
        else
        {
            /* success event for this request */
            EventID = CFE_ES_STOP_INF_EID;
        }
        break;

    case CFE_ES_RunStatus_SYS_RESTART:
        ReqName = "Restart";
        if ( CleanupStatus != CFE_SUCCESS )
        {
            /* error event for this request */
            EventID = CFE_ES_RESTART_APP_ERR4_EID;
        }
        else if ( StartupStatus != CFE_SUCCESS )
        {
            /* error event for this request */
            EventID = CFE_ES_RESTART_APP_ERR3_EID;
        }
        else
        {
            /* success event for this request */
            EventID = CFE_ES_RESTART_APP_INF_EID;
        }
        break;

    case CFE_ES_RunStatus_SYS_RELOAD:
        ReqName = "Reload";
        if ( CleanupStatus != CFE_SUCCESS )
        {
            /* error event for this request */
            EventID = CFE_ES_RELOAD_APP_ERR4_EID;
        }
        else if ( StartupStatus != CFE_SUCCESS )
        {
            /* error event for this request */
            EventID = CFE_ES_RELOAD_APP_ERR3_EID;
        }
        else
        {
            /* success event for this request */
            EventID = CFE_ES_RELOAD_APP_INF_EID;
        }
        break;

        /*
         * These two cases below should never occur so they are always
         * reported as errors, but the  CFE_ES_CleanUpApp() should hopefully
         * have fixed it either way.
         */
    case CFE_ES_RunStatus_SYS_EXCEPTION:
        ReqName = "ES_ProcControlReq: Invalid State";
        EventID = CFE_ES_PCR_ERR1_EID;
        snprintf(MessageDetail, sizeof(MessageDetail), "EXCEPTION");
        break;

    default:
        ReqName = "ES_ProcControlReq: Unknown State";
        EventID = CFE_ES_PCR_ERR2_EID;
        snprintf(MessageDetail, sizeof(MessageDetail), "( %lu )",
                (unsigned long)PendingControlReq);
        break;
    }

    if (EventID != 0 && ReqName != NULL)
    {
        if ( MessageDetail[0] != 0 )
        {
            /* Detail message already set, assume it is an error event */
            EventType = CFE_EVS_EventType_ERROR;
        }
        else if ( StartupStatus != CFE_SUCCESS )
        {
            /* Make detail message for event containing startup error code */
            EventType = CFE_EVS_EventType_ERROR;
            snprintf(MessageDetail, sizeof(MessageDetail),
                    "Failed: AppCreate Error 0x%08X.",(unsigned int)StartupStatus);
        }
        else if ( CleanupStatus != CFE_SUCCESS )
        {
            /* Make detail message for event containing cleanup error code */
            EventType = CFE_EVS_EventType_ERROR;
            snprintf(MessageDetail, sizeof(MessageDetail),
                    "Failed: CleanUpApp Error 0x%08X.",(unsigned int)CleanupStatus);
        }
        else if (CFE_ES_ResourceID_IsDefined(NewAppId))
        {
            /* Record success message for event where app is restarted */
            EventType = CFE_EVS_EventType_INFORMATION;
            snprintf(MessageDetail, sizeof(MessageDetail), "Completed, AppID=%lu",
                    CFE_ES_ResourceID_ToInteger(NewAppId));
        }
        else
        {
            /* Record success message for event */
            EventType = CFE_EVS_EventType_INFORMATION;
            snprintf(MessageDetail, sizeof(MessageDetail), "Completed.");
        }

        CFE_EVS_SendEvent(EventID, EventType, "%s Application %s %s",
                ReqName, OrigStartParams.BasicInfo.Name, MessageDetail);
    }

} /* End Function */

/*
**---------------------------------------------------------------------------------------
**   Name: CFE_ES_CleanUpApp
**
**   Purpose: Delete an application by cleaning up all of it's resources.
**---------------------------------------------------------------------------------------
*/
int32 CFE_ES_CleanUpApp(CFE_ES_ResourceID_t AppId)
{
    uint32                   i;
    int32                    Status;
    int32                    ReturnCode;
    CFE_ES_ResourceID_t      TaskList[OS_MAX_TASKS];
    CFE_ES_MemHandle_t       PoolList[CFE_PLATFORM_ES_MAX_MEMORY_POOLS];
    osal_id_t                ModuleId;
    uint32                   NumTasks;
    uint32                   NumPools;
    CFE_ES_AppRecord_t      *AppRecPtr;
    CFE_ES_TaskRecord_t     *TaskRecPtr;
    CFE_ES_MemPoolRecord_t  *MemPoolRecPtr;


    NumTasks = 0;
    NumPools = 0;
    ModuleId = OS_OBJECT_ID_UNDEFINED;
    ReturnCode = CFE_SUCCESS;

    AppRecPtr = CFE_ES_LocateAppRecordByID(AppId);

    /*
     * Collect a list of resources previously owned by this app, which
     * must be done while the global data is locked.
     */
    CFE_ES_LockSharedData(__func__,__LINE__);

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
            ModuleId = AppRecPtr->ModuleInfo.ModuleId;
        }

        /*
         * Collect all tasks associated with this app
         */
        TaskRecPtr = CFE_ES_Global.TaskTable;
        for ( i = 0; i < OS_MAX_TASKS; i++ )
        {
            if ( CFE_ES_TaskRecordIsUsed(TaskRecPtr) &&
                    CFE_ES_ResourceID_Equal(TaskRecPtr->AppId, AppId))
            {
                TaskList[NumTasks] = CFE_ES_TaskRecordGetID(TaskRecPtr);

                /* Store the main task ID at index 0 (swap with whatever was there) */
                if (CFE_ES_ResourceID_Equal(TaskList[NumTasks], AppRecPtr->MainTaskId) &&
                        NumTasks != 0)
                {
                    TaskList[NumTasks] = TaskList[0];
                    TaskList[0] = AppRecPtr->MainTaskId;
                }

                /* Mark record for removal */
                CFE_ES_TaskRecordSetUsed(TaskRecPtr, CFE_ES_RESOURCEID_RESERVED);
                ++NumTasks;
            }

            ++TaskRecPtr;
        } /* end for */

        CFE_ES_Global.RegisteredTasks -= NumTasks;

        /*
         * Collect memory pools associated with this app
         */
        MemPoolRecPtr = CFE_ES_Global.MemPoolTable;
        for ( i = 0; i < CFE_PLATFORM_ES_MAX_MEMORY_POOLS; i++ )
        {
            if ( CFE_ES_MemPoolRecordIsUsed(MemPoolRecPtr) &&
                    CFE_ES_ResourceID_Equal(MemPoolRecPtr->OwnerAppID, AppId))
            {
                PoolList[NumPools] = CFE_ES_MemPoolRecordGetID(MemPoolRecPtr);
                ++NumPools;
            }

            ++MemPoolRecPtr;
        } /* end for */

        /*
         * Set the record to RESERVED.
         *
         * This prevents reallocation of this slot while the remainder
         * of resources are freed.
         */
        CFE_ES_AppRecordSetUsed(AppRecPtr, CFE_ES_RESOURCEID_RESERVED);

        ReturnCode = CFE_SUCCESS;
    }
    else
    {
        CFE_ES_SysLogWrite_Unsync("CFE_ES_CleanUpApp: AppID %lu is not valid for deletion\n",
                CFE_ES_ResourceID_ToInteger(AppId));
        ReturnCode = CFE_ES_APP_CLEANUP_ERR;
    }

    CFE_ES_UnlockSharedData(__func__, __LINE__);

    if (ReturnCode != CFE_SUCCESS)
    {
        return ReturnCode;
    }


    /*
     * Now actually delete all the resources associated with the task.
     *
     * Most of this involves calling into other subsystems, so it is
     * done while the ES global data is UNLOCKED to avoid holding more
     * than one lock at a time.
     */

    /*
     ** Call the Table Clean up function
     */
    CFE_TBL_CleanUpApp(AppId);

    /*
     ** Call the Software Bus clean up function
     */
    CFE_SB_CleanUpApp(AppId);

    /*
     ** Call the TIME Clean up function
     */
    CFE_TIME_CleanUpApp(AppId);

    /*
     ** Call the EVS Clean up function
     */
    Status = CFE_EVS_CleanUpApp(AppId);
    if ( Status != CFE_SUCCESS )
    {
        CFE_ES_WriteToSysLog("CFE_ES_CleanUpApp: Call to CFE_EVS_CleanUpApp returned Error: 0x%08X\n",(unsigned int)Status);
        ReturnCode = CFE_ES_APP_CLEANUP_ERR;
    }

    /*
     * Delete all tasks.
     *
     * Note that the main task is always positioned at index 0 in this list.
     *
     * This iterates the list in reverse order, such that the child
     * tasks are deleted first (in any order) and main task is deleted last.
     */
    i = NumTasks;
    while (i > 0)
    {
        --i;
        Status = CFE_ES_CleanupTaskResources(TaskList[i]);
        if ( Status != CFE_SUCCESS )
        {
            CFE_ES_WriteToSysLog("CFE_ES_CleanUpApp: CleanUpTaskResources for Task ID:%lu returned Error: 0x%08X\n",
                    CFE_ES_ResourceID_ToInteger(TaskList[i]), (unsigned int)Status);
            ReturnCode = CFE_ES_APP_CLEANUP_ERR;
        }
    }

    /*
     * Delete all mem pools.
     */
    for (i=0; i < NumPools; ++i)
    {
        Status = CFE_ES_PoolDelete(PoolList[i]);
        if ( Status != CFE_SUCCESS )
        {
            CFE_ES_WriteToSysLog("CFE_ES_MemPoolCleanupApp: delete pool %lu returned Error: 0x%08X\n",
                    CFE_ES_ResourceID_ToInteger(PoolList[i]), (unsigned int)Status);
            ReturnCode = CFE_ES_APP_CLEANUP_ERR;
        }
    }

    /*
     ** Unload the module, if applicable
     */
    if ( OS_ObjectIdDefined(ModuleId) )
    {
        /*
         ** Unload the module only if it is an external app
         */
        Status = OS_ModuleUnload(ModuleId);
        if ( Status != OS_SUCCESS )
        {
            CFE_ES_WriteToSysLog("CFE_ES_CleanUpApp: Module (ID:0x%08lX) Unload failed. RC=0x%08X\n",
                    OS_ObjectIdToInteger(ModuleId), (unsigned int)Status);
            ReturnCode = CFE_ES_APP_CLEANUP_ERR;
        }
    }

    /*
     * Finally, re-acquire the ES lock and set all
     * table entries free for re-use.
     */
    CFE_ES_LockSharedData(__func__,__LINE__);

#ifdef jphfix
    /*
     * This just confirms that the main task ID associated with this app was properly
     * located and deleted during the previous process.
     *
     * If not, display a log message about it - this indicates table corruption or a
     * bug with the record keeping.
     */
    if (NumTasks == 0 || !CFE_ES_ResourceID_Equal(TaskList[0], AppRecPtr->MainTaskId))
    {
        CFE_ES_SysLogWrite_Unsync("CFE_ES_CleanUpApp: MainTask ID:%lu not found when deleting AppID %lu\n",
                CFE_ES_ResourceID_ToInteger(AppRecPtr->MainTaskId), CFE_ES_ResourceID_ToInteger(AppId));
        ReturnCode = CFE_ES_APP_CLEANUP_ERR;
    }
#endif

    /*
     * Free all task records.
     */
    for (i=0; i < NumTasks; ++i)
    {
        TaskRecPtr = CFE_ES_LocateTaskRecordByID(TaskList[i]);
        if (CFE_ES_TaskRecordIsMatch(TaskRecPtr, CFE_ES_RESOURCEID_RESERVED))
        {
            CFE_ES_TaskRecordSetFree(TaskRecPtr);
        }
    }

    /*
     * Now finally delete the record and allow re-use of the slot.
     */
    if (CFE_ES_AppRecordIsMatch(AppRecPtr, CFE_ES_RESOURCEID_RESERVED))
    {
        CFE_ES_AppRecordSetFree(AppRecPtr);
    }

    CFE_ES_UnlockSharedData(__func__,__LINE__);

    return(ReturnCode);

} /* end function */


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
**---------------------------------------------------------------------------------------
**   Name: CFE_ES_CleanupObjectCallback
**
**   Purpose: Helper function clean up all objects.
**
**   NOTE: This is called while holding the ES global lock
**---------------------------------------------------------------------------------------
*/
void CFE_ES_CleanupObjectCallback(osal_id_t ObjectId, void *arg)
{
    CFE_ES_CleanupState_t   *CleanState;
    int32                   Status;
    osal_objtype_t          ObjType;
    bool                    ObjIsValid;

    CleanState = (CFE_ES_CleanupState_t *)arg;
    ObjIsValid = true;

    ObjType = OS_IdentifyObject(ObjectId);
    switch(ObjType)
    {
    case OS_OBJECT_TYPE_OS_TASK:
        Status = OS_TaskDelete(ObjectId);
        break;
    case OS_OBJECT_TYPE_OS_QUEUE:
        Status = OS_QueueDelete(ObjectId);
        break;
    case OS_OBJECT_TYPE_OS_BINSEM:
        Status = OS_BinSemDelete(ObjectId);
        break;
    case OS_OBJECT_TYPE_OS_COUNTSEM:
        Status = OS_CountSemDelete(ObjectId);
        break;
    case OS_OBJECT_TYPE_OS_MUTEX:
        Status = OS_MutSemDelete(ObjectId);
        break;
    case OS_OBJECT_TYPE_OS_TIMECB:
        Status = OS_TimerDelete(ObjectId);
        break;
    case OS_OBJECT_TYPE_OS_STREAM:
        Status = OS_close(ObjectId);
        break;
    case OS_OBJECT_TYPE_OS_MODULE:
        Status = OS_ModuleUnload(ObjectId);
        break;
    default:
        ObjIsValid = false;
        Status = OS_ERROR;
        break;
    }

    if (ObjIsValid)
    {
        ++CleanState->FoundObjects;
        if (Status == OS_SUCCESS)
        {
            ++CleanState->DeletedObjects;
        }
        else
        {
            CFE_ES_SysLogWrite_Unsync("Call to OSAL Delete Object (ID:%lu) failed. RC=0x%08X\n",
                         OS_ObjectIdToInteger(ObjectId), (unsigned int)Status);
            if (CleanState->OverallStatus == CFE_SUCCESS)
            {
                /*
                 * Translate any OS failures into the appropriate CFE_ES return codes
                 * (Some object types have special return codes, depending on what type
                 * of object failed to delete)
                 */
                switch(ObjType)
                {
                case OS_OBJECT_TYPE_OS_TASK:
                    CleanState->OverallStatus = CFE_ES_ERR_CHILD_TASK_DELETE;
                    break;
                case OS_OBJECT_TYPE_OS_QUEUE:
                    CleanState->OverallStatus = CFE_ES_QUEUE_DELETE_ERR;
                    break;
                case OS_OBJECT_TYPE_OS_BINSEM:
                    CleanState->OverallStatus = CFE_ES_BIN_SEM_DELETE_ERR;
                    break;
                case OS_OBJECT_TYPE_OS_COUNTSEM:
                    CleanState->OverallStatus = CFE_ES_COUNT_SEM_DELETE_ERR;
                    break;
                case OS_OBJECT_TYPE_OS_MUTEX:
                    CleanState->OverallStatus = CFE_ES_MUT_SEM_DELETE_ERR;
                    break;
                case OS_OBJECT_TYPE_OS_TIMECB:
                    CleanState->OverallStatus = CFE_ES_TIMER_DELETE_ERR;
                    break;
                default:
                    /* generic failure */
                    CleanState->OverallStatus = CFE_ES_APP_CLEANUP_ERR;
                    break;
                }
            }
        }
    }
}

/*
**---------------------------------------------------------------------------------------
**   Name: CFE_ES_CleanupTaskResources
**
**   Purpose: Clean up the OS resources associated with an individual Task
**   Note: This is called when the ES global is UNLOCKED - so it should not touch
**         any ES global data structures.  It should only clean up at the OSAL level.
**---------------------------------------------------------------------------------------
*/
int32 CFE_ES_CleanupTaskResources(CFE_ES_ResourceID_t TaskId)
{
    CFE_ES_CleanupState_t   CleanState;
    int32                   Result;
    osal_id_t               OsalId;

    /* Get the Task ID for calling OSAL APIs (convert type) */
    OsalId = CFE_ES_ResourceID_ToOSAL(TaskId);

    /*
    ** Delete all OSAL resources that belong to this task
    */
    memset(&CleanState, 0, sizeof(CleanState));
    --CleanState.PrevFoundObjects;
    while (1)
    {
        OS_ForEachObject (OsalId, CFE_ES_CleanupObjectCallback, &CleanState);
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
        CleanState.ErrorFlag = (CleanState.DeletedObjects == 0 ||
                CleanState.FoundObjects >= CleanState.PrevFoundObjects);
        CleanState.PrevFoundObjects = CleanState.FoundObjects;
        CleanState.FoundObjects = 0;
        CleanState.DeletedObjects = 0;
    }

    /*
    ** Delete the task itself
    **
    ** Note, if the task self exited, then the ID becomes invalid.
    ** In this case the OS_ERR_INVALID_ID status is returned, but
    ** that is OK, there is nothing else needed to do.
    */
    Result = OS_TaskDelete(OsalId);
    if (Result == OS_SUCCESS || Result == OS_ERR_INVALID_ID)
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

    return(Result);

}

/*
**---------------------------------------------------------------------------------------
**   Name: CFE_ES_CopyModuleBasicInfo
**
**   Purpose: Populate the cFE_ES_AppInfo structure with the data for an app.
**
**   This internal function does not log any errors/events.  The caller is expected
**   to check the return code and log any relevant errors based on the context.
**---------------------------------------------------------------------------------------
*/
void CFE_ES_CopyModuleBasicInfo(const CFE_ES_ModuleLoadParams_t *ParamsPtr, CFE_ES_AppInfo_t *AppInfoPtr)
{
    strncpy(AppInfoPtr->Name, ParamsPtr->Name,
            sizeof(AppInfoPtr->Name)-1);
    AppInfoPtr->Name[sizeof(AppInfoPtr->Name)-1] = '\0';

    strncpy(AppInfoPtr->EntryPoint, ParamsPtr->EntryPoint,
            sizeof(AppInfoPtr->EntryPoint) - 1);
    AppInfoPtr->EntryPoint[sizeof(AppInfoPtr->EntryPoint) - 1] = '\0';

    strncpy(AppInfoPtr->FileName, ParamsPtr->FileName,
            sizeof(AppInfoPtr->FileName) - 1);
    AppInfoPtr->FileName[sizeof(AppInfoPtr->FileName) - 1] = '\0';
}

/*
**---------------------------------------------------------------------------------------
**   Name: CFE_ES_CopyModuleStatusInfo
**
**   Purpose: Populate the cFE_ES_AppInfo structure with the data for an app.
**
**   This internal function does not log any errors/events.  The caller is expected
**   to check the return code and log any relevant errors based on the context.
**---------------------------------------------------------------------------------------
*/
void CFE_ES_CopyModuleStatusInfo(const CFE_ES_ModuleLoadStatus_t *StatusPtr, CFE_ES_AppInfo_t *AppInfoPtr)
{
    AppInfoPtr->StartAddress = CFE_ES_MEMADDRESS_C(StatusPtr->EntryAddress);
}

/*
**---------------------------------------------------------------------------------------
**   Name: CFE_ES_CopyModuleAddressInfo
**
**   Purpose: Populate the cFE_ES_AppInfo structure with the data for an app.
**
**   This internal function does not log any errors/events.  The caller is expected
**   to check the return code and log any relevant errors based on the context.
**---------------------------------------------------------------------------------------
*/
void CFE_ES_CopyModuleAddressInfo(osal_id_t ModuleId, CFE_ES_AppInfo_t *AppInfoPtr)
{
    OS_module_prop_t   ModuleInfo;
    int32              ReturnCode;

    ReturnCode = OS_ModuleInfo ( ModuleId, &ModuleInfo );
    if ( ReturnCode == OS_SUCCESS )
    {
        AppInfoPtr->AddressesAreValid =
                (sizeof(ModuleInfo.addr.code_address) <= sizeof(AppInfoPtr->CodeAddress)) &&
                ModuleInfo.addr.valid;
    }
    else
    {
        AppInfoPtr->AddressesAreValid = false;
        memset(&ModuleInfo, 0, sizeof(ModuleInfo));
    }

    /*
     * Convert the internal size and address to the telemetry format.
     * (The telemetry format may be a different bitwidth than the native processor)
     */
    AppInfoPtr->CodeAddress = CFE_ES_MEMADDRESS_C(ModuleInfo.addr.code_address);
    AppInfoPtr->CodeSize = CFE_ES_MEMOFFSET_C(ModuleInfo.addr.code_size);
    AppInfoPtr->DataAddress = CFE_ES_MEMADDRESS_C(ModuleInfo.addr.data_address);
    AppInfoPtr->DataSize = CFE_ES_MEMOFFSET_C(ModuleInfo.addr.data_size);
    AppInfoPtr->BSSAddress = CFE_ES_MEMADDRESS_C(ModuleInfo.addr.bss_address);
    AppInfoPtr->BSSSize = CFE_ES_MEMOFFSET_C(ModuleInfo.addr.bss_size);
}


