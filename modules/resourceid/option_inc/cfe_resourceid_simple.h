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
 * Declarations and prototypes for simple (uint32 compatible) resource ID implementation
 */

#ifndef CFE_RESOURCEID_SIMPLE_H
#define CFE_RESOURCEID_SIMPLE_H

#include "common_types.h"

/**
 * @brief A type that provides a common, abstract identifier for
 * all ES managed resources (e.g. apps, tasks, counters, etc).
 *
 * Fundamentally an unsigned integer but users should treat it as
 * opaque, and only go through the ES API for introspection.
 */
typedef uint32 CFE_ResourceId_t;

/**
 * @brief A macro providing a type for app-specific IDs
 *
 * Local ID types are just direct typedefs to CFE_ResourceId_t in this mode,
 * this means all ID values can be interchanged.
 */
#define CFE_RESOURCEID_BASE_TYPE CFE_ResourceId_t

/**
 * @brief A fixed bit that will be set in all CFE resource ID values
 *
 * In simple mode this is zero/disabled so that OSAL IDs and CFE IDs will
 * have the same underlying values.  This replicates historical behavior where
 * CFE Task IDs and OSAL task IDs are same thing and are interchangeable.
 */
#define CFE_RESOURCEID_MARK 0

/*
 * Wrap/Unwrap macros.
 *
 * In simple mode these are a pass through/no-op as values are not
 * wrapped/protected.
 */
#define CFE_RESOURCEID_WRAP(x)   x
#define CFE_RESOURCEID_UNWRAP(x) x

#endif /* CFE_RESOURCEID_SIMPLE_H */
