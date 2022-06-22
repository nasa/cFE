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
 * \file
 *   Functional test of basic ES Error APIs
 */

/*
 * Includes
 */

#include "cfe_test.h"

void TestStatusToString_Helper(CFE_Status_t status)
{
    CFE_StatusString_t status_string;
    char *             rtn_addr;
    char               expected[CFE_STATUS_STRING_LENGTH + 1];

    /* Used oversized string to test for truncation */
    snprintf(expected, sizeof(expected), "0x%08x", (unsigned int)status);
    rtn_addr = CFE_ES_StatusToString(status, &status_string);
    UtAssert_ADDRESS_EQ(rtn_addr, status_string);
    UtAssert_STRINGBUF_EQ(status_string, sizeof(status_string), expected, sizeof(expected));
}

void TestStatusToString(void)
{
    /* NULL test */
    UtAssert_ADDRESS_EQ(CFE_ES_StatusToString(CFE_SUCCESS, NULL), NULL);

    /* Status value tests */
    TestStatusToString_Helper(CFE_SUCCESS);
    TestStatusToString_Helper(CFE_SEVERITY_ERROR);
    TestStatusToString_Helper(CFE_STATUS_C(INT32_MAX));
    TestStatusToString_Helper(CFE_STATUS_C(INT32_MIN));
}

void ESErrorTestSetup(void)
{
    UtTest_Add(TestStatusToString, NULL, NULL, "TestStatusToString");
}
