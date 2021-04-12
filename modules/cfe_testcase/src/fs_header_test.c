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
** File: es_info_test.c
**
** Purpose:
**   Functional test of basic ES Information APIs
**
**   Demonstration of how to register and use the UT assert functions.
**
*************************************************************************/

/*
 * Includes
 */

#include "cfe_test.h"

void TestHeader(void)
{
    UtPrintf("Testing: CFE_FS_ReadHeader, CFE_FS_InitHeader, CFE_FS_WriteHeader, CFE_FS_SetTimestamp");

    UtAssert_INT32_EQ(2, 2);
}

int32 FSHeaderTestSetup(int32 LibId)
{
    UtTest_Add(TestHeader, NULL, NULL, "Test Header");

    return CFE_SUCCESS;
}
