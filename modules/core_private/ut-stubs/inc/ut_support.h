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

/**
 * @file
 *
 * Purpose:
 * Unit specification for Unit Test Stubs
 *
 * Notes:
 * These routines contain a minimum amount of functionality required for
 * unit testing full path coverage
 *
 */

#ifndef UT_SUPPORT_H
#define UT_SUPPORT_H

/*
** Includes
*/
#include <stddef.h>

#include "cfe.h"
#include "cfe_es_resetdata_typedef.h"

#include "utassert.h"
#include "uttest.h"
#include "uttools.h"
#include "utbsp.h"
#include "utstubs.h"

/*
 * Conversion from old-style OS_printf stub
 */
extern const char *UT_OSP_MESSAGES[];

/* CDS type declarations */
#define CFE_ES_CDS_CHECK_PATTERN   0x5a5a
#define CFE_ES_CDS_BLOCK_USED      0xaaaa
#define CFE_ES_CDS_BLOCK_UNUSED    0xdddd
#define CFE_ES_CDS_NUM_BLOCK_SIZES 17

/* MIN_BLOCK_SIZE must be < 16 bytes */
#define CFE_ES_CDS_MIN_BLOCK_SIZE 8
#define UT_EVENT_HISTORY_SIZE     64
#define UT_MAX_MESSAGE_LENGTH     300

/* Macro to add a test to the UT assert list */
#define UT_ADD_TEST(Func) UtTest_Add(Func, NULL, NULL, #Func)

/* Required to be defined for GetMsgId and SetMsgId stubs
 * Actual macro definitions are in cfe_sb_msg_id_utils.h
 * #ifndef so that the actual macros are used for unit
 * testing of SB
 * */
#ifndef CFE_SB_CMD_MESSAGE_TYPE
#define CFE_SB_CMD_MESSAGE_TYPE 0x0000080
#endif

#ifndef CFE_SB_RD_APID_FROM_MSGID
#define CFE_SB_RD_APID_FROM_MSGID(MsgId) (MsgId & 0x0000007F)
#endif

#ifndef CFE_SB_RD_SUBSYS_ID_FROM_MSGID
#define CFE_SB_RD_SUBSYS_ID_FROM_MSGID(MsgId) ((MsgId & 0x0000FF00) >> 8)
#endif

#ifndef CFE_SB_RD_TYPE_FROM_MSGID
#define CFE_SB_RD_TYPE_FROM_MSGID(MsgId) ((MsgId & CFE_SB_CMD_MESSAGE_TYPE) >> 7)
#endif

/*
 * NOTE: There are some UT cases in TBL that are dependent on
 * the endianness of the local CPU.  This "endian check" exists
 * to provide hints to the test code.
 *
 * All endian-specific code should be replaced with endian-neutral
 * code in future versions.  This check will be removed in a future
 * version once that is complete.  No new test code should use this.
 */
#define UT_LITTLE_ENDIAN 1
#define UT_BIG_ENDIAN    2
extern uint8 UT_Endianess;

typedef struct
{
    CFE_SB_MsgId_t MsgId;
    uint16         SnapshotOffset;
    size_t         SnapshotSize;
    uint16         Count;
    void *         SnapshotBuffer;
} UT_SoftwareBusSnapshot_Entry_t;

/**
 * Style of message dispatch to perform
 */
typedef enum
{
    /**
     * If method is set to NONE, then no dispatch will be set up.
     * This can be used to e.g. check error paths but not actual message handling.
     */
    UT_TaskPipeDispatchMethod_NONE,

    /**
     * If method is set to MSG_ID_CC, then no dispatch will be set up based on the
     * MsgID value and command code.  This is the traditional method and works with
     * task pipe implementations that utilize a local switch() statement.
     */
    UT_TaskPipeDispatchMethod_MSG_ID_CC,

    /**
     * If method is set to TABLE_OFFSET, then no dispatch will be set up based on the
     * offset into a dispatch table.  This is the EDS method and works with
     * task pipe implementations that perform message dispatch via a table lookup.
     */
    UT_TaskPipeDispatchMethod_TABLE_OFFSET

} UT_TaskPipeDispatchMethod_t;

/*
 * Information to identify a message in the "Task Pipe"
 * or message dispatch routines, to indicate which of
 * the message handlers the routine should invoke.
 *
 * Initially, this is the combination of MsgID + Command Code,
 * but this could be different for other implementations
 * that utilize a more abstract dispatch table.
 *
 * Using this structure wrapper allows the task pipe function
 * to evolve without necessarily breaking all the UT test
 * cases that need to invoke a specific handler.
 */
typedef struct
{
    /**
     * Method of dispatch to use.
     * This should match how the source was compiled,
     * and it controls how the stubs are configured.
     */
    UT_TaskPipeDispatchMethod_t Method;

    /**
     * Invoke the handler for this MsgID
     * This is only used/relevant when Method is set to MSG_ID_CC
     */
    CFE_SB_MsgId_t MsgId;

    /**
     * Offset of handler function to invoke
     * This is only used/relevant when Method is set to TABLE_OFFSET
     */
    int32 TableOffset;

    /**
     * Specifies the sub-command to invoke
     * (ignored if the handler does not use command codes,
     * set to zero in this case).
     */
    CFE_MSG_FcnCode_t CommandCode;

    /**
     * Set nonzero to indicate a code to be returned from dispatcher.
     * This may be relevant for any dispatch method
     */
    CFE_Status_t DispatchError;

    /**
     * Expected size of the message being handled
     */
    size_t NominalMsgSize;

} UT_TaskPipeDispatchId_t;

/*
 * The following macros set certain fields inside the UT_TaskPipeDispatchId_t
 * They can be combined as needed for various situations
 */
#define UT_TPD_SETSIZE(cmd) .NominalMsgSize = sizeof(cmd##_t)
#define UT_TPD_SETCC(cc)    .CommandCode = cc
#define UT_TPD_SETERR(err)  .DispatchError = err

/*
** Functions
*/
/*****************************************************************************/
/**
** \brief Initialize unit test
**
** \par Description
**        Set up logging file, initialize variables, and perform endian check.
**        This function must be called prior to executing any of the cFE unit
**        tests.
**
** \param[in] subsys  Pointer to subsystem ID character string
**
** \returns
**        This function does not return a value.
**
** \sa #UT_InitData, #UT_EndianCheck
**
******************************************************************************/
void UT_Init(const char *subsys);

/*****************************************************************************/
/**
** \brief Initialize unit test variables
**
** \par Description
**        This initialization function should be called prior to each unit test
**        in order to set the variables to known states.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_ClearEventHistory, #UT_ResetPoolBufferIndex, #UT_SetOSFail,
** \sa #UT_SetBSPFail, #UT_SetRtnCode, #UT_ResetCDS
**
******************************************************************************/
void UT_InitData(void);

/*****************************************************************************/
/**
** \brief Initialize the CDS
**
** \par Description
**         Initialize the CDS values to all zeroes in preparation for the
**         next test.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void UT_ResetCDS(void);

/*****************************************************************************/
/**
** \brief Reset pool buffer index
**
** \par Description
**        Reset pool buffer index to zero.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
/* Reset pool buffer array index */
void UT_ResetPoolBufferIndex(void);

/*****************************************************************************/
/**
** \brief Sets up stubs to follow the intended path through a dispatch call
**
** \par Description
**        Configures the MSG stubs appropriately so the intended command handler
**        is called when invoking a "TaskPipe" handler function.
**
**        DispatchReq should point to the intended MsgId + command code to set up.
**
**        If DispatchReq is NULL, then any existing stub config is cleared/reset.
**
**        If ExpectFailureEvent is set true, then it sets a second stub registration
**        for both MsgId and FcnCode to account for failure event reporting
**
** \returns
**        This function does not return a value.
******************************************************************************/
void UT_SetupBasicMsgDispatch(const UT_TaskPipeDispatchId_t *DispatchReq, CFE_MSG_Size_t MsgSize,
                              bool ExpectFailureEvent);

/*****************************************************************************/
/**
** \brief Send a message via the software bus
**
** \par Description
**        Does the required work to send a software bus message.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in] msg_ptr  Pointer to the buffer that contains the software
**                     bus message
**
** \param[in] id       Message ID to put into the message header
**
** \param[in] code     Command code to include in the message
**
** \returns
**        This function does not return a value.
******************************************************************************/
void UT_CallTaskPipe(void (*TaskPipeFunc)(const CFE_SB_Buffer_t *), const CFE_MSG_Message_t *MsgPtr, size_t MsgSize,
                     UT_TaskPipeDispatchId_t DispatchId);

/*****************************************************************************/
/**
** \brief A UT-Assert compatible hook function to get a snapshot of a software bus message
**
** \par Description
**        Captures a user-specified piece of information out of the software bus message
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Passes through the return code from the handler
**
******************************************************************************/
int32 UT_SoftwareBusSnapshotHook(void *UserObj, int32 StubRetcode, uint32 CallCount, const UT_StubContext_t *Context);

/*****************************************************************************/
/**
** \brief Set the application ID returned by unit test stubs
**
** \par Description
**        Set the application ID value returned by the unit test CFS stub
**        functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in] AppID_in  Application ID
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void UT_SetAppID(CFE_ES_AppId_t AppID_in);

/*****************************************************************************/
/**
** \brief Set the return code of CFE_PSP_GetResetArea and the data validation
**        signature
**
** \par Description
**        Set the return code of CFE_PSP_GetResetArea and the data validation
**        signature.
**
** \param[in] status       CFE_PSP_GetResetArea return code
**
** \param[in] Signature    Data validation signature used to verify data
**                         structure contents
**
** \param[in] ClockSignal  Clock signal type
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void UT_SetStatusBSPResetArea(int32 status, uint32 Signature, uint32 ClockSignal);

/*****************************************************************************/
/**
** \brief Set the contents of the buffer to read
**
** \par Description
**        Set the contents of the buffer to read; e.g., the startup script.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in] Buff      Pointer to information to place into the buffer
**
** \param[in] NumBytes  Length in bytes of the buffer contents
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void UT_SetReadBuffer(void *Buff, size_t NumBytes);

/*****************************************************************************/
/**
** \brief Set the contents of the header to read
**
** \par Description
**        Set the contents of the header to read.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in] Hdr       Pointer to information to place into the header
**
** \param[in] NumBytes  Length in bytes of the header contents
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void UT_SetReadHeader(void *Hdr, size_t NumBytes);

/*****************************************************************************/
/**
** \brief Set the dummy function return code
**
** \par Description
**        Used to set the return code when ES is starting tasks by pointers;
**        the dummy function is started instead of actually tasks.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in] Return  Dummy function return code
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void UT_SetDummyFuncRtn(int Return);

/*****************************************************************************/
/**
** \brief Set the size of the ES reset area
**
** \par Description
**        Set the size of the ES reset area that will be returned by calls
**        to the CFE_PSP_GetResetArea stub function.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in] Size  Reset area size
**
** \returns
**        This function does not return a value.
**
** \sa
**
******************************************************************************/
void UT_SetSizeofESResetArea(size_t Size);

/*****************************************************************************/
/**
** \brief Set the CDS size returned by the BSP
**
** \par Description
**        Set the CDS size returned by the BSP.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in] Size  CDS size
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
uint8 *UT_SetCDSSize(size_t Size);

/*****************************************************************************/
/**
** \brief Set BSP time
**
** \par Description
**        Set the time to be returned by calls to the CFE_PSP_GetTime stub
**        function.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in] seconds    Time, seconds
**
** \param[in] microsecs  Time, microseconds
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void UT_SetBSP_Time(uint32 seconds, uint32 microsecs);

/*****************************************************************************/
/**
** \brief Clear the event history
**
** \par Description
**        Clear the event history so that one test's event messages won't be
          confused with a subsequent test (some tests depend on an accurate
          count of the number of messages generated for that test).
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa
**
******************************************************************************/
void UT_ClearEventHistory(void);

/*****************************************************************************/
/**
** \brief Search the event history for a specified event ID
**
** \par Description
**        Search the event history for a specified event ID.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in] EventIDToSearchFor  Event ID to search for in the event history
**
** \returns
**        true if the event ID to search for is found in the event history;
**        false if the event ID is not found.
**
******************************************************************************/
bool UT_EventIsInHistory(uint16 EventIDToSearchFor);

/*****************************************************************************/
/**
** \brief Search the syslog history for a specified log message template
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in] LogMsgToSearchFor  Template to search for in the syslog history
**
** \returns
**        Number of matches of the template found in the history
**
******************************************************************************/
uint32 UT_SyslogIsInHistory(const char *LogMsgToSearchFor);

/*****************************************************************************/
/**
** \brief Search the printf history for a specified message template
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in] MsgToSearchFor  Template to search for in the syslog history
**
** \returns
**        Number of matches of the template found in the history
**
******************************************************************************/
uint32 UT_PrintfIsInHistory(const char *MsgToSearchFor);

/*****************************************************************************/
/**
** \brief Return number of events issued
**
** \par Description
**        Return number of events issued since the last UT_ClearEventHistory
**        call.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Number of events issued.
**
******************************************************************************/
uint16 UT_GetNumEventsSent(void);

/*****************************************************************************/
/**
** \brief Display the contents of a packet
**
** \par Description
**         Display the contents of a packet.  The contents is displayed as a
**         series of bytes in hexadecimal.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in] ptr   Pointer to packet to display
**
** \param[in] size  Size of packet in bytes
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void UT_DisplayPkt(CFE_MSG_Message_t *MsgPtr, size_t size);

/*****************************************************************************/
/**
** \brief Gets a reference to the CFE ES Reset Data Object
**
** \par Description
**        Some CFE test cases may need to check or modify the reset data
**        that is maintained by CFE ES.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Pointer to simulated reset data object
**
******************************************************************************/
CFE_ES_ResetData_t *UT_GetResetDataPtr(void);

/*
** Assert Helper Functions
** Wrappers around the UtAssert functions to tune them for CFE use.
** These should all return a generalized pass/fail status (bool)
*/

/*****************************************************************************/
/**
** \brief Helper function for nominal CFE calls
**
** \par Description
**        This helper function wraps the normal UtAssert function, intended for verifying
**        CFE API calls that are expected to return successfully (#CFE_SUCCESS typically).
**
**        This can also be used to confirm setup and teardown operations by passing the CaseType
**        parameter appropriately (UTASSERT_CASETYPE_TSF or UTASSERT_CASETYPE_TTF, respectively).
**
** \par Assumptions, External Events, and Notes:
**        When used for setup (TSF) or teardown (TTF) then the test case is only logged to
**        the output if it fails.  This is to keep logs more concise, by not including
**        test cases that are not related to the main focus of the code under test.
**
**        Note this will accept any non-negative value as logical "success", so it
**        also works with functions that return a size or other non-error status.
**
** \returns Test pass status, returns true if status was successful, false if it failed.
**
******************************************************************************/
bool CFE_UtAssert_SuccessCheck_Impl(CFE_Status_t Status, UtAssert_CaseType_t CaseType, const char *File, uint32 Line,
                                    const char *Text);

/*****************************************************************************/
/**
** \brief Helper function for message check (printf/syslog) verifications
**
** \par Description
**        This helper function wraps the normal UtAssert function, intended for verifying
**        CFE API calls that are expected to generate printf or syslog messages.  This
**        includes the actual message in the log, but scrubs it for newlines and other
**        items that may affect the ability to parse the log file via a script.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns Test pass status, returns true if status was successful, false if it failed.
**
******************************************************************************/
bool CFE_UtAssert_MessageCheck_Impl(bool Status, const char *File, uint32 Line, const char *Desc,
                                    const char *FormatString);

/*****************************************************************************/
/**
** \brief Checks the successful execution of a setup function.
**
** \par Description
**        Many tests require a number of steps of setup to configure CFE such
**        that the actual test can be performed. Failure of any setup steps
**        result in a text message and the test being considered failed.
**
** \par Assumptions, External Events, and Notes:
**        To keep logs clean, this only generates a log message if it fails
**
******************************************************************************/
#define CFE_UtAssert_SETUP(FN) CFE_UtAssert_SuccessCheck_Impl(FN, UTASSERT_CASETYPE_TSF, __FILE__, __LINE__, #FN)

/*****************************************************************************/
/**
** \brief Asserts the nominal execution of the function being tested.
**
** \par Description
**        The core of each unit test is the execution of the function being tested.
**        This function and macro should be used to test the nominal execution of the
**        function; the expectation is that it will return CFE_SUCCESS.
**
** \par Assumptions, External Events, and Notes:
**        None
**
******************************************************************************/
#define CFE_UtAssert_SUCCESS(FN) CFE_UtAssert_SuccessCheck_Impl(FN, UTASSERT_CASETYPE_FAILURE, __FILE__, __LINE__, #FN)

/*****************************************************************************/
/**
** \brief Ensures that the test generated the expected number of events.
**
** \par Description
**        Most tests will generate a number of events, and the number generated
**        should be checked via this macro. If the number of events is different,
**        the test is considered to have failed and an error is reported.
**
** \par Assumptions, External Events, and Notes:
**        None
**
******************************************************************************/
#define CFE_UtAssert_EVENTCOUNT(EXP)                                                                                   \
    UtAssert_GenericUnsignedCompare(UT_GetNumEventsSent(), UtAssert_Compare_EQ, EXP, UtAssert_Radix_DECIMAL, __FILE__, \
                                    __LINE__, "Event Count: ", "Sent", "Expected")

/*****************************************************************************/
/**
** \brief Ensures that the code under test generated an expected event.
**
** \par Description
**        Most tests will generate a number of events, and this function and macro check whether an
**        event was generated. If not, the test is considered to have failed and an error is reported.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \sa #CFE_UtAssert_EVENTNOTSENT
**
******************************************************************************/
#define CFE_UtAssert_EVENTSENT(EVT)                                                                            \
    UtAssert_GenericUnsignedCompare(UT_EventIsInHistory(EVT), UtAssert_Compare_NEQ, 0, UtAssert_Radix_DECIMAL, \
                                    __FILE__, __LINE__, "Event Generated: ", #EVT, "")

/*****************************************************************************/
/**
** \brief Ensures that the code under test did not generate an unexpected event.
**
** \par Description
**        Most tests will generate a number of events, and this function and macro check whether an
**        event was generated.  This is the inverse of #CFE_UtAssert_EVENTSENT, and asserts that the
**        code under test did NOT generate the given event ID.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \sa #CFE_UtAssert_EVENTSENT
**
******************************************************************************/
#define CFE_UtAssert_EVENTNOTSENT(EVT)                                                                        \
    UtAssert_GenericUnsignedCompare(UT_EventIsInHistory(EVT), UtAssert_Compare_EQ, 0, UtAssert_Radix_DECIMAL, \
                                    __FILE__, __LINE__, "Event Not Generated: ", #EVT, "")

/*****************************************************************************/
/**
** \brief Checks if the code under test invoked CFE_ES_WriteToSysLog with the specified format string
**
** \par Description
**        Confirms that the code followed a path which invoked a specific syslog call
**
** \par Assumptions, External Events, and Notes:
**        SysLog messages are typically not relevant to requirements/correctness, but this
**        is potentially useful to confirm a specific code path was followed.
**
******************************************************************************/
#define CFE_UtAssert_SYSLOG(str) \
    CFE_UtAssert_MessageCheck_Impl(UT_SyslogIsInHistory(str), __FILE__, __LINE__, "Syslog generated: ", str)

/*****************************************************************************/
/**
** \brief Checks if the code under test invoked OS_printf with the specified format string
**
** \par Description
**        Confirms that the code followed a path which invoked a specific OS_printf call
**
** \par Assumptions, External Events, and Notes:
**        OS_printf messages are typically not relevant to requirements/correctness, but this
**        is potentially useful to confirm a specific code path was followed.
**
******************************************************************************/
#define CFE_UtAssert_PRINTF(str) \
    CFE_UtAssert_MessageCheck_Impl(UT_PrintfIsInHistory(str), __FILE__, __LINE__, "Printf generated: ", str)

/*****************************************************************************/
/**
** \brief Checks the successful execution of a teardown function.
**
** \par Description
**        Many tests require a number of steps of setup to configure CFE such that the actual test
**        can be performed, and undoing that configuration is the role of the teardown steps. Failure
**        of any teardown steps result in a text message and the test being considered failed.
**
** \par Assumptions, External Events, and Notes:
**        To keep logs clean, this only generates a log message if it fails
**
******************************************************************************/
#define CFE_UtAssert_TEARDOWN(FN) CFE_UtAssert_SuccessCheck_Impl(FN, UTASSERT_CASETYPE_TTF, __FILE__, __LINE__, #FN)

/*****************************************************************************/
/**
** \brief Macro to check CFE resource ID for equality
**
** \par Description
**        A macro that checks two resource ID values for equality.
**
** \par Assumptions, External Events, and Notes:
**        The generic #UtAssert_UINT32_EQ check should not be used, as ID values
**        and integers may not be interchangeable with strict type checking.
**
******************************************************************************/
#define CFE_UtAssert_RESOURCEID_EQ(id1, id2)                                                                         \
    UtAssert_GenericUnsignedCompare(CFE_RESOURCEID_TO_ULONG(id1), UtAssert_Compare_EQ, CFE_RESOURCEID_TO_ULONG(id2), \
                                    UtAssert_Radix_HEX, __FILE__, __LINE__, "Resource ID Check: ", #id1, #id2)

/*****************************************************************************/
/**
** \brief Macro to check CFE message ID for equality
**
** \par Description
**        A macro that checks two message ID values for equality.
**
** \par Assumptions, External Events, and Notes:
**        The generic #UtAssert_UINT32_EQ check should not be used, as CFE_SB_MsgId_t values
**        and integers may not be interchangeable with strict type checking.
**
******************************************************************************/
#define CFE_UtAssert_MSGID_EQ(mid1, mid2)                                                                      \
    UtAssert_GenericUnsignedCompare(CFE_SB_MsgIdToValue(mid1), UtAssert_Compare_EQ, CFE_SB_MsgIdToValue(mid2), \
                                    UtAssert_Radix_HEX, __FILE__, __LINE__, "MsgId Check: ", #mid1, #mid2)

#endif /* UT_SUPPORT_H */
