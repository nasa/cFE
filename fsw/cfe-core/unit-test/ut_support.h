/*
**  GSC-18128-1, "Core Flight Executive Version 6.6"
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
** File: ut_support.h
**
** Purpose:
** Unit specification for Unit Test Stubs
**
** Notes:
** These routines contain a minimum amount of functionality required for
** unit testing full path coverage
**
*/
#ifndef _UT_CFE_SUPPORT_H_
#define _UT_CFE_SUPPORT_H_

/* Compiler flags:
   - Define UT_SHOW_PASS to display PASSED (and FAILED) test results;
     undefine to show only FAILED test results
   - Define UT_VERBOSE to display stub routine messages in unit test
     log files
   - Define SOCKET_QUEUE to use Linux socket queue functions; undefine
     to use an embedded queue replacement
   - Define CFE_LINUX if compiling under Linux; undefine when compiling
     on the GHS target
   - Define OS_USE_EMBEDDED_PRINTF to use the embedded (replacement) printf
     functions; undefine to use the standard printf library functions
*/

/*
** Includes
*/
#include <stddef.h>

#include "cfe.h"
#include "cfe_sb.h"
#include "cfe_es.h"
#include "common_types.h"
#include "cfe_evs_task.h"
#include "cfe_es_global.h"
#include "cfe_es_cds.h"
#include "cfe_esmempool.h"
#include "cfe_time_utils.h"

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
#define CFE_ES_CDS_MIN_BLOCK_SIZE   8
#define UT_EVENT_HISTORY_SIZE       64
#define UT_MAX_MESSAGE_LENGTH       300

/* Macro to add a test to the UT assert list */
#define UT_ADD_TEST(Func)       UtTest_Add(Func, NULL, NULL, #Func)


/* Required to be defined for GetMsgId and SetMsgId stubs
 * Actual macro defitiions are in cfe_sb_msg_id_utils.h
 * #ifndef so that the actual macros are used for unit 
 * testing of SB
 * */
#ifndef CFE_SB_CMD_MESSAGE_TYPE
#define CFE_SB_CMD_MESSAGE_TYPE 0x0000080
#endif

#ifndef CFE_SB_RD_APID_FROM_MSGID
#define CFE_SB_RD_APID_FROM_MSGID(MsgId)          (MsgId & 0x0000007F)  
#endif

#ifndef CFE_SB_RD_SUBSYS_ID_FROM_MSGID
#define CFE_SB_RD_SUBSYS_ID_FROM_MSGID(MsgId)  ( (MsgId & 0x0000FF00) >> 8) 
#endif

#ifndef CFE_SB_RD_TYPE_FROM_MSGID
#define CFE_SB_RD_TYPE_FROM_MSGID(MsgId)       ( (MsgId & CFE_SB_CMD_MESSAGE_TYPE) >> 7) 
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
extern uint8  UT_Endianess;


typedef struct
{
    CFE_SB_MsgId_t MsgId;
    uint16 SnapshotOffset;
    uint16 SnapshotSize;
    uint16 Count;
    void *SnapshotBuffer;
} UT_SoftwareBusSnapshot_Entry_t;

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
     * Invoke the handler for this MsgID
     */
    CFE_SB_MsgId_t MsgId;

    /**
     * Specifies the sub-command to invoke
     * (ignored if the handler does not use command codes,
     * set to zero in this case).
     */
    uint16 CommandCode;

} UT_TaskPipeDispatchId_t;

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
** \par Assumptions, External Events, and Notes:
**        A log file is written only if CFE_LINUX is defined.
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
** \brief Output text string
**
** \par Description
**        Output a text string to the log file or other target.
**
** \par Assumptions, External Events, and Notes:
**        If CFE_LINUX is not defined then a suitable target function for the
**        text output must be designated.
**
** \param[in] text  Pointer to character string to output
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
/* Output text string to log file */
void UT_Text(const char *text);

/*****************************************************************************/
/**
** \brief Output single test's pass/fail status
**
** \par Description
**        Out the results and description for a single test, and update the
**        pass/fail counters.
**
** \par Assumptions, External Events, and Notes:
**
** \param[in] test      Equals 0 if the test failed; non-zero if the
**                      test passed
**
** \param[in] fun_name  Character string pointer to the name of the function
**                      being tested
**
** \param[in] info      Character string pointer to the description of the test
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text
**
******************************************************************************/
void UT_Report(const char *file, uint32 line, bool test, const char *fun_name,
		       const char *info);

/*****************************************************************************/
/**
** \brief Test pass/fail summary
**
** \par Description
**        Output total number of tests passed and failed.
**
** \par Assumptions, External Events, and Notes:
**        If CFE_LINUX is defined then the output is also sent to the console
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text
**
******************************************************************************/
void UT_ReportFailures(void);

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
**
** \sa #CFE_SB_SetMsgId, #CFE_SB_SetCmdCode, #CFE_SB_SendMsg
**
******************************************************************************/
void UT_CallTaskPipe(void (*TaskPipeFunc)(CFE_SB_MsgPtr_t), CFE_SB_MsgPtr_t Msg, uint32 MsgSize,
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
int32 UT_SoftwareBusSnapshotHook(void *UserObj, int32 StubRetcode, uint32 CallCount,
        const UT_StubContext_t *Context);

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
void UT_SetAppID(int32 AppID_in);

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
void UT_SetStatusBSPResetArea(int32 status, uint32 Signature,
                              uint32 ClockSignal);

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
void UT_SetReadBuffer(void *Buff, int NumBytes);

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
void UT_SetReadHeader(void *Hdr, int NumBytes);

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
void UT_SetSizeofESResetArea(int32 Size);

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
uint8* UT_SetCDSSize(int32 Size);

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
void UT_DisplayPkt(CFE_SB_MsgPtr_t ptr, uint32 size);

/*****************************************************************************/
/**
** \brief Return the actual packet length
**
** \par Description
**        Return the actual packet length field from the specified packet.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in] MsgPtr  Message packet
**
** \returns
**        Returns a pointer to the SB packet length.
**
******************************************************************************/
/*uint16 TODO*/int16 UT_GetActualPktLenField(CFE_SB_MsgPtr_t MsgPtr);

/*****************************************************************************/
/**
** \brief Return the actual command code
**
** \par Description
**        Return the actual command code from the specified packet.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in] MsgPtr  Message packet
**
** \returns
**        Returns the actual SB packet command code.
**
******************************************************************************/
/*uint16 TODO*/uint8 UT_GetActualCmdCodeField(CFE_SB_MsgPtr_t MsgPtr);

/*****************************************************************************/
/**
** \brief Report and close any sockets found open
**
** \par Description
**        Determine if any sockets are open; if so, close them.  If UT_VERBOSE
**        is defined then output the socket status to the test log file.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void UT_CheckForOpenSockets(void);

/*****************************************************************************/
/**
** \brief Gets a reference to the CFE ES Reset Data Object
**
** \par Description
**        Some CFE test casess may need to check or modify the reset data
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

#endif /* __UT_STUBS_H_ */
