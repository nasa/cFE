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
 *  Purpose:
 *	Unit specification for Executive Services library functions and macros.
 *
 *  References:
 *     Flight Software Branch C Coding Standard Version 1.0a
 *     cFE Flight Software Application Developers Guide
 *
 *	Notes:
 *
 */

#ifndef CFE_ES_API_TYPEDEFS_H
#define CFE_ES_API_TYPEDEFS_H

/*
** Includes
*/
#include "common_types.h"
#include "cfe_es_extern_typedefs.h"

/*
** Note about reset type and subtypes:
**
** These values come from the PSP so the actual definition of these enumerations
** was moved to the PSP header file <cfe_psp.h>.
**
** In the future the Electronic Data sheets (EDS) for PSP/ES
** will define the exact values to use in telemetry messages.
*/

/*
**  Reset types
*/
/** \name Reset Type extensions */
/** \{ */
#define CFE_ES_APP_RESTART CFE_PSP_RST_TYPE_MAX /**< Application only was reset (extend the PSP enumeration here) */
/** \} */

/*****************************************************************************/
/*
** Type Definitions
*/

/*
** Entry Function Prototypes
*/
typedef void (*CFE_ES_TaskEntryFuncPtr_t)(void); /**< \brief Required Prototype of Task Main Functions */
typedef int32 (*CFE_ES_LibraryEntryFuncPtr_t)(
    CFE_ES_LibId_t LibId); /**< \brief Required Prototype of Library Initialization Functions */

/**
 * \brief Compatible typedef for ES child task entry point.
 *
 * All ES task functions (main + child) use the same entry point type.
 */
typedef CFE_ES_TaskEntryFuncPtr_t CFE_ES_ChildTaskMainFuncPtr_t;

/**
 * @brief Type for the stack pointer of tasks.
 *
 * This type is used in the CFE ES task API.
 */
typedef void *CFE_ES_StackPointer_t; /* aka osal_stackptr_t in proposed OSAL change */

/**
 * \brief Checksum/CRC algorithm identifiers
 *
 * Currently only CFE_ES_CrcType_16_ARC is supported.
 *
 * All defined CRC algorithms should include a check value, which is the accepted
 * result of computing the CRC across the fixed string "123456789"
 */
typedef enum CFE_ES_CrcType_Enum
{
    /**
     * \brief Reserved placeholder value
     *
     * No computation is performed, always returns 0 as a result.
     */
    CFE_ES_CrcType_NONE = 0,

    /**
     * \brief Implementation of CRC-16/ARC
     *
     * \par
     *   Polynomial: 0x8005 <br>
     *   Initialization: 0x0000 <br>
     *   Reflect Input/Output: true <br>
     *   Check value: 0xbb3d <br>
     *   XorOut: 0x0000 <br>
     */
    CFE_ES_CrcType_16_ARC = 1,

    /**
     * Placeholder for end of normal enumeration list
     * This should reflect the number of algorithms defined.
     */
    CFE_ES_CrcType_MAX = 2,

    /*
     * Backward compatibility values.
     * For compatibility with apps, these simplified symbols need to be defined,
     * and they also must map to nonzero values.
     */

    /** CRC-16 historically implied CRC-16/ARC */
    CFE_ES_CrcType_CRC_16 = CFE_ES_CrcType_16_ARC,

    /**< CRC-8 historically defined but not implemented, value must not be 0 */
    CFE_ES_CrcType_CRC_8 = CFE_ES_CrcType_MAX + 1,

    /**< CRC-32 historically defined but not implemented, value must not be 0 */
    CFE_ES_CrcType_CRC_32 = CFE_ES_CrcType_MAX + 2,

} CFE_ES_CrcType_Enum_t;

/**
 * \brief Pool Alignment
 *
 * Union that can be used for minimum memory alignment of ES memory pools on the target.
 * It contains the longest native data types such that the alignment of this structure
 * should reflect the largest possible alignment requirements for any data on this processor.
 */
typedef union CFE_ES_PoolAlign
{
    void *Ptr; /**< \brief Aligned pointer */
    /* note -- native types (int/double) are intentional here */
    long long int LongInt;    /**< \brief Aligned Long Integer */
    long double   LongDouble; /**< \brief Aligned Long Double */
} CFE_ES_PoolAlign_t;

/**
 * \brief Static Pool Type
 *
 * A macro to help instantiate static memory pools that are correctly aligned.
 * This resolves to a union type that contains a member called "Data" that will
 * be correctly aligned to be a memory pool and sized according to the argument.
 */
#define CFE_ES_STATIC_POOL_TYPE(size)  \
    union                              \
    {                                  \
        CFE_ES_PoolAlign_t Align;      \
        uint8              Data[size]; \
    }

/**
 * @brief Pointer type used for memory pool API
 *
 * This is used in the Get/Put API calls to refer to a pool buffer.
 *
 * This pointer is expected to be type cast to the real object
 * type after getting a new buffer.  Using void* allows this
 * type conversion to occur easily.
 *
 * @note Older versions of CFE implemented the API using a uint32*,
 * which required explicit type casting everywhere it was called.
 * Although the API type is now void* to make usage easier, the
 * pool buffers are aligned to machine requirements - typically 64 bits.
 */
typedef void *CFE_ES_MemPoolBuf_t;

/**
 * @brief Conversion macro to create buffer pointer from another type
 *
 * In cases where the actual buffer pointer is computed, this macro
 * aids in converting the computed address (typically an OSAL "cpuaddr"
 * type) into a buffer pointer.
 *
 * @note Any address calculation needs to take machine alignment
 * requirements into account.
 */
#define CFE_ES_MEMPOOLBUF_C(x) ((CFE_ES_MemPoolBuf_t)(x))

/** \name Conversions for ES resource IDs */
/** \{ */

/*
 * Conversion macros for each ES resource ID subtype
 *
 * These accept a generic/non-specific CFE_ResourceId_t value
 * and convert it to the corresponding resource-specific type.
 *
 * These should only be used when with the resource ID constants,
 * or where the code has confirmed or is determining the generic
 * identifier does correspond to a resource of that type.
 */
#define CFE_ES_APPID_C(val)     ((CFE_ES_AppId_t)CFE_RESOURCEID_WRAP(val))
#define CFE_ES_TASKID_C(val)    ((CFE_ES_TaskId_t)CFE_RESOURCEID_WRAP(val))
#define CFE_ES_LIBID_C(val)     ((CFE_ES_LibId_t)CFE_RESOURCEID_WRAP(val))
#define CFE_ES_COUNTERID_C(val) ((CFE_ES_CounterId_t)CFE_RESOURCEID_WRAP(val))
#define CFE_ES_MEMHANDLE_C(val) ((CFE_ES_MemHandle_t)CFE_RESOURCEID_WRAP(val))
#define CFE_ES_CDSHANDLE_C(val) ((CFE_ES_CDSHandle_t)CFE_RESOURCEID_WRAP(val))

/** \} */

/** \name Type-specific initializers for "undefined" resource IDs */
/** \{ */

#define CFE_ES_APPID_UNDEFINED     CFE_ES_APPID_C(CFE_RESOURCEID_UNDEFINED)
#define CFE_ES_TASKID_UNDEFINED    CFE_ES_TASKID_C(CFE_RESOURCEID_UNDEFINED)
#define CFE_ES_LIBID_UNDEFINED     CFE_ES_LIBID_C(CFE_RESOURCEID_UNDEFINED)
#define CFE_ES_COUNTERID_UNDEFINED CFE_ES_COUNTERID_C(CFE_RESOURCEID_UNDEFINED)
#define CFE_ES_MEMHANDLE_UNDEFINED CFE_ES_MEMHANDLE_C(CFE_RESOURCEID_UNDEFINED)
#define CFE_ES_CDS_BAD_HANDLE      CFE_ES_CDSHANDLE_C(CFE_RESOURCEID_UNDEFINED)
/** \} */

/** \name Task Stack Constants */
/** \{ */

/**
 * \brief Indicates that the stack for the child task should be dynamically allocated.
 *
 * This value may be supplied as the Stack Pointer argument to CFE_ES_ChildTaskCreate()
 * to indicate that the stack should be dynamically allocated.
 */
#define CFE_ES_TASK_STACK_ALLOCATE NULL /* aka OS_TASK_STACK_ALLOCATE in proposed OSAL change */
/** \} */

#define CFE_ES_NO_MUTEX  false /**< \brief Indicates that the memory pool selection will not use a semaphore */
#define CFE_ES_USE_MUTEX true  /**< \brief Indicates that the memory pool selection will use a semaphore */

#endif /* CFE_ES_API_TYPEDEFS_H */
