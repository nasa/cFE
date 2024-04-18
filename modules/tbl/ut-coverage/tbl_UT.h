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
 *    TBL unit test header
 *
 * References:
 *    1. cFE Application Developers Guide
 *    2. unit test standard 092503
 *    3. C Coding Standard 102904
 *
 * Notes:
 *    1. This is unit test code only, not for use in flight
 *
 */

#ifndef TBL_UT_H
#define TBL_UT_H

/*
** Includes
*/
#include <string.h>
#include "cfe_tbl_module_all.h"
#include "ut_support.h"

typedef struct
{
    uint32 TblElement1;
    uint32 TblElement2;
} UT_Table1_t;

typedef struct
{
    uint32 TblElement1;
    uint32 TblElement2;
    uint32 TblElement3;
} UT_Table2_t;

typedef struct
{
    CFE_TBL_File_Hdr_t TblHeader;
    UT_Table1_t        TblData;
} UT_TempFile_t;

/* TBL unit test functions */

/*****************************************************************************/
/**
** \brief Initialize the registry names
**
** \par Description
**        Fill the whole table registry with known table names in order to
**        detect when a table name is not registered, and set an owning
**        application ID for each table entry
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void UT_InitializeTableRegistryNames(void);

/*****************************************************************************/
/**
** \brief Initialize table registry values
**
** \returns
**        This function does not return a value.
******************************************************************************/
void UT_ResetTableRegistry(void);

/*****************************************************************************/
/**
** \brief Tests to cover table task initialization functions
**
** \par Description
**        This function tests the table task initialization functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CFE_TBL_TaskInit(void);

/*****************************************************************************/
/**
** \brief Test table service application data initialization
**
** \par Description
**        This function tests table service application data initialization.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CFE_TBL_InitData(void);

/*****************************************************************************/
/**
** \brief Test command handler table message ID (or command code) search
**        function
**
** \par Description
**        This function tests the command handler table message ID (or command
**        code) search function.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CFE_TBL_SearchCmdHndlrTbl(void);

/*****************************************************************************/
/**
** \brief Test the delete critical table's CDS command message function
**
** \par Description
**        This function tests the delete critical table's CDS command message
**        function.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CFE_TBL_DeleteCDSCmd(void);

/*****************************************************************************/
/**
** \brief Test the processing telemetry table registry command message function
**
** \par Description
**        This function tests the processing telemetry table registry command
**        message function.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CFE_TBL_TlmRegCmd(void);

/*****************************************************************************/
/**
** \brief Test the processing abort load command message function
**
** \par Description
**        This function tests the processing abort load command message
**        function.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CFE_TBL_AbortLoadCmd(void);

/*****************************************************************************/
/**
** \brief Test the activate table command message function
**
** \par Description
**        This function tests the activate table command message function.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CFE_TBL_ActivateCmd(void);

/*****************************************************************************/
/**
** \brief Test the write table data to a file function
**
** \par Description
**        This function tests the write table data to a file function.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CFE_TBL_DumpToFile(void);

/*****************************************************************************/
/**
** \brief Test the processing reset counters command message function
**
** \par Description
**        This function tests the processing reset counters command message
**        function.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CFE_TBL_ResetCmd(void);

/*****************************************************************************/
/**
** \brief Test the validate table command message function
**
** \par Description
**        This function tests the validate table command message function.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CFE_TBL_ValidateCmd(void);

/*****************************************************************************/
/**
** \brief Test the processing no-operation command message function
**
** \par Description
**        This function tests the processing no-operation command message
**        function.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CFE_TBL_NoopCmd(void);

/*****************************************************************************/
/**
** \brief Test the function which converts table registry entries for tables
**        into messages
**
** \par Description
**        This function tests the function which converts table registry
**        entries for tables into messages.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CFE_TBL_GetTblRegData(void);

/*****************************************************************************/
/**
** \brief Test the function that collects data and stores it in the
**        housekeeping message
**
** \par Description
**        This function tests the function that collects data and stores it in
**        the housekeeping message.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CFE_TBL_GetHkData(void);

/*****************************************************************************/
/**
** \brief Test the function that processes dump table registration to file
**        command message
**
** \par Description
**        This function tests the function that processes dump table
**        registration to file command message.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CFE_TBL_DumpRegCmd(void);

/*****************************************************************************/
/**
** \brief Test the function that processes dump table to file command message
**
** \par Description
**        This function tests the function that processes dump table to file
**        command message.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CFE_TBL_DumpCmd(void);

/*****************************************************************************/
/**
** \brief Test the function that processes load table file to buffer command
**        message
**
** \par Description
**        This function tests the function that processes load table file to
**        buffer command message.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CFE_TBL_LoadCmd(void);

/*****************************************************************************/
/**
** \brief Test the function that processes housekeeping request message
**
** \par Description
**        This function tests the function that processes housekeeping request
**        message.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CFE_TBL_SendHkCmd(void);

/*****************************************************************************/
/**
** \brief Prepare for test table API functions
**
** \par Description
**        Function to prepare for test table API functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CFE_TBL_ApiInit(void);

/*****************************************************************************/
/**
** \brief Test function that registers a table with cFE to obtain table
**        management services
**
** \par Description
**        This function tests the function that registers a table with cFE to
**        obtain table management services.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CFE_TBL_Register(void);

/*****************************************************************************/
/**
** \brief Test function that obtains the handle of table registered by another
**        application
**
** \par Description
**        This function tests the function that obtains the handle of table
**        registered by another application.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CFE_TBL_Share(void);

/*****************************************************************************/
/**
** \brief Function to test unregistering a previously registered table and
**        freeing associated resources
**
** \par Description
**        This function tests unregistering a previously registered table and
**        freeing associated resources.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CFE_TBL_Unregister(void);

/*****************************************************************************/
/**
** \brief Test function that instructs table services to notify calling
**        application whenever the specified table requires management
**
** \par Description
**        This function tests the function that instructs table services to
**        notify calling application whenever the specified table requires
**        management.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CFE_TBL_NotifyByMessage(void);

/*****************************************************************************/
/**
** \brief Function to test loading a specified table with data from the
**        specified source
**
** \par Description
**        This function tests the function for loading a specified table with
**        data from the specified source.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CFE_TBL_Load(void);

/*****************************************************************************/
/**
** \brief Function to test obtaining the current address of the contents
**        of the specified table
**
** \par Description
**        This function tests the function for obtaining the current address of
**        the contents of the specified table.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CFE_TBL_GetAddress(void);

/*****************************************************************************/
/**
** \brief Function to test release of a previously obtained pointer to the
**        contents of the specified table
**
** \par Description
**        This function tests the function for releasing a previously obtained
**        pointer to the contents of the specified table.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CFE_TBL_ReleaseAddress(void);

/*****************************************************************************/
/**
** \brief Test function that obtains the current addresses of the contents of
**        a collection of tables
**
** \par Description
**        This function tests the function that obtains the current addresses
**        of the contents of a collection of tables.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CFE_TBL_GetAddresses(void);

/*****************************************************************************/
/**
** \brief Test function that releases previously obtained pointers to the
**        contents of the specified tables
**
** \par Description
**        This function tests the function that releases previously obtained
**        pointers to the contents of the specified tables.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CFE_TBL_ReleaseAddresses(void);

/*****************************************************************************/
/**
** \brief Test function for validating the table image for a specified table
**
** \par Description
**        This function tests the function for validating the table image for a
**        specified table.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CFE_TBL_Validate(void);

/*****************************************************************************/
/**
** \brief Test function for performing standard table maintenance on the
**        specified table
**
** \par Description
**        This function tests the function for performing standard table
**        maintenance on the specified table.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CFE_TBL_Manage(void);

/*****************************************************************************/
/**
** \brief Test function for dumping to a buffer
**
** \par Description
**        This function tests the function for dumping a table to a buffer
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CFE_TBL_DumpToBuffer(void);

/*****************************************************************************/
/**
** \brief Test function that updates the contents of a table if an update
**        is pending
**
** \par Description
**        This function tests the function that updates the contents of a table
**        if an update is pending.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CFE_TBL_Update(void);

/*****************************************************************************/
/**
** \brief Test function that obtains the pending action status for specified
**        table
**
** \par Description
**        This function tests the function that obtains the pending action
**        status for specified table.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CFE_TBL_GetStatus(void);

/*****************************************************************************/
/**
** \brief Test function that obtains characteristics/information for a
**        specified table
**
** \par Description
**        This function tests the function that obtains characteristics/
**        information for a specified table.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CFE_TBL_GetInfo(void);

/*****************************************************************************/
/**
** \brief Test function that loads a specified table with data from the
**        specified source
**
** \par Description
**        This function tests the function that loads a specified table with
**        data from the specified source.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CFE_TBL_TblMod(void);

/*****************************************************************************/
/**
** \brief Tests for the remaining functions in cfe_tbl_internal.c
**
** \par Description
**        This function tests the remaining functions in cfe_tbl_internal.c.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CFE_TBL_Internal(void);

/*****************************************************************************/
/**
** \brief Test function executed when the contents of a table need to be
**        validated
**
** \par Description
**        This function tests the function executed when the contents of a
**        table need to be validated.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**      Always returns 0
**        This function does not return a value.
**
******************************************************************************/
int32 Test_CFE_TBL_ValidationFunc(void *TblPtr);

/* Test cases for resource ID access patterns based on shared resource types */
void Test_CFE_TBL_ResourceID_ValidationResult(void);
void Test_CFE_TBL_ResourceID_RegistryRecord(void);
void Test_CFE_TBL_ResourceID_AccessDescriptor(void);
void Test_CFE_TBL_ResourceID_DumpControl(void);

#endif /* TBL_UT_H */
