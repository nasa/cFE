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
 *   CFE Executive Services (CFE_ES) Application Private Config Definitions
 *
 * This provides default values for configurable items that are internal
 * to this module and do NOT affect the interface(s) of this module.  Changes
 * to items in this file only affect the local module and will be transparent
 * to external entities that are using the public interface(s).
 *
 * @note This file may be overridden/superceded by mission-provided definitions
 * either by overriding this header or by generating definitions from a command/data
 * dictionary tool.
 */
#ifndef CFE_ES_INTERNAL_CFG_H
#define CFE_ES_INTERNAL_CFG_H

/**
**  \cfeescfg Define ES Task Priority
**
**  \par Description:
**       Defines the cFE_ES Task priority.
**
**  \par Limits
**       Not Applicable
*/
#define CFE_PLATFORM_ES_START_TASK_PRIORITY 68

/**
**  \cfeescfg Define ES Task Stack Size
**
**  \par Description:
**       Defines the cFE_ES Task Stack Size
**
**  \par Limits
**       There is a lower limit of 2048 on this configuration parameter.  There
**       are no restrictions on the upper limit however, the maximum stack size
**       is system dependent and should be verified.  Most operating systems provide
**       tools for measuring the amount of stack used by a task during operation. It
**       is always a good idea to verify that no more than 1/2 of the stack is used.
*/
#define CFE_PLATFORM_ES_START_TASK_STACK_SIZE CFE_PLATFORM_ES_DEFAULT_STACK_SIZE

/**
**  \cfeescfg Default virtual path for persistent storage
**
**  \par Description:
**       This configures the default location in the virtual file system
**       for persistent/non-volatile storage.  Files such as the startup
**       script, app/library dynamic modules, and configuration tables are
**       expected to be stored in this directory.
**
*/
#define CFE_PLATFORM_ES_NONVOL_DISK_MOUNT_STRING "/cf"

/**
**  \cfeescfg Default virtual path for volatile storage
**
**  \par Description:
**      The #CFE_PLATFORM_ES_RAM_DISK_MOUNT_STRING parameter is used to set the cFE mount path
**      for the CFE RAM disk. This is a parameter for missions that do not want to
**      use the default value of "/ram", or for missions that need to have a different
**      value for different CPUs or Spacecraft.
**      Note that the vxWorks OSAL cannot currently handle names that have more than one
**      path separator in it. The names "/ram", "/ramdisk", "/disk123" will all work, but
**      "/disks/ram" will not.
**      Multiple separators can be used with the posix or RTEMS ports.
**
*/
#define CFE_PLATFORM_ES_RAM_DISK_MOUNT_STRING "/ram"

/**
**  \cfeescfg Define Max Number of Applications
**
**  \par Description:
**       Defines the maximum number of applications that can be loaded into the
**       system. This number does not include child tasks.
**
**  \par Limits
**       There is a lower limit of 6.  The lower limit corresponds to the cFE internal
**       applications.  There are no restrictions on the upper limit however, the
**       maximum number of applications is system dependent and should be verified.
**       AppIDs that are checked against this configuration are defined by a 32 bit
**       data word.
*/
#define CFE_PLATFORM_ES_MAX_APPLICATIONS 32

/**
**  \cfeescfg Define Max Number of Shared libraries
**
**  \par Description:
**       Defines the maximum number of cFE Shared libraries that can be loaded into
**       the system.
**
**  \par Limits
**       There is a lower limit of 1.  There are no restrictions on the upper limit
**       however, the maximum number of libraries is system dependent and should be
**       verified.
*/
#define CFE_PLATFORM_ES_MAX_LIBRARIES 10

/**
**  \cfeescfg Define Max Number of ER (Exception and Reset) log entries
**
**  \par Description:
**       Defines the maximum number of ER (Exception and Reset) log entries
**
**  \par Limits
**       There is a lower limit of 1.  There are no restrictions on the upper limit
**       however, the maximum number of log entries is system dependent and should be
**       verified.
*/
#define CFE_PLATFORM_ES_ER_LOG_ENTRIES 20

/** \cfeescfg Maximum size of CPU Context in ES Error Log
**
**  \par Description:
**       This should be large enough to accommodate the CPU context
**       information supplied by the PSP on the given platform.
**
**  \par Limits:
**       Must be greater than zero and a multiple of sizeof(uint32).
**       Limited only by the available memory and the number of entries
**       in the error log. Any context information beyond this size will
**       be truncated.
*/
#define CFE_PLATFORM_ES_ER_LOG_MAX_CONTEXT_SIZE 256

/**
**  \cfeescfg Define Size of the cFE System Log.
**
**  \par Description:
**       Defines the size in bytes of the cFE system log. The system log holds
**       variable length strings that are terminated by a linefeed and null
**       character.
**
**  \par Limits
**       There is a lower limit of 512.  There are no restrictions on the upper limit
**       however, the maximum system log size is system dependent and should be
**       verified.
*/
#define CFE_PLATFORM_ES_SYSTEM_LOG_SIZE 3072

/**
**  \cfeescfg Define Number of entries in the ES Object table
**
**  \par Description:
**       Defines the number of entries in the ES Object table. This table controls
**       the core cFE startup.
**
**  \par Limits
**       There is a lower limit of 15.  There are no restrictions on the upper limit
**       however, the maximum object table size is system dependent and should be
**       verified.
*/
#define CFE_PLATFORM_ES_OBJECT_TABLE_SIZE 30

/**
**  \cfeescfg Define Max Number of Generic Counters
**
**  \par Description:
**       Defines the maximum number of Generic Counters that can be registered.
**
**  \par Limits
**       This parameter has a lower limit of 1 and an upper limit of 65535.
*/
#define CFE_PLATFORM_ES_MAX_GEN_COUNTERS 8

/**
**  \cfeescfg Define ES Application Control Scan Rate
**
**  \par Description:
**       ES Application Control Scan Rate. This parameter controls the speed that ES
**       scans the Application Table looking for App Delete/Restart/Reload requests.
**       All Applications are deleted, restarted, or reloaded by the ES Application.
**       ES will periodically scan for control requests to process. The scan rate is
**       controlled by this parameter, which is given in milliseconds. A value of
**       1000 means that ES will scan the Application Table once per second. Be
**       careful not to set the value of this too low, because ES will use more CPU
**       cycles scanning the table.
**
**  \par Limits
**       There is a lower limit of 100 and an upper limit of 20000 on this
**       configuration parameter. millisecond units.
*/
#define CFE_PLATFORM_ES_APP_SCAN_RATE 1000

/**
**  \cfeescfg Define ES Application Kill Timeout
**
**  \par Description:
**      ES Application Kill Timeout. This parameter controls the number of
**      "scan periods" that ES will wait for an application to Exit after getting
**      the signal Delete, Reload or Restart. The sequence works as follows:
**      -# ES will set the control request for an App to Delete/Restart/Reload and
**         set this kill timer to the value in this parameter.
**      -# If the App is responding and Calls it's RunLoop function, it will drop out
**         of it's main loop and call CFE_ES_ExitApp. Once it calls Exit App, then
**         ES can delete, restart, or reload the app the next time it scans the app
**         table.
**      -# If the App is not responding, the ES App will decrement this Kill Timeout
**         value each time it runs. If the timeout value reaches zero, ES will kill
**         the app.
**
**      The Kill timeout value depends on the #CFE_PLATFORM_ES_APP_SCAN_RATE. If the Scan Rate
**      is 1000, or 1 second, and this #CFE_PLATFORM_ES_APP_KILL_TIMEOUT is set to 5, then it
**      will take 5 seconds to kill a non-responding App.
**      If the Scan Rate is 250, or 1/4 second, and the #CFE_PLATFORM_ES_APP_KILL_TIMEOUT is
**      set to 2, then it will take 1/2 second to time out.
**
**  \par Limits
**       There is a lower limit of 1 and an upper limit of 100 on this configuration
**       parameter. Units are number of #CFE_PLATFORM_ES_APP_SCAN_RATE cycles.
*/
#define CFE_PLATFORM_ES_APP_KILL_TIMEOUT 5

/**
**  \cfeescfg ES Ram Disk Sector Size
**
**  \par Description:
**       Defines the ram disk sector size. The ram disk is 1 of 4 memory areas that
**       are preserved on a processor reset.
**       NOTE: Changing this value changes memory allocation, and may
**       require changes to platform specific values (in CFE_PSP) such as
**       USER_RESERVED_MEM in VxWorks depending on the memory areas
**       being used for preserved data and on OS specific behavior.
**
**  \par Limits
**       There is a lower limit of 128.  There are no restrictions on the upper limit
**       however, the maximum RAM disk sector size is system dependent and should be
**       verified.
*/
#define CFE_PLATFORM_ES_RAM_DISK_SECTOR_SIZE 512

/**
**  \cfeescfg ES Ram Disk Number of Sectors
**
**  \par Description:
**       Defines the ram disk number of sectors. The ram disk is one of four memory
**       areas that are preserved on a processor reset.
**       NOTE: Changing this value changes memory allocation, and may
**       require changes to platform specific values (in CFE_PSP) such as
**       USER_RESERVED_MEM in VxWorks depending on the memory areas
**       being used for preserved data and on OS specific behavior.
**
**  \par Limits
**       There is a lower limit of 128.  There are no restrictions on the upper limit
**       however, the maximum number of RAM sectors is system dependent and should be
**       verified.
*/
#define CFE_PLATFORM_ES_RAM_DISK_NUM_SECTORS 4096

/**
**  \cfeescfg Percentage of Ram Disk Reserved for Decompressing Apps
**
**  \par Description:
**      The #CFE_PLATFORM_ES_RAM_DISK_PERCENT_RESERVED parameter is used to make sure that the
**      Volatile ( RAM ) Disk has a defined amount of free space during a processor
**      reset. The cFE uses the Volatile disk to decompress cFE applications during
**      system startup. If this Volatile disk happens to get filled with logs and
**      misc files, then a processor reset may not work, because there will be no
**      room to decompress cFE apps. To solve that problem, this parameter sets the
**      "Low Water Mark" for disk space on a Processor reset. It should be set to
**      allow the largest cFE Application to be decompressed.
**      During a Processor reset, if there is not sufficient space left on the disk,
**      it will be re-formatted in order to clear up some space.
**
**      This feature can be turned OFF by setting the parameter to 0.
**
**  \par Limits
**       There is a lower limit of 0 and an upper limit of 75 on this configuration
**       parameter.Units are percentage. A setting of zero will turn this feature
**       off.
*/
#define CFE_PLATFORM_ES_RAM_DISK_PERCENT_RESERVED 30

/**
**  \cfeescfg Define Critical Data Store Size
**
**  \par Description:
**       Defines the Critical Data Store (CDS) area size in bytes size. The CDS is
**       one of four memory areas that are preserved during a processor reset.
**       NOTE: Changing this value changes memory allocation, and may
**       require changes to platform specific values (in CFE_PSP) such as
**       USER_RESERVED_MEM in VxWorks depending on the memory areas
**       being used for preserved data and on OS specific behavior.
**
**  \par Limits
**       There is a lower limit of 8192 and an upper limit of UINT_MAX (4 Gigabytes)
**       on this configuration parameter.
*/
#define CFE_PLATFORM_ES_CDS_SIZE (128 * 1024)

/**
**  \cfeescfg Define User Reserved Memory Size
**
**  \par Description:
**       User Reserved Memory Size. This is the size in bytes of the cFE User
**       reserved Memory area. This is a block of memory that is available for cFE
**       application use. The address is obtained by calling
**       #CFE_PSP_GetUserReservedArea. The User Reserved Memory is one of four memory
**       areas that are preserved during a processor reset.
**       NOTE: Changing this value changes memory allocation, and may
**       require changes to platform specific values (in CFE_PSP) such as
**       USER_RESERVED_MEM in VxWorks depending on the memory areas
**       being used for preserved data and on OS specific behavior.
**
**  \par Limits
**       There is a lower limit of 1024 and an upper limit of UINT_MAX (4 Gigabytes)
**       on this configuration parameter.
*/
#define CFE_PLATFORM_ES_USER_RESERVED_SIZE (1024 * 1024)

/**
**  \cfeescfg Define Memory Pool Alignment Size
**
**  \par Description:
**       Ensures that buffers obtained from a memory pool are aligned
**       to a certain minimum block size.  Note the allocator will always
**       align to the minimum required by the CPU architecture.  This may
**       be set greater than the CPU requirement as desired for optimal
**       performance.
**
**       For some architectures/applications it may be beneficial to set this
**       to the cache line size of the target CPU, or to use special SIMD
**       instructions that require a more stringent memory alignment.
**
**  \par Limits
**       This must always be a power of 2, as it is used as a binary address mask.
*/
#define CFE_PLATFORM_ES_MEMPOOL_ALIGN_SIZE_MIN 4

/**
**  \cfeescfg ES Nonvolatile Startup Filename
**
**  \par Description:
**       The value of this constant defines the path and name of the file that
**       contains a list of modules that will be loaded and started by the cFE after
**       the cFE finishes its startup sequence.
**
**  \par Limits
**       The length of each string, including the NULL terminator cannot exceed the
**       #OS_MAX_PATH_LEN value.
*/
#define CFE_PLATFORM_ES_NONVOL_STARTUP_FILE "/cf/cfe_es_startup.scr"

/**
**  \cfeescfg ES Volatile Startup Filename
**
**  \par Description:
**       The value of this constant defines the path and name of the file that
**       contains a list of modules that will be loaded and started by the cFE after
**       the cFE finishes its startup sequence.
**
**  \par Limits
**       The length of each string, including the NULL terminator cannot exceed the
**       #OS_MAX_PATH_LEN value.
*/
#define CFE_PLATFORM_ES_VOLATILE_STARTUP_FILE "/ram/cfe_es_startup.scr"

/**
**  \cfeescfg Default Application Information Filename
**
**  \par Description:
**       The value of this constant defines the filename used to store information
**       pertaining to all of the Applications that are registered with Executive
**       Services. This filename is used only when no filename is specified in the
**       the command to query all system apps.
**
**  \par Limits
**       The length of each string, including the NULL terminator cannot exceed the
**       #OS_MAX_PATH_LEN value.
*/
#define CFE_PLATFORM_ES_DEFAULT_APP_LOG_FILE "/ram/cfe_es_app_info.log"

/**
**  \cfeescfg Default Application Information Filename
**
**  \par Description:
**       The value of this constant defines the filename used to store information
**       pertaining to all of the Applications that are registered with Executive
**       Services. This filename is used only when no filename is specified in the
**       the command to query all system tasks.
**
**  \par Limits
**       The length of each string, including the NULL terminator cannot exceed the
**       #OS_MAX_PATH_LEN value.
*/
#define CFE_PLATFORM_ES_DEFAULT_TASK_LOG_FILE "/ram/cfe_es_taskinfo.log"

/**
**  \cfeescfg Default System Log Filename
**
**  \par Description:
**       The value of this constant defines the filename used to store important
**       information (as ASCII text strings) that might not be able to be sent in an
**       Event Message. This filename is used only when no filename is specified in
**       the command to dump the system log. No file specified in the cmd means the
**       first character in the cmd filename is a NULL terminator (zero).
**
**  \par Limits
**       The length of each string, including the NULL terminator cannot exceed the
**       #OS_MAX_PATH_LEN value.
*/
#define CFE_PLATFORM_ES_DEFAULT_SYSLOG_FILE "/ram/cfe_es_syslog.log"

/**
**  \cfeescfg Default Exception and Reset (ER) Log Filename
**
**  \par Description:
**       The value of this constant defines the filename used to store the
**       Exception and Reset (ER) Log. This filename is used only when no filename is
**       specified in the command to dump the ER log. No file specified in the cmd
**       means the first character in the cmd filename is a NULL terminator (zero).
**
**  \par Limits
**       The length of each string, including the NULL terminator cannot exceed the
**       #OS_MAX_PATH_LEN value.
*/
#define CFE_PLATFORM_ES_DEFAULT_ER_LOG_FILE "/ram/cfe_erlog.log"

/**
**  \cfeescfg Default Performance Data Filename
**
**  \par Description:
**       The value of this constant defines the filename used to store the
**       Performance Data. This filename is used only when no filename is specified
**       in the command to stop performance data collecting.
**
**  \par Limits
**       The length of each string, including the NULL terminator cannot exceed the
**       #OS_MAX_PATH_LEN value.
*/
#define CFE_PLATFORM_ES_DEFAULT_PERF_DUMP_FILENAME "/ram/cfe_es_perf.dat"

/**
**  \cfeescfg Default Critical Data Store Registry Filename
**
**  \par Description:
**       The value of this constant defines the filename used to store the
**       Critical Data Store Registry. This filename is used only when no filename is
**       specified in the command to stop performance data collecting.
**
**  \par Limits
**       The length of each string, including the NULL terminator cannot exceed the
**       #OS_MAX_PATH_LEN value.
*/
#define CFE_PLATFORM_ES_DEFAULT_CDS_REG_DUMP_FILE "/ram/cfe_cds_reg.log"

/**
**  \cfeescfg Define Default System Log Mode following Power On Reset
**
**  \par Description:
**       Defines the default mode for the operation of the ES System log following a power
**       on reset. The log may operate in either Overwrite mode = 0, where once the
**       log becomes full the oldest message in the log will be overwritten, or
**       Discard mode = 1, where once the log becomes full the contents of the log are
**       preserved and the new event is discarded.  This constant may hold a value of
**       either 0 or 1 depending on the desired default.
**       Overwrite Mode = 0, Discard Mode = 1.
**
**  \par Limits
**       There is a lower limit of 0 and an upper limit of 1 on this configuration
**       parameter.
*/
#define CFE_PLATFORM_ES_DEFAULT_POR_SYSLOG_MODE 0

/**
**  \cfeescfg Define Default System Log Mode following Processor Reset
**
**  \par Description:
**       Defines the default mode for the operation of the ES System log following a
**       processor reset. The log may operate in either Overwrite mode = 0, where once
**       the log becomes full the oldest message in the log will be overwritten, or
**       Discard mode = 1, where once the log becomes full the contents of the log are
**       preserved and the new event is discarded.  This constant may hold a value of
**       either 0 or 1 depending on the desired default.
**       Overwrite Mode = 0, Discard Mode = 1.
**
**  \par Limits
**       There is a lower limit of 0 and an upper limit of 1 on this configuration
**       parameter.
*/
#define CFE_PLATFORM_ES_DEFAULT_PR_SYSLOG_MODE 1

/**
**  \cfeescfg Define Max Size of Performance Data Buffer
**
**  \par Description:
**       Defines the maximum size of the performance data buffer. Units are number of
**       performance data entries. An entry is defined by a 32 bit data word followed
**       by a 64 bit time stamp.
**
**  \par Limits
**       There is a lower limit of 1025.  There are no restrictions on the upper limit
**       however, the maximum buffer size is system dependent and should be verified.
**       The units are number of entries. An entry is defined by a 32 bit data word followed
**       by a 64 bit time stamp.
*/
#define CFE_PLATFORM_ES_PERF_DATA_BUFFER_SIZE 10000

/**
**  \cfeescfg Define Filter Mask Setting for Disabling All Performance Entries
**
**  \par Description:
**       Defines the filter mask for disabling all performance entries. The value is a
**       bit mask.  For each bit, 0 means the corresponding entry is disabled and
**       1 means it is enabled.
*/
#define CFE_PLATFORM_ES_PERF_FILTMASK_NONE 0

/**
**  \cfeescfg Define Filter Mask Setting for Enabling All Performance Entries
**
**  \par Description:
**       Defines the filter mask for enabling all performance entries. The value is a
**       bit mask.  For each bit, 0 means the corresponding entry is disabled and
**       1 means it is enabled.
*/
#define CFE_PLATFORM_ES_PERF_FILTMASK_ALL ~CFE_PLATFORM_ES_PERF_FILTMASK_NONE

/**
**  \cfeescfg Define Default Filter Mask Setting for Performance Data Buffer
**
**  \par Description:
**       Defines the default filter mask for the performance data buffer. The value is a
**       bit mask.  For each bit, 0 means the corresponding entry is disabled and 1
**       means it is enabled.
**
*/
#define CFE_PLATFORM_ES_PERF_FILTMASK_INIT CFE_PLATFORM_ES_PERF_FILTMASK_ALL

/**
**  \cfeescfg Define Default Filter Trigger Setting for Disabling All Performance Entries
**
**  \par Description:
**       Defines the default trigger mask for disabling all performance data entries. The value
**       is a bit mask.  For each bit, 0 means the trigger for the corresponding entry is
**       disabled and 1 means it is enabled.
**
*/
#define CFE_PLATFORM_ES_PERF_TRIGMASK_NONE 0

/**
**  \cfeescfg Define Filter Trigger Setting for Enabling All Performance Entries
**
**  \par Description:
**       Defines the trigger mask for enabling all performance data entries. The value is
**       a bit mask.  For each bit, 0 means the trigger for the corresponding entry is
**       disabled and 1 means it is enabled.
**
*/
#define CFE_PLATFORM_ES_PERF_TRIGMASK_ALL ~CFE_PLATFORM_ES_PERF_TRIGMASK_NONE

/**
**  \cfeescfg Define Default Filter Trigger Setting for Performance Data Buffer
**
**  \par Description:
**       Defines the default trigger mask for the performance data buffer. The value is a
**       32-bit mask.  For each bit, 0 means the trigger for the corresponding entry is
**       disabled and 1 means it is enabled.
**
*/
#define CFE_PLATFORM_ES_PERF_TRIGMASK_INIT CFE_PLATFORM_ES_PERF_TRIGMASK_NONE

/**
**  \cfeescfg Define Performance Analyzer Child Task Priority
**
**  \par Description:
**       This parameter defines the priority of the child task spawned by the
**       Executive Services to write performance data to a file.  Lower numbers
**       are higher priority, with 1 being the highest priority in the case of a
**       child task.
**
**  \par Limits
**       Valid range for a child task is 1 to 255 however, the priority cannot
**       be higher (lower number) than the ES parent application priority.
*/
#define CFE_PLATFORM_ES_PERF_CHILD_PRIORITY 200

/**
**  \cfeescfg Define Performance Analyzer Child Task Stack Size
**
**  \par Description:
**       This parameter defines the stack size of the child task spawned by the
**       Executive Services to write performance data to a file.
**
**  \par Limits
**       It is recommended this parameter be greater than or equal to 4KB. This parameter
**       is limited by the maximum value allowed by the data type. In this case, the data
**       type is an unsigned 32-bit integer, so the valid range is 0 to 0xFFFFFFFF.
*/
#define CFE_PLATFORM_ES_PERF_CHILD_STACK_SIZE 4096

/**
**  \cfeescfg Define Performance Analyzer Child Task Delay
**
**  \par Description:
**       This parameter defines the delay time (in milliseconds) between performance
**       data file writes performed by the Executive Services Performance Analyzer
**       Child Task.
**
**  \par Limits
**       It is recommended this parameter be greater than or equal to 20ms. This parameter
**       is limited by the maximum value allowed by the data type. In this case, the data
**       type is an unsigned 32-bit integer, so the valid range is 0 to 0xFFFFFFFF.
*/
#define CFE_PLATFORM_ES_PERF_CHILD_MS_DELAY 20

/**
**  \cfeescfg Define Performance Analyzer Child Task Number of Entries Between Delay
**
**  \par Description:
**       This parameter defines the number of performance analyzer entries the Performance
**       Analyzer Child Task will write to the file between delays.
**
*/
#define CFE_PLATFORM_ES_PERF_ENTRIES_BTWN_DLYS 50

/**
**  \cfeescfg Define Default Stack Size for an Application
**
**  \par Description:
**       This parameter defines a default stack size. This parameter is used by the
**       cFE Core Applications.
**
**  \par Limits
**       There is a lower limit of 2048.  There are no restrictions on the upper limit
**       however, the maximum stack size is system dependent and should be verified.
**       Most operating systems provide tools for measuring the amount of stack used by a
**       task during operation. It is always a good idea to verify that no more than 1/2
**       of the stack is used.
*/
#define CFE_PLATFORM_ES_DEFAULT_STACK_SIZE 8192

/**
**  \cfeescfg Define Maximum Number of Registered CDS Blocks
**
**  \par Description:
**       Maximum number of registered CDS Blocks
**
**  \par Limits
**       There is a lower limit of 8.  There are no restrictions on the upper limit
**       however, the maximum number of CDS entries is system dependent and
**       should be verified.
*/
#define CFE_PLATFORM_ES_CDS_MAX_NUM_ENTRIES 512

/**
**  \cfeescfg Define Number of Processor Resets Before a Power On Reset
**
**  \par Description:
**       Number of Processor Resets before a Power On Reset is called. If set to 2,
**       then 2 processor resets will occur, and the 3rd processor reset will be a
**       power on reset instead.
**
**  \par Limits
**       There is a lower limit of 0.  There are no restrictions on the upper limit
**       however, the maximum number of processor resets may be system dependent and
**       should be verified.
*/
#define CFE_PLATFORM_ES_MAX_PROCESSOR_RESETS 2

/** \cfeescfg Maximum number of block sizes in pool structures
**
**  \par Description:
**      The upper limit for the number of block sizes supported in the generic
**      pool implementation, which in turn implements the memory pools and CDS.
**
**  \par Limits:
**       Must be at least one.  No specific upper limit, but the number is
**       anticipated to be reasonably small (i.e. tens, not hundreds).  Large
**       values have not been tested.
**
**       The ES and CDS block size lists must correlate with this value
*/
#define CFE_PLATFORM_ES_POOL_MAX_BUCKETS 17

/** \cfeescfg Maximum number of memory pools
**
**  \par Description:
**      The upper limit for the number of memory pools that can concurrently
**      exist within the system.
**
**      The CFE_SB and CFE_TBL core subsystems each define a memory pool.
**
**      Individual applications may also create memory pools, so this value
**      should be set sufficiently high enough to support the applications
**      being used on this platform.
**
**  \par Limits:
**       Must be at least 2 to support CFE core - SB and TBL pools.  No
**       specific upper limit.
*/
#define CFE_PLATFORM_ES_MAX_MEMORY_POOLS 10

/**
**  \cfeescfg Define Default ES Memory Pool Block Sizes
**
**  \par Description:
**       Default Intermediate ES Memory Pool Block Sizes.  If an application
**       is using the CFE_ES Memory Pool APIs (#CFE_ES_PoolCreate, #CFE_ES_PoolCreateNoSem,
**       #CFE_ES_GetPoolBuf and #CFE_ES_PutPoolBuf) but finds these sizes
**       inappropriate for their use, they may wish to use the #CFE_ES_PoolCreateEx
**       API to specify their own intermediate block sizes
**
**  \par Limits
**       These sizes MUST be increasing and MUST be an integral multiple of 4.  Also,
**       CFE_PLATFORM_ES_MAX_BLOCK_SIZE must be larger than CFE_MISSION_SB_MAX_SB_MSG_SIZE and both
**       CFE_PLATFORM_TBL_MAX_SNGL_TABLE_SIZE and CFE_PLATFORM_TBL_MAX_DBL_TABLE_SIZE.  Note that if Table
**       Services have been removed from the CFE, the table size limits are still
**       enforced although the table size definitions may be reduced.
*/
#define CFE_PLATFORM_ES_MEM_BLOCK_SIZE_01 8
#define CFE_PLATFORM_ES_MEM_BLOCK_SIZE_02 16
#define CFE_PLATFORM_ES_MEM_BLOCK_SIZE_03 32
#define CFE_PLATFORM_ES_MEM_BLOCK_SIZE_04 48
#define CFE_PLATFORM_ES_MEM_BLOCK_SIZE_05 64
#define CFE_PLATFORM_ES_MEM_BLOCK_SIZE_06 96
#define CFE_PLATFORM_ES_MEM_BLOCK_SIZE_07 128
#define CFE_PLATFORM_ES_MEM_BLOCK_SIZE_08 160
#define CFE_PLATFORM_ES_MEM_BLOCK_SIZE_09 256
#define CFE_PLATFORM_ES_MEM_BLOCK_SIZE_10 512
#define CFE_PLATFORM_ES_MEM_BLOCK_SIZE_11 1024
#define CFE_PLATFORM_ES_MEM_BLOCK_SIZE_12 2048
#define CFE_PLATFORM_ES_MEM_BLOCK_SIZE_13 4096
#define CFE_PLATFORM_ES_MEM_BLOCK_SIZE_14 8192
#define CFE_PLATFORM_ES_MEM_BLOCK_SIZE_15 16384
#define CFE_PLATFORM_ES_MEM_BLOCK_SIZE_16 32768
#define CFE_PLATFORM_ES_MAX_BLOCK_SIZE    80000

/**
**  \cfeescfg Define ES Critical Data Store Memory Pool Block Sizes
**
**  \par Description:
**       Intermediate ES Critical Data Store Memory Pool Block Sizes
**
**  \par Limits
**       These sizes MUST be increasing and MUST be an integral multiple of 4.
*/
#define CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_01 8
#define CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_02 16
#define CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_03 32
#define CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_04 48
#define CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_05 64
#define CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_06 96
#define CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_07 128
#define CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_08 160
#define CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_09 256
#define CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_10 512
#define CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_11 1024
#define CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_12 2048
#define CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_13 4096
#define CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_14 8192
#define CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_15 16384
#define CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_16 32768
#define CFE_PLATFORM_ES_CDS_MAX_BLOCK_SIZE    80000

/** \cfeescfg Poll timer for startup sync delay
**
**  \par Description:
**      During startup, some tasks may need to synchronize their own initialization
**      with the initialization of other applications in the system.
**
**      CFE ES implements an API to accomplish this, that performs a task delay (sleep)
**      while polling the overall system state until other tasks are ready.
**
**      This value controls the amount of time that the CFE_ES_ApplicationSyncDelay
**      will sleep between each check of the system state.  This should be large enough
**      to allow other tasks to run, but not so large as to noticeably delay the startup
**      completion.
**
**      Units are in milliseconds
**
**  \par Limits:
**       Must be defined as an integer value that is greater than
**       or equal to zero.
*/
#define CFE_PLATFORM_ES_STARTUP_SYNC_POLL_MSEC 50

/** \cfeescfg Startup script timeout
**
**  \par Description:
**      The upper limit for the total amount of time that all apps listed in the CFE ES startup
**      script may take to all become ready.
**
**      Unlike the "core" app timeout, this is a soft limit; if the allotted time is exceeded,
**      it probably indicates an issue with one of the apps, but does not cause CFE ES to take
**      any additional action other than logging the event to the syslog.
**
**      Units are in milliseconds
**
**  \par Limits:
**       Must be defined as an integer value that is greater than
**       or equal to zero.
*/
#define CFE_PLATFORM_ES_STARTUP_SCRIPT_TIMEOUT_MSEC 1000

#endif
