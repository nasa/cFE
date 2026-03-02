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
 * to be a full 64 bit integer value.
 *
 * This is the simplest and most efficient approach to use 64 bit addressing,
 * but may introduce unexpected padding in certain cases.  On most systems a
 * uint64 value needs to be aligned on a 64-bit boundary, and CFS has some
 * TLM and CMD structures where this is not the case.  These will be padded by
 * the compiler and may cause interoperability issues.
 *
 * To use this implementation, clone this file as "cfe_es_memaddress.h"
 * in your local defs dir.
 */

#ifndef DEFAULT_CFE_ES_MEMADDRESS_H
#define DEFAULT_CFE_ES_MEMADDRESS_H

#include "common_types.h"

/*
 * NOTE: On a typical Linux/GCC build environment, this produces
 * sizeof(CFE_ES_AppInfo_t) == 224 based on the current definition
 */

/**
 * @brief Type used for memory sizes and offsets in commands and telemetry
 *
 * Use a full 64-bit integer value for memory offsets
 */
typedef uint64 CFE_ES_MemOffset_t;

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
 * Use a full 64-bit integer value for memory offsets
 */
typedef uint64 CFE_ES_MemAddress_t;

/**
 * @brief Memory Address initializer wrapper
 *
 * A converter macro to use when initializing a CFE_ES_MemAddress_t
 * from a pointer value of a different type.
 */
#define CFE_ES_MEMADDRESS_C(x) ((CFE_ES_MemAddress_t)(cpuaddr)(x))

/**
 * @brief Memory Address to pointer wrapper
 *
 * A converter macro to use when interpreting a CFE_ES_MemAddress_t
 * as a pointer value.
 */
#define CFE_ES_MEMADDRESS_TO_PTR(x) ((void *)(cpuaddr)(x))

#endif /* CFE_ES_EXTERN_TYPEDEFS_H */
