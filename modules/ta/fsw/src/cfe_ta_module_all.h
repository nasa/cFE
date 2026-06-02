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
**  File:
**    cfe_ta_module_all.h
**
** Encapsulates all TA module internal header files, as well
** as the public API from all other CFE core modules, OSAL, and PSP.
**
** This simplifies the set of include files that need to be put at the
** start of every source file.
*/

#ifndef CFE_TA_MODULE_ALL_H
#define CFE_TA_MODULE_ALL_H

/*
** Includes
*/
#include "cfe.h"
#include "cfe_platform_cfg.h"
#include "cfe_msgids.h"
#include "cfe_perfids.h"

#include "cfe_ta_msgids.h"
#include "cfe_ta_eventids.h"
#include "cfe_ta_msg.h"
#include "cfe_ta_msgstruct.h"
#include "cfe_ta_task.h"
#include "cfe_ta_tasklist.h"
#include "cfe_ta_dispatch.h"
#include "cfe_ta_fcncodes.h"
#include "cfe_ta_mission_cfg.h"

/*
 * Additionally TA needs to use special/extra CDS APIs that are not in the normal API
 */
#include "cfe_es_core_internal.h"

#endif /* CFE_TA_MODULE_ALL_H */
