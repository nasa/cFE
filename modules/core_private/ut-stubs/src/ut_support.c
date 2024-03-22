/************************************************************************
 * NASA Docket No. GSC-18,719-1, and identified as “core Flight System: Bootes”
 *
 * Copyright (c) 2020 United States Government as represented by the
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
static size_t         UT_LastCDSSize = 0;

typedef union
{
    long long int AlignLong;
    long double   AlignDbl;
    void *        AlignPtr;
    char          Content[2 * CFE_PLATFORM_ES_MAX_BLOCK_SIZE];
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

int32 dummy_function(void);

static const UT_EntryKey_t UT_TABLE_DISPATCHER = 0;

/*
** Functions
*/
/*
** Initialize unit test
*/
void UT_Init(const char *subsys)
{
    size_t i;

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

void UT_DispatchTableHandler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    UT_TaskPipeDispatchId_t *DispatchId    = UserObj;
    const CFE_SB_Buffer_t *  Buffer        = UT_Hook_GetArgValueByName(Context, "Buffer", const CFE_SB_Buffer_t *);
    const void *             DispatchTable = UT_Hook_GetArgValueByName(Context, "DispatchTable", const void *);
    const uint8 *            Addr;
    CFE_Status_t (*MsgHandler)(const CFE_SB_Buffer_t *);
    CFE_Status_t Status;

    MsgHandler = NULL;
    UT_Stub_GetInt32StatusCode(Context, &Status);

    if (Status == 0 && DispatchId != NULL)
    {
        Status = DispatchId->DispatchError;

        if (DispatchId->Method == UT_TaskPipeDispatchMethod_TABLE_OFFSET && DispatchTable != NULL)
        {
            Addr = DispatchTable;
            Addr += DispatchId->TableOffset;
            memcpy(&MsgHandler, Addr, sizeof(void *));
        }
    }

    if (MsgHandler != NULL)
    {
        Status = MsgHandler(Buffer);
    }

    UT_Stub_SetReturnValue(FuncKey, Status);
}

/*
** Sets up the MSG stubs in preparation to invoke a "TaskPipe" dispatch function
**
** This is part of the general UT_CallTaskPipe process, but split off to support use cases
** where the task pipe is reached through other means.
*/
void UT_SetupBasicMsgDispatch(const UT_TaskPipeDispatchId_t *DispatchReq, CFE_MSG_Size_t MsgSize,
                              bool ExpectFailureEvent)
{
    CFE_Status_t ErrorCode;

    if (DispatchReq != NULL)
    {
        if (DispatchReq->Method == UT_TaskPipeDispatchMethod_MSG_ID_CC)
        {
            /* Set up for the typical task pipe related calls */
            UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), (void *)&DispatchReq->MsgId, sizeof(DispatchReq->MsgId), true);
            UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), true);
            UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), (void *)&DispatchReq->CommandCode,
                             sizeof(DispatchReq->CommandCode), true);
        }

        if (DispatchReq->Method == UT_TaskPipeDispatchMethod_TABLE_OFFSET)
        {
            /* If the code uses EDS dispatch, this will invoke the right member function from the table (based on
             * offset).  This requires setting up the function used for table dispatching first. */
            if (UT_TABLE_DISPATCHER == 0)
            {
                UtAssert_Failed(
                    "Setup error: Method set to TABLE_OFFSET but table dispatcher function is not configured");
            }
            else
            {
                UT_SetHandlerFunction(UT_TABLE_DISPATCHER, UT_DispatchTableHandler, (void *)DispatchReq);
            }
        }

        /* If a failure event is being set up, set for MsgId/FcnCode retrieval as part of failure event reporting  */
        if (ExpectFailureEvent || DispatchReq->DispatchError != CFE_SUCCESS)
        {
            UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), (void *)&DispatchReq->MsgId, sizeof(DispatchReq->MsgId), true);
            UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), true);
            UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), (void *)&DispatchReq->CommandCode,
                             sizeof(DispatchReq->CommandCode), true);

            if (UT_TABLE_DISPATCHER != 0)
            {
                /* If the code uses EDS dispatch, this will cause it to return the specified error */
                if (DispatchReq->DispatchError != CFE_SUCCESS)
                {
                    ErrorCode = DispatchReq->DispatchError;
                }
                else
                {
                    /* If not specified, default to WRONG_MSG_LENGTH as this feature was historically used for testing
                     * bad length */
                    ErrorCode = CFE_STATUS_WRONG_MSG_LENGTH;
                }

                UT_SetDefaultReturnValue(UT_TABLE_DISPATCHER, ErrorCode);
            }
        }
    }
    else
    {
        /* Clear any stub config that may have been already set */
        UT_ResetState(UT_KEY(CFE_MSG_GetMsgId));
        UT_ResetState(UT_KEY(CFE_MSG_GetSize));
        UT_ResetState(UT_KEY(CFE_MSG_GetFcnCode));
        if (UT_TABLE_DISPATCHER != 0)
        {
            UT_ResetState(UT_TABLE_DISPATCHER);
        }
    }
}

/*
** Calls the specified "task pipe" function
**
** This first sets up the various stubs according to the test case,
** then invokes the pipe function.
*/
void UT_CallTaskPipe(void (*TaskPipeFunc)(const CFE_SB_Buffer_t *), const CFE_MSG_Message_t *MsgPtr, size_t MsgSize,
                     UT_TaskPipeDispatchId_t DispatchId)
{
    union
    {
        CFE_SB_Buffer_t Buf;
        uint8           Bytes[CFE_MISSION_SB_MAX_SB_MSG_SIZE];
    } SBBuf;

    /* Copy message into aligned SB buffer */
    memcpy(SBBuf.Bytes, MsgPtr, MsgSize);

    /* Passing MsgSize == 0 indicates intent to perform a size validation failure */
    UT_SetupBasicMsgDispatch(&DispatchId, MsgSize, (MsgSize == 0));

    /*
     * Finally, call the actual task pipe requested.
     */
    TaskPipeFunc(&SBBuf.Buf);

    /*
     * UN-set the stub config, as some values may point to values on stack.
     * This removes any pointers to stack values that may not have been consumed during the call
     */
    UT_SetupBasicMsgDispatch(NULL, 0, false);
}

int32 UT_SoftwareBusSnapshotHook(void *UserObj, int32 StubRetcode, uint32 CallCount, const UT_StubContext_t *Context)
{
    UT_SoftwareBusSnapshot_Entry_t *Snapshot = UserObj;
    const CFE_MSG_Message_t *       MsgPtr   = UT_Hook_GetArgValueByName(Context, "MsgPtr", CFE_MSG_Message_t *);
    const uint8_t *                 BytePtr;

    if (MsgPtr != NULL && Snapshot != NULL)
    {
        ++Snapshot->Count;
        if (Snapshot->SnapshotSize > 0 && Snapshot->SnapshotBuffer != NULL)
        {
            BytePtr = (const uint8 *)MsgPtr;
            memcpy(Snapshot->SnapshotBuffer, &BytePtr[Snapshot->SnapshotOffset], Snapshot->SnapshotSize);
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
void UT_SetReadBuffer(void *Buff, size_t NumBytes)
{
    UT_SetDataBuffer(UT_KEY(OS_read), Buff, NumBytes, true);
}

/*
** Set the contents of the header to read
*/
void UT_SetReadHeader(void *Hdr, size_t NumBytes)
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
void UT_SetSizeofESResetArea(size_t Size)
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
uint8 *UT_SetCDSSize(size_t Size)
{
    UT_ResetState(UT_KEY(CFE_PSP_GetCDSSize));
    UT_ResetState(UT_KEY(CFE_PSP_ReadFromCDS));
    UT_ResetState(UT_KEY(CFE_PSP_WriteToCDS));
    if (Size == 0)
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
    void *  TempBuff;
    uint16 *EvBuf;

    UT_GetDataBuffer(Func, &TempBuff, &MaxSize, &Position);
    if (TempBuff != NULL && MaxSize > 0)
    {
        EvBuf = TempBuff;
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
    uint8 *BytePtr = (uint8 *)MsgPtr;
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
static int UT_StrCmpFormatStr(const char *FormatStr, const char *TestStr, size_t FormatLength, size_t TestLength)
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
static int UT_StrCmpExact(const char *RefStr, const char *TestStr, size_t RefLength, size_t TestLength)
{
    return (RefLength == TestLength && memcmp(RefStr, TestStr, RefLength) == 0);
}

static uint32 UT_GetMessageCount(const char *Msg, UT_Buffer_t *Buf,
                                 int (*Comparator)(const char *, const char *, size_t, size_t))
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

bool CFE_UtAssert_SuccessCheck_Impl(CFE_Status_t Status, UtAssert_CaseType_t CaseType, const char *File, uint32 Line,
                                    const char *Text)
{
    bool Result = (Status >= CFE_SUCCESS);

    if (!Result || (CaseType != UTASSERT_CASETYPE_TSF && CaseType != UTASSERT_CASETYPE_TTF))
    {
        UtAssertEx(Result, CaseType, File, Line, "%s (0x%lx) == CFE_SUCCESS", Text, (unsigned long)Status);
    }

    return Result;
}

bool CFE_UtAssert_MessageCheck_Impl(bool Status, const char *File, uint32 Line, const char *Desc,
                                    const char *FormatString)
{
    char        ScrubbedFormat[256];
    const char *EndPtr;
    size_t      FormatLen;

    /* Locate the actual end of the string, but limited to length of local buffer */
    /* Reserve two extra chars for quotes */
    EndPtr = memchr(FormatString, 0, sizeof(ScrubbedFormat) - 3);
    if (EndPtr != NULL)
    {
        FormatLen = EndPtr - FormatString;
    }
    else
    {
        FormatLen = sizeof(ScrubbedFormat) - 3;
    }

    /* Check for a newline within that range, and if present, end the string there instead */
    EndPtr = memchr(FormatString, '\n', FormatLen);
    if (EndPtr != NULL)
    {
        FormatLen = EndPtr - FormatString;
    }

    /* Need to make a copy, as the input string is "const" */
    ScrubbedFormat[0] = '\'';
    memcpy(&ScrubbedFormat[1], FormatString, FormatLen);
    ScrubbedFormat[FormatLen + 1] = '\'';
    ScrubbedFormat[FormatLen + 2] = 0;

    return UtAssert_GenericSignedCompare(Status, UtAssert_Compare_GT, 0, UtAssert_Radix_DECIMAL, File, Line, Desc,
                                         ScrubbedFormat, "");
}
