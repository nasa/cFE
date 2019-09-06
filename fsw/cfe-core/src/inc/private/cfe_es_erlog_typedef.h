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

/**
 * \file cfe_es_erlog_typedef.h
 *
 *  Created on: Jan 22, 2015
 *      Author: joseph.p.hickey@nasa.gov
 *
 * Definition of the CFE_ES_ERLog structure type.
 * This was moved into its own header file since it is referenced by multiple CFE core apps.
 */

#ifndef CFE_ES_ERLOG_TYPEDEF_H_
#define CFE_ES_ERLOG_TYPEDEF_H_

#include <common_types.h>
#include <cfe_time.h>   /* Needed for CFE_TIME_SysTime_t */

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
** Exception and Reset Log Structure
*/
typedef struct
{
     uint32                  LogEntryType;                  /* What type of log entry */
     uint32                  ResetType;                     /* Main cause for the reset */
     uint32                  ResetSubtype;                  /* The sub-type for the reset */
     uint32                  BootSource;                    /* The boot source  */
     uint32                  ProcessorResetCount;           /* The number of processor resets */
     uint32                  MaxProcessorResetCount;        /* The maximum number before a Power On */
     CFE_ES_DebugVariables_t DebugVars;                     /* ES Debug variables */
     CFE_TIME_SysTime_t      TimeCode;                      /* Time code */
     char                    Description[80];               /* The ascii data for the event */
     uint32                  ContextSize;                   /* Indicates the context data is valid */
     uint32                  AppID;                         /* The application ID */
     uint32                  Context[CFE_PLATFORM_ES_ER_LOG_MAX_CONTEXT_SIZE / sizeof(uint32)];  /* cpu  context */
} CFE_ES_ERLog_t;



#endif /* CFE_ES_ERLOG_TYPEDEF_H_ */
