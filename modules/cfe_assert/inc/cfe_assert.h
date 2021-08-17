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
** File: cfe_assert.h
**
** Purpose:
**   Specification for the CFE assert (UT assert wrapper) functions.
**
*************************************************************************/

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
#define CFE_UtAssert_STATUS_OK(FN) \
    CFE_UtAssert_StatusCheck(FN, true, UTASSERT_CASETYPE_FAILURE, __FILE__, __LINE__, #FN)

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
#define CFE_UtAssert_STATUS_ERROR(FN) \
    CFE_UtAssert_StatusCheck(FN, false, UTASSERT_CASETYPE_FAILURE, __FILE__, __LINE__, #FN)

/*****************************************************************************/
/**
** \brief Macro to check CFE resource ID for equality
**
** \par Description
**        A macro that checks two resource ID values for equality.
**
** \par Assumptions, External Events, and Notes:
**        The generic #UtAssert_UINT32_EQ check should not be used, as ID values
**        and integers may not be interchangable with strict type checking.
**
******************************************************************************/
#define CFE_UtAssert_RESOURCEID_EQ(id1, id2)                                                                         \
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
#define CFE_UtAssert_RESOURCEID_UNDEFINED(id) \
    UtAssert_True(!CFE_RESOURCEID_TEST_DEFINED(id), "%s (0x%lx) not defined", #id, CFE_RESOURCEID_TO_ULONG(id))

/*****************************************************************************/
/**
** \brief Macro to check CFE memory size/offset for equality
**
** \par Description
**        A macro that checks two memory offset/size values for equality.
**
** \par Assumptions, External Events, and Notes:
**        This is a simple unsigned comparison which logs the values as hexadecimal
**
******************************************************************************/
#define CFE_UtAssert_MEMOFFSET_EQ(off1, off2)                                                                \
    UtAssert_GenericUnsignedCompare(off1, UtAssert_Compare_EQ, off2, UtAssert_Radix_HEX, __FILE__, __LINE__, \
                                    "Offset Check: ", #off1, #off2)

/*****************************************************************************/
/**
** \brief Macro to check CFE message ID for equality
**
** \par Description
**        A macro that checks two message ID values for equality.
**
** \par Assumptions, External Events, and Notes:
**        The generic #UtAssert_UINT32_EQ check should not be used, as CFE_SB_MsgId_t values
**        and integers may not be interchangable with strict type checking.
**
******************************************************************************/
#define CFE_UtAssert_MSGID_EQ(mid1, mid2)                                                                      \
    UtAssert_GenericUnsignedCompare(CFE_SB_MsgIdToValue(mid1), UtAssert_Compare_EQ, CFE_SB_MsgIdToValue(mid2), \
                                    UtAssert_Radix_HEX, __FILE__, __LINE__, "MsgId Check: ", #mid1, #mid2)

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
**  \return None
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
**
**  \return None
**
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
 *
 * \return None
 *
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
 *
 * \return None
 *
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
bool CFE_UtAssert_StatusCheck(CFE_Status_t Status, bool ExpectSuccess, UtAssert_CaseType_t CaseType, const char *File,
                              uint32 Line, const char *Text);

#endif /* CFE_ASSERT_H */
