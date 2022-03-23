/**
 * @file
 *
 * Auto-Generated stub implementations for functions defined in cfe_sb_core_internal header
 */

#include "cfe_sb_core_internal.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_SB_CleanUpApp()
 * ----------------------------------------------------
 */
int32 CFE_SB_CleanUpApp(CFE_ES_AppId_t AppId)
{
    UT_GenStub_SetupReturnBuffer(CFE_SB_CleanUpApp, int32);

    UT_GenStub_AddParam(CFE_SB_CleanUpApp, CFE_ES_AppId_t, AppId);

    UT_GenStub_Execute(CFE_SB_CleanUpApp, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_SB_CleanUpApp, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_SB_EarlyInit()
 * ----------------------------------------------------
 */
int32 CFE_SB_EarlyInit(void)
{
    UT_GenStub_SetupReturnBuffer(CFE_SB_EarlyInit, int32);

    UT_GenStub_Execute(CFE_SB_EarlyInit, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_SB_EarlyInit, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_SB_TaskMain()
 * ----------------------------------------------------
 */
void CFE_SB_TaskMain(void)
{

    UT_GenStub_Execute(CFE_SB_TaskMain, Basic, NULL);
}
