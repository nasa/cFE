/**
 * @file
 *
 * Auto-Generated stub implementations for functions defined in cfe_fs_core_internal header
 */

#include "cfe_fs_core_internal.h"
#include "utgenstub.h"

void UT_DefaultHandler_CFE_FS_RunBackgroundFileDump(void *, UT_EntryKey_t, const UT_StubContext_t *);

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_FS_EarlyInit()
 * ----------------------------------------------------
 */
int32 CFE_FS_EarlyInit(void)
{
    UT_GenStub_SetupReturnBuffer(CFE_FS_EarlyInit, int32);

    UT_GenStub_Execute(CFE_FS_EarlyInit, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_FS_EarlyInit, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_FS_RunBackgroundFileDump()
 * ----------------------------------------------------
 */
bool CFE_FS_RunBackgroundFileDump(uint32 ElapsedTime, void *Arg)
{
    UT_GenStub_SetupReturnBuffer(CFE_FS_RunBackgroundFileDump, bool);

    UT_GenStub_AddParam(CFE_FS_RunBackgroundFileDump, uint32, ElapsedTime);
    UT_GenStub_AddParam(CFE_FS_RunBackgroundFileDump, void *, Arg);

    UT_GenStub_Execute(CFE_FS_RunBackgroundFileDump, Basic, UT_DefaultHandler_CFE_FS_RunBackgroundFileDump);

    return UT_GenStub_GetReturnValue(CFE_FS_RunBackgroundFileDump, bool);
}
