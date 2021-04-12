/*************************************************************************
**
**      GSC-18128-1, "Core Flight Executive Version 6.7"
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
** File: cfe_assert_io.c
**
** Purpose:
**   Implementation of the CFE assert (UT assert wrapper) functions.
**
*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "cfe.h"
#include "cfe_assert_priv.h"

#include "utbsp.h"
#include "uttest.h"

CFE_Assert_Global_t CFE_Assert_Global;

void UT_BSP_Setup(void)
{
    CFE_Assert_Global.CurrVerbosity = (2 << UTASSERT_CASETYPE_PASS) - 1;
    UT_BSP_DoText(UTASSERT_CASETYPE_BEGIN, "CFE FUNCTIONAL TEST");
}

void UT_BSP_StartTestSegment(uint32 SegmentNumber, const char *SegmentName)
{
    char ReportBuffer[128];

    snprintf(ReportBuffer, sizeof(ReportBuffer), "%02u %s", (unsigned int)SegmentNumber, SegmentName);
    UT_BSP_DoText(UTASSERT_CASETYPE_BEGIN, ReportBuffer);
}

void UT_BSP_SysLogStatusReport(uint8 MessageType, const char *Prefix, const char *OutputMessage)
{
    uint32 MsgEnabled = CFE_Assert_Global.CurrVerbosity >> MessageType;

    if (MsgEnabled & 1)
    {
        CFE_ES_WriteToSysLog("[%5s] %s\n", Prefix, OutputMessage);
    }
}

void UT_BSP_DoText(uint8 MessageType, const char *OutputMessage)
{
    const char *                Prefix;
    CFE_Assert_StatusCallback_t StatusCallback;

    switch (MessageType)
    {
        case UTASSERT_CASETYPE_ABORT:
            Prefix = "ABORT";
            break;
        case UTASSERT_CASETYPE_FAILURE:
            Prefix = "FAIL";
            break;
        case UTASSERT_CASETYPE_MIR:
            Prefix = "MIR";
            break;
        case UTASSERT_CASETYPE_TSF:
            Prefix = "TSF";
            break;
        case UTASSERT_CASETYPE_TTF:
            Prefix = "TTF";
            break;
        case UTASSERT_CASETYPE_NA:
            Prefix = "N/A";
            break;
        case UTASSERT_CASETYPE_BEGIN:
            Prefix = "BEGIN";
            break;
        case UTASSERT_CASETYPE_END:
            Prefix = "END";
            break;
        case UTASSERT_CASETYPE_PASS:
            Prefix = "PASS";
            break;
        case UTASSERT_CASETYPE_INFO:
            Prefix = "INFO";
            break;
        case UTASSERT_CASETYPE_DEBUG:
            Prefix = "DEBUG";
            break;
        default:
            Prefix = "OTHER";
            break;
    }

    StatusCallback = CFE_Assert_Global.StatusCallback;

    /* If not set, report status to CFE ES Syslog facility */
    if (StatusCallback == NULL)
    {
        StatusCallback = UT_BSP_SysLogStatusReport;
    }

    StatusCallback(MessageType, Prefix, OutputMessage);

    /*
     * If any ABORT (major failure) message is thrown,
     * then call a BSP-provided routine to stop the test and possibly dump a core
     */
    if (MessageType == UTASSERT_CASETYPE_ABORT)
    {
        OS_TaskExit();
    }
}

void UT_BSP_EndTest(const UtAssert_TestCounter_t *TestCounters)
{
    /*
     * Only output a "summary" if there is more than one test Segment.
     * Otherwise it is a duplicate of the report already given.
     */
    if (TestCounters->TestSegmentCount > 1)
    {
        UtAssert_DoTestSegmentReport("SUMMARY", TestCounters);
    }

    CFE_ES_WriteToSysLog("TEST COMPLETE: %u tests Segment(s) executed\n\n",
                         (unsigned int)TestCounters->TestSegmentCount);

    OS_TaskExit();
}
