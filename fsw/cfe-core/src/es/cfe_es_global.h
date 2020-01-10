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
**  cfe_es_global.h 
**
**  Purpose:
**  This file contains the ES global data definitions.
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**     cFE Flight Software Application Developers Guide
**
*/

#ifndef _cfe_es_global_
#define _cfe_es_global_

/*
** Includes:
*/
#include "osapi.h"
#include "private/cfe_private.h"
#include "private/cfe_es_resetdata_typedef.h"
#include "cfe_es.h"
#include "cfe_es_apps.h"
#include "cfe_es_cds.h"
#include "cfe_es_perf.h"
#include "cfe_time.h"
#include "cfe_platform_cfg.h"
#include "cfe_evs.h"
#include "cfe_psp.h"


/*
** Defines
*/

/*
** Typedefs
*/

/*
** CFE_ES_GenCounterRecord_t is an internal structure used to keep track of
** Generic Counters that are active in the system.
*/
typedef struct
{
   bool           RecordUsed;                     /* Is the record used(1) or available(0) ? */
   uint32         Counter;
   char           CounterName[OS_MAX_API_NAME];   /* Counter Name */      
} CFE_ES_GenCounterRecord_t;


/*
** Executive Services Global Memory Data
** This is the regular global data that is not preserved on a 
**  processor reset.
*/
typedef struct
{
   /*
   ** Debug Variables
   */
   CFE_ES_DebugVariables_t DebugVars;
   
   /*
   ** Shared Data Semaphore
   */
   uint32 SharedDataMutex;
   
   /*
   ** Startup Sync
   */
   uint32  SystemState;

   /*
   ** ES Task Table
   */ 
   uint32              RegisteredTasks;
   CFE_ES_TaskRecord_t TaskTable[OS_MAX_TASKS];

   /*
   ** ES App Table
   */
   uint32             RegisteredCoreApps;
   uint32             RegisteredExternalApps;
   CFE_ES_AppRecord_t AppTable[CFE_PLATFORM_ES_MAX_APPLICATIONS];
   
   /*
   ** ES Shared Library Table
   */
   uint32             RegisteredLibs;
   CFE_ES_LibRecord_t LibTable[CFE_PLATFORM_ES_MAX_LIBRARIES];

   /*
   ** ES Generic Counters Table
   */
   CFE_ES_GenCounterRecord_t CounterTable[CFE_PLATFORM_ES_MAX_GEN_COUNTERS];

   /*
   ** Critical Data Store Management Variables
   */
   CFE_ES_CDSVariables_t CDSVars;

} CFE_ES_Global_t;

/*
** The Executive Services Global Data declaration
*/
extern CFE_ES_Global_t CFE_ES_Global; 

/*
** The Executive Services Nonvolatile Data declaration
*/
extern CFE_ES_ResetData_t *CFE_ES_ResetDataPtr;


/*
** Functions used to lock/unlock shared data
*/
extern int32 CFE_ES_GetAppIDInternal(uint32 *AppIdPtr);
extern void  CFE_ES_LockSharedData(const char *FunctionName, int32 LineNumber);
extern void  CFE_ES_UnlockSharedData(const char *FunctionName, int32 LineNumber);


#endif
