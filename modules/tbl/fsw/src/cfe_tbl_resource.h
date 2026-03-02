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
 * Contains basic prototypes and definitions related to CFE TBL resource
 * management and related resource IDs.
 *
 * A CFE TBL Resource ID is a common way to identify CFE-managed resources such
 * as registry entries, buffers, state records, and other entities.
 *
 * ABOUT RESOURCE TABLE ACCESSORS
 * ==============================
 *
 * These accessors facilitate consistent lookup/matching/allocation/deallocation patterns
 * across all TBL resources.  The following types of resources can be managed in this
 * fashion:
 *
 *  - Access Descriptors (Table Handles, external identifiers)
 *  - Registry Records (Table registry, internal identifiers)
 *  - Load Buffers (both shared and table-specific)
 *  - Validation Results
 *  - Dump State
 *  - CDS registries
 *
 * A full set of accessors contains the following basic methods:
 *
 * | **Method**  | **Description**                                       |
 * |:------------|:------------------------------------------------------|
 * | LocateByID  | Returns a pointer to the entry associated with an ID  |
 * | ToIndex     | Converts an entry ID to a 0-based array index         |
 * | IsUsed      | Checks if a given entry is currently in use           |
 * | SetUsed     | Sets an entry as being in use / not available         |
 * | SetFree     | Sets an entry as being available / not in use         |
 * | GetId       | Gets the resource ID associated with an entry pointer |
 * | IsMatch     | Checks if an entry pointer is a match to the given ID |
 * | GetNext     | Returns the next/pending ID suitable for a new record |
 *
 * Each set of methods, along with the concrete definition of the records being
 * managed is defined in a separate header file per resource type.
 *
 * This general file implements the IDs (abstract identifiers for these
 * objects/table entries) and an abstract structure definition for each.
 *
 */

#ifndef CFE_TBL_RESOURCE_H
#define CFE_TBL_RESOURCE_H

#include "common_types.h"
#include "cfe_resourceid.h"
#include "cfe_fs_extern_typedefs.h"

/*
 * ---------------------------------------------------------------------------------------
 *
 *     ~~~  LOAD BUFFER SECTION ~~~
 *
 * These operate on CFE_TBL_LoadBuff_t* and buffer ID
 *
 * ---------------------------------------------------------------------------------------
 */

/**
 * Type definition for CFE_TBL_LoadBuffId_t
 *
 * This uses the resource ID mechanisms/patterns to avoid ambiguity
 */
typedef CFE_RESOURCEID_BASE_TYPE CFE_TBL_LoadBuffId_t;

/*
 * Standard set of macros for resource IDs --
 * Initialization Cast/Conversion: CFE_TBL_LOADBUFFID_C
 * Undefined constant: CFE_TBL_LOADBUFFID_UNDEFINED
 * Equality check: CFE_TBL_LOADBUFFID_EQ
 * Integer cast: CFE_TBL_LOADBUFFID_INT
 * Validity check: CFE_TBL_LOADBUFFID_IS_VALID
 */

/* initialization macros */
#define CFE_TBL_LOADBUFFID_C(val)    ((CFE_TBL_LoadBuffId_t)CFE_RESOURCEID_WRAP(val))
#define CFE_TBL_LOADBUFFID_UNDEFINED CFE_TBL_LOADBUFFID_C(CFE_RESOURCEID_UNDEFINED)

/* check/validation macros */
#define CFE_TBL_LOADBUFFID_EQ(x, y)    CFE_RESOURCEID_TEST_EQUAL(x, y)
#define CFE_TBL_LOADBUFFID_INT(x)      CFE_RESOURCEID_TO_ULONG(x)
#define CFE_TBL_LOADBUFFID_IS_VALID(x) CFE_RESOURCEID_TEST_DEFINED(x)

/*
 * ---------------------------------------------------------------------------------------
 *
 *     ~~~  REGISTRY RECORD SECTION ~~~
 *
 * These operate on CFE_TBL_RegistryRec_t* and Registry ID
 *
 * ---------------------------------------------------------------------------------------
 */

/*
 * NOTE: The CFE_TBL_RegId_t type is externally exposed because it appears
 * in telemetry and other output files
 *
 * Additionally the initialization macros are defined externally
 * Initialization Cast/Conversion: CFE_TBL_REGID_C
 * Undefined constant: CFE_TBL_REGID_UNDEFINED
 */

/*
 * Standard set of macros for resource IDs --
 * Equality check: CFE_TBL_REGID_EQ
 * Integer cast: CFE_TBL_REGID_INT
 * Validity check: CFE_TBL_REGID_IS_VALID
 */

/* check/validation macros */
#define CFE_TBL_REGID_EQ(x, y)    CFE_RESOURCEID_TEST_EQUAL(x, y)
#define CFE_TBL_REGID_INT(x)      CFE_RESOURCEID_TO_ULONG(x)
#define CFE_TBL_REGID_IS_VALID(x) CFE_RESOURCEID_TEST_DEFINED(x)

/*
 * ---------------------------------------------------------------------------------------
 *
 *     ~~~  ACCESS DESCRIPTOR SECTION ~~~
 *
 * These operate on CFE_TBL_AccessDescriptor_t* and Handle ID
 *
 * ---------------------------------------------------------------------------------------
 */

/*
 * NOTE: The CFE_TBL_HandleId_t type is externally exposed because it is part of the API
 *
 * Additionally the initialization macros are defined externally
 *
 * Initialization Cast/Conversion: CFE_TBL_HANDLEID_C(val)
 * Undefined constant: CFE_TBL_HANDLEID_UNDEFINED
 * Equality check: CFE_TBL_HandleID_IsEqual
 * Integer cast: CFE_TBL_HandleID_AsInt
 * Validity check: CFE_TBL_HandleID_IsDefined
 */

/*
 * ---------------------------------------------------------------------------------------
 *
 *     ~~~  VALIDATION RESULT RECORD SECTION ~~~
 *
 * These operate on CFE_TBL_ValidationResult_t* and Validation Result ID
 *
 * ---------------------------------------------------------------------------------------
 */

/** \brief Value indicating when no Validation is Pending */
/**
**  This macro is used to indicate no Validation is Pending by assigning it to
**  #CFE_TBL_RegistryRec_t::ValidateActiveId or #CFE_TBL_RegistryRec_t::ValidateInactiveId
*/
#define CFE_TBL_NO_VALIDATION_PENDING CFE_TBL_VALRESULTID_UNDEFINED

/**
 * @brief A type for Validation Result Buffer IDs
 *
 * This is the type that is used for any API accepting or returning a Validation Result ID
 */
typedef CFE_RESOURCEID_BASE_TYPE CFE_TBL_ValidationResultId_t;

#define CFE_TBL_VALRESULTID_C(val)    ((CFE_TBL_ValidationResultId_t)CFE_RESOURCEID_WRAP(val))
#define CFE_TBL_VALRESULTID_UNDEFINED CFE_TBL_VALRESULTID_C(CFE_RESOURCEID_UNDEFINED)

#define CFE_TBL_VALRESULTID_EQ(x, y)    CFE_RESOURCEID_TEST_EQUAL(x, y)
#define CFE_TBL_VALRESULTID_INT(x)      CFE_RESOURCEID_TO_ULONG(x)
#define CFE_TBL_VALRESULTID_IS_VALID(x) CFE_RESOURCEID_TEST_DEFINED(x)

/*
 * ---------------------------------------------------------------------------------------
 *
 *     ~~~  DUMP CONTROL RECORD SECTION ~~~
 *
 * These operate on CFE_TBL_DumpControl_t* and Dump Control ID
 *
 * ---------------------------------------------------------------------------------------
 */

/** \brief Value indicating when no Dump is Pending on a Dump-Only Table */
/**
**  This macro is used to indicate no Dump is Pending by assigning it to
**  #CFE_TBL_RegistryRec_t::DumpControlId
*/
#define CFE_TBL_NO_DUMP_PENDING CFE_TBL_DUMPCTRLID_UNDEFINED

/************************  Internal Structure Definitions  *****************************/

/**
 * @brief A type for Dump Control Block IDs
 *
 * This is the type that is used for any API accepting or returning a dump control block
 */
typedef CFE_RESOURCEID_BASE_TYPE CFE_TBL_DumpCtrlId_t;

#define CFE_TBL_DUMPCTRLID_C(val)    ((CFE_TBL_DumpCtrlId_t)CFE_RESOURCEID_WRAP(val))
#define CFE_TBL_DUMPCTRLID_UNDEFINED CFE_TBL_DUMPCTRLID_C(CFE_RESOURCEID_UNDEFINED)

#define CFE_TBL_DUMPCTRLID_EQ(x, y)    CFE_RESOURCEID_TEST_EQUAL(x, y)
#define CFE_TBL_DUMPCTRLID_INT(x)      CFE_RESOURCEID_TO_ULONG(x)
#define CFE_TBL_DUMPCTRLID_IS_VALID(x) CFE_RESOURCEID_TEST_DEFINED(x)

/* Abstract definitions of all typedefs */
typedef struct CFE_TBL_AccessDescriptor CFE_TBL_AccessDescriptor_t;
typedef struct CFE_TBL_RegistryRec      CFE_TBL_RegistryRec_t;
typedef struct CFE_TBL_LoadBuff         CFE_TBL_LoadBuff_t;
typedef struct CFE_TBL_ValidationResult CFE_TBL_ValidationResult_t;
typedef struct CFE_TBL_DumpControl      CFE_TBL_DumpControl_t;

#endif /* CFE_TBL_RESOURCE_H */
