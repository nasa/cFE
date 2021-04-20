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
**   Functional test of basic ES Critical Data Store APIs
**
**   Demonstration of how to register and use the UT assert functions.
**
*************************************************************************/

/*
 * Includes
 */

#include "cfe_test.h"

void TestCDS(void)
{
    CFE_ES_CDSHandle_t CDSHandlePtr;
    size_t             BlockSize = 10;
    const char *       Name      = "CDS_Testing";
    CFE_ES_CDSHandle_t IdByName;
    char               CDSNameBuf[CFE_MISSION_ES_CDS_MAX_NAME_LENGTH];

    UtPrintf("Testing: CFE_ES_RegisterCDS, CFE_ES_GetCDSBlockIDByName, CFE_ES_GetCDSBlockName");

    UtAssert_INT32_EQ(CFE_ES_RegisterCDS(&CDSHandlePtr, BlockSize, Name), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_GetCDSBlockIDByName(&IdByName, Name), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_GetCDSBlockName(CDSNameBuf, IdByName, sizeof(CDSNameBuf)), CFE_SUCCESS);
    UtAssert_StrCmp(CDSNameBuf, Name, "CFE_ES_GetCDSBlockName() = %s", CDSNameBuf);
}

int32 ESCDSTestSetup(int32 LibId)
{
    UtTest_Add(TestCDS, NULL, NULL, "Test CDS");

    return CFE_SUCCESS;
}
