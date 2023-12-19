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
 *   Create a file containing a CFE Test Table
 */

/*
 * Includes
 */

#include "cfe_tbl_filedef.h"
#include "cfe_test_tbl.h"

/*
 * The test table data should contain some identifiable numeric values,
 * so any issues with paritial loading/byteswapping are morely likely
 * to be detected.
 */
CFE_TEST_TestTable_t TestTable = {0xf007, 0xba11};

CFE_TBL_FILEDEF(TestTable, CFE_TEST_APP.TestTable, Table Test Table, cfe_test_tbl.tbl)
