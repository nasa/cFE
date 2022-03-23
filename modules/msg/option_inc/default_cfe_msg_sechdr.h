/**
 * @file
 *
 * Define cFS standard secondary header
 *  - Avoid direct access for portability, use APIs
 *  - Used to construct message structures
 */

#ifndef DEFAULT_CFE_MSG_SECHDR_H
#define DEFAULT_CFE_MSG_SECHDR_H

/*
 * Include Files
 */

#include "common_types.h"
#include "cfe_mission_cfg.h"

/*
 * Defines
 */

/*
 * Type Definitions
 */

/**********************************************************************
 * Structure definitions for secondary headers
 *
 * These are based on historically supported definitions and not
 * an open source standard.
 */

/**
 * \brief cFS command secondary header
 */
typedef struct
{

    uint8 FunctionCode; /**< \brief Command Function Code */
                        /* bits shift ---------description-------- */
                        /* 0x7F  0    Command function code        */
                        /* 0x80  7    Reserved                     */

    uint8 Checksum; /**< \brief Command checksum  (all bits, 0xFF)      */

} CFE_MSG_CommandSecondaryHeader_t;

/**
 * \brief cFS telemetry secondary header
 */
typedef struct
{

    uint8 Time[6]; /**< \brief Time, big endian: 4 byte seconds, 2 byte subseconds */

} CFE_MSG_TelemetrySecondaryHeader_t;

#endif /* DEFAULT_CFE_MSG_SECHDR_H */
