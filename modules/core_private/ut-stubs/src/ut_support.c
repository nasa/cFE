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
** File: ut_support.c
**
** Purpose:
** Unit test stubs
**
** Notes:
** Minimal work is done, only what is required for unit testing
**
*/

/*
** Includes
*/
#include "ut_support.h"
#include "cfe_core_resourceid_basevalues.h"

#include <string.h>

/*
** Global variables
*/
uint8 UT_Endianess;

static char           UT_appname[80];
static char           UT_subsys[5];
static CFE_ES_AppId_t UT_AppID;
static uint32         UT_LastCDSSize = 0;

typedef union
{
    long long int AlignLong;
    long double   AlignDbl;
    void *        AlignPtr;
    char          Content[128 * 1024];
} UT_Buffer_t;

static UT_Buffer_t UT_CFE_ES_MemoryPool;
static UT_Buffer_t UT_CDS_Data;
static UT_Buffer_t UT_SyslogBuffer;
static UT_Buffer_t UT_PrintfBuffer;
static union
{
    UT_Buffer_t        Buff;
    CFE_ES_ResetData_t ResetData;
} UT_CFE_ES_ResetData;

static uint16 UT_SendEventHistory[UT_EVENT_HISTORY_SIZE];
static uint16 UT_SendTimedEventHistory[UT_EVENT_HISTORY_SIZE];
static uint16 UT_SendEventAppIDHistory[UT_EVENT_HISTORY_SIZE * 10];

extern int32 dummy_function(void);

/*
** Functions
*/
/*
** Initialize unit test
*/
void UT_Init(const char *subsys)
{
    int8 i;

    /* Copy the application name for later use */
    strncpy(UT_subsys, subsys, sizeof(UT_subsys) - 1);
    UT_subsys[sizeof(UT_subsys) - 1] = 0;
    snprintf(UT_appname, 80, "ut_cfe_%s", subsys);

    /* Convert to upper case */
    for (i = 0; i < strlen(UT_subsys); ++i)
    {
        if (UT_subsys[i] >= 'a' && UT_subsys[i] <= 'z')
        {
            UT_subsys[i] = (UT_subsys[i] - 'a') + 'A';
        }
    }

    UT_InitData();

    /*
     * NOTE: There are some UT cases in TBL that are dependent on
     * the endianness of the local CPU.  This "endian check" exists
     * to provide hints to the test code.
     *
     * All endian-specific code should be replaced with endian-neutral
     * code in future versions.  This check will be removed in a future
     * version once that is complete.  No new test code should use this.
     */
    int32 EndianCheck = 0x01020304;

    if ((*(char *)&EndianCheck) == 0x04)
    {
        UT_Endianess = UT_LITTLE_ENDIAN;
    }
    else
    {
        UT_Endianess = UT_BIG_ENDIAN;
    }
}

/*
** Initialize unit test variables
*/
void UT_InitData(void)
{
    /*
     * Purge all entries in the entire stub table
     */
    UT_ResetState(0);

    /*
     * Set up entries to capture Events and Syslog writes
     * These are frequently needed by many tests
     */
    UT_SyslogBuffer.AlignLong = 0;
    UT_SetDataBuffer(UT_KEY(CFE_ES_WriteToSysLog), &UT_SyslogBuffer, sizeof(UT_SyslogBuffer), false);

    UT_PrintfBuffer.AlignLong = 0;
    UT_SetDataBuffer(UT_KEY(OS_printf), &UT_PrintfBuffer, sizeof(UT_PrintfBuffer), false);

    /*
     * Set up the CFE_ES reset area
     */
    UT_SetDataBuffer(UT_KEY(CFE_PSP_GetResetArea), &UT_CFE_ES_ResetData, sizeof(UT_CFE_ES_ResetData), false);

    /*
     * Set up the CDS area
     */
    UT_SetCDSSize(UT_LastCDSSize);

    /*
     * Set up for the CFE_SB_ReceiveBuffer() call.
     *
     * The existing test cases assume that this will return success once,
     * followed by a timeout response.
     *
     * Specific test cases may provide an actual message buffer to return for
     * the first call, or they may override this default behavior entirely.
     */
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_ReceiveBuffer), 2, CFE_SB_TIME_OUT);

    /*
     * Set up CFE_ES_GetAppName() and friends
     * This should return the UT_appname
     */
    UT_SetDataBuffer(UT_KEY(CFE_ES_GetAppName), (uint8 *)UT_appname, sizeof(UT_appname), false);

    /*
     * Set up CFE_ES_GetAppID() to return the equivalent of AppID 1 by default
     */
    UT_SetAppID(CFE_ES_APPID_C(CFE_ResourceId_FromInteger(CFE_ES_APPID_BASE + 1)));

    /*
     * Reset the OSAL stubs to the default state
     */
    OS_API_Init();

    UT_ClearEventHistory();
    UT_ResetPoolBufferIndex();
}

void UT_ResetCDS(void)
{
    memset(&UT_CDS_Data, 0, sizeof(UT_CDS_Data));
}

/*
** Reset pool buffer index
*/
void UT_ResetPoolBufferIndex(void)
{
    /*
     * Initialize the buffer which ES mempool will allocate from
     * Set the whole memory space to a pattern.
     */
    UT_ResetState(UT_KEY(CFE_ES_GetPoolBuf));
    UT_SetDataBuffer(UT_KEY(CFE_ES_GetPoolBuf), &UT_CFE_ES_MemoryPool, sizeof(UT_CFE_ES_MemoryPool), false);
}

/*
** Output single test's pass/fail status
*/
void UT_Report(const char *file, uint32 line, bool test, const char *fun_name, const char *info)
{
    UtAssertEx(test, UtAssert_GetContext(), file, line, "%s - %s", fun_name, info);
}

/*
** Calls the specified "task pipe" function
**
** This first sets up the various stubs according to the test case,
** then invokes the pipe function.
*/
void UT_CallTaskPipe(void (*TaskPipeFunc)(CFE_SB_Buffer_t *), CFE_MSG_Message_t *MsgPtr, size_t MsgSize,
                     UT_TaskPipeDispatchId_t DispatchId)
{
    union
    {
        CFE_SB_Buffer_t Buf;
        uint8           Bytes[CFE_MISSION_SB_MAX_SB_MSG_SIZE];
    } SBBuf;

    /* Copy message into aligned SB buffer */
    memcpy(SBBuf.Bytes, MsgPtr, MsgSize);

    /* Set up for the typical task pipe related calls */
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &DispatchId.MsgId, sizeof(DispatchId.MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &DispatchId.CommandCode, sizeof(DispatchId.CommandCode), false);

    /* If 0 size passed in, set buffers for calls in the command length failure reporting */
    if (MsgSize == 0)
    {
        UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &DispatchId.MsgId, sizeof(DispatchId.MsgId), false);
        UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &DispatchId.CommandCode, sizeof(DispatchId.CommandCode), false);
    }

    /*
     * Finally, call the actual task pipe requested.
     */
    TaskPipeFunc(&SBBuf.Buf);
}

int32 UT_SoftwareBusSnapshotHook(void *UserObj, int32 StubRetcode, uint32 CallCount, const UT_StubContext_t *Context)
{
    UT_SoftwareBusSnapshot_Entry_t *Snapshot = UserObj;
    const CFE_MSG_Message_t *       MsgPtr;

    if (Context->ArgCount > 0)
    {
        MsgPtr = Context->ArgPtr[0];
    }
    else
    {
        MsgPtr = NULL;
    }

    if (MsgPtr != NULL && Snapshot != NULL)
    {
        ++Snapshot->Count;
        if (Snapshot->SnapshotSize > 0 && Snapshot->SnapshotBuffer != NULL)
        {
            memcpy(Snapshot->SnapshotBuffer, &MsgPtr->Byte[Snapshot->SnapshotOffset], Snapshot->SnapshotSize);
        }
    }

    return StubRetcode;
}

/*
** Set the application ID returned by unit test stubs
*/
void UT_SetAppID(CFE_ES_AppId_t AppID_in)
{
    UT_AppID = AppID_in;
    UT_SetDataBuffer(UT_KEY(CFE_ES_GetAppID), (uint8 *)&UT_AppID, sizeof(UT_AppID), false);
}

/*
** Set the return code of CFE_PSP_GetResetArea and the data validation
** signature
*/
void UT_SetStatusBSPResetArea(int32 status, uint32 Signature, uint32 ClockSignal)
{
    UT_CFE_ES_ResetData.ResetData.TimeResetVars.Signature   = Signature;
    UT_CFE_ES_ResetData.ResetData.TimeResetVars.ClockSignal = ClockSignal;
    if (status != 0)
    {
        UT_SetDefaultReturnValue(UT_KEY(CFE_PSP_GetResetArea), status);
    }
    else
    {
        UT_ClearDefaultReturnValue(UT_KEY(CFE_PSP_GetResetArea));
    }
}

/*
** Set the contents of the buffer to read
*/
void UT_SetReadBuffer(void *Buff, int NumBytes)
{
    UT_SetDataBuffer(UT_KEY(OS_read), Buff, NumBytes, true);
}

/*
** Set the contents of the header to read
*/
void UT_SetReadHeader(void *Hdr, int NumBytes)
{
    UT_SetDataBuffer(UT_KEY(CFE_FS_ReadHeader), Hdr, NumBytes, true);
}

/*
** Set the dummy function return code
*/
void UT_SetDummyFuncRtn(int Return)
{
    UT_SetDefaultReturnValue(UT_KEY(dummy_function), Return);
}

/*
** Set the size of the ES reset area
*/
void UT_SetSizeofESResetArea(int32 Size)
{
    UT_ResetState(UT_KEY(CFE_PSP_GetResetArea));
    if (Size > 0)
    {
        if (Size > sizeof(UT_CFE_ES_ResetData))
        {
            UtAssert_Abort("Reset area size too large");
            Size = sizeof(UT_CFE_ES_ResetData);
        }
        UT_SetDataBuffer(UT_KEY(CFE_PSP_GetResetArea), &UT_CFE_ES_ResetData, Size, false);
    }
}

/*
** Set the CDS size returned by the BSP
*/
uint8 *UT_SetCDSSize(int32 Size)
{
    UT_ResetState(UT_KEY(CFE_PSP_GetCDSSize));
    UT_ResetState(UT_KEY(CFE_PSP_ReadFromCDS));
    UT_ResetState(UT_KEY(CFE_PSP_WriteToCDS));
    if (Size <= 0)
    {
        UT_LastCDSSize = 0;
        return NULL;
    }

    if (Size > sizeof(UT_CDS_Data))
    {
        UtAssert_Abort("Size of CDS too large");
        Size = sizeof(UT_CDS_Data);
    }
    UT_SetDataBuffer(UT_KEY(CFE_PSP_GetCDSSize), &UT_CDS_Data, Size, false);
    UT_SetDataBuffer(UT_KEY(CFE_PSP_ReadFromCDS), &UT_CDS_Data, Size, false);
    UT_SetDataBuffer(UT_KEY(CFE_PSP_WriteToCDS), &UT_CDS_Data, Size, false);
    UT_LastCDSSize = Size;
    return (uint8 *)UT_CDS_Data.Content;
}

/*
** Set BSP time
*/
void UT_SetBSP_Time(uint32 seconds, uint32 microsecs)
{
    OS_time_t BSP_Time;

    BSP_Time = OS_TimeAssembleFromNanoseconds(seconds, microsecs * 1000);

    UT_SetDataBuffer(UT_KEY(CFE_PSP_GetTime), &BSP_Time, sizeof(BSP_Time), true);
}

void UT_ClearEventHistory(void)
{
    UT_ResetState(UT_KEY(CFE_EVS_SendEvent));
    UT_ResetState(UT_KEY(CFE_EVS_SendEventWithAppID));
    UT_ResetState(UT_KEY(CFE_EVS_SendTimedEvent));
    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), UT_SendEventHistory, sizeof(UT_SendEventHistory), false);
    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendTimedEvent), UT_SendTimedEventHistory, sizeof(UT_SendTimedEventHistory), false);
    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEventWithAppID), UT_SendEventAppIDHistory, sizeof(UT_SendEventAppIDHistory),
                     false);
}

static bool UT_CheckEventHistoryFromFunc(UT_EntryKey_t Func, uint16 EventIDToSearchFor)
{
    bool    Result = false;
    size_t  Position;
    size_t  MaxSize;
    uint16 *EvBuf;

    UT_GetDataBuffer(Func, (void **)&EvBuf, &MaxSize, &Position);
    if (EvBuf != NULL && MaxSize > 0)
    {
        Position /= sizeof(*EvBuf);
        while (Position > 0)
        {
            if (*EvBuf == EventIDToSearchFor)
            {
                Result = true;
                break;
            }
            ++EvBuf;
            --Position;
        }
    }

    return Result;
}

/*
** Search the event history for a specified event ID
*/
bool UT_EventIsInHistory(uint16 EventIDToSearchFor)
{
    return (UT_CheckEventHistoryFromFunc(UT_KEY(CFE_EVS_SendEvent), EventIDToSearchFor) ||
            UT_CheckEventHistoryFromFunc(UT_KEY(CFE_EVS_SendEventWithAppID), EventIDToSearchFor) ||
            UT_CheckEventHistoryFromFunc(UT_KEY(CFE_EVS_SendTimedEvent), EventIDToSearchFor));
}

/*
** Return number of events issued
*/
uint16 UT_GetNumEventsSent(void)
{
    uint16 Total = 0;
    size_t Position;
    size_t MaxSize;
    void * EvBuf;

    UT_GetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EvBuf, &MaxSize, &Position);
    Total += Position / sizeof(uint16);
    UT_GetDataBuffer(UT_KEY(CFE_EVS_SendEventWithAppID), &EvBuf, &MaxSize, &Position);
    Total += Position / sizeof(uint16);
    UT_GetDataBuffer(UT_KEY(CFE_EVS_SendTimedEvent), &EvBuf, &MaxSize, &Position);
    Total += Position / sizeof(uint16);

    return Total;
}

/*
** Display the contents of a packet
*/
void UT_DisplayPkt(CFE_MSG_Message_t *MsgPtr, size_t size)
{
    uint8 *BytePtr = MsgPtr->Byte;
    size_t i;
    size_t BufSize = UT_MAX_MESSAGE_LENGTH;
    char   DisplayMsg[UT_MAX_MESSAGE_LENGTH];
    char * msgPtr = DisplayMsg;

    DisplayMsg[0] = '\0';

    for (i = 0; i < size && BufSize > 3; i++)
    {
        snprintf(msgPtr, BufSize, "%02x ", *BytePtr);
        msgPtr += 3;
        BufSize -= 3;
        ++BytePtr;
    }

    UtPrintf("%s", DisplayMsg);
}

CFE_ES_ResetData_t *UT_GetResetDataPtr(void)
{
    return &UT_CFE_ES_ResetData.ResetData;
}

/*
** Compare two strings using printf-style conversion specifiers.
**
** Determines if the TestStr could have been an output from a printf-style call
** using FormatStr.  Common conversions specifiers in FormatStr are recognized:
** %c, %d, %f, %F, %g, %G, %i, %u, %s, %p, %x, %X, and %%
**
** To keep it simple this just skips over the conversion specifier entirely,
** it does not validate whether the content of the string could have come from
** that conversion specifier or not.  In essence any conversion specifier is
** treated as a wildcard that matches any string.
**
** HOWEVER - this could be enhanced in the future, if necessary, to actually
** perform that validation by e.g. passing the string to "sscanf()" or similar
** and checking if the reverse conversion was successful.
**
** NOTE this logic is not foolproof - particularly where %s is in use, as the
** content of the string could alias the next part of the format string.
** This does not seem to be an issue for the strings generated by CFE.
**
** But on the upside this concession avoids a far more complicated issue of
** needing a fully-fledged implementation of printf in the OS_printf stub.
*/
static int UT_StrCmpFormatStr(const char *FormatStr, const char *TestStr, uint32 FormatLength, uint32 TestLength)
{
    const char *ChunkStart;
    const char *ChunkEnd;
    const char *TestStart;
    int         WildCard;
    int         MatchGood;
    size_t      ChunkLen;

    /* Step through each character in both strings */
    ChunkStart = FormatStr;
    TestStart  = TestStr;
    WildCard   = 1;
    MatchGood  = 1;
    while (FormatLength > 0)
    {
        if (*ChunkStart == '%')
        {
            /* At a Conversion specifier - Find the END of it */
            do
            {
                --FormatLength;
                ++ChunkStart;
            } while (FormatLength > 0 && strchr("%cdfFgGiuspxX ", *ChunkStart) == NULL);

            if (FormatLength == 0)
            {
                /* Invalid format string */
                MatchGood = 0;
                break;
            }

            if (*ChunkStart != '%')
            {
                WildCard = 1;
                ++ChunkStart;
                --FormatLength;
            }
            else if (TestLength > 0 && *TestStart == '%')
            {
                MatchGood = 1;
                ++TestStart;
                --TestLength;
            }
            else
            {
                /* Insufficient length on TestString */
                MatchGood = 0;
                break;
            }
        }
        else
        {
            /* Matchable content - Find the NEXT conversion specifier in the format string */
            MatchGood = 0;
            ChunkEnd  = memchr(ChunkStart, '%', FormatLength);
            if (ChunkEnd != NULL)
            {
                ChunkLen = ChunkEnd - ChunkStart;
            }
            else
            {
                ChunkLen = FormatLength;
            }

            while (TestLength >= ChunkLen)
            {
                if (memcmp(ChunkStart, TestStart, ChunkLen) == 0)
                {
                    MatchGood = 1;
                    break;
                }
                if (!WildCard)
                {
                    break;
                }
                ++TestStart;
                --TestLength;
            }

            if (!MatchGood)
            {
                break;
            }

            WildCard = 0;
            ChunkStart += ChunkLen;
            TestStart += ChunkLen;
            TestLength -= ChunkLen;
            FormatLength -= ChunkLen;
        }
    }

    return MatchGood;
}

/*
 * A string comparison function that will match exact strings only
 * (Printf style conversion strings are compared literally)
 */
static int UT_StrCmpExact(const char *RefStr, const char *TestStr, uint32 RefLength, uint32 TestLength)
{
    return (RefLength == TestLength && memcmp(RefStr, TestStr, RefLength) == 0);
}

static uint32 UT_GetMessageCount(const char *Msg, UT_Buffer_t *Buf,
                                 int (*Comparator)(const char *, const char *, uint32, uint32))
{
    uint32 Count  = 0;
    uint32 MsgLen = strlen(Msg);
    char * Start  = Buf->Content;
    char * End;

    while (MsgLen > 0 && Msg[MsgLen - 1] == '\n')
    {
        --MsgLen;
    }

    while (1)
    {
        End = strchr(Start, '\n');
        if (End == NULL)
        {
            End = Start + strlen(Start);
        }

        if (Start == End)
        {
            break;
        }

        if (Comparator(Msg, Start, MsgLen, End - Start))
        {
            ++Count;
        }

        if (*End == 0)
        {
            break;
        }

        Start = End + 1;
    }

    return Count;
}

uint32 UT_SyslogIsInHistory(const char *LogMsgToSearchFor)
{
    return UT_GetMessageCount(LogMsgToSearchFor, &UT_SyslogBuffer, UT_StrCmpExact);
}

uint32 UT_PrintfIsInHistory(const char *MsgToSearchFor)
{
    return UT_GetMessageCount(MsgToSearchFor, &UT_PrintfBuffer, UT_StrCmpFormatStr);
}

void UT_AddSubTest(void (*Test)(void), void (*Setup)(void), void (*Teardown)(void), const char *GroupName,
                   const char *TestName)
{
    char        CompleteTestName[128];
    const char *GroupPtr;
    const char *TestPtr;

    /* Remove any common prefix between the two names.
     * They are often function names that all start with "Test_XXX"
     * and this repetitive information just becomes clutter.
     */
    GroupPtr = GroupName;
    TestPtr  = TestName;
    while (*GroupPtr != 0 && *GroupPtr == *TestPtr)
    {
        ++GroupPtr;
        ++TestPtr;
    }

    /*
     * Only break at an underscore(_) to avoid weird effects
     */
    while (TestPtr > TestName && *TestPtr != '_')
    {
        --TestPtr;
    }
    if (*TestPtr == '_')
    {
        ++TestPtr;
    }

    /*
     * Remove a remaining "Test_" prefix on the group name.
     * Again just to remove common repetitive content
     */
    GroupPtr = GroupName;
    if (strncmp(GroupPtr, "Test_", 5) == 0)
    {
        GroupPtr += 5;
    }

    (void)snprintf(CompleteTestName, sizeof(CompleteTestName), "%s.%s", GroupPtr, TestPtr);

    /*
     * Note that UtTest_Add() does not copy the string - it retains the passed-in pointer.
     * Therefore it must be duplicated.
     *
     * This isn't an issue on Linux as the memory is freed when the process exits, but
     * might be an issue on VxWorks.
     */

    UtTest_Add(Test, Setup, Teardown, strdup(CompleteTestName));
}

void UT_SETUP_impl(const char *FileName, int LineNum, const char *TestName, const char *FnName, int32 FnRet)
{
    UtAssertEx(FnRet == CFE_SUCCESS, UTASSERT_CASETYPE_TSF, FileName, LineNum, "%s - Setup - %s returned 0x%lx",
               TestName, FnName, (long int)FnRet);
}

void UT_ASSERT_impl(const char *FileName, int LineNum, const char *TestName, const char *FnName, int32 FnRet)
{
    UtAssertEx(FnRet == CFE_SUCCESS, UtAssert_GetContext(), FileName, LineNum,
               "%s - %s returned 0x%lx, expected CFE_SUCCESS", TestName, FnName, (long int)FnRet);
}

void UT_ASSERT_EQ_impl(const char *FileName, int LineNum, const char *FnName, int32 FnRet, const char *ExpName,
                       int32 Exp)
{
    UtAssertEx(FnRet == Exp, UtAssert_GetContext(), FileName, LineNum, "%s - value %ld 0x%lx, expected %s[%ld 0x%lx]",
               FnName, (long)FnRet, (long)FnRet, ExpName, (long)Exp, (long)Exp);
}

void UT_ASSERT_TRUE_impl(const char *FileName, int LineNum, const char *TestName, const char *ExpName, bool Exp)
{
    UtAssertEx(Exp, UtAssert_GetContext(), FileName, LineNum, "%s - %s", TestName, ExpName);
}

void UT_EVTCNT_impl(const char *FileName, int LineNum, const char *TestName, int32 CntExp)
{
    int32 CntSent = UT_GetNumEventsSent();

    UtAssertEx(CntSent == CntExp, UtAssert_GetContext(), FileName, LineNum, "%s - event count (sent %ld, expected %ld)",
               TestName, (long int)CntSent, (long int)CntExp);
}

void UT_EVTSENT_impl(const char *FileName, int LineNum, const char *TestName, const char *EvtName, int32 EvtId)
{
    UtAssertEx(UT_EventIsInHistory(EvtId), UtAssert_GetContext(), FileName, LineNum, "%s - sent event %s [%ld]",
               TestName, EvtName, (long int)EvtId);
}

void UT_TEARDOWN_impl(const char *FileName, int LineNum, const char *TestName, const char *FnName, int32 FnRet)
{
    UtAssertEx(FnRet == CFE_SUCCESS, UTASSERT_CASETYPE_TTF, FileName, LineNum,
               "%s - Teardown failed (%s returned 0x%lx)", TestName, FnName, (long int)FnRet);
}
