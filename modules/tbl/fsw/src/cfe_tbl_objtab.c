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

/**
 * File:
 *   cfe_tbl_objtab.c
 *
 * CFE TBL core module object table entry.
 *
 * TBL is a task module with both EarlyInit and Cleanup functions.
 */

#include "target_objtab.h"
#include "cfe_tbl_core_internal.h"
#include "cfe_es_internal_cfg.h"
#include "cfe_tbl_platform_cfg.h"

const Target_ObjectTable_t CFE_TBL_ModuleEntry = { .Name      = "CFE_TBL",
                                                   .EarlyInit = CFE_TBL_EarlyInit,
                                                   .TaskMain  = CFE_TBL_TaskMain,
                                                   .Cleanup   = CFE_TBL_CleanUpApp,
                                                   .Priority  = CFE_PLATFORM_TBL_START_TASK_PRIORITY,
                                                   .StackSize = CFE_PLATFORM_TBL_START_TASK_STACK_SIZE };
