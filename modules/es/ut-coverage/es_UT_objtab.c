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
 *  es_UT_objtab.c
 *
 * Stub definitions for core module entries used in ES unit tests.
 * These replace the real module entries from other CFE modules that
 * are not linked into the ES UT build.
 */

#include "es_UT.h"
#include "es_ut_helpers.h"
#include "target_objtab.h"

extern int32 CFE_EVS_EarlyInit(void);
extern void  CFE_EVS_TaskMain(void);
extern int32 CFE_EVS_CleanUpApp(uint32 AppId);

extern int32 CFE_SB_EarlyInit(void);
extern void  CFE_SB_TaskMain(void);
extern int32 CFE_SB_CleanUpApp(uint32 AppId);

extern int32 CFE_TIME_EarlyInit(void);
extern void  CFE_TIME_TaskMain(void);

extern int32 CFE_TBL_EarlyInit(void);
extern void  CFE_TBL_TaskMain(void);

extern int32 CFE_FS_EarlyInit(void);

extern int32 CFE_Config_Init(void);

const Target_ObjectTable_t CFE_EVS_ModuleEntry = { .Name      = "CFE_EVS",
                                                   .EarlyInit = CFE_EVS_EarlyInit,
                                                   .TaskMain  = CFE_EVS_TaskMain,
                                                   .Cleanup   = CFE_EVS_CleanUpApp,
                                                   .Priority  = 64,
                                                   .StackSize = 8192 };

const Target_ObjectTable_t CFE_SB_ModuleEntry = { .Name      = "CFE_SB",
                                                  .EarlyInit = CFE_SB_EarlyInit,
                                                  .TaskMain  = CFE_SB_TaskMain,
                                                  .Cleanup   = CFE_SB_CleanUpApp,
                                                  .Priority  = 64,
                                                  .StackSize = 8192 };

const Target_ObjectTable_t CFE_TIME_ModuleEntry = { .Name      = "CFE_TIME",
                                                    .EarlyInit = CFE_TIME_EarlyInit,
                                                    .TaskMain  = CFE_TIME_TaskMain,
                                                    .Cleanup   = NULL,
                                                    .Priority  = 64,
                                                    .StackSize = 8192 };

const Target_ObjectTable_t CFE_TBL_ModuleEntry = { .Name      = "CFE_TBL",
                                                   .EarlyInit = CFE_TBL_EarlyInit,
                                                   .TaskMain  = CFE_TBL_TaskMain,
                                                   .Cleanup   = NULL,
                                                   .Priority  = 64,
                                                   .StackSize = 8192 };

const Target_ObjectTable_t CFE_FS_ModuleEntry = { .Name      = "CFE_FS",
                                                  .EarlyInit = CFE_FS_EarlyInit,
                                                  .TaskMain  = NULL,
                                                  .Cleanup   = NULL,
                                                  .Priority  = 0,
                                                  .StackSize = 0 };

const Target_ObjectTable_t CFE_CONFIG_ModuleEntry = { .Name      = "CFE_CONFIG",
                                                      .EarlyInit = CFE_Config_Init,
                                                      .TaskMain  = NULL,
                                                      .Cleanup   = NULL,
                                                      .Priority  = 0,
                                                      .StackSize = 0 };

const Target_ObjectTable_t CFE_MSG_ModuleEntry = { .Name      = "CFE_MSG",
                                                   .EarlyInit = NULL,
                                                   .TaskMain  = NULL,
                                                   .Cleanup   = NULL,
                                                   .Priority  = 0,
                                                   .StackSize = 0 };

const Target_ObjectTable_t CFE_SBR_ModuleEntry = { .Name      = "CFE_SBR",
                                                   .EarlyInit = NULL,
                                                   .TaskMain  = NULL,
                                                   .Cleanup   = NULL,
                                                   .Priority  = 0,
                                                   .StackSize = 0 };

const Target_ObjectTable_t CFE_RESOURCEID_ModuleEntry = { .Name      = "CFE_RESOURCEID",
                                                          .EarlyInit = NULL,
                                                          .TaskMain  = NULL,
                                                          .Cleanup   = NULL,
                                                          .Priority  = 0,
                                                          .StackSize = 0 };
