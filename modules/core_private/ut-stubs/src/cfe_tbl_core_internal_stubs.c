/**
 * @file
 *
 * Auto-Generated stub implementations for functions defined in cfe_tbl_core_internal header
 */

#include "cfe_tbl_core_internal.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_TBL_CleanUpApp()
 * ----------------------------------------------------
 */
int32 CFE_TBL_CleanUpApp(CFE_ES_AppId_t AppId)
{
    UT_GenStub_SetupReturnBuffer(CFE_TBL_CleanUpApp, int32);

    UT_GenStub_AddParam(CFE_TBL_CleanUpApp, CFE_ES_AppId_t, AppId);

    UT_GenStub_Execute(CFE_TBL_CleanUpApp, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_TBL_CleanUpApp, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_TBL_EarlyInit()
 * ----------------------------------------------------
 */
int32 CFE_TBL_EarlyInit(void)
{
    UT_GenStub_SetupReturnBuffer(CFE_TBL_EarlyInit, int32);

    UT_GenStub_Execute(CFE_TBL_EarlyInit, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_TBL_EarlyInit, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_TBL_TaskMain()
 * ----------------------------------------------------
 */
void CFE_TBL_TaskMain(void)
{

    UT_GenStub_Execute(CFE_TBL_TaskMain, Basic, NULL);
}
