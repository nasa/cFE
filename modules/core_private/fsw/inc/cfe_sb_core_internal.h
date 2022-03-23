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
 * Purpose:
 *      This header file contains all definitions for the cFE Software Bus
 *      Application Programmer's Interface.
 *
 * Author:   R.McGraw/SSI
 *
 */

#ifndef CFE_SB_CORE_INTERNAL_H
#define CFE_SB_CORE_INTERNAL_H

#include "common_types.h"
#include "cfe_es_extern_typedefs.h"

/*
 * The internal APIs prototyped within this block are only intended to be invoked from
 * other CFE core apps.  They still need to be prototyped in the shared header such that
 * they can be called from other core modules, but applications should not call these.
 */

/** @defgroup CFEAPISBCoreInternal cFE Internal Software Bus APIs, internal to CFE core
 * @{
 */

/*****************************************************************************/
/**
** \brief Entry Point for cFE Core Application
**
** \par Description
**        This is the entry point to the cFE SB Core Application.
**
** \par Assumptions, External Events, and Notes:
**          None
**
******************************************************************************/
void CFE_SB_TaskMain(void);

/*****************************************************************************/
/**
** \brief Initializes the cFE core module API Library
**
** \par Description
**        Initializes the cFE core module API Library
**
** \par Assumptions, External Events, and Notes:
**        -# This function MUST be called before any module API's are called.
**
******************************************************************************/
int32 CFE_SB_EarlyInit(void);

/*****************************************************************************/
/**
** \brief Removes SB resources associated with specified Application
**
** \par Description
**        This function is called by cFE Executive Services to cleanup after
**        an Application has been terminated.  It frees resources
**        that have been allocated to the specified Application.
**
******************************************************************************/
int32 CFE_SB_CleanUpApp(CFE_ES_AppId_t AppId);

/**@}*/

#endif /* CFE_SB_CORE_INTERNAL_H */
