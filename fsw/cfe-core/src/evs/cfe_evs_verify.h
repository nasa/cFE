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

/******************************************************************************
** File: cfe_evs_verify.h
**
** Purpose:
**   This header file performs compile time checking for EVS configuration
**   parameters.
**
** Author:   K.Audra(Microtel)
**
** Notes:
**
**
******************************************************************************/

#ifndef _cfe_evs_verify_
#define _cfe_evs_verify_

/* NOTE: Besides the checks in this file, there is one more in cfe_evs_task.h.
 * The check is not here because it is checking a local #define based on a 
 * configuration parameter
 */

#if CFE_PLATFORM_EVS_DEFAULT_TYPE_FLAG > 0x0F 
    #error CFE_PLATFORM_EVS_DEFAULT_TYPE_FLAG cannot be more than 0x0F!
#endif

#if (CFE_PLATFORM_EVS_DEFAULT_LOG_MODE != 0) && (CFE_PLATFORM_EVS_DEFAULT_LOG_MODE != 1)
    #error CFE_PLATFORM_EVS_DEFAULT_LOG_MODE can only be 0 (Overwrite) or 1 (Discard)!
#endif

#if( CFE_PLATFORM_EVS_DEFAULT_MSG_FORMAT_MODE != CFE_EVS_MsgFormat_LONG) && (CFE_PLATFORM_EVS_DEFAULT_MSG_FORMAT_MODE != CFE_EVS_MsgFormat_SHORT)
    #error CFE_EVS_DEFAULT_MSG_FORMAT can only be CFE_EVS_MsgFormat_LONG or CFE_EVS_MsgFormat_SHORT !
#endif

#if CFE_PLATFORM_EVS_PORT_DEFAULT > 0x0F
    #error CFE_PLATFORM_EVS_PORT_DEFAULT cannot be greater than 0x0F!
#endif

/*
** Validate task stack size...
*/
#if CFE_PLATFORM_EVS_START_TASK_STACK_SIZE < 2048
    #error CFE_PLATFORM_EVS_START_TASK_STACK_SIZE must be greater than or equal to 2048
#endif

#endif /* _cfe_evs_verify_ */
/*****************************************************************************/
