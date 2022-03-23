/**
 * @file
 *
 * Prototypes for private functions and type definitions for SB
 * routing internal use.
 */

#ifndef CFE_SBR_PRIV_H
#define CFE_SBR_PRIV_H

/*
 * Includes
 */
#include "cfe_sbr.h"

/******************************************************************************
 * Function prototypes
 */

/*---------------------------------------------------------------------------------------*/
/**
 *  \brief Routing map initialization
 */
void CFE_SBR_Init_Map(void);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Associates the given route ID with the given message ID
 *
 * Used for implementations that use a mapping table (typically hash or direct)
 * and need this information to later get the route id from the message id.
 *
 * \note Typically not needed for a search implementation.  Assumes
 *       message ID is valid
 *
 * \param[in] MsgId   Message id to associate with route id
 * \param[in] RouteId Route id to associate with message id
 *
 * \returns Number of collisions
 */
uint32 CFE_SBR_SetRouteId(CFE_SB_MsgId_t MsgId, CFE_SBR_RouteId_t RouteId);

#endif /* CFE_SBR_PRIV_H */
