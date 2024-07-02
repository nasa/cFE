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
 *  cFE core startup module defines, data types and prototypes.
 *
 *  References:
 *     Flight Software Branch C Coding Standard Version 1.0a
 *     cFE Flight Software Application Developers Guide
 *
 *  Notes:
 *
 */

#ifndef CFE_ES_START_H
#define CFE_ES_START_H

/*
** Include Files
*/
#include "cfe_es_api_typedefs.h"

/*
** Macro Definitions
*/

/*
** values of object_type in OS object table
*/
#define CFE_ES_NULL_ENTRY    0x00
#define CFE_ES_CORE_TASK     0x01
#define CFE_ES_DRIVER_TASK   0x02
#define CFE_ES_BIN_SEM       0x03
#define CFE_ES_FUNCTION_CALL 0x04
#define CFE_ES_MUTEX_SEM     0x05

/*
** Type Definitions
*/

typedef int32 (*CFE_ES_EarlyInitFuncPtr_t)(void); /**< \brief Req'd prototype of Early Init Functions */

typedef union
{
    CFE_ES_EarlyInitFuncPtr_t FunctionPtr;
    CFE_ES_TaskEntryFuncPtr_t MainTaskPtr;
    void *                    VoidPtr;
} CFE_ES_FuncPtrUnion_t;

typedef struct
{
    uint32                ObjectType;                  /* The type of object being created */
    char                  ObjectName[OS_MAX_API_NAME]; /* task or OS object name */
    CFE_ES_FuncPtrUnion_t FuncPtrUnion;                /* task or function reference */
    uint32                ObjectPriority;              /* object priority */
    size_t                ObjectSize;                  /* size used for stack, queue size, etc. */
    uint32                ObjectFlags;                 /* extra flags to pass */
} CFE_ES_ObjectTable_t;

/*
** Exported data
*/
extern CFE_ES_ObjectTable_t CFE_ES_ObjectTable[CFE_PLATFORM_ES_OBJECT_TABLE_SIZE]; /* es object table */

/*
 * Name: CFE_ES_CreateObjects
 *
 * Purpose: This function reads the es_object_table and performs all of the
 *          application layer initialization.
 */
void CFE_ES_CreateObjects(void);

/*
 * Name: CFE_ES_SetupResetVariables
 *
 * Purpose: This function initializes the ES reset variables depending on the reset type.
 *          It will also initiate a power on reset when too many processor resets
 *          have happened.
 *
 */
void CFE_ES_SetupResetVariables(uint32 StartType, uint32 StartSubtype, uint32 BootSource);

/*
 * Name: CFE_ES_InitializeFileSystems
 *
 * Purpose: This function initializes the file systems used in the cFE core.
 *
 */
void CFE_ES_InitializeFileSystems(uint32 StartType);

/*
 * Name: CFE_ES_SetupPerfVariables
 *
 * Purpose:This function initializes filter mask,trigger mask, data & state vals
 *
 * Assumptions and Notes: This gets called from CFE_ES_Main() at startup
 * This code must be called before any other task or code that would use
 * CFE_ES_PerfLogEntry() / CFE_ES_PerfLogExit() functions
 */
void CFE_ES_SetupPerfVariables(uint32 ResetType);

#endif /* CFE_ES_START_H */
