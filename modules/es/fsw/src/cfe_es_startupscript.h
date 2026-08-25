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
 * @file
 *
 *  Purpose:
 *  This file contains the Internal interface for the cFE Application control functions of ES.
 *  These functions and data structures manage the Applications and Child tasks in the cFE.
 *
 *  References:
 *     Flight Software Branch C Coding Standard Version 1.0a
 *     cFE Flight Software Application Developers Guide
 *
 */

#ifndef CFE_ES_STARTUPSCRIPT_H
#define CFE_ES_STARTUPSCRIPT_H

/*
** Include Files
*/
#include "common_types.h"

#include "cfe_es_api_typedefs.h"
#include "cfe_fs_api_typedefs.h"

/*
** Macro Definitions
*/
#define CFE_ES_STARTSCRIPT_MAX_TOKENS_PER_LINE 8
#define CFE_ES_START_BUFF_SIZE                 128

/*****************************************************************************/
/*
** Function prototypes
*/

/*---------------------------------------------------------------------------------------*/
/**
 * This routine loads/starts cFE applications.
 */
void CFE_ES_StartApplications(uint32 ResetType, const char *StartFilePath);

/*---------------------------------------------------------------------------------------*/
/**
 * This function parses the startup file line for an individual cFE application.
 */
int32 CFE_ES_ParseFileEntry(const char **TokenList, uint32 NumTokens);

#endif /* CFE_ES_STARTUPSCRIPT_H */
