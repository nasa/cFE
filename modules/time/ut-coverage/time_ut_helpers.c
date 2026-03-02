/************************************************************************
 * NASA Docket No. GSC-19,200-1, and identified as "cFS Draco"
 *
 * Copyright (c) 2023 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

/*
** File:
**    time_UT.c
**
** Purpose:
**    Time Services unit test
**
** References:
**    1. cFE Application Developers Guide
**    2. unit test standard 092503
**    3. C Coding Standard 102904
**
** Notes:
**    1. This is unit test code only, not for use in flight
**
*/
#include "time_ut_helpers.h"

#if (CFE_PLATFORM_TIME_CFG_SIGNAL == true)
/*****************************************************************************/
/**
** \brief OS_SelectTone stub function
**
** \par Description
**        This function is used as a placeholder for the OS API function
**        OS_SelectTone.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void OS_SelectTone(int16 Signal) {}
#endif

/*
 * A hook function for CFE_PSP_GetTime that updates the Reference State.
 * This mimics what would happen if a time update occurred at the moment
 * another task was reading the time.
 */
int32 UT_TimeRefUpdateHook(void *UserObj, int32 StubRetcode, uint32 CallCount, const UT_StubContext_t *Context)
{
    volatile CFE_TIME_ReferenceState_t *RefState;
    uint32 *                            UpdateCount = UserObj;
    uint32                              i;

    /*
     * NOTE: in order to trigger a read retry, this actually needs to do CFE_TIME_REFERENCE_BUF_DEPTH
     * updates, such that the buffer being read is overwritten.
     */
    if (*UpdateCount > 0)
    {
        for (i = 0; i < CFE_TIME_REFERENCE_BUF_DEPTH; ++i)
        {
            RefState                      = CFE_TIME_StartReferenceUpdate();
            RefState->AtToneLatch.Seconds = 1 + CallCount;
            RefState->ClockSetState       = CFE_TIME_SetState_WAS_SET;
            CFE_TIME_FinishReferenceUpdate(RefState);
        }
        --(*UpdateCount);
    }

    return StubRetcode;
}
