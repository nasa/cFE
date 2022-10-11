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
 *   Initialization of variables used by table functional tests and
 *   function definitions for setup and teardown table functions
 */

/*
 * Includes
 */

#include "cfe_test.h"
#include "cfe_test_table.h"

/* Setup function to register a table */
void RegisterTestTable(void)
{
    UtAssert_INT32_EQ(CFE_TBL_Register(&CFE_FT_Global.TblHandle, CFE_FT_Global.TblName, sizeof(TBL_TEST_Table_t),
                                       CFE_TBL_OPT_DEFAULT, NULL),
                      CFE_SUCCESS);
}

/* Teardown function to unregister a table */
void UnregisterTestTable(void)
{
    UtAssert_INT32_EQ(CFE_TBL_Unregister(CFE_FT_Global.TblHandle), CFE_SUCCESS);
}
