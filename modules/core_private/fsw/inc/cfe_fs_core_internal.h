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
 * Purpose:  cFE File Services (FS) library API header file
 *
 * Author:   S.Walling/Microtel
 *
 */

#ifndef CFE_FS_CORE_INTERNAL_H
#define CFE_FS_CORE_INTERNAL_H

#include "common_types.h"

/*
 * The internal APIs prototyped within this block are only intended to be invoked from
 * other CFE core apps.  They still need to be prototyped in the shared header such that
 * they can be called from other core modules, but applications should not call these.
 */

/** @defgroup CFEAPIFSCoreInternal cFE Internal File Service APIs, internal to CFE core
 * @{
 */

/*****************************************************************************/
/**
** \brief Initializes the cFE core module API Library
**
** \par Description
**      Initialize the FS data structures before the cFE runs.
**
** \par Assumptions, External Events, and Notes:
**        -# This function MUST be called before any module API's are called.
**
******************************************************************************/
int32 CFE_FS_EarlyInit(void);

/*****************************************************************************/
/**
** \brief Execute the background file write job(s)
**
** \par Description
**        Runs the state machine associated with background file write requests
**
** \par Assumptions, External Events, and Notes:
**        This should only be invoked as a background job from the ES background task,
**        it should not be invoked directly.
**
** \param[in] ElapsedTime       The amount of time passed since last invocation (ms)
** \param[in] Arg               Not used/ignored
**
** \return true if jobs are pending, false if idle
**
******************************************************************************/
bool CFE_FS_RunBackgroundFileDump(uint32 ElapsedTime, void *Arg);

/**@}*/

#endif /* CFE_FS_CORE_INTERNAL_H */
