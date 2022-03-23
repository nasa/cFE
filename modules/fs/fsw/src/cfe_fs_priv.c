/*
** File: cfe_fs_priv.c
**
** Purpose:  cFE File Services (FS) library API Initialization
**
** Author:  Alan Cudmore/NASA GSFC
**
** Notes:
**
*/

/*
** Required header files
*/
#include "cfe_fs_module_all.h"

#include <string.h>

/*
** Global data
**
*/
CFE_FS_Global_t CFE_FS_Global;

/*----------------------------------------------------------------
 *
 * Function: CFE_FS_EarlyInit
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_FS_EarlyInit(void)
{
    int32 OsStatus;

    memset(&CFE_FS_Global, 0, sizeof(CFE_FS_Global));

    OsStatus = OS_MutSemCreate(&CFE_FS_Global.SharedDataMutexId, "CFE_FS_SharedMutex", 0);
    if (OsStatus != OS_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: Shared Data Mutex creation failed! RC=%ld\n", __func__, (long)OsStatus);
        return CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
    } /* end if */

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Function: CFE_FS_LockSharedData
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_FS_LockSharedData(const char *FunctionName)
{
    int32          OsStatus;
    CFE_ES_AppId_t AppId;

    OsStatus = OS_MutSemTake(CFE_FS_Global.SharedDataMutexId);
    if (OsStatus != OS_SUCCESS)
    {
        CFE_ES_GetAppID(&AppId);
        CFE_ES_WriteToSysLog("%s: SharedData Mutex Take Err Stat=%ld,App=%lu,Function=%s\n", __func__, (long)OsStatus,
                             CFE_RESOURCEID_TO_ULONG(AppId), FunctionName);

    } /* end if */

    return;
}

/*----------------------------------------------------------------
 *
 * Function: CFE_FS_UnlockSharedData
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_FS_UnlockSharedData(const char *FunctionName)
{
    int32          OsStatus;
    CFE_ES_AppId_t AppId;

    OsStatus = OS_MutSemGive(CFE_FS_Global.SharedDataMutexId);
    if (OsStatus != OS_SUCCESS)
    {
        CFE_ES_GetAppID(&AppId);
        CFE_ES_WriteToSysLog("%s: SharedData Mutex Give Err Stat=%ld,App=%lu,Function=%s\n", __func__, (long)OsStatus,
                             CFE_RESOURCEID_TO_ULONG(AppId), FunctionName);

    } /* end if */
    return;
}
