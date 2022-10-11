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
 * Purpose:
 *   Unsorted routing implementation
 *   Used with route map implementations where order of routes doesn't matter
 *
 * Notes:
 *   These functions manipulate/access global variables and need
 *   to be protected by the SB Shared data lock.
 */

/*
 * Include Files
 */

#include "common_types.h"
#include "cfe_sbr.h"
#include "cfe_sbr_priv.h"
#include <string.h>

#include "cfe_sb.h"
#include "cfe_msg.h"

/******************************************************************************
 * Type Definitions
 */

/** \brief Routing table entry */
typedef struct
{
    CFE_SB_DestinationD_t * ListHeadPtr; /**< \brief Destination list head */
    CFE_SB_MsgId_t          MsgId;       /**< \brief Message ID associated with route */
    CFE_MSG_SequenceCount_t SeqCnt;      /**< \brief Message sequence counter */
} CFE_SBR_RouteEntry_t;

/** \brief Module data */
typedef struct
{
    CFE_SBR_RouteEntry_t  RoutingTbl[CFE_PLATFORM_SB_MAX_MSG_IDS]; /**< \brief Routing table */
    CFE_SB_RouteId_Atom_t RouteIdxTop;                             /**< \brief First unused entry in RoutingTbl */
} cfe_sbr_route_data_t;

/******************************************************************************
 * Shared data
 */

/** \brief Routing module shared data */
cfe_sbr_route_data_t CFE_SBR_RDATA;

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 *-----------------------------------------------------------------*/
void CFE_SBR_Init(void)
{
    CFE_SB_RouteId_Atom_t routeidx;

    /* Clear the shared data */
    memset(&CFE_SBR_RDATA, 0, sizeof(CFE_SBR_RDATA));

    /* Only non-zero value for shared data initialization is the invalid MsgId */
    for (routeidx = 0; routeidx < CFE_PLATFORM_SB_MAX_MSG_IDS; routeidx++)
    {
        CFE_SBR_RDATA.RoutingTbl[routeidx].MsgId = CFE_SB_INVALID_MSG_ID;
    }

    /* Initialize map */
    CFE_SBR_Init_Map();
}

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 *-----------------------------------------------------------------*/
CFE_SBR_RouteId_t CFE_SBR_AddRoute(CFE_SB_MsgId_t MsgId, uint32 *CollisionsPtr)
{
    CFE_SBR_RouteId_t routeid    = CFE_SBR_INVALID_ROUTE_ID;
    uint32            collisions = 0;

    if (CFE_SB_IsValidMsgId(MsgId) && (CFE_SBR_RDATA.RouteIdxTop < CFE_PLATFORM_SB_MAX_MSG_IDS))
    {
        routeid    = CFE_SBR_ValueToRouteId(CFE_SBR_RDATA.RouteIdxTop);
        collisions = CFE_SBR_SetRouteId(MsgId, routeid);

        CFE_SBR_RDATA.RoutingTbl[CFE_SBR_RDATA.RouteIdxTop].MsgId = MsgId;
        CFE_SBR_RDATA.RouteIdxTop++;
    }

    if (CollisionsPtr != NULL)
    {
        *CollisionsPtr = collisions;
    }

    return routeid;
}

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 *-----------------------------------------------------------------*/
CFE_SB_MsgId_t CFE_SBR_GetMsgId(CFE_SBR_RouteId_t RouteId)
{
    CFE_SB_MsgId_t msgid = CFE_SB_INVALID_MSG_ID;

    if (CFE_SBR_IsValidRouteId(RouteId))
    {
        msgid = CFE_SBR_RDATA.RoutingTbl[CFE_SBR_RouteIdToValue(RouteId)].MsgId;
    }

    return msgid;
}

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 *-----------------------------------------------------------------*/
CFE_SB_DestinationD_t *CFE_SBR_GetDestListHeadPtr(CFE_SBR_RouteId_t RouteId)
{
    CFE_SB_DestinationD_t *destptr = NULL;

    if (CFE_SBR_IsValidRouteId(RouteId))
    {
        destptr = CFE_SBR_RDATA.RoutingTbl[CFE_SBR_RouteIdToValue(RouteId)].ListHeadPtr;
    }

    return destptr;
}

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 *-----------------------------------------------------------------*/
void CFE_SBR_SetDestListHeadPtr(CFE_SBR_RouteId_t RouteId, CFE_SB_DestinationD_t *DestPtr)
{
    if (CFE_SBR_IsValidRouteId(RouteId))
    {
        CFE_SBR_RDATA.RoutingTbl[CFE_SBR_RouteIdToValue(RouteId)].ListHeadPtr = DestPtr;
    }
}

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 *-----------------------------------------------------------------*/
void CFE_SBR_IncrementSequenceCounter(CFE_SBR_RouteId_t RouteId)
{
    CFE_MSG_SequenceCount_t *cnt;

    if (CFE_SBR_IsValidRouteId(RouteId))
    {
        cnt  = &CFE_SBR_RDATA.RoutingTbl[CFE_SBR_RouteIdToValue(RouteId)].SeqCnt;
        *cnt = CFE_MSG_GetNextSequenceCount(*cnt);
    }
}

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 *-----------------------------------------------------------------*/
CFE_MSG_SequenceCount_t CFE_SBR_GetSequenceCounter(CFE_SBR_RouteId_t RouteId)
{
    CFE_MSG_SequenceCount_t seqcnt = 0;

    if (CFE_SBR_IsValidRouteId(RouteId))
    {
        seqcnt = CFE_SBR_RDATA.RoutingTbl[CFE_SBR_RouteIdToValue(RouteId)].SeqCnt;
    }

    return seqcnt;
}

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 *-----------------------------------------------------------------*/
void CFE_SBR_ForEachRouteId(CFE_SBR_CallbackPtr_t CallbackPtr, void *ArgPtr, CFE_SBR_Throttle_t *ThrottlePtr)
{
    CFE_SB_RouteId_Atom_t routeidx;
    CFE_SB_RouteId_Atom_t startidx = 0;
    CFE_SB_RouteId_Atom_t endidx   = CFE_SBR_RDATA.RouteIdxTop;

    /* Update throttle settings if needed */
    if (ThrottlePtr != NULL)
    {
        startidx = ThrottlePtr->StartIndex;

        /* Return next index of zero if full range is processed */
        ThrottlePtr->NextIndex = 0;

        if ((startidx + ThrottlePtr->MaxLoop) < endidx)
        {
            endidx                 = startidx + ThrottlePtr->MaxLoop;
            ThrottlePtr->NextIndex = endidx;
        }
    }

    for (routeidx = startidx; routeidx < endidx; routeidx++)
    {
        (*CallbackPtr)(CFE_SBR_ValueToRouteId(routeidx), ArgPtr);
    }
}
