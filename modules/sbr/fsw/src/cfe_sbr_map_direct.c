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
 * Function: CFE_SBR_Init_Map
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
 * Function: CFE_SBR_SetRouteId
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
 * Function: CFE_SBR_GetRouteId
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
