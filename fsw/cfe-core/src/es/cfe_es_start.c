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
**  File: 
**  cfe_es_start.c
**
**  Purpose:
**  This file contains the Main entrypoint and startup code for the cFE core.
**  The entry point is called by the board support package for the OS. When the
**  entry point is finished, the cFE should be fully initialized and running.
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**     cFE Flight Software Application Developers Guide
**
*/

/*
** Includes
*/

#include "private/cfe_private.h"
#include "cfe_es.h"
#include "cfe_es_global.h"       
#include "cfe_es_start.h" 
#include "cfe_es_apps.h"
#include "cfe_es_log.h"
#include "cfe_psp.h"

#include <stdio.h>
#include <string.h>

static int32 CFE_ES_MainTaskSyncDelay(uint32 AppMinState, uint32 TimeOutMilliseconds);

/***************************************************************************/

/*
** Defines for this module
*/

/* 
** Number of msecs to delay before exiting cFE. Allows LogMsg to get through 
*/
#define CFE_ES_PANIC_DELAY  500


/*
** Global data for the ES startup code and Runtime library
*/
CFE_ES_Global_t     CFE_ES_Global;

/*
** Pointer to the Reset data that is preserved on a processor reset
*/
CFE_ES_ResetData_t  *CFE_ES_ResetDataPtr;


/***************************************************************************/
/*
** Code
*/

/*
** Name: CFE_ES_Main
** Purpose: This is the entry point to the cFE application code.
**
*/
void CFE_ES_Main(uint32 StartType, uint32 StartSubtype, uint32 ModeId, const char *StartFilePath )
{
   uint32 i;
   int32 ReturnCode;

   /*
   ** Indicate that the CFE is the earliest initialization state
   */
   CFE_ES_Global.SystemState = CFE_ES_SystemState_EARLY_INIT;

   /*
   ** Initialize the Reset variables. This call is required
   ** Before most of the ES functions can be used including the 
   ** ES System log.
   */
   CFE_ES_SetupResetVariables(StartType, StartSubtype, ModeId);

   /*
   ** Initialize the Logic Perf variables
   ** Because this is in the ES Reset area, it must be called after
   ** CFE_ES_SetupResetVariables.
   */
   CFE_ES_SetupPerfVariables(StartType);

   /*
   ** Create the ES Shared Data Mutex
   ** This must be done before ANY calls to CFE_ES_WriteToSysLog(), since this uses the mutex
   */
   ReturnCode = OS_MutSemCreate(&(CFE_ES_Global.SharedDataMutex), "ES_DATA_MUTEX", 0 );
   if(ReturnCode != OS_SUCCESS)
   {
      CFE_ES_SysLogWrite_Unsync("ES Startup: Error: ES Shared Data Mutex could not be created. RC=0x%08X\n",
              (unsigned int)ReturnCode);

      /*
      ** Delay to allow the message to be read
      */
      OS_TaskDelay(CFE_ES_PANIC_DELAY);

      /*
      ** cFE Cannot continue to start up.
      */
      CFE_PSP_Panic(CFE_PSP_PANIC_STARTUP_SEM);

      /*
       * Normally CFE_PSP_Panic() will not return but it will under UT
       */
      return;
   } /* end if */

   /*
   ** Announce the startup
   */
   CFE_ES_WriteToSysLog("ES Startup: CFE_ES_Main in EARLY_INIT state\n");

   /*
   ** Create and Mount the filesystems needed
   */
   CFE_ES_InitializeFileSystems(StartType);
   
   /*
   ** Install exception Handlers ( Placeholder )
   */
   CFE_PSP_AttachExceptions();

   /*
   ** Initialize the ES Application Table
   ** to mark all entries as unused.
   */
   for ( i = 0; i < CFE_PLATFORM_ES_MAX_APPLICATIONS; i++ )
   {
      CFE_ES_Global.AppTable[i].AppState = CFE_ES_AppState_UNDEFINED;
   }
   
   /*
   ** Initialize the ES Task Table
   ** to mark all entries as unused.
   */
   for ( i = 0; i < OS_MAX_TASKS; i++ )
   {
      CFE_ES_Global.TaskTable[i].RecordUsed = false;
   }

   /*
   ** Initialize the ES Generic Counter Table
   ** to mark all entries as unused.
   */
   for ( i = 0; i < CFE_PLATFORM_ES_MAX_GEN_COUNTERS; i++ )
   {
      CFE_ES_Global.CounterTable[i].RecordUsed = false;
   }

   /*
   ** Indicate that the CFE core is now starting up / going multi-threaded
   */
   CFE_ES_WriteToSysLog("ES Startup: CFE_ES_Main entering CORE_STARTUP state\n");
   CFE_ES_Global.SystemState = CFE_ES_SystemState_CORE_STARTUP;

   /*
   ** Create the tasks, OS objects, and initialize hardware
   */
   CFE_ES_CreateObjects();

   /*
   ** Indicate that the CFE core is ready
   */
   CFE_ES_WriteToSysLog("ES Startup: CFE_ES_Main entering CORE_READY state\n");
   CFE_ES_Global.SystemState = CFE_ES_SystemState_CORE_READY;

   /*
   ** Start the cFE Applications from the disk using the file
   ** specified in the CFE_PLATFORM_ES_NONVOL_STARTUP_FILE or CFE_PLATFORM_ES_VOLATILE_STARTUP_FILE 
   ** ( defined in the cfe_platform_cfg.h file )
   */   
   CFE_ES_StartApplications(StartType, StartFilePath );

   /*
    * Wait for applications to be in at least "LATE_INIT"
    *
    * However, if not everything starts up, that is not a fatal error, we will
    * continue anyway since the core apps are OK and control/telemetry should function.
    * The problem app could be deleted/restarted/etc by the ground station.
    */
   if (CFE_ES_MainTaskSyncDelay(CFE_ES_AppState_LATE_INIT,
           CFE_PLATFORM_ES_STARTUP_SCRIPT_TIMEOUT_MSEC) != CFE_SUCCESS)
   {
       CFE_ES_WriteToSysLog("ES Startup: Startup Sync failed - Applications may not have all initialized\n");
   }

   CFE_ES_WriteToSysLog("ES Startup: CFE_ES_Main entering APPS_INIT state\n");
   CFE_ES_Global.SystemState = CFE_ES_SystemState_APPS_INIT;

   /*
    * Wait for applications to be "RUNNING" before moving to operational system state.
    *
    * However, if not everything starts up, that is not a fatal error, we will
    * continue anyway since the core apps are OK and control/telemetry should function.
    * The problem app could be deleted/restarted/etc by the ground station.
    */
   if (CFE_ES_MainTaskSyncDelay(CFE_ES_AppState_RUNNING,
           CFE_PLATFORM_ES_STARTUP_SCRIPT_TIMEOUT_MSEC) != CFE_SUCCESS)
   {
       CFE_ES_WriteToSysLog("ES Startup: Startup Sync failed - Applications may not have all started\n");
   }

   /*
   ** Startup is fully complete
   */
   CFE_ES_WriteToSysLog("ES Startup: CFE_ES_Main entering OPERATIONAL state\n");
   CFE_ES_Global.SystemState = CFE_ES_SystemState_OPERATIONAL;
}

/*
** Name: CFE_ES_SetupResetVariables
**
** Purpose: This function initializes the ES reset variables depending on the reset type.
**          It will also initiate a power on reset when too many processor resets
**           have happened.
**
** SYSLOGGING NOTE: Any logging in here must use CFE_ES_SysLogWrite_Unsync() as the necessary
** primitives are not even initialized yet.  There is no chance for log contention here.
**
*/
void CFE_ES_SetupResetVariables(uint32 StartType, uint32 StartSubtype, uint32 BootSource )
{

   int32  status;
   uint32 resetAreaSize;
   cpuaddr ResetDataAddr;
   
   /*
   ** Get the pointer to the Reset area from the BSP
   */
   status = CFE_PSP_GetResetArea (&ResetDataAddr, &resetAreaSize);
      
   /*
   ** Make sure the status is OK or size is big enough
   */
   if ( status != CFE_PSP_SUCCESS )
   {
      /*
      ** Cannot use the ES System log without the Reset Area
      */
      OS_printf("ES Startup: CFE_PSP_GetResetArea call Failed (0x%08x)!\n", (unsigned int)status);
      
      /*
      ** Delay to allow the message to be read
      */
      OS_TaskDelay(CFE_ES_PANIC_DELAY);
      
      /* 
      ** cFE Cannot continue to start up. 
      */
      CFE_PSP_Panic(CFE_PSP_PANIC_MEMORY_ALLOC);
      
      /*
       * Normally unreachable, except in UT where
       * CFE_PSP_Panic is a stub that may return
       */
      return;

   }
   else if ( resetAreaSize < sizeof(CFE_ES_ResetData_t))
   {
      /*
      ** Cannot use the ES system log without the Reset Area
      */
      OS_printf("ES Startup: Error: ES Reset area not big enough. Needed: %d, Given: %d.\n",
            (int)sizeof(CFE_ES_ResetData_t),
            (int)resetAreaSize);
      /*
      ** Delay to allow the message to be read
      */
      OS_TaskDelay(CFE_ES_PANIC_DELAY);
      
      /* 
      ** cFE Cannot continue to start up. 
      */
      CFE_PSP_Panic(CFE_PSP_PANIC_MEMORY_ALLOC);
      
      /*
       * Normally unreachable, except in UT where
       * CFE_PSP_Panic is a stub that may return
       */
      return;
   }

   CFE_ES_ResetDataPtr = (CFE_ES_ResetData_t *)ResetDataAddr;

   /*
   ** Record the BootSource (bank) so it will be valid in the ER log entries.
   */
   CFE_ES_ResetDataPtr->ResetVars.BootSource   = BootSource;

   /*
   ** Determine how the system was started. The choices are:
   **   CFE_ES_POWER_ON_RESET, or CFE_PSP_RST_TYPE_PROCESSOR
   ** The subtypes include:
   **   CFE_PSP_RST_SUBTYPE_POWER_CYCLE, CFE_ES_PUSH_BUTTON, CFE_PSP_RST_SUBTYPE_HW_SPECIAL_COMMAND,
   **   CFE_ES_HW_WATCHDOG, CFE_PSP_RST_TYPE_COMMAND, or CFE_ES_EXCEPTION.
   ** Some of these reset types are logged before the system is restarted.
   **  ( CFE_PSP_RST_TYPE_COMMAND, CFE_ES_EXCEPTION ) while others occur
   **  without the knowledge of the software and must be logged here.
   */
   if ( StartType == CFE_PSP_RST_TYPE_POWERON )
   {
      /*
      ** Record the reset type and subtype
      */
      CFE_ES_ResetDataPtr->ResetVars.ResetSubtype = StartSubtype;
      CFE_ES_ResetDataPtr->ResetVars.ResetType = CFE_PSP_RST_TYPE_POWERON;

      /*
      ** Log the power-on reset. 
      */
      if ( StartSubtype == CFE_PSP_RST_SUBTYPE_POWER_CYCLE )
      {
         CFE_ES_SysLogWrite_Unsync("POWER ON RESET due to Power Cycle (Power Cycle).\n");
         CFE_ES_WriteToERLog(CFE_ES_LogEntryType_CORE, CFE_PSP_RST_TYPE_POWERON, StartSubtype,
                             "POWER ON RESET due to Power Cycle (Power Cycle)", NULL,0 );
      }
      else if ( StartSubtype == CFE_PSP_RST_SUBTYPE_HW_SPECIAL_COMMAND )
      {
         CFE_ES_SysLogWrite_Unsync("POWER ON RESET due to HW Special Cmd (Hw Spec Cmd).\n");
         CFE_ES_WriteToERLog(CFE_ES_LogEntryType_CORE, CFE_PSP_RST_TYPE_POWERON, StartSubtype,
                             "POWER ON RESET due to HW Special Cmd (Hw Spec Cmd)", NULL,0 );
      }
      else
      {
         CFE_ES_SysLogWrite_Unsync("POWER ON RESET due to other cause (See Subtype).\n");
         CFE_ES_WriteToERLog(CFE_ES_LogEntryType_CORE, CFE_PSP_RST_TYPE_POWERON, StartSubtype,
                             "POWER ON RESET due to other cause (See Subtype)", NULL,0 );
      }

      /*
      ** Initialize all reset counters.
      */
      CFE_ES_ResetDataPtr->ResetVars.ProcessorResetCount = 0;
      CFE_ES_ResetDataPtr->ResetVars.MaxProcessorResetCount = CFE_PLATFORM_ES_MAX_PROCESSOR_RESETS;
      CFE_ES_Global.DebugVars.DebugFlag = 0;
      
   }
   else if ( StartType == CFE_PSP_RST_TYPE_PROCESSOR )
   {
      /*
      ** If a Processor reset was not commanded, it must be a watchdog or other non-commanded reset
      ** Log the reset before updating any reset variables.
      */
      if ( CFE_ES_ResetDataPtr->ResetVars.ES_CausedReset != true )
      {
         CFE_ES_ResetDataPtr->ResetVars.ResetType = CFE_PSP_RST_TYPE_PROCESSOR;
         CFE_ES_ResetDataPtr->ResetVars.ResetSubtype = StartSubtype; 
         CFE_ES_ResetDataPtr->ResetVars.ProcessorResetCount++;
         
         /*
         ** When coming up from a Processor reset that was not caused by ES, check to see 
         ** if the maximum number has been exceeded
         */
         if ( CFE_ES_ResetDataPtr->ResetVars.ProcessorResetCount > 
              CFE_ES_ResetDataPtr->ResetVars.MaxProcessorResetCount )
         {
             if ( StartSubtype == CFE_PSP_RST_SUBTYPE_HW_SPECIAL_COMMAND )
             {
                 CFE_ES_ResetDataPtr->ResetVars.ResetSubtype = CFE_PSP_RST_SUBTYPE_HW_SPECIAL_COMMAND;
                 CFE_ES_SysLogWrite_Unsync("POWER ON RESET due to max proc resets (HW Spec Cmd).\n");

                 /*
                 ** Log the reset in the ER Log. The log will be wiped out, but it's good to have
                 ** the entry just in case something fails.
                 */
                 CFE_ES_WriteToERLog(CFE_ES_LogEntryType_CORE, CFE_PSP_RST_TYPE_POWERON, StartSubtype,
                                     "POWER ON RESET due to max proc resets (HW Spec Cmd).", NULL,0 );
             }
             else
             {
                 CFE_ES_ResetDataPtr->ResetVars.ResetSubtype = CFE_PSP_RST_SUBTYPE_HW_WATCHDOG;
                 CFE_ES_SysLogWrite_Unsync("POWER ON RESET due to max proc resets (Watchdog).\n");

                 /*
                 ** Log the reset in the ER Log. The log will be wiped out, but it's good to have
                 ** the entry just in case something fails.
                 */
                 CFE_ES_WriteToERLog(CFE_ES_LogEntryType_CORE, CFE_PSP_RST_TYPE_POWERON, StartSubtype,
                                     "POWER ON RESET due to max proc resets (Watchdog).", NULL,0 );
             } 
             /*
             ** Call the BSP reset routine 
             */
             CFE_PSP_Restart(CFE_PSP_RST_TYPE_POWERON);
             
             /*
             ** Should not return here.
             */
             CFE_ES_SysLogWrite_Unsync("ES Startup: Error: CFE_PSP_Restart returned.\n");
           
         }
         else /* Maximum processor reset not exceeded */
         {
             if ( StartSubtype == CFE_PSP_RST_SUBTYPE_HW_SPECIAL_COMMAND )
             {
                CFE_ES_ResetDataPtr->ResetVars.ResetSubtype = CFE_PSP_RST_SUBTYPE_HW_SPECIAL_COMMAND;
                CFE_ES_SysLogWrite_Unsync("PROCESSOR RESET due to Hardware Special Command (HW Spec Cmd).\n");

                /*
                ** Log the watchdog reset 
                */
                CFE_ES_WriteToERLog(CFE_ES_LogEntryType_CORE, CFE_PSP_RST_TYPE_PROCESSOR, StartSubtype,
                                    "PROCESSOR RESET due to Hardware Special Command (Hw Spec Cmd).", NULL,0 );
 
             }
             else
             {
                CFE_ES_ResetDataPtr->ResetVars.ResetSubtype = CFE_PSP_RST_SUBTYPE_HW_WATCHDOG;
                CFE_ES_SysLogWrite_Unsync("PROCESSOR RESET due to Watchdog (Watchdog).\n");

                /*
                ** Log the watchdog reset 
                */
                CFE_ES_WriteToERLog(CFE_ES_LogEntryType_CORE, CFE_PSP_RST_TYPE_PROCESSOR, StartSubtype,
                                    "PROCESSOR RESET due to Watchdog (Watchdog).", NULL,0 );

             }
 
         } /* end if */
         
      }
      /*
      ** If a processor reset is due to a command or exception, the reset has already been logged.
      ** Update the reset variables only.
      ** The logic for detecting maximum resets is done on the command/exception side
      ** on the "way down" when the command or exception handler is executed.
      */
      else
      {
         CFE_ES_ResetDataPtr->ResetVars.ResetType    = CFE_PSP_RST_TYPE_PROCESSOR;
         CFE_ES_ResetDataPtr->ResetVars.ResetSubtype = StartSubtype;
      }

      /*
      ** Initialize processor reset counters.
      */
      CFE_ES_Global.DebugVars.DebugFlag = 0;
   }
   
   /*
   ** Clear the commanded reset flag, in case a watchdog happens.
   */
   CFE_ES_ResetDataPtr->ResetVars.ES_CausedReset = false;
      
}

/*
** Name: CFE_ES_InitializeFileSystems
**
** Purpose: This function initializes the file systems used in the cFE core.
**
*/
void CFE_ES_InitializeFileSystems(uint32 start_type)
{
   int32   RetStatus;
   cpuaddr RamDiskMemoryAddress;
   uint32  RamDiskMemorySize;
   int32   BlocksFree;
   int32   PercentFree;
 
   /* 
   ** Get the memory area for the RAM disk 
   */
   RetStatus = CFE_PSP_GetVolatileDiskMem(&(RamDiskMemoryAddress), &(RamDiskMemorySize));

   if ( RetStatus != CFE_PSP_SUCCESS )
   {
      CFE_ES_WriteToSysLog("ES Startup: Cannot Get Memory for Volatile Disk. EC = 0x%08X\n",(unsigned int)RetStatus);

      /*
      ** Delay to allow the message to be read
      */
      OS_TaskDelay(CFE_ES_PANIC_DELAY);
      
      /* 
      ** cFE Cannot continue to start up.  
      */
      CFE_PSP_Panic(CFE_PSP_PANIC_VOLATILE_DISK);

   }      
   
   /*
   ** Next, either format, or just initialize the RAM disk depending on
   ** the reset type
   */
   if ( start_type == CFE_PSP_RST_TYPE_POWERON )
   {
      RetStatus = OS_mkfs((void *)RamDiskMemoryAddress, "/ramdev0", "RAM", CFE_PLATFORM_ES_RAM_DISK_SECTOR_SIZE, CFE_PLATFORM_ES_RAM_DISK_NUM_SECTORS );
      if ( RetStatus != OS_FS_SUCCESS )
      {
         CFE_ES_WriteToSysLog("ES Startup: Error Creating Volatile(RAM) Volume. EC = 0x%08X\n",(unsigned int)RetStatus);

         /*
         ** Delay to allow the message to be read
         */
         OS_TaskDelay(CFE_ES_PANIC_DELAY);
      
         /* 
         ** cFE Cannot continue to start up.  
         */
         CFE_PSP_Panic(CFE_PSP_PANIC_VOLATILE_DISK);
      }
   }
   else
   {
      RetStatus = OS_initfs((void *)RamDiskMemoryAddress, "/ramdev0", "RAM", CFE_PLATFORM_ES_RAM_DISK_SECTOR_SIZE, CFE_PLATFORM_ES_RAM_DISK_NUM_SECTORS );
      if ( RetStatus != OS_FS_SUCCESS )
      {
         CFE_ES_WriteToSysLog("ES Startup: Error Initializing Volatile(RAM) Volume. EC = 0x%08X\n",(unsigned int)RetStatus);
         CFE_ES_WriteToSysLog("ES Startup: Formatting Volatile(RAM) Volume.\n");
         
         RetStatus = OS_mkfs((void *)RamDiskMemoryAddress, "/ramdev0", "RAM", CFE_PLATFORM_ES_RAM_DISK_SECTOR_SIZE, CFE_PLATFORM_ES_RAM_DISK_NUM_SECTORS );
         if ( RetStatus != OS_SUCCESS )
         {
            CFE_ES_WriteToSysLog("ES Startup: Error Creating Volatile(RAM) Volume. EC = 0x%08X\n",(unsigned int)RetStatus);

            /*
            ** Delay to allow the message to be read
            */
            OS_TaskDelay(CFE_ES_PANIC_DELAY);
      
            /* 
            ** cFE Cannot continue to start up.  
            */
            CFE_PSP_Panic(CFE_PSP_PANIC_VOLATILE_DISK);
         }
         
      }
   }

   /*
   ** Now, mount the RAM disk
   */
   RetStatus = OS_mount("/ramdev0", CFE_PLATFORM_ES_RAM_DISK_MOUNT_STRING);
   if ( RetStatus != OS_FS_SUCCESS )
   {
      CFE_ES_WriteToSysLog("ES Startup: Error Mounting Volatile(RAM) Volume. EC = 0x%08X\n",(unsigned int)RetStatus);
      /*
      ** Delay to allow the message to be read
      */
      OS_TaskDelay(CFE_ES_PANIC_DELAY);
      
      /* 
      ** cFE Cannot continue to start up.  
      */
      CFE_PSP_Panic(CFE_PSP_PANIC_VOLATILE_DISK);
   }


   /*
   ** During a Processor reset, if the RAM disk has less than a defined 
   ** amount of free space, reformat and re-mount it.
   ** The parameter being checked is CFE_PLATFORM_ES_RAM_DISK_PERCENT_RESERVED
   ** Note: When CFE_PLATFORM_ES_RAM_DISK_PERCENT_RESERVED is set to 0, this feature is 
   **       disabled.
   */
   if ((start_type == CFE_PSP_RST_TYPE_PROCESSOR) && (CFE_PLATFORM_ES_RAM_DISK_PERCENT_RESERVED > 0))
   {
      /*
      ** See how many blocks are free in the RAM disk
      */
      BlocksFree = OS_fsBlocksFree(CFE_PLATFORM_ES_RAM_DISK_MOUNT_STRING);   
      if ( BlocksFree >= 0 )
      {
         /*
         ** Need a sanity check for the desktop systems.
         ** Because the desktop ports map the volatile disk to the host 
         ** hard disk, it will report more free blocks than the defined number
         ** of sectors ( blocks ). Therefore it must be truncated.
         */
         if ( BlocksFree > CFE_PLATFORM_ES_RAM_DISK_NUM_SECTORS )
         {
             BlocksFree = CFE_PLATFORM_ES_RAM_DISK_NUM_SECTORS - 1;
         }
         
         /*
         ** Determine if the disk is too full 
         */
         BlocksFree = BlocksFree * 100;
         PercentFree = BlocksFree / CFE_PLATFORM_ES_RAM_DISK_NUM_SECTORS;
         CFE_ES_WriteToSysLog("Volatile Disk has %d Percent free space.\n",(int)PercentFree);

         if ( PercentFree < CFE_PLATFORM_ES_RAM_DISK_PERCENT_RESERVED )
         {
            CFE_ES_WriteToSysLog("ES Startup: Insufficent Free Space on Volatile Disk, Reformatting.\n");
          
            /*
            ** First, unmount the disk
            */
            RetStatus = OS_unmount(CFE_PLATFORM_ES_RAM_DISK_MOUNT_STRING);
            if ( RetStatus == OS_FS_SUCCESS )
            {

               /*
               ** Remove the file system from the OSAL
               */
               RetStatus = OS_rmfs("/ramdev0");
               if ( RetStatus == OS_FS_SUCCESS )
               {
               
                  /*
                  ** Next, make a new file system on the disk
                  */
                  RetStatus = OS_mkfs((void *)RamDiskMemoryAddress, "/ramdev0", 
                                      "RAM", CFE_PLATFORM_ES_RAM_DISK_SECTOR_SIZE, 
                                       CFE_PLATFORM_ES_RAM_DISK_NUM_SECTORS );
                  if ( RetStatus == OS_FS_SUCCESS )
                  {
                     /*
                     ** Last, remount the disk
                     */
                     RetStatus = OS_mount("/ramdev0", CFE_PLATFORM_ES_RAM_DISK_MOUNT_STRING);
                     if ( RetStatus != OS_FS_SUCCESS )
                     {
                        CFE_ES_WriteToSysLog("ES Startup: Error Re-Mounting Volatile(RAM) Volume. EC = 0x%08X\n",(unsigned int)RetStatus);
                        /*
                        ** Delay to allow the message to be read
                        */
                        OS_TaskDelay(CFE_ES_PANIC_DELAY);
                     
                        /* 
                        ** cFE Cannot continue to start up.  
                        */
                        CFE_PSP_Panic(CFE_PSP_PANIC_VOLATILE_DISK);
                     
                     } /* end if mount */
                  }
                  else
                  {

                     CFE_ES_WriteToSysLog("ES Startup: Error Re-Formating Volatile(RAM) Volume. EC = 0x%08X\n",(unsigned int)RetStatus);
                     /*
                     ** Delay to allow the message to be read
                     */
                     OS_TaskDelay(CFE_ES_PANIC_DELAY);
      
                     /* 
                     ** cFE Cannot continue to start up.  
                     */
                     CFE_PSP_Panic(CFE_PSP_PANIC_VOLATILE_DISK);
                                 
                  } /* end if mkfs */

              }
              else /* could not Remove File system */
              {

                  CFE_ES_WriteToSysLog("ES Startup: Error Removing Volatile(RAM) Volume. EC = 0x%08X\n",(unsigned int)RetStatus);
                  /*
                  ** Delay to allow the message to be read
                  */
                  OS_TaskDelay(CFE_ES_PANIC_DELAY);
      
                  /* 
                  ** cFE Cannot continue to start up.  
                  */
                  CFE_PSP_Panic(CFE_PSP_PANIC_VOLATILE_DISK);

              } /* end if OS_rmfs */

            }
            else /* could not un-mount disk */
            {
               CFE_ES_WriteToSysLog("ES Startup: Error Un-Mounting Volatile(RAM) Volume. EC = 0x%08X\n",(unsigned int)RetStatus);
               /*
               ** Delay to allow the message to be read
               */
               OS_TaskDelay(CFE_ES_PANIC_DELAY);
               
               /* 
               ** cFE Cannot continue to start up.  
               */
               CFE_PSP_Panic(CFE_PSP_PANIC_VOLATILE_DISK);            
            }
            
         } /* end if enough free space */
         
      }
      else  /* could not determine free blocks */
      {         
         /* Log error message -- note that BlocksFree returns the error code in this case */
         CFE_ES_WriteToSysLog("ES Startup: Error Determining Blocks Free on Volume. EC = 0x%08X\n",(unsigned int)BlocksFree);

         /*
         ** Delay to allow the message to be read
         */
         OS_TaskDelay(CFE_ES_PANIC_DELAY);

         /* 
         ** cFE Cannot continue to start up.  
         */
         CFE_PSP_Panic(CFE_PSP_PANIC_VOLATILE_DISK);
         
      } /* end if BlocksFree */
   
   } /* end if processor reset */
     
} /* end function */

/*-------------------------------------------------------------------------
**
**                  Functional Prolog
**
**   Name: CFE_ES_CreateObjects
**
**   Purpose: This function reads the es_object_table and performs all of the
**            application layer initialization.
**----------------------------------------------------------------------------
*/
void  CFE_ES_CreateObjects(void)
{
    int32     ReturnCode;
    uint32    TaskIndex;
    bool      AppSlotFound;
    uint16    i;
    uint16    j;

    CFE_ES_WriteToSysLog("ES Startup: Starting Object Creation calls.\n");

    for ( i = 0; i < CFE_PLATFORM_ES_OBJECT_TABLE_SIZE; i++ )
    {
        switch ( CFE_ES_ObjectTable[i].ObjectType )
        {
            case CFE_ES_DRIVER_TASK:
            case CFE_ES_CORE_TASK:

            /*
            ** Allocate an ES AppTable entry
            */
            AppSlotFound = false;
            for ( j = 0; j < CFE_PLATFORM_ES_MAX_APPLICATIONS; j++ )
            {
               if ( CFE_ES_Global.AppTable[j].AppState == CFE_ES_AppState_UNDEFINED )
               {
                  AppSlotFound = true;
                  break;
               }
            }

            /*
            ** If a slot was found, create the application
            */
            if ( AppSlotFound == true )
            {
            
               CFE_ES_LockSharedData(__func__,__LINE__);

               /*
               ** Allocate and populate the ES_AppTable entry
               */
               memset ( &(CFE_ES_Global.AppTable[j]), 0, sizeof(CFE_ES_AppRecord_t));
               /*
               ** Core apps still have the notion of an init/running state
               ** Set the state here to mark the record as used.
               */
               CFE_ES_Global.AppTable[j].AppState = CFE_ES_AppState_EARLY_INIT;
               
               CFE_ES_Global.AppTable[j].Type = CFE_ES_AppType_CORE;
               
               /*
               ** Fill out the parameters in the AppStartParams sub-structure
               */         
               strncpy((char *)CFE_ES_Global.AppTable[j].StartParams.Name, (char *)CFE_ES_ObjectTable[i].ObjectName, OS_MAX_API_NAME);
               CFE_ES_Global.AppTable[j].StartParams.Name[OS_MAX_API_NAME - 1] = '\0';
               /* EntryPoint field is not valid here for base apps */
               /* FileName is not valid for base apps, either */
               CFE_ES_Global.AppTable[j].StartParams.StackSize = CFE_ES_ObjectTable[i].ObjectSize;
               CFE_ES_Global.AppTable[j].StartParams.StartAddress = (cpuaddr)CFE_ES_ObjectTable[i].FuncPtrUnion.VoidPtr;
               CFE_ES_Global.AppTable[j].StartParams.ExceptionAction = CFE_ES_ExceptionAction_PROC_RESTART;
               CFE_ES_Global.AppTable[j].StartParams.Priority = CFE_ES_ObjectTable[i].ObjectPriority;
               
               
               /*
               ** Fill out the Task Info
               */
               strncpy((char *)CFE_ES_Global.AppTable[j].TaskInfo.MainTaskName, (char *)CFE_ES_ObjectTable[i].ObjectName, OS_MAX_API_NAME);
               CFE_ES_Global.AppTable[j].TaskInfo.MainTaskName[OS_MAX_API_NAME - 1] = '\0';
               CFE_ES_Global.AppTable[j].TaskInfo.NumOfChildTasks = 0;
               
               /*
               ** Create the task
               */
               ReturnCode = OS_TaskCreate(&CFE_ES_Global.AppTable[j].TaskInfo.MainTaskId, /* task id */
                                  CFE_ES_ObjectTable[i].ObjectName,              /* task name */
                                  CFE_ES_ObjectTable[i].FuncPtrUnion.MainAppPtr, /* task function pointer */
                                  NULL,                                          /* stack pointer */
                                  CFE_ES_ObjectTable[i].ObjectSize,              /* stack size */
                                  CFE_ES_ObjectTable[i].ObjectPriority,          /* task priority */
                                  OS_FP_ENABLED);                                /* task options */

               if(ReturnCode != OS_SUCCESS)
               {
                  CFE_ES_Global.AppTable[j].AppState = CFE_ES_AppState_UNDEFINED;
                  CFE_ES_UnlockSharedData(__func__,__LINE__);

                  CFE_ES_WriteToSysLog("ES Startup: OS_TaskCreate error creating core App: %s: EC = 0x%08X\n",
                                        CFE_ES_ObjectTable[i].ObjectName, (unsigned int)ReturnCode);

                  /*
                  ** Delay to allow the message to be read
                  */
                  OS_TaskDelay(CFE_ES_PANIC_DELAY);
      
                  /* 
                  ** cFE Cannot continue to start up.  
                  */
                  CFE_PSP_Panic(CFE_PSP_PANIC_CORE_APP);
                                              
               }
               else
               {
                  OS_ConvertToArrayIndex(CFE_ES_Global.AppTable[j].TaskInfo.MainTaskId, &TaskIndex);

                  /*
                  ** Allocate and populate the CFE_ES_Global.TaskTable entry
                  */
                  if ( CFE_ES_Global.TaskTable[TaskIndex].RecordUsed == true )
                  {
                     CFE_ES_WriteToSysLog("ES Startup: CFE_ES_Global.TaskTable record used error for App: %s, continuing.\n",
                                           CFE_ES_ObjectTable[i].ObjectName);
                  }
                  else
                  {
                     CFE_ES_Global.TaskTable[TaskIndex].RecordUsed = true;
                  }
                  CFE_ES_Global.TaskTable[TaskIndex].AppId = j;
                  CFE_ES_Global.TaskTable[TaskIndex].TaskId = CFE_ES_Global.AppTable[j].TaskInfo.MainTaskId;
                  strncpy((char *)CFE_ES_Global.TaskTable[TaskIndex].TaskName, (char *)CFE_ES_Global.AppTable[j].TaskInfo.MainTaskName, OS_MAX_API_NAME);
                  CFE_ES_Global.TaskTable[TaskIndex].TaskName[OS_MAX_API_NAME - 1] = '\0';

                  CFE_ES_WriteToSysLog("ES Startup: Core App: %s created. App ID: %d\n",
                                       CFE_ES_ObjectTable[i].ObjectName,j);
                                       
                  /*
                  ** Increment the registered App and Registered External Task variables.
                  */
                  CFE_ES_Global.RegisteredTasks++;
                  CFE_ES_Global.RegisteredCoreApps++;
                  
                  CFE_ES_UnlockSharedData(__func__,__LINE__);
                                                                                                      
               }
            }
            else /* appSlot not found -- This should never happen!*/
            {
               CFE_ES_WriteToSysLog("ES Startup: Error, No free application slots available for CORE App!\n");
               /*
               ** Delay to allow the message to be read
               */
               OS_TaskDelay(CFE_ES_PANIC_DELAY);
      
               /* 
               ** cFE Cannot continue to start up.  
               */
               CFE_PSP_Panic(CFE_PSP_PANIC_CORE_APP);
            
            }

            /*
             * CFE_ES_MainTaskSyncDelay() will delay this thread until the
             * newly-started thread calls CFE_ES_WaitForSystemState()
             */
            if (CFE_ES_MainTaskSyncDelay(CFE_ES_AppState_RUNNING, CFE_PLATFORM_CORE_MAX_STARTUP_MSEC) != CFE_SUCCESS)
            {
                CFE_ES_WriteToSysLog("ES Startup: Core App %s did not complete initialization\n",
                                      CFE_ES_ObjectTable[i].ObjectName);

                /*
                ** Delay to allow the message to be read
                */
                OS_TaskDelay(CFE_ES_PANIC_DELAY);

                /*
                ** cFE Cannot continue to start up.
                */
                CFE_PSP_Panic(CFE_PSP_PANIC_CORE_APP);
            }
            break;

            case CFE_ES_FUNCTION_CALL: /*----------------------------------------------------------*/

               if ( CFE_ES_ObjectTable[i].FuncPtrUnion.FunctionPtr != NULL )
               {
                  CFE_ES_WriteToSysLog("ES Startup: Calling %s\n",CFE_ES_ObjectTable[i].ObjectName);
                  /*
                  ** Call the function
                  */
                  ReturnCode = (*CFE_ES_ObjectTable[i].FuncPtrUnion.FunctionPtr)();
                  if(ReturnCode != CFE_SUCCESS)
                  {
                     CFE_ES_WriteToSysLog("ES Startup: Error returned when calling function: %s: EC = 0x%08X\n",
                                           CFE_ES_ObjectTable[i].ObjectName, (unsigned int)ReturnCode);
                                        
                     /*
                     ** Delay to allow the message to be read
                     */
                     OS_TaskDelay(CFE_ES_PANIC_DELAY);
      
                     /* 
                     ** cFE Cannot continue to start up.  
                     */
                     CFE_PSP_Panic(CFE_PSP_PANIC_CORE_APP);
                                              
                  } /* end if */
                  
               }
               else
               {
                  CFE_ES_WriteToSysLog("ES Startup: bad function pointer ( table entry = %d).\n",i);
               }
               break;

            case CFE_ES_NULL_ENTRY: /*-------------------------------------------------------*/
               break;
            default:
               break;
       } /* end switch */

    } /* end for */

    CFE_ES_WriteToSysLog("ES Startup: Finished ES CreateObject table entries.\n");
}

/*
** Function: CFE_ES_MainTaskSyncDelay
**
** Purpose:  Waits for all of the applications that CFE has started thus far to
**           reach the indicated state, by polling the app counters in a delay loop.
**
*/
int32 CFE_ES_MainTaskSyncDelay(uint32 AppStateId, uint32 TimeOutMilliseconds)
{
    int32 Status;
    uint32 i;
    uint32 WaitTime;
    uint32 WaitRemaining;
    uint32 AppNotReadyCounter;

    Status = CFE_ES_OPERATION_TIMED_OUT;
    WaitRemaining = TimeOutMilliseconds;
    while (true)
    {
        AppNotReadyCounter = 0;

        /*
         * Count the number of apps that are NOT in (at least) in the state requested
         */
        CFE_ES_LockSharedData(__func__,__LINE__);
        for ( i = 0; i < CFE_PLATFORM_ES_MAX_APPLICATIONS; i++ )
        {
           if (CFE_ES_Global.AppTable[i].AppState != CFE_ES_AppState_UNDEFINED &&
               (CFE_ES_Global.AppTable[i].AppState < AppStateId))
           {
               ++AppNotReadyCounter;
           }
        }
        CFE_ES_UnlockSharedData(__func__,__LINE__);

        if (AppNotReadyCounter == 0)
        {
            /* Condition Met */
            Status = CFE_SUCCESS;
            break;
        }

        /*
         * Must delay and check again
         */
        if (WaitRemaining > CFE_PLATFORM_ES_STARTUP_SYNC_POLL_MSEC)
        {
            WaitTime = CFE_PLATFORM_ES_STARTUP_SYNC_POLL_MSEC;
        }
        else if (WaitRemaining > 0)
        {
            WaitTime = WaitRemaining;
        }
        else
        {
            break;
        }

        OS_TaskDelay(WaitTime);
        WaitRemaining -= WaitTime;
    }

    return Status;
}

