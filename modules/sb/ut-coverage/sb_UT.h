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
 *    SB unit test header
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

#ifndef SB_UT_H
#define SB_UT_H

/*
** Includes
*/
#include <string.h>
#include "cfe_sb_module_all.h"
#include "ut_support.h"

/*
** Structures
*/
typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader;
    uint32                  Cmd32Param1;
    uint16                  Cmd16Param1;
    uint16                  Cmd16Param2;
    uint8                   Cmd8Param1;
    uint8                   Cmd8Param2;
    uint8                   Cmd8Param3;
    uint8                   Cmd8Param4;
} SB_UT_Test_Cmd_t;

typedef struct
{
    CFE_MSG_TelemetryHeader_t TelemetryHeader;
    uint32                    Tlm32Param1;
    uint16                    Tlm16Param1;
    uint16                    Tlm16Param2;
    uint8                     Tlm8Param1;
    uint8                     Tlm8Param2;
    uint8                     Tlm8Param3;
    uint8                     Tlm8Param4;
} SB_UT_Test_Tlm_t;

typedef struct
{
    CFE_MSG_Message_t Message;
    uint8             Tlm8Param1;
    uint8             Tlm8Param2;
    uint32            Tlm32Param1;
    uint16            Tlm16Param1;
    uint16            Tlm16Param2;
} SB_UT_TstPktWoSecHdr_t;

#define SB_UT_CMD_MID_VALUE_BASE 0x100
#define SB_UT_TLM_MID_VALUE_BASE 0x200

/* SB unit test functions */
/*****************************************************************************/
/**
** \brief Initialize settings in preparation for the next unit test
**
** \par Description
**        This function initializes settings in preparation for the next unit
**        test.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void SB_ResetUnitTest(void);

/*****************************************************************************/
/**
** \brief Function for calling SB application test functions
**
** \par Description
**        Function for calling SB application test functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_App(void);

/*****************************************************************************/
/**
** \brief Function for calling SB application initialization
**        test functions
**
** \par Description
**        Function for calling SB application initialization
**        test functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_AppInit(void);

/*****************************************************************************/
/**
** \brief Test task init with EVS_Register returning error
**
** \par Description
**        This function tests task init with EVS_Register returning error.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_AppInit_EVSRegFail(void);

/*****************************************************************************/
/**
** \brief Test task init with EVS_SendEvent returning error on task init event
**
** \par Description
**        This function tests task init with EVS_SendEvent returning error on
**        task init event.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_AppInit_EVSSendEvtFail(void);

/*****************************************************************************/
/**
** \brief Test task init with pipe create failure
**
** \par Description
**        This function tests task init with pipe create failure.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_AppInit_CrPipeFail(void);

/*****************************************************************************/
/**
** \brief Test task init with a failure on first subscription request
**
** \par Description
**        This function tests task init with a failure on first subscription
**        request.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_AppInit_Sub1Fail(void);

/*****************************************************************************/
/**
** \brief Test task init with a failure on second subscription request
**
** \par Description
**        This function tests task init with a failure on second subscription
**        request.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_AppInit_Sub2Fail(void);

/*****************************************************************************/
/**
** \brief Test task init with a failure on third subscription request
**
** \par Description
**        This function tests task init with a failure on third subscription
**        request.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_AppInit_Sub3Fail(void);

/*****************************************************************************/
/**
** \brief Test task init with a GetPool failure
**
** \par Description
**        This function tests task init with a GetPool failure.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_AppInit_GetPoolFail(void);

/*****************************************************************************/
/**
** \brief Test task init with a PutPool failure
**
** \par Description
**        This function tests task init with a PutPool failure.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_AppInit_PutPoolFail(void);

/*****************************************************************************/
/**
** \brief Function for calling SB main task test functions
**
** \par Description
**        Function for calling SB main task test functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_MainRoutine(void);

/*****************************************************************************/
/**
** \brief Test main task with a packet receive error
**
** \par Description
**        This function tests main task with a packet receive error.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_Main_RcvErr(void);

/*****************************************************************************/
/**
** \brief Test main task with an application initialization error
**
** \par Description
**        This function tests main task with an application initialization
**        error.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_Main_InitErr(void);

/*****************************************************************************/
/**
** \brief Test main task nominal path
**
** \par Description
**        This function tests main task that gets a command successfully
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_Main_Nominal(void);

/*****************************************************************************/
/**
** \brief Function for calling SB command test functions
**
** \par Description
**        Function for calling SB command test functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_Cmds(void);

/*****************************************************************************/
/**
** \brief Test no-op command
**
** \par Description
**        This function tests the no-op command.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_Cmds_Noop(void);

/*****************************************************************************/
/**
** \brief Test reset counters command
**
** \par Description
**        This function tests the reset counters command.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_Cmds_RstCtrs(void);

/*****************************************************************************/
/**
** \brief Test send SB stats command
**
** \par Description
**        This function tests the end SB stats command.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_Cmds_Stats(void);

/*****************************************************************************/
/**
** \brief Test send routing information command default/nominal path
**
** \par Description
**        This function tests the send routing information command using the
**        default file name.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_Cmds_RoutingInfoDef(void);

/*****************************************************************************/
/**
** \brief Test send routing information command with request already pending
**
** \par Description
**        This function tests the send routing information command using a
**        specified file name.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_Cmds_RoutingInfoAlreadyPending(void);

/*****************************************************************************/
/**
** \brief Test routing information data getter
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_Cmds_RoutingInfoDataGetter(void);

/*****************************************************************************/
/**
** \brief Function for calling SB pipe opts API test functions
**
** \par Description
**        Function for calling SB pipe opts API test functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_PipeOpts_API(void);

/*****************************************************************************/
/**
** \brief Test setting pipe options with invalid pipe ID.
**
** \par Description
**        This function tests the set pipe options command with an invalid
**        pipe id.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SetPipeOpts_BadID(void);

/*****************************************************************************/
/**
** \brief Test setting pipe opts with the app not being the owner of the pipe.
**
** \par Description
**        This function tests the set pipe options command with the
**        app ID not being the owner of the pipe.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SetPipeOpts_NotOwner(void);

/*****************************************************************************/
/**
** \brief Test setting pipe options with a valid pipe ID.
**
** \par Description
**        This function tests the set pipe options command with a valid
**        pipe id.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SetPipeOpts(void);

/*****************************************************************************/
/**
** \brief Test getting pipe options with invalid pipe ID.
**
** \par Description
**        This function tests the get pipe options command with an invalid
**        pipe id.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_GetPipeOpts_BadID(void);

/*****************************************************************************/
/**
** \brief Test getting pipe opts with an invalid options pointer.
**
** \par Description
**        This function tests the get pipe options command with the
**        options pointer being NULL.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_GetPipeOpts_BadPtr(void);

/*****************************************************************************/
/**
** \brief Test getting pipe options with a valid pipe ID and opts ptr.
**
** \par Description
**        This function tests the get pipe options command with a valid
**        pipe id and options pointer.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_GetPipeOpts(void);

/*****************************************************************************/
/**
** \brief Function for calling SB get pipe name by id API test functions
**
** \par Description
**        Function for calling SB get pipe name by id API test functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_GetPipeName_API(void);

/*****************************************************************************/
/**
** \brief Test get pipe name by id response to a null pointer for name buf
**
** \par Description
**        This function tests the get pipe name by id command with
**        a null pointer for the name buffer.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_GetPipeName_NullPtr(void);

/*****************************************************************************/
/**
** \brief Test get pipe name by id response to a non-existent pipe id
**
** \par Description
**        This function tests the get pipe name by id command when
**        called with an id that doesn't exist.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_GetPipeName_InvalidId(void);

/*****************************************************************************/
/**
** \brief Test getting pipe name by id with a valid id.
**
** \par Description
**        This function tests the get pipe name by id command with a valid
**        id.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_GetPipeName(void);

/*****************************************************************************/
/**
** \brief Function for calling SB get pipe id by name  API test functions
**
** \par Description
**        Function for calling SB get pipe id by name API test functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_GetPipeIdByName_API(void);

/*****************************************************************************/
/**
** \brief Test get pipe id by name response to a null pointer for name or pipe id
**
** \par Description
**        This function tests the get pipe id by name command with either
**        a null pointer for the name or a null pointer for the ID output.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_GetPipeIdByName_NullPtrs(void);

/*****************************************************************************/
/**
** \brief Test get pipe id by name response to a non-existent pipe name
**
** \par Description
**        This function tests the get pipe id by name command when
**        called with a name that doesn't exist.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_GetPipeIdByName_InvalidName(void);

/*****************************************************************************/
/**
** \brief Test getting pipe id by name with a valid name.
**
** \par Description
**        This function tests the get pipe id by name command with a valid
**        name.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_GetPipeIdByName(void);

/*****************************************************************************/
/**
** \brief Test send pipe information command default / nominal path
**
** \par Description
**        This function tests the send pipe information command using the
**        default file name.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_Cmds_PipeInfoDef(void);

/*****************************************************************************/
/**
** \brief Test send pipe information command when already pending
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_Cmds_PipeInfoAlreadyPending(void);

/*****************************************************************************/
/**
** \brief Test pipe information data getter
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_Cmds_PipeInfoDataGetter(void);

/*****************************************************************************/
/**
** \brief Test background file writer event handler
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_Cmds_BackgroundFileWriteEvents(void);

/*****************************************************************************/
/**
** \brief Test send map information command using the defaults / nominal path
**
** \par Description
**        This function tests the send map information command using the
**        default file name.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_Cmds_MapInfoDef(void);

/*****************************************************************************/
/**
** \brief Test send map information command when already pending
**
** \par Description
**        This function tests the send map information command using a
**        specified file name.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_Cmds_MapInfoAlreadyPending(void);

/*****************************************************************************/
/**
** \brief Test map information data getter function
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_Cmds_MapInfoDataGetter(void);

/*****************************************************************************/
/**
** \brief Test command to enable a specific route using a valid route
**
** \par Description
**        This function tests the command to enable a specific route using a
**        valid route.  The requested pipe is defined and subscribed.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_Cmds_EnRouteValParam(void);

/*****************************************************************************/
/**
** \brief Test command to enable a specific route using a non-existent route
**
** \par Description
**        This function tests the command to enable a specific route using a
**        non-existent route.  The requested pipe is defined but not
**        subscribed.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_Cmds_EnRouteNonExist(void);

/*****************************************************************************/
/**
** \brief Test command to enable a specific route using an invalid pipe ID
**
** \par Description
**        This function tests the command to enable a specific route using an
**        invalid pipe ID (the requested pipe is undefined).
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_Cmds_EnRouteInvParam(void);

/*****************************************************************************/
/**
** \brief Test command to enable a specific route using an invalid message ID
**
** \par Description
**        This function tests the command to enable a specific route using an
**        invalid message ID (= CFE_SB_INVALID_MSG_ID).
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_Cmds_EnRouteInvParam2(void);

/*****************************************************************************/
/**
** \brief Test command to enable a specific route using a message ID greater
**        than the maximum allowed
**
** \par Description
**        This function tests the command to enable a specific route using a
**        message ID greater than the maximum allowed
**        (CFE_PLATFORM_SB_HIGHEST_VALID_MSGID).
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_Cmds_EnRouteInvParam3(void);

/*****************************************************************************/
/**
** \brief Test command to disable a specific route using a valid route
**
** \par Description
**        This function tests the command to disable a specific route using a
**        valid route.  The requested pipe is defined and subscribed.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_Cmds_DisRouteValParam(void);

/*****************************************************************************/
/**
** \brief Test command to disable a specific route using an invalid pipe ID
**
** \par Description
**        This function tests the command to disable a specific route using an
**        invalid pipe ID.  The requested pipe is undefined.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_Cmds_DisRouteNonExist(void);

/*****************************************************************************/
/**
** \brief Test command to disable a specific route using an invalid pipe ID
**
** \par Description
**        This function tests the command to disable a specific route using an
**        invalid pipe ID.  The requested pipe is undefined.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_Cmds_DisRouteInvParam(void);

/*****************************************************************************/
/**
** \brief Test command to disable a specific route using an invalid message ID
**
** \par Description
**        This function tests the command to disable a specific route using an
**        invalid message ID (= CFE_SB_INVALID_MSG_ID).
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_Cmds_DisRouteInvParam2(void);

/*****************************************************************************/
/**
** \brief Test command to disable a specific route using a message ID greater
**        than the maximum allowed
**
** \par Description
**        This function tests the command to disable a specific route using a
**        message ID greater than the maximum allowed
**        (CFE_PLATFORM_SB_HIGHEST_VALID_MSGID).
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_Cmds_DisRouteInvParam3(void);

/*****************************************************************************/
/**
** \brief Test send housekeeping information command
**
** \par Description
**        This function tests the send housekeeping information command.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_Cmds_SendHK(void);

/*****************************************************************************/
/**
** \brief Test command to build and send a SB packet containing a complete
**        list of current subscriptions
**
** \par Description
**        This function tests the command to build and send a SB packet
**        containing a complete list of current subscriptions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_Cmds_SendPrevSubs(void);

/*****************************************************************************/
/**
** \brief Test command to enable subscription reporting
**
** \par Description
**        This function tests the command to enable subscription reporting.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_Cmds_SubRptOn(void);

/*****************************************************************************/
/**
** \brief Test command to disable subscription reporting
**
** \par Description
**        This function tests the command to disable subscription reporting.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_Cmds_SubRptOff(void);

/*****************************************************************************/
/**
** \brief Test command handler response to an invalid command code
**
** \par Description
**        This function tests the command handler response to an invalid
**        command code in the CFE_SB_CMD_MID handler.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_Cmds_CmdUnexpCmdCode(void);

/*****************************************************************************/
/**
** \brief Test command handler response to a bad message length
**
** \par Description
**        This function tests the command handler response to an invalid
**        command length.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_Cmds_BadCmdLength(void);

/*****************************************************************************/
/**
** \brief Test command handler response to an invalid message ID
**
** \par Description
**        This function tests the command handler response to an invalid
**        message ID.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_Cmds_UnexpMsgId(void);

/*****************************************************************************/
/**
** \brief Function for calling SB early initialization, API, and utility
**        test functions
**
** \par Description
**        Function for calling SB early initialization, API, and utility
**        test functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_Lib(void);

/*****************************************************************************/
/**
** \brief Function for calling early initialization tests
**
** \par Description
**        Function for calling early initialization tests.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_EarlyInit(void);

/*****************************************************************************/
/**
** \brief Test early initialization response to a semaphore create failure
**
** \par Description
**        This function tests the early initialization response to a semaphore
**        create failure.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_EarlyInit_SemCreateError(void);

/*****************************************************************************/
/**
** \brief Test early initialization response to a pool create ex failure
**
** \par Description
**        This function tests the early initialization response to a pool
**        create ex failure.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_EarlyInit_PoolCreateError(void);

/*****************************************************************************/
/**
** \brief Test successful early initialization
**
** \par Description
**        This function tests successful early initialization.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_EarlyInit_NoErrors(void);

/*****************************************************************************/
/**
** \brief Function for calling SB API test functions
**
** \par Description
**        Function for calling SB API test functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_APIs(void);

/*****************************************************************************/
/**
** \brief Function for calling SB create pipe API test functions
**
** \par Description
**        TFunction for calling SB create pipe API test functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CreatePipe_API(void);

/*****************************************************************************/
/**
** \brief Test create pipe response to a null pipe ID pointer
**
** \par Description
**        This function tests the create pipe response to a null pipe ID
**        pointer.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CreatePipe_NullPtr(void);

/*****************************************************************************/
/**
** \brief Test create pipe response to valid pipe depths
**
** \par Description
**        This function tests the create pipe response to valid pipe depths.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CreatePipe_ValPipeDepth(void);

/*****************************************************************************/
/**
** \brief Test create pipe response to invalid pipe depths
**
** \par Description
**        This function tests the create pipe response to invalid pipe depths.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CreatePipe_InvalPipeDepth(void);

/*****************************************************************************/
/**
** \brief Test create pipe response to an empty pipe name
**
** \par Description
**        This function tests the create pipe response to an empty pipe name.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CreatePipe_EmptyPipeName(void);

/*****************************************************************************/
/**
** \brief Test create pipe response to a NULL pipe name
**
** \par Description
**        This function tests the create pipe response to a null pipe name pointer.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CreatePipe_PipeName_NullPtr(void);

/*****************************************************************************/
/**
** \brief Test create pipe response to a pipe name longer than allowed
**
** \par Description
**        This function tests the create pipe response to a pipe name longer
**        than allowed.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CreatePipe_LongPipeName(void);

/*****************************************************************************/
/**
** \brief Test create pipe response to duplicate pipe names
**
** \par Description
**        This function tests the create pipe response to duplicate pipe names.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CreatePipe_SamePipeName(void);

/*****************************************************************************/
/**
** \brief Test create pipe response to too many pipes
**
** \par Description
**        This function tests the create pipe response to too many pipes.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CreatePipe_MaxPipes(void);

/*****************************************************************************/
/**
** \brief Function for calling SB delete pipe API test functions
**
** \par Description
**        Function for calling SB delete pipe API test functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_DeletePipe_API(void);

/*****************************************************************************/
/**
** \brief Test delete pipe response to deleting a pipe with no subscriptions
**
** \par Description
**        This function tests the delete pipe response to deleting a pipe with
**        no subscriptions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_DeletePipe_NoSubs(void);

/*****************************************************************************/
/**
** \brief Test delete pipe response to deleting a pipe with subscriptions
**
** \par Description
**        This function tests the delete pipe response to deleting a pipe with
**        subscriptions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_DeletePipe_WithSubs(void);

/*****************************************************************************/
/**
** \brief Test delete pipe response to an invalid pipe ID
**
** \par Description
**        This function tests the delete pipe response to an invalid pipe ID.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_DeletePipe_InvalidPipeId(void);

/*****************************************************************************/
/**
** \brief Test delete pipe response to an invalid pipe owner
**
** \par Description
**        This function tests the delete pipe response to an invalid pipe
**        owner.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_DeletePipe_InvalidPipeOwner(void);

/*****************************************************************************/
/**
** \brief Test successful pipe delete with app ID
**
** \par Description
**        This function tests successful pipe delete with app ID.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_DeletePipe_WithAppid(void);

/*****************************************************************************/
/**
** \brief Function for calling SB subscribe API test functions
**
** \par Description
**        Function for calling SB subscribe API test functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_Subscribe_API(void);

/*****************************************************************************/
/**
** \brief Test API to globally subscribe to a message
**
** \par Description
**        This function tests globally subscribing to a message.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_Subscribe_SubscribeEx(void);

/*****************************************************************************/
/**
** \brief Test message subscription response to an invalid pipe ID
**
** \par Description
**        This function tests the message subscription response to an invalid
**        pipe ID.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_Subscribe_InvalidPipeId(void);

/*****************************************************************************/
/**
** \brief Test message subscription response to an invalid message ID
**
** \par Description
**        This function tests the message subscription response to an invalid
**        message ID.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_Subscribe_InvalidMsgId(void);

/*****************************************************************************/
/**
** \brief Test message subscription response to the maximum message limit
**
** \par Description
**        This function tests the message subscription response to the maximum
**        message limit.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_Subscribe_MaxMsgLim(void);

/*****************************************************************************/
/**
** \brief Test message subscription response to a duplicate subscription
**
** \par Description
**        This function tests the message subscription response to a duplicate
**        subscription.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_Subscribe_DuplicateSubscription(void);

/*****************************************************************************/
/**
** \brief Test API to locally subscribe to a message
**
** \par Description
**        This function tests locally subscribing to a message.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_Subscribe_LocalSubscription(void);

/*****************************************************************************/
/**
** \brief Test message subscription response to reaching the maximum
**        destination count
**
** \par Description
**        This function tests the message subscription response to reaching the
**        maximum destination count.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_Subscribe_MaxDestCount(void);

/*****************************************************************************/
/**
** \brief Test message subscription response to reaching the maximum
**        message ID count
**
** \par Description
**        This function tests the message subscription response to reaching the
**        maximum message ID count.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_Subscribe_MaxMsgIdCount(void);

/*****************************************************************************/
/**
** \brief Test obtaining the list of current message subscriptions
**
** \par Description
**        This function tests obtaining the list of current message
**        subscriptions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_Subscribe_SendPrevSubs(void);

/*****************************************************************************/
/**
** \brief Test function to get a count of the global message IDs in use
**
** \par Description
**        This function tests the function to get a count of the global message
**        IDs in use.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_Subscribe_FindGlobalMsgIdCnt(void);

/*****************************************************************************/
/**
** \brief Test message subscription response to nonexistent pipe
**
** \par Description
**        This function tests the message subscription response to nonexistent
**        pipe.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_Subscribe_PipeNonexistent(void);

/*****************************************************************************/
/**
** \brief Test enabling and disabling subscription reporting
**
** \par Description
**        This function tests enabling and disabling subscription reporting.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_Subscribe_SubscriptionReporting(void);

/*****************************************************************************/
/**
** \brief Test message subscription response to an invalid pipe owner
**
** \par Description
**        This function tests the message subscription response to an invalid
**        pipe owner.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_Subscribe_InvalidPipeOwner(void);

/*****************************************************************************/
/**
** \brief Function for calling SB unsubscribe API test functions
**
** \par Description
**        Function for calling SB unsubscribe API test functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_Unsubscribe_API(void);

/*****************************************************************************/
/**
** \brief Test API used to unsubscribe to a message (successful)
**
** \par Description
**        This function tests unsubscribing to a message (successful).
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_Unsubscribe_Basic(void);

/*****************************************************************************/
/**
** \brief Test CFE internal API used to locally unsubscribe to a message
**        (successful)
**
** \par Description
**        This function tests locally unsubscribing to a message
**        (successful).
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_Unsubscribe_Local(void);

/*****************************************************************************/
/**
** \brief Test CFE internal API used to unsubscribe to a message with AppId
**        (successful)
**
** \par Description
**        This function tests locally unsubscribing to a message
**        (successful).
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_Unsubscribe_AppId(void);

/*****************************************************************************/
/**
** \brief Test message unsubscription response to an invalid message ID
**
** \par Description
**        This function tests the message unsubscription response to an invalid
**        message ID.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_Unsubscribe_InvalParam(void);

/*****************************************************************************/
/**
** \brief Test message unsubscription response to a message ID that is
**        not subscribed
**
** \par Description
**        This function tests the message unsubscription response to a message
**        ID that is not subscribed.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_Unsubscribe_NoMatch(void);

/*****************************************************************************/
/**
** \brief Test message unsubscription response to enabling/disabling
**        subscription reporting
**
** \par Description
**        This function tests the message unsubscription response to enabling/
**        disabling subscription reporting.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_Unsubscribe_SubscriptionReporting(void);

/*****************************************************************************/
/**
** \brief Test message unsubscription response to an invalid pipe ID
**
** \par Description
**        This function tests the message unsubscription response to an invalid
**        pipe ID.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_Unsubscribe_InvalidPipe(void);

/*****************************************************************************/
/**
** \brief Test message unsubscription response to an invalid pipe owner
**
** \par Description
**        This function tests the message unsubscription response to an invalid
**        pipe owner.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_Unsubscribe_InvalidPipeOwner(void);

/*****************************************************************************/
/**
** \brief Test message unsubscription response to the first pipe destination
**        when the message is subscribed to by multiple pipes
**
** \par Description
**        This function tests the message unsubscription response to the first
**        pipe destination when the message is subscribed to by multiple pipes.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_Unsubscribe_FirstDestWithMany(void);

/*****************************************************************************/
/**
** \brief Test message unsubscription response to a middle pipe destination
**        when the message is subscribed to by multiple pipes
**
** \par Description
**        This function tests the message unsubscription response to a middle
**        pipe destination when the message is subscribed to by multiple pipes.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_Unsubscribe_MiddleDestWithMany(void);

/*****************************************************************************/
/**
** \brief  Test message unsubscription by verifying the message destination
**         pointer no longer points to the pipe
**
** \par Description
**        This function tests message unsubscription by verifying the message
**        destination pointer no longer points to the pipe.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_Unsubscribe_GetDestPtr(void);

/*****************************************************************************/
/**
** \brief Function for calling SB send message API test functions
**
** \par Description
**        Function for calling SB send message API test functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_TransmitMsg_API(void);

/*****************************************************************************/
/**
** \brief Test response to sending a null message on the software bus
**
** \par Description
**        This function tests the response to sending a null message on the
**        software bus.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_TransmitMsg_NullPtr(void);

/*****************************************************************************/
/**
** \brief Test response to sending a message with an invalid ID
**
** \par Description
**        This function tests the response to sending a message with an invalid
**        ID.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_TransmitMsg_InvalidMsgId(void);

/*****************************************************************************/
/**
** \brief Test response to sending a message which has no subscribers
**
** \par Description
**        This function tests the response to sending a message which has no
**        subscribers.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_TransmitMsg_NoSubscribers(void);

/*****************************************************************************/
/**
** \brief Test response to sending a message with the message size larger
**        than allowed
**
** \par Description
**        This function tests the response to sending a message with the
**        message size larger than allowed.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_TransmitMsg_MaxMsgSizePlusOne(void);

/*****************************************************************************/
/**
** \brief Test successfully sending a message on the software bus
**
** \par Description
**        This function tests successfully sending a message on the software
**        bus.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_TransmitMsg_BasicSend(void);

/*****************************************************************************/
/**
** \brief Test successful send of messages with header update
**
** \par Description
**        This function tests successful send/receive for packet sequence
**        count.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_TransmitMsg_UpdateHeader(void);

/*****************************************************************************/
/**
** \brief Test send message response to a socket queue 'put' error
**
** \par Description
**        This function tests the send message response to a socket queue 'put'
**        error.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_TransmitMsg_QueuePutError(void);

/*****************************************************************************/
/**
** \brief Test send message response when the socket queue is full
**
** \par Description
**        This function tests the send message response when the socket queue
**        is full.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_TransmitMsg_PipeFull(void);

/*****************************************************************************/
/**
** \brief Test send message response to too many messages sent to the pipe
**
** \par Description
**        This function tests the send message response to too many messages
**        sent to the pipe.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_TransmitMsg_MsgLimitExceeded(void);

/*****************************************************************************/
/**
** \brief Test send message response to a buffer descriptor allocation failure
**
** \par Description
**        This function tests the send message response to a buffer descriptor
**        allocation failure.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_TransmitMsg_GetPoolBufErr(void);

/*****************************************************************************/
/**
** \brief Test getting a pointer to a buffer for zero copy mode with buffer
**        allocation failures
**
** \par Description
**        This function tests getting a pointer to a buffer for zero copy mode
**        with buffer allocation failures.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_AllocateMessageBuffer(void);

/*****************************************************************************/
/**
** \brief Test successfully sending a message in zero copy mode (telemetry
**        source sequence count increments)
**
** \par Description
**        This function tests successfully sending a message in zero copy mode
**        (telemetry source sequence count increments).
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_TransmitBuffer_IncrementSeqCnt(void);

/*****************************************************************************/
/**
** \brief Test the zero-copy handle validation performed by CFE_SB_ZeroCopyHandleValidate()
**
** \par Description
**        Exercises the validation checks within this function
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_TransmitMsg_ZeroCopyBufferValidate(void);

/*****************************************************************************/
/**
** \brief Test successfully sending a message in zero copy mode (telemetry
**        source sequence count is unchanged)
**
** \par Description
**        This function tests successfully sending a message in zero copy mode
**        (telemetry source sequence count is unchanged).
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_TransmitBuffer_NoIncrement(void);

/*****************************************************************************/
/**
** \brief Test releasing a pointer to a buffer for zero copy mode
**
** \par Description
**        This function tests releasing a pointer to a buffer for zero copy
**        mode.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_ReleaseMessageBuffer(void);

/*****************************************************************************/
/**
** \brief Test send message response with the destination disabled
**
** \par Description
**        This function tests the send message response with the destination
**        disabled.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_TransmitMsg_DisabledDestination(void);

/*****************************************************************************/
/**
** \brief Test CFE_SB_TransmitTxn_BroadcastToRoute
**
** \par Description
**        This function tests broadcasting a message buffer with the metadata.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_TransmitTxn_PipeHandler(void);

/*****************************************************************************/
/**
** \brief Test response to sending a message which has an invalid Msg ID
**
** \par Description
**        This function tests the response to sending a message which has an
**        invalid Message ID
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_TransmitTxn_SetupFromMsg_InvalidMsgId(void);

/*****************************************************************************/
/**
** \brief Test response to sending a message with the message size larger
**        than allowed
**
** \par Description
**        This function tests the response to sending a message with the
**        message size larger than allowed.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_TransmitTxn_SetupFromMsg_MaxMsgSizePlusOne(void);

/*****************************************************************************/
/**
** \brief Function for calling SB receive message API test functions
**
** \par Description
**        Function for calling SB receive message API test functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_ReceiveBuffer_API(void);

/*****************************************************************************/
/**
** \brief Test receiving a message from the software bus with an invalid
**        pipe ID
**
** \par Description
**        This function tests receiving a message from the software bus with an
**        invalid pipe ID.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_ReceiveBuffer_InvalidPipeId(void);

/*****************************************************************************/
/**
** \brief Test receiving a message response to an invalid timeout value (< -1)
**
** \par Description
**        This function tests receiving a message response to an invalid
**        timeout value (< -1).
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_ReceiveBuffer_InvalidTimeout(void);

/*****************************************************************************/
/**
** \brief Test receiving a message response when there is no message on
**        the queue
**
** \par Description
**        This function tests receiving a message response when there is no
**        message on the queue.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_ReceiveBuffer_Poll(void);

/*****************************************************************************/
/**
** \brief Test receiving a message response to a timeout
**
** \par Description
**        This function tests receiving a message response to a timeout.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_ReceiveBuffer_Timeout(void);

/*****************************************************************************/
/**
** \brief Test receiving a message response to a pipe read error
**
** \par Description
**        This function tests receiving a message response to a pipe read
**        error.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_ReceiveBuffer_PipeReadError(void);

/*****************************************************************************/
/**
** \brief Test receiving a message response to a "pend forever" timeout
**
** \par Description
**        This function tests receiving a message response to a "pend forever"
**        timeout.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_ReceiveBuffer_PendForever(void);

/*****************************************************************************/
/**
** \brief Test receiving a message response to an invalid buffer pointer (null)
**
** \par Description
**        This function tests receiving a message response to an invalid buffer
**        pointer (null).
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_ReceiveBuffer_InvalidBufferPtr(void);

/*****************************************************************************/
/**
** \brief Test releasing zero copy buffers for all pipes owned by a
**        given app ID
**
** \par Description
**        This function tests releasing zero copy buffers for all pipes owned
**        by a given app ID.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CleanupApp_API(void);

/*****************************************************************************/
/**
** \brief Function for calling SB utility API test functions
**
** \par Description
**        Function for calling SB utility API test functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_Utils(void);

/*****************************************************************************/
/**
** \brief Test getting the size of a message header
**
** \par Description
**        This function tests getting the size of a command message
**        header.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CFE_SB_MsgHdrSize(void);

/*****************************************************************************/
/**
** \brief Test getting a pointer to the user data portion of a message
**
** \par Description
**        This function tests getting a pointer to the user data portion of a
**        message.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CFE_SB_GetUserData(void);

/*****************************************************************************/
/**
** \brief Test setting and getting the user data size of a message
**
** \par Description
**        This function tests setting and getting the user data size of a
**        message.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CFE_SB_SetGetUserDataLength(void);

/*****************************************************************************/
/**
** \brief Test validating a valid and invalid msg id
**
** \par Description
**        Test validating a valid and invalid msg id
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CFE_SB_ValidateMsgId(void);

/*****************************************************************************/
/**
** \brief Test Tracking List functions
**
** \par Description
**        Test tracking list functions (some of which are inline)
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CFE_SB_ZeroCopyReleaseAppId(void);

/*****************************************************************************/
/**
** \brief Function for calling SB special test cases functions
**
** \par Description
**        Function for calling SB special test cases functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_SB_SpecialCases(void);

/*****************************************************************************/
/**
** \brief Test Use Count rollover protection
**
** \par Description
**        This function tests a buffer descriptor with the max possible
**        use count.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_UseCount_Rollover_Prevention(void);

/*****************************************************************************/
/**
** \brief Test pipe creation with semaphore take and give failures
**
** \par Description
**        This function tests pipe creation with semaphore take and give
**        failures.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_OS_MutSem_ErrLogic(void);

/*****************************************************************************/
/**
** \brief Test successful recursive event prevention
**
** \par Description
**        This function tests successful recursive event prevention.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_ReqToSendEvent_ErrLogic(void);

/*****************************************************************************/
/**
** \brief Test getting a destination descriptor to the SB memory pool using a
**        null destination pointer
**
** \par Description
**        This function tests getting a destination descriptor to the SB memory
**        pool using a null destination pointer.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_PutDestBlk_ErrLogic(void);

/*****************************************************************************/
/**
** \brief Test functions that involve a buffer in the SB buffer pool
**
** \par Description
**        This function tests the functions that involve a buffer in the SB
**        buffer pool.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CFE_SB_Buffers(void);

/*****************************************************************************/
/**
** \brief Test functions that involve bad pipe information
**
** \par Description
**        This function tests the functions that involve bad pipe information.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_CFE_SB_BadPipeInfo(void);

/*****************************************************************************/
/**
** \brief Test ReceiveBuffer function unsubscribe/resubscribe path
**
** \par Description
**        This function tests the branch path in the ReceiveBuffer function when a
**        message in the pipe is unsubscribed, then resubscribed.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_ReceiveBuffer_UnsubResubPath(void);

/*****************************************************************************/
/**
** \brief Test MessageStringSet and MessageStringGet function paths
**
** \par Description
**        This function tests the branch paths in the MessageStringSet and
**        MessageStringGet functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
******************************************************************************/
void Test_MessageString(void);

void Test_SB_Macros(void);

void Test_SB_CCSDSPriHdr_Macros(void);
void Test_SB_CCSDSSecHdr_Macros(void);
void Test_SB_IdxPushPop(void);

/*
 * TopicID <-> MsgID conversion test routines
 */
void Test_CFE_SB_CmdTopicIdToMsgId(void);
void Test_CFE_SB_TlmTopicIdToMsgId(void);
void Test_CFE_SB_GlobalCmdTopicIdToMsgId(void);
void Test_CFE_SB_GlobalTlmTopicIdToMsgId(void);
void Test_CFE_SB_LocalCmdTopicIdToMsgId(void);
void Test_CFE_SB_LocalTlmTopicIdToMsgId(void);

#endif /* SB_UT_H */
