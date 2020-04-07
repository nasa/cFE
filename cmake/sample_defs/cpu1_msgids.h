/*
**  GSC-18128-1, "Core Flight Executive Version 6.7"
**
**  Copyright (c) 2006-2019 United States Government as represented by
**  the Administrator of the National Aeronautics and Space Administration.
**  All Rights Reserved.
**
**  Licensed under the Apache License, Version 2.0 (the "License");
**  you may not use this file except in compliance with the License.
**  You may obtain a copy of the License at
**
**    http://www.apache.org/licenses/LICENSE-2.0
**
**  Unless required by applicable law or agreed to in writing, software
**  distributed under the License is distributed on an "AS IS" BASIS,
**  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**  See the License for the specific language governing permissions and
**  limitations under the License.
*/

/******************************************************************************
** File: cfe_msgids.h
**
** Purpose:
**   This header file contains the Message Id's for messages used by the
**   cFE core.
**
** Author:   R.McGraw/SSI
**
** Notes:
**   This file should not contain messages defined by cFE external
**   applications.
**
******************************************************************************/
#ifndef _cfe_msgids_
#define _cfe_msgids_

/*
** Includes
*/

#include "cfe_mission_cfg.h"
#include "cfe_sb.h"             /* for use of the CFE_SB_MSGID_LITERAL macro */

/*
 * NOTE: For backward compatibility/transitioning from the integer CFE_SB_MsgId_t type,
 * this defines both a "bare value" (integer) as well as the type safe
 * CFE_SB_MsgId_t literal value.
 *
 * The previous MID symbol now resolves to the type-safe literal value,
 * which for many use cases is transparent (it can be passed to CFE_SB_Subscribe, etc).
 *
 * However it does not work for compile-time/static assignment.  In this case the
 * integer value must still be used.
 *
 * A future version of CFE should eventually phase out the hardcoded/manually-assigned 
 * integer values in favor of a runtime lookup base on some implementation of a command
 * and data dictionary.
 */

/*
** cFE Command Message Id's
*/
#define CFE_EVS_CMD_MID_VALUE            CFE_MISSION_CMD_MID_BASE1 + CFE_MISSION_EVS_CMD_MSG       /* 0x1801 */
                                                       /* Message ID 0x1802 is available  */
#define CFE_SB_CMD_MID_VALUE             CFE_MISSION_CMD_MID_BASE1 + CFE_MISSION_SB_CMD_MSG        /* 0x1803 */
#define CFE_TBL_CMD_MID_VALUE            CFE_MISSION_CMD_MID_BASE1 + CFE_MISSION_TBL_CMD_MSG       /* 0x1804 */
#define CFE_TIME_CMD_MID_VALUE           CFE_MISSION_CMD_MID_BASE1 + CFE_MISSION_TIME_CMD_MSG      /* 0x1805 */
#define CFE_ES_CMD_MID_VALUE             CFE_MISSION_CMD_MID_BASE1 + CFE_MISSION_ES_CMD_MSG        /* 0x1806 */

#define CFE_ES_SEND_HK_MID_VALUE         CFE_MISSION_CMD_MID_BASE1 + CFE_MISSION_ES_SEND_HK_MSG    /* 0x1808 */
#define CFE_EVS_SEND_HK_MID_VALUE        CFE_MISSION_CMD_MID_BASE1 + CFE_MISSION_EVS_SEND_HK_MSG   /* 0x1809 */
                                                       /* Message ID 0x180A is available  */
#define CFE_SB_SEND_HK_MID_VALUE         CFE_MISSION_CMD_MID_BASE1 + CFE_MISSION_SB_SEND_HK_MSG    /* 0x180B */
#define CFE_TBL_SEND_HK_MID_VALUE        CFE_MISSION_CMD_MID_BASE1 + CFE_MISSION_TBL_SEND_HK_MSG   /* 0x180C */
#define CFE_TIME_SEND_HK_MID_VALUE       CFE_MISSION_CMD_MID_BASE1 + CFE_MISSION_TIME_SEND_HK_MSG  /* 0x180D */

#define CFE_TIME_TONE_CMD_MID_VALUE      CFE_MISSION_CMD_MID_BASE1 + CFE_MISSION_TIME_TONE_CMD_MSG /* 0x1810 */
#define CFE_TIME_1HZ_CMD_MID_VALUE       CFE_MISSION_CMD_MID_BASE1 + CFE_MISSION_TIME_1HZ_CMD_MSG  /* 0x1811 */



#define CFE_EVS_CMD_MID         CFE_SB_MSGID_LITERAL(CFE_EVS_CMD_MID_VALUE          )
#define CFE_SB_CMD_MID          CFE_SB_MSGID_LITERAL(CFE_SB_CMD_MID_VALUE           )
#define CFE_TBL_CMD_MID         CFE_SB_MSGID_LITERAL(CFE_TBL_CMD_MID_VALUE          )
#define CFE_TIME_CMD_MID        CFE_SB_MSGID_LITERAL(CFE_TIME_CMD_MID_VALUE         )
#define CFE_ES_CMD_MID          CFE_SB_MSGID_LITERAL(CFE_ES_CMD_MID_VALUE           )
#define CFE_ES_SEND_HK_MID      CFE_SB_MSGID_LITERAL(CFE_ES_SEND_HK_MID_VALUE       )
#define CFE_EVS_SEND_HK_MID     CFE_SB_MSGID_LITERAL(CFE_EVS_SEND_HK_MID_VALUE      )
#define CFE_SB_SEND_HK_MID      CFE_SB_MSGID_LITERAL(CFE_SB_SEND_HK_MID_VALUE       )
#define CFE_TBL_SEND_HK_MID     CFE_SB_MSGID_LITERAL(CFE_TBL_SEND_HK_MID_VALUE      )
#define CFE_TIME_SEND_HK_MID    CFE_SB_MSGID_LITERAL(CFE_TIME_SEND_HK_MID_VALUE     )
#define CFE_TIME_TONE_CMD_MID   CFE_SB_MSGID_LITERAL(CFE_TIME_TONE_CMD_MID_VALUE    )
#define CFE_TIME_1HZ_CMD_MID    CFE_SB_MSGID_LITERAL(CFE_TIME_1HZ_CMD_MID_VALUE     )



/*
** cFE Global Command Message Id's
*/
#define CFE_TIME_DATA_CMD_MID_VALUE   CFE_MISSION_CMD_MID_BASE_GLOB + CFE_MISSION_TIME_DATA_CMD_MSG   /* 0x1860 */
#define CFE_TIME_SEND_CMD_MID_VALUE   CFE_MISSION_CMD_MID_BASE_GLOB + CFE_MISSION_TIME_SEND_CMD_MSG   /* 0x1862 */

#define CFE_TIME_DATA_CMD_MID         CFE_SB_MSGID_LITERAL(CFE_TIME_DATA_CMD_MID_VALUE)
#define CFE_TIME_SEND_CMD_MID         CFE_SB_MSGID_LITERAL(CFE_TIME_SEND_CMD_MID_VALUE)

/*
** CFE Telemetry Message Id's
*/
#define CFE_ES_HK_TLM_MID_VALUE            CFE_MISSION_TLM_MID_BASE1 + CFE_MISSION_ES_HK_TLM_MSG           /* 0x0800 */
#define CFE_EVS_HK_TLM_MID_VALUE           CFE_MISSION_TLM_MID_BASE1 + CFE_MISSION_EVS_HK_TLM_MSG          /* 0x0801 */
                                                       /* Message ID 0x0802 is available  */
#define CFE_SB_HK_TLM_MID_VALUE            CFE_MISSION_TLM_MID_BASE1 + CFE_MISSION_SB_HK_TLM_MSG           /* 0x0803 */
#define CFE_TBL_HK_TLM_MID_VALUE           CFE_MISSION_TLM_MID_BASE1 + CFE_MISSION_TBL_HK_TLM_MSG          /* 0x0804 */
#define CFE_TIME_HK_TLM_MID_VALUE          CFE_MISSION_TLM_MID_BASE1 + CFE_MISSION_TIME_HK_TLM_MSG         /* 0x0805 */
#define CFE_TIME_DIAG_TLM_MID_VALUE        CFE_MISSION_TLM_MID_BASE1 + CFE_MISSION_TIME_DIAG_TLM_MSG       /* 0x0806 */
#define CFE_EVS_LONG_EVENT_MSG_MID_VALUE   CFE_MISSION_TLM_MID_BASE1 + CFE_MISSION_EVS_LONG_EVENT_MSG_MSG  /* 0x0808 */
#define CFE_EVS_SHORT_EVENT_MSG_MID_VALUE  CFE_MISSION_TLM_MID_BASE1 + CFE_MISSION_EVS_SHORT_EVENT_MSG_MSG /* 0x0809 */
#define CFE_SB_STATS_TLM_MID_VALUE         CFE_MISSION_TLM_MID_BASE1 + CFE_MISSION_SB_STATS_TLM_MSG        /* 0x080A */
#define CFE_ES_APP_TLM_MID_VALUE           CFE_MISSION_TLM_MID_BASE1 + CFE_MISSION_ES_APP_TLM_MSG          /* 0x080B */
#define CFE_TBL_REG_TLM_MID_VALUE          CFE_MISSION_TLM_MID_BASE1 + CFE_MISSION_TBL_REG_TLM_MSG         /* 0x080C */
#define CFE_SB_ALLSUBS_TLM_MID_VALUE       CFE_MISSION_TLM_MID_BASE1 + CFE_MISSION_SB_ALLSUBS_TLM_MSG      /* 0x080D */
#define CFE_SB_ONESUB_TLM_MID_VALUE        CFE_MISSION_TLM_MID_BASE1 + CFE_MISSION_SB_ONESUB_TLM_MSG       /* 0x080E */
#define CFE_ES_SHELL_TLM_MID_VALUE         CFE_MISSION_TLM_MID_BASE1 + CFE_MISSION_ES_SHELL_TLM_MSG        /* 0x080F */
#define CFE_ES_MEMSTATS_TLM_MID_VALUE      CFE_MISSION_TLM_MID_BASE1 + CFE_MISSION_ES_MEMSTATS_TLM_MSG     /* 0x0810 */


#define CFE_ES_HK_TLM_MID           CFE_SB_MSGID_LITERAL(CFE_ES_HK_TLM_MID_VALUE           )
#define CFE_EVS_HK_TLM_MID          CFE_SB_MSGID_LITERAL(CFE_EVS_HK_TLM_MID_VALUE          )
#define CFE_SB_HK_TLM_MID           CFE_SB_MSGID_LITERAL(CFE_SB_HK_TLM_MID_VALUE           )
#define CFE_TBL_HK_TLM_MID          CFE_SB_MSGID_LITERAL(CFE_TBL_HK_TLM_MID_VALUE          )
#define CFE_TIME_HK_TLM_MID         CFE_SB_MSGID_LITERAL(CFE_TIME_HK_TLM_MID_VALUE         )
#define CFE_TIME_DIAG_TLM_MID       CFE_SB_MSGID_LITERAL(CFE_TIME_DIAG_TLM_MID_VALUE       )
#define CFE_EVS_LONG_EVENT_MSG_MID  CFE_SB_MSGID_LITERAL(CFE_EVS_LONG_EVENT_MSG_MID_VALUE  )
#define CFE_EVS_SHORT_EVENT_MSG_MID CFE_SB_MSGID_LITERAL(CFE_EVS_SHORT_EVENT_MSG_MID_VALUE )
#define CFE_SB_STATS_TLM_MID        CFE_SB_MSGID_LITERAL(CFE_SB_STATS_TLM_MID_VALUE        )
#define CFE_ES_APP_TLM_MID          CFE_SB_MSGID_LITERAL(CFE_ES_APP_TLM_MID_VALUE          )
#define CFE_TBL_REG_TLM_MID         CFE_SB_MSGID_LITERAL(CFE_TBL_REG_TLM_MID_VALUE         )
#define CFE_SB_ALLSUBS_TLM_MID      CFE_SB_MSGID_LITERAL(CFE_SB_ALLSUBS_TLM_MID_VALUE      )
#define CFE_SB_ONESUB_TLM_MID       CFE_SB_MSGID_LITERAL(CFE_SB_ONESUB_TLM_MID_VALUE       )
#define CFE_ES_SHELL_TLM_MID        CFE_SB_MSGID_LITERAL(CFE_ES_SHELL_TLM_MID_VALUE        )
#define CFE_ES_MEMSTATS_TLM_MID     CFE_SB_MSGID_LITERAL(CFE_ES_MEMSTATS_TLM_MID_VALUE     )


/*
 * MID definitions by these older names are required to make some existing apps compile
 * These are deprecated and should be removed after CFE 6.6
 */
#ifndef CFE_OMIT_DEPRECATED_6_6

#define CFE_EVS_EVENT_MSG_MID        CFE_EVS_LONG_EVENT_MSG_MID

#endif

#endif
