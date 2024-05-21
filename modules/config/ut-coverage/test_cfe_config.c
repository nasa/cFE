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

/*
 * Includes
 */
#include "cfe_config.h"
#include "cfe_config_priv.h"
#include "cfe_config_nametable.h"

#include "utassert.h"
#include "utstubs.h"
#include "uttest.h"

#include "ut_support.h"

/* clang-format off */
const CFE_Config_IdNameEntry_t CFE_CONFIGID_NAMETABLE[CFE_ConfigIdOffset_MAX] =
{
    {"UT_CHECK_1"},
    {"UT_CHECK_2"},
    {"UT_CHECK_3"},
    {"UT_CHECK_4"},
    {"MOD_SRCVER_COREMODULE1"},
    {"MOD_SRCVER_COREMODULE2"}
};
/* clang-format on */

#define CFE_CONFIGID_UT_CHECK_1 CFE_CONFIGID_C(CFE_RESOURCEID_WRAP(CFE_CONFIGID_BASE + 0))
#define CFE_CONFIGID_UT_CHECK_2 CFE_CONFIGID_C(CFE_RESOURCEID_WRAP(CFE_CONFIGID_BASE + 1))
#define CFE_CONFIGID_UT_CHECK_3 CFE_CONFIGID_C(CFE_RESOURCEID_WRAP(CFE_CONFIGID_BASE + 2))
#define CFE_CONFIGID_UT_CHECK_4 CFE_CONFIGID_C(CFE_RESOURCEID_WRAP(CFE_CONFIGID_BASE + 3))
#define CFE_CONFIGID_UT_CHECK_5 CFE_CONFIGID_C(CFE_RESOURCEID_WRAP(CFE_CONFIGID_BASE + 4))

const char UT_UNKNOWN_STR[] = "UT-Unknown";
const char UT_VALUE_STR[]   = "UT-Value";

const uint32                  UT_ARR_ELEMENTS[4] = {1, 2, 3, 4};
const CFE_Config_ArrayValue_t UT_ARRAY           = {4, UT_ARR_ELEMENTS};

const struct
{
    uint16 val;
} UT_TEST_OBJ = {456};

CFE_StaticModuleLoadEntry_t UT_PspModuleListSet[3] = {{"pspmodule1"}, {"pspmodule2"}, {NULL}};
CFE_ConfigName_t            UT_StaticModuleSet[3]  = {{"staticmodule1"}, {"staticmodule2"}, {NULL}};
CFE_ConfigName_t            UT_CoreModuleSet[3]    = {{"coremodule1"}, {"coremodule2"}, {NULL}};
CFE_ConfigName_t *          UT_ModuleListSet[2]    = {UT_CoreModuleSet, UT_StaticModuleSet};
CFE_ConfigKeyValue_t        UT_ActiveList[3]       = {{"coremodule1", "ut1"}, {"staticmodule2", "ut2"}, {NULL, NULL}};

void Test_CFE_Config_GetValue(void)
{
    /*
     * Test case for:
     * uint32 CFE_Config_GetValue(CFE_ConfigId_t ConfigId)
     */
    UtAssert_UINT32_EQ(CFE_Config_GetValue(CFE_CONFIGID_UNDEFINED), 0);
    UtAssert_UINT32_EQ(CFE_Config_GetValue(CFE_CONFIGID_UT_CHECK_1), 123);
    UtAssert_UINT32_EQ(CFE_Config_GetValue(CFE_CONFIGID_UT_CHECK_2), 0);
    UtAssert_UINT32_EQ(CFE_Config_GetValue(CFE_CONFIGID_UT_CHECK_3), 0);
    UtAssert_UINT32_EQ(CFE_Config_GetValue(CFE_CONFIGID_UT_CHECK_4), 0);
}

void Test_CFE_Config_GetObjPointer(void)
{
    /*
     * Test case for:
     * const void *CFE_Config_GetObjPointer(CFE_ConfigId_t ConfigId)
     */
    UtAssert_NULL(CFE_Config_GetObjPointer(CFE_CONFIGID_UNDEFINED));
    UtAssert_NULL(CFE_Config_GetObjPointer(CFE_CONFIGID_UT_CHECK_1));
    UtAssert_ADDRESS_EQ(CFE_Config_GetObjPointer(CFE_CONFIGID_UT_CHECK_2), &UT_TEST_OBJ);
    UtAssert_ADDRESS_EQ(CFE_Config_GetObjPointer(CFE_CONFIGID_UT_CHECK_3), UT_VALUE_STR);
    UtAssert_NULL(CFE_Config_GetObjPointer(CFE_CONFIGID_UT_CHECK_4));
}

void Test_CFE_Config_GetArrayValue(void)
{
    /*
     * Test case for:
     * CFE_Config_ArrayValue_t CFE_Config_GetArrayValue(CFE_ConfigId_t ConfigId)
     */

    CFE_Config_ArrayValue_t ArrVal;

    ArrVal = CFE_Config_GetArrayValue(CFE_CONFIGID_UNDEFINED);
    UtAssert_ZERO(ArrVal.NumElements);
    UtAssert_NULL(ArrVal.ElementPtr);

    ArrVal = CFE_Config_GetArrayValue(CFE_CONFIGID_UT_CHECK_1);
    UtAssert_ZERO(ArrVal.NumElements);
    UtAssert_NULL(ArrVal.ElementPtr);

    ArrVal = CFE_Config_GetArrayValue(CFE_CONFIGID_UT_CHECK_2);
    UtAssert_ZERO(ArrVal.NumElements);
    UtAssert_NULL(ArrVal.ElementPtr);

    ArrVal = CFE_Config_GetArrayValue(CFE_CONFIGID_UT_CHECK_3);
    UtAssert_ZERO(ArrVal.NumElements);
    UtAssert_NULL(ArrVal.ElementPtr);

    ArrVal = CFE_Config_GetArrayValue(CFE_CONFIGID_UT_CHECK_4);
    UtAssert_ZERO(ArrVal.NumElements);
    UtAssert_NULL(ArrVal.ElementPtr);

    ArrVal = CFE_Config_GetArrayValue(CFE_CONFIGID_UT_CHECK_5);
    UtAssert_EQ(size_t, ArrVal.NumElements, UT_ARRAY.NumElements);
    UtAssert_ADDRESS_EQ(ArrVal.ElementPtr, UT_ARRAY.ElementPtr);
}

void Test_CFE_Config_GetString(void)
{
    /*
     * Test case for:
     * const char *CFE_Config_GetString(CFE_ConfigId_t ConfigId)
     */
    UtAssert_ADDRESS_EQ(CFE_Config_GetString(CFE_CONFIGID_UNDEFINED), UT_UNKNOWN_STR);
    UtAssert_ADDRESS_EQ(CFE_Config_GetString(CFE_CONFIGID_UT_CHECK_1), UT_UNKNOWN_STR);
    UtAssert_ADDRESS_EQ(CFE_Config_GetString(CFE_CONFIGID_UT_CHECK_2), UT_UNKNOWN_STR);
    UtAssert_ADDRESS_EQ(CFE_Config_GetString(CFE_CONFIGID_UT_CHECK_3), UT_VALUE_STR);
    UtAssert_ADDRESS_EQ(CFE_Config_GetString(CFE_CONFIGID_UT_CHECK_4), UT_UNKNOWN_STR);
}

void Test_CFE_Config_SetValue(void)
{
    /*
     * Test case for:
     * void CFE_Config_SetValue(CFE_ConfigId_t ConfigId, uint32 Value)
     */
    UtAssert_VOIDCALL(CFE_Config_SetValue(CFE_CONFIGID_UNDEFINED, 789));
    UtAssert_VOIDCALL(CFE_Config_SetValue(CFE_CONFIGID_UT_CHECK_4, 789));
    UtAssert_UINT32_EQ(CFE_Config_GetValue(CFE_CONFIGID_UNDEFINED), 0);
    UtAssert_UINT32_EQ(CFE_Config_GetValue(CFE_CONFIGID_UT_CHECK_4), 789);
}

void Test_CFE_Config_SetObjPointer(void)
{
    /*
     * Test case for:
     * void CFE_Config_SetObjPointer(CFE_ConfigId_t ConfigId, const void *Ptr)
     */
    static const char TESTOBJ[] = "TEST-OBJ";

    UtAssert_VOIDCALL(CFE_Config_SetObjPointer(CFE_CONFIGID_UNDEFINED, TESTOBJ));
    UtAssert_VOIDCALL(CFE_Config_SetObjPointer(CFE_CONFIGID_UT_CHECK_4, TESTOBJ));
    UtAssert_NULL(CFE_Config_GetObjPointer(CFE_CONFIGID_UNDEFINED));
    UtAssert_ADDRESS_EQ(CFE_Config_GetObjPointer(CFE_CONFIGID_UT_CHECK_4), TESTOBJ);
}

void Test_CFE_Config_SetArrayValue(void)
{
    /*
     * Test case for:
     * void CFE_Config_SetArrayValue(CFE_ConfigId_t ConfigId, const CFE_Config_ArrayValue_t *ArrayPtr)
     */
    static uint16                  TESTARR[] = {5, 6, 7, 8, 9, 10};
    static CFE_Config_ArrayValue_t TESTOBJ   = {6, TESTARR};
    CFE_Config_ArrayValue_t        ArrVal;

    UtAssert_VOIDCALL(CFE_Config_SetArrayValue(CFE_CONFIGID_UNDEFINED, &TESTOBJ));
    UtAssert_VOIDCALL(CFE_Config_SetArrayValue(CFE_CONFIGID_UT_CHECK_2, &TESTOBJ));

    ArrVal = CFE_Config_GetArrayValue(CFE_CONFIGID_UNDEFINED);
    UtAssert_ZERO(ArrVal.NumElements);
    UtAssert_NULL(ArrVal.ElementPtr);

    ArrVal = CFE_Config_GetArrayValue(CFE_CONFIGID_UT_CHECK_2);
    UtAssert_EQ(size_t, ArrVal.NumElements, TESTOBJ.NumElements);
    UtAssert_ADDRESS_EQ(ArrVal.ElementPtr, TESTOBJ.ElementPtr);
}

void Test_CFE_Config_SetString(void)
{
    /*
     * Test case for:
     * void CFE_Config_SetString(CFE_ConfigId_t ConfigId, const char *Ptr)
     */
    static const char TESTSTR[] = "TEST-STRING";

    UtAssert_VOIDCALL(CFE_Config_SetString(CFE_CONFIGID_UNDEFINED, TESTSTR));
    UtAssert_VOIDCALL(CFE_Config_SetString(CFE_CONFIGID_UT_CHECK_4, TESTSTR));
    UtAssert_ADDRESS_EQ(CFE_Config_GetString(CFE_CONFIGID_UNDEFINED), UT_UNKNOWN_STR);
    UtAssert_ADDRESS_EQ(CFE_Config_GetString(CFE_CONFIGID_UT_CHECK_4), TESTSTR);
}

void Test_CFE_Config_GetName(void)
{
    /*
     * Test case for:
     * const char *CFE_Config_GetName(CFE_ConfigId_t ConfigId)
     */
    UtAssert_STRINGBUF_EQ(CFE_Config_GetName(CFE_CONFIGID_UT_CHECK_1), -1, "UT_CHECK_1", -1);
    UtAssert_STRINGBUF_EQ(CFE_Config_GetName(CFE_CONFIGID_UNDEFINED), -1, UT_UNKNOWN_STR, -1);
}

void Test_CFE_Config_GetIdByName(void)
{
    /*
     * Test case for:
     * CFE_ConfigId_t CFE_Config_GetIdByName(const char *Name)
     */
    CFE_UtAssert_RESOURCEID_EQ(CFE_Config_GetIdByName("UT_CHECK_2"), CFE_CONFIGID_UT_CHECK_2);
    CFE_UtAssert_RESOURCEID_EQ(CFE_Config_GetIdByName("INVALID"), CFE_CONFIGID_UNDEFINED);
}

void UT_Callback(void *Arg, CFE_ConfigId_t Id, const char *Name)
{
    uint32 *Count = Arg;

    ++(*Count);

    UtAssert_NOT_NULL(Name);
}

void Test_CFE_Config_IterateAll(void)
{
    /*
     * Test case for:
     * void CFE_Config_IterateAll(void *Arg, CFE_Config_Callback_t Callback)
     */
    uint32 Count;

    Count = 0;
    CFE_Config_IterateAll(&Count, UT_Callback);

    /* Callback should only be given for configured/set entities */
    UtAssert_UINT32_EQ(Count, 4);
}

void Test_CFE_Config_StrCaseEq(void)
{
    /*
     * Test case for:
     * bool CFE_Config_StrCaseEq(const char *c1, const char *c2)
     */
    UtAssert_BOOL_FALSE(CFE_Config_StrCaseEq("xx", "yy"));
    UtAssert_BOOL_TRUE(CFE_Config_StrCaseEq("STR", "STR"));
    UtAssert_BOOL_TRUE(CFE_Config_StrCaseEq("STR", "str"));
    UtAssert_BOOL_TRUE(CFE_Config_StrCaseEq("StR", "sTr"));
    UtAssert_BOOL_FALSE(CFE_Config_StrCaseEq("St", "sTr"));
    UtAssert_BOOL_FALSE(CFE_Config_StrCaseEq("StR", "sT"));
}

void Test_CFE_Config_FindTargetKeyValue(void)
{
    /*
     * Test case for:
     * const char *CFE_Config_FindTargetKeyValue(const CFE_ConfigKeyValue_t *ConfigList, const char *KeyName)
     */

    UtAssert_NOT_NULL(CFE_Config_FindTargetKeyValue(UT_ActiveList, "staticmodule2"));
    UtAssert_NULL(CFE_Config_FindTargetKeyValue(UT_ActiveList, "nonexisting"));
    UtAssert_NULL(CFE_Config_FindTargetKeyValue(UT_ActiveList, NULL));
    UtAssert_NULL(CFE_Config_FindTargetKeyValue(NULL, "staticmodule2"));
}

void Test_CFE_Config_IsPspModule(void)
{
    /*
     * Test case for:
     * const char *CFE_Config_IsPspModule(const char *ModuleName)
     */
    UtAssert_STRINGBUF_EQ(CFE_Config_IsPspModule(UT_PspModuleListSet, "pspmodule1"), -1, "pspmodule1", -1);
    UtAssert_STRINGBUF_EQ(CFE_Config_IsPspModule(UT_PspModuleListSet, "pspmodule2"), -1, "pspmodule2", -1);
    UtAssert_NULL(CFE_Config_IsPspModule(UT_PspModuleListSet, "notapspmodule"));
    UtAssert_NULL(CFE_Config_IsPspModule(NULL, "notapspmodule"));
}

void Test_CFE_Config_FindStaticModName(void)
{
    /*
     * Test case for:
     * const char *CFE_Config_FindStaticModName(const char *ModuleName)
     */
    UtAssert_STRINGBUF_EQ(CFE_Config_FindStaticModName(UT_ModuleListSet, 2, "staticmodule2"), -1, "staticmodule2", -1);
    UtAssert_STRINGBUF_EQ(CFE_Config_FindStaticModName(UT_ModuleListSet, 2, "coremodule2"), -1, "coremodule2", -1);
    UtAssert_NULL(CFE_Config_FindStaticModName(UT_ModuleListSet, 2, "notastaticmodule"));
}

void Test_CFE_Config_SetupModuleVersions(void)
{
    /*
     * Test case for:
     * void CFE_Config_SetupModuleVersions(void)
     */
    UtAssert_VOIDCALL(CFE_Config_SetupModuleVersions(UT_ModuleListSet, 2, UT_ActiveList));
}

void Test_CFE_Config_Init(void)
{
    /*
     * Test case for:
     * int32 CFE_Config_Init(void)
     */
    UtAssert_INT32_EQ(CFE_Config_Init(), CFE_SUCCESS);
}

void Test_CFE_Config_Setup(void)
{
    memset(&CFE_Config_Global, 0, sizeof(CFE_Config_Global));

    CFE_Config_Global.UnknownString = UT_UNKNOWN_STR;

    /* Set up one entry of each type */
    CFE_Config_Global.Table[0].ActualType      = CFE_ConfigType_VALUE;
    CFE_Config_Global.Table[0].Datum.AsInteger = 123;
    CFE_Config_Global.Table[1].ActualType      = CFE_ConfigType_POINTER;
    CFE_Config_Global.Table[1].Datum.AsPointer = &UT_TEST_OBJ;
    CFE_Config_Global.Table[2].ActualType      = CFE_ConfigType_STRING;
    CFE_Config_Global.Table[2].Datum.AsPointer = UT_VALUE_STR;
    CFE_Config_Global.Table[3].ActualType      = CFE_ConfigType_UNDEFINED;
    CFE_Config_Global.Table[3].Datum.AsPointer = NULL;
    CFE_Config_Global.Table[4].ActualType      = CFE_ConfigType_ARRAY;
    CFE_Config_Global.Table[4].Datum.AsPointer = &UT_ARRAY;

    /* NOTE: Leaves last entry unset */
}

void UtTest_Setup(void)
{
    UtTest_Add(Test_CFE_Config_GetValue, Test_CFE_Config_Setup, NULL, "Test CFE_Config_GetValue()");
    UtTest_Add(Test_CFE_Config_GetObjPointer, Test_CFE_Config_Setup, NULL, "Test CFE_Config_GetObjPointer()");
    UtTest_Add(Test_CFE_Config_GetArrayValue, Test_CFE_Config_Setup, NULL, "Test CFE_Config_GetArrayValue()");
    UtTest_Add(Test_CFE_Config_GetString, Test_CFE_Config_Setup, NULL, "Test CFE_Config_GetString()");
    UtTest_Add(Test_CFE_Config_SetValue, Test_CFE_Config_Setup, NULL, "Test CFE_Config_SetValue()");
    UtTest_Add(Test_CFE_Config_SetObjPointer, Test_CFE_Config_Setup, NULL, "Test CFE_Config_SetObjPointer()");
    UtTest_Add(Test_CFE_Config_SetString, Test_CFE_Config_Setup, NULL, "Test CFE_Config_SetString()");
    UtTest_Add(Test_CFE_Config_SetArrayValue, Test_CFE_Config_Setup, NULL, "Test CFE_Config_SetArrayValue()");

    UtTest_Add(Test_CFE_Config_GetName, Test_CFE_Config_Setup, NULL, "Test CFE_Config_GetName()");
    UtTest_Add(Test_CFE_Config_GetIdByName, Test_CFE_Config_Setup, NULL, "Test CFE_Config_GetIdByName()");
    UtTest_Add(Test_CFE_Config_IterateAll, Test_CFE_Config_Setup, NULL, "Test CFE_Config_IterateAll()");
    UtTest_Add(Test_CFE_Config_StrCaseEq, Test_CFE_Config_Setup, NULL, "Test CFE_Config_StrCaseEq()");
    UtTest_Add(Test_CFE_Config_FindTargetKeyValue, Test_CFE_Config_Setup, NULL, "Test CFE_Config_FindTargetKeyValue()");
    UtTest_Add(Test_CFE_Config_IsPspModule, Test_CFE_Config_Setup, NULL, "Test CFE_Config_IsPspModule()");
    UtTest_Add(Test_CFE_Config_FindStaticModName, Test_CFE_Config_Setup, NULL, "Test CFE_Config_FindStaticModName()");
    UtTest_Add(Test_CFE_Config_SetupModuleVersions, Test_CFE_Config_Setup, NULL,
               "Test CFE_Config_SetupModuleVersions()");
    UtTest_Add(Test_CFE_Config_Init, Test_CFE_Config_Setup, NULL, "Test CFE_Config_Init()");
}
