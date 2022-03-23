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
 * An implementation of CFE resource ID base values/limits that will be
 * compatible with OSAL IDs.  This is intended as a transitional tool to
 * provide runtime value uniqueness, particularly when the "simple" (compatible)
 * resource ID implementation is used.  In this mode, compiler type checking
 * is disabled, and so OSAL IDs can be silently interchanged with CFE IDs.
 *
 * However, by ensuring uniqueness in the runtime values, any ID handling
 * errors may at least be detectable at runtime.
 *
 * This still works fine with the "strict" resource ID option, but is less
 * important as the compiler type checking should prevent this type of error
 * before the code even runs.
 *
 * The downside to this implementation is that it has a dependency on the
 * OSAL ID structure.
 */

#ifndef CFE_RESOURCEID_BASEVALUE_H
#define CFE_RESOURCEID_BASEVALUE_H

/*
** Include Files
*/
#include "cfe_resourceid_typedef.h"

/*
 * In this configuration, CFE resource IDs are tailored to not
 * conflict/overlap with OSAL IDs, and are structured in a similar manner.
 */
#include "osapi-idmap.h"

/*
 * Limits/definitions related to CFE_ResourceId_t values.
 *
 * Defining based on OSAL ID values makes this object a superset of
 * the OSAL ID type, such that OSAL IDs can be represented as resource IDs
 * and not conflict with/alias each other.
 *
 * NOTE: This reflects a bit if "inside knowledge" about how OSAL IDs are
 * constructed.  The overlap between OSAL IDs and ES IDs may not always be
 * consistent, and they can diverge in a future version.
 */
#define CFE_RESOURCEID_SHIFT OS_OBJECT_TYPE_SHIFT
#define CFE_RESOURCEID_MAX   OS_OBJECT_INDEX_MASK

/**
 * @brief A macro to generate a CFE resource ID base value from an offset
 *
 * Each CFE ID range is effectively an extension of OSAL ID ranges by
 * starting at OS_OBJECT_TYPE_USER.
 */
#define CFE_RESOURCEID_MAKE_BASE(offset) (CFE_RESOURCEID_MARK | ((offset) << CFE_RESOURCEID_SHIFT))

#endif /* CFE_RESOURCEID_BASEVALUE_H */
