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
 * Internal Declarations and prototypes for cfe_assert module
 */

#ifndef CFE_ASSERT_PRIV_H
#define CFE_ASSERT_PRIV_H

/************************************************************************
** Includes
*************************************************************************/
#include "common_types.h"
#include "cfe_assert.h"
#include "osconfig.h"
#include "cfe_mission_cfg.h"

/**
 * Maximum length of a single line in the test log file
 *
 * Note this only applies to the log file.  The user callback
 * may have other limitations.
 */
#define CFE_ASSERT_MAX_LOG_LINE_LENGTH 512

/**
 * State of the CFE assert library.
 *
 * Note that typically tests need to be deferred until the CFE system
 * reaches "operational" state. CFE assert has its own internal state
 * that needs to be managed as well.
 */
typedef enum
{
    CFE_Assert_State_INIT,    /**< Initial state prior to CFE_Assert_LibInit() */
    CFE_Assert_State_STARTUP, /**< cFE starting: successful CFE_Assert_LibInit(), but no tests run yet. */
    CFE_Assert_State_ACTIVE   /**< cFE operational: Normal test applications are allowed to run */
} CFE_Assert_State_Enum_t;

/************************************************************************
** Type Definitions
*************************************************************************/

typedef struct
{
    CFE_Assert_State_Enum_t LibState;

    /**
     * Verbosity of default (syslog) output
     *
     * This controls the type(s) of assert messages that will be written to syslog.
     * This only applies for default syslog output.  Use of a status callback
     * function overrides this.
     */
    uint32 CurrVerbosity;

    /**
     * Function to invoke to report test status
     */
    CFE_Assert_StatusCallback_t StatusCallback;

    /**
     * Name of final log file for test results
     *
     * The temporary file will be renamed to this at the end of testing
     */
    char LogFileFinal[OS_MAX_PATH_LEN];

    /**
     * Name of temporary log file for test results
     *
     * This is the file name that is actively written during the test
     */
    char LogFileTemp[OS_MAX_PATH_LEN];

    /**
     * Log File descriptor
     *
     * Should be set to OS_OBJECT_ID_UNDEFINED if no log file is open
     */
    osal_id_t LogFileDesc;

    /**
     * Mutex to control access to UtAssert structures.
     *
     * The UtAssert library is designed for single-threaded testing.  To use it
     * in a multi-threaded environment like CFE, it requires synchronization between
     * apps, such that only one test app registers/runs tests at a time.
     */
    osal_id_t AccessMutex;

    /**
     * AppID of the current UtAssert resource owner.
     *
     * Only one test application may use UtAssert facilities at a given time.
     * This records the AppID of the current owner.  It is set when AccessMutex
     * is first acquired, and cleared once the tests have executed and the
     * resource becomes available to another app.
     */
    CFE_ES_AppId_t OwnerAppId;

    /**
     * Name of current test set being prepared/executed.
     *
     * This is set when the AccessMutex is first acquired, and cleared when
     * the mutex is released.  It is a free-form string to indicate the owner,
     * and may or may not match the app name (i.e. test apps may implement
     * more than one test set).
     */
    char CurrentTestName[CFE_MISSION_MAX_API_LEN];

    /* The following members support the "Deferred" assert feature */

    /**
     * Actual CFE status value from a previous function call
     */
    CFE_Status_t StoredStatus;

    /**
     * Full text of previous function call that produced "StoredStatus"
     */
    char StoredText[CFE_ASSERT_MAX_LOG_LINE_LENGTH];

    /**
     * File name of source file that produced "StoredStatus"
     */
    char StoredFile[CFE_MISSION_MAX_PATH_LEN];

    /**
     * Line number of source file that produced "StoredStatus"
     */
    uint32 StoredLine;
} CFE_Assert_Global_t;

extern CFE_Assert_Global_t CFE_Assert_Global;

#endif /* CFE_ASSERT_PRIV_H */
