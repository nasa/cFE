/**
 * @file
 *
 * Auto-Generated stub implementations for functions defined in cfe_time_core_internal header
 */

#include "cfe_time_core_internal.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_TIME_CleanUpApp()
 * ----------------------------------------------------
 */
int32 CFE_TIME_CleanUpApp(CFE_ES_AppId_t AppId)
{
    UT_GenStub_SetupReturnBuffer(CFE_TIME_CleanUpApp, int32);

    UT_GenStub_AddParam(CFE_TIME_CleanUpApp, CFE_ES_AppId_t, AppId);

    UT_GenStub_Execute(CFE_TIME_CleanUpApp, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_TIME_CleanUpApp, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_TIME_EarlyInit()
 * ----------------------------------------------------
 */
int32 CFE_TIME_EarlyInit(void)
{
    UT_GenStub_SetupReturnBuffer(CFE_TIME_EarlyInit, int32);

    UT_GenStub_Execute(CFE_TIME_EarlyInit, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_TIME_EarlyInit, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_TIME_TaskMain()
 * ----------------------------------------------------
 */
void CFE_TIME_TaskMain(void)
{

    UT_GenStub_Execute(CFE_TIME_TaskMain, Basic, NULL);
}
