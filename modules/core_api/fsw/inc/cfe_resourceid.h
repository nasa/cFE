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

#ifndef CFE_RESOURCEID_H
#define CFE_RESOURCEID_H

/*
 * The basic resource ID API definitions
 */
#include "cfe_resourceid_api_typedefs.h"

/** \name Resource ID test/conversion macros and inline functions */
/** \{ */

/**
 * \brief Convert a derived (app-specific) ID directly into an "unsigned long"
 *
 * This generic routine is implemented as a macro so it is agnostic to the actual argument type,
 * and it will evaluate correctly so long as the argument type is based on the CFE_RESOURCEID_BASE_TYPE.
 *
 * There is no inverse of this macro, as it depends on the actual derived type desired.
 * Applications needing to recreate an ID from an integer should use CFE_ResourceId_FromInteger()
 * combined with a cast/conversion to the correct/intended derived type, as needed.
 *
 * \note This evaluates as an "unsigned long" such that it can be used in
 * printf()-style functions with the "%lx" modifier without extra casting,
 * as this is the most typical use-case for representing an ID as an integer.
 */
#define CFE_RESOURCEID_TO_ULONG(id) CFE_ResourceId_ToInteger(CFE_RESOURCEID_UNWRAP(id))

/**
 * \brief Determine if a derived (app-specific) ID is defined or not
 *
 * This generic routine is implemented as a macro so it is agnostic to the actual argument type,
 * and it will evaluate correctly so long as the argument type is based on the CFE_RESOURCEID_BASE_TYPE.
 */
#define CFE_RESOURCEID_TEST_DEFINED(id) CFE_ResourceId_IsDefined(CFE_RESOURCEID_UNWRAP(id))

/**
 * \brief Determine if two derived (app-specific) IDs are equal
 *
 * This generic routine is implemented as a macro so it is agnostic to the actual argument type,
 * and it will evaluate correctly so long as the argument type is based on the CFE_RESOURCEID_BASE_TYPE.
 */
#define CFE_RESOURCEID_TEST_EQUAL(id1, id2) CFE_ResourceId_Equal(CFE_RESOURCEID_UNWRAP(id1), CFE_RESOURCEID_UNWRAP(id2))

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
 * CFE_ResourceId_ToIndex() for cases where a zero-based array/table index
 * is needed.
 *
 * @sa CFE_ResourceId_FromInteger()
 *
 * @param[in]   id    Resource ID to convert
 * @returns Integer value corresponding to ID
 */
static inline unsigned long CFE_ResourceId_ToInteger(CFE_ResourceId_t id)
{
    return (unsigned long)CFE_RESOURCEID_UNWRAP(id);
}

/**
 * @brief Convert an integer to a resource ID.
 *
 * This is the inverse of CFE_ResourceId_ToInteger(), and reconstitutes
 * the original CFE_ResourceId_t value from the integer representation.
 *
 * This may be used, for instance, where an ID value is parsed from a text
 * file or message using C library APIs such as scanf() or strtoul().
 *
 * @sa CFE_ResourceId_ToInteger()
 *
 * @param[in]   Value    Integer value to convert
 * @returns ID value corresponding to integer
 */
static inline CFE_ResourceId_t CFE_ResourceId_FromInteger(unsigned long Value)
{
    return (CFE_ResourceId_t)CFE_RESOURCEID_WRAP(Value);
}

/**
 * @brief Compare two Resource ID values for equality
 *
 * @param[in]   id1    Resource ID to check
 * @param[in]   id2    Resource ID to check
 * @returns true if id1 and id2 are equal, false otherwise.
 */
static inline bool CFE_ResourceId_Equal(CFE_ResourceId_t id1, CFE_ResourceId_t id2)
{
    return (CFE_RESOURCEID_UNWRAP(id1) == CFE_RESOURCEID_UNWRAP(id2));
}

/**
 * @brief Check if a resource ID value is defined
 *
 * The constant #CFE_RESOURCEID_UNDEFINED represents an undefined ID value,
 * such that the expression:
 *
 *      CFE_ResourceId_IsDefined(CFE_RESOURCEID_UNDEFINED)
 *
 * Always returns false.
 *
 * @param[in]   id    Resource ID to check
 * @returns True if the ID may refer to a defined entity, false if invalid/undefined.
 */
static inline bool CFE_ResourceId_IsDefined(CFE_ResourceId_t id)
{
    return (!CFE_ResourceId_Equal(id, CFE_RESOURCEID_UNDEFINED));
}

/** \} */

/*
 * Non-inline API functions provided by the Resource ID module
 */

/**
 * @brief Get the Base value (type/category) from a resource ID value
 *
 * This masks out the ID serial number to obtain the base value, which is different
 * for each resource type.
 *
 * @note The value is NOT shifted or otherwise adjusted.
 *
 * @param[in]   ResourceId   the resource ID to decode
 * @returns     The base value associated with that ID
 */
uint32 CFE_ResourceId_GetBase(CFE_ResourceId_t ResourceId);

/**
 * @brief Get the Serial Number (sequential ID) from a resource ID value
 *
 * This masks out the ID base value to obtain the serial number, which is different
 * for each entity created.
 *
 * @param[in]   ResourceId   the resource ID to decode
 * @returns     The serial number associated with that ID
 */
uint32 CFE_ResourceId_GetSerial(CFE_ResourceId_t ResourceId);

/**
 * @brief Locate the next resource ID which does not map to an in-use table entry
 *
 * This begins searching from StartId which should be the most recently issued ID
 * for the resource category.  This will then search for the next ID which does
 * _not_ map to a table entry that is in use.  That is, it does not alias any
 * valid ID when converted to an array index.
 *
 * returns an undefined ID value if no open slots are available
 *
 * @param[in]   StartId   the last issued ID for the resource category (app, lib, etc).
 * @param[in]   TableSize the maximum size of the target table
 * @param[in]   CheckFunc a function to check if the given ID is available
 * @returns     Next ID value which does not map to a valid entry
 * @retval      #CFE_RESOURCEID_UNDEFINED if no open slots or bad arguments.
 *
 */
CFE_ResourceId_t CFE_ResourceId_FindNext(CFE_ResourceId_t StartId, uint32 TableSize,
                                         bool (*CheckFunc)(CFE_ResourceId_t));

/**
 * @brief Internal routine to aid in converting an ES resource ID to an array index

 * @param[in]  Id        The resource ID
 * @param[in]  BaseValue The respective ID base value corresponding to the ID type
 * @param[in]  TableSize The actual size of the internal table (MAX index value + 1)
 * @param[out] Idx       The output index
 *
 * @return Execution status, see @ref CFEReturnCodes
 * @retval #CFE_SUCCESS                     @copybrief CFE_SUCCESS
 * @retval #CFE_ES_BAD_ARGUMENT             @copybrief CFE_ES_BAD_ARGUMENT
 * @retval #CFE_ES_ERR_RESOURCEID_NOT_VALID @copybrief CFE_ES_ERR_RESOURCEID_NOT_VALID
 */
int32 CFE_ResourceId_ToIndex(CFE_ResourceId_t Id, uint32 BaseValue, uint32 TableSize, uint32 *Idx);

#endif /* CFE_RESOURCEID_H */
