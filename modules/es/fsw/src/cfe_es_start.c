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

#include "cfe_es_module_all.h"

#include <stdio.h>
#include <string.h>

static int32 CFE_ES_MainTaskSyncDelay(uint32 AppStateId, uint32 TimeOutMilliseconds);

/***************************************************************************/

/*
** Defines for this module
*/

/*
** Number of msecs to delay before exiting cFE. Allows LogMsg to get through
*/
#define CFE_ES_PANIC_DELAY 500

/*
** Global data for the ES startup code and Runtime library
*/
CFE_ES_Global_t CFE_ES_Global;

/***************************************************************************/
/*
** Code
*/

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_ES_Main(uint32 StartType, uint32 StartSubtype, uint32 ModeId, const char *StartFilePath)
{
    int32 OsStatus;

    /*
     * Clear the entire global data structure.
     * This also takes care of setting all resource IDs on all table entries
     * to be "undefined" (not in use).
     */
    memset(&CFE_ES_Global, 0, sizeof(CFE_ES_Global));

    /*
    ** Indicate that the CFE is the earliest initialization state
    */
    CFE_ES_Global.SystemState = CFE_ES_SystemState_EARLY_INIT;

    /*
    ** Create the ES Shared Data Mutex
    ** This must be done before ANY calls to CFE_ES_WriteToSysLog(), since this uses the mutex
    */
    OsStatus = OS_MutSemCreate(&(CFE_ES_Global.SharedDataMutex), "ES_DATA_MUTEX", 0);
    if (OsStatus != OS_SUCCESS)
    {
        /* Cannot use SysLog here, since that requires the reset area to be set up */
        OS_printf("ES Startup: Error: ES Shared Data Mutex could not be created. RC=%ld\n", (long)OsStatus);

        /*
        ** Delay to allow the message to be printed
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
    }

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
    ** Also Create the ES Performance Data Mutex
    ** This is to separately protect against concurrent writes to the global performance log data
    */
    OsStatus = OS_MutSemCreate(&CFE_ES_Global.PerfDataMutex, "ES_PERF_MUTEX", 0);
    if (OsStatus != OS_SUCCESS)
    {
        CFE_ES_SysLogWrite_Unsync("%s: Error: ES Performance Data Mutex could not be created. RC=%ld\n", __func__,
                                  (long)OsStatus);

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
    }

    /*
    ** Announce the startup
    */
    CFE_ES_WriteToSysLog("%s: CFE_ES_Main in EARLY_INIT state\n", __func__);

    /*
    ** Create and Mount the filesystems needed
    */
    CFE_ES_InitializeFileSystems(StartType);

    /*
    ** Install exception Handlers ( Placeholder )
    */
    CFE_PSP_AttachExceptions();

    /*
    ** Initialize the Last Id
    */
    CFE_ES_Global.LastAppId     = CFE_ResourceId_FromInteger(CFE_ES_APPID_BASE);
    CFE_ES_Global.LastLibId     = CFE_ResourceId_FromInteger(CFE_ES_LIBID_BASE);
    CFE_ES_Global.LastCounterId = CFE_ResourceId_FromInteger(CFE_ES_COUNTID_BASE);
    CFE_ES_Global.LastMemPoolId = CFE_ResourceId_FromInteger(CFE_ES_POOLID_BASE);

    /*
    ** Indicate that the CFE core is now starting up / going multi-threaded
    */
    CFE_ES_WriteToSysLog("%s: CFE_ES_Main entering CORE_STARTUP state\n", __func__);
    CFE_ES_Global.SystemState = CFE_ES_SystemState_CORE_STARTUP;

    /*
    ** Create the tasks, OS objects, and initialize hardware
    */
    CFE_ES_CreateObjects();

    /*
    ** Indicate that the CFE core is ready
    */
    CFE_ES_WriteToSysLog("%s: CFE_ES_Main entering CORE_READY state\n", __func__);
    CFE_ES_Global.SystemState = CFE_ES_SystemState_CORE_READY;

    /*
    ** Start the cFE Applications from the disk using the file
    ** specified in the CFE_PLATFORM_ES_NONVOL_STARTUP_FILE or CFE_PLATFORM_ES_VOLATILE_STARTUP_FILE
    ** ( defined in the cfe_platform_cfg.h file )
    */
    CFE_ES_StartApplications(StartType, StartFilePath);

    /*
     * Wait for applications to be in at least "LATE_INIT"
     *
     * However, if not everything starts up, that is not a fatal error, we will
     * continue anyway since the core apps are OK and control/telemetry should function.
     * The problem app could be deleted/restarted/etc by the ground station.
     */
    if (CFE_ES_MainTaskSyncDelay(CFE_ES_AppState_LATE_INIT, CFE_PLATFORM_ES_STARTUP_SCRIPT_TIMEOUT_MSEC) != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: Startup Sync failed - Applications may not have all initialized\n", __func__);
    }

    CFE_ES_WriteToSysLog("%s: CFE_ES_Main entering APPS_INIT state\n", __func__);
    CFE_ES_Global.SystemState = CFE_ES_SystemState_APPS_INIT;

    /*
     * Wait for applications to be "RUNNING" before moving to operational system state.
     *
     * However, if not everything starts up, that is not a fatal error, we will
     * continue anyway since the core apps are OK and control/telemetry should function.
     * The problem app could be deleted/restarted/etc by the ground station.
     */
    if (CFE_ES_MainTaskSyncDelay(CFE_ES_AppState_RUNNING, CFE_PLATFORM_ES_STARTUP_SCRIPT_TIMEOUT_MSEC) != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: Startup Sync failed - Applications may not have all started\n", __func__);
    }

    /*
    ** Startup is fully complete
    */
    CFE_ES_WriteToSysLog("%s: CFE_ES_Main entering OPERATIONAL state\n", __func__);
    CFE_ES_Global.SystemState = CFE_ES_SystemState_OPERATIONAL;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 * SYSLOGGING NOTE: Any logging in here must use CFE_ES_SysLogWrite_Unsync() as the necessary
 * primitives are not even initialized yet.  There is no chance for log contention here.
 *-----------------------------------------------------------------*/
void CFE_ES_SetupResetVariables(uint32 StartType, uint32 StartSubtype, uint32 BootSource)
{
    int32   PspStatus;
    uint32  resetAreaSize;
    cpuaddr ResetDataAddr;

    /*
    ** Get the pointer to the Reset area from the BSP
    */
    PspStatus = CFE_PSP_GetResetArea(&ResetDataAddr, &resetAreaSize);

    /*
    ** Make sure the status is OK or size is big enough
    */
    if (PspStatus != CFE_PSP_SUCCESS)
    {
        /*
        ** Cannot use the ES System log without the Reset Area
        */
        OS_printf("ES Startup: CFE_PSP_GetResetArea call Failed (0x%08x)!\n", (unsigned int)PspStatus);

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
    else if (resetAreaSize < sizeof(CFE_ES_ResetData_t))
    {
        /*
        ** Cannot use the ES system log without the Reset Area
        */
        OS_printf("ES Startup: Error: ES Reset area not big enough. Needed: %d, Given: %d.\n",
                  (int)sizeof(CFE_ES_ResetData_t), (int)resetAreaSize);
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

    CFE_ES_Global.ResetDataPtr = (CFE_ES_ResetData_t *)ResetDataAddr;

    /*
    ** Record the BootSource (bank) so it will be valid in the ER log entries.
    */
    CFE_ES_Global.ResetDataPtr->ResetVars.BootSource = BootSource;

    /*
    ** Determine how the system was started. The choices are:
    **   CFE_ES_POWER_ON_RESET, or CFE_PSP_RST_TYPE_PROCESSOR
    ** The subtypes include:
    **   CFE_PSP_RST_SUBTYPE_POWER_CYCLE, CFE_PSP_RST_SUBTYPE_PUSH_BUTTON, CFE_PSP_RST_SUBTYPE_HW_SPECIAL_COMMAND,
    **   CFE_PSP_RST_SUBTYPE_HW_WATCHDOG, CFE_PSP_RST_TYPE_COMMAND, or CFE_PSP_RST_SUBTYPE_EXCEPTION.
    ** Some of these reset types are logged before the system is restarted.
    **  ( CFE_PSP_RST_TYPE_COMMAND, CFE_PSP_RST_SUBTYPE_EXCEPTION ) while others occur
    **  without the knowledge of the software and must be logged here.
    */
    if (StartType == CFE_PSP_RST_TYPE_POWERON)
    {
        /*
        ** Record the reset type and subtype
        */
        CFE_ES_Global.ResetDataPtr->ResetVars.ResetSubtype = StartSubtype;
        CFE_ES_Global.ResetDataPtr->ResetVars.ResetType    = CFE_PSP_RST_TYPE_POWERON;

        /*
        ** Log the power-on reset.
        */
        if (StartSubtype == CFE_PSP_RST_SUBTYPE_POWER_CYCLE)
        {
            CFE_ES_SysLogWrite_Unsync("%s: POWER ON RESET due to Power Cycle (Power Cycle).\n", __func__);
            CFE_ES_WriteToERLog(CFE_ES_LogEntryType_CORE, CFE_PSP_RST_TYPE_POWERON, StartSubtype,
                                "POWER ON RESET due to Power Cycle (Power Cycle)");
        }
        else if (StartSubtype == CFE_PSP_RST_SUBTYPE_HW_SPECIAL_COMMAND)
        {
            CFE_ES_SysLogWrite_Unsync("%s: POWER ON RESET due to HW Special Cmd (Hw Spec Cmd).\n", __func__);
            CFE_ES_WriteToERLog(CFE_ES_LogEntryType_CORE, CFE_PSP_RST_TYPE_POWERON, StartSubtype,
                                "POWER ON RESET due to HW Special Cmd (Hw Spec Cmd)");
        }
        else
        {
            CFE_ES_SysLogWrite_Unsync("%s: POWER ON RESET due to other cause (See Subtype).\n", __func__);
            CFE_ES_WriteToERLog(CFE_ES_LogEntryType_CORE, CFE_PSP_RST_TYPE_POWERON, StartSubtype,
                                "POWER ON RESET due to other cause (See Subtype)");
        }

        /*
        ** Initialize all reset counters.
        */
        CFE_ES_Global.ResetDataPtr->ResetVars.ProcessorResetCount    = 0;
        CFE_ES_Global.ResetDataPtr->ResetVars.MaxProcessorResetCount = CFE_PLATFORM_ES_MAX_PROCESSOR_RESETS;
        CFE_ES_Global.DebugVars.DebugFlag                            = 0;
    }
    else if (StartType == CFE_PSP_RST_TYPE_PROCESSOR)
    {
        /*
        ** If a Processor reset was not commanded, it must be a watchdog or other non-commanded reset
        ** Log the reset before updating any reset variables.
        */
        if (CFE_ES_Global.ResetDataPtr->ResetVars.ES_CausedReset != true)
        {
            CFE_ES_Global.ResetDataPtr->ResetVars.ResetType = CFE_PSP_RST_TYPE_PROCESSOR;
            CFE_ES_Global.ResetDataPtr->ResetVars.ProcessorResetCount++;

            /*
            ** When coming up from a Processor reset that was not caused by ES, check to see
            ** if the maximum number has been exceeded
            */
            if (CFE_ES_Global.ResetDataPtr->ResetVars.ProcessorResetCount >
                CFE_ES_Global.ResetDataPtr->ResetVars.MaxProcessorResetCount)
            {
                if (StartSubtype == CFE_PSP_RST_SUBTYPE_HW_SPECIAL_COMMAND)
                {
                    CFE_ES_Global.ResetDataPtr->ResetVars.ResetSubtype = CFE_PSP_RST_SUBTYPE_HW_SPECIAL_COMMAND;
                    CFE_ES_SysLogWrite_Unsync("%s: POWER ON RESET due to max proc resets (HW Spec Cmd).\n", __func__);

                    /*
                    ** Log the reset in the ER Log. The log will be wiped out, but it's good to have
                    ** the entry just in case something fails.
                    */
                    CFE_ES_WriteToERLog(CFE_ES_LogEntryType_CORE, CFE_PSP_RST_TYPE_POWERON, StartSubtype,
                                        "POWER ON RESET due to max proc resets (HW Spec Cmd).");
                }
                else
                {
                    CFE_ES_Global.ResetDataPtr->ResetVars.ResetSubtype = CFE_PSP_RST_SUBTYPE_HW_WATCHDOG;
                    CFE_ES_SysLogWrite_Unsync("%s: POWER ON RESET due to max proc resets (Watchdog).\n", __func__);

                    /*
                    ** Log the reset in the ER Log. The log will be wiped out, but it's good to have
                    ** the entry just in case something fails.
                    */
                    CFE_ES_WriteToERLog(CFE_ES_LogEntryType_CORE, CFE_PSP_RST_TYPE_POWERON, StartSubtype,
                                        "POWER ON RESET due to max proc resets (Watchdog).");
                }
                /*
                ** Call the BSP reset routine
                */
                CFE_PSP_Restart(CFE_PSP_RST_TYPE_POWERON);

                /*
                ** Should not return here.
                */
                CFE_ES_SysLogWrite_Unsync("%s: Error: CFE_PSP_Restart returned.\n", __func__);
            }
            else /* Maximum processor reset not exceeded */
            {
                if (StartSubtype == CFE_PSP_RST_SUBTYPE_HW_SPECIAL_COMMAND)
                {
                    CFE_ES_Global.ResetDataPtr->ResetVars.ResetSubtype = CFE_PSP_RST_SUBTYPE_HW_SPECIAL_COMMAND;
                    CFE_ES_SysLogWrite_Unsync("%s: PROCESSOR RESET due to Hardware Special Command (HW Spec Cmd).\n",
                                              __func__);

                    /*
                    ** Log the watchdog reset
                    */
                    CFE_ES_WriteToERLog(CFE_ES_LogEntryType_CORE, CFE_PSP_RST_TYPE_PROCESSOR, StartSubtype,
                                        "PROCESSOR RESET due to Hardware Special Command (Hw Spec Cmd).");
                }
                else
                {
                    CFE_ES_Global.ResetDataPtr->ResetVars.ResetSubtype = CFE_PSP_RST_SUBTYPE_HW_WATCHDOG;
                    CFE_ES_SysLogWrite_Unsync("%s: PROCESSOR RESET due to Watchdog (Watchdog).\n", __func__);

                    /*
                    ** Log the watchdog reset
                    */
                    CFE_ES_WriteToERLog(CFE_ES_LogEntryType_CORE, CFE_PSP_RST_TYPE_PROCESSOR, StartSubtype,
                                        "PROCESSOR RESET due to Watchdog (Watchdog).");
                }
            }
        }
        /*
        ** If a processor reset is due to a command or exception, the reset has already been logged.
        ** Update the reset variables only.
        ** The logic for detecting maximum resets is done on the command/exception side
        ** on the "way down" when the command or exception handler is executed.
        */
        else
        {
            CFE_ES_Global.ResetDataPtr->ResetVars.ResetType    = CFE_PSP_RST_TYPE_PROCESSOR;
            CFE_ES_Global.ResetDataPtr->ResetVars.ResetSubtype = StartSubtype;
        }

        /*
        ** Initialize processor reset counters.
        */
        CFE_ES_Global.DebugVars.DebugFlag = 0;
    }

    /*
    ** Clear the commanded reset flag, in case a watchdog happens.
    */
    CFE_ES_Global.ResetDataPtr->ResetVars.ES_CausedReset = false;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_ES_InitializeFileSystems(uint32 StartType)
{
    int32        OsStatus;
    int32        PspStatus;
    cpuaddr      RamDiskMemoryAddress = 0;
    uint32       RamDiskMemorySize;
    int32        PercentFree;
    OS_statvfs_t StatBuf;

    memset(&StatBuf, 0, sizeof(StatBuf));

    /*
    ** Get the memory area for the RAM disk
    */
    PspStatus = CFE_PSP_GetVolatileDiskMem(&(RamDiskMemoryAddress), &(RamDiskMemorySize));

    if (PspStatus != CFE_PSP_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: Cannot Get Memory for Volatile Disk. EC = 0x%08X\n", __func__,
                             (unsigned int)PspStatus);

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
    if (StartType == CFE_PSP_RST_TYPE_POWERON)
    {
        OsStatus = OS_mkfs((void *)RamDiskMemoryAddress, "/ramdev0", "RAM", CFE_PLATFORM_ES_RAM_DISK_SECTOR_SIZE,
                           CFE_PLATFORM_ES_RAM_DISK_NUM_SECTORS);
        if (OsStatus != OS_SUCCESS)
        {
            CFE_ES_WriteToSysLog("%s: Error Creating Volatile(RAM) Volume. EC = %ld\n", __func__, (long)OsStatus);

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
        OsStatus = OS_initfs((void *)RamDiskMemoryAddress, "/ramdev0", "RAM", CFE_PLATFORM_ES_RAM_DISK_SECTOR_SIZE,
                             CFE_PLATFORM_ES_RAM_DISK_NUM_SECTORS);
        if (OsStatus != OS_SUCCESS)
        {
            CFE_ES_WriteToSysLog("%s: Error Initializing Volatile(RAM) Volume. EC = %ld\n", __func__, (long)OsStatus);
            CFE_ES_WriteToSysLog("%s: Formatting Volatile(RAM) Volume.\n", __func__);

            OsStatus = OS_mkfs((void *)RamDiskMemoryAddress, "/ramdev0", "RAM", CFE_PLATFORM_ES_RAM_DISK_SECTOR_SIZE,
                               CFE_PLATFORM_ES_RAM_DISK_NUM_SECTORS);
            if (OsStatus != OS_SUCCESS)
            {
                CFE_ES_WriteToSysLog("%s: Error Creating Volatile(RAM) Volume. EC = %ld\n", __func__, (long)OsStatus);

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
    OsStatus = OS_mount("/ramdev0", CFE_PLATFORM_ES_RAM_DISK_MOUNT_STRING);
    if (OsStatus != OS_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: Error Mounting Volatile(RAM) Volume. EC = %ld\n", __func__, (long)OsStatus);
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
    if ((StartType == CFE_PSP_RST_TYPE_PROCESSOR) && (CFE_PLATFORM_ES_RAM_DISK_PERCENT_RESERVED > 0))
    {
        /*
        ** See how many blocks are free in the RAM disk
        */
        OsStatus = OS_FileSysStatVolume(CFE_PLATFORM_ES_RAM_DISK_MOUNT_STRING, &StatBuf);
        if (OsStatus == OS_SUCCESS && StatBuf.total_blocks > 0)
        {
            /*
            ** Determine if the disk is too full
            */
            PercentFree = (StatBuf.blocks_free * 100) / StatBuf.total_blocks;
            CFE_ES_WriteToSysLog("%s: Volatile Disk has %d Percent free space.\n", __func__, (int)PercentFree);

            if (PercentFree < CFE_PLATFORM_ES_RAM_DISK_PERCENT_RESERVED)
            {
                CFE_ES_WriteToSysLog("%s: Insufficient Free Space on Volatile Disk, Reformatting.\n", __func__);

                /*
                ** First, unmount the disk
                */
                OsStatus = OS_unmount(CFE_PLATFORM_ES_RAM_DISK_MOUNT_STRING);
                if (OsStatus == OS_SUCCESS)
                {
                    /*
                    ** Remove the file system from the OSAL
                    */
                    OsStatus = OS_rmfs("/ramdev0");
                    if (OsStatus == OS_SUCCESS)
                    {
                        /*
                        ** Next, make a new file system on the disk
                        */
                        OsStatus = OS_mkfs((void *)RamDiskMemoryAddress, "/ramdev0", "RAM",
                                           CFE_PLATFORM_ES_RAM_DISK_SECTOR_SIZE, CFE_PLATFORM_ES_RAM_DISK_NUM_SECTORS);
                        if (OsStatus == OS_SUCCESS)
                        {
                            /*
                            ** Last, remount the disk
                            */
                            OsStatus = OS_mount("/ramdev0", CFE_PLATFORM_ES_RAM_DISK_MOUNT_STRING);
                            if (OsStatus != OS_SUCCESS)
                            {
                                CFE_ES_WriteToSysLog("%s: Error Re-Mounting Volatile(RAM) Volume. EC = %ld\n", __func__,
                                                     (long)OsStatus);
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
                            CFE_ES_WriteToSysLog("%s: Error Re-Formatting Volatile(RAM) Volume. EC = %ld\n", __func__,
                                                 (long)OsStatus);
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
                        CFE_ES_WriteToSysLog("%s: Error Removing Volatile(RAM) Volume. EC = %ld\n", __func__,
                                             (long)OsStatus);
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
                    CFE_ES_WriteToSysLog("%s: Error Un-Mounting Volatile(RAM) Volume. EC = %ld\n", __func__,
                                         (long)OsStatus);
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
        else /* could not determine free blocks */
        {
            /* Log error message -- note that BlocksFree returns the error code in this case */
            CFE_ES_WriteToSysLog("%s: Error Determining Blocks Free on Volume. EC = %ld\n", __func__, (long)OsStatus);

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
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_ES_CreateObjects(void)
{
    int32               ReturnCode;
    uint16              i;
    CFE_ES_AppRecord_t *AppRecPtr;
    CFE_ResourceId_t    PendingAppId;

    CFE_ES_WriteToSysLog("%s: Starting Object Creation calls.\n", __func__);

    for (i = 0; i < CFE_PLATFORM_ES_OBJECT_TABLE_SIZE; i++)
    {
        switch (CFE_ES_ObjectTable[i].ObjectType)
        {
            case CFE_ES_DRIVER_TASK:
            case CFE_ES_CORE_TASK:

                /*
                ** Allocate an ES AppTable entry
                */
                CFE_ES_LockSharedData(__func__, __LINE__);

                PendingAppId = CFE_ResourceId_FindNext(CFE_ES_Global.LastAppId, CFE_PLATFORM_ES_MAX_APPLICATIONS,
                                                       CFE_ES_CheckAppIdSlotUsed);
                AppRecPtr    = CFE_ES_LocateAppRecordByID(CFE_ES_APPID_C(PendingAppId));
                if (AppRecPtr != NULL)
                {
                    /*
                    ** Fill out the parameters in the AppStartParams sub-structure
                    */
                    AppRecPtr->Type = CFE_ES_AppType_CORE;

                    strncpy(AppRecPtr->AppName, CFE_ES_ObjectTable[i].ObjectName, sizeof(AppRecPtr->AppName) - 1);
                    AppRecPtr->AppName[sizeof(AppRecPtr->AppName) - 1] = '\0';

                    /* FileName and EntryPoint is not valid for core apps */
                    AppRecPtr->StartParams.MainTaskInfo.StackSize = CFE_ES_ObjectTable[i].ObjectSize;
                    AppRecPtr->StartParams.MainTaskInfo.Priority  = CFE_ES_ObjectTable[i].ObjectPriority;
                    AppRecPtr->StartParams.ExceptionAction        = CFE_ES_ExceptionAction_PROC_RESTART;

                    /*
                    ** Fill out the Task State info
                    */
                    AppRecPtr->ControlReq.AppControlRequest = CFE_ES_RunStatus_APP_RUN;
                    AppRecPtr->ControlReq.AppTimerMsec      = 0;

                    CFE_ES_AppRecordSetUsed(AppRecPtr, CFE_RESOURCEID_RESERVED);
                    CFE_ES_Global.LastAppId = PendingAppId;
                }

                CFE_ES_UnlockSharedData(__func__, __LINE__);

                /*
                ** If a slot was found, create the application
                */
                if (AppRecPtr != NULL)
                {
                    /*
                    ** Start the core app main task
                    ** (core apps are already in memory - no loading needed)
                    */
                    ReturnCode = CFE_ES_StartAppTask(
                        &AppRecPtr->MainTaskId, AppRecPtr->AppName, CFE_ES_ObjectTable[i].FuncPtrUnion.MainTaskPtr,
                        &AppRecPtr->StartParams.MainTaskInfo, CFE_ES_APPID_C(PendingAppId));

                    /*
                     * Finalize data in the app table entry, which must be done under lock.
                     * This transitions the entry from being RESERVED to the real type,
                     * either MAIN_TASK (success) or returning to INVALID (failure).
                     */
                    CFE_ES_LockSharedData(__func__, __LINE__);

                    if (ReturnCode == CFE_SUCCESS)
                    {
                        CFE_ES_AppRecordSetUsed(AppRecPtr, PendingAppId);

                        /*
                        ** Increment the Core App counter.
                        */
                        CFE_ES_Global.RegisteredCoreApps++;
                    }
                    else
                    {
                        /* failure mode - just clear the whole app table entry.
                         * This will set the AppType back to CFE_ES_ResourceType_INVALID (0),
                         * as well as clearing any other data that had been written */
                        memset(AppRecPtr, 0, sizeof(*AppRecPtr));
                    }

                    CFE_ES_UnlockSharedData(__func__, __LINE__);
                }
                else
                {
                    /* appSlot not found -- This should never happen!*/
                    CFE_ES_WriteToSysLog("%s: Error, No free application slots available for CORE App!\n", __func__);
                    ReturnCode = CFE_ES_ERR_APP_CREATE;
                }

                if (ReturnCode == CFE_SUCCESS)
                {
                    /*
                     * CFE_ES_MainTaskSyncDelay() will delay this thread until the
                     * newly-started thread calls CFE_ES_WaitForSystemState()
                     */
                    ReturnCode =
                        CFE_ES_MainTaskSyncDelay(CFE_ES_AppState_RUNNING, CFE_PLATFORM_CORE_MAX_STARTUP_MSEC * 1000);
                }

                if (ReturnCode != CFE_SUCCESS)
                {
                    CFE_ES_WriteToSysLog("%s: OS_TaskCreate error creating core App: %s: EC = 0x%08X\n", __func__,
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
                break;

            case CFE_ES_FUNCTION_CALL: /*----------------------------------------------------------*/

                if (CFE_ES_ObjectTable[i].FuncPtrUnion.FunctionPtr != NULL)
                {
                    CFE_ES_WriteToSysLog("%s: Calling %s\n", __func__, CFE_ES_ObjectTable[i].ObjectName);
                    /*
                    ** Call the function
                    */
                    ReturnCode = (*CFE_ES_ObjectTable[i].FuncPtrUnion.FunctionPtr)();
                    if (ReturnCode != CFE_SUCCESS)
                    {
                        CFE_ES_WriteToSysLog("%s: Error returned when calling function: %s: EC = 0x%08X\n", __func__,
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
                }
                else
                {
                    CFE_ES_WriteToSysLog("%s: bad function pointer ( table entry = %d).\n", __func__, i);
                }
                break;

            case CFE_ES_NULL_ENTRY: /*-------------------------------------------------------*/
                break;
            default:
                break;
        } /* end switch */

    } /* end for */

    CFE_ES_WriteToSysLog("%s: Finished ES CreateObject table entries.\n", __func__);
}

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 * Waits for all of the applications that CFE has started thus far to
 * reach the indicated state, by polling the app counters in a delay loop.
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_MainTaskSyncDelay(uint32 AppStateId, uint32 TimeOutMilliseconds)
{
    int32               Status;
    uint32              i;
    uint32              WaitTime;
    uint32              WaitRemaining;
    uint32              AppNotReadyCounter;
    CFE_ES_AppRecord_t *AppRecPtr;

    Status        = CFE_ES_OPERATION_TIMED_OUT;
    WaitRemaining = TimeOutMilliseconds;
    while (true)
    {
        AppNotReadyCounter = 0;

        /*
         * Count the number of apps that are NOT in (at least) in the state requested
         */
        CFE_ES_LockSharedData(__func__, __LINE__);
        AppRecPtr = CFE_ES_Global.AppTable;
        for (i = 0; i < CFE_PLATFORM_ES_MAX_APPLICATIONS; i++)
        {
            if (CFE_ES_AppRecordIsUsed(AppRecPtr) && (AppRecPtr->AppState < AppStateId))
            {
                ++AppNotReadyCounter;
            }
            ++AppRecPtr;
        }
        CFE_ES_UnlockSharedData(__func__, __LINE__);

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
