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

/*
**  File:
**    cfe_es_api.c
**
**  Purpose:
**    This file implements the cFE Executive Services API functions.
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**     cFE Flight Software Application Developers Guide
**
**  Notes:
**
*/

/*
** Required header files.
*/
#include "cfe_config_priv.h"

#include "cfe_resourceid.h"
#include "cfe_core_resourceid_basevalues.h"

/*
 * Instantiate the global table
 */
CFE_Config_Global_t CFE_Config_Global;

/*----------------------------------------------------------------
 *
 * Application scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Config_ValueEntry_t *CFE_Config_LocateConfigRecordByID(CFE_ConfigId_t ConfigId)
{
    uint32 OffsetVal;

    OffsetVal = CFE_Config_IdToOffset(ConfigId);

    if (OffsetVal >= CFE_ConfigIdOffset_MAX)
    {
        return NULL;
    }

    return &CFE_Config_Global.Table[OffsetVal];
}
