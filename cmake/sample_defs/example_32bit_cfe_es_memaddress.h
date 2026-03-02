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
 * Example header file override that defines memory addresses and offsets
 * to be a 32 bit integer values.
 *
 * This is backward compatible with prior CFS versions where all memory
 * references were assumed to be 32 bits in size.  It is still possible
 * to execute this on a 64-bit CPU, but all address will be truncated to
 * the lower 32 bits.  One will also not be able to send commands that
 * require a memory address, as the address will be incomplete.
 *
 * To use this implementation, clone this file as "cfe_es_memaddress.h"
 * in your local defs dir.
 */

#ifndef EXAMPLE_32BIT_CFE_ES_MEMADDRESS_H
#define EXAMPLE_32BIT_CFE_ES_MEMADDRESS_H

#include "common_types.h"

/*
 * NOTE: On a typical Linux/GCC build environment, this produces
 * sizeof(CFE_ES_AppInfo_t) == 184 based on the current definition
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
 */
typedef uint32 CFE_ES_MemOffset_t;

/**
 * @brief Memory Offset initializer wrapper
 *
 * A converter macro to use when initializing a CFE_ES_MemOffset_t
 * from an integer value of a different type.
 */
#define CFE_ES_MEMOFFSET_C(x) ((CFE_ES_MemOffset_t)(x))

/**
 * @brief Memory Offset to integer value (size_t) wrapper
 *
 * A converter macro to use when interpreting a CFE_ES_MemOffset_t
 * value as a "size_t" type
 */
#define CFE_ES_MEMOFFSET_TO_SIZET(x) ((size_t)(x))

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
typedef uint32 CFE_ES_MemAddress_t;

/**
 * @brief Memory Address initializer wrapper
 *
 * A converter macro to use when initializing a CFE_ES_MemAddress_t
 * from a pointer value of a different type.
 */
#define CFE_ES_MEMADDRESS_C(x) ((CFE_ES_MemAddress_t)((cpuaddr)(x)&0xFFFFFFFF))

/**
 * @brief Memory Address to pointer wrapper
 *
 * A converter macro to use when interpreting a CFE_ES_MemAddress_t
 * as a pointer value.
 */
#define CFE_ES_MEMADDRESS_TO_PTR(x) ((void *)(cpuaddr)(x))

#endif /* CFE_ES_EXTERN_TYPEDEFS_H */
