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
 * Purpose:  cFE Time Services (TIME) configuration verification
 *
 * Author:   S.Walling/Microtel
 *
 * Notes:
 *
 */

#ifndef CFE_TIME_VERIFY_H
#define CFE_TIME_VERIFY_H

#include "cfe_mission_cfg.h"
#include "cfe_platform_cfg.h"

/*************************************************************************/

/*
** Validate default time client/server selection...
*/
#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
#if (CFE_PLATFORM_TIME_CFG_CLIENT == true)
#error Cannot define both CFE_PLATFORM_TIME_CFG_SERVER and CFE_PLATFORM_TIME_CFG_CLIENT as true!
#endif
#else
#if (CFE_PLATFORM_TIME_CFG_CLIENT != true)
#error Must define either CFE_PLATFORM_TIME_CFG_SERVER or CFE_PLATFORM_TIME_CFG_CLIENT as true!
#endif
#endif

/*
** Validate default time format selection...
*/
#if (CFE_MISSION_TIME_CFG_DEFAULT_TAI == true)
#if (CFE_MISSION_TIME_CFG_DEFAULT_UTC == true)
#error Cannot define both CFE_MISSION_TIME_CFG_DEFAULT_UTC and CFE_MISSION_TIME_CFG_DEFAULT_TAI as true!
#endif
#else
#if (CFE_MISSION_TIME_CFG_DEFAULT_UTC != true)
#error Must define either CFE_MISSION_TIME_CFG_DEFAULT_UTC or CFE_MISSION_TIME_CFG_DEFAULT_TAI as true!
#endif
#endif

/*
** Validate time source selection...
*/
#if (CFE_PLATFORM_TIME_CFG_CLIENT == true)
#if (CFE_PLATFORM_TIME_CFG_SOURCE == true)
#error Cannot define both CFE_PLATFORM_TIME_CFG_CLIENT and CFE_PLATFORM_TIME_CFG_SOURCE as true!
#endif
#endif

#if (CFE_PLATFORM_TIME_CFG_SOURCE == true)
#if (CFE_PLATFORM_TIME_CFG_VIRTUAL != true)
#error Cannot define CFE_PLATFORM_TIME_CFG_SOURCE as true without defining CFE_PLATFORM_TIME_CFG_VIRTUAL as true!
#endif
#endif

/*
** Validate local MET selections...
*/
#if (CFE_PLATFORM_TIME_CFG_CLIENT == true)
#if (CFE_PLATFORM_TIME_CFG_VIRTUAL != true)
#error Cannot define CFE_PLATFORM_TIME_CFG_CLIENT as true without defining CFE_PLATFORM_TIME_CFG_VIRTUAL as true!
#endif
#endif

/*
** Validate time source type selection...
*/
#if (CFE_PLATFORM_TIME_CFG_SRC_MET == true)
#if (CFE_PLATFORM_TIME_CFG_SOURCE != true)
#error Cannot define CFE_PLATFORM_TIME_CFG_SRC_MET as true without defining CFE_PLATFORM_TIME_CFG_SOURCE as true!
#endif
#if (CFE_PLATFORM_TIME_CFG_SRC_GPS == true)
#error Cannot define both CFE_PLATFORM_TIME_CFG_SRC_MET and CFE_PLATFORM_TIME_CFG_SRC_GPS as true!
#endif
#if (CFE_PLATFORM_TIME_CFG_SRC_TIME == true)
#error Cannot define both CFE_PLATFORM_TIME_CFG_SRC_MET and CFE_PLATFORM_TIME_CFG_SRC_TIME as true!
#endif
#endif

#if (CFE_PLATFORM_TIME_CFG_SRC_GPS == true)
#if (CFE_PLATFORM_TIME_CFG_SOURCE != true)
#error Cannot define CFE_PLATFORM_TIME_CFG_SRC_GPS as true without defining CFE_PLATFORM_TIME_CFG_SOURCE as true!
#endif
#if (CFE_PLATFORM_TIME_CFG_SRC_TIME == true)
#error Cannot define both CFE_PLATFORM_TIME_CFG_SRC_GPS and CFE_PLATFORM_TIME_CFG_SRC_TIME as true!
#endif
#endif

#if (CFE_PLATFORM_TIME_CFG_SRC_TIME == true)
#if (CFE_PLATFORM_TIME_CFG_SOURCE != true)
#error Cannot define CFE_PLATFORM_TIME_CFG_SRC_TIME as true without defining CFE_PLATFORM_TIME_CFG_SOURCE as true!
#endif
#endif

/*
** Validate tone signal and data packet arrival selection...
*/
#if (CFE_MISSION_TIME_AT_TONE_WAS == true)
#if (CFE_MISSION_TIME_AT_TONE_WILL_BE == true)
#error Both CFE_MISSION_TIME_AT_TONE_WAS and CFE_MISSION_TIME_AT_TONE_WILL_BE have been defined as true!
#endif
#else
#if (CFE_MISSION_TIME_AT_TONE_WILL_BE != true)
#error Either CFE_MISSION_TIME_AT_TONE_WAS or CFE_MISSION_TIME_AT_TONE_WILL_BE must be defined as true!
#endif
#endif

/*
** Validate simulated tone signal and external time source selection...
*/
#if (CFE_MISSION_TIME_CFG_FAKE_TONE == true)
#if (CFE_PLATFORM_TIME_CFG_SOURCE == true)
#error Cannot define both CFE_MISSION_TIME_CFG_FAKE_TONE and CFE_PLATFORM_TIME_CFG_SOURCE as true!
#endif
#endif

/*
** Validate simulated tone signal and data packet arrival selection...
*/
#if (CFE_MISSION_TIME_CFG_FAKE_TONE == true)
#if (CFE_MISSION_TIME_AT_TONE_WILL_BE == true)
#error Cannot define both CFE_MISSION_TIME_CFG_FAKE_TONE and CFE_MISSION_TIME_AT_TONE_WILL_BE as true!
#endif
#endif

/*
** Validate task priorities...
*/
#if CFE_PLATFORM_TIME_START_TASK_PRIORITY < 0
#error CFE_PLATFORM_TIME_START_TASK_PRIORITY must be greater than or equal to zero
#elif CFE_PLATFORM_TIME_START_TASK_PRIORITY > 255
#error CFE_PLATFORM_TIME_START_TASK_PRIORITY must be less than or equal to 255
#endif
#if CFE_PLATFORM_TIME_TONE_TASK_PRIORITY < 0
#error CFE_PLATFORM_TIME_TONE_TASK_PRIORITY must be greater than or equal to zero
#elif CFE_PLATFORM_TIME_TONE_TASK_PRIORITY > 255
#error CFE_PLATFORM_TIME_TONE_TASK_PRIORITY must be less than or equal to 255
#endif
#if CFE_PLATFORM_TIME_ONEHZ_TASK_PRIORITY < 0
#error CFE_PLATFORM_TIME_ONEHZ_TASK_PRIORITY must be greater than or equal to zero
#elif CFE_PLATFORM_TIME_ONEHZ_TASK_PRIORITY > 255
#error CFE_PLATFORM_TIME_ONEHZ_TASK_PRIORITY must be less than or equal to 255
#endif

/*
** Validate task stack sizes...
*/
#if CFE_PLATFORM_TIME_START_TASK_STACK_SIZE < 2048
#error CFE_PLATFORM_TIME_START_TASK_STACK_SIZE must be greater than or equal to 2048
#endif

#if CFE_PLATFORM_TIME_TONE_TASK_STACK_SIZE < 2048
#error CFE_PLATFORM_TIME_TONE_TASK_STACK_SIZE must be greater than or equal to 2048
#endif

#if CFE_PLATFORM_TIME_ONEHZ_TASK_STACK_SIZE < 2048
#error CFE_PLATFORM_TIME_ONEHZ_TASK_STACK_SIZE must be greater than or equal to 2048
#endif

/*************************************************************************/

#endif /* CFE_TIME_VERIFY_H */
