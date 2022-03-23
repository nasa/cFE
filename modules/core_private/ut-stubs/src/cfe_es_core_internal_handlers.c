/*
** File: cfe_es_core_internal_handlers.c
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
#include "cfe_es_core_internal.h"
#include "cfe_resourceid.h"
#include "cfe_resourceid_basevalue.h"

#include "utstubs.h"
#include "utassert.h"

/*
 * Assign ID base values for UT
 */
enum
{
    UT_CFE_ES_CDSBLOCKID_BASE = CFE_RESOURCEID_MAKE_BASE(0x26)
};

/*
** Functions
*/

/*------------------------------------------------------------
 *
 * Default handler for CFE_ES_RegisterCDSEx coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_ES_RegisterCDSEx(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_ES_CDSHandle_t *HandlePtr = UT_Hook_GetArgValueByName(Context, "HandlePtr", CFE_ES_CDSHandle_t *);
    int32               status;

    UT_Stub_GetInt32StatusCode(Context, &status);

    if (status >= 0)
    {
        if (UT_Stub_CopyToLocal(UT_KEY(CFE_ES_RegisterCDSEx), HandlePtr, sizeof(*HandlePtr)) < sizeof(*HandlePtr))
        {
            *HandlePtr =
                (CFE_ES_CDSHandle_t)CFE_RESOURCEID_WRAP(CFE_ResourceId_FromInteger(UT_CFE_ES_CDSBLOCKID_BASE + 1));
        }
    }
}
