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
 * Declarations and prototypes for strict (type-safe) resource ID implementation
 */

#ifndef CFE_RESOURCEID_STRICT_H
#define CFE_RESOURCEID_STRICT_H

#include "common_types.h"

/*
 * Inform other code that the "strict mode" is enabled for resource IDs
 *
 * This in turn allows ES to adjust other macros/conversions as necessary
 * to work with the strict type checking.
 */
#define CFE_RESOURCEID_STRICT_MODE

/**
 * @brief A type that provides a common, abstract identifier for
 * all CFE managed resources (e.g. apps, tasks, counters, etc).
 *
 * Fundamentally an unsigned integer but users should treat it as
 * opaque, and only go through the ES API for introspection.
 *
 */
typedef struct
{
    uint32 id;
} CFE_ResourceId_t;

/**
 * @brief A macro to generate a basetype for app-specific IDs
 *
 * Resource IDs may be "wrapped" a second time to make a unique
 * typedef for application-specific ID values.
 *
 * Defining this base type as a macro rather than a typedef is intentional
 * such that every time this is used it makes an equivalent but different
 * type.  That is, it is a different type per the compiler type checking
 * but has the same content/structure.
 */
#define CFE_RESOURCEID_BASE_TYPE \
    struct                       \
    {                            \
        CFE_ResourceId_t id;     \
    }

/**
 * @brief A fixed bit that should be set in all CFE resource ID values
 *
 * In strict mode this is nonzero so that OSAL IDs and CFE IDs will have
 * different values.  This means that CFE Task IDs will not be interchangeable
 * with OSAL task IDs, either in value or type.
 */
#define CFE_RESOURCEID_MARK 0x02000000

/*
 * Wrap/Unwrap macros.
 *
 * These are helpers for transparently accessing through wrapper types.
 *
 * These are not type-safe - Whenever possible applications should use
 * the type-safe inline functions  provided in cfe_resourceid.h instead.
 */
#define CFE_RESOURCEID_WRAP(x) \
    {                          \
        x                      \
    }
#define CFE_RESOURCEID_UNWRAP(x) (x).id

#endif /* CFE_RESOURCEID_STRICT_H */
