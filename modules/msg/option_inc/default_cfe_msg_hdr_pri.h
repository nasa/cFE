/**
 * @file
 *
 * Define cFS standard full header
 *  - Avoid direct access for portability, use APIs
 *  - Used to construct message structures
 */

#ifndef DEFAULT_CFE_MSG_HDR_PRI_H
#define DEFAULT_CFE_MSG_HDR_PRI_H

/*
 * Include Files
 */

#include "common_types.h"
#include "ccsds_hdr.h"
#include "cfe_msg_sechdr.h"
#include "cfe_msg_api_typedefs.h"

/*****************************************************************************/
/**
 * \brief Convert from a CFE_MSG_TelemetryHeader_t or CFE_MSG_CommandHeader_t to a CFE_MSG_Message_t
 *
 * \par Description
 *       Given a pointer to a secondary header structure, return a pointer
 *       to the CFE_MSG_Message_t (base header/primary header) object.
 *
 * \par Notes
 *       Implemented as a macro, so it should work with both Command and Telemetry headers, as well as
 *       both const and non-const headers.  The const-ness of the pointer type should be preserved.
 *
 */
#define CFE_MSG_PTR(shdr) (&((shdr).Msg))

/*
 * Type Definitions
 */

/**********************************************************************
 * Structure definitions for full header
 *
 * These are based on historically supported definitions and not
 * an open source standard.
 */

/**
 * \brief Full CCSDS header
 */
typedef struct
{
    CCSDS_PrimaryHeader_t Pri; /**< \brief CCSDS Primary Header */
} CCSDS_SpacePacket_t;

/**
 * \brief cFS generic base message
 *
 * This provides the definition of CFE_MSG_Message_t
 */
union CFE_MSG_Message
{
    CCSDS_SpacePacket_t CCSDS;                             /**< \brief CCSDS Header (Pri or Pri + Ext) */
    uint8               Byte[sizeof(CCSDS_SpacePacket_t)]; /**< \brief Byte level access */
};

/**
 * \brief cFS command header
 *
 * This provides the definition of CFE_MSG_CommandHeader_t
 */
struct CFE_MSG_CommandHeader
{
    CFE_MSG_Message_t                Msg; /**< \brief Base message */
    CFE_MSG_CommandSecondaryHeader_t Sec; /**< \brief Secondary header */
};

/**
 * \brief cFS telemetry header
 *
 * This provides the definition of CFE_MSG_TelemetryHeader_t
 */
struct CFE_MSG_TelemetryHeader
{
    CFE_MSG_Message_t                  Msg;      /**< \brief Base message */
    CFE_MSG_TelemetrySecondaryHeader_t Sec;      /**< \brief Secondary header */
    uint8                              Spare[4]; /**< \brief Pad to avoid compiler padding if payload
                                                             requires 64 bit alignment */
};

#endif /* DEFAULT_CFE_MSG_HDR_PRI_H */
