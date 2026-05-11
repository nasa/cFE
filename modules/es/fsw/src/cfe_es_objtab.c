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

/*
 * File:
 *  cfe_es_objtab.c
 *
 * CFE ES core module object table entry.
 *
 * ES is a task module. It does not have a separate EarlyInit because
 * the ES startup sequence itself serves that role. ES also does not
 * have a Cleanup function - it is the last module standing during
 * shutdown.
 */

#include "target_objtab.h"
#include "cfe_es_core_internal.h"
#include "cfe_platform_cfg.h"

const Target_ObjectTable_t CFE_ES_ModuleEntry = {
    .Name      = "CFE_ES",
    .EarlyInit = CFE_ES_CDS_EarlyInit, /* ES drives its own startup, this is for CDS*/
    .TaskMain  = CFE_ES_TaskMain,
    .Cleanup   = NULL, /* ES does not clean up - it owns shutdown */
    .Priority  = CFE_PLATFORM_ES_START_TASK_PRIORITY,
    .StackSize = CFE_PLATFORM_ES_START_TASK_STACK_SIZE
};
