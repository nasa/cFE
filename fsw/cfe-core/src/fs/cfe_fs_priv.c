/*
**  GSC-18128-1, "Core Flight Executive Version 6.7"
**
**  Copyright (c) 2006-2019 United States Government as represented by
**  the Administrator of the National Aeronautics and Space Administration.
**  All Rights Reserved.
**
**  Licensed under the Apache License, Version 2.0 (the "License");
**  you may not use this file except in compliance with the License.
**  You may obtain a copy of the License at
**
**    http://www.apache.org/licenses/LICENSE-2.0
**
**  Unless required by applicable law or agreed to in writing, software
**  distributed under the License is distributed on an "AS IS" BASIS,
**  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**  See the License for the specific language governing permissions and
**  limitations under the License.
*/

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
#include "osapi.h"
#include "private/cfe_private.h"
#include "cfe_es.h"
#include "cfe_fs.h"
#include "cfe_fs_priv.h"

#include <string.h>

/*
** Global data
** 
*/
CFE_FS_t CFE_FS;

/******************************************************************************
**  Function:  CFE_FS_EarlyInit()
**
**  Purpose:
**    Initialize the FS data structures before the cFE runs.
**
**  Arguments:
**
**  Notes:
**    This function MUST be called before any FS API's are called.
**
**  Return:
**    CFE_SUCCESS
*/
int32 CFE_FS_EarlyInit (void) 
{
    int32 Stat;

    Stat = OS_MutSemCreate(&CFE_FS.SharedDataMutexId, "CFE_FS_SharedMutex", 0);
    if( Stat != OS_SUCCESS )
    {
      CFE_ES_WriteToSysLog("FS Shared Data Mutex creation failed! RC=0x%08x\n",(unsigned int)Stat);
      return Stat;
    }/* end if */

    return Stat;

}/* end CFE_FS_EarlyInit */

/******************************************************************************
**  Function:  CFE_FS_LockSharedData()
**
**  Purpose:
**    FS internal function to handle a semaphore take failure for the FS 
**    Data Mutex
**
**  Arguments:
**    FunctionName   - the Function Name of the code that generated the error.
**
**  Return:
**    None
*/
void CFE_FS_LockSharedData(const char *FunctionName)
{
    int32   Status;
    CFE_ES_ResourceID_t  AppId;

    Status = OS_MutSemTake(CFE_FS.SharedDataMutexId);
    if (Status != OS_SUCCESS) 
    {
        CFE_ES_GetAppID(&AppId);

        CFE_ES_WriteToSysLog("FS SharedData Mutex Take Err Stat=0x%x,App=%lu,Function=%s\n",
                (unsigned int)Status,CFE_ES_ResourceID_ToInteger(AppId),FunctionName);

    }/* end if */

    return;

}/* end CFE_FS_LockSharedData */

/******************************************************************************
**  Function:  CFE_FS_UnlockSharedData()
**
**  Purpose:
**    FS internal function to handle a semaphore give failure for the Shared
**    Data Mutex
**
**  Arguments:
**    FunctionName - the Function containing the code that generated the error.
**
**  Return:
**    None
*/
void CFE_FS_UnlockSharedData(const char *FunctionName)
{
   int32   Status;
   CFE_ES_ResourceID_t  AppId;

   Status = OS_MutSemGive(CFE_FS.SharedDataMutexId);
   if (Status != OS_SUCCESS) 
   {
       CFE_ES_GetAppID(&AppId);
       CFE_ES_WriteToSysLog("FS SharedData Mutex Give Err Stat=0x%x,App=%lu,Function=%s\n",
               (unsigned int)Status,CFE_ES_ResourceID_ToInteger(AppId),FunctionName);

   }/* end if */
   return;

}/* end CFE_FS_UnlockSharedData */

/************************/
/*  End of File Comment */
/************************/
