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

/******************************************************************************
 * Direct routing map implementation
 *
 * Notes:
 *   These functions manipulate/access global variables and need
 *   to be protected by the SB Shared data lock.
 *
 */

/*
 * Include Files
 */

#include "common_types.h"
#include "cfe_sbr.h"
#include "cfe_sbr_priv.h"
#include <string.h>

#include "cfe_sb.h"

/*
 * Macro Definitions
 */

/**
 * \brief Message map size
 *
 * For direct mapping, map size is maximum valid MsgId value + 1 (since MsgId 0 is valid)
 */
#define CFE_SBR_MSG_MAP_SIZE (CFE_PLATFORM_SB_HIGHEST_VALID_MSGID + 1)

/******************************************************************************
 * Shared data
 */

/** \brief Message map shared data */
CFE_SBR_RouteId_t CFE_SBR_MSGMAP[CFE_SBR_MSG_MAP_SIZE];

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_SBR_Init_Map(void)
{
    /* Clear the shared data */
    memset(&CFE_SBR_MSGMAP, 0, sizeof(CFE_SBR_MSGMAP));
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
uint32 CFE_SBR_SetRouteId(CFE_SB_MsgId_t MsgId, CFE_SBR_RouteId_t RouteId)
{
    if (CFE_SB_IsValidMsgId(MsgId))
    {
        CFE_SBR_MSGMAP[CFE_SB_MsgIdToValue(MsgId)] = RouteId;
    }

    /* Direct lookup never collides, always return 0 */
    return 0;
}

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 *-----------------------------------------------------------------*/
CFE_SBR_RouteId_t CFE_SBR_GetRouteId(CFE_SB_MsgId_t MsgId)
{
    CFE_SBR_RouteId_t routeid = CFE_SBR_INVALID_ROUTE_ID;

    if (CFE_SB_IsValidMsgId(MsgId))
    {
        routeid = CFE_SBR_MSGMAP[CFE_SB_MsgIdToValue(MsgId)];
    }

    return routeid;
}
