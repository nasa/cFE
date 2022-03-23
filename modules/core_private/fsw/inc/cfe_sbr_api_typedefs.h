/**
 * @file
 *
 * Purpose:
 *      Prototypes for private functions and type definitions for SB
 *      routing internal use.
 */

#ifndef CFE_SBR_API_TYPEDEFS_H
#define CFE_SBR_API_TYPEDEFS_H

/*
 * Includes
 */
#include "common_types.h"
#include "cfe_sb_extern_typedefs.h"

/*
 * Macro Definitions
 */

/** \brief Invalid route id */
#define CFE_SBR_INVALID_ROUTE_ID ((CFE_SBR_RouteId_t) {.RouteId = 0})

/******************************************************************************
 * Type Definitions
 */

/**
 * \brief Routing table id
 *
 * This is intended as a form of "strong typedef" where direct assignments should
 * be restricted.  Software bus uses numeric indexes into multiple tables to perform
 * its duties, and it is important that these index values are distinct and separate
 * and not mixed together.
 *
 * Using this holding structure prevents assignment directly into a different index
 * or direct usage as numeric value.
 */
typedef struct
{
    CFE_SB_RouteId_Atom_t RouteId; /**< \brief Holding value, do not use directly in code */
} CFE_SBR_RouteId_t;

/** \brief Callback throttling structure */
typedef struct
{
    uint32 StartIndex; /**< /brief 0 based index to start at */
    uint32 MaxLoop;    /**< /brief Max number to process */
    uint32 NextIndex;  /**< /brief Next start index (output), 0 if completed */
} CFE_SBR_Throttle_t;

/** \brief For each id callback function prototype */
typedef void (*CFE_SBR_CallbackPtr_t)(CFE_SBR_RouteId_t RouteId, void *ArgPtr);

#endif /* CFE_SBR_API_TYPEDEFS_H */
