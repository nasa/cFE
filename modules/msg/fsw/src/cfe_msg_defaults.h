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
