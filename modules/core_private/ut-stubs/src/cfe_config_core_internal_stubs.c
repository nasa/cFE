/**
 * @file
 *
 * Auto-Generated stub implementations for functions defined in cfe_config_core_internal header
 */

#include "cfe_config_core_internal.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_Config_Init()
 * ----------------------------------------------------
 */
int32 CFE_Config_Init(void)
{
    UT_GenStub_SetupReturnBuffer(CFE_Config_Init, int32);

    UT_GenStub_Execute(CFE_Config_Init, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_Config_Init, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_Config_SetValue()
 * ----------------------------------------------------
 */
void CFE_Config_SetValue(CFE_ConfigId_t ConfigId, uint32 Value)
{
    UT_GenStub_AddParam(CFE_Config_SetValue, CFE_ConfigId_t, ConfigId);
    UT_GenStub_AddParam(CFE_Config_SetValue, uint32, Value);

    UT_GenStub_Execute(CFE_Config_SetValue, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_Config_SetPointer()
 * ----------------------------------------------------
 */
void CFE_Config_SetPointer(CFE_ConfigId_t ConfigId, const void *Ptr)
{
    UT_GenStub_AddParam(CFE_Config_SetPointer, CFE_ConfigId_t, ConfigId);
    UT_GenStub_AddParam(CFE_Config_SetPointer, const void *, Ptr);

    UT_GenStub_Execute(CFE_Config_SetPointer, Basic, NULL);
}
