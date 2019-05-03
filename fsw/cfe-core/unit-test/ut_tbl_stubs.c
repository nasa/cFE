/*
**
**      GSC-18128-1, "Core Flight Executive Version 6.6"
**
**      Copyright (c) 2006-2019 United States Government as represented by
**      the Administrator of the National Aeronautics and Space Administration.
**      All Rights Reserved.
**
**      Licensed under the Apache License, Version 2.0 (the "License");
**      you may not use this file except in compliance with the License.
**      You may obtain a copy of the License at
**
**        http://www.apache.org/licenses/LICENSE-2.0
**
**      Unless required by applicable law or agreed to in writing, software
**      distributed under the License is distributed on an "AS IS" BASIS,
**      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**      See the License for the specific language governing permissions and
**      limitations under the License.
**
*/

/*
** Includes
*/
#include <string.h>
#include "cfe.h"
#include "common_types.h"
#include "ut_stubs.h"

/*
** External global variables
*/
extern UT_SetRtn_t TBLEarlyInitRtn;

/*
** Functions
*/
/*****************************************************************************/
/**
** \brief CFE_TBL_EarlyInit stub function
**
** \par Description
**        This function is used to mimic the response of the cFE TBL function
**        CFE_TBL_EarlyInit.  The user can adjust the response by setting
**        the values in the TBLEarlyInitRtn structure prior to this function
**        being called.  If the value TBLEarlyInitRtn.count is non-zero then
**        the return value is set to -1.  CFE_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either -1 or CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_TBL_EarlyInit(void)
{
    int32 status = CFE_SUCCESS;

    if (TBLEarlyInitRtn.count != 0)
    {
        status = -1;
    }

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_TBL_TaskMain stub function
**
** \par Description
**        This function is used as a placeholder for the cFE TBL function
**        CFE_TBL_TaskMain.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void CFE_TBL_TaskMain(void)
{
}

/*****************************************************************************/
/**
** \brief CFE_TBL_CleanUpApp stub function
**
** \par Description
**        This function is used to mimic the response of the cFE TBL function
**        CFE_TBL_CleanUpApp.  It always returns CFE_SUCCESS when called.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_TBL_CleanUpApp(uint32 AppId)
{
    return CFE_SUCCESS;
}
