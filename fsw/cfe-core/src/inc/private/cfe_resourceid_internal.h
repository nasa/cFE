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
 * \file cfe_resourceid_internal.h
 *
 * Contains CFE internal prototypes and definitions related to resource
 * management and related CFE resource IDs.
 *
 * A CFE ES Resource ID is a common way to identify CFE-managed resources such
 * as apps, tasks, counters, memory pools, CDS blocks, and other entities.
 */

#ifndef CFE_RESOURCEID_INTERNAL_H
#define CFE_RESOURCEID_INTERNAL_H

/*
** Include Files
*/
#include "cfe_resourceid.h"
#include "osapi-idmap.h"

/*
** Defines
*/

/*
 * Limits/definitions related to CFE_ES_ResourceID_t values.
 *
 * Defining based on OSAL ID values makes this object a superset of
 * the OSAL ID type, such that OSAL IDs can be represented as ES resource IDs
 * and not conflict with/alias each other.
 *
 * NOTE: This reflects a bit if "inside knowledge" about how OSAL IDs are
 * constructed.  The overlap between OSAL IDs and ES IDs may not always be
 * consistent, and they can diverge in a future version.
 */
#define CFE_ES_RESOURCEID_SHIFT OS_OBJECT_TYPE_SHIFT
#define CFE_ES_RESOURCEID_MAX   ((1 << CFE_ES_RESOURCEID_SHIFT)-1)
#define CFE_ES_RESOURCEID_MARK  (0x02000000)

/** 
 * @defgroup CFEResourceIDBase Resource ID base values 
 * @{
 */
#define CFE_ES_APPID_BASE       (CFE_ES_RESOURCEID_MARK | ((OS_OBJECT_TYPE_USER+1) << CFE_ES_RESOURCEID_SHIFT))
#define CFE_ES_LIBID_BASE       (CFE_ES_RESOURCEID_MARK | ((OS_OBJECT_TYPE_USER+2) << CFE_ES_RESOURCEID_SHIFT))
#define CFE_ES_COUNTID_BASE     (CFE_ES_RESOURCEID_MARK | ((OS_OBJECT_TYPE_USER+3) << CFE_ES_RESOURCEID_SHIFT))
#define CFE_ES_POOLID_BASE      (CFE_ES_RESOURCEID_MARK | ((OS_OBJECT_TYPE_USER+4) << CFE_ES_RESOURCEID_SHIFT))
#define CFE_ES_CDSBLOCKID_BASE  (CFE_ES_RESOURCEID_MARK | ((OS_OBJECT_TYPE_USER+5) << CFE_ES_RESOURCEID_SHIFT))

/** @} */


/**
 * @brief Get the Base value (type/category) from a resource ID value
 *
 * This masks out the ID serial number to obtain the base value, which is different
 * for each resource type.
 * 
 * @note The value is NOT shifted or otherwise adjusted.  It should match one of the
 * defined base values in @ref CFEESResourceIDBase.
 *
 * @param[in]   ResourceId   the resource ID to decode
 * @returns     The base value associated with that ID
 */
static inline uint32 CFE_ES_ResourceID_GetBase(CFE_ES_ResourceID_t ResourceId)
{
    uint32 ResourceType = CFE_ES_ResourceID_ToInteger(ResourceId);
    return (ResourceType - (ResourceType & CFE_ES_RESOURCEID_MAX));
}

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
 * @retval      #CFE_ES_RESOURCEID_UNDEFINED if no open slots.
 *
 */
CFE_ES_ResourceID_t CFE_ES_FindNextAvailableId(CFE_ES_ResourceID_t StartId, uint32 TableSize, bool (*CheckFunc)(CFE_ES_ResourceID_t) );

/**
 * @brief Convert an ES Task ID to an OSAL task ID
 *
 * Task IDs created via CFE ES are also OSAL task IDs, but technically
 * do refer to a different scope and therefore have a different type
 * to represent them.
 *
 * This function facilitates converting between the types.
 *
 * @note Currently the numeric values are the same and can be interchanged
 * for backward compatibility, however they may diverge in a future version.
 * New code should not assume equivalence between OSAL and ES task IDs.
 *
 * @sa CFE_ES_ResourceID_FromOSAL
 *
 * @param[in] id    The ES task ID
 * @returns         The OSAL task ID
 */
osal_id_t CFE_ES_ResourceID_ToOSAL(CFE_ES_ResourceID_t id);

/**
 * @brief Convert an ES Task ID to an OSAL task ID
 *
 * Task IDs created via CFE ES are also OSAL task IDs, but technically
 * do refer to a different scope and therefore have a different type
 * to represent them.
 *
 * This function facilitates converting between the types.
 *
 * @note Currently the numeric values are the same and can be interchanged
 * for backward compatibility, however they may diverge in a future version.
 * New code should not assume equivalence between OSAL and ES task IDs.
 *
 * @sa CFE_ES_ResourceID_ToOSAL
 *
 * @param[in] id    The OSAL task ID
 * @returns         The ES task ID
 */
CFE_ES_ResourceID_t CFE_ES_ResourceID_FromOSAL(osal_id_t id);

/**
 * @brief Internal routine to aid in converting an ES resource ID to an array index

 * @param[in]  Serial    The resource serial number (type info masked out)
 * @param[in]  TableSize The size of the internal table (MAX value)
 * @param[out] Idx       The output index
 * @returns Status code, CFE_SUCCESS if successful.
 */
int32 CFE_ES_ResourceID_ToIndex(uint32 Serial, uint32 TableSize, uint32 *Idx);


#endif  /* CFE_RESOURCEID_H */
