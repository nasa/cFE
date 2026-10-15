/************************************************************************
 * NASA Docket No. GSC-19,200-1, and identified as "cFS Draco"
 *
 * Copyright (c) 2023 United States Government as represented by the
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

#ifndef TARGET_OBJTAB_H
#define TARGET_OBJTAB_H

/**
 * File:
 *  target_objtab.h
 *
 * CFE ES Core Module Object Table type definition.
 *
 * Defines the descriptor structure used by each CFE core module to
 * register its early init, main task, and cleanup entry points.
 * The generated cfe_es_objtab.c collects pointers to these descriptors
 * into a single table that ES iterates at startup and shutdown.
 */

#include "common_types.h"

/**
 * @brief Descriptor for a CFE core module
 *
 * Each CFE core module defines one instance of this struct in its own
 * objtab.c file.  ES iterates the table of pointers to these descriptors
 * at each lifecycle phase:
 *
 *  - EarlyInit    : called before any tasks are started
 *  - TaskMain     : entry point passed to OS_TaskCreate(); NULL for library modules
 *  - AppCleanupCb : Notification function called when state from an external is cleaned up.
 *           If this module has any resources associated with the given app, this routine
 *           should free those resources.  If this module does not track any per-app state,
 *           then this should be set NULL.  The parameter is the CFE ES AppId converted to
 *           a plain unsigned integer value (the CFE_ES_AppId_t typedef does not exist here).
 */
typedef struct
{
    const char *Name;                 /**< Module name, e.g. "CFE_EVS" */
    int32 (*EarlyInit)(void);         /**< Early initialization function; must not be NULL */
    void (*TaskMain)(void);           /**< Task entry point; NULL for library-only modules */
    int32 (*AppCleanupCb)(uint32 Id); /**< Callback function to invoke when external apps are stopped */
    uint32 Priority;                  /**< Task priority; ignored if TaskMain is NULL */
    uint32 StackSize;                 /**< Task stack size in bytes; ignored if TaskMain is NULL */
} Target_ObjectTable_t;

#endif /* CFE_ES_OBJTAB_H */
