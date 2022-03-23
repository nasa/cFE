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
 * Contains global prototypes and definitions related to resource
 * management and related CFE resource IDs.
 *
 * A CFE ES Resource ID is a common way to identify CFE-managed resources such
 * as apps, tasks, counters, memory pools, CDS blocks, and other entities.
 *
 * Simple operations are provided as inline functions, which
 * should alleviate the need to do direct manipulation of resource IDs:
 *
 *  - Check for undefined ID value
 *  - Check for equality of two ID values
 *  - Convert ID to simple integer (typically for printing/logging)
 *  - Convert simple integer to ID (inverse of above)
 */

#ifndef CFE_RESOURCEID_API_TYPEDEFS_H
#define CFE_RESOURCEID_API_TYPEDEFS_H

/*
 * The basic resource ID typedef
 *
 * This is provided via the external resourceid library
 * and may be customized by the user/mission preferences.
 */
#include "cfe_resourceid_typedef.h"

/*
** Defines
*/

/** \name Resource ID predefined values */
/** \{ */

/**
 * @brief A resource ID value that represents an undefined/unused resource
 *
 * This constant may be used to initialize local variables of the
 * CFE_ResourceId_t type to a safe value that will not alias a valid ID.
 *
 * By design, this value is also the result of zeroing a CFE_ResourceId_t
 * type via standard functions like memset(), such that objects initialized
 * using this method will also be set to safe values.
 */
#define CFE_RESOURCEID_UNDEFINED ((CFE_ResourceId_t)CFE_RESOURCEID_WRAP(0))

/**
 * @brief A resource ID value that represents a reserved entry
 *
 * This is not a valid value for any resource type, but is used to mark
 * table entries that are not available for use.  For instance, this may
 * be used while setting up an entry initially.
 */
#define CFE_RESOURCEID_RESERVED ((CFE_ResourceId_t)CFE_RESOURCEID_WRAP(0xFFFFFFFF))

/** \} */

#endif /* CFE_RESOURCEID_API_TYPEDEFS_H */
