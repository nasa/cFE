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
 *   CFE Executive Services (CFE_ES) Application Message IDs
 */
#ifndef DEFAULT_CFE_ES_MSGIDS_H
#define DEFAULT_CFE_ES_MSGIDS_H

#include "cfe_es_msgid_values.h"

/*
** cFE ES Command Message Id's
*/
#define CFE_ES_CMD_MID     CFE_PLATFORM_ES_CMD_MIDVAL(CMD)
#define CFE_ES_SEND_HK_MID CFE_PLATFORM_ES_CMD_MIDVAL(SEND_HK)

/*
** CFE ES Telemetry Message Id's
*/
#define CFE_ES_HK_TLM_MID       CFE_PLATFORM_ES_TLM_MIDVAL(HK_TLM)
#define CFE_ES_APP_TLM_MID      CFE_PLATFORM_ES_TLM_MIDVAL(APP_TLM)
#define CFE_ES_MEMSTATS_TLM_MID CFE_PLATFORM_ES_TLM_MIDVAL(MEMSTATS_TLM)

#endif
