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
**   File:
**    cfe_es_objtab.c
**
**   Purpose:
**     This file contains the OS_object_table for MAP Build1.
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**     cFE Flight Software Application Developers Guide
**
**  Notes:
**
*/

/*
** Include files
*/
#include "private/cfe_private.h"
#include "cfe_es_global.h"
#include "cfe_es_start.h"

/*
**
** ES_object_table
** Note: The name field in this table should be no more than OS_MAX_API_NAME -1 characters.
**
*/
CFE_ES_ObjectTable_t  CFE_ES_ObjectTable[CFE_PLATFORM_ES_OBJECT_TABLE_SIZE] =
{
   /*
   ** Spare entries -- The spares should be distributed evenly through this table
   */
   {
           .ObjectType = CFE_ES_NULL_ENTRY
   },
   {
           .ObjectType = CFE_ES_NULL_ENTRY
   },
   {
           .ObjectType = CFE_ES_NULL_ENTRY
   },
   {
           .ObjectType = CFE_ES_NULL_ENTRY
   },
   
   /*
   ** cFE core early initialization calls. These must be done before the tasks start
   */
   {
           .ObjectType = CFE_ES_FUNCTION_CALL,
           .ObjectName = "CFE_ES_CDSEarlyInit",
           .FuncPtrUnion.FunctionPtr = CFE_ES_CDS_EarlyInit
   },
   {
           .ObjectType = CFE_ES_NULL_ENTRY
   },
   {
           .ObjectType = CFE_ES_FUNCTION_CALL,
           .ObjectName = "CFE_EVS_EarlyInit",
           .FuncPtrUnion.FunctionPtr = CFE_EVS_EarlyInit
   },
   {
           .ObjectType = CFE_ES_NULL_ENTRY
   },
   {
           .ObjectType = CFE_ES_FUNCTION_CALL,
           .ObjectName = "CFE_SB_EarlyInit",
           .FuncPtrUnion.FunctionPtr = CFE_SB_EarlyInit
   },
   {
           .ObjectType = CFE_ES_NULL_ENTRY
   },
   {
           .ObjectType = CFE_ES_FUNCTION_CALL,
           .ObjectName = "CFE_TIME_EarlyInit",
           .FuncPtrUnion.FunctionPtr = CFE_TIME_EarlyInit
   },
   {
           .ObjectType = CFE_ES_NULL_ENTRY },
#ifndef EXCLUDE_CFE_TBL
   {
           .ObjectType = CFE_ES_FUNCTION_CALL,
           .ObjectName = "CFE_TBL_EarlyInit",
           .FuncPtrUnion.FunctionPtr = CFE_TBL_EarlyInit
   },

#else
   {
           .ObjectType = CFE_ES_NULL_ENTRY
   },
#endif
   {
           .ObjectType = CFE_ES_NULL_ENTRY
   },
   {
           .ObjectType = CFE_ES_FUNCTION_CALL,
           .ObjectName = "CFE_FS_EarlyInit",
           .FuncPtrUnion.FunctionPtr = CFE_FS_EarlyInit
   },


   /*
   ** Spare entries
   */
   {
           .ObjectType = CFE_ES_NULL_ENTRY
   },
   {
           .ObjectType = CFE_ES_NULL_ENTRY
   },

   /*
   ** cFE core tasks
   */
   {
           .ObjectType = CFE_ES_CORE_TASK,
           .ObjectName = "CFE_EVS",
           .FuncPtrUnion.MainAppPtr = CFE_EVS_TaskMain,
           .ObjectPriority = CFE_PLATFORM_EVS_START_TASK_PRIORITY,
           .ObjectSize = CFE_PLATFORM_EVS_START_TASK_STACK_SIZE
   },
   {
           .ObjectType = CFE_ES_NULL_ENTRY
   },
   {
           .ObjectType = CFE_ES_CORE_TASK,
           .ObjectName = "CFE_SB",
           .FuncPtrUnion.MainAppPtr = CFE_SB_TaskMain,
           .ObjectPriority = CFE_PLATFORM_SB_START_TASK_PRIORITY,
           .ObjectSize = CFE_PLATFORM_SB_START_TASK_STACK_SIZE
   },
   {
           .ObjectType = CFE_ES_NULL_ENTRY
   },
   {
           .ObjectType = CFE_ES_CORE_TASK,
           .ObjectName = "CFE_ES",
           .FuncPtrUnion.MainAppPtr = CFE_ES_TaskMain,
           .ObjectPriority = CFE_PLATFORM_ES_START_TASK_PRIORITY,
           .ObjectSize = CFE_PLATFORM_ES_START_TASK_STACK_SIZE
   },
   {
           .ObjectType = CFE_ES_NULL_ENTRY
   },
   {
           .ObjectType = CFE_ES_CORE_TASK,
           .ObjectName = "CFE_TIME",
           .FuncPtrUnion.MainAppPtr = CFE_TIME_TaskMain,
           .ObjectPriority = CFE_PLATFORM_TIME_START_TASK_PRIORITY,
           .ObjectSize = CFE_PLATFORM_TIME_START_TASK_STACK_SIZE
   },
   {
           .ObjectType = CFE_ES_NULL_ENTRY
   },
#ifndef EXCLUDE_CFE_TBL
   {
           .ObjectType = CFE_ES_CORE_TASK,
           .ObjectName = "CFE_TBL",
           .FuncPtrUnion.MainAppPtr = CFE_TBL_TaskMain,
           .ObjectPriority = CFE_PLATFORM_TBL_START_TASK_PRIORITY,
           .ObjectSize = CFE_PLATFORM_TBL_START_TASK_STACK_SIZE
   },
#else
   {
           .ObjectType = CFE_ES_NULL_ENTRY
   },
#endif

   /*
   ** Spare entries
   */
   {
           .ObjectType = CFE_ES_NULL_ENTRY
   },
   {
           .ObjectType = CFE_ES_NULL_ENTRY
   },
   {
           .ObjectType = CFE_ES_NULL_ENTRY
   },
   {
           .ObjectType = CFE_ES_NULL_ENTRY
   }

};
