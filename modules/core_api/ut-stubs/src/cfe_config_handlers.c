/*
** File: ut_config_handlers.c
**
** Purpose:
** Unit test handlers for Configuration Service routines
**
** Notes:
** Mostly implement a NULL default return where needed, as this is not done
** automatically by UT assert.
**
*/

/*
** Includes
*/
#include <string.h>
#include "cfe_config.h"

#include "utstubs.h"
#include "utassert.h"

/*
** Functions
*/

/*------------------------------------------------------------
 *
 * Default handler for CFE_Config_GetString coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_Config_GetString(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    int32      status;
    const char FIXED_BUFFER[] = "UT";
    void *     Buffer;

    Buffer = NULL;
    UT_Stub_GetInt32StatusCode(Context, &status);

    if (status >= 0)
    {
        UT_GetDataBuffer(FuncKey, &Buffer, NULL, NULL);

        /*
         * The public API has a contract _not_ to return a NULL
         * pointer, so this should be mimiced by the UT stub
         */
        if (Buffer == NULL)
        {
            Buffer = (void *)FIXED_BUFFER;
        }
    }

    UT_Stub_SetReturnValue(FuncKey, Buffer);
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_Config_GetObjPointer coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_Config_GetObjPointer(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    int32 status;
    void *Buffer;

    Buffer = NULL;
    UT_Stub_GetInt32StatusCode(Context, &status);

    if (status >= 0)
    {
        UT_GetDataBuffer(FuncKey, &Buffer, NULL, NULL);
    }

    UT_Stub_SetReturnValue(FuncKey, Buffer);
}
