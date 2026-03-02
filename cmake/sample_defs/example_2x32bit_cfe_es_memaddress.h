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
 * Defines memory addresses and offsets to be 64 bit integer values
 *
 * This expands the traditional 32 bit memory addresses in commands and telemetry
 * out to 64 bits for compatibility with modern CPUs, but does so as a pair
 * of 32 bit values rather than a single 64 bit value.  This is done to avoid
 * the introduction of implicit padding if addresses or sizes are not aligned
 * at 64 bit offsets within the respective parent structure.
 *
 * This does necessitate that all access to these values is done via
 * the provided conversion macros.  Attempts to directly assign these
 * values to an integer type will fail to compile.
 */

#ifndef EXAMPLE_2X32BIT_CFE_ES_MEMADDRESS_H
#define EXAMPLE_2X32BIT_CFE_ES_MEMADDRESS_H

#include "common_types.h"

/*
 * NOTE: On a typical Linux/GCC build environment, this produces
 * sizeof(CFE_ES_AppInfo_t) == 216 based on the current definition
 */

/**
 * @brief Type used for memory sizes and offsets in commands and telemetry
 *
 * For backward compatibility with existing CFE code this should be uint32,
 * but all telemetry information will be limited to 4GB in size as a result.
 *
 * On 64-bit platforms this can be a 64-bit value which will allow larger
 * memory objects, but this will break compatibility with existing control
 * systems, and may also change the alignment/padding of messages.
 *
 * In either case this must be an unsigned type.
 *
 * @note It is defined as two uint32s rather than a uint64 in case it
 * is not aligned in the parent structure.
 */
typedef struct
{
    uint32 bits[2];
} CFE_ES_MemOffset_t;

static inline size_t CFE_ES_MemOffset_ToNative(const CFE_ES_MemOffset_t *val)
{
    /* this is done as two separate shifts, in case size_t is 32 bits */
    /* if that is the case then bits[1] should be entirely shifted out */
    size_t size = (size_t)val->bits[1] << 16;
    return ((size << 16) | (size_t)val->bits[0]);
}

static inline CFE_ES_MemOffset_t CFE_ES_MemOffset_FromNative(size_t val)
{
    CFE_ES_MemOffset_t offs;
    offs.bits[0] = (uint32)val;
    val >>= 16;
    offs.bits[1] = (val >> 16);
    return offs;
}

/**
 * @brief Memory Offset initializer wrapper
 *
 * A converter macro to use when initializing a CFE_ES_MemOffset_t
 * from an integer value of a different type.
 */
#define CFE_ES_MEMOFFSET_C(x) CFE_ES_MemOffset_FromNative(x)

/**
 * @brief Memory Offset to integer value (size_t) wrapper
 *
 * A converter macro to use when interpreting a CFE_ES_MemOffset_t
 * value as a "size_t" type
 */
#define CFE_ES_MEMOFFSET_TO_SIZET(x) CFE_ES_MemOffset_ToNative(&(x))

/**
 * @brief Type used for memory addresses in command and telemetry messages
 *
 * For backward compatibility with existing CFE code this should be uint32,
 * but if running on a 64-bit platform, addresses in telemetry will be
 * truncated to 32 bits and therefore will not be valid.
 *
 * On 64-bit platforms this can be a 64-bit address which will allow the
 * full memory address in commands and telemetry, but this will break
 * compatibility with existing control systems, and may also change
 * the alignment/padding of messages.
 *
 * In either case this must be an unsigned type.
 *
 * FSW code should access this value via the macros provided, which
 * converts to the native "cpuaddr" type provided by OSAL.  This macro
 * provides independence between the message representation and local
 * representation of a memory address.
 */
typedef struct
{
    uint32 bits[2];
} CFE_ES_MemAddress_t;

static inline cpuaddr CFE_ES_MemAddress_ToNative(const CFE_ES_MemAddress_t *val)
{
    /* this is done as two separate shifts, in case size_t is 32 bits */
    /* if that is the case then bits[1] should be entirely shifted out */
    cpuaddr size = (cpuaddr)val->bits[1] << 16;
    return ((size << 16) | (cpuaddr)val->bits[0]);
}

static inline CFE_ES_MemAddress_t CFE_ES_MemAddress_FromNative(cpuaddr val)
{
    CFE_ES_MemAddress_t offs;
    offs.bits[0] = (uint32)val;
    val >>= 16;
    offs.bits[1] = (val >> 16);
    return offs;
}
/**
 * @brief Memory Address initializer wrapper
 *
 * A converter macro to use when initializing a CFE_ES_MemAddress_t
 * from a pointer value of a different type.
 */
#define CFE_ES_MEMADDRESS_C(x) CFE_ES_MemAddress_FromNative((cpuaddr)(x))

/**
 * @brief Memory Address to pointer wrapper
 *
 * A converter macro to use when interpreting a CFE_ES_MemAddress_t
 * as a pointer value.
 */
#define CFE_ES_MEMADDRESS_TO_PTR(x) ((void *)CFE_ES_MemAddress_ToNative(&(x)))

#endif /* CFE_ES_EXTERN_TYPEDEFS_H */
