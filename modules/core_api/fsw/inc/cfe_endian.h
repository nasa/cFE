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
 * Purpose:
 *      Define macros to enforce big-endian/network byte order for 16 and 32 bit integers
 *
 */

#ifndef CFE_ENDIAN_H
#define CFE_ENDIAN_H

/*
** Include Files
*/
#include "common_types.h"

/*
 * SOFTWARE_BIG/LITTLE_BIT_ORDER COMPATIBILITY MACRO -
 *
 * This is provided only for backward compatibility.  Do not write any new code that
 * uses this macro.
 */
#if !defined(SOFTWARE_BIG_BIT_ORDER) && !defined(SOFTWARE_LITTLE_BIT_ORDER)

#if defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN || defined(__BIG_ENDIAN__) || defined(__ARMEB__) || \
    defined(__THUMBEB__) || defined(__AARCH64EB__) || defined(_MIBSEB) || defined(__MIBSEB) || defined(__MIBSEB__)
/* It is a big-endian target architecture */
#define SOFTWARE_BIG_BIT_ORDER
#elif defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN || defined(__LITTLE_ENDIAN__) || defined(__ARMEL__) || \
    defined(__THUMBEL__) || defined(__AARCH64EL__) || defined(_MIPSEL) || defined(__MIPSEL) || defined(__MIPSEL__) || \
    defined(__i386) || defined(__i386__) || defined(__i686) || defined(__i686__) || defined(__x86_64) ||              \
    defined(__x86_64__)
/* It is a little-endian target architecture */
#define SOFTWARE_LITTLE_BIT_ORDER
#else
#error Unknown byte order on this platform
#endif

#endif /* !defined(SOFTWARE_BIG_BIT_ORDER) && !defined(SOFTWARE_LITTLE_BIT_ORDER) */

/* Macro to convert 16/32 bit types from platform "endianness" to Big Endian */
#ifdef SOFTWARE_BIG_BIT_ORDER
#define CFE_MAKE_BIG16(n) (n)
#define CFE_MAKE_BIG32(n) (n)
#else
#define CFE_MAKE_BIG16(n) ((((n)&0x00FF) << 8) | (((n)&0xFF00) >> 8))
#define CFE_MAKE_BIG32(n) \
    ((((n)&0x000000FF) << 24) | (((n)&0x0000FF00) << 8) | (((n)&0x00FF0000) >> 8) | (((n)&0xFF000000) >> 24))
#endif

#endif /* CFE_ENDIAN_H */
