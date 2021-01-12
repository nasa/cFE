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

/**
 * \file cfe_resourceid.h
 *
 * Contains global prototypes and definitions related to resource
 * management and related CFE resource IDs.
 *
 * A CFE ES Resource ID is a common way to identify CFE-managed resources such
 * as apps, tasks, counters, memory pools, CDS blocks, and other entities.
 */

#ifndef CFE_RESOURCEID_H
#define CFE_RESOURCEID_H

/*
** Include Files
**
** The actual CFE_ES_ResourceID_t type is part of the ES global/extern typedefs,
** as the concept originated in ES and has been extended across all of CFE.
*/
#include "cfe_es_extern_typedefs.h"

/*
** Defines
*/

/** \name Resource ID predefined values */
/** \{ */

/**
 * @brief A resource ID value that represents an undefined/unused resource
 *
 * This constant may be used to initialize local variables of the
 * CFE_ES_ResourceID_t type to a safe value that will not alias a valid ID.
 *
 * By design, this value is also the result of zeroing a CFE_ES_ResourceID_t
 * type via standard functions like memset(), such that objects initialized
 * using this method will also be set to safe values.
 */
#define CFE_ES_RESOURCEID_UNDEFINED     ((CFE_ES_ResourceID_t){0})

/**
 * @brief A resource ID value that represents a reserved entry
 *
 * This is not a valid value for any resource type, but is used to mark
 * table entries that are not available for use.  For instance, this may
 * be used while setting up an entry initially.
 */
#define CFE_ES_RESOURCEID_RESERVED      ((CFE_ES_ResourceID_t){0xFFFFFFFF})

/** \} */


/**
 * @brief Convert a resource ID to an integer.
 *
 * This is primarily intended for logging purposes, such was writing
 * to debug console, event messages, or log files, using printf-like APIs.
 *
 * For compatibility with C library APIs, this returns an "unsigned long"
 * type and should be used with the "%lx" format specifier in a printf
 * format string.
 *
 * @note No assumptions should be made about the actual integer value,
 * such as its base/range.  It may be printed, but should not be modified
 * or tested/compared using other arithmetic ops, and should never be used
 * as the index to an array or table.  See the related function
 * CFE_ES_ResourceID_ToIndex() for cases where a zero-based array/table index
 * is needed.
 *
 * @sa CFE_ES_ResourceID_FromInteger()
 *
 * @param[in]   id    Resource ID to convert
 * @returns Integer value corresponding to ID
 */
static inline unsigned long CFE_ES_ResourceID_ToInteger(CFE_ES_ResourceID_t id)
{
    return ((unsigned long)id);
}

/**
 * @brief Convert an integer to a resource ID.
 *
 * This is the inverse of CFE_ES_ResourceID_ToInteger(), and reconstitutes
 * the original CFE_ES_ResourceID_t value from the integer representation.
 *
 * This may be used, for instance, where an ID value is parsed from a text
 * file or message using C library APIs such as scanf() or strtoul().
 *
 * @sa CFE_ES_ResourceID_ToInteger()
 *
 * @param[in]   Value    Integer value to convert
 * @returns ID value corresponding to integer
 */
static inline CFE_ES_ResourceID_t CFE_ES_ResourceID_FromInteger(unsigned long Value)
{
    return ((CFE_ES_ResourceID_t)Value);
}

/**
 * @brief Compare two Resource ID values for equality
 *
 * @param[in]   id1    Resource ID to check
 * @param[in]   id2    Resource ID to check
 * @returns true if id1 and id2 are equal, false otherwise.
 */
static inline bool CFE_ES_ResourceID_Equal(CFE_ES_ResourceID_t id1, CFE_ES_ResourceID_t id2)
{
    return (id1 == id2);
}

/**
 * @brief Check if a resource ID value is defined
 *
 * The constant #CFE_ES_RESOURCEID_UNDEFINED represents an undefined ID value,
 * such that the expression:
 *
 *      CFE_ES_ResourceID_IsDefined(CFE_ES_RESOURCEID_UNDEFINED)
 *
 * Always returns false.
 *
 * @param[in]   id    Resource ID to check
 * @returns True if the ID may refer to a defined entity, false if invalid/undefined.
 */
static inline bool CFE_ES_ResourceID_IsDefined(CFE_ES_ResourceID_t id)
{
    return (id != 0);
}


#endif  /* CFE_RESOURCEID_H */
