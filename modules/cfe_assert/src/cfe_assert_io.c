/**
 * \file
 *   Implementation of the CFE assert (UT assert wrapper) functions.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "cfe.h"
#include "cfe_assert_priv.h"

#include "utbsp.h"
#include "uttest.h"

CFE_Assert_Global_t CFE_Assert_Global;

void UT_BSP_Lock(void)
{
    int32 OsStatus;

    OsStatus = OS_MutSemTake(CFE_Assert_Global.AccessMutex);
    if (OsStatus != OS_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s(): Error from OS_MutSemTake(): %ld\n", __func__, (long)OsStatus);
    }
}

void UT_BSP_Unlock(void)
{
    int32 OsStatus;

    OsStatus = OS_MutSemGive(CFE_Assert_Global.AccessMutex);
    if (OsStatus != OS_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s(): Error from OS_MutSemGive(): %ld\n", __func__, (long)OsStatus);
    }
}

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

void UT_BSP_WriteLogFile(osal_id_t FileDesc, uint8 MessageType, const char *Prefix, const char *OutputMessage)
{
    char   LogFileBuffer[CFE_ASSERT_MAX_LOG_LINE_LENGTH];
    uint32 MsgEnabled = CFE_Assert_Global.CurrVerbosity >> MessageType;

    if (MsgEnabled & 1)
    {
        snprintf(LogFileBuffer, sizeof(LogFileBuffer), "[%5s] %s\n", Prefix, OutputMessage);
        OS_write(FileDesc, LogFileBuffer, strlen(LogFileBuffer));
    }
}

void UT_BSP_DoText(uint8 MessageType, const char *OutputMessage)
{
    const char *                Prefix;
    CFE_Assert_StatusCallback_t StatusCallback;

    Prefix = UtAssert_GetCaseTypeAbbrev(MessageType);

    StatusCallback = CFE_Assert_Global.StatusCallback;

    /* If not set, report status to CFE ES Syslog facility */
    if (StatusCallback == NULL)
    {
        StatusCallback = UT_BSP_SysLogStatusReport;
    }

    StatusCallback(MessageType, Prefix, OutputMessage);

    if (OS_ObjectIdDefined(CFE_Assert_Global.LogFileDesc))
    {
        UT_BSP_WriteLogFile(CFE_Assert_Global.LogFileDesc, MessageType, Prefix, OutputMessage);
    }

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
}
