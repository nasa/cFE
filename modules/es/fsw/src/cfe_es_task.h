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

/**
 * @file
 *
 *  Purpose:
 *  cFE Executive Services (ES) task header file
 *
 *  References:
 *     Flight Software Branch C Coding Standard Version 1.0a
 *     cFE Flight Software Application Developers Guide
 *
 *  Notes:
 *
 */

#ifndef CFE_ES_TASK_H
#define CFE_ES_TASK_H

/*
** Includes
*/
#include "cfe_es_msg.h"

#include "cfe_es_api_typedefs.h"
#include "cfe_fs_api_typedefs.h"
#include "cfe_sb_api_typedefs.h"
#include "cfe_es_erlog_typedef.h"

/*************************************************************************/

#define CFE_ES_PIPE_NAME  "ES_CMD_PIPE"
#define CFE_ES_PIPE_DEPTH 12
#define CFE_ES_LIMIT_HK   2
#define CFE_ES_LIMIT_CMD  4

/*
** ES File descriptions
*/
#define CFE_ES_SYS_LOG_DESC  "ES system log data file"
#define CFE_ES_TASK_LOG_DESC "ES Task Info file"
#define CFE_ES_APP_LOG_DESC  "ES Application Info file"
#define CFE_ES_ER_LOG_DESC   "ES ERlog data file"
#define CFE_ES_PERF_LOG_DESC "ES Performance data file"

/*
 * Limit for the total number of entries that may be
 * produced by a "query all" type command.
 */
#define CFE_ES_QUERY_ALL_MAX_ENTRIES (CFE_PLATFORM_ES_MAX_APPLICATIONS + CFE_PLATFORM_ES_MAX_LIBRARIES)

/*************************************************************************/
/*
** Type definitions
*/

/*************************************************************************/

/*
** ES Task function prototypes
*/
void  CFE_ES_TaskMain(void);
int32 CFE_ES_TaskInit(void);
void  CFE_ES_TaskPipe(CFE_SB_Buffer_t *SBBufPtr);

/*
 * Functions related to the ES background helper task for low-priority tasks
 */
int32 CFE_ES_BackgroundInit(void);
void  CFE_ES_BackgroundTask(void);
void  CFE_ES_BackgroundCleanup(void);

/*
** ES Task message dispatch functions
*/
int32 CFE_ES_HousekeepingCmd(const CFE_MSG_CommandHeader_t *data);
int32 CFE_ES_NoopCmd(const CFE_ES_NoopCmd_t *Cmd);
int32 CFE_ES_ResetCountersCmd(const CFE_ES_ResetCountersCmd_t *data);
int32 CFE_ES_RestartCmd(const CFE_ES_RestartCmd_t *data);
int32 CFE_ES_StartAppCmd(const CFE_ES_StartAppCmd_t *data);
int32 CFE_ES_StopAppCmd(const CFE_ES_StopAppCmd_t *data);
int32 CFE_ES_RestartAppCmd(const CFE_ES_RestartAppCmd_t *data);
int32 CFE_ES_ReloadAppCmd(const CFE_ES_ReloadAppCmd_t *data);
int32 CFE_ES_QueryOneCmd(const CFE_ES_QueryOneCmd_t *data);
int32 CFE_ES_QueryAllCmd(const CFE_ES_QueryAllCmd_t *data);
int32 CFE_ES_QueryAllTasksCmd(const CFE_ES_QueryAllTasksCmd_t *data);
int32 CFE_ES_ClearSysLogCmd(const CFE_ES_ClearSysLogCmd_t *data);
int32 CFE_ES_OverWriteSysLogCmd(const CFE_ES_OverWriteSysLogCmd_t *data);
int32 CFE_ES_WriteSysLogCmd(const CFE_ES_WriteSysLogCmd_t *data);
int32 CFE_ES_ClearERLogCmd(const CFE_ES_ClearERLogCmd_t *data);
int32 CFE_ES_WriteERLogCmd(const CFE_ES_WriteERLogCmd_t *data);
int32 CFE_ES_ResetPRCountCmd(const CFE_ES_ResetPRCountCmd_t *data);
int32 CFE_ES_SetMaxPRCountCmd(const CFE_ES_SetMaxPRCountCmd_t *data);
int32 CFE_ES_DeleteCDSCmd(const CFE_ES_DeleteCDSCmd_t *data);
int32 CFE_ES_StartPerfDataCmd(const CFE_ES_StartPerfDataCmd_t *data);
int32 CFE_ES_StopPerfDataCmd(const CFE_ES_StopPerfDataCmd_t *data);
int32 CFE_ES_SetPerfFilterMaskCmd(const CFE_ES_SetPerfFilterMaskCmd_t *data);
int32 CFE_ES_SetPerfTriggerMaskCmd(const CFE_ES_SetPerfTriggerMaskCmd_t *data);
int32 CFE_ES_SendMemPoolStatsCmd(const CFE_ES_SendMemPoolStatsCmd_t *data);
int32 CFE_ES_DumpCDSRegistryCmd(const CFE_ES_DumpCDSRegistryCmd_t *data);

/*
** Message Handler Helper Functions
*/
bool CFE_ES_ValidateHandle(CFE_ES_MemHandle_t Handle);
bool CFE_ES_VerifyCmdLength(CFE_MSG_Message_t *MsgPtr, size_t ExpectedLength);
void CFE_ES_FileWriteByteCntErr(const char *Filename, size_t Requested, int32 Status);

/*************************************************************************/

#endif /* CFE_ES_TASK_H */
