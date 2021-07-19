/*************************************************************************
**
**      GSC-18128-1, "Core Flight Executive Version 6.7"
**
**      Copyright (c) 2006-2019 United States Government as represented by
**      the Administrator of the National Aeronautics and Space Administration.
**      All Rights Reserved.
**
**      Licensed under the Apache License, Version 2.0 (the "License");
**      you may not use this file except in compliance with the License.
**      You may obtain a copy of the License at
**
**        http://www.apache.org/licenses/LICENSE-2.0
**
**      Unless required by applicable law or agreed to in writing, software
**      distributed under the License is distributed on an "AS IS" BASIS,
**      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**      See the License for the specific language governing permissions and
**      limitations under the License.
**
** File: cfe_test.c
**
** Purpose:
**   Initialization routine for CFE functional test
**   Demonstration of how to register and use the UT assert functions.
**
*************************************************************************/

/**
 * @file
 *
 * Declarations and prototypes for cfe_test module
 */

#ifndef CFE_TEST_H
#define CFE_TEST_H

/*
 * Includes
 */
#include "cfe.h"

#include "uttest.h"
#include "utassert.h"

/**
 * Name of log file to write
 *
 * This file captures all of the test results, independently of the
 * events generated during the test run.  The file can be used as part
 * of scripted tests and/or capturing test artifacts.
 */
#define CFE_ASSERT_LOG_FILE_NAME "/cf/cfe_test.log"

/* Compare two Resource IDs */
#define cFE_FTAssert_ResourceID_EQ(actual, expect)                                            \
    UtAssert_True(CFE_RESOURCEID_TEST_EQUAL(actual, expect), "%s (%lu) == %s (%lu)", #actual, \
                  CFE_RESOURCEID_TO_ULONG(actual), #expect, CFE_RESOURCEID_TO_ULONG(expect))

/* Check if a Resource ID is Undefined */
#define cFE_FTAssert_ResourceID_Undefined(id) \
    UtAssert_True(!CFE_RESOURCEID_TEST_DEFINED(id), "%s (%lu) not defined", #id, CFE_RESOURCEID_TO_ULONG(id))

/* Assert a return code is not equal to cfe_success */
#define cFE_FTAssert_NOT_CFE_SUCCESS(actual)                                      \
    do                                                                            \
    {                                                                             \
        int32 rcact = (int32)(actual);                                            \
        UtAssert_True(rcact < CFE_SUCCESS, "%s == (%ld) ", #actual, (long)rcact); \
    } while (0)

/* Log calls to void functions */
#define cFE_FTAssert_VOIDCALL(func) (func, UtAssert(true, #func, __FILE__, __LINE__))

void CFE_TestMain(void);
void ESCDSTestSetup(void);
void ESInfoTestSetup(void);
void ESMemPoolTestSetup(void);
void ESMiscTestSetup(void);
void ESTaskTestSetup(void);
void FSHeaderTestSetup(void);
void FSUtilTestSetup(void);
void SBPipeMangSetup(void);
void TimeCurrentTestSetup(void);

#endif /* CFE_TEST_H */
