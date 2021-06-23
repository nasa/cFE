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

/************************************************************************
** Type Definitions
*************************************************************************/

typedef void (*CFE_Assert_StatusCallback_t)(uint8 MessageType, const char *Prefix, const char *OutputMessage);

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

#endif /* CFE_ASSERT_H */
