/*
** File: cfe_fs_core_internal_handlers.c
**
** Purpose:
** Unit test stubs for Executive Service routines
**
** Notes:
** Minimal work is done, only what is required for unit testing
**
*/

/*
** Includes
*/
#include <string.h>
#include "cfe_fs_core_internal.h"

#include "utstubs.h"
#include "utassert.h"

/*
** Functions
*/

/*------------------------------------------------------------
 *
 * Default handler for CFE_FS_RunBackgroundFileDump coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_FS_RunBackgroundFileDump(void *UserObj, UT_EntryKey_t FuncKey,
                                                    const UT_StubContext_t *Context)
{
    int32 status;
    bool  return_value;

    UT_Stub_GetInt32StatusCode(Context, &status);

    return_value = status;

    UT_Stub_SetReturnValue(FuncKey, return_value);
}
