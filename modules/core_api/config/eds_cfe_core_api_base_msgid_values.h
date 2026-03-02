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
 * This header file contains the platform-specific base msg ID values and
 * logic to convert a topic ID to a message ID value.
 *
 */

#ifndef EDS_CFE_CORE_BASE_MSGID_VALUES_H
#define EDS_CFE_CORE_BASE_MSGID_VALUES_H

/*
 * NOTE: In an EDS build the base midval is intentionally left undefined, all msgids
 * should be determined by looking up the topic in the EDS database at runtime.
 */
#define CFE_CORE_API_BASE_MIDVAL(x)

#endif /* CFE_CORE_BASE_MSGIDS_H */
