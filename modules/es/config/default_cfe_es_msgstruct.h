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
 *  Purpose:
 *  cFE Executive Services (ES) Command and Telemetry packet definition file.
 *
 *  References:
 *     Flight Software Branch C Coding Standard Version 1.0a
 *     cFE Flight Software Application Developers Guide
 *
 *  Notes:
 *
 */
#ifndef CFE_ES_MSGSTRUCT_H
#define CFE_ES_MSGSTRUCT_H

/************************************************************************
 * Includes
 ************************************************************************/
#include "cfe_es_msgdefs.h"
#include "cfe_msg_hdr.h"
#include "cfe_mission_cfg.h"

/********************************/
/* Command Message Data Formats */
/********************************/
/**
** \brief Generic "no arguments" command
**
** This command structure is used for commands that do not have any parameters.
** This includes:
** -# The Housekeeping Request Message
** -# The No-Op Command (For details, see #CFE_ES_NOOP_CC)
** -# The Reset Counters Command (For details, see #CFE_ES_RESET_COUNTERS_CC)
*/
typedef struct CFE_ES_NoArgsCmd
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command header */
} CFE_ES_NoArgsCmd_t;

/*
 * The following commands all share the "NoArgs" format
 *
 * They are each given their own type name matching the command name, which
 * allows them to change independently in the future without changing the prototype
 * of the handler function.
 */
typedef CFE_ES_NoArgsCmd_t CFE_ES_NoopCmd_t;
typedef CFE_ES_NoArgsCmd_t CFE_ES_ResetCountersCmd_t;
typedef CFE_ES_NoArgsCmd_t CFE_ES_ClearSysLogCmd_t;
typedef CFE_ES_NoArgsCmd_t CFE_ES_ClearERLogCmd_t;
typedef CFE_ES_NoArgsCmd_t CFE_ES_ResetPRCountCmd_t;
typedef CFE_ES_NoArgsCmd_t CFE_ES_SendHkCmd_t;

/**
** \brief Restart cFE Command Payload
**
** For command details, see #CFE_ES_RESTART_CC
**
**/
typedef struct CFE_ES_RestartCmd_Payload
{
    uint16 RestartType; /**< \brief #CFE_PSP_RST_TYPE_PROCESSOR=Processor Reset
                             or #CFE_PSP_RST_TYPE_POWERON=Power-On Reset        */
} CFE_ES_RestartCmd_Payload_t;

/**
 * \brief Restart cFE Command
 */
typedef struct CFE_ES_RestartCmd
{
    CFE_MSG_CommandHeader_t     CommandHeader; /**< \brief Command header */
    CFE_ES_RestartCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_ES_RestartCmd_t;

/**
** \brief Generic file name command payload
**
** This format is shared by several executive services commands.
** For command details, see #CFE_ES_QUERY_ALL_CC, #CFE_ES_QUERY_ALL_TASKS_CC,
** #CFE_ES_WRITE_SYSLOG_CC, and #CFE_ES_WRITE_ER_LOG_CC
**
**/
typedef struct CFE_ES_FileNameCmd_Payload
{
    char FileName[CFE_MISSION_MAX_PATH_LEN]; /**< \brief ASCII text string containing full path and
                                                 filename of file in which Application data is to be dumped */
} CFE_ES_FileNameCmd_Payload_t;

/**
 * \brief Generic file name command
 */
typedef struct CFE_ES_FileNameCmd
{
    CFE_MSG_CommandHeader_t      CommandHeader; /**< \brief Command header */
    CFE_ES_FileNameCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_ES_FileNameCmd_t;

/*
 * Unique typedefs for each of the commands that utilize the FileNameCmd
 * packet format
 */
typedef CFE_ES_FileNameCmd_t CFE_ES_QueryAllCmd_t;
typedef CFE_ES_FileNameCmd_t CFE_ES_QueryAllTasksCmd_t;
typedef CFE_ES_FileNameCmd_t CFE_ES_WriteSysLogCmd_t;
typedef CFE_ES_FileNameCmd_t CFE_ES_WriteERLogCmd_t;

/**
** \brief Overwrite/Discard System Log Configuration Command Payload
**
** For command details, see #CFE_ES_OVER_WRITE_SYSLOG_CC
**
**/
typedef struct CFE_ES_OverWriteSysLogCmd_Payload
{
    uint32 Mode; /**< \brief #CFE_ES_LogMode_DISCARD=Throw away most recent messages,
                             #CFE_ES_LogMode_OVERWRITE=Overwrite oldest with most recent */
} CFE_ES_OverWriteSysLogCmd_Payload_t;

/**
 * \brief Overwrite/Discard System Log Configuration Command Payload
 */
typedef struct CFE_ES_OverWriteSysLogCmd
{
    CFE_MSG_CommandHeader_t             CommandHeader; /**< \brief Command header */
    CFE_ES_OverWriteSysLogCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_ES_OverWriteSysLogCmd_t;

/**
** \brief Start Application Command Payload
**
** For command details, see #CFE_ES_START_APP_CC
**
**/
typedef struct CFE_ES_StartAppCmd_Payload
{
    char Application[CFE_MISSION_MAX_API_LEN];   /**< \brief Name of Application to be started */
    char AppEntryPoint[CFE_MISSION_MAX_API_LEN]; /**< \brief Symbolic name of Application's entry point */
    char AppFileName[CFE_MISSION_MAX_PATH_LEN];  /**< \brief Full path and filename of Application's
                                                    executable image */

    CFE_ES_MemOffset_t StackSize; /**< \brief Desired stack size for the new application */

    CFE_ES_ExceptionAction_Enum_t ExceptionAction; /**< \brief #CFE_ES_ExceptionAction_RESTART_APP=On exception,
                                                       restart Application,
                                                       #CFE_ES_ExceptionAction_PROC_RESTART=On exception,
                                                       perform a Processor Reset */
    CFE_ES_TaskPriority_Atom_t Priority;           /**< \brief The new Applications runtime priority. */
} CFE_ES_StartAppCmd_Payload_t;

/**
 * \brief Start Application Command
 */
typedef struct CFE_ES_StartApp
{
    CFE_MSG_CommandHeader_t      CommandHeader; /**< \brief Command header */
    CFE_ES_StartAppCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_ES_StartAppCmd_t;

/**
** \brief Generic application name command payload
**
** For command details, see #CFE_ES_STOP_APP_CC, #CFE_ES_RESTART_APP_CC, #CFE_ES_QUERY_ONE_CC
**
**/
typedef struct CFE_ES_AppNameCmd_Payload
{
    char Application[CFE_MISSION_MAX_API_LEN]; /**< \brief ASCII text string containing Application or Library Name */
} CFE_ES_AppNameCmd_Payload_t;

/**
 * \brief Generic application name command
 */
typedef struct CFE_ES_AppNameCmd
{
    CFE_MSG_CommandHeader_t     CommandHeader; /**< \brief Command header */
    CFE_ES_AppNameCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_ES_AppNameCmd_t;

/*
 * Like NoArgsCmd, this message definition is shared by multiple commands.
 * Create a separate typedef for each one so they can all evolve independently
 * without affecting the prototype.
 */
typedef CFE_ES_AppNameCmd_t CFE_ES_StopAppCmd_t;
typedef CFE_ES_AppNameCmd_t CFE_ES_RestartAppCmd_t;
typedef CFE_ES_AppNameCmd_t CFE_ES_QueryOneCmd_t;

/**
** \brief Reload Application Command Payload
**
** For command details, see #CFE_ES_RELOAD_APP_CC
**
**/
typedef struct CFE_ES_AppReloadCmd_Payload
{
    char Application[CFE_MISSION_MAX_API_LEN];  /**< \brief ASCII text string containing Application Name */
    char AppFileName[CFE_MISSION_MAX_PATH_LEN]; /**< \brief Full path and filename of Application's
                                                   executable image */
} CFE_ES_AppReloadCmd_Payload_t;

/**
 * \brief Reload Application Command
 */
typedef struct CFE_ES_ReloadAppCmd
{
    CFE_MSG_CommandHeader_t       CommandHeader; /**< \brief Command header */
    CFE_ES_AppReloadCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_ES_ReloadAppCmd_t;

/**
** \brief Set Maximum Processor Reset Count Command Payload
**
** For command details, see #CFE_ES_SET_MAX_PR_COUNT_CC
**
**/
typedef struct CFE_ES_SetMaxPRCountCmd_Payload
{
    uint16 MaxPRCount; /**< \brief New maximum number of Processor Resets before
                                   an automatic Power-On Reset is performed */
} CFE_ES_SetMaxPRCountCmd_Payload_t;

/**
 * \brief Set Maximum Processor Reset Count Command
 */
typedef struct CFE_ES_SetMaxPRCountCmd
{
    CFE_MSG_CommandHeader_t           CommandHeader; /**< \brief Command header */
    CFE_ES_SetMaxPRCountCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_ES_SetMaxPRCountCmd_t;

/**
** \brief Delete Critical Data Store Command Payload
**
** For command details, see #CFE_ES_DELETE_CDS_CC
**
**/
typedef struct CFE_ES_DeleteCDSCmd_Payload
{
    char
        CdsName[CFE_MISSION_ES_CDS_MAX_FULL_NAME_LEN]; /**< \brief ASCII text string containing name of CDS to delete */
} CFE_ES_DeleteCDSCmd_Payload_t;

/**
 * \brief Delete Critical Data Store Command
 */
typedef struct CFE_ES_DeleteCDSCmd
{
    CFE_MSG_CommandHeader_t       CommandHeader; /**< \brief Command header */
    CFE_ES_DeleteCDSCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_ES_DeleteCDSCmd_t;

/**
** \brief Start Performance Analyzer Command Payload
**
** For command details, see #CFE_ES_START_PERF_DATA_CC
**
**/
typedef struct CFE_ES_StartPerfCmd_Payload
{
    uint32 TriggerMode; /**< \brief Desired trigger position (Start, Center, End) */
} CFE_ES_StartPerfCmd_Payload_t;

/**
 * \brief Start Performance Analyzer Command
 */
typedef struct CFE_ES_StartPerfDataCmd
{
    CFE_MSG_CommandHeader_t       CommandHeader; /**< \brief Command header */
    CFE_ES_StartPerfCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_ES_StartPerfDataCmd_t;

/**
** \brief Stop Performance Analyzer Command Payload
**
** For command details, see #CFE_ES_STOP_PERF_DATA_CC
**
**/
typedef struct CFE_ES_StopPerfCmd_Payload
{
    char DataFileName[CFE_MISSION_MAX_PATH_LEN]; /**< \brief ASCII text string of full path and filename
                                                    of file Performance Analyzer data is to be written */
} CFE_ES_StopPerfCmd_Payload_t;

/**
 * \brief Stop Performance Analyzer Command
 */
typedef struct CFE_ES_StopPerfDataCmd
{
    CFE_MSG_CommandHeader_t      CommandHeader; /**< \brief Command header */
    CFE_ES_StopPerfCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_ES_StopPerfDataCmd_t;

/**
** \brief Set Performance Analyzer Filter Mask Command Payload
**
** For command details, see #CFE_ES_SET_PERF_FILTER_MASK_CC
**
**/
typedef struct CFE_ES_SetPerfFilterMaskCmd_Payload
{
    uint32 FilterMaskNum; /**< \brief Index into array of Filter Masks */
    uint32 FilterMask;    /**< \brief New Mask for specified entry in array of Filter Masks */
} CFE_ES_SetPerfFilterMaskCmd_Payload_t;

/**
 * \brief Set Performance Analyzer Filter Mask Command
 */
typedef struct CFE_ES_SetPerfFilterMaskCmd
{
    CFE_MSG_CommandHeader_t               CommandHeader; /**< \brief Command header */
    CFE_ES_SetPerfFilterMaskCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_ES_SetPerfFilterMaskCmd_t;

/**
** \brief Set Performance Analyzer Trigger Mask Command Payload
**
** For command details, see #CFE_ES_SET_PERF_TRIGGER_MASK_CC
**
**/
typedef struct CFE_ES_SetPerfTrigMaskCmd_Payload
{
    uint32 TriggerMaskNum; /**< \brief Index into array of Trigger Masks */
    uint32 TriggerMask;    /**< \brief New Mask for specified entry in array of Trigger Masks */
} CFE_ES_SetPerfTrigMaskCmd_Payload_t;

/**
 * \brief Set Performance Analyzer Trigger Mask Command
 */
typedef struct CFE_ES_SetPerfTriggerMaskCmd
{
    CFE_MSG_CommandHeader_t             CommandHeader; /**< \brief Command header */
    CFE_ES_SetPerfTrigMaskCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_ES_SetPerfTriggerMaskCmd_t;

/**
** \brief Send Memory Pool Statistics Command Payload
**
** For command details, see #CFE_ES_SEND_MEM_POOL_STATS_CC
**
**/
typedef struct CFE_ES_SendMemPoolStatsCmd_Payload
{
    char               Application[CFE_MISSION_MAX_API_LEN]; /**< \brief - RESERVED - should be all zeroes */
    CFE_ES_MemHandle_t PoolHandle; /**< \brief Handle of Pool whose statistics are to be telemetered */
} CFE_ES_SendMemPoolStatsCmd_Payload_t;

/**
 * \brief Send Memory Pool Statistics Command
 */
typedef struct CFE_ES_SendMemPoolStatsCmd
{
    CFE_MSG_CommandHeader_t              CommandHeader; /**< \brief Command header */
    CFE_ES_SendMemPoolStatsCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_ES_SendMemPoolStatsCmd_t;

/**
** \brief Dump CDS Registry Command Payload
**
** For command details, see #CFE_ES_DUMP_CDS_REGISTRY_CC
**
**/
typedef struct CFE_ES_DumpCDSRegistryCmd_Payload
{
    char DumpFilename[CFE_MISSION_MAX_PATH_LEN]; /**< \brief ASCII text string of full path and filename
                                                    of file CDS Registry is to be written */
} CFE_ES_DumpCDSRegistryCmd_Payload_t;

/**
 * \brief Dump CDS Registry Command
 */
typedef struct CFE_ES_DumpCDSRegistryCmd
{
    CFE_MSG_CommandHeader_t             CommandHeader; /**< \brief Command header */
    CFE_ES_DumpCDSRegistryCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_ES_DumpCDSRegistryCmd_t;

/*************************************************************************/

/************************************/
/* Telemetry Interface Data Formats */
/************************************/

/**********************************/
/* Telemetry Message Data Formats */
/**********************************/
/**
**  \cfeestlm Single Application Information Packet
**/
typedef struct CFE_ES_OneAppTlm_Payload
{
    CFE_ES_AppInfo_t AppInfo; /**< \brief For more information, see #CFE_ES_AppInfo_t */
} CFE_ES_OneAppTlm_Payload_t;

typedef struct CFE_ES_OneAppTlm
{
    CFE_MSG_TelemetryHeader_t  TelemetryHeader; /**< \brief Telemetry header */
    CFE_ES_OneAppTlm_Payload_t Payload;         /**< \brief Telemetry payload */
} CFE_ES_OneAppTlm_t;

/**
**  \cfeestlm Memory Pool Statistics Packet
**/
typedef struct CFE_ES_PoolStatsTlm_Payload
{
    CFE_ES_MemHandle_t PoolHandle;   /**< \cfetlmmnemonic \ES_POOLHANDLE
                                          \brief Handle of memory pool whose stats are being telemetered */
    CFE_ES_MemPoolStats_t PoolStats; /**< \brief For more info, see #CFE_ES_MemPoolStats_t */
} CFE_ES_PoolStatsTlm_Payload_t;

typedef struct CFE_ES_MemStatsTlm
{
    CFE_MSG_TelemetryHeader_t     TelemetryHeader; /**< \brief Telemetry header */
    CFE_ES_PoolStatsTlm_Payload_t Payload;         /**< \brief Telemetry payload */
} CFE_ES_MemStatsTlm_t;

/*************************************************************************/

/**
**  \cfeestlm Executive Services Housekeeping Packet
**/
typedef struct CFE_ES_HousekeepingTlm_Payload
{
    uint8 CommandCounter;      /**< \cfetlmmnemonic \ES_CMDPC
                                \brief The ES Application Command Counter */
    uint8 CommandErrorCounter; /**< \cfetlmmnemonic \ES_CMDEC
                           \brief The ES Application Command Error Counter */

    uint16 CFECoreChecksum;    /**< \cfetlmmnemonic \ES_CKSUM
                                    \brief Checksum of cFE Core Code */
    uint8 CFEMajorVersion;     /**< \cfetlmmnemonic \ES_CFEMAJORVER
                                    \brief Major Version Number of cFE */
    uint8 CFEMinorVersion;     /**< \cfetlmmnemonic \ES_CFEMINORVER
                                    \brief Minor Version Number of cFE */
    uint8 CFERevision;         /**< \cfetlmmnemonic \ES_CFEREVISION
                                    \brief Sub-Minor Version Number of cFE */
    uint8 CFEMissionRevision;  /**< \cfetlmmnemonic \ES_CFEMISSIONREV
                                    \brief Mission Version Number of cFE */
    uint8 OSALMajorVersion;    /**< \cfetlmmnemonic \ES_OSMAJORVER
                                    \brief OS Abstraction Layer Major Version Number */
    uint8 OSALMinorVersion;    /**< \cfetlmmnemonic \ES_OSMINORVER
                                    \brief OS Abstraction Layer Minor Version Number */
    uint8 OSALRevision;        /**< \cfetlmmnemonic \ES_OSREVISION
                                    \brief OS Abstraction Layer Revision Number */
    uint8 OSALMissionRevision; /**< \cfetlmmnemonic \ES_OSMISSIONREV
                                    \brief OS Abstraction Layer MissionRevision Number */

    uint8 PSPMajorVersion;    /**< \cfetlmmnemonic \ES_PSPMAJORVER
                                   \brief Platform Support Package Major Version Number */
    uint8 PSPMinorVersion;    /**< \cfetlmmnemonic \ES_PSPMINORVER
                                   \brief Platform Support Package Minor Version Number */
    uint8 PSPRevision;        /**< \cfetlmmnemonic \ES_PSPREVISION
                                   \brief Platform Support Package Revision Number */
    uint8 PSPMissionRevision; /**< \cfetlmmnemonic \ES_PSPMISSIONREV
                                   \brief Platform Support Package MissionRevision Number */

    CFE_ES_MemOffset_t SysLogBytesUsed; /**< \cfetlmmnemonic \ES_SYSLOGBYTEUSED
                                             \brief Total number of bytes used in system log */
    CFE_ES_MemOffset_t SysLogSize;      /**< \cfetlmmnemonic \ES_SYSLOGSIZE
                                             \brief Total size of the system log */
    uint32 SysLogEntries;               /**< \cfetlmmnemonic \ES_SYSLOGENTRIES
                                             \brief Number of entries in the system log */
    uint32 SysLogMode;                  /**< \cfetlmmnemonic \ES_SYSLOGMODE
                                             \brief Write/Overwrite Mode */

    uint32 ERLogIndex;   /**< \cfetlmmnemonic \ES_ERLOGINDEX
                              \brief Current index of the ER Log (wraps around) */
    uint32 ERLogEntries; /**< \cfetlmmnemonic \ES_ERLOGENTRIES
                              \brief Number of entries made in the ER Log since the power on */

    uint32 RegisteredCoreApps;     /**< \cfetlmmnemonic \ES_REGCOREAPPS
                                        \brief Number of Applications registered with ES */
    uint32 RegisteredExternalApps; /**< \cfetlmmnemonic \ES_REGEXTAPPS
                                        \brief Number of Applications registered with ES */
    uint32 RegisteredTasks;        /**< \cfetlmmnemonic \ES_REGTASKS
                                        \brief Number of Tasks ( main AND child tasks ) registered with ES */
    uint32 RegisteredLibs;         /**< \cfetlmmnemonic \ES_REGLIBS
                                        \brief Number of Libraries registered with ES */

    uint32 ResetType;          /**< \cfetlmmnemonic \ES_RESETTYPE
                                    \brief Reset type ( PROCESSOR or POWERON ) */
    uint32 ResetSubtype;       /**< \cfetlmmnemonic \ES_RESETSUBTYPE
                                    \brief Reset Sub Type */
    uint32 ProcessorResets;    /**< \cfetlmmnemonic \ES_PROCRESETCNT
                                    \brief Number of processor resets since last power on */
    uint32 MaxProcessorResets; /**< \cfetlmmnemonic \ES_MAXPROCRESETS
                                    \brief Max processor resets before a power on is done */
    uint32 BootSource;         /**< \cfetlmmnemonic \ES_BOOTSOURCE
                                    \brief Boot source ( as provided from BSP ) */

    uint32 PerfState;                                        /**< \cfetlmmnemonic \ES_PERFSTATE
                                                                  \brief Current state of Performance Analyzer */
    uint32 PerfMode;                                         /**< \cfetlmmnemonic \ES_PERFMODE
                                                                  \brief Current mode of Performance Analyzer */
    uint32 PerfTriggerCount;                                 /**< \cfetlmmnemonic \ES_PERFTRIGCNT
                                                                  \brief Number of Times Performance Analyzer has Triggered */
    uint32 PerfFilterMask[CFE_MISSION_ES_PERF_MAX_IDS / 32]; /**< \cfetlmmnemonic \ES_PERFFLTRMASK
                                                          \brief Current Setting of Performance Analyzer Filter Masks */
    uint32
        PerfTriggerMask[CFE_MISSION_ES_PERF_MAX_IDS / 32]; /**< \cfetlmmnemonic \ES_PERFTRIGMASK
                                                        \brief Current Setting of Performance Analyzer Trigger Masks */
    uint32 PerfDataStart;                                  /**< \cfetlmmnemonic \ES_PERFDATASTART
                                                                \brief Identifies First Stored Entry in Performance Analyzer Log */
    uint32 PerfDataEnd;                                    /**< \cfetlmmnemonic \ES_PERFDATAEND
                                                                \brief Identifies Last Stored Entry in Performance Analyzer Log */
    uint32 PerfDataCount;                                  /**< \cfetlmmnemonic \ES_PERFDATACNT
                                                                \brief Number of Entries Put Into the Performance Analyzer Log */
    uint32
        PerfDataToWrite;                 /**< \cfetlmmnemonic \ES_PERFDATA2WRITE
                                              \brief Number of Performance Analyzer Log Entries Left to be Written to Log Dump File */
    CFE_ES_MemOffset_t HeapBytesFree;    /**< \cfetlmmnemonic \ES_HEAPBYTESFREE
                                            \brief Number of free bytes remaining in the OS heap */
    CFE_ES_MemOffset_t HeapBlocksFree;   /**< \cfetlmmnemonic \ES_HEAPBLKSFREE
                                            \brief Number of free blocks remaining in the OS heap */
    CFE_ES_MemOffset_t HeapMaxBlockSize; /**< \cfetlmmnemonic \ES_HEAPMAXBLK
                                            \brief Number of bytes in the largest free block */
} CFE_ES_HousekeepingTlm_Payload_t;

typedef struct CFE_ES_HousekeepingTlm
{
    CFE_MSG_TelemetryHeader_t        TelemetryHeader; /**< \brief Telemetry header */
    CFE_ES_HousekeepingTlm_Payload_t Payload;         /**< \brief Telemetry payload */
} CFE_ES_HousekeepingTlm_t;

#endif
