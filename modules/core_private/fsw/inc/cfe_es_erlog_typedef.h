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
 * Definition of the CFE_ES_ERLog structure type.
 * This was moved into its own header file since it is referenced by multiple CFE core apps.
 */

#ifndef CFE_ES_ERLOG_TYPEDEF_H
#define CFE_ES_ERLOG_TYPEDEF_H

#include "common_types.h"
#include "cfe_platform_cfg.h"

#include "cfe_time_extern_typedefs.h" /* Needed for CFE_TIME_SysTime_t */
#include "cfe_es_extern_typedefs.h"   /* Needed for CFE_ES_AppId_t */

#define CFE_ES_ERLOG_DESCRIPTION_MAX_LENGTH 80

/*
** Debug variables type
*/
typedef struct
{
    uint32 DebugFlag;
    uint32 WatchdogWriteFlag;
    uint32 PrintfEnabledFlag;
    uint32 LastAppId;
} CFE_ES_DebugVariables_t;

/*
** Exception and Reset Log Base Structure
**
** This is the common data structure that is stored in RAM and log files
*/
typedef struct
{
    uint32                  LogEntryType;                                     /* What type of log entry */
    uint32                  ResetType;                                        /* Main cause for the reset */
    uint32                  ResetSubtype;                                     /* The sub-type for the reset */
    uint32                  BootSource;                                       /* The boot source  */
    uint32                  ProcessorResetCount;                              /* The number of processor resets */
    uint32                  MaxProcessorResetCount;                           /* The maximum number before a Power On */
    CFE_ES_DebugVariables_t DebugVars;                                        /* ES Debug variables */
    CFE_TIME_SysTime_t      TimeCode;                                         /* Time code */
    char                    Description[CFE_ES_ERLOG_DESCRIPTION_MAX_LENGTH]; /* The ascii data for the event */
} CFE_ES_ERLog_BaseInfo_t;

/*
** Exception and Reset Log File Structure
**
** This is the "export" data structure that gets written to a log file
** It is intended to be binary-compatible with the historical definition of this
** structure, to work with existing tools that may read log files.
**
** Note that "AppID" really belongs in the base info, but it is kept here
** for backward compatibility.
*/
typedef struct
{
    CFE_ES_ERLog_BaseInfo_t BaseInfo;                                         /* basic info about the event */
    uint32                  ContextSize;                                      /* Indicates the context data is valid */
    uint32                  AppID;                                            /* The application ID */
    uint8                   Context[CFE_PLATFORM_ES_ER_LOG_MAX_CONTEXT_SIZE]; /* cpu  context */
} CFE_ES_ERLog_FileEntry_t;

/*
** Exception and Reset Log Metadata Structure
** This is stored in ES RAM, not _directly_ written to ER log files.
*/
typedef struct
{
    CFE_ES_ERLog_BaseInfo_t BaseInfo;     /**< Core Log Data */
    CFE_ES_AppId_t          AppID;        /* The application ID */
    uint32                  PspContextId; /**< Reference to context information stored in PSP */
} CFE_ES_ERLog_MetaData_t;

#endif /* CFE_ES_ERLOG_TYPEDEF_H */
