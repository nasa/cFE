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
 * Declarations and prototypes for cfe_assert module
 */

#ifndef CFE_ASSERT_H
#define CFE_ASSERT_H

/************************************************************************
** Includes
*************************************************************************/
#include "common_types.h"
#include "cfe_es_api_typedefs.h"
#include "utassert.h"
#include "cfe_error.h"

/************************************************************************
** Type Definitions
*************************************************************************/

typedef void (*CFE_Assert_StatusCallback_t)(uint8 MessageType, const char *Prefix, const char *OutputMessage);

/*************************************************************************
** CFE-specific assertion macros
** (similar to macros in the CFE coverage test)
*************************************************************************/

/*****************************************************************************/
/**
** \brief Asserts the nominal execution of the function being tested.
**
** \par Description
**        The core of each unit test is the execution of the function being tested.
**        This function and macro should be used to test the nominal execution of the
**        function; the expectation is that it will return CFE_SUCCESS or an
**        unspecified positive value.
**
** \par Assumptions, External Events, and Notes:
**        None
**
******************************************************************************/
#define CFE_Assert_STATUS_OK(FN) CFE_Assert_StatusCheck(FN, true, UTASSERT_CASETYPE_FAILURE, __FILE__, __LINE__, #FN)

/*****************************************************************************/
/**
** \brief Asserts the off-nominal execution of the function being tested.
**
** \par Description
**        The core of each unit test is the execution of the function being tested.
**        This function and macro should be used to test the generic off-nominal execution
**        of the function; the expectation is that it will return an unspecified negative
**        value.
**
** \par Assumptions, External Events, and Notes:
**        This should be used in cases where a specific error for a particular condition
**        is not known/documented.  Whenever a specific error is indicated by the documentation,
**        tests should check for that error instead of using this.
**
******************************************************************************/
#define CFE_Assert_STATUS_ERROR(FN) \
    CFE_Assert_StatusCheck(FN, false, UTASSERT_CASETYPE_FAILURE, __FILE__, __LINE__, #FN)

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
#define CFE_Assert_RESOURCEID_EQ(id1, id2)                                                                           \
    UtAssert_GenericUnsignedCompare(CFE_RESOURCEID_TO_ULONG(id1), UtAssert_Compare_EQ, CFE_RESOURCEID_TO_ULONG(id2), \
                                    UtAssert_Radix_HEX, __FILE__, __LINE__, "Resource ID Check: ", #id1, #id2)

/*****************************************************************************/
/**
** \brief Check if a Resource ID is Undefined
**
** \par Description
**        A macro that checks if resource ID value is undefined.
**
** \par Assumptions, External Events, and Notes:
**        This utilizes the "TEST_DEFINED" macro provided by the resourceid module, as the
**        set of undefined IDs is more than the single value of CFE_RESOURCEID_UNDEFINED.
**
******************************************************************************/
#define CFE_Assert_RESOURCEID_UNDEFINED(id) \
    UtAssert_True(!CFE_RESOURCEID_TEST_DEFINED(id), "%s (0x%lx) not defined", #id, CFE_RESOURCEID_TO_ULONG(id))

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
#define CFE_Assert_MSGID_EQ(mid1, mid2)                                                                        \
    UtAssert_GenericUnsignedCompare(CFE_SB_MsgIdToValue(mid1), UtAssert_Compare_EQ, CFE_SB_MsgIdToValue(mid2), \
                                    UtAssert_Radix_HEX, __FILE__, __LINE__, "MsgId Check: ", #mid1, #mid2)

/*****************************************************************************/
/**
** \brief Calls a function that returns CFE_Status_t with deferred validation
**
** \par Description
**        The typical method of using #UtAssert_INT32_EQ to validate the result of a CFE call
**        will both invoke the function and check the result in a single macro.  However, this
**        requires advance knowledge of what the result is supposed to be, before the call is made.
**
**        This macro does invokes the function like the other macros do, but does _not_ check the
**        return value.  Rather, it  stores the return the value in a local buffer for later checking,
**        using the #CFE_Assert_STATUS_MAY_BE or #CFE_Assert_STATUS_MUST_BE macros.
**
** \par Assumptions, External Events, and Notes:
**        In some functional test circumstances, particularly where the test is not being run
**        in a clean/isolated environment, it may not always be feasible to predict the correct
**        return code from an API call.
**
**        In these cases, the test program will need to check the result of the call itself,
**        typically by storing the result on the stack and checking it for correctness by
**        reading system state as necessary.
**
**        While the normal UtAssert_INT32_EQ assertion macro can still be used to retroactively check
**        any status variable value (including a value on the stack from a previous API call), this
**        will not include the "full text" of the API call, and so the test log will not reflect the
**        full call details.
**
**        The pair of macros (CFE_Assert_STATUS_STORE and CFE_Assert_STATUS_WAS) can be used in these
**        circumstances, to call an API function and record the full text of the call, when the expected
**        status is not fully known, but still confirm the status was correct once it is known.
**
** \sa #CFE_Assert_STATUS_MAY_BE, #CFE_Assert_STATUS_MUST_BE
**
** \returns Actual CFE_Status_t value from the call
*/
#define CFE_Assert_STATUS_STORE(FN) CFE_Assert_Status_Store(FN, __FILE__, __LINE__, #FN)

/**
** \brief Check if the stored status matches a possible value
**
** Allows the caller to check the value from a previous invocation of CFE_Assert_STATUS_STORE()
** without directly asserting on the value or changing test state.
**
** This may be useful in situations where a large volume of tests are being performed, such as
** during performance or load testing, where reporting all "PASS" cases may add signficant
** extra CPU usage and log volume, interfering with the result.  This macro can be followed
** or combined with CFE_Assert_STATUS_MAY_BE / CFE_Assert_STATUS_MUST_BE to do actual reporting.

** \sa #CFE_Assert_STATUS_STORE, #CFE_Assert_STATUS_MAY_BE, #CFE_Assert_STATUS_MUST_BE
**
** \returns Actual CFE_Status_t value from the call
*/
#define CFE_Assert_STATUS_SILENTCHECK(expected) CFE_Assert_Status_SilentCheck(expected)

/*****************************************************************************/
/**
** \brief Retroactively check for an acceptable status value from CFE_Assert_STATUS_STORE
**
** \par Description
**        The typical method of using #UtAssert_INT32_EQ to validate the result of a CFE call
**        will both invoke the function and check the result in a single macro.  However, this
**        requires advance knowledge of what the result is supposed to be, before the call is made.
**
**        This retroactive macro does _not_ invoke any function, but rather checks the stored status
**        from a previous call to #CFE_Assert_STATUS_STORE.  It should be used for each status
**        code that should be considered acceptable from the previous function call.
**
** \par Assumptions, External Events, and Notes:
**
**        While the normal UtAssert_INT32_EQ assertion macro can still be used to check any
**        status variable value (including a value on the stack from a previous API call), this
**        will not include the "full text" of the API call.  This macro is intended for those cases
**        where it is desired to log the full text (function + arguments) of the API call, but
**        when the call has already been made and the status value is stored in a local variable.
**
** \sa #CFE_Assert_STATUS_STORE, #CFE_Assert_STATUS_MUST_BE
**
** \returns Boolean pass/fail status
**
******************************************************************************/
#define CFE_Assert_STATUS_MAY_BE(expected) \
    CFE_Assert_Status_DeferredCheck(expected, UTASSERT_CASETYPE_FLOW, __FILE__, __LINE__, #expected)

/*****************************************************************************/
/**
** \brief Retroactively check for a required status value from CFE_Assert_STATUS_STORE
**
** \par Description
**        The typical method of using #UtAssert_INT32_EQ to validate the result of a CFE call
**        will both invoke the function and check the result in a single macro.  However, this
**        requires advance knowledge of what the result is supposed to be, before the call is made.
**
**        This retroactive macro does _not_ invoke any function, but rather checks the stored status
**        from a previous call to #CFE_Assert_STATUS_STORE.  This should be used as the final
**        assertion, after checking for other acceptable values via #CFE_Assert_STATUS_MAY_BE.
**
** \par Assumptions, External Events, and Notes:
**
**        While the normal UtAssert_INT32_EQ assertion macro can still be used to check any
**        status variable value (including a value on the stack from a previous API call), this
**        will not include the "full text" of the API call.  This macro is intended for those cases
**        where it is desired to log the full text (function + arguments) of the API call, but
**        when the call has already been made and the status value is stored in a local variable.
**
** \sa #CFE_Assert_STATUS_STORE, #CFE_Assert_STATUS_MAY_BE
**
** \returns Boolean pass/fail status
**
******************************************************************************/
#define CFE_Assert_STATUS_MUST_BE(expected) \
    CFE_Assert_Status_DeferredCheck(expected, UTASSERT_CASETYPE_FAILURE, __FILE__, __LINE__, #expected)

/*************************************************************************
** Exported Functions
*************************************************************************/

/************************************************************************/
/** \brief Library Init Function
**
**  \par Description
**        This function should be specified in the cfe_es_startup.scr file
**        as part of loading this library.
**
**  \par Assumptions, External Events, and Notes:
**        None
**
**  \return #CFE_SUCCESS if successful, or error code
**
*************************************************************************/
int32 CFE_Assert_LibInit(CFE_ES_LibId_t LibId);

/************************************************************************/
/** \brief Start Test
**
**  \par Description
**
**  \par Assumptions, External Events, and Notes:
**        Must be followed by a call to CFE_Assert_ExecuteTest()
**
**  \return #CFE_SUCCESS if successful, or error code
**
*************************************************************************/
int32 CFE_Assert_RegisterTest(const char *TestName);

/************************************************************************/
/** \brief Execute Test and Exit
**
**  \par Description
**
**  \par Assumptions, External Events, and Notes:
**        None
*************************************************************************/
void CFE_Assert_ExecuteTest(void);

/************************************************************************/
/** \brief Register a test status callback
 *
 *  \par Description
 *        This user-supplied function will be invoked with the status
 *        of each test and the associated message.  It may be used to
 *        write the test messages to a location other than CFE ES Syslog.
 *
 *  \par Assumptions, External Events, and Notes:
 *        None
 *
 * \param[in] Callback  Callback function to invoke after every test
 */
void CFE_Assert_RegisterCallback(CFE_Assert_StatusCallback_t Callback);

/************************************************************************/
/** \brief Start a test log file
 *
 *  \par Description
 *        Sets the name of a file which will store the results of all tests
 *        The output is saved to the log file in addition to the normal callback
 *        function provided in CFE_Assert_RegisterCallback().
 *
 *  \par Assumptions, External Events, and Notes:
 *        Only the test outputs are saved to this log file, thereby providing
 *        a file that can be checked by a script.  During test operation, the
 *        file is first created with a "tmp" extension, and then will be renamed
 *        to the name given here once the test is complete.
 *
 * \param[in] Filename  Name of log file to write
 *
 * \return CFE Status code
 * \retval #CFE_SUCCESS if file was opened successfully
 *
 */
int32 CFE_Assert_OpenLogFile(const char *Filename);

/************************************************************************/
/** \brief Complete a test log file
 *
 *  \par Description
 *        Closes the test log file that was previously opened via CFE_Assert_OpenLogFile
 *
 *  \par Assumptions, External Events, and Notes:
 *        This should be called once test cases have completed
 */
void CFE_Assert_CloseLogFile(void);

/*****************************************************************************/
/**
** \brief Helper function for nominal CFE calls
**
** \par Description
**        This helper function wraps the normal UtAssert function, intended for verifying
**        CFE API calls that are expected to return successfully.
**
**        Note that this checks for a logical "success", which includes the specific #CFE_SUCCESS
**        status code, as well as all other status codes that represent a successful completion
**        of the function objectives (i.e. such as a nonzero size, from functions that return a
**        size).
**
**        This can also be used to report with an alternative pass/fail marker by passing the CaseType
**        parameter appropriately.
**
** \par Assumptions, External Events, and Notes:
**        Note this will accept any non-negative value as logical "success", so it
**        also works with functions that return a size or other non-error status.
**
** \returns Test pass status, returns true if status was successful, false if it failed.
**
******************************************************************************/
bool CFE_Assert_StatusCheck(CFE_Status_t Status, bool ExpectSuccess, UtAssert_CaseType_t CaseType, const char *File,
                            uint32 Line, const char *Text);

/*****************************************************************************/
/**
** \brief Helper function for nominal CFE calls with deferred check
**
** \par Description
**        This helper function will store the status into a temporary holding area,
**        but _not_ assert on any specific value.
**
** \par Assumptions, External Events, and Notes:
**        This facility should only be used by one task at a time.  Normally tests
**        are single-threaded, and CFE assert will serialize test apps, so this is
**        not a concern in the typical test environment.
**
**        However, if a particular test case uses child tasks, then the programmer must
**        explicitly ensure that only one task uses this facility at a time.
**
** \returns Status value (pass through)
*/
CFE_Status_t CFE_Assert_Status_Store(CFE_Status_t Status, const char *File, uint32 Line, const char *Text);

/**
** \brief Helper function to silently check status of a previously stored result
**
** \par Description
**        This helper function will check the status previously stored to a
**        temporary holding area, but does not assert on it.
**
** \returns true if status code matched, false if it did not match.
*/
bool CFE_Assert_Status_SilentCheck(CFE_Status_t Status);

/**
** \brief Helper function for nominal CFE calls with deferred check
**
** \par Description
**        This helper function will assert on the status previously stored to a
**        temporary holding area.
**
** \par Assumptions, External Events, and Notes:
**        This facility should only be used by one task at a time.  Normally tests
**        are single-threaded, and CFE assert will serialize test apps, so this is
**        not a concern in the typical test environment.
**
**        However, if a particular test case uses child tasks, then the programmer must
**        explicitly ensure that only one task uses this facility at a time.
**
** \returns Test pass status, returns true if status was successful, false if it failed.
*/
bool CFE_Assert_Status_DeferredCheck(CFE_Status_t Status, UtAssert_CaseType_t CaseType, const char *File, uint32 Line,
                                     const char *Text);

#endif /* CFE_ASSERT_H */
