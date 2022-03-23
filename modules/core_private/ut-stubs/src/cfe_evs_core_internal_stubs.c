/**
 * @file
 *
 * Auto-Generated stub implementations for functions defined in cfe_evs_core_internal header
 */

#include "cfe_evs_core_internal.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_EVS_CleanUpApp()
 * ----------------------------------------------------
 */
int32 CFE_EVS_CleanUpApp(CFE_ES_AppId_t AppId)
{
    UT_GenStub_SetupReturnBuffer(CFE_EVS_CleanUpApp, int32);

    UT_GenStub_AddParam(CFE_EVS_CleanUpApp, CFE_ES_AppId_t, AppId);

    UT_GenStub_Execute(CFE_EVS_CleanUpApp, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_EVS_CleanUpApp, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_EVS_EarlyInit()
 * ----------------------------------------------------
 */
int32 CFE_EVS_EarlyInit(void)
{
    UT_GenStub_SetupReturnBuffer(CFE_EVS_EarlyInit, int32);

    UT_GenStub_Execute(CFE_EVS_EarlyInit, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_EVS_EarlyInit, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_EVS_TaskMain()
 * ----------------------------------------------------
 */
void CFE_EVS_TaskMain(void)
{

    UT_GenStub_Execute(CFE_EVS_TaskMain, Basic, NULL);
}
