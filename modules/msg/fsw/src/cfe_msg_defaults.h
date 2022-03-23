/**
 * @file
 *
 * Message header defaults, used to initialize messages
 *  - Avoid including outside this module
 */

#ifndef CFE_MSG_DEFAULTS_H
#define CFE_MSG_DEFAULTS_H

/*
 * Includes
 */
#include "cfe_platform_cfg.h"
#include "cfe_mission_cfg.h"
#include "cfe_msg_api_typedefs.h"

/*
 * Defines
 */

/* Backwards compatibility */
#ifndef CFE_PLATFORM_DEFAULT_APID
#define CFE_PLATFORM_DEFAULT_APID 0 /**< \brief Default APID, for bits not in MsgId */
#endif

#ifndef CFE_MISSION_CCSDSVER
#define CFE_MISSION_CCSDSVER 0 /**< \brief Default CCSDS Version */
#endif

#ifndef CFE_PLATFORM_DEFAULT_SUBSYS
#define CFE_PLATFORM_DEFAULT_SUBSYS 0 /**< \brief Default SubSystem, for bits not in MsgId */
#endif

#ifndef CFE_PLATFORM_EDSVER
#define CFE_PLATFORM_EDSVER 1 /**< \brief Default EDS version, cFS historically = 1 */
#endif

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Set CCSDS Primary header defaults
 *
 * \par DESCRIPTION
 *     Only sets the constant defaults.  Internal function assumes
 *     pointer is valid.
 *
 * \param[out]     MsgPtr  Message to set
 */
void CFE_MSG_SetDefaultCCSDSPri(CFE_MSG_Message_t *MsgPtr);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Set CCSDS Extended header defaults
 *
 * \par DESCRIPTION
 *     Only sets the constant defaults. Internal function assumes
 *     pointer is valid.
 *
 * \param[out]     MsgPtr  Message to set
 */
void CFE_MSG_SetDefaultCCSDSExt(CFE_MSG_Message_t *MsgPtr);

#endif /* CFE_MSG_DEFAULTS_H */
