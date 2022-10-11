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
 * Hash routing map implementation
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
#include "cfe_sb.h"

#include <string.h>
#include <limits.h>

/*
 * Macro Definitions
 */

/**
 * \brief Message map size
 *
 * For hash mapping, map size is a multiple of maximum number of routes.
 * The multiple impacts the number of collisions when the routes fill up.
 * 4 was initially chosen to provide for plenty of holes in the map, while
 * still remaining much smaller than the routing table.  Note the
 * multiple must be a factor of 2 to use the efficient shift logic, and
 * can't be bigger than what can be indexed by CFE_SB_MsgId_Atom_t
 */
#define CFE_SBR_MSG_MAP_SIZE (4 * CFE_PLATFORM_SB_MAX_MSG_IDS)

/* Verify power of two */
#if ((CFE_SBR_MSG_MAP_SIZE & (CFE_SBR_MSG_MAP_SIZE - 1)) != 0)
#error CFE_SBR_MSG_MAP_SIZE must be a power of 2 for hash algorithm to work
#endif

/** \brief Hash algorithm magic number
 *
 * Ref:
 * https://stackoverflow.com/questions/664014/what-integer-hash-function-are-good-that-accepts-an-integer-hash-key/12996028#12996028
 */
#define CFE_SBR_HASH_MAGIC (0x45d9f3b)

/******************************************************************************
 * Shared data
 */

/** \brief Message map shared data */
CFE_SBR_RouteId_t CFE_SBR_MSGMAP[CFE_SBR_MSG_MAP_SIZE];

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 * Hashes the message id
 *
 * Note: algorithm designed for a 32 bit int, changing the size of
 * CFE_SB_MsgId_Atom_t may require an update to this implementation
 *
 *-----------------------------------------------------------------*/
CFE_SB_MsgId_Atom_t CFE_SBR_MsgIdHash(CFE_SB_MsgId_t MsgId)
{
    CFE_SB_MsgId_Atom_t hash;

    hash = CFE_SB_MsgIdToValue(MsgId);

    hash = ((hash >> 16) ^ hash) * CFE_SBR_HASH_MAGIC;
    hash = ((hash >> 16) ^ hash) * CFE_SBR_HASH_MAGIC;
    hash = (hash >> 16) ^ hash;

    /* Reduce to fit in map */
    hash &= CFE_SBR_MSG_MAP_SIZE - 1;

    return hash;
}

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
    CFE_SB_MsgId_Atom_t hash;
    uint32              collisions = 0;

    if (CFE_SB_IsValidMsgId(MsgId))
    {
        hash = CFE_SBR_MsgIdHash(MsgId);

        /*
         * Increment from original hash to find the next open slot.
         * Since map is larger than possible routes this will
         * never deadlock
         */
        while (CFE_SBR_IsValidRouteId(CFE_SBR_MSGMAP[hash]))
        {
            /* Increment or loop to start of array */
            hash = (hash + 1) & (CFE_SBR_MSG_MAP_SIZE - 1);
            collisions++;
        }

        CFE_SBR_MSGMAP[hash] = RouteId;
    }

    return collisions;
}

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 *-----------------------------------------------------------------*/
CFE_SBR_RouteId_t CFE_SBR_GetRouteId(CFE_SB_MsgId_t MsgId)
{
    CFE_SB_MsgId_Atom_t hash;
    CFE_SBR_RouteId_t   routeid = CFE_SBR_INVALID_ROUTE_ID;

    if (CFE_SB_IsValidMsgId(MsgId))
    {
        hash    = CFE_SBR_MsgIdHash(MsgId);
        routeid = CFE_SBR_MSGMAP[hash];

        /*
         * Increment from original hash to find matching route.
         * Since map is larger than possible routes this will
         * never deadlock
         */
        while (CFE_SBR_IsValidRouteId(routeid) && !CFE_SB_MsgId_Equal(CFE_SBR_GetMsgId(routeid), MsgId))
        {
            /* Increment or loop to start of array */
            hash    = (hash + 1) & (CFE_SBR_MSG_MAP_SIZE - 1);
            routeid = CFE_SBR_MSGMAP[hash];
        }
    }

    return routeid;
}
