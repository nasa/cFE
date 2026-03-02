/************************************************************************
 * NASA Docket No. GSC-19,200-1, and identified as "cFS Draco"
 *
 * Copyright (c) 2023 United States Government as represented by the
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
 *   CFE Time Services (CFE_TIME) Application Message IDs
 */
#ifndef DEFAULT_CFE_TIME_MSGIDS_H
#define DEFAULT_CFE_TIME_MSGIDS_H

#include "cfe_time_msgid_values.h"

/*
** cFE Command Message Id's
*/
#define CFE_TIME_CMD_MID       CFE_PLATFORM_TIME_CMD_MIDVAL(CMD)
#define CFE_TIME_SEND_HK_MID   CFE_PLATFORM_TIME_CMD_MIDVAL(SEND_HK)
#define CFE_TIME_TONE_CMD_MID  CFE_PLATFORM_TIME_CMD_MIDVAL(TONE_CMD)
#define CFE_TIME_ONEHZ_CMD_MID CFE_PLATFORM_TIME_CMD_MIDVAL(ONEHZ_CMD)

/*
** cFE Global Command Message Id's
*/
#define CFE_TIME_DATA_CMD_MID CFE_PLATFORM_TIME_GLBCMD_MIDVAL(DATA_CMD)
#define CFE_TIME_SEND_CMD_MID CFE_PLATFORM_TIME_GLBCMD_MIDVAL(SEND_CMD)

/*
** CFE Telemetry Message Id's
*/
#define CFE_TIME_HK_TLM_MID   CFE_PLATFORM_TIME_TLM_MIDVAL(HK_TLM)
#define CFE_TIME_DIAG_TLM_MID CFE_PLATFORM_TIME_TLM_MIDVAL(DIAG_TLM)

#ifndef CFE_OMIT_DEPRECATED_6_8
/*
 * This name was changed/deprecated because technically it constitutes an
 * identifier that starts with a digit (1) when the CFE_TIME_ namespace
 * qualifier is removed.  This matters in some circumstances.
 */
#define CFE_TIME_1HZ_CMD_MID CFE_TIME_ONEHZ_CMD_MID
#endif

#endif
