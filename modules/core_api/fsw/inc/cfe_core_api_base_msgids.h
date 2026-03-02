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
 *
 * This header file contains the logic to convert a topic ID to a
 * message ID value.
 *
 * @note
 * Traditionally in CFS the MsgID values were translated using an offset
 * added to a base value that was specific to that CFS instance.  The
 * name of this file reflects that heritage and is preserved for backward
 * compatibility with existing apps.
 *
 * Although this file is called "base_msgids.h" the translation can take
 * on any form, even a runtime lookup.  The file name of
 * "cfe_core_api_msgid_mapping.h" better conveys the intent that this is
 * not limited to a base+offset paradigm.
 *
 * This file is now just just an alias/wrapper around the msgid_mapping header
 * file to allow apps and libraries to continue building.
 *
 * To customize the mapping, override "cfe_core_api_msgid_mapping.h"
 */

#ifndef CFE_CORE_API_BASE_MSGIDS_H
#define CFE_CORE_API_BASE_MSGIDS_H

#include "cfe_core_api_msgid_mapping.h"

#endif /* CFE_CORE_BASE_MSGIDS_H */
