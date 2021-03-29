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
** File:
**    tbl_UT.c
**
** Purpose:
**    Table Services unit test
**
** References:
**    1. cFE Application Developers Guide
**    2. unit test standard 092503
**    3. C Coding Standard 102904
**
** Notes:
**    1. This is unit test code only, not for use in flight
**
*/

/*
** Includes
*/
#include "tbl_UT.h"
#include "cfe_core_resourceid_basevalues.h"

/*
** External global variables
*/

extern CFE_TBL_Global_t CFE_TBL_Global;

/*
** Global variables
*/
CFE_TBL_Handle_t App1TblHandle1;
CFE_TBL_Handle_t App1TblHandle2;
CFE_TBL_Handle_t App2TblHandle1;
CFE_TBL_Handle_t App2TblHandle2;
CFE_TBL_Handle_t ArrayOfHandles[2];

#define UT_TBL_APPID_1  CFE_ES_APPID_C(CFE_ResourceId_FromInteger(CFE_ES_APPID_BASE + 1))
#define UT_TBL_APPID_2  CFE_ES_APPID_C(CFE_ResourceId_FromInteger(CFE_ES_APPID_BASE + 2))
#define UT_TBL_APPID_3  CFE_ES_APPID_C(CFE_ResourceId_FromInteger(CFE_ES_APPID_BASE + 3))
#define UT_TBL_APPID_10 CFE_ES_APPID_C(CFE_ResourceId_FromInteger(CFE_ES_APPID_BASE + 10))

void * Tbl1Ptr = NULL;
void * Tbl2Ptr = NULL;
void **ArrayOfPtrsToTblPtrs[2];

static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TBL_CMD_NOOP_CC = {.MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_TBL_CMD_MID),
                                                                    .CommandCode = CFE_TBL_NOOP_CC};
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TBL_CMD_RESET_COUNTERS_CC = {
    .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_TBL_CMD_MID), .CommandCode = CFE_TBL_RESET_COUNTERS_CC};
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TBL_INVALID_MID = {.MsgId = CFE_SB_MSGID_RESERVED, .CommandCode = 0};
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TBL_CMD_INVALID_CC = {
    .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_TBL_CMD_MID), .CommandCode = 0x7F};

CFE_TBL_RegistryRec_t Original[CFE_PLATFORM_TBL_MAX_NUM_TABLES];

/*
 * UT helper routines
 */
void UT_TBL_SetupHeader(CFE_TBL_File_Hdr_t *TblFileHeader, size_t Offset, size_t NumBytes)
{
    TblFileHeader->Offset   = CFE_ES_MEMOFFSET_C(Offset);
    TblFileHeader->NumBytes = CFE_ES_MEMOFFSET_C(NumBytes);

    if (UT_Endianess == UT_LITTLE_ENDIAN)
    {
        CFE_TBL_ByteSwapUint32(&TblFileHeader->Offset);
        CFE_TBL_ByteSwapUint32(&TblFileHeader->NumBytes);
    }
}

/*
** Functions
*/
void UtTest_Setup(void)
{
    /* Initialize unit test */
    UT_Init("tbl");
    UtPrintf("cFE TBL Unit Test Output File\n\n");
    UT_InitializeTableRegistryNames();

    /* cfe_tbl_task.c functions */
    UT_ADD_TEST(Test_CFE_TBL_TaskInit);
    UT_ADD_TEST(Test_CFE_TBL_InitData);
    UT_ADD_TEST(Test_CFE_TBL_SearchCmdHndlrTbl);

    /* cfe_tbl_task_cmds.c functions */
    /* This should be done first (it initializes working data structures) */
    UT_ADD_TEST(Test_CFE_TBL_DeleteCDSCmd);
    UT_ADD_TEST(Test_CFE_TBL_TlmRegCmd);
    UT_ADD_TEST(Test_CFE_TBL_AbortLoadCmd);
    UT_ADD_TEST(Test_CFE_TBL_ActivateCmd);
    UT_ADD_TEST(Test_CFE_TBL_DumpToFile);
    UT_ADD_TEST(Test_CFE_TBL_ResetCmd);
    UT_ADD_TEST(Test_CFE_TBL_ValidateCmd);
    UT_ADD_TEST(Test_CFE_TBL_NoopCmd);
    UT_ADD_TEST(Test_CFE_TBL_GetTblRegData);
    UT_ADD_TEST(Test_CFE_TBL_GetHkData);
    UT_ADD_TEST(Test_CFE_TBL_DumpRegCmd);
    UT_ADD_TEST(Test_CFE_TBL_DumpCmd);
    UT_ADD_TEST(Test_CFE_TBL_LoadCmd);
    UT_ADD_TEST(Test_CFE_TBL_HousekeepingCmd);

    /* cfe_tbl_api.c and cfe_tbl_internal.c functions */
    UT_ADD_TEST(Test_CFE_TBL_ApiInit);
    UT_ADD_TEST(Test_CFE_TBL_Register);
    UT_ADD_TEST(Test_CFE_TBL_Share);
    UT_ADD_TEST(Test_CFE_TBL_Unregister);
    UT_ADD_TEST(Test_CFE_TBL_NotifyByMessage);
    UT_ADD_TEST(Test_CFE_TBL_Load);
    UT_ADD_TEST(Test_CFE_TBL_GetAddress);
    UT_ADD_TEST(Test_CFE_TBL_ReleaseAddress);
    UT_ADD_TEST(Test_CFE_TBL_GetAddresses);
    UT_ADD_TEST(Test_CFE_TBL_ReleaseAddresses);
    UT_ADD_TEST(Test_CFE_TBL_Validate);
    UT_ADD_TEST(Test_CFE_TBL_Manage);
    UT_ADD_TEST(Test_CFE_TBL_Update);
    UT_ADD_TEST(Test_CFE_TBL_GetStatus);
    UT_ADD_TEST(Test_CFE_TBL_GetInfo);
    UT_ADD_TEST(Test_CFE_TBL_TblMod);

    /* Miscellaneous cfe_tbl_internal.c tests */
    UT_ADD_TEST(Test_CFE_TBL_Internal);
}

/*
** Fill the whole table registry with known table names and set an owner ID
*/
void UT_InitializeTableRegistryNames()
{
    int i;

    for (i = 0; i < CFE_PLATFORM_TBL_MAX_NUM_TABLES; i++)
    {
        snprintf(CFE_TBL_Global.Registry[i].Name, CFE_TBL_MAX_FULL_NAME_LEN, "%d", i);
        CFE_TBL_Global.Registry[i].OwnerAppId = UT_TBL_APPID_2;
    }
}

/*
** Initialize table registry values
*/
void UT_ResetTableRegistry(void)
{
    int32 i = 0;

    for (i = 0; i < CFE_PLATFORM_TBL_MAX_NUM_TABLES; i++)
    {
        CFE_TBL_InitRegistryRecord(&CFE_TBL_Global.Registry[i]);
    }

    /* Initialize the table access descriptors */
    for (i = 0; i < CFE_PLATFORM_TBL_MAX_NUM_HANDLES; i++)
    {
        CFE_TBL_Global.Handles[i].AppId       = CFE_TBL_NOT_OWNED;
        CFE_TBL_Global.Handles[i].RegIndex    = 0;
        CFE_TBL_Global.Handles[i].PrevLink    = CFE_TBL_END_OF_LIST;
        CFE_TBL_Global.Handles[i].NextLink    = CFE_TBL_END_OF_LIST;
        CFE_TBL_Global.Handles[i].UsedFlag    = false;
        CFE_TBL_Global.Handles[i].LockFlag    = false;
        CFE_TBL_Global.Handles[i].Updated     = false;
        CFE_TBL_Global.Handles[i].BufferIndex = 0;
    }

    /* Initialize the table validation results records */
    for (i = 0; i < CFE_PLATFORM_TBL_MAX_NUM_VALIDATIONS; i++)
    {
        CFE_TBL_Global.ValidationResults[i].State        = CFE_TBL_VALIDATION_FREE;
        CFE_TBL_Global.ValidationResults[i].CrcOfTable   = 0;
        CFE_TBL_Global.ValidationResults[i].Result       = 0;
        CFE_TBL_Global.ValidationResults[i].ActiveBuffer = false;
        CFE_TBL_Global.ValidationResults[i].TableName[0] = '\0';
    }

    /* Initialize the dump-only table dump control blocks */
    for (i = 0; i < CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS; i++)
    {
        CFE_TBL_Global.DumpControlBlocks[i].State         = CFE_TBL_DUMP_FREE;
        CFE_TBL_Global.DumpControlBlocks[i].DumpBufferPtr = NULL;
        CFE_TBL_Global.DumpControlBlocks[i].Size          = 0;
        CFE_TBL_Global.DumpControlBlocks[i].TableName[0]  = '\0';

        /* Free all shared buffers */
        CFE_TBL_Global.LoadBuffs[i].Taken = false;
    }

    CFE_TBL_Global.ValidationCounter = 0;
    CFE_TBL_Global.HkTlmTblRegIndex  = CFE_TBL_NOT_FOUND;
    CFE_TBL_Global.LastTblUpdated    = CFE_TBL_NOT_FOUND;
}

/*
** Tests to cover table task initialization functions
*/
void Test_CFE_TBL_TaskInit(void)
{
    uint32 ExitCode;
    union
    {
        CFE_TBL_NoArgsCmd_t NoArgsCmd;
        CFE_MSG_Message_t   Msg;
    } CmdBuf;
    CFE_SB_MsgId_t    MsgId   = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t FcnCode = 0;

    UtPrintf("Begin Test Task Init");

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    /* Test successful table services main entry point execution */
    UT_InitData();
    ExitCode = 0;
    UT_SetDataBuffer(UT_KEY(CFE_ES_ExitApp), &ExitCode, sizeof(ExitCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    CFE_TBL_TaskMain();
    UT_Report(__FILE__, __LINE__,
              ExitCode == CFE_ES_RunStatus_CORE_APP_RUNTIME_ERROR && UT_GetStubCount(UT_KEY(CFE_ES_ExitApp)) == 1,
              "CFE_TBL_TaskMain", "Success");

    /* Test successful table services core application initialization */
    UT_InitData();
    UT_Report(__FILE__, __LINE__, CFE_TBL_TaskInit() == CFE_SUCCESS, "CFE_TBL_TaskInit", "Success");

    /* Test table services core application initialization response to a pipe
     * creation failure
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_CreatePipe), 1, -2);
    UT_Report(__FILE__, __LINE__, CFE_TBL_TaskInit() == -2, "CFE_TBL_TaskInit", "Create pipe fail");

    /* Test table services core application initialization response to a
     * housekeeping request subscription error
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_Subscribe), 1, -3);
    UT_Report(__FILE__, __LINE__, CFE_TBL_TaskInit() == -3, "CFE_TBL_TaskInit", "Housekeeping request subscribe fail");

    /* Test table services core application initialization response to a
     * ground command subscription error
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_Subscribe), 2, -4);
    UT_Report(__FILE__, __LINE__, CFE_TBL_TaskInit() == -4, "CFE_TBL_TaskInit", "Ground command subscribe fail");

    /* Test table services core application initialization response to a
     * send initialization event error
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_SendEvent), 1, -5);
    UT_Report(__FILE__, __LINE__, CFE_TBL_TaskInit() == -5, "CFE_TBL_TaskInit", "Send initialization event fail");

    /* Test table services core application initialization response to an
     * EVS register failure
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_Register), 1, -6);
    UT_Report(__FILE__, __LINE__, CFE_TBL_TaskInit() == -6, "CFE_TBL_TaskInit", "EVS register fail");

    /* Test command pipe messages handler response to a valid command */
    UT_InitData();
    UT_CallTaskPipe(CFE_TBL_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.NoArgsCmd), UT_TPID_CFE_TBL_CMD_NOOP_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_TBL_NOOP_INF_EID), "CFE_TBL_TaskPipe",
              "Valid command (no-op) - success");

    /* Test command pipe messages handler response to an invalid
     * message length
     */
    UT_InitData();
    UT_CallTaskPipe(CFE_TBL_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.NoArgsCmd) - 1, UT_TPID_CFE_TBL_CMD_NOOP_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_TBL_LEN_ERR_EID), "CFE_TBL_TaskPipe",
              "Invalid message length");

    /* Test command pipe messages handler response to an invalid
     * command code
     */
    UT_InitData();
    UT_CallTaskPipe(CFE_TBL_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.NoArgsCmd), UT_TPID_CFE_TBL_CMD_INVALID_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_TBL_CC1_ERR_EID), "CFE_TBL_TaskPipe", "Invalid command code");

    /* Test command pipe messages handler response to other errors */
    /* Test command pipe messages handler response to "message type" message */
    UT_InitData();
    CFE_TBL_Global.CommandCounter      = 0;
    CFE_TBL_Global.CommandErrorCounter = 0;
    UT_CallTaskPipe(CFE_TBL_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.NoArgsCmd), UT_TPID_CFE_TBL_INVALID_MID);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_TBL_MID_ERR_EID) && CFE_TBL_Global.CommandCounter == 0 &&
                  CFE_TBL_Global.CommandErrorCounter == 0,
              "CFE_TBL_TaskPipe", "'Message' type message");

    /* Test command pipe messages handler response to "command type" message */
    UT_InitData();
    UT_CallTaskPipe(CFE_TBL_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.NoArgsCmd), UT_TPID_CFE_TBL_CMD_RESET_COUNTERS_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_TBL_RESET_INF_EID) && CFE_TBL_Global.CommandCounter == 0 &&
                  CFE_TBL_Global.CommandErrorCounter == 0,
              "CFE_TBL_TaskPipe", "'Command' type message");
}

/*
** Test table service application data initialization
*/
void Test_CFE_TBL_InitData(void)
{
    UtPrintf("Begin Test Init Data");

    /* This function has only one possible path with no return code */
    UT_InitData();
    CFE_TBL_InitData();
    ASSERT_EQ(UT_GetStubCount(UT_KEY(CFE_MSG_Init)), 3);
}

/*
** Test command handler table message ID (or command code) search function
*/
void Test_CFE_TBL_SearchCmdHndlrTbl(void)
{
    int16          TblIndex = 1;
    uint16         CmdCode;
    CFE_SB_MsgId_t MsgID;

    UtPrintf("Begin Test Search Command Handler Table");

    /* Test successfully finding a matching message ID and command code */
    UT_InitData();
    MsgID   = CFE_SB_ValueToMsgId(CFE_TBL_CMD_MID);
    CmdCode = CFE_TBL_NOOP_CC;
    UT_Report(__FILE__, __LINE__, CFE_TBL_SearchCmdHndlrTbl(MsgID, CmdCode) == TblIndex, "CFE_TBL_SearchCmdHndlrTbl",
              "Found matching message ID and command code");

    /* Test using a message that is not a command message with specific
     * command code
     */
    UT_InitData();
    TblIndex = 0;
    MsgID    = CFE_SB_ValueToMsgId(CFE_TBL_SEND_HK_MID);
    UT_Report(__FILE__, __LINE__, CFE_TBL_SearchCmdHndlrTbl(MsgID, CmdCode) == TblIndex, "CFE_TBL_SearchCmdHndlrTbl",
              "Message is not a command message with specific command code");

    /* Test with a message ID that matches but the command code does
     * not match
     */
    UT_InitData();
    TblIndex = CFE_TBL_BAD_CMD_CODE;
    MsgID    = CFE_SB_ValueToMsgId(CFE_TBL_CMD_MID);
    CmdCode  = 0xffff;
    UT_Report(__FILE__, __LINE__, CFE_TBL_SearchCmdHndlrTbl(MsgID, CmdCode) == TblIndex, "CFE_TBL_SearchCmdHndlrTbl",
              "Message ID matches, command code must does not match");

    /* Test with a message ID that does not match */
    UT_InitData();
    TblIndex = CFE_TBL_BAD_MSG_ID;
    MsgID    = CFE_SB_INVALID_MSG_ID;
    UT_Report(__FILE__, __LINE__, CFE_TBL_SearchCmdHndlrTbl(MsgID, CmdCode) == TblIndex, "CFE_TBL_SearchCmdHndlrTbl",
              "Message ID does not match");
}

/*
** Test the delete critical table's CDS command message
*/
void Test_CFE_TBL_DeleteCDSCmd(void)
{
    int                    j, k;
    CFE_TBL_DeleteCDSCmd_t DelCDSCmd;

    UtPrintf("Begin Test Delete CDS Command");

    /* Test successfully finding the table name in the table registry */
    UT_InitData();
    strncpy(DelCDSCmd.Payload.TableName, "0", sizeof(DelCDSCmd.Payload.TableName) - 1);
    DelCDSCmd.Payload.TableName[sizeof(DelCDSCmd.Payload.TableName) - 1] = '\0';
    UT_Report(__FILE__, __LINE__, CFE_TBL_DeleteCDSCmd(&DelCDSCmd) == CFE_TBL_INC_ERR_CTR, "CFE_TBL_DeleteCDSCmd",
              "Table name found in table registry");

    /* Test failure to find table in the critical table registry */
    UT_InitData();
    k = CFE_PLATFORM_TBL_MAX_CRITICAL_TABLES + CFE_PLATFORM_TBL_MAX_NUM_TABLES;

    for (j = CFE_PLATFORM_TBL_MAX_NUM_TABLES; j < k; j++)
    {
        snprintf(CFE_TBL_Global.CritReg[j - CFE_PLATFORM_TBL_MAX_NUM_TABLES].Name, CFE_TBL_MAX_FULL_NAME_LEN, "%d", j);
    }

    strncpy(DelCDSCmd.Payload.TableName, "-1", sizeof(DelCDSCmd.Payload.TableName) - 1);
    DelCDSCmd.Payload.TableName[sizeof(DelCDSCmd.Payload.TableName) - 1] = '\0';
    UT_Report(__FILE__, __LINE__, CFE_TBL_DeleteCDSCmd(&DelCDSCmd) == CFE_TBL_INC_ERR_CTR, "CFE_TBL_DeleteCDSCmd",
              "Table not found in critical table registry");

    /* Test finding the table in the critical table registry, but CDS is not
     * tagged as a table
     */
    UT_InitData();
    snprintf(DelCDSCmd.Payload.TableName, sizeof(DelCDSCmd.Payload.TableName), "%d",
             CFE_PLATFORM_TBL_MAX_CRITICAL_TABLES + CFE_PLATFORM_TBL_MAX_NUM_TABLES - 1);
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_DeleteCDS), 1, CFE_ES_CDS_WRONG_TYPE_ERR);
    UT_Report(__FILE__, __LINE__, CFE_TBL_DeleteCDSCmd(&DelCDSCmd) == CFE_TBL_INC_ERR_CTR, "CFE_TBL_DeleteCDSCmd",
              "Table is in critical table registry but CDS is not tagged "
              "as a table");

    /* Test deletion when CDS owning application is still active */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_DeleteCDS), 1, CFE_ES_CDS_OWNER_ACTIVE_ERR);
    UT_Report(__FILE__, __LINE__, CFE_TBL_DeleteCDSCmd(&DelCDSCmd) == CFE_TBL_INC_ERR_CTR, "CFE_TBL_DeleteCDSCmd",
              "CDS owning application is still active");

    /* Test deletion where the table cannot be located in the CDS registry */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_DeleteCDS), 1, CFE_ES_ERR_NAME_NOT_FOUND);
    UT_Report(__FILE__, __LINE__, CFE_TBL_DeleteCDSCmd(&DelCDSCmd) == CFE_TBL_INC_ERR_CTR, "CFE_TBL_DeleteCDSCmd",
              "Unable to locate table in CDS registry");

    /* Test deletion error while deleting table from the CDS */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_DeleteCDS), 1, CFE_SUCCESS - 1);
    UT_Report(__FILE__, __LINE__, CFE_TBL_DeleteCDSCmd(&DelCDSCmd) == CFE_TBL_INC_ERR_CTR, "CFE_TBL_DeleteCDSCmd",
              "Error while deleting table from CDS");

    /* Test successful removal of the table from the CDS */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_DeleteCDS), 1, CFE_SUCCESS);
    UT_Report(__FILE__, __LINE__, CFE_TBL_DeleteCDSCmd(&DelCDSCmd) == CFE_TBL_INC_CMD_CTR, "CFE_TBL_DeleteCDSCmd",
              "Successfully removed table from CDS");
}

/*
** Test the processing telemetry table registry command message function
*/
void Test_CFE_TBL_TlmRegCmd(void)
{
    CFE_TBL_SendRegistryCmd_t TlmRegCmd;

    UtPrintf("Begin Test Telemetry Registry Command");

    /* Test when table name does exist */
    UT_InitData();

    /* Registry[0].Name used because it is confirmed to be a registered
     * table name
     */
    strncpy(TlmRegCmd.Payload.TableName, CFE_TBL_Global.Registry[0].Name, sizeof(TlmRegCmd.Payload.TableName) - 1);
    TlmRegCmd.Payload.TableName[sizeof(TlmRegCmd.Payload.TableName) - 1] = '\0';
    UT_Report(__FILE__, __LINE__, CFE_TBL_SendRegistryCmd(&TlmRegCmd) == CFE_TBL_INC_CMD_CTR, "CFE_TBL_SendRegistryCmd",
              "Table registry entry for telemetry does exist");

    /* Test when table name does not exist */
    UT_InitData();

    snprintf(TlmRegCmd.Payload.TableName, sizeof(TlmRegCmd.Payload.TableName), "%d",
             CFE_PLATFORM_TBL_MAX_NUM_TABLES + 1);
    UT_Report(__FILE__, __LINE__, CFE_TBL_SendRegistryCmd(&TlmRegCmd) == CFE_TBL_INC_ERR_CTR, "CFE_TBL_SendRegistryCmd",
              "Table registry entry for telemetry doesn't exist");
}

/*
** Test the processing abort load command message function
*/
void Test_CFE_TBL_AbortLoadCmd(void)
{
    int                    load = (int)CFE_TBL_Global.Registry[0].LoadInProgress;
    CFE_TBL_AbortLoadCmd_t AbortLdCmd;

    UtPrintf("Begin Test Abort Load Command");

    /* Test when table name does exist and a table load is in progress */
    UT_InitData();

    /* Entering the if statement with a table name that has to be in
     * the registry
     */
    strncpy(AbortLdCmd.Payload.TableName, CFE_TBL_Global.Registry[0].Name, sizeof(AbortLdCmd.Payload.TableName) - 1);
    AbortLdCmd.Payload.TableName[sizeof(AbortLdCmd.Payload.TableName) - 1] = '\0';
    CFE_TBL_Global.Registry[0].LoadInProgress                              = 1;
    UT_Report(__FILE__, __LINE__, CFE_TBL_AbortLoadCmd(&AbortLdCmd) == CFE_TBL_INC_CMD_CTR, "CFE_TBL_AbortLoadCmd",
              "Table registry entry exists & load in progress");

    /* Test when table name does exist but no table load is in progress */
    UT_InitData();
    CFE_TBL_Global.Registry[0].LoadInProgress = CFE_TBL_NO_LOAD_IN_PROGRESS;
    UT_Report(__FILE__, __LINE__, CFE_TBL_AbortLoadCmd(&AbortLdCmd) == CFE_TBL_INC_ERR_CTR, "CFE_TBL_AbortLoadCmd",
              "Table registry entry exists but no load in progress");

    /* Test when table name does exist, a table load is in progress, and the
     * table is dump only
     */
    UT_InitData();
    CFE_TBL_Global.Registry[0].LoadInProgress = CFE_TBL_NO_LOAD_IN_PROGRESS + 1;
    CFE_TBL_Global.Registry[0].DumpOnly       = true;
    UT_Report(__FILE__, __LINE__, CFE_TBL_AbortLoadCmd(&AbortLdCmd) == CFE_TBL_INC_ERR_CTR, "CFE_TBL_AbortLoadCmd",
              "Table registry entry exists, load in progress, dump only");

    /* Test when table name not found in the registry */
    UT_InitData();
    snprintf(AbortLdCmd.Payload.TableName, sizeof(AbortLdCmd.Payload.TableName), "%d",
             CFE_PLATFORM_TBL_MAX_NUM_TABLES + 1);
    UT_Report(__FILE__, __LINE__, CFE_TBL_AbortLoadCmd(&AbortLdCmd) == CFE_TBL_INC_ERR_CTR, "CFE_TBL_AbortLoadCmd",
              "Table registry entry doesn't exist");

    /* Test when table is double buffered */
    UT_InitData();
    CFE_TBL_Global.Registry[0].DoubleBuffered = true;
    CFE_TBL_Global.LoadBuffs[0].Taken         = true;
    CFE_TBL_AbortLoad(&CFE_TBL_Global.Registry[0]);
    UT_Report(__FILE__, __LINE__, CFE_TBL_Global.LoadBuffs[0].Taken == true, "CFE_TBL_AbortLoad",
              "Table is double buffered");

    /* Restore values for subsequent tests */
    CFE_TBL_Global.Registry[0].LoadInProgress = load;
    CFE_TBL_Global.LoadBuffs[0].Taken         = false;
}

/*
** Test the activate table command message function
*/
void Test_CFE_TBL_ActivateCmd(void)
{
    int                   load = (int)CFE_TBL_Global.Registry[0].LoadInProgress;
    uint8                 dump = CFE_TBL_Global.Registry[0].DumpOnly;
    CFE_TBL_ActivateCmd_t ActivateCmd;

    UtPrintf("Begin Test Activate Command");

    /* Enter the if statement with a table name that is in the registry */
    strncpy(ActivateCmd.Payload.TableName, CFE_TBL_Global.Registry[0].Name, sizeof(ActivateCmd.Payload.TableName) - 1);
    ActivateCmd.Payload.TableName[sizeof(ActivateCmd.Payload.TableName) - 1] = '\0';

    /* Test when table name exists, but attempts to activate a dump-only
     * table
     */
    UT_InitData();
    CFE_TBL_Global.Registry[0].DumpOnly = true;
    UT_Report(__FILE__, __LINE__, CFE_TBL_ActivateCmd(&ActivateCmd) == CFE_TBL_INC_ERR_CTR, "CFE_TBL_ActivateCmd",
              "Table registry exists, but dump-only table attempted to load");

    /* Test when table name exists, the table is not a dump-only, a load is in
     * progress, and the table is double-buffered
     */
    UT_InitData();
    CFE_TBL_Global.Registry[0].DumpOnly       = false;
    CFE_TBL_Global.Registry[0].LoadInProgress = CFE_TBL_NO_LOAD_IN_PROGRESS + 1;
    CFE_TBL_Global.Registry[0].DoubleBuffered = true;
    UT_Report(__FILE__, __LINE__, CFE_TBL_ActivateCmd(&ActivateCmd) == CFE_TBL_INC_ERR_CTR, "CFE_TBL_ActivateCmd",
              "Table registry exists, not a dump-only tbl, and a load in "
              "progress: Table is double-buffered");

    /* Test when table name exists, the table is not a dump-only, a load is in
     * progress, the table isn't double-buffered, and ValidationStatus = true
     */
    UT_InitData();
    CFE_TBL_Global.Registry[0].DoubleBuffered                                     = false;
    CFE_TBL_Global.LoadBuffs[CFE_TBL_Global.Registry[0].LoadInProgress].Validated = true;
    UT_Report(__FILE__, __LINE__, CFE_TBL_ActivateCmd(&ActivateCmd) == CFE_TBL_INC_CMD_CTR, "CFE_TBL_ActivateCmd",
              "Table registry exists, not a dump-only tbl, and a load in "
              "progress: Table isn't double-buffered");

    /* Test when table name exists, the table is not a dump-only, no load is in
     * progress, and no notification message should be sent
     */
    UT_InitData();
    CFE_TBL_Global.Registry[0].NotifyByMsg    = false;
    CFE_TBL_Global.Registry[0].LoadInProgress = CFE_TBL_NO_LOAD_IN_PROGRESS;
    UT_Report(__FILE__, __LINE__, CFE_TBL_ActivateCmd(&ActivateCmd) == CFE_TBL_INC_ERR_CTR, "CFE_TBL_ActivateCmd",
              "Table registry exists, not a dump-only tbl, no load in "
              "progress, no notification message");

    /* Test when table name exists, the table is not a dump-only, no load in in
     * progress, and a notification message should be sent
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_TransmitMsg), 1, CFE_SB_INTERNAL_ERR);
    CFE_TBL_Global.Registry[0].NotifyByMsg    = true;
    CFE_TBL_Global.Registry[0].LoadInProgress = CFE_TBL_NO_LOAD_IN_PROGRESS + 1;
    UT_Report(__FILE__, __LINE__, CFE_TBL_ActivateCmd(&ActivateCmd) == CFE_TBL_INC_CMD_CTR, "CFE_TBL_ActivateCmd",
              "Table registry exists, not a dump-only tbl, no load in "
              "progress, send notification message");

    /* Test when the table name doesn't exist */
    UT_InitData();
    snprintf(ActivateCmd.Payload.TableName, sizeof(ActivateCmd.Payload.TableName), "%d",
             CFE_PLATFORM_TBL_MAX_NUM_TABLES + 1);
    UT_Report(__FILE__, __LINE__, CFE_TBL_ActivateCmd(&ActivateCmd) == CFE_TBL_INC_ERR_CTR, "CFE_TBL_ActivateCmd",
              "Table registry entry doesn't exist");

    /* Restore original values */
    CFE_TBL_Global.Registry[0].LoadInProgress = load;
    CFE_TBL_Global.Registry[0].DumpOnly       = dump;
}

/*
** Test the write table data to a file function
*/
void Test_CFE_TBL_DumpToFile(void)
{
    uint32 TblSizeInBytes = 9;

    UtPrintf("Begin Test Dump to File");

    /* Test with an error creating the dump file */
    UT_InitData();
    UT_SetDefaultReturnValue(UT_KEY(OS_OpenCreate), OS_ERROR);
    UT_Report(__FILE__, __LINE__,
              CFE_TBL_DumpToFile("filename", "tablename", "dumpaddress", TblSizeInBytes) == CFE_TBL_INC_ERR_CTR,
              "CFE_TBL_DumpToFile", "Error creating dump file");

    /* Test with an error writing the cFE file header */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_WriteHeader), 1, sizeof(CFE_FS_Header_t) - 1);
    UT_Report(__FILE__, __LINE__,
              CFE_TBL_DumpToFile("filename", "tablename", "dumpaddress", TblSizeInBytes) == CFE_TBL_INC_ERR_CTR,
              "CFE_TBL_DumpToFile", "Error writing cFE file header");

    /* Test with an error writing the table file header */
    UT_InitData();

    /* Set the count for the FSWriteHdrRtn return code variable to a large
     * enough value to pass through every time
     */
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_WriteHeader), 6, sizeof(CFE_FS_Header_t));
    UT_SetDeferredRetcode(UT_KEY(OS_write), 1, sizeof(CFE_TBL_File_Hdr_t) - 1);
    UT_Report(__FILE__, __LINE__,
              CFE_TBL_DumpToFile("filename", "tablename", "dumpaddress", TblSizeInBytes) == CFE_TBL_INC_ERR_CTR,
              "CFE_TBL_DumpToFile", "Error writing cFE file header");

    /* Test with an error writing the table to a file */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(OS_write), 2, TblSizeInBytes - 1);
    UT_Report(__FILE__, __LINE__,
              CFE_TBL_DumpToFile("filename", "tablename", "dumpaddress", TblSizeInBytes) == CFE_TBL_INC_ERR_CTR,
              "CFE_TBL_DumpToFile", "Error writing cFE file header");

    /* Test successful file creation and data dumped */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(OS_OpenCreate), 1, OS_ERROR);
    UT_Report(__FILE__, __LINE__,
              CFE_TBL_DumpToFile("filename", "tablename", "dumpaddress", TblSizeInBytes) == CFE_TBL_INC_CMD_CTR,
              "CFE_TBL_DumpToFile", "File created and data dumped");

    /* Test where file already exists so data isoverwritten */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_TBL_DumpToFile("filename", "tablename", "dumpaddress", TblSizeInBytes) == CFE_TBL_INC_CMD_CTR,
              "CFE_TBL_DumpToFile", "File existed previously => data overwritten");
}

/*
** Test the processing reset counters command message function
*/
void Test_CFE_TBL_ResetCmd(void)
{
    UtPrintf("Begin Test Reset Command");

    /* Test run through function (there are no additional paths) */
    UT_InitData();
    UT_Report(__FILE__, __LINE__, CFE_TBL_ResetCountersCmd(NULL) == CFE_TBL_DONT_INC_CTR, "CFE_TBL_ResetCountersCmd",
              "Function run and completed");
}

/*
** Test the validate table command message function
*/
void Test_CFE_TBL_ValidateCmd(void)
{
    int                       i;
    uint8                     Buff;
    uint8 *                   BuffPtr = &Buff;
    CFE_TBL_ValidateCmd_t     ValidateCmd;
    CFE_TBL_CallbackFuncPtr_t ValFuncPtr = (CFE_TBL_CallbackFuncPtr_t)((unsigned long)&UT_InitializeTableRegistryNames);

    UtPrintf("Begin Test Validate Command");

    /* Test when table name is not found in the registry */
    UT_InitData();
    snprintf(ValidateCmd.Payload.TableName, sizeof(ValidateCmd.Payload.TableName), "%d",
             CFE_PLATFORM_TBL_MAX_NUM_TABLES + 1);
    UT_Report(__FILE__, __LINE__, CFE_TBL_ValidateCmd(&ValidateCmd) == CFE_TBL_INC_ERR_CTR, "CFE_TBL_ValidateCmd",
              "Table registry entry doesn't exist");

    /* Test where the active buffer has data, but too many table validations
     * have been requested
     */
    UT_InitData();
    strncpy(ValidateCmd.Payload.TableName, CFE_TBL_Global.Registry[0].Name, sizeof(ValidateCmd.Payload.TableName) - 1);
    ValidateCmd.Payload.TableName[sizeof(ValidateCmd.Payload.TableName) - 1] = '\0';
    ValidateCmd.Payload.ActiveTableFlag                                      = CFE_TBL_BufferSelect_ACTIVE;
    CFE_TBL_Global.Registry[0].Buffers[CFE_TBL_Global.Registry[0].ActiveBufferIndex].BufferPtr = BuffPtr;

    for (i = 0; i < CFE_PLATFORM_TBL_MAX_NUM_VALIDATIONS; i++)
    {
        CFE_TBL_Global.ValidationResults[i].State = CFE_TBL_VALIDATION_PENDING;
    }

    UT_Report(__FILE__, __LINE__, CFE_TBL_ValidateCmd(&ValidateCmd) == CFE_TBL_INC_ERR_CTR, "CFE_TBL_ValidateCmd",
              "Active buffer with data: too many table validations have "
              "been requested");

    /* Test where the active buffer has data, but there is no validation
     * function pointer
     */
    UT_InitData();
    CFE_TBL_Global.ValidationResults[0].State    = CFE_TBL_VALIDATION_FREE;
    CFE_TBL_Global.Registry[0].ValidationFuncPtr = NULL;
    UT_Report(__FILE__, __LINE__, CFE_TBL_ValidateCmd(&ValidateCmd) == CFE_TBL_INC_CMD_CTR, "CFE_TBL_ValidateCmd",
              "Active buffer with data: No validation function pointer");

    /* Test where the active buffer has data, the validation function pointer
     * exists, and the active table flag is set
     */
    UT_InitData();
    CFE_TBL_Global.ValidationResults[0].State    = CFE_TBL_VALIDATION_FREE;
    CFE_TBL_Global.Registry[0].ValidationFuncPtr = ValFuncPtr;
    ValidateCmd.Payload.ActiveTableFlag          = true;
    UT_Report(__FILE__, __LINE__, CFE_TBL_ValidateCmd(&ValidateCmd) == CFE_TBL_INC_CMD_CTR, "CFE_TBL_ValidateCmd",
              "Active buffer with data: validation function pointer and "
              "active table flag");

    /* Test with the buffer inactive, the table is double-buffered, and the
     * validation function pointer exists
     */
    UT_InitData();
    ValidateCmd.Payload.ActiveTableFlag       = CFE_TBL_BufferSelect_INACTIVE;
    CFE_TBL_Global.Registry[0].DoubleBuffered = true;
    CFE_TBL_Global.Registry[0].Buffers[1 - CFE_TBL_Global.Registry[0].ActiveBufferIndex].BufferPtr = BuffPtr;
    CFE_TBL_Global.ValidationResults[0].State    = CFE_TBL_VALIDATION_FREE;
    CFE_TBL_Global.Registry[0].ValidationFuncPtr = ValFuncPtr;
    UT_Report(__FILE__, __LINE__, CFE_TBL_ValidateCmd(&ValidateCmd) == CFE_TBL_INC_CMD_CTR, "CFE_TBL_ValidateCmd",
              "Inactive buffer: double buffered table : validation "
              "function pointer");

    /* Test with the buffer inactive, the table is single-buffered with a
     * load in progress, the validation function pointer exists, and no
     * notification message should be sent
     */
    UT_InitData();
    CFE_TBL_Global.Registry[0].NotifyByMsg                                        = false;
    CFE_TBL_Global.Registry[0].DoubleBuffered                                     = false;
    CFE_TBL_Global.LoadBuffs[CFE_TBL_Global.Registry[0].LoadInProgress].BufferPtr = BuffPtr;
    CFE_TBL_Global.ValidationResults[0].State                                     = CFE_TBL_VALIDATION_FREE;
    CFE_TBL_Global.Registry[0].LoadInProgress                                     = CFE_TBL_NO_LOAD_IN_PROGRESS + 1;
    UT_Report(__FILE__, __LINE__, CFE_TBL_ValidateCmd(&ValidateCmd) == CFE_TBL_INC_CMD_CTR, "CFE_TBL_ValidateCmd",
              "Inactive buffer: single buffered table with load in progress: "
              "validation function pointer, no notification message");

    /* Test with the buffer inactive, the table is single-buffered with a
     * load in progress, the validation function pointer exists, and a
     * notification message should be sent
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_TransmitMsg), 1, CFE_SB_INTERNAL_ERR);
    CFE_TBL_Global.Registry[0].NotifyByMsg                                        = true;
    CFE_TBL_Global.Registry[0].DoubleBuffered                                     = false;
    CFE_TBL_Global.LoadBuffs[CFE_TBL_Global.Registry[0].LoadInProgress].BufferPtr = BuffPtr;
    CFE_TBL_Global.ValidationResults[0].State                                     = CFE_TBL_VALIDATION_FREE;
    CFE_TBL_Global.Registry[0].LoadInProgress                                     = CFE_TBL_NO_LOAD_IN_PROGRESS + 1;
    UT_Report(__FILE__, __LINE__, CFE_TBL_ValidateCmd(&ValidateCmd) == CFE_TBL_INC_CMD_CTR, "CFE_TBL_ValidateCmd",
              "Inactive buffer: single buffered table with load in progress: "
              "validation function pointer, send notification message");

    /* Test where no inactive buffer is present (single-buffered table without
     * load in progress)
     */
    UT_InitData();
    CFE_TBL_Global.Registry[0].LoadInProgress = CFE_TBL_NO_LOAD_IN_PROGRESS;
    UT_Report(__FILE__, __LINE__, CFE_TBL_ValidateCmd(&ValidateCmd) == CFE_TBL_INC_ERR_CTR, "CFE_TBL_ValidateCmd",
              "Inactive buffer: single buffered table with load in progress");

    /* Test with an illegal buffer */
    UT_InitData();
    ValidateCmd.Payload.ActiveTableFlag = 0xffff;
    UT_Report(__FILE__, __LINE__, CFE_TBL_ValidateCmd(&ValidateCmd) == CFE_TBL_INC_ERR_CTR, "CFE_TBL_ValidateCmd",
              "Illegal buffer");
}

/*
** Test the processing no-operation command message function
*/
void Test_CFE_TBL_NoopCmd(void)
{

    UtPrintf("Begin Test No-Op Command");

    /* Test run through function (there are no additional paths) */
    UT_InitData();
    UT_Report(__FILE__, __LINE__, CFE_TBL_NoopCmd(NULL) == CFE_TBL_INC_CMD_CTR, "CFE_TBL_NoopCmd",
              "Function run and completed");
}

/*
** Test the function which converts table registry entries for tables into
** messages
*/
void Test_CFE_TBL_GetTblRegData(void)
{
    UtPrintf("Begin Test Get Table Registry Command");

    /* Test using a double buffered table */
    UT_InitData();
    CFE_TBL_Global.TblRegPacket.Payload.InactiveBufferAddr                  = CFE_ES_MEMADDRESS_C(0);
    CFE_TBL_Global.Registry[CFE_TBL_Global.HkTlmTblRegIndex].DoubleBuffered = true;
    CFE_TBL_GetTblRegData();
    UT_Report(__FILE__, __LINE__, CFE_TBL_Global.TblRegPacket.Payload.InactiveBufferAddr != 0, "CFE_TBL_GetTblRegData",
              "Double buffered table");

    /* Test using a single buffered table and the buffer is inactive */
    UT_InitData();
    CFE_TBL_Global.TblRegPacket.Payload.InactiveBufferAddr                  = CFE_ES_MEMADDRESS_C(0);
    CFE_TBL_Global.Registry[CFE_TBL_Global.HkTlmTblRegIndex].DoubleBuffered = false;
    CFE_TBL_Global.Registry[CFE_TBL_Global.HkTlmTblRegIndex].LoadInProgress = CFE_TBL_NO_LOAD_IN_PROGRESS + 1;
    CFE_TBL_GetTblRegData();
    UT_Report(__FILE__, __LINE__, CFE_TBL_Global.TblRegPacket.Payload.InactiveBufferAddr != 0, "CFE_TBL_GetTblRegData",
              "Single buffered table - inactive buffer");

    /* Test with no inactive buffer */
    UT_InitData();
    CFE_TBL_Global.TblRegPacket.Payload.InactiveBufferAddr                  = CFE_ES_MEMADDRESS_C(0);
    CFE_TBL_Global.Registry[CFE_TBL_Global.HkTlmTblRegIndex].LoadInProgress = CFE_TBL_NO_LOAD_IN_PROGRESS;
    CFE_TBL_GetTblRegData();
    UT_Report(__FILE__, __LINE__, CFE_TBL_Global.TblRegPacket.Payload.InactiveBufferAddr == 0, "CFE_TBL_GetTblRegData",
              "No inactive buffer");
}

/*
** Test the function that collects data and stores it in the housekeeping
** message
*/
void Test_CFE_TBL_GetHkData(void)
{
    int            i;
    int32          NumLoadPendingIndex = CFE_PLATFORM_TBL_MAX_NUM_TABLES - 1;
    int32          FreeSharedBuffIndex = CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS - 1;
    int32          ValTableIndex       = CFE_PLATFORM_TBL_MAX_NUM_VALIDATIONS - 1;
    CFE_ES_AppId_t AppID;

    /* Get the AppID being used for UT */
    CFE_ES_GetAppID(&AppID);

    UtPrintf("Begin Test Get Housekeeping Data");

    for (i = 0; i < CFE_PLATFORM_TBL_MAX_NUM_TABLES; i++)
    {
        CFE_TBL_Global.Registry[i].LoadPending = false;
    }

    /* Test raising the count of load pending tables */
    UT_InitData();
    CFE_TBL_Global.Registry[NumLoadPendingIndex].LoadPending = true;
    CFE_TBL_Global.Registry[NumLoadPendingIndex].OwnerAppId  = AppID;
    CFE_TBL_GetHkData();
    UT_Report(__FILE__, __LINE__, CFE_TBL_Global.HkPacket.Payload.NumLoadPending == 1, "CFE_TBL_GetHkData",
              "Raise load pending table count");

    /* Test lowering the count of free shared buffers */
    UT_InitData();
    CFE_TBL_Global.LoadBuffs[FreeSharedBuffIndex].Taken = true;
    CFE_TBL_GetHkData();
    UT_Report(__FILE__, __LINE__,
              CFE_TBL_Global.HkPacket.Payload.NumFreeSharedBufs == CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS - 1,
              "CFE_TBL_GetHkData", "Lower free shared buffer count");

    /* Test making a ValPtr with result = CFE_SUCCESS */
    UT_InitData();
    CFE_TBL_Global.SuccessValCounter                       = 0;
    CFE_TBL_Global.ValidationResults[ValTableIndex].State  = CFE_TBL_VALIDATION_PERFORMED;
    CFE_TBL_Global.ValidationResults[ValTableIndex].Result = CFE_SUCCESS;
    CFE_TBL_GetHkData();
    UT_Report(__FILE__, __LINE__, CFE_TBL_Global.SuccessValCounter == 1, "CFE_TBL_GetHkData",
              "ValPtr result CFE_SUCCESS");

    /* Test making a ValPtr without result = CFE_SUCCESS */
    UT_InitData();
    CFE_TBL_Global.FailedValCounter                        = 0;
    CFE_TBL_Global.ValidationResults[ValTableIndex].State  = CFE_TBL_VALIDATION_PERFORMED;
    CFE_TBL_Global.ValidationResults[ValTableIndex].Result = CFE_SUCCESS - 1;
    CFE_TBL_GetHkData();
    UT_Report(__FILE__, __LINE__, CFE_TBL_Global.FailedValCounter == 1, "CFE_TBL_GetHkData",
              "ValPtr result != CFE_SUCCESS");

    /* Test with an invalid registry entry */
    UT_InitData();
    CFE_TBL_Global.Registry[CFE_TBL_Global.LastTblUpdated].OwnerAppId = CFE_TBL_NOT_OWNED;
    CFE_TBL_Global.HkPacket.Payload.LastUpdateTime.Seconds            = 19283;
    CFE_TBL_GetHkData();
    UT_Report(__FILE__, __LINE__, CFE_TBL_Global.HkPacket.Payload.LastUpdateTime.Seconds == 19283, "CFE_TBL_GetHkData",
              "Invalid registry entry");

    /* Test with invalid last valid table updated out of range (low) */
    UT_InitData();
    CFE_TBL_Global.LastTblUpdated                          = -1;
    CFE_TBL_Global.HkPacket.Payload.LastUpdateTime.Seconds = 12345;
    CFE_TBL_GetHkData();
    UT_Report(__FILE__, __LINE__, CFE_TBL_Global.HkPacket.Payload.LastUpdateTime.Seconds == 12345, "CFE_TBL_GetHkData",
              "Last valid table updated out of range (low)");

    /* Test with invalid last valid table updated out of range (high) */
    UT_InitData();
    CFE_TBL_Global.LastTblUpdated                          = CFE_PLATFORM_TBL_MAX_NUM_TABLES;
    CFE_TBL_Global.HkPacket.Payload.LastUpdateTime.Seconds = 54321;
    CFE_TBL_GetHkData();
    UT_Report(__FILE__, __LINE__, CFE_TBL_Global.HkPacket.Payload.LastUpdateTime.Seconds == 54321, "CFE_TBL_GetHkData",
              "Last valid table updated out of range (high)");
}

/*
** Test the function that processes dump table registration to file
** command message
*/
void Test_CFE_TBL_DumpRegCmd(void)
{
    int                       q;
    CFE_TBL_DumpRegistryCmd_t DumpRegCmd;
    CFE_ES_AppId_t            AppID;
    size_t                    LocalSize;
    void *                    LocalBuf;
    bool                      IsEOF;

    /* Get the AppID being used for UT */
    CFE_ES_GetAppID(&AppID);

    UtPrintf("Begin Test Dump Register Command");

    for (q = 0; q < CFE_PLATFORM_TBL_MAX_NUM_TABLES; q++)
    {
        CFE_TBL_Global.Registry[q].HeadOfAccessList = CFE_TBL_END_OF_LIST;
    }

    /* Test command using the default dump file name (nominal path) */
    UT_InitData();
    UT_SetDefaultReturnValue(UT_KEY(CFE_FS_BackgroundFileDumpIsPending), false);
    strncpy(DumpRegCmd.Payload.DumpFilename, "X", sizeof(DumpRegCmd.Payload.DumpFilename) - 1);
    DumpRegCmd.Payload.DumpFilename[sizeof(DumpRegCmd.Payload.DumpFilename) - 1] = '\0';
    UT_Report(__FILE__, __LINE__, CFE_TBL_DumpRegistryCmd(&DumpRegCmd) == CFE_TBL_INC_CMD_CTR,
              "CFE_TBL_DumpRegistryCmd", "Default dump file name");

    /* Test command with a bad file name */
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_ParseInputFileNameEx), 1, CFE_FS_INVALID_PATH);
    UT_Report(__FILE__, __LINE__, CFE_TBL_DumpRegistryCmd(&DumpRegCmd) == CFE_TBL_INC_ERR_CTR,
              "CFE_TBL_DumpRegistryCmd", "Dump file name invalid");
    UT_ResetState(UT_KEY(CFE_FS_ParseInputFileNameEx));

    /* Test command with the dump file already pending (max requests pending) */
    UT_SetDefaultReturnValue(UT_KEY(CFE_FS_BackgroundFileDumpIsPending), true);
    UT_SetDefaultReturnValue(UT_KEY(CFE_FS_BackgroundFileDumpRequest), CFE_STATUS_REQUEST_ALREADY_PENDING);
    UT_Report(__FILE__, __LINE__, CFE_TBL_DumpRegistryCmd(&DumpRegCmd) == CFE_TBL_INC_ERR_CTR,
              "CFE_TBL_DumpRegistryCmd", "Dump file already pending (FS max requests)");
    UT_ResetState(UT_KEY(CFE_FS_BackgroundFileDumpRequest));

    /* Test command with the dump file already pending (local) */
    UT_SetDefaultReturnValue(UT_KEY(CFE_FS_BackgroundFileDumpIsPending), false);
    UT_SetDefaultReturnValue(UT_KEY(CFE_FS_BackgroundFileDumpRequest), CFE_STATUS_REQUEST_ALREADY_PENDING);
    UT_Report(__FILE__, __LINE__, CFE_TBL_DumpRegistryCmd(&DumpRegCmd) == CFE_TBL_INC_ERR_CTR,
              "CFE_TBL_DumpRegistryCmd", "Dump file already pending (local)");

    /* Check event generators */
    UT_ClearEventHistory();
    CFE_TBL_Global.RegDumpState.FileExisted = true;
    CFE_TBL_DumpRegistryEventHandler(&CFE_TBL_Global.RegDumpState, CFE_FS_FileWriteEvent_COMPLETE, CFE_SUCCESS, 10, 0,
                                     1000);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_TBL_OVERWRITE_REG_DUMP_INF_EID),
              "CFE_TBL_DumpRegistryEventHandler", "Dump file created event (overwrite)");

    UT_ClearEventHistory();
    CFE_TBL_Global.RegDumpState.FileExisted = false;
    CFE_TBL_DumpRegistryEventHandler(&CFE_TBL_Global.RegDumpState, CFE_FS_FileWriteEvent_COMPLETE, CFE_SUCCESS, 10, 0,
                                     1000);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_TBL_WRITE_REG_DUMP_INF_EID),
              "CFE_TBL_DumpRegistryEventHandler", "Dump file created event (new)");

    UT_ClearEventHistory();
    CFE_TBL_DumpRegistryEventHandler(&CFE_TBL_Global.RegDumpState, CFE_FS_FileWriteEvent_RECORD_WRITE_ERROR,
                                     CFE_SUCCESS, 10, 10, 1000);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_TBL_WRITE_TBL_REG_ERR_EID),
              "CFE_TBL_DumpRegistryEventHandler", "Dump file record write error event");

    UT_ClearEventHistory();
    CFE_TBL_DumpRegistryEventHandler(&CFE_TBL_Global.RegDumpState, CFE_FS_FileWriteEvent_HEADER_WRITE_ERROR,
                                     CFE_SUCCESS, 10, 10, 1000);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_TBL_WRITE_CFE_HDR_ERR_EID),
              "CFE_TBL_DumpRegistryEventHandler", "Dump file header write error event");

    UT_ClearEventHistory();
    CFE_TBL_DumpRegistryEventHandler(&CFE_TBL_Global.RegDumpState, CFE_FS_FileWriteEvent_CREATE_ERROR, OS_ERROR, 10, 0,
                                     0);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_TBL_CREATING_DUMP_FILE_ERR_EID),
              "CFE_TBL_DumpRegistryEventHandler", "Dump file created error event");

    UT_ClearEventHistory();
    CFE_TBL_DumpRegistryEventHandler(&CFE_TBL_Global.RegDumpState, CFE_FS_FileWriteEvent_UNDEFINED, OS_ERROR, 0, 0, 0);
    UT_Report(__FILE__, __LINE__, UT_GetNumEventsSent() == 0, "CFE_TBL_DumpRegistryEventHandler",
              "Undefined event is ignored");

    /* Test where the table is owned, the file doesn't already exist, and the
     * table is successfully dumped
     */
    UT_InitData();
    CFE_TBL_Global.Registry[0].OwnerAppId       = AppID;
    CFE_TBL_Global.Registry[0].HeadOfAccessList = CFE_TBL_END_OF_LIST;
    CFE_TBL_Global.Registry[1].OwnerAppId       = CFE_TBL_NOT_OWNED;
    CFE_TBL_Global.Registry[0].LoadInProgress   = CFE_TBL_NO_LOAD_IN_PROGRESS + 1;
    CFE_TBL_Global.Registry[0].DoubleBuffered   = true;
    LocalBuf                                    = NULL;
    LocalSize                                   = 0;
    IsEOF = CFE_TBL_DumpRegistryGetter(&CFE_TBL_Global.RegDumpState, 0, &LocalBuf, &LocalSize);
    UT_Report(__FILE__, __LINE__, !IsEOF, "CFE_TBL_DumpRegistryGetter", "Nominal, first record, not end of file");
    UtAssert_NOT_NULL(LocalBuf);
    UtAssert_NONZERO(LocalSize);

    CFE_TBL_Global.Registry[CFE_PLATFORM_TBL_MAX_NUM_TABLES - 1].OwnerAppId       = CFE_TBL_NOT_OWNED;
    CFE_TBL_Global.Registry[CFE_PLATFORM_TBL_MAX_NUM_TABLES - 1].HeadOfAccessList = 2;
    CFE_TBL_Global.Handles[2].NextLink                                            = CFE_TBL_END_OF_LIST;
    LocalBuf                                                                      = NULL;
    LocalSize                                                                     = 0;
    IsEOF = CFE_TBL_DumpRegistryGetter(&CFE_TBL_Global.RegDumpState, CFE_PLATFORM_TBL_MAX_NUM_TABLES - 1, &LocalBuf,
                                       &LocalSize);
    UT_Report(__FILE__, __LINE__, IsEOF, "CFE_TBL_DumpRegistryGetter",
              "Nominal, last record, multiple accessors, end of file");
    UtAssert_NOT_NULL(LocalBuf);
    UtAssert_NONZERO(LocalSize);

    /* Test with record numb beyond EOF (should be ignored, return null) */
    IsEOF = CFE_TBL_DumpRegistryGetter(&CFE_TBL_Global.RegDumpState, CFE_PLATFORM_TBL_MAX_NUM_TABLES + 1, &LocalBuf,
                                       &LocalSize);
    UT_Report(__FILE__, __LINE__, IsEOF, "CFE_TBL_DumpRegistryGetter", "Past end of file");
    UtAssert_NULL(LocalBuf);
    UtAssert_ZERO(LocalSize);
}

/*
** Test the function that processes dump table to file command message
*/
void Test_CFE_TBL_DumpCmd(void)
{
    int                i, k, u;
    uint8              Buff;
    uint8 *            BuffPtr = &Buff;
    CFE_TBL_LoadBuff_t Load    = {0};
    CFE_TBL_DumpCmd_t  DumpCmd;
    CFE_ES_AppId_t     AppID;

    CFE_ES_GetAppID(&AppID);

    UtPrintf("Begin Test Dump Command");

    memset(&DumpCmd, 0, sizeof(DumpCmd));

    /* Test where the table cannot be found in the registry */
    UT_InitData();
    snprintf(DumpCmd.Payload.TableName, sizeof(DumpCmd.Payload.TableName), "%d", CFE_PLATFORM_TBL_MAX_NUM_TABLES + 1);
    UT_Report(__FILE__, __LINE__, CFE_TBL_DumpCmd(&DumpCmd) == CFE_TBL_INC_ERR_CTR, "CFE_TBL_DumpCmd",
              "Table registry entry doesn't exist");

    /* Test with an active buffer, the pointer is created, validation passes,
     * the table is dump only, no dump is already in progress, and have a
     * working buffer; load in progress, single-buffered
     */
    UT_InitData();
    strncpy(CFE_TBL_Global.Registry[2].Name, "DumpCmdTest", sizeof(CFE_TBL_Global.Registry[2].Name) - 1);
    CFE_TBL_Global.Registry[2].Name[sizeof(CFE_TBL_Global.Registry[2].Name) - 1] = '\0';
    CFE_TBL_Global.Registry[2].OwnerAppId                                        = AppID;
    strncpy(DumpCmd.Payload.TableName, CFE_TBL_Global.Registry[2].Name, sizeof(DumpCmd.Payload.TableName) - 1);
    DumpCmd.Payload.TableName[sizeof(DumpCmd.Payload.TableName) - 1] = '\0';
    DumpCmd.Payload.ActiveTableFlag                                  = CFE_TBL_BufferSelect_ACTIVE;
    CFE_TBL_Global.Registry[2].Buffers[CFE_TBL_Global.Registry[2].ActiveBufferIndex].BufferPtr = BuffPtr;

    for (i = 0; i < CFE_PLATFORM_TBL_MAX_NUM_TABLES; i++)
    {
        CFE_TBL_Global.Registry[i].DumpOnly = true;
    }

    CFE_TBL_Global.DumpControlBlocks[2].State                           = CFE_TBL_DUMP_PENDING;
    CFE_TBL_Global.DumpControlBlocks[3].State                           = CFE_TBL_DUMP_FREE;
    CFE_TBL_Global.Registry[2].DumpControlIndex                         = CFE_TBL_NO_DUMP_PENDING;
    CFE_TBL_Global.Registry[2].LoadInProgress                           = CFE_TBL_NO_LOAD_IN_PROGRESS + 1;
    CFE_TBL_Global.Registry[2].DoubleBuffered                           = false;
    CFE_TBL_Global.LoadBuffs[CFE_TBL_Global.Registry[2].LoadInProgress] = Load;
    CFE_TBL_Global.Registry[2].NotifyByMsg                              = true;
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_TransmitMsg), 1, CFE_SB_INTERNAL_ERR);
    UT_Report(__FILE__, __LINE__, CFE_TBL_DumpCmd(&DumpCmd) == CFE_TBL_INC_CMD_CTR, "CFE_TBL_DumpCmd",
              "Active buffer, pointer created, validation passes, is a dump "
              "only table, no dump already in progress, got working buffer");

    /* Test with an active buffer, a pointer is created, the table is dump
     * only, no dump is already progress, and fails to get a working buffer;
     * no load in progress, TableLoadedOnce = true, single-buffered, no buffer
     * available
     */
    UT_InitData();
    CFE_TBL_Global.DumpControlBlocks[2].State   = CFE_TBL_DUMP_FREE;
    CFE_TBL_Global.Registry[2].DumpControlIndex = CFE_TBL_NO_DUMP_PENDING;
    CFE_TBL_Global.Registry[2].LoadInProgress   = CFE_TBL_NO_LOAD_IN_PROGRESS;
    CFE_TBL_Global.Registry[2].TableLoadedOnce  = true;
    CFE_TBL_Global.Registry[2].DoubleBuffered   = false;

    for (u = 0; u < CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS; u++)
    {
        CFE_TBL_Global.LoadBuffs[u].Taken = true;
    }

    CFE_TBL_Global.Registry[2].NotifyByMsg = true;
    UT_Report(__FILE__, __LINE__, CFE_TBL_DumpCmd(&DumpCmd) == CFE_TBL_INC_ERR_CTR, "CFE_TBL_DumpCmd",
              "Active buffer, pointer created, is a dump only table, no dump "
              "already in progress, fails to get a working buffer: No "
              "working buffers available");

    /* Test with an active buffer, a pointer is created, the table is dump
     * only, and no dump fails to find a free dump control block; too many
     * dump only table dumps have been requested
     */
    UT_InitData();
    CFE_TBL_Global.Registry[2].DumpControlIndex = CFE_TBL_NO_DUMP_PENDING;

    for (k = 0; k < CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS; k++)
    {
        CFE_TBL_Global.DumpControlBlocks[k].State = CFE_TBL_DUMP_PENDING;
    }

    CFE_TBL_Global.Registry[2].NotifyByMsg = true;
    UT_Report(__FILE__, __LINE__, CFE_TBL_DumpCmd(&DumpCmd) == CFE_TBL_INC_ERR_CTR, "CFE_TBL_DumpCmd",
              "Active buffer, pointer created, is dump only table, fails to "
              "find a free dump control block: too many dump only table "
              "dumps have been requested");

    /* Test with an inactive buffer, double-buffered, dump already in progress;
     * dump is already pending
     */
    UT_InitData();
    DumpCmd.Payload.ActiveTableFlag           = CFE_TBL_BufferSelect_INACTIVE;
    CFE_TBL_Global.Registry[2].DoubleBuffered = true;
    CFE_TBL_Global.Registry[2].Buffers[(1 - CFE_TBL_Global.Registry[2].ActiveBufferIndex)].BufferPtr = BuffPtr;
    CFE_TBL_Global.Registry[2].DumpControlIndex = CFE_TBL_NO_DUMP_PENDING + 1;
    UT_Report(__FILE__, __LINE__, CFE_TBL_DumpCmd(&DumpCmd) == CFE_TBL_INC_ERR_CTR, "CFE_TBL_DumpCmd",
              "Inactive buffer, double-buffered, dump already in progress: "
              "dump is already pending");

    /* Test with an inactive buffer, single-buffered, pointer created, is a
     * dump only table
     */
    UT_InitData();
    CFE_TBL_Global.Registry[2].DoubleBuffered                                     = false;
    CFE_TBL_Global.Registry[2].LoadInProgress                                     = CFE_TBL_NO_LOAD_IN_PROGRESS + 1;
    CFE_TBL_Global.LoadBuffs[CFE_TBL_Global.Registry[2].LoadInProgress].BufferPtr = BuffPtr;
    CFE_TBL_Global.Registry[2].DumpOnly                                           = false;
    strncpy(DumpCmd.Payload.DumpFilename, CFE_TBL_Global.Registry[2].LastFileLoaded,
            sizeof(DumpCmd.Payload.DumpFilename) - 1);
    DumpCmd.Payload.DumpFilename[sizeof(DumpCmd.Payload.DumpFilename) - 1] = '\0';
    UT_Report(__FILE__, __LINE__, CFE_TBL_DumpCmd(&DumpCmd) == CFE_TBL_INC_CMD_CTR, "CFE_TBL_DumpCmd",
              "Inactive buffer, single-buffered, pointer created, is a dump "
              "only table");

    /* Test with an inactive buffer, single-buffered: No inactive buffer for
     * table due to load in progress
     */
    UT_InitData();
    CFE_TBL_Global.Registry[2].LoadInProgress = CFE_TBL_NO_LOAD_IN_PROGRESS;
    UT_Report(__FILE__, __LINE__, CFE_TBL_DumpCmd(&DumpCmd) == CFE_TBL_INC_ERR_CTR, "CFE_TBL_DumpCmd",
              "Inactive buffer, single-buffered: no inactive buffer for table "
              "due to load in progress");

    /* Test with an inactive buffer, single-buffered: No inactive buffer for
     * table due to user defined address
     */
    UT_InitData();
    CFE_TBL_Global.Registry[2].LoadInProgress = CFE_TBL_NO_LOAD_IN_PROGRESS + 1;
    CFE_TBL_Global.Registry[2].UserDefAddr    = true;
    UT_Report(__FILE__, __LINE__, CFE_TBL_DumpCmd(&DumpCmd) == CFE_TBL_INC_ERR_CTR, "CFE_TBL_DumpCmd",
              "Inactive buffer, single-buffered: no inactive buffer for table "
              "due to user defined address");

    /* Test with an illegal buffer parameter */
    UT_InitData();
    DumpCmd.Payload.ActiveTableFlag = CFE_TBL_BufferSelect_ACTIVE + 1;
    UT_Report(__FILE__, __LINE__, CFE_TBL_DumpCmd(&DumpCmd) == CFE_TBL_INC_ERR_CTR, "CFE_TBL_DumpCmd",
              "Illegal buffer parameter");
}

/*
** Test the function that processes load table file to buffer command
** message
*/
void Test_CFE_TBL_LoadCmd(void)
{
    int                i, j;
    CFE_TBL_File_Hdr_t TblFileHeader;
    CFE_FS_Header_t    StdFileHeader;
    CFE_TBL_LoadBuff_t BufferPtr = CFE_TBL_Global.LoadBuffs[0];
    CFE_TBL_LoadCmd_t  LoadCmd;
    CFE_ES_AppId_t     AppID;

    CFE_ES_GetAppID(&AppID);

    UtPrintf("Begin Test Load Command");

    StdFileHeader.SpacecraftID = CFE_PLATFORM_TBL_VALID_SCID_1;
    StdFileHeader.ProcessorID  = CFE_PLATFORM_TBL_VALID_PRID_1;

    /* Test response to inability to open file */
    UT_InitData();
    strncpy(LoadCmd.Payload.LoadFilename, "LoadFileName", sizeof(LoadCmd.Payload.LoadFilename) - 1);
    LoadCmd.Payload.LoadFilename[sizeof(LoadCmd.Payload.LoadFilename) - 1] = '\0';
    UT_SetDefaultReturnValue(UT_KEY(OS_OpenCreate), OS_ERROR);
    UT_Report(__FILE__, __LINE__, CFE_TBL_LoadCmd(&LoadCmd) == CFE_TBL_INC_ERR_CTR, "CFE_TBL_LoadCmd",
              "Unable to open file");

    /* Test response to inability to find the table in the registry */
    UT_InitData();

    for (i = 0; i < CFE_PLATFORM_TBL_MAX_NUM_TABLES; i++)
    {
        CFE_TBL_Global.Registry[i].OwnerAppId  = CFE_TBL_NOT_OWNED;
        CFE_TBL_Global.Registry[i].LoadPending = false;
    }

    strncpy(TblFileHeader.TableName, CFE_TBL_Global.Registry[0].Name, sizeof(TblFileHeader.TableName) - 1);
    TblFileHeader.TableName[sizeof(TblFileHeader.TableName) - 1] = '\0';
    strncpy(StdFileHeader.Description, "FS header description", sizeof(StdFileHeader.Description) - 1);
    StdFileHeader.Description[sizeof(StdFileHeader.Description) - 1] = '\0';
    StdFileHeader.ContentType                                        = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType                                            = CFE_FS_SubType_TBL_IMG;
    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_Report(__FILE__, __LINE__, CFE_TBL_LoadCmd(&LoadCmd) == CFE_TBL_INC_ERR_CTR, "CFE_TBL_LoadCmd",
              "Table registry entry doesn't exist");

    /* Test attempt to load a dump only table */
    UT_InitData();
    CFE_TBL_Global.Registry[0].OwnerAppId = AppID;
    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    CFE_TBL_Global.Registry[0].Size     = sizeof(CFE_TBL_File_Hdr_t) + 1;
    CFE_TBL_Global.Registry[0].DumpOnly = true;
    UT_Report(__FILE__, __LINE__, CFE_TBL_LoadCmd(&LoadCmd) == CFE_TBL_INC_ERR_CTR, "CFE_TBL_LoadCmd",
              "Attempting to load a dump only table");

    /* Test attempt to load a table with a load already pending */
    UT_InitData();
    CFE_TBL_Global.Registry[0].OwnerAppId = AppID;
    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    CFE_TBL_Global.Registry[0].Size        = sizeof(CFE_TBL_File_Hdr_t) + 1;
    CFE_TBL_Global.Registry[0].DumpOnly    = false;
    CFE_TBL_Global.Registry[0].LoadPending = true;
    UT_Report(__FILE__, __LINE__, CFE_TBL_LoadCmd(&LoadCmd) == CFE_TBL_INC_ERR_CTR, "CFE_TBL_LoadCmd",
              "Attempting to load a table with load already pending");
    CFE_TBL_Global.Registry[0].LoadPending = false;

    /* Test where the file isn't dump only and passes table checks, get a
     * working buffer, and there is an extra byte (more data than header
     * indicates)
     */
    UT_InitData();
    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(CFE_TBL_File_Hdr_t));
    CFE_TBL_Global.Registry[0].TableLoadedOnce = true;

    CFE_TBL_Global.Registry[0].Size                                               = sizeof(CFE_TBL_File_Hdr_t);
    CFE_TBL_Global.Registry[0].LoadInProgress                                     = CFE_TBL_NO_LOAD_IN_PROGRESS + 1;
    CFE_TBL_Global.Registry[0].DoubleBuffered                                     = false;
    CFE_TBL_Global.LoadBuffs[CFE_TBL_Global.Registry[0].LoadInProgress].BufferPtr = (uint8 *)&BufferPtr;
    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    CFE_TBL_Global.Registry[0].DumpOnly = false;
    UT_Report(__FILE__, __LINE__, CFE_TBL_LoadCmd(&LoadCmd) == CFE_TBL_INC_ERR_CTR, "CFE_TBL_LoadCmd",
              "There is more data than the file indicates");

    /* Test with no extra byte => successful load */
    UT_InitData();
    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(CFE_TBL_File_Hdr_t));

    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, 0);
    strncpy(TblFileHeader.TableName, CFE_TBL_Global.Registry[0].Name, sizeof(TblFileHeader.TableName) - 1);
    TblFileHeader.TableName[sizeof(TblFileHeader.TableName) - 1] = '\0';
    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_Report(__FILE__, __LINE__, CFE_TBL_LoadCmd(&LoadCmd) == CFE_TBL_INC_CMD_CTR, "CFE_TBL_LoadCmd",
              "Successful load");

    /* Test with differing amount of data from header's claim */
    UT_InitData();
    TblFileHeader.NumBytes = CFE_ES_MEMOFFSET_C(sizeof(CFE_TBL_File_Hdr_t));

    if (UT_Endianess == UT_LITTLE_ENDIAN)
    {
        CFE_TBL_ByteSwapUint32(&TblFileHeader.NumBytes);
    }

    strncpy(TblFileHeader.TableName, CFE_TBL_Global.Registry[0].Name, sizeof(TblFileHeader.TableName) - 1);
    TblFileHeader.TableName[sizeof(TblFileHeader.TableName) - 1] = '\0';
    UT_SetDeferredRetcode(UT_KEY(OS_read), 2, 0);
    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_Report(__FILE__, __LINE__, CFE_TBL_LoadCmd(&LoadCmd) == CFE_TBL_INC_ERR_CTR, "CFE_TBL_LoadCmd",
              "Incomplete load of file into the working buffer");

    /* Test with no working buffers available */
    UT_InitData();
    CFE_TBL_Global.Registry[0].LoadInProgress                                        = CFE_TBL_NO_LOAD_IN_PROGRESS;
    CFE_TBL_Global.Registry[0].TableLoadedOnce                                       = true;
    CFE_TBL_Global.Registry[0].DoubleBuffered                                        = false;
    CFE_TBL_Global.Registry[0].Buffers[CFE_TBL_Global.Registry[0].ActiveBufferIndex] = BufferPtr;

    for (j = 0; j < CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS; j++)
    {
        CFE_TBL_Global.LoadBuffs[j].Taken = true;
    }

    strncpy(TblFileHeader.TableName, CFE_TBL_Global.Registry[0].Name, sizeof(TblFileHeader.TableName) - 1);
    TblFileHeader.TableName[sizeof(TblFileHeader.TableName) - 1] = '\0';
    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_Report(__FILE__, __LINE__, CFE_TBL_LoadCmd(&LoadCmd) == CFE_TBL_INC_ERR_CTR, "CFE_TBL_LoadCmd",
              "No working buffers available");

    /* Test with table header indicating data beyond size of the table */
    UT_InitData();
    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(CFE_TBL_File_Hdr_t));

    CFE_TBL_Global.Registry[0].Size = sizeof(CFE_TBL_File_Hdr_t) - 1;
    strncpy(TblFileHeader.TableName, CFE_TBL_Global.Registry[0].Name, sizeof(TblFileHeader.TableName) - 1);
    TblFileHeader.TableName[sizeof(TblFileHeader.TableName) - 1] = '\0';
    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_Report(__FILE__, __LINE__, CFE_TBL_LoadCmd(&LoadCmd) == CFE_TBL_INC_ERR_CTR, "CFE_TBL_LoadCmd",
              "Table header indicates data beyond size of the table");

    /* Test with table header indicating no data in the file */
    UT_InitData();
    UT_TBL_SetupHeader(&TblFileHeader, 0, 0);

    strncpy(TblFileHeader.TableName, CFE_TBL_Global.Registry[0].Name, sizeof(TblFileHeader.TableName) - 1);
    TblFileHeader.TableName[sizeof(TblFileHeader.TableName) - 1] = '\0';
    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_Report(__FILE__, __LINE__, CFE_TBL_LoadCmd(&LoadCmd) == CFE_TBL_INC_ERR_CTR, "CFE_TBL_LoadCmd",
              "Table header indicates no data in file");

    /* Test where file has partial load for uninitialized table and offset
     * is non-zero
     */
    UT_InitData();
    UT_TBL_SetupHeader(&TblFileHeader, 1, 1);

    CFE_TBL_Global.Registry[0].TableLoadedOnce = false;

    CFE_TBL_Global.Registry[0].Size = sizeof(CFE_TBL_File_Hdr_t);
    strncpy(TblFileHeader.TableName, CFE_TBL_Global.Registry[0].Name, sizeof(TblFileHeader.TableName) - 1);
    TblFileHeader.TableName[sizeof(TblFileHeader.TableName) - 1] = '\0';
    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_Report(__FILE__, __LINE__, CFE_TBL_LoadCmd(&LoadCmd) == CFE_TBL_INC_ERR_CTR, "CFE_TBL_LoadCmd",
              "File has partial load for uninitialized table and offset "
              "is non-zero");

    /* Test where file has partial load for uninitialized table and offset
     * is zero
     */
    UT_InitData();
    UT_TBL_SetupHeader(&TblFileHeader, 0, 1);

    CFE_TBL_Global.Registry[0].TableLoadedOnce = false;

    CFE_TBL_Global.Registry[0].Size = sizeof(CFE_TBL_File_Hdr_t);
    strncpy(TblFileHeader.TableName, CFE_TBL_Global.Registry[0].Name, sizeof(TblFileHeader.TableName) - 1);
    TblFileHeader.TableName[sizeof(TblFileHeader.TableName) - 1] = '\0';
    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_Report(__FILE__, __LINE__, CFE_TBL_LoadCmd(&LoadCmd) == CFE_TBL_INC_ERR_CTR, "CFE_TBL_LoadCmd",
              "File has partial load for uninitialized table and offset "
              "is zero");

    /* Test response to inability to read the file header */
    UT_InitData();
    strncpy(LoadCmd.Payload.LoadFilename, "LoadFileName", sizeof(LoadCmd.Payload.LoadFilename) - 1);
    LoadCmd.Payload.LoadFilename[sizeof(LoadCmd.Payload.LoadFilename) - 1] = '\0';
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_ReadHeader), 1, sizeof(CFE_FS_Header_t) - 1);
    UT_Report(__FILE__, __LINE__, CFE_TBL_LoadCmd(&LoadCmd) == CFE_TBL_INC_ERR_CTR, "CFE_TBL_LoadCmd",
              "Unable to read file header");
}

/*
** Test the function that processes housekeeping request message
*/
void Test_CFE_TBL_HousekeepingCmd(void)
{
    int                   i;
    CFE_TBL_LoadBuff_t    DumpBuff;
    CFE_TBL_LoadBuff_t *  DumpBuffPtr = &DumpBuff;
    CFE_TBL_RegistryRec_t RegRecPtr;
    uint8                 Buff;
    void *                BuffPtr    = &Buff;
    uint32                Secs       = 0;
    uint32                SubSecs    = 0;
    int32                 LoadInProg = 0;

    UtPrintf("Begin Test Housekeeping Command");

    /* Test response to inability to update timestamp in dump file + inability
     * to send Hk packet
     */
    UT_InitData();
    strncpy(CFE_TBL_Global.DumpControlBlocks[0].TableName, "housekeepingtest",
            sizeof(CFE_TBL_Global.DumpControlBlocks[0].TableName) - 1);
    CFE_TBL_Global.DumpControlBlocks[0].TableName[sizeof(CFE_TBL_Global.DumpControlBlocks[0].TableName) - 1] = '\0';
    CFE_TBL_Global.DumpControlBlocks[0].Size                                                                 = 10;
    LoadInProg                                    = CFE_TBL_NO_LOAD_IN_PROGRESS + 1;
    RegRecPtr.LoadInProgress                      = LoadInProg;
    CFE_TBL_Global.DumpControlBlocks[0].RegRecPtr = &RegRecPtr;
    DumpBuffPtr->Taken                            = true;
    DumpBuffPtr->Validated                        = true;
    DumpBuffPtr->BufferPtr                        = BuffPtr;
    DumpBuffPtr->FileCreateTimeSecs               = Secs;
    DumpBuffPtr->FileCreateTimeSubSecs            = SubSecs;
    strncpy(DumpBuffPtr->DataSource, "hkSource", sizeof(DumpBuffPtr->DataSource) - 1);
    DumpBuffPtr->DataSource[sizeof(DumpBuffPtr->DataSource) - 1] = '\0';
    CFE_TBL_Global.DumpControlBlocks[0].DumpBufferPtr            = DumpBuffPtr;
    CFE_TBL_Global.DumpControlBlocks[0].State                    = CFE_TBL_DUMP_PERFORMED;

    for (i = 1; i < CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS; i++)
    {
        CFE_TBL_Global.DumpControlBlocks[i].State = CFE_TBL_DUMP_PENDING;
    }

    UT_SetDeferredRetcode(UT_KEY(CFE_SB_TransmitMsg), 1, CFE_SUCCESS - 1);
    CFE_TBL_Global.HkTlmTblRegIndex = CFE_TBL_NOT_FOUND + 1;
    UT_Report(__FILE__, __LINE__, CFE_TBL_HousekeepingCmd(NULL) == CFE_TBL_DONT_INC_CTR, "CFE_TBL_HousekeepingCmd",
              "Able to open dump file");

    for (i = 1; i < CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS; i++)
    {
        CFE_TBL_Global.DumpControlBlocks[i].State = CFE_TBL_DUMP_PENDING;
    }

    RegRecPtr.LoadInProgress                      = LoadInProg;
    CFE_TBL_Global.DumpControlBlocks[0].RegRecPtr = &RegRecPtr;

    /* Test response to inability to open dump file */
    UT_InitData();
    CFE_TBL_Global.DumpControlBlocks[0].State = CFE_TBL_DUMP_PERFORMED;
    CFE_TBL_Global.HkTlmTblRegIndex           = CFE_TBL_NOT_FOUND + 1;
    UT_SetDefaultReturnValue(UT_KEY(OS_OpenCreate), OS_ERROR);
    UT_Report(__FILE__, __LINE__, CFE_TBL_HousekeepingCmd(NULL) == CFE_TBL_DONT_INC_CTR, "CFE_TBL_HousekeepingCmd",
              "Unable to open dump file");

    /* Test response to an invalid table and a dump file create failure */
    UT_InitData();
    CFE_TBL_Global.HkTlmTblRegIndex           = CFE_TBL_NOT_FOUND;
    CFE_TBL_Global.DumpControlBlocks[0].State = CFE_TBL_DUMP_PERFORMED;
    UT_SetDefaultReturnValue(UT_KEY(OS_OpenCreate), OS_ERROR);
    UT_Report(__FILE__, __LINE__, CFE_TBL_HousekeepingCmd(NULL) == CFE_TBL_DONT_INC_CTR, "CFE_TBL_HousekeepingCmd",
              "Invalid table and dump file create failure");

    /* Test response to a file time stamp failure */
    UT_InitData();
    CFE_TBL_Global.DumpControlBlocks[0].State = CFE_TBL_DUMP_PERFORMED;
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_SetTimestamp), 1, OS_SUCCESS - 1);
    UT_Report(__FILE__, __LINE__, CFE_TBL_HousekeepingCmd(NULL) == CFE_TBL_DONT_INC_CTR, "CFE_TBL_HousekeepingCmd",
              "Time stamp file failure");
}

/*
** Function to prepare for test table API functions
*/
void Test_CFE_TBL_ApiInit(void)
{
    UT_ResetCDS();
    CFE_TBL_EarlyInit();
    CFE_TBL_Global.TableTaskAppId = UT_TBL_APPID_10;
}

/*
** Test function that registers a table with cFE to obtain table management
** services
*/
void Test_CFE_TBL_Register(void)
{
    int32                       RtnCode;
    int32                       RtnCode2;
    CFE_TBL_Handle_t            TblHandle1;
    CFE_TBL_Handle_t            TblHandle2;
    CFE_TBL_Handle_t            TblHandle3;
    bool                        EventsCorrect;
    char                        TblName[CFE_MISSION_TBL_MAX_NAME_LENGTH + 2];
    int16                       i;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr;
    CFE_TBL_RegistryRec_t *     RegRecPtr;

    UtPrintf("Begin Test Register");

    /* Test response to an invalid application ID */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetAppID), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    RtnCode       = CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_DEFAULT, NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_ES_ERR_RESOURCEID_NOT_VALID && EventsCorrect, "CFE_TBL_Register",
              "Invalid application ID");

    /* Test response to a table name longer than the maximum allowed */
    UT_InitData();

    /* Create a table name longer than alowed */
    for (i = 0; i < CFE_MISSION_TBL_MAX_NAME_LENGTH + 1; i++)
    {
        TblName[i] = 'A';
    }

    TblName[i]    = '\0';
    RtnCode       = CFE_TBL_Register(&TblHandle1, TblName, sizeof(UT_Table1_t), CFE_TBL_OPT_DEFAULT, NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_INVALID_NAME && EventsCorrect, "CFE_TBL_Register",
              "Table name too long");

    /* Test response to a table name shorter than the minimum allowed */
    UT_InitData();
    RtnCode       = CFE_TBL_Register(&TblHandle1, "", sizeof(UT_Table1_t), CFE_TBL_OPT_DEFAULT, NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_INVALID_NAME && EventsCorrect, "CFE_TBL_Register",
              "Table name too short");

    /*  Test response to a table size of zero */
    UT_InitData();
    RtnCode       = CFE_TBL_Register(&TblHandle1, "UT_Table1", 0, CFE_TBL_OPT_DEFAULT, NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_INVALID_SIZE && EventsCorrect, "CFE_TBL_Register",
              "Size of table = 0");

    /* Test response to a table size larger than the maximum allowed */
    UT_InitData();
    RtnCode       = CFE_TBL_Register(&TblHandle1, "UT_Table1", (CFE_PLATFORM_TBL_MAX_SNGL_TABLE_SIZE + 1),
                               CFE_TBL_OPT_DEFAULT, NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_INVALID_SIZE && EventsCorrect, "CFE_TBL_Register",
              "Table size too large");

    /* Test response to a double-buffered table size larger than the
     * maximum allowed
     */
    UT_InitData();
    RtnCode       = CFE_TBL_Register(&TblHandle1, "UT_Table1", (CFE_PLATFORM_TBL_MAX_DBL_TABLE_SIZE + 1),
                               CFE_TBL_OPT_DBL_BUFFER, NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_INVALID_SIZE && EventsCorrect, "CFE_TBL_Register",
              "Double-buffered table size too large");

    /* Test response to an invalid table option combination
     * (CFE_TBL_OPT_USR_DEF_ADDR | CFE_TBL_OPT_DBL_BUFFER)
     */
    UT_InitData();
    RtnCode       = CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t),
                               ((CFE_TBL_OPT_USR_DEF_ADDR & ~CFE_TBL_OPT_LD_DMP_MSK) | CFE_TBL_OPT_DBL_BUFFER), NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_INVALID_OPTIONS && EventsCorrect, "CFE_TBL_Register",
              "Invalid table option combination (CFE_TBL_OPT_USR_DEF_ADDR | "
              "CFE_TBL_OPT_DBL_BUFFER)");

    /* Test response to an invalid table option combination
     * (CFE_TBL_OPT_USR_DEF_ADDR | CFE_TBL_OPT_LOAD_DUMP)
     */
    UT_InitData();
    RtnCode       = CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t),
                               (CFE_TBL_OPT_USR_DEF_ADDR & ~CFE_TBL_OPT_LD_DMP_MSK), NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_INVALID_OPTIONS && EventsCorrect, "CFE_TBL_Register",
              "Invalid table option combination (CFE_TBL_OPT_USR_DEF_ADDR | "
              "CFE_TBL_OPT_LOAD_DUMP)");

    /* Test response to an invalid table option combination
     * (CFE_TBL_OPT_USR_DEF_ADDR | CFE_TBL_OPT_CRITICAL)
     */
    UT_InitData();
    RtnCode       = CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t),
                               ((CFE_TBL_OPT_USR_DEF_ADDR & ~CFE_TBL_OPT_LD_DMP_MSK) | CFE_TBL_OPT_CRITICAL), NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_INVALID_OPTIONS && EventsCorrect, "CFE_TBL_Register",
              "Invalid table option combination (CFE_TBL_OPT_USR_DEF_ADDR | "
              "CFE_TBL_OPT_CRITICAL)");

    /* Test response to an invalid table option combination
     * (CFE_TBL_OPT_DUMP_ONLY | CFE_TBL_OPT_DBL_BUFFER)
     */
    UT_InitData();
    RtnCode       = CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t),
                               (CFE_TBL_OPT_DUMP_ONLY | CFE_TBL_OPT_DBL_BUFFER), NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_INVALID_OPTIONS && EventsCorrect, "CFE_TBL_Register",
              "Invalid table option combination (CFE_TBL_OPT_DUMP_ONLY | "
              "CFE_TBL_OPT_DBL_BUFFER)");

    /* Test response to a memory handle error */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetPoolBuf), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    RtnCode       = CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_DEFAULT, NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_ES_ERR_RESOURCEID_NOT_VALID && EventsCorrect, "CFE_TBL_Register",
              "Memory handle error");

    /* Test response to a memory block size error */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetPoolBuf), 1, CFE_ES_ERR_MEM_BLOCK_SIZE);
    RtnCode       = CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_DEFAULT, NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_ES_ERR_MEM_BLOCK_SIZE && EventsCorrect, "CFE_TBL_Register",
              "Memory block size error");

    /* Test response to a memory block size error (for a second buffer) */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetPoolBuf), 2, CFE_ES_ERR_MEM_BLOCK_SIZE);
    RtnCode       = CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_DBL_BUFFER, NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_ES_ERR_MEM_BLOCK_SIZE && EventsCorrect, "CFE_TBL_Register",
              "Memory block size error (for second buffer)");

    /* Test successfully getting a double buffered table */
    UT_InitData();
    RtnCode       = CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_DBL_BUFFER, NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == false && UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_Register",
              "Get a double buffered table - successful");

    /* Test attempt to register table owned by another application */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_2);
    RtnCode       = CFE_TBL_Register(&TblHandle3, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_DBL_BUFFER, NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_DUPLICATE_NOT_OWNED && EventsCorrect, "CFE_TBL_Register",
              "Table owned by another application");

    /* Test attempt to register existing table with a different size */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    RtnCode       = CFE_TBL_Register(&TblHandle3, "UT_Table1", sizeof(UT_Table2_t), CFE_TBL_OPT_DBL_BUFFER, NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_DUPLICATE_DIFF_SIZE && EventsCorrect, "CFE_TBL_Register",
              "Table size mismatch");

    /* Test attempt to register a table with the same size and name */
    /* a. Test setup */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_2);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    RtnCode       = CFE_TBL_Share(&TblHandle3, "ut_cfe_tbl.UT_Table1");
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_Register",
              "Table with same size and name (setup)");

    /* b. Perform test */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1); /* Restore AppID to proper value */
    RtnCode       = CFE_TBL_Register(&TblHandle2, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_DBL_BUFFER, NULL);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_WARN_DUPLICATE && EventsCorrect && TblHandle1 == TblHandle2,
              "CFE_TBL_Register", "Table with same size and name");

    /* c. Test cleanup: unregister tables */
    UT_ClearEventHistory();
    UT_SetAppID(UT_TBL_APPID_1);
    RtnCode = CFE_TBL_Unregister(TblHandle2);
    UT_SetAppID(UT_TBL_APPID_2);
    RtnCode2      = CFE_TBL_Unregister(TblHandle3);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && RtnCode2 == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_Unregister", "Table with same size and name (cleanup)");

    /* Test registering a single buffered table */
    /* a. Perform test */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    RtnCode       = CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_DEFAULT, NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == false && UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_Register",
              "Register single buffered table");

    /* b. Test cleanup: unregister table */
    UT_ClearEventHistory();
    RtnCode       = CFE_TBL_Unregister(TblHandle1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_Unregister",
              "Register single buffered table (cleanup)");

    /* Test registering a single buffered dump-only table */
    /* a. Perform test */
    UT_InitData();
    RtnCode       = CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t),
                               (CFE_TBL_OPT_SNGL_BUFFER | CFE_TBL_OPT_DUMP_ONLY), NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == false && UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_Register",
              "Register single buffered dump-only table");

    /* b. Test cleanup: unregister table */
    UT_ClearEventHistory();
    RtnCode       = CFE_TBL_Unregister(TblHandle1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_Unregister",
              "Register single buffered dump-only table (cleanup)");

    /* Test registering a user defined address table */
    /* a. Perform test */
    UT_InitData();
    RtnCode       = CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_USR_DEF_ADDR, NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == false && UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_Register",
              "Register user defined address table");

    /* b. Test cleanup: unregister table */
    UT_ClearEventHistory();
    RtnCode       = CFE_TBL_Unregister(TblHandle1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_Unregister",
              "Register user defined address table (cleanup)");

    /* Test registering a critical table */
    /* a. Perform test */
    UT_InitData();
    RtnCode       = CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_CRITICAL, NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == false && UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_Register",
              "Register critical table");

    /* b. Test cleanup: unregister table */
    UT_ClearEventHistory();
    RtnCode       = CFE_TBL_Unregister(TblHandle1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_Unregister",
              "Register critical table (cleanup)");

    /* Test registering a critical table that already has an allocated CDS */
    /* a. Perform test */
    UT_ClearEventHistory();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RegisterCDSEx), 1, CFE_ES_CDS_ALREADY_EXISTS);
    CFE_TBL_Global.CritReg[0].TableLoadedOnce = true;
    RtnCode       = CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_CRITICAL, NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == false && UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_INFO_RECOVERED_TBL && EventsCorrect, "CFE_TBL_Register",
              "Register critical table that already has an allocated CDS");

    /* b. Test cleanup: unregister table */
    UT_ClearEventHistory();
    RtnCode       = CFE_TBL_Unregister(TblHandle1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_Unregister",
              "Register critical table that already has an allocated "
              "CDS (cleanup)");

    /* Test registering a critical table that already has an allocated CDS
     * and recovery fails
     */
    /* a. Perform test */
    UT_ClearEventHistory();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RegisterCDSEx), 1, CFE_ES_CDS_ALREADY_EXISTS);
    CFE_TBL_Global.CritReg[0].TableLoadedOnce = false;
    RtnCode       = CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_CRITICAL, NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == false && UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_Register",
              "Register critical table that already has an allocated CDS where "
              "recovery fails");

    /* b. Test cleanup: unregister table */
    UT_ClearEventHistory();
    RtnCode       = CFE_TBL_Unregister(TblHandle1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_Unregister",
              "Register critical table that already has an allocated "
              "CDS where recovery fails (cleanup)");

    /* Test registering a critical table that already has an allocated CDS but
     * fails recovery
     */
    /* a. Perform test */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RegisterCDSEx), 1, CFE_ES_CDS_ALREADY_EXISTS);
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RestoreFromCDS), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    RtnCode       = CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_CRITICAL, NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == false && UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_Register",
              "Register critical table that already has an allocated CDS but "
              "fails recovery");

    /* b. Test cleanup: unregister table */
    UT_ClearEventHistory();
    RtnCode       = CFE_TBL_Unregister(TblHandle1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_Unregister",
              "Register critical table that already has an allocated CDS but "
              "fails recovery (cleanup)");

    /* Test registering a critical table that already has an allocated CDS but
     * no critical table registry entry
     */
    /* a. Perform test */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RegisterCDSEx), 1, CFE_ES_CDS_ALREADY_EXISTS);

    /* Remove all entries from critical table registry */
    for (i = 0; i < CFE_PLATFORM_TBL_MAX_CRITICAL_TABLES; i++)
    {
        CFE_TBL_Global.CritReg[i].CDSHandle = CFE_ES_CDS_BAD_HANDLE;
    }

    RtnCode       = CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_CRITICAL, NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == false && UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_Register",
              "Register Critical table that already has an allocated CDS "
              "but no critical table registry entry");

    /* b. Test cleanup: unregister table */
    UT_ClearEventHistory();
    RtnCode       = CFE_TBL_Unregister(TblHandle1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_Unregister",
              "Register Critical table that already has an allocated CDS "
              "but no critical table registry entry (cleanup)");

    /* Test registering a critical table but no critical table registry entry
     * is free
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RegisterCDSEx), 1, CFE_SUCCESS);

    /* Remove all entries from critical table registry */
    /* a. Perform test */
    for (i = 0; i < CFE_PLATFORM_TBL_MAX_CRITICAL_TABLES; i++)
    {
        CFE_TBL_Global.CritReg[i].CDSHandle = CFE_ES_CDS_BAD_HANDLE;
    }

    RtnCode       = CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_CRITICAL, NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == false && UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_Register",
              "Register Critical table but no critical table registry entry "
              "is free");

    /* b. Test cleanup: unregister table */
    UT_ClearEventHistory();
    RtnCode       = CFE_TBL_Unregister(TblHandle1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_Unregister",
              "Register Critical table but no critical table registry entry "
              "is free (cleanup)");

    /* Test attempt to register a critical table when the CDS registry
     * is full
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RegisterCDSEx), 1, CFE_ES_NO_RESOURCE_IDS_AVAILABLE);
    RtnCode       = CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_CRITICAL, NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == false && UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_WARN_NOT_CRITICAL && EventsCorrect, "CFE_TBL_Register",
              "Register critical table when CDS registry is full");

    /* Test attempt to register a table when the registry is full */
    /* a. Test setup */
    UT_InitData();
    UT_ResetTableRegistry();
    i       = 0;
    RtnCode = CFE_SUCCESS;

    while (i < CFE_PLATFORM_TBL_MAX_NUM_TABLES && RtnCode == CFE_SUCCESS)
    {
        snprintf(TblName, CFE_MISSION_TBL_MAX_NAME_LENGTH, "UT_Table%d", i + 1);
        RtnCode = CFE_TBL_Register(&TblHandle1, TblName, sizeof(UT_Table1_t), CFE_TBL_OPT_DEFAULT, NULL);
        i++;
    }

    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS, "CFE_TBL_Register", "Registry full (setup)");

    /* b. Perform test */
    UT_ClearEventHistory();
    snprintf(TblName, CFE_MISSION_TBL_MAX_NAME_LENGTH, "UT_Table%d", i + 1);
    RtnCode       = CFE_TBL_Register(&TblHandle2, TblName, sizeof(UT_Table1_t), CFE_TBL_OPT_DEFAULT, NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_REGISTRY_FULL && EventsCorrect, "CFE_TBL_Register",
              "Registry full");

    /* c. Test cleanup: unregister table */
    UT_ClearEventHistory();
    RtnCode       = CFE_TBL_Unregister(TblHandle1);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == false && UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_Unregister",
              "Registry full (cleanup)");

    /* Test response to no available handles */
    /* a. Test setup */
    UT_InitData();

    do
    {
        RtnCode = CFE_TBL_Share(&TblHandle1, "ut_cfe_tbl.UT_Table2");
    } while ((TblHandle1 < CFE_PLATFORM_TBL_MAX_NUM_HANDLES - 1) && RtnCode == CFE_SUCCESS);

    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS, "CFE_TBL_Share", "No available handles (setup)");

    /* b. Perform test */
    UT_ClearEventHistory();
    snprintf(TblName, CFE_MISSION_TBL_MAX_NAME_LENGTH, "UT_Table%d", CFE_PLATFORM_TBL_MAX_NUM_TABLES);
    RtnCode       = CFE_TBL_Register(&TblHandle1, TblName, sizeof(UT_Table1_t), CFE_TBL_OPT_DEFAULT, NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_HANDLES_FULL && EventsCorrect, "CFE_TBL_Register",
              "No available handles");

    /* Test response to an invalid table option combination
     * (CFE_TBL_OPT_USR_DEF_ADDR | CFE_TBL_OPT_CRITICAL)
     */
    UT_InitData();
    RtnCode       = CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t),
                               CFE_TBL_OPT_USR_DEF_ADDR | CFE_TBL_OPT_CRITICAL, NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_INVALID_OPTIONS && EventsCorrect, "CFE_TBL_Register",
              "Invalid table option combination (CFE_TBL_OPT_USR_DEF_ADDR |"
              "CFE_TBL_OPT_CRITICAL)");

    /* Test response to an invalid table option combination
     * (CFE_TBL_OPT_DUMP_ONLY | CFE_TBL_OPT_CRITICAL)
     */
    UT_InitData();
    RtnCode       = CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t),
                               CFE_TBL_OPT_DUMP_ONLY | CFE_TBL_OPT_CRITICAL, NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_INVALID_OPTIONS && EventsCorrect, "CFE_TBL_Register",
              "Invalid table option combination (CFE_TBL_OPT_DUMP_ONLY |"
              "CFE_TBL_OPT_CRITICAL)");

    /* Test attempt to register a table with UsedFlag = false */
    UT_InitData();
    CFE_TBL_Global.Handles[0].UsedFlag = false;
    RtnCode       = CFE_TBL_Register(&TblHandle2, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_DBL_BUFFER, NULL);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_WARN_DUPLICATE && EventsCorrect && TblHandle1 == TblHandle2,
              "CFE_TBL_Register", "UsedFlag is false");

    /* Test attempt to register a table with an invalid registry index */
    UT_InitData();
    CFE_TBL_Global.Handles[0].UsedFlag = true;
    CFE_TBL_Global.Handles[0].RegIndex = -1;
    RtnCode       = CFE_TBL_Register(&TblHandle2, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_DBL_BUFFER, NULL);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_WARN_DUPLICATE && EventsCorrect && TblHandle1 == TblHandle2,
              "CFE_TBL_Register", "Invalid registry index");

    /* Test attempt to register a table with access index at end of list */
    UT_InitData();

    for (i = 0; i < CFE_PLATFORM_TBL_MAX_NUM_TABLES; i++)
    {
        CFE_TBL_Global.Registry[i].HeadOfAccessList = CFE_TBL_END_OF_LIST;
    }

    RtnCode       = CFE_TBL_Register(&TblHandle2, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_DBL_BUFFER, NULL);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_WARN_DUPLICATE && EventsCorrect && TblHandle1 == TblHandle2,
              "CFE_TBL_Register", "Access index at end of list");

    /* Test attempt to register a double buffered table with a pool buffer
     * error */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetPoolBuf), 1, CFE_SEVERITY_ERROR);
    snprintf(TblName, CFE_MISSION_TBL_MAX_NAME_LENGTH, "UT_Table%d", CFE_PLATFORM_TBL_MAX_NUM_TABLES);
    CFE_TBL_Global.Handles[0].UsedFlag = false;
    RtnCode       = CFE_TBL_Register(&TblHandle2, TblName, sizeof(UT_Table1_t) + 1, CFE_TBL_OPT_DBL_BUFFER, NULL);
    AccessDescPtr = &CFE_TBL_Global.Handles[TblHandle2];
    RegRecPtr     = &CFE_TBL_Global.Registry[AccessDescPtr->RegIndex];
    EventsCorrect = RegRecPtr->DoubleBuffered == false && RegRecPtr->ActiveBufferIndex == 0;
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SEVERITY_ERROR && EventsCorrect, "CFE_TBL_Register",
              "Register a double buffered table with pool buffer error");
    CFE_TBL_Global.Handles[0].UsedFlag = true;
}

/*
** Test function that obtains the handle of table registered by another
** application
*/
void Test_CFE_TBL_Share(void)
{
    int32              RtnCode;
    bool               EventsCorrect;
    CFE_FS_Header_t    StdFileHeader;
    CFE_TBL_File_Hdr_t TblFileHeader;

    UtPrintf("Begin Test Share");

    StdFileHeader.SpacecraftID = CFE_PLATFORM_TBL_VALID_SCID_1;
    StdFileHeader.ProcessorID  = CFE_PLATFORM_TBL_VALID_PRID_1;

    /* Test response to an invalid application ID */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetAppID), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    RtnCode       = CFE_TBL_Share(&App2TblHandle1, "ut_cfe_tbl.UT_Table2");
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_SHARE_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_ES_ERR_RESOURCEID_NOT_VALID && EventsCorrect, "CFE_TBL_Share",
              "Invalid application ID");

    /* Test response when table name is not in the registry */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    RtnCode       = CFE_TBL_Share(&App1TblHandle1, "ut_cfe_tbl.NOT_Table2");
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_SHARE_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_INVALID_NAME && EventsCorrect, "CFE_TBL_Share",
              "Table name not in registry");

    /* Test response when there are no available table handles */
    UT_InitData();
    RtnCode       = CFE_TBL_Share(&App1TblHandle1, "ut_cfe_tbl.UT_Table3");
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_SHARE_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_HANDLES_FULL && EventsCorrect, "CFE_TBL_Share",
              "No available table handles");

    /* Test unregistering tables to free handles */
    UT_InitData();
    RtnCode       = CFE_TBL_Unregister(CFE_PLATFORM_TBL_MAX_NUM_HANDLES / 2);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_Unregister", "Free handles");

    /* Test unregister response to a PutPoolBuf error */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_PutPoolBuf), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    RtnCode       = CFE_TBL_Unregister(CFE_PLATFORM_TBL_MAX_NUM_HANDLES / 2 + 1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_Unregister", "PutPoolBuf error");

    /* Test successful first load of a table */
    UT_InitData();
    strncpy(StdFileHeader.Description, "FS header description", sizeof(StdFileHeader.Description) - 1);
    StdFileHeader.Description[sizeof(StdFileHeader.Description) - 1] = '\0';
    StdFileHeader.ContentType                                        = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType                                            = CFE_FS_SubType_TBL_IMG;
    strncpy(TblFileHeader.TableName, "ut_cfe_tbl.UT_Table4", sizeof(TblFileHeader.TableName) - 1);
    TblFileHeader.TableName[sizeof(TblFileHeader.TableName) - 1] = '\0';

    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(UT_Table1_t));

    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, 0);
    RtnCode       = CFE_TBL_Load(3, CFE_TBL_SRC_FILE, "TblSrcFileName.dat");
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_LOAD_SUCCESS_INF_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_Load",
              "First load of a table - successful");

    /* Test successful share of a table that has not been loaded once */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_2);
    RtnCode       = CFE_TBL_Share(&App2TblHandle1, "ut_cfe_tbl.UT_Table3");
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_Share",
              "Share of table that has not been loaded - successful");

    /* Test successful share of a table that has been loaded once */
    UT_InitData();
    RtnCode       = CFE_TBL_Share(&App2TblHandle2, "ut_cfe_tbl.UT_Table4");
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_Share",
              "Share of table that has been loaded - successful");
}

/*
** Function to test unregistering a previously registered table and freeing
** associated resources
*/
void Test_CFE_TBL_Unregister(void)
{
    int32 RtnCode;
    bool  EventsCorrect;

    UtPrintf("Begin Test Unregister");

    /* Test response to unregistering a table with an invalid handle */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    RtnCode       = CFE_TBL_Unregister(CFE_PLATFORM_TBL_MAX_NUM_HANDLES);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_UNREGISTER_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, (RtnCode == CFE_TBL_ERR_INVALID_HANDLE && EventsCorrect), "CFE_TBL_Unregister",
              "Invalid handle");

    /* Unregister a shared table to make it unowned */
    UT_InitData();
    RtnCode       = CFE_TBL_Unregister(3);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_Unregister",
              "Unregister shared table to make it unowned");

    /* Test response to unregistering an unowned table */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_2);
    RtnCode       = CFE_TBL_Unregister(App2TblHandle2);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_UNREGISTER_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_NO_ACCESS && EventsCorrect, "CFE_TBL_Unregister",
              "Unregister unowned table");
}

/*
** Test function that instructs table services to notify calling application
** whenever the specified table requires management
*/
void Test_CFE_TBL_NotifyByMessage(void)
{
    int32 RtnCode;
    bool  EventsCorrect;

    UtPrintf("Begin Test Notify by Message");

    /* Set up notify by message tests */
    UT_InitData();
    Test_CFE_TBL_ApiInit();
    UT_SetAppID(UT_TBL_APPID_1);
    UT_ResetPoolBufferIndex();

    RtnCode       = CFE_TBL_Register(&App1TblHandle1, "NBMsg_Tbl", sizeof(UT_Table1_t), CFE_TBL_OPT_CRITICAL, NULL);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_Register",
              "Notify by message (setup)");

    /* Test successful notification */
    UT_InitData();
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    RtnCode       = CFE_TBL_NotifyByMessage(App1TblHandle1, CFE_SB_ValueToMsgId(1), 1, 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_NotifyByMessage",
              "Notify - success");

    /* Test response to notification request when the application doesn't
     * own the table handle
     */
    UT_InitData();
    CFE_TBL_Global.Registry[0].OwnerAppId = CFE_TBL_NOT_OWNED;
    EventsCorrect                         = (UT_GetNumEventsSent() == 0);
    RtnCode                               = CFE_TBL_NotifyByMessage(App1TblHandle1, CFE_SB_ValueToMsgId(1), 1, 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_NO_ACCESS && EventsCorrect, "CFE_TBL_NotifyByMessage",
              "Notify - no access");

    /* Test response to notification request when the application ID is bad */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetAppID), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    RtnCode       = CFE_TBL_NotifyByMessage(App1TblHandle1, CFE_SB_ValueToMsgId(1), 1, 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_ES_ERR_RESOURCEID_NOT_VALID && EventsCorrect,
              "CFE_TBL_NotifyByMessage", "Notify - bad application ID");
}

/*
** Function to test loading a specified table with data from the
** specified source
*/
void Test_CFE_TBL_Load(void)
{
    CFE_TBL_Handle_t            DumpOnlyTblHandle;
    CFE_TBL_Handle_t            UserDefTblHandle;
    UT_Table1_t                 TestTable1;
    int32                       RtnCode;
    bool                        EventsCorrect;
    CFE_FS_Header_t             StdFileHeader;
    CFE_TBL_File_Hdr_t          TblFileHeader;
    UT_Table1_t *               App2TblPtr;
    CFE_TBL_RegistryRec_t *     RegRecPtr;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr;

    UtPrintf("Begin Test Load");

    StdFileHeader.SpacecraftID = CFE_PLATFORM_TBL_VALID_SCID_1;
    StdFileHeader.ProcessorID  = CFE_PLATFORM_TBL_VALID_PRID_1;

    /* Set up for table load test */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    UT_ResetTableRegistry();
    RtnCode       = CFE_TBL_Register(&App1TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_DEFAULT,
                               Test_CFE_TBL_ValidationFunc);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == false && UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_Register",
              "Load setup - single buffered table");

    /* Test attempt to perform partial INITIAL load */
    UT_InitData();
    strncpy(StdFileHeader.Description, "Test description", sizeof(StdFileHeader.Description) - 1);
    StdFileHeader.Description[sizeof(StdFileHeader.Description) - 1] = '\0';
    StdFileHeader.ContentType                                        = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType                                            = CFE_FS_SubType_TBL_IMG;
    strncpy(TblFileHeader.TableName, "ut_cfe_tbl.UT_Table1", sizeof(TblFileHeader.TableName) - 1);
    TblFileHeader.TableName[sizeof(TblFileHeader.TableName) - 1] = '\0';
    UT_TBL_SetupHeader(&TblFileHeader, 1, sizeof(UT_Table1_t) - 1);

    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, 0);
    RtnCode       = CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_FILE, "TblSrcFileName.dat");
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_PARTIAL_LOAD_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_PARTIAL_LOAD && EventsCorrect, "CFE_TBL_Load",
              "Attempt to perform partial INITIAL load");

    /* Test attempt to perform partial INITIAL load with table already
     * loaded
     */
    UT_InitData();
    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, 0);
    AccessDescPtr              = &CFE_TBL_Global.Handles[App1TblHandle1];
    RegRecPtr                  = &CFE_TBL_Global.Registry[AccessDescPtr->RegIndex];
    RegRecPtr->UserDefAddr     = true;
    RegRecPtr->TableLoadedOnce = true;
    RtnCode                    = CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_FILE, "TblSrcFileName.dat");
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_PARTIAL_LOAD_ERR_EID) == false && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_Load",
              "Attempt to perform partial INITIAL load with table already loaded");

    /* Test attempt to load a file that has incompatible data for the
     * specified table
     */
    UT_InitData();
    strncpy(StdFileHeader.Description, "Test description", sizeof(StdFileHeader.Description) - 1);
    StdFileHeader.Description[sizeof(StdFileHeader.Description) - 1] = '\0';
    StdFileHeader.ContentType                                        = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType                                            = CFE_FS_SubType_TBL_IMG;
    strncpy(TblFileHeader.TableName, "ut_cfe_tbl.NotUT_Table1", sizeof(TblFileHeader.TableName) - 1);
    TblFileHeader.TableName[sizeof(TblFileHeader.TableName) - 1] = '\0';
    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(UT_Table1_t));

    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, 0);
    RtnCode       = CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_FILE, "TblSrcFileName.dat");
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_LOAD_TBLNAME_MISMATCH_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_FILE_FOR_WRONG_TABLE && EventsCorrect, "CFE_TBL_Load",
              "File data incompatible with table");

    /* Set up for double buffer table load test */
    /* Test setup - register a double buffered table */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    RtnCode       = CFE_TBL_Register(&App1TblHandle2, "UT_Table2x", sizeof(UT_Table1_t), CFE_TBL_OPT_DBL_BUFFER,
                               Test_CFE_TBL_ValidationFunc);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == false && UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_Register",
              "CFE_TBL_Load setup - register a double buffered table");

    /* Test attempt to load a file that has incompatible data for the
     * specified double buffered table that is already loaded
     */
    UT_InitData();
    strncpy(StdFileHeader.Description, "Test description", sizeof(StdFileHeader.Description) - 1);
    StdFileHeader.Description[sizeof(StdFileHeader.Description) - 1] = '\0';
    StdFileHeader.ContentType                                        = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType                                            = CFE_FS_SubType_TBL_IMG;
    strncpy(TblFileHeader.TableName, "ut_cfe_tbl.NotUT_Table1", sizeof(TblFileHeader.TableName) - 1);
    TblFileHeader.TableName[sizeof(TblFileHeader.TableName) - 1] = '\0';
    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(UT_Table1_t));

    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, 0);
    AccessDescPtr              = &CFE_TBL_Global.Handles[App1TblHandle2];
    RegRecPtr                  = &CFE_TBL_Global.Registry[AccessDescPtr->RegIndex];
    RegRecPtr->DoubleBuffered  = true;
    RegRecPtr->TableLoadedOnce = true;
    RtnCode                    = CFE_TBL_Load(App1TblHandle2, CFE_TBL_SRC_FILE, "TblSrcFileName.dat");
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_LOAD_TBLNAME_MISMATCH_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_FILE_FOR_WRONG_TABLE && EventsCorrect, "CFE_TBL_Load",
              "File data incompatible with table, double buffered, already "
              "loaded");

    /* Test performing a Load from memory */
    UT_InitData();
    RtnCode       = CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_ADDRESS, &TestTable1);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_LOAD_SUCCESS_INF_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_Load", "Perform load from memory");

    /* Test attempt to load from an illegal source type (not a file or
     * from memory)
     */
    UT_InitData();
    RtnCode       = CFE_TBL_Load(App1TblHandle1, (CFE_TBL_SrcEnum_t)99, &TestTable1);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_LOAD_TYPE_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_ILLEGAL_SRC_TYPE && EventsCorrect, "CFE_TBL_Load",
              "Attempt to load from illegal source type");

    /* Test failure of validation function on table load using a negative
     * return code
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(Test_CFE_TBL_ValidationFunc), 1, -1);
    RtnCode       = CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_ADDRESS, &TestTable1);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_VALIDATION_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == -1 && EventsCorrect, "CFE_TBL_Load",
              "Fail validation function on table load (negative return code)");

    /* Test failure of validation function on table load using a positive
     * return code
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(Test_CFE_TBL_ValidationFunc), 1, 1);
    RtnCode       = CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_ADDRESS, &TestTable1);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_LOAD_VAL_ERR_EID) == true && UT_GetNumEventsSent() == 2);
    UT_Report(__FILE__, __LINE__, RtnCode == -1 && EventsCorrect, "CFE_TBL_Load",
              "Fail validation function on table load (positive return code)");

    /* Test attempt to load a table with a bad handle */
    UT_InitData();
    RtnCode       = CFE_TBL_Load(CFE_PLATFORM_TBL_MAX_NUM_HANDLES, CFE_TBL_SRC_ADDRESS, &TestTable1);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_HANDLE_ACCESS_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_INVALID_HANDLE && EventsCorrect, "CFE_TBL_Load",
              "Attempt to load table with bad handle");

    /* Test attempt to load a dump-only table */
    /* a. Test setup */
    UT_InitData();
    RtnCode       = CFE_TBL_Register(&DumpOnlyTblHandle, "UT_Table2", sizeof(UT_Table1_t), CFE_TBL_OPT_DUMP_ONLY, NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == false && UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_Register",
              "Attempt to load a dump-only table (setup)");

    /* b. Perform test */
    UT_InitData();
    RtnCode       = CFE_TBL_Load(DumpOnlyTblHandle, CFE_TBL_SRC_ADDRESS, &TestTable1);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_LOADING_A_DUMP_ONLY_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_DUMP_ONLY && EventsCorrect, "CFE_TBL_Load",
              "Attempt to load a dump-only table");

    /* Test attempt to load a dump-only table with the table already loaded */
    UT_InitData();
    AccessDescPtr              = &CFE_TBL_Global.Handles[DumpOnlyTblHandle];
    RegRecPtr                  = &CFE_TBL_Global.Registry[AccessDescPtr->RegIndex];
    RegRecPtr->UserDefAddr     = true;
    RegRecPtr->TableLoadedOnce = true;
    RtnCode                    = CFE_TBL_Load(DumpOnlyTblHandle, CFE_TBL_SRC_ADDRESS, &TestTable1);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_LOADING_A_DUMP_ONLY_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_DUMP_ONLY && EventsCorrect, "CFE_TBL_Load",
              "Attempt to load a dump-only table with table already loaded");

    /* Test specifying a table address for a user defined table */
    /* a. Test setup */
    UT_InitData();
    RtnCode = CFE_TBL_Register(&UserDefTblHandle, "UT_Table3", sizeof(UT_Table1_t), CFE_TBL_OPT_USR_DEF_ADDR, NULL);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_Register",
              "Specify table address for a user defined table");

    /* Perform test */
    UT_InitData();
    RtnCode       = CFE_TBL_Load(UserDefTblHandle, CFE_TBL_SRC_ADDRESS, &TestTable1);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_LOAD_SUCCESS_INF_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_Load",
              "Specify table address for a user defined table");

    /* Test attempt to load a locked shared table */
    /* a. Test setup part 1 */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_2);
    RtnCode       = CFE_TBL_Share(&App2TblHandle1, "ut_cfe_tbl.UT_Table1");
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_Share",
              "Attempt to load locked shared table (setup part 1)");

    /* a. Test setup part 2 */
    RtnCode       = CFE_TBL_GetAddress((void **)&App2TblPtr, App2TblHandle1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_INFO_UPDATED && EventsCorrect, "CFE_TBL_GetAddress",
              "Attempt to load locked shared table (setup part 2)");

    /* c. Perform test */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    RtnCode       = CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_ADDRESS, &TestTable1);
    EventsCorrect = (UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_INFO_TABLE_LOCKED && EventsCorrect, "CFE_TBL_Load",
              "Attempt to load locked shared table");

    /* d. Test cleanup */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_2);
    RtnCode       = CFE_TBL_ReleaseAddress(App2TblHandle1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_ReleaseAddress",
              "Attempt to load locked shared table (cleanup)");
}

/*
** Function to test obtaining the current address of the contents
** of the specified table
*/
void Test_CFE_TBL_GetAddress(void)
{
    int32        RtnCode;
    bool         EventsCorrect;
    UT_Table1_t *App3TblPtr;
    UT_Table1_t *App2TblPtr;

    UtPrintf("Begin Test Get Address");

    /* Test attempt to get the address of a table for which the application
     * does not have access
     */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_3);
    RtnCode       = CFE_TBL_GetAddress((void **)&App3TblPtr, App2TblHandle1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_NO_ACCESS && EventsCorrect, "CFE_TBL_GetAddress",
              "Application does not have access to table");

    /* Test attempt to get the address with an invalid application ID */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetAppID), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    RtnCode       = CFE_TBL_GetAddress((void **)&App3TblPtr, App2TblHandle1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_ES_ERR_RESOURCEID_NOT_VALID && EventsCorrect, "CFE_TBL_GetAddress",
              "Invalid application ID");

    /* Test attempt to get the address with an invalid handle */
    UT_InitData();
    RtnCode       = CFE_TBL_GetAddress((void **)&App3TblPtr, CFE_PLATFORM_TBL_MAX_NUM_HANDLES);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_INVALID_HANDLE && EventsCorrect, "CFE_TBL_GetAddress",
              "Invalid table handle");

    /* Attempt to get the address of an unregistered (unowned) table */
    /* a. Test setup */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    RtnCode       = CFE_TBL_Unregister(App1TblHandle1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_Unregister",
              "Attempt to get address of unregistered table (setup)");

    /* b. Perform test */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_2);
    RtnCode       = CFE_TBL_GetAddress((void **)&App2TblPtr, App2TblHandle1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_UNREGISTERED && EventsCorrect, "CFE_TBL_GetAddress",
              "Attempt to get address of unregistered table");
}

/*
** Function to test release of a previously obtained pointer to the
** contents of the specified table
*/
void Test_CFE_TBL_ReleaseAddress(void)
{
    int32 RtnCode;
    bool  EventsCorrect;

    UtPrintf("Begin Test Release Address");

    /* Test address release using an invalid application ID */
    /* a. Test setup - register single buffered table */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    UT_ResetTableRegistry();
    RtnCode       = CFE_TBL_Register(&App1TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_DEFAULT,
                               Test_CFE_TBL_ValidationFunc);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == false && UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_Register",
              "Attempt to release address with invalid application ID (setup)");

    /* b. Perform test */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetAppID), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    RtnCode       = CFE_TBL_ReleaseAddress(App1TblHandle1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_ES_ERR_RESOURCEID_NOT_VALID && EventsCorrect, "CFE_TBL_GetAddress",
              "Attempt to release address with invalid application ID");
}

/*
** Test function that obtains the current addresses of the contents of a
** collection of tables
*/
void Test_CFE_TBL_GetAddresses(void)
{
    int32 RtnCode;
    bool  EventsCorrect;

    UtPrintf("Begin Test Get Addresses");

    /* Test setup - register a double buffered table */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    RtnCode       = CFE_TBL_Register(&App1TblHandle2, "UT_Table2", sizeof(UT_Table1_t), CFE_TBL_OPT_DBL_BUFFER,
                               Test_CFE_TBL_ValidationFunc);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == false && UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_Register",
              "CFE_TBL_GetAddresses setup - register a double buffered table");

    /* b. Perform test */
    UT_InitData();

    /* Test getting the addresses of two tables that have not been loaded */
    ArrayOfHandles[0]       = App1TblHandle1;
    ArrayOfHandles[1]       = App1TblHandle2;
    ArrayOfPtrsToTblPtrs[0] = &Tbl1Ptr;
    ArrayOfPtrsToTblPtrs[1] = &Tbl2Ptr;

    RtnCode       = CFE_TBL_GetAddresses(ArrayOfPtrsToTblPtrs, 2, ArrayOfHandles);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__,
              RtnCode == CFE_TBL_ERR_NEVER_LOADED && EventsCorrect && Tbl1Ptr != NULL && Tbl2Ptr != NULL,
              "CFE_TBL_GetAddresses",
              "Get addresses of two tables (neither of which have "
              "been loaded)");

    /* Test attempt to get addresses of tables that the application is not
     * allowed to see
     */
    UT_InitData();
    UT_SetAppID(CFE_ES_APPID_UNDEFINED);
    RtnCode       = CFE_TBL_GetAddresses(ArrayOfPtrsToTblPtrs, 2, ArrayOfHandles);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_NO_ACCESS && EventsCorrect, "CFE_TBL_Validate",
              "Attempt to get addresses of tables that application is not "
              "allowed to see");
}

/*
** Test function that releases previously obtained pointers to the contents
** of the specified tables
*/
void Test_CFE_TBL_ReleaseAddresses(void)
{
    int32 RtnCode;
    bool  EventsCorrect;

    UtPrintf("Begin Test Release Addresses");

    /* Test response to releasing two tables that have not been loaded */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    RtnCode       = CFE_TBL_ReleaseAddresses(2, ArrayOfHandles);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_NEVER_LOADED && EventsCorrect, "CFE_TBL_ReleaseAddresses",
              "Release addresses of two tables (neither of which have "
              "been loaded)");
}

/*
** Test function for validating the table image for a specified table
*/
void Test_CFE_TBL_Validate(void)
{
    int32 RtnCode;
    bool  EventsCorrect;

    UtPrintf("Begin Test Validate");

    /* Test response to attempt to validate a table that an application is
     * not allowed to see
     */
    UT_InitData();
    UT_SetAppID(CFE_ES_APPID_UNDEFINED);
    RtnCode       = CFE_TBL_Validate(App1TblHandle1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_NO_ACCESS && EventsCorrect, "CFE_TBL_Validate",
              "Attempt to validate table that application is not allowed "
              "to see");

    /* Test response to attempt to validate a table when no validation is
     * pending
     */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    RtnCode       = CFE_TBL_Validate(App1TblHandle1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_INFO_NO_VALIDATION_PENDING && EventsCorrect, "CFE_TBL_Validate",
              "Attempt to validate table when no validation is pending");
}

/*
** Test function for performing standard table maintenance on the
** specified table
*/
void Test_CFE_TBL_Manage(void)
{
    int32                       RtnCode;
    bool                        EventsCorrect;
    int16                       RegIndex;
    CFE_TBL_RegistryRec_t *     RegRecPtr;
    CFE_TBL_LoadBuff_t *        WorkingBufferPtr;
    UT_Table1_t                 TestTable1;
    UT_Table1_t *               App2TblPtr;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr;
    CFE_TBL_Handle_t            AccessIterator;

    UtPrintf("Begin Test Manage");

    /* Test response to attempt to manage a table that doesn't need managing */
    UT_InitData();
    RtnCode       = CFE_TBL_Manage(App1TblHandle1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_Manage",
              "Manage table that doesn't need managing");

    /* Test response to attempt to load while a load is in progress */
    UT_InitData();

    /* "Load" image into inactive buffer for table */
    RegIndex  = CFE_TBL_FindTableInRegistry("ut_cfe_tbl.UT_Table1");
    RegRecPtr = &CFE_TBL_Global.Registry[RegIndex];
    RtnCode   = CFE_TBL_GetWorkingBuffer(&WorkingBufferPtr, RegRecPtr, false);
    UT_SetAppID(UT_TBL_APPID_1);
    RtnCode       = CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_ADDRESS, &TestTable1);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_LOAD_IN_PROGRESS_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_LOAD_IN_PROGRESS && EventsCorrect, "CFE_TBL_Load",
              "Attempt to load while a load is in progress");

    /* Test response to processing an unsuccessful validation request on
     * inactive buffer; validation function return code is valid
     */
    UT_InitData();

    /* Configure table for validation */
    CFE_TBL_Global.ValidationResults[0].State  = CFE_TBL_VALIDATION_PENDING;
    CFE_TBL_Global.ValidationResults[0].Result = 0;
    strncpy(CFE_TBL_Global.ValidationResults[0].TableName, "ut_cfe_tbl.UT_Table1",
            sizeof(CFE_TBL_Global.ValidationResults[0].TableName) - 1);
    CFE_TBL_Global.ValidationResults[0].TableName[sizeof(CFE_TBL_Global.ValidationResults[0].TableName) - 1] = '\0';
    CFE_TBL_Global.ValidationResults[0].CrcOfTable                                                           = 0;
    CFE_TBL_Global.ValidationResults[0].ActiveBuffer                                                         = false;
    RegRecPtr->ValidateInactiveIndex                                                                         = 0;

    /* Perform validation via manage call */
    UT_SetDeferredRetcode(UT_KEY(Test_CFE_TBL_ValidationFunc), 1, -1);
    RtnCode       = CFE_TBL_Manage(App1TblHandle1);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_VALIDATION_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__,
              RtnCode == CFE_SUCCESS && EventsCorrect && CFE_TBL_Global.ValidationResults[0].Result == -1,
              "CFE_TBL_Manage",
              "Manage table that has a failed validation pending on "
              "inactive buffer (valid function return code)");

    /* Test response to processing an unsuccessful validation request on
     * inactive buffer ; validation function return code is invalid
     */
    UT_InitData();

    /* Configure table for validation */
    CFE_TBL_Global.ValidationResults[0].State  = CFE_TBL_VALIDATION_PENDING;
    CFE_TBL_Global.ValidationResults[0].Result = 0;
    strncpy(CFE_TBL_Global.ValidationResults[0].TableName, "ut_cfe_tbl.UT_Table1",
            sizeof(CFE_TBL_Global.ValidationResults[0].TableName) - 1);
    CFE_TBL_Global.ValidationResults[0].TableName[sizeof(CFE_TBL_Global.ValidationResults[0].TableName) - 1] = '\0';
    CFE_TBL_Global.ValidationResults[0].CrcOfTable                                                           = 0;
    CFE_TBL_Global.ValidationResults[0].ActiveBuffer                                                         = false;
    RegRecPtr->ValidateInactiveIndex                                                                         = 0;

    /* Perform validation via manage call */
    UT_SetDeferredRetcode(UT_KEY(Test_CFE_TBL_ValidationFunc), 1, 1);
    RtnCode       = CFE_TBL_Manage(App1TblHandle1);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_VALIDATION_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__,
              RtnCode == CFE_SUCCESS && EventsCorrect && CFE_TBL_Global.ValidationResults[0].Result == 1,
              "CFE_TBL_Manage",
              "Manage table that has a failed validation pending on "
              "inactive buffer (invalid function return code)");

    /* Test response to processing a successful validation request on an
     * inactive buffer
     */
    UT_InitData();

    /* Configure table for validation */
    CFE_TBL_Global.ValidationResults[0].State  = CFE_TBL_VALIDATION_PENDING;
    CFE_TBL_Global.ValidationResults[0].Result = 1;
    strncpy(CFE_TBL_Global.ValidationResults[0].TableName, "ut_cfe_tbl.UT_Table1",
            sizeof(CFE_TBL_Global.ValidationResults[0].TableName) - 1);
    CFE_TBL_Global.ValidationResults[0].TableName[sizeof(CFE_TBL_Global.ValidationResults[0].TableName) - 1] = '\0';
    CFE_TBL_Global.ValidationResults[0].CrcOfTable                                                           = 0;
    CFE_TBL_Global.ValidationResults[0].ActiveBuffer                                                         = false;
    RegRecPtr->ValidateInactiveIndex                                                                         = 0;

    /* Perform validation via manage call */
    UT_SetDeferredRetcode(UT_KEY(Test_CFE_TBL_ValidationFunc), 1, CFE_SUCCESS);
    RtnCode       = CFE_TBL_Manage(App1TblHandle1);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_VALIDATION_INF_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__,
              RtnCode == CFE_SUCCESS && EventsCorrect && CFE_TBL_Global.ValidationResults[0].Result == 0,
              "CFE_TBL_Manage",
              "Manage table that has a successful validation pending on "
              "an inactive buffer");

    /* Test response to processing an unsuccessful validation request on an
     * active buffer
     */
    UT_InitData();

    /* Configure table for validation */
    CFE_TBL_Global.ValidationResults[0].State  = CFE_TBL_VALIDATION_PENDING;
    CFE_TBL_Global.ValidationResults[0].Result = 0;
    strncpy(CFE_TBL_Global.ValidationResults[0].TableName, "ut_cfe_tbl.UT_Table1",
            sizeof(CFE_TBL_Global.ValidationResults[0].TableName) - 1);
    CFE_TBL_Global.ValidationResults[0].TableName[sizeof(CFE_TBL_Global.ValidationResults[0].TableName) - 1] = '\0';
    CFE_TBL_Global.ValidationResults[0].CrcOfTable                                                           = 0;
    CFE_TBL_Global.ValidationResults[0].ActiveBuffer                                                         = true;
    RegRecPtr->ValidateActiveIndex                                                                           = 0;

    /* Perform validation via manage call */
    UT_SetDeferredRetcode(UT_KEY(Test_CFE_TBL_ValidationFunc), 1, -1);
    RtnCode       = CFE_TBL_Manage(App1TblHandle1);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_VALIDATION_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__,
              RtnCode == CFE_SUCCESS && EventsCorrect && CFE_TBL_Global.ValidationResults[0].Result == -1,
              "CFE_TBL_Manage",
              "Manage table that has an unsuccessful validation pending on "
              "an active buffer");

    /* Test response to processing an unsuccessful validation request on
     * an active buffer
     */
    UT_InitData();

    /* Configure table for validation */
    CFE_TBL_Global.ValidationResults[0].State  = CFE_TBL_VALIDATION_PENDING;
    CFE_TBL_Global.ValidationResults[0].Result = 0;
    strncpy(CFE_TBL_Global.ValidationResults[0].TableName, "ut_cfe_tbl.UT_Table1",
            sizeof(CFE_TBL_Global.ValidationResults[0].TableName) - 1);
    CFE_TBL_Global.ValidationResults[0].TableName[sizeof(CFE_TBL_Global.ValidationResults[0].TableName) - 1] = '\0';
    CFE_TBL_Global.ValidationResults[0].CrcOfTable                                                           = 0;
    CFE_TBL_Global.ValidationResults[0].ActiveBuffer                                                         = true;
    RegRecPtr->ValidateActiveIndex                                                                           = 0;

    /* Perform validation via manage call */
    UT_SetDeferredRetcode(UT_KEY(Test_CFE_TBL_ValidationFunc), 1, 1);
    RtnCode       = CFE_TBL_Manage(App1TblHandle1);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_VALIDATION_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__,
              RtnCode == CFE_SUCCESS && EventsCorrect && CFE_TBL_Global.ValidationResults[0].Result == 1,
              "CFE_TBL_Manage",
              "Manage table that has an unsuccessful validation pending "
              "on an active buffer");

    /* Test response to processing a successful validation request on an
     * active buffer
     */
    UT_InitData();

    /* Configure table for validation */
    CFE_TBL_Global.ValidationResults[0].State  = CFE_TBL_VALIDATION_PENDING;
    CFE_TBL_Global.ValidationResults[0].Result = 1;
    strncpy(CFE_TBL_Global.ValidationResults[0].TableName, "ut_cfe_tbl.UT_Table1",
            sizeof(CFE_TBL_Global.ValidationResults[0].TableName) - 1);
    CFE_TBL_Global.ValidationResults[0].TableName[sizeof(CFE_TBL_Global.ValidationResults[0].TableName) - 1] = '\0';
    CFE_TBL_Global.ValidationResults[0].CrcOfTable                                                           = 0;
    CFE_TBL_Global.ValidationResults[0].ActiveBuffer                                                         = true;
    RegRecPtr->ValidateActiveIndex                                                                           = 0;

    /* Perform validation via manage call */
    UT_SetDeferredRetcode(UT_KEY(Test_CFE_TBL_ValidationFunc), 1, CFE_SUCCESS);
    RtnCode       = CFE_TBL_Manage(App1TblHandle1);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_VALIDATION_INF_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__,
              RtnCode == CFE_SUCCESS && EventsCorrect && CFE_TBL_Global.ValidationResults[0].Result == 0,
              "CFE_TBL_Manage",
              "Manage table that has a successful validation pending on "
              "an active buffer");

    /* Test response to processing an update request on a locked table */
    /* a. Test setup - part 1 */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_2);
    RtnCode       = CFE_TBL_Share(&App2TblHandle1, "ut_cfe_tbl.UT_Table1");
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_Share",
              "Process an update request on a locked table (setup - part 1)");

    /* a. Test setup - part 2 */
    RtnCode       = CFE_TBL_GetAddress((void **)&App2TblPtr, App2TblHandle1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_NEVER_LOADED && EventsCorrect, "CFE_TBL_GetAddress",
              "Process an update request on a locked table (setup - part 2)");

    /* c. Perform test */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);

    /* Configure table for update */
    RegRecPtr->LoadPending = true;
    RtnCode                = CFE_TBL_Manage(App1TblHandle1);
    EventsCorrect          = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_INFO_TABLE_LOCKED && EventsCorrect, "CFE_TBL_Manage",
              "Process an update request on a locked table");

    /* Save the previous table's information for a subsequent test */
    AccessDescPtr  = &CFE_TBL_Global.Handles[App1TblHandle1];
    RegRecPtr      = &CFE_TBL_Global.Registry[AccessDescPtr->RegIndex];
    AccessIterator = RegRecPtr->HeadOfAccessList;

    /* Test unlocking a table by releasing the address */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_2);
    RtnCode       = CFE_TBL_ReleaseAddress(App2TblHandle1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_NEVER_LOADED && EventsCorrect, "CFE_TBL_ReleaseAddress",
              "Release address to unlock shared table");

    /* Test response to processing an update request on a single
     * buffered table
     */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);

    /* Configure table for Update */
    RegRecPtr->LoadPending = true;
    RtnCode                = CFE_TBL_Manage(App1TblHandle1);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_UPDATE_SUCCESS_INF_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_INFO_UPDATED && EventsCorrect, "CFE_TBL_Manage",
              "Process an update request on a single buffered table");

    /* Test response to processing an unsuccessful validation request on an
     * inactive buffer (double buffered)
     */
    UT_InitData();

    /* Reset the current table entry pointer to a previous table in order to
     * exercise the path where no buffer is available
     */
    AccessDescPtr                                      = &CFE_TBL_Global.Handles[App1TblHandle2];
    RegRecPtr                                          = &CFE_TBL_Global.Registry[AccessDescPtr->RegIndex];
    CFE_TBL_Global.Handles[AccessIterator].NextLink    = RegRecPtr->HeadOfAccessList;
    CFE_TBL_Global.Handles[AccessIterator].AppId       = UT_TBL_APPID_2;
    RegRecPtr->HeadOfAccessList                        = AccessIterator;
    CFE_TBL_Global.Handles[AccessIterator].BufferIndex = 1;
    CFE_TBL_Global.Handles[AccessIterator].LockFlag    = true;

    /* Attempt to "load" image into inactive buffer for table */
    RegIndex  = CFE_TBL_FindTableInRegistry("ut_cfe_tbl.UT_Table2");
    RegRecPtr = &CFE_TBL_Global.Registry[RegIndex];
    RtnCode   = CFE_TBL_GetWorkingBuffer(&WorkingBufferPtr, RegRecPtr, false);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_NO_BUFFER_AVAIL, "CFE_TBL_GetWorkingBuffer",
              "No buffer available");

    /* Reset the table information for subsequent tests */
    CFE_TBL_Global.Handles[AccessIterator].BufferIndex = 1;
    CFE_TBL_Global.Handles[AccessIterator].LockFlag    = false;

    /* Successfully "load" image into inactive buffer for table */
    RtnCode = CFE_TBL_GetWorkingBuffer(&WorkingBufferPtr, RegRecPtr, false);

    /* Configure table for validation */
    CFE_TBL_Global.ValidationResults[0].State  = CFE_TBL_VALIDATION_PENDING;
    CFE_TBL_Global.ValidationResults[0].Result = 0;
    strncpy(CFE_TBL_Global.ValidationResults[0].TableName, "ut_cfe_tbl.UT_Table2",
            sizeof(CFE_TBL_Global.ValidationResults[0].TableName) - 1);
    CFE_TBL_Global.ValidationResults[0].TableName[sizeof(CFE_TBL_Global.ValidationResults[0].TableName) - 1] = '\0';
    CFE_TBL_Global.ValidationResults[0].CrcOfTable                                                           = 0;
    CFE_TBL_Global.ValidationResults[0].ActiveBuffer                                                         = false;
    RegRecPtr->ValidateInactiveIndex                                                                         = 0;

    /* Perform validation via manage call */
    UT_SetDeferredRetcode(UT_KEY(Test_CFE_TBL_ValidationFunc), 1, -1);
    RtnCode       = CFE_TBL_Manage(App1TblHandle2);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_VALIDATION_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__,
              RtnCode == CFE_SUCCESS && EventsCorrect && CFE_TBL_Global.ValidationResults[0].Result == -1,
              "CFE_TBL_Manage",
              "Manage table that has a failed validation pending on an "
              "inactive buffer (double buffered)");

    /* Test successfully processing a validation request on an inactive buffer
     * (double buffered)
     */
    UT_InitData();

    /* Configure table for validation */
    CFE_TBL_Global.ValidationResults[0].State  = CFE_TBL_VALIDATION_PENDING;
    CFE_TBL_Global.ValidationResults[0].Result = 1;
    strncpy(CFE_TBL_Global.ValidationResults[0].TableName, "ut_cfe_tbl.UT_Table2",
            sizeof(CFE_TBL_Global.ValidationResults[0].TableName) - 1);
    CFE_TBL_Global.ValidationResults[0].TableName[sizeof(CFE_TBL_Global.ValidationResults[0].TableName) - 1] = '\0';
    CFE_TBL_Global.ValidationResults[0].CrcOfTable                                                           = 0;
    CFE_TBL_Global.ValidationResults[0].ActiveBuffer                                                         = false;
    RegRecPtr->ValidateInactiveIndex                                                                         = 0;

    /* Perform validation via manage call */
    UT_SetDeferredRetcode(UT_KEY(Test_CFE_TBL_ValidationFunc), 1, CFE_SUCCESS);
    RtnCode       = CFE_TBL_Manage(App1TblHandle2);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_VALIDATION_INF_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__,
              RtnCode == CFE_SUCCESS && EventsCorrect && CFE_TBL_Global.ValidationResults[0].Result == 0,
              "CFE_TBL_Manage",
              "Manage table that has a successful validation pending on an "
              "inactive buffer (double buffered)");

    /* Test processing an unsuccessful validation request on an active buffer
     * (double buffered)
     */
    UT_InitData();

    /* Configure table for validation */
    CFE_TBL_Global.ValidationResults[0].State  = CFE_TBL_VALIDATION_PENDING;
    CFE_TBL_Global.ValidationResults[0].Result = 0;
    strncpy(CFE_TBL_Global.ValidationResults[0].TableName, "ut_cfe_tbl.UT_Table2",
            sizeof(CFE_TBL_Global.ValidationResults[0].TableName) - 1);
    CFE_TBL_Global.ValidationResults[0].TableName[sizeof(CFE_TBL_Global.ValidationResults[0].TableName) - 1] = '\0';
    CFE_TBL_Global.ValidationResults[0].CrcOfTable                                                           = 0;
    CFE_TBL_Global.ValidationResults[0].ActiveBuffer                                                         = true;
    RegRecPtr->ValidateActiveIndex                                                                           = 0;

    /* Perform validation via manage call */
    UT_SetDeferredRetcode(UT_KEY(Test_CFE_TBL_ValidationFunc), 1, -1);
    RtnCode       = CFE_TBL_Manage(App1TblHandle2);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_VALIDATION_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__,
              RtnCode == CFE_SUCCESS && EventsCorrect && CFE_TBL_Global.ValidationResults[0].Result == -1,
              "CFE_TBL_Manage",
              "Manage table that has an unsuccessful validation pending on an "
              "active buffer (double buffered)");

    /* Test successfully processing a validation request on active buffer
     * (double buffered)
     */
    UT_InitData();

    /* Configure table for validation */
    CFE_TBL_Global.ValidationResults[0].State  = CFE_TBL_VALIDATION_PENDING;
    CFE_TBL_Global.ValidationResults[0].Result = 1;
    strncpy(CFE_TBL_Global.ValidationResults[0].TableName, "ut_cfe_tbl.UT_Table2",
            sizeof(CFE_TBL_Global.ValidationResults[0].TableName) - 1);
    CFE_TBL_Global.ValidationResults[0].TableName[sizeof(CFE_TBL_Global.ValidationResults[0].TableName) - 1] = '\0';
    CFE_TBL_Global.ValidationResults[0].CrcOfTable                                                           = 0;
    CFE_TBL_Global.ValidationResults[0].ActiveBuffer                                                         = true;
    RegRecPtr->ValidateActiveIndex                                                                           = 0;

    /* Perform validation via manage call */
    UT_SetDeferredRetcode(UT_KEY(Test_CFE_TBL_ValidationFunc), 1, CFE_SUCCESS);
    RtnCode       = CFE_TBL_Manage(App1TblHandle2);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_VALIDATION_INF_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__,
              RtnCode == CFE_SUCCESS && EventsCorrect && CFE_TBL_Global.ValidationResults[0].Result == 0,
              "CFE_TBL_Manage",
              "Manage table that has a successful validation pending on an "
              "active buffer (double buffered)");

    /* Test successfully processing a table dump request */
    UT_InitData();
    RtnCode                                       = CFE_TBL_GetWorkingBuffer(&WorkingBufferPtr, RegRecPtr, false);
    CFE_TBL_Global.DumpControlBlocks[0].State     = CFE_TBL_DUMP_PENDING;
    CFE_TBL_Global.DumpControlBlocks[0].RegRecPtr = RegRecPtr;

    /* Save the name of the desired dump filename, table name, and size for
     * later
     */
    CFE_TBL_Global.DumpControlBlocks[0].DumpBufferPtr = WorkingBufferPtr;
    strncpy(CFE_TBL_Global.DumpControlBlocks[0].DumpBufferPtr->DataSource, "MyDumpFilename",
            sizeof(CFE_TBL_Global.DumpControlBlocks[0].DumpBufferPtr->DataSource) - 1);
    CFE_TBL_Global.DumpControlBlocks[0]
        .DumpBufferPtr->DataSource[sizeof(CFE_TBL_Global.DumpControlBlocks[0].DumpBufferPtr->DataSource) - 1] = 0;
    strncpy(CFE_TBL_Global.DumpControlBlocks[0].TableName, "ut_cfe_tbl.UT_Table2",
            sizeof(CFE_TBL_Global.DumpControlBlocks[0].TableName) - 1);
    CFE_TBL_Global.DumpControlBlocks[0].TableName[sizeof(CFE_TBL_Global.DumpControlBlocks[0].TableName) - 1] = 0;
    CFE_TBL_Global.DumpControlBlocks[0].Size = RegRecPtr->Size;
    RegRecPtr->DumpControlIndex              = 0;
    RtnCode                                  = CFE_TBL_Manage(App1TblHandle2);
    EventsCorrect                            = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_Manage",
              "Manage table that has a dump request pending");
}

/*
** Test function that updates the contents of a table if an update is pending
*/
void Test_CFE_TBL_Update(void)
{
    int32 RtnCode;
    bool  EventsCorrect;

    UtPrintf("Begin Test Update");

    /* Test processing an update on a single buffered table without
     * privileges
     */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_2);
    RtnCode       = CFE_TBL_Update(App1TblHandle1);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_UPDATE_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_NO_ACCESS && EventsCorrect, "CFE_TBL_Update",
              "Process an update on a single buffered table "
              "without privileges");

    /* Test processing an update on a single buffered table when no update
     * is pending
     */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    RtnCode       = CFE_TBL_Update(App1TblHandle1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_INFO_NO_UPDATE_PENDING && EventsCorrect, "CFE_TBL_Update",
              "Process an update on a single buffered table when no update "
              "is pending");

    /* Test processing an update on an application with a bad ID
     */
    UT_InitData();
    UT_SetAppID(CFE_ES_APPID_UNDEFINED);
    RtnCode       = CFE_TBL_Update(App1TblHandle1);
    EventsCorrect = (UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_NO_ACCESS && EventsCorrect, "CFE_TBL_Update",
              "Bad application ID");
}

/*
** Test function that obtains the pending action status for specified table
*/
void Test_CFE_TBL_GetStatus(void)
{
    int32 RtnCode;
    bool  EventsCorrect;

    UtPrintf("Begin Test Get Status");

    /* Test response to an attempt to get the status on a table that the
     * application is not allowed to see
     */
    UT_InitData();
    UT_SetAppID(CFE_ES_APPID_UNDEFINED);
    RtnCode       = CFE_TBL_GetStatus(App1TblHandle1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_NO_ACCESS && EventsCorrect, "CFE_TBL_GetStatus",
              "Attempt to get status on a table that the application is not "
              "allowed to see");

    /* Test response to an attempt to dump the buffer on a table that the
     * application is not allowed to see
     */
    UT_InitData();
    UT_SetAppID(CFE_ES_APPID_UNDEFINED);
    RtnCode       = CFE_TBL_DumpToBuffer(App1TblHandle1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_NO_ACCESS && EventsCorrect, "CFE_TBL_GetStatus",
              "Attempt to to dump the buffer on a table that the application "
              "is not allowed to see");
}

/*
** Test function that obtains characteristics/information for a specified table
*/
void Test_CFE_TBL_GetInfo(void)
{
    int32          RtnCode;
    bool           EventsCorrect;
    CFE_TBL_Info_t TblInfo;

    UtPrintf("Begin Test Get Info");

    /* Test successfully getting information on a table */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    RtnCode       = CFE_TBL_GetInfo(&TblInfo, "ut_cfe_tbl.UT_Table1");
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_GetInfo",
              "Get information on UT_Table1");

    /* Test response to attempt to get information on a non-existent table */
    UT_InitData();
    RtnCode       = CFE_TBL_GetInfo(&TblInfo, "ut_cfe_tbl.UT_Table_Not");
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_INVALID_NAME && EventsCorrect, "CFE_TBL_GetInfo",
              "Get information on non-existent table");
}

/*
** Test function that loads a specified table with data from the
** specified source
*/
void Test_CFE_TBL_TblMod(void)
{
    int32                       RtnCode;
    int32                       RtnCode2;
    bool                        EventsCorrect;
    CFE_FS_Header_t             FileHeader;
    UT_TempFile_t               File;
    uint32                      Index;
    CFE_TBL_Info_t              TblInfo1;
    UT_Table1_t *               TblDataPtr;
    char                        MyFilename[OS_MAX_PATH_LEN];
    CFE_TBL_AccessDescriptor_t *AccessDescPtr;
    CFE_TBL_RegistryRec_t *     RegRecPtr;
    CFE_TBL_Handle_t            AccessIterator;
    uint8                       CDS_Data[sizeof(UT_Table1_t)];

    UtPrintf("Begin Test Table Modified");

    FileHeader.SpacecraftID = CFE_PLATFORM_TBL_VALID_SCID_1;
    FileHeader.ProcessorID  = CFE_PLATFORM_TBL_VALID_PRID_1;

    /* Test adding a TBL API for notifying table services that the table has
     * been updated by the application
     */
    /* a. Test setup */
    UT_InitData();
    Test_CFE_TBL_ApiInit();
    UT_SetAppID(UT_TBL_APPID_1);
    UT_ResetPoolBufferIndex();

    /* Test setup for CFE_TBL_Modified; register a non critical table */
    RtnCode       = CFE_TBL_Register(&App1TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_CRITICAL, NULL);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_Register",
              "Add TBL API for notifying table services that table has "
              "been updated by application (setup)");

    /* b. Perform test */
    UT_ClearEventHistory();

    /* Configure for successful file read to initialize table */
    strncpy(FileHeader.Description, "FS header description", sizeof(FileHeader.Description) - 1);
    FileHeader.Description[sizeof(FileHeader.Description) - 1] = '\0';
    FileHeader.ContentType                                     = CFE_FS_FILE_CONTENT_ID;
    FileHeader.SubType                                         = CFE_FS_SubType_TBL_IMG;
    FileHeader.TimeSeconds                                     = 1704;
    FileHeader.TimeSubSeconds                                  = 104;
    strncpy(File.TblHeader.TableName, "ut_cfe_tbl.UT_Table1", sizeof(File.TblHeader.TableName) - 1);
    File.TblHeader.TableName[sizeof(File.TblHeader.TableName) - 1] = '\0';
    UT_TBL_SetupHeader(&File.TblHeader, 0, sizeof(UT_Table1_t));

    if (UT_Endianess == UT_LITTLE_ENDIAN)
    {
        File.TblData.TblElement1 = 0x04030201;
        File.TblData.TblElement2 = 0x08070605;
    }
    else
    {
        File.TblData.TblElement1 = 0x01020304;
        File.TblData.TblElement2 = 0x05060708;
    }

    UT_SetReadBuffer(&File, sizeof(File));
    UT_SetReadHeader(&FileHeader, sizeof(FileHeader));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, 0);

    /* Perform load */
    RtnCode       = CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_FILE, "MyInputFile");
    EventsCorrect = (UT_GetNumEventsSent() == 1 && UT_EventIsInHistory(CFE_TBL_LOAD_SUCCESS_INF_EID) == true);

    /* Modify the contents of the table */
    CFE_TBL_GetAddress((void **)&TblDataPtr, App1TblHandle1);
    TblDataPtr->TblElement1 ^= 0xFFFFFFFF;
    File.TblData.TblElement1 ^= 0xFFFFFFFF;

    /* Notify Table Services that the table has been modified */
    UT_SetDataBuffer(UT_KEY(CFE_ES_CopyToCDS), CDS_Data, sizeof(CDS_Data), false);
    RtnCode  = CFE_TBL_Modified(App1TblHandle1);
    RtnCode2 = CFE_TBL_GetInfo(&TblInfo1, "ut_cfe_tbl.UT_Table1");
    UT_Report(__FILE__, __LINE__,
              RtnCode == CFE_SUCCESS && RtnCode2 == CFE_SUCCESS && EventsCorrect &&
                  TblInfo1.TimeOfLastUpdate.Seconds == TblInfo1.TimeOfLastUpdate.Subseconds &&
                  strcmp(TblInfo1.LastFileLoaded, "MyInputFile(*)") == 0 &&
                  memcmp(CDS_Data, &File.TblData, sizeof(CDS_Data)) == 0,
              "CFE_TBL_Modified",
              "Add TBL API for notifying table services that table has "
              "been updated by application");

    /* Save the previous table's information for a subsequent test */
    AccessDescPtr  = &CFE_TBL_Global.Handles[App1TblHandle1];
    RegRecPtr      = &CFE_TBL_Global.Registry[AccessDescPtr->RegIndex];
    AccessIterator = RegRecPtr->HeadOfAccessList;

    /* Test response to adding a TBL API for notifying table services that
     * the table has been updated by application
     */
    /* a. Test setup */
    UT_InitData();

    /* Register a non critical table */
    RtnCode       = CFE_TBL_Register(&App1TblHandle1, "UT_Table2", sizeof(UT_Table1_t), CFE_TBL_OPT_DEFAULT, NULL);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_Register",
              "Add TBL API for notifying table services that table has "
              "been updated by application (setup)");

    /* Reset the current table entry pointer to a previous table in order to
     * exercise the path where one of the application IDs don't match
     */
    AccessDescPtr                                   = &CFE_TBL_Global.Handles[App1TblHandle1];
    RegRecPtr                                       = &CFE_TBL_Global.Registry[AccessDescPtr->RegIndex];
    CFE_TBL_Global.Handles[AccessIterator].NextLink = RegRecPtr->HeadOfAccessList;
    CFE_TBL_Global.Handles[AccessIterator].AppId    = UT_TBL_APPID_2;
    RegRecPtr->HeadOfAccessList                     = AccessIterator;

    /* Configure for successful file read to initialize table */
    strncpy(FileHeader.Description, "FS header description", sizeof(FileHeader.Description) - 1);
    FileHeader.Description[sizeof(FileHeader.Description) - 1] = '\0';
    FileHeader.ContentType                                     = CFE_FS_FILE_CONTENT_ID;
    FileHeader.SubType                                         = CFE_FS_SubType_TBL_IMG;
    FileHeader.TimeSeconds                                     = 1704;
    FileHeader.TimeSubSeconds                                  = 104;
    strncpy(File.TblHeader.TableName, "ut_cfe_tbl.UT_Table2", sizeof(File.TblHeader.TableName) - 1);
    File.TblHeader.TableName[sizeof(File.TblHeader.TableName) - 1] = '\0';
    UT_TBL_SetupHeader(&File.TblHeader, 0, sizeof(UT_Table1_t));

    File.TblData.TblElement1 = 0x04030201;
    File.TblData.TblElement2 = 0x08070605;
    UT_SetReadBuffer(&File, sizeof(File));
    UT_SetReadHeader(&FileHeader, sizeof(FileHeader));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, 0);

    /* Perform load with extra long filename */
    for (Index = 0; Index < sizeof(MyFilename) - 1; Index++)
    {
        MyFilename[Index] = 'a';
    }
    MyFilename[sizeof(MyFilename) - 1] = '\0';

    ASSERT(CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_FILE, MyFilename));
    ASSERT_EQ(UT_GetNumEventsSent(), 1);
    ASSERT_TRUE(UT_EventIsInHistory(CFE_TBL_LOAD_SUCCESS_INF_EID));

    /* Notify Table Services that the table has been modified */
    ASSERT(CFE_TBL_Modified(App1TblHandle1));
    ASSERT(CFE_TBL_GetInfo(&TblInfo1, "ut_cfe_tbl.UT_Table2"));
    ASSERT_EQ(TblInfo1.TimeOfLastUpdate.Seconds, TblInfo1.TimeOfLastUpdate.Subseconds);

    /*
     * LastFileLoaded (limited by mission) can be bigger than MyFilename (limited by osal),
     * need to adjust length of check to account for difference and modified marking
     */
    UtAssert_StrnCmp(TblInfo1.LastFileLoaded, MyFilename, sizeof(MyFilename) - 4, "%s == %s, %ld",
                     TblInfo1.LastFileLoaded, MyFilename, (long)sizeof(MyFilename) - 4);
    UtAssert_StrCmp(&TblInfo1.LastFileLoaded[sizeof(MyFilename) - 4], "(*)", "%s == (*)",
                    &TblInfo1.LastFileLoaded[sizeof(MyFilename) - 4]);

    /* Test response to an invalid handle */
    RtnCode = CFE_TBL_Modified(CFE_TBL_BAD_TABLE_HANDLE);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_INVALID_HANDLE && EventsCorrect, "CFE_TBL_Modified",
              "Invalid table handle");
}

/*
** Tests for the remaining functions in cfe_tbl_internal.c
*/
void Test_CFE_TBL_Internal(void)
{
    int32                       RtnCode;
    bool                        EventsCorrect;
    CFE_TBL_LoadBuff_t *        WorkingBufferPtr;
    CFE_TBL_RegistryRec_t *     RegRecPtr;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr;
    char                        FilenameLong[OS_MAX_PATH_LEN + 10];
    char                        Filename[OS_MAX_PATH_LEN];
    int32                       i;
    CFE_FS_Header_t             StdFileHeader;
    CFE_TBL_File_Hdr_t          TblFileHeader;
    osal_id_t                   FileDescriptor;
    void *                      TblPtr;

    UtPrintf("Begin Test Internal");

    FileDescriptor             = OS_OBJECT_ID_UNDEFINED;
    StdFileHeader.SpacecraftID = CFE_PLATFORM_TBL_VALID_SCID_1;
    StdFileHeader.ProcessorID  = CFE_PLATFORM_TBL_VALID_PRID_1;

    /* Test setup - register a double buffered table */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    RtnCode       = CFE_TBL_Register(&App1TblHandle2, "UT_Table3", sizeof(UT_Table1_t), CFE_TBL_OPT_DBL_BUFFER,
                               Test_CFE_TBL_ValidationFunc);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == false && UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_Register",
              "CFE_TBL_GetWorkingBuffer setup - register a double buffered table");

    /* Test successful initial load of double buffered table */
    UT_InitData();
    AccessDescPtr = &CFE_TBL_Global.Handles[App1TblHandle2];
    RegRecPtr     = &CFE_TBL_Global.Registry[AccessDescPtr->RegIndex];
    strncpy(RegRecPtr->Name, "ut_cfe_tbl.UT_Table3", sizeof(RegRecPtr->Name) - 1);
    RegRecPtr->Name[sizeof(RegRecPtr->Name) - 1] = '\0';
    RegRecPtr->TableLoadedOnce                   = false;
    RegRecPtr->LoadInProgress                    = CFE_TBL_NO_LOAD_IN_PROGRESS;
    RtnCode                                      = CFE_TBL_GetWorkingBuffer(&WorkingBufferPtr, RegRecPtr, true);
    EventsCorrect                                = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__,
              RtnCode == CFE_SUCCESS && EventsCorrect &&
                  WorkingBufferPtr == &RegRecPtr->Buffers[RegRecPtr->ActiveBufferIndex],
              "CFE_TBL_GetWorkingBuffer", "Initial load of double buffered table");

    /* Test response to a single buffered table with a mutex sem take
     * failure
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemTake), 1, OS_ERROR);
    AccessDescPtr = &CFE_TBL_Global.Handles[App1TblHandle1];
    RegRecPtr     = &CFE_TBL_Global.Registry[AccessDescPtr->RegIndex];
    RtnCode       = CFE_TBL_GetWorkingBuffer(&WorkingBufferPtr, RegRecPtr, false);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_GetWorkingBuffer",
              "Single buffered table has mutex sem take failure");

    /* Test CFE_TBL_LoadFromFile response to a file name that is too long */
    UT_InitData();

    for (i = 0; i < sizeof(FilenameLong) - 1; i++)
    {
        FilenameLong[i] = 'a';
    }
    FilenameLong[i] = '\0'; /* Null terminate file name string */

    RtnCode       = CFE_TBL_LoadFromFile("UT", WorkingBufferPtr, RegRecPtr, FilenameLong);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_LOAD_FILENAME_LONG_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_FILENAME_TOO_LONG && EventsCorrect, "CFE_TBL_LoadFromFile",
              "Filename too long");

    /* Test CFE_TBL_LoadFromFile response to a file that's content is too large
     * (according to the header)
     */
    UT_InitData();
    strncpy(Filename, FilenameLong, sizeof(Filename) - 1);
    Filename[sizeof(Filename) - 1] = '\0';
    strncpy(StdFileHeader.Description, "FS header description", sizeof(StdFileHeader.Description) - 1);
    StdFileHeader.Description[sizeof(StdFileHeader.Description) - 1] = '\0';
    StdFileHeader.ContentType                                        = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType                                            = CFE_FS_SubType_TBL_IMG;
    strncpy(TblFileHeader.TableName, "ut_cfe_tbl.UT_Table2", sizeof(TblFileHeader.TableName) - 1);
    TblFileHeader.TableName[sizeof(TblFileHeader.TableName) - 1] = '\0';
    UT_TBL_SetupHeader(&TblFileHeader, 1, sizeof(UT_Table1_t));

    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    RtnCode       = CFE_TBL_LoadFromFile("UT", WorkingBufferPtr, RegRecPtr, Filename);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_LOAD_EXCEEDS_SIZE_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_FILE_TOO_LARGE && EventsCorrect, "CFE_TBL_LoadFromFile",
              "File content too large (according to header)");

    /* Test CFE_TBL_LoadFromFile response that's file content is too large
     * (too much content)
     */
    UT_InitData();
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType     = CFE_FS_SubType_TBL_IMG;
    strncpy(TblFileHeader.TableName, "ut_cfe_tbl.UT_Table2", sizeof(TblFileHeader.TableName) - 1);
    TblFileHeader.TableName[sizeof(TblFileHeader.TableName) - 1] = '\0';
    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(UT_Table1_t));

    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 2, sizeof(UT_Table1_t));
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_FILE_TOO_BIG_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_FILE_TOO_LARGE && EventsCorrect, "CFE_TBL_LoadFromFile",
              "File content too large (too much content)");

    /* Test CFE_TBL_LoadFromFile response to the file content being
     * incomplete
     */
    UT_InitData();
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType     = CFE_FS_SubType_TBL_IMG;
    strncpy(TblFileHeader.TableName, "ut_cfe_tbl.UT_Table2", sizeof(TblFileHeader.TableName) - 1);
    TblFileHeader.TableName[sizeof(TblFileHeader.TableName) - 1] = '\0';
    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(UT_Table1_t));

    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 2, sizeof(UT_Table1_t) - 1);
    UT_SetDeferredRetcode(UT_KEY(OS_read), 1, 0);
    RtnCode       = CFE_TBL_LoadFromFile("UT", WorkingBufferPtr, RegRecPtr, Filename);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_FILE_INCOMPLETE_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_LOAD_INCOMPLETE && EventsCorrect, "CFE_TBL_LoadFromFile",
              "File content incomplete");

    /* Test CFE_TBL_LoadFromFile response to the file being for the
     * wrong table
     */
    UT_InitData();
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType     = CFE_FS_SubType_TBL_IMG;
    strncpy(TblFileHeader.TableName, "ut_cfe_tbl.NotUT_Table2", sizeof(TblFileHeader.TableName) - 1);
    TblFileHeader.TableName[sizeof(TblFileHeader.TableName) - 1] = '\0';
    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(UT_Table1_t));

    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, 0);
    RtnCode       = CFE_TBL_LoadFromFile("UT", WorkingBufferPtr, RegRecPtr, Filename);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_LOAD_TBLNAME_MISMATCH_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_FILE_FOR_WRONG_TABLE && EventsCorrect, "CFE_TBL_LoadFromFile",
              "File for wrong table");

    /* Test CFE_TBL_LoadFromFile response to an OS open error */
    UT_InitData();
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType     = CFE_FS_SubType_TBL_IMG;
    strncpy(TblFileHeader.TableName, "ut_cfe_tbl.UT_Table1", sizeof(TblFileHeader.TableName) - 1);
    TblFileHeader.TableName[sizeof(TblFileHeader.TableName) - 1] = '\0';
    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(UT_Table1_t));

    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_SetDefaultReturnValue(UT_KEY(OS_OpenCreate), OS_ERROR);
    RtnCode       = CFE_TBL_LoadFromFile("UT", WorkingBufferPtr, RegRecPtr, Filename);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_FILE_ACCESS_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_ACCESS && EventsCorrect, "CFE_TBL_LoadFromFile",
              "OS open error");

    /* Test CFE_TBL_LoadFromFile response to a file too short warning */
    UT_InitData();
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType     = CFE_FS_SubType_TBL_IMG;
    strncpy(TblFileHeader.TableName, "ut_cfe_tbl.UT_Table2", sizeof(TblFileHeader.TableName) - 1);
    TblFileHeader.TableName[sizeof(TblFileHeader.TableName) - 1] = '\0';
    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(UT_Table1_t) - 1);

    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, 0);
    RtnCode       = CFE_TBL_LoadFromFile("UT", WorkingBufferPtr, RegRecPtr, Filename);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_WARN_SHORT_FILE && EventsCorrect, "CFE_TBL_LoadFromFile",
              "File too short warning");

    /* Test CFE_TBL_ReadHeaders response to a failure reading the standard cFE
     * file header
     */
    UT_InitData();
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType     = CFE_FS_SubType_TBL_IMG;
    strncpy(TblFileHeader.TableName, "ut_cfe_tbl.UT_Table1", sizeof(TblFileHeader.TableName) - 1);
    TblFileHeader.TableName[sizeof(TblFileHeader.TableName) - 1] = '\0';
    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(UT_Table1_t) - 1);

    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    strncpy(Filename, "MyTestInputFilename", sizeof(Filename) - 1);
    Filename[sizeof(Filename) - 1] = '\0';
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_ReadHeader), 1, sizeof(CFE_FS_Header_t) - 1);
    RtnCode       = CFE_TBL_ReadHeaders(FileDescriptor, &StdFileHeader, &TblFileHeader, Filename);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_FILE_STD_HDR_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_NO_STD_HEADER && EventsCorrect, "CFE_TBL_ReadHeaders",
              "Failure reading standard cFE file header");

    /* Test CFE_TBL_ReadHeaders response to a bad magic number in cFE
     * standard header
     */
    UT_InitData();
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID - 1;
    StdFileHeader.SubType     = CFE_FS_SubType_TBL_IMG;
    strncpy(TblFileHeader.TableName, "ut_cfe_tbl.UT_Table1", sizeof(TblFileHeader.TableName) - 1);
    TblFileHeader.TableName[sizeof(TblFileHeader.TableName) - 1] = '\0';
    TblFileHeader.NumBytes                                       = sizeof(UT_Table1_t) - 1;
    TblFileHeader.Offset                                         = 0;

    if (UT_Endianess == UT_LITTLE_ENDIAN)
    {
        CFE_TBL_ByteSwapUint32(&TblFileHeader.NumBytes);
        CFE_TBL_ByteSwapUint32(&TblFileHeader.Offset);
    }

    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    RtnCode       = CFE_TBL_ReadHeaders(FileDescriptor, &StdFileHeader, &TblFileHeader, Filename);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_FILE_TYPE_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_BAD_CONTENT_ID && EventsCorrect, "CFE_TBL_ReadHeaders",
              "Bad magic number in cFE standard header");

    /* Test CFE_TBL_ReadHeaders response to a wrong cFE file subtype */
    UT_InitData();
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType     = CFE_FS_SubType_TBL_IMG - 1;
    strncpy(TblFileHeader.TableName, "ut_cfe_tbl.UT_Table1", sizeof(TblFileHeader.TableName) - 1);
    TblFileHeader.TableName[sizeof(TblFileHeader.TableName) - 1] = '\0';
    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(UT_Table1_t) - 1);

    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    RtnCode       = CFE_TBL_ReadHeaders(FileDescriptor, &StdFileHeader, &TblFileHeader, Filename);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_FILE_SUBTYPE_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_BAD_SUBTYPE_ID && EventsCorrect, "CFE_TBL_ReadHeaders",
              "Wrong cFE file subType");

    /* Test CFE_TBL_ReadHeaders response to a failure reading the cFE
     * table header
     */
    UT_InitData();
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType     = CFE_FS_SubType_TBL_IMG;
    strncpy(TblFileHeader.TableName, "ut_cfe_tbl.UT_Table1", sizeof(TblFileHeader.TableName) - 1);
    TblFileHeader.TableName[sizeof(TblFileHeader.TableName) - 1] = '\0';
    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(UT_Table1_t) - 1);

    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 1, sizeof(CFE_TBL_File_Hdr_t) - 1);
    RtnCode       = CFE_TBL_ReadHeaders(FileDescriptor, &StdFileHeader, &TblFileHeader, Filename);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_FILE_TBL_HDR_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_NO_TBL_HEADER && EventsCorrect, "CFE_TBL_ReadHeaders",
              "Failure reading cFE table header");

    /* Test CFE_TBL_RemoveAccessLink response to a failure to put back the
     * memory buffer for a double buffered table
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_PutPoolBuf), 2, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    RtnCode       = CFE_TBL_RemoveAccessLink(App1TblHandle2);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_ES_ERR_RESOURCEID_NOT_VALID && EventsCorrect,
              "CFE_TBL_RemoveAccessLink", "Fail to put back memory buffer for double buffered table");

    /* EarlyInit - Table Registry Mutex Create Failure */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemCreate), 1, OS_ERROR);
    RtnCode       = CFE_TBL_EarlyInit();
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == OS_ERROR && EventsCorrect, "CFE_TBL_EarlyInit",
              "Table registry mutex create failure");

    /* Test CFE_TBL_EarlyInit response to a work buffer mutex create failure */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemCreate), 2, OS_ERROR);
    RtnCode       = CFE_TBL_EarlyInit();
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == OS_ERROR && EventsCorrect, "CFE_TBL_EarlyInit",
              "Work buffer mutex create failure");

    /* Test CFE_TBL_EarlyInit response to a memory pool create failure */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_PoolCreate), 1, CFE_ES_BAD_ARGUMENT);
    RtnCode       = CFE_TBL_EarlyInit();
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_ES_BAD_ARGUMENT && EventsCorrect, "CFE_TBL_EarlyInit",
              "Memory pool create failure");

    /* Test CFE_TBL_EarlyInit reponse to a get pool buffer failure */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetPoolBuf), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    RtnCode       = CFE_TBL_EarlyInit();
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_ES_ERR_RESOURCEID_NOT_VALID && EventsCorrect, "CFE_TBL_EarlyInit",
              "Get pool buffer failure");

    /* Test CFE_TBL_EarlyInit response where the CDS already exists but
     * restore fails
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RegisterCDSEx), 1, CFE_ES_CDS_ALREADY_EXISTS);
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RestoreFromCDS), 1, CFE_ES_CDS_BLOCK_CRC_ERR);
    RtnCode       = CFE_TBL_EarlyInit();
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_EarlyInit",
              "CDS already exists but restore fails");

    /* Test CFE_TBL_EarlyInit response when no CDS is available */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RegisterCDSEx), 1, CFE_ES_NOT_IMPLEMENTED);
    RtnCode       = CFE_TBL_EarlyInit();
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_EarlyInit", "No CDS available");

    /* Test CFE_TBL_EarlyInit response to a failure to save a critical table
     * registry to the CDS
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CopyToCDS), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    RtnCode       = CFE_TBL_EarlyInit();
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_EarlyInit",
              "Fail to save critical table registry to CDS");

    /* Reset, then register tables for subsequent tests */
    /* a. Reset tables */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    RtnCode = CFE_TBL_EarlyInit();
    UT_Report(__FILE__, __LINE__, (RtnCode == CFE_SUCCESS), "CFE_TBL_EarlyInit", "Reset (setup - part 1)");

    /* b. Register critical single buffered table */
    UT_InitData();
    RtnCode = CFE_TBL_Register(&App1TblHandle1, "UT_Table1", sizeof(UT_Table1_t),
                               CFE_TBL_OPT_DEFAULT | CFE_TBL_OPT_CRITICAL, Test_CFE_TBL_ValidationFunc);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS, "CFE_TBL_Register",
              "Critical single buffered table (setup - part 2)");

    /* c. Register critical double buffered table */
    UT_InitData();
    RtnCode = CFE_TBL_Register(&App1TblHandle2, "UT_Table2", sizeof(UT_Table1_t),
                               CFE_TBL_OPT_DBL_BUFFER | CFE_TBL_OPT_CRITICAL, Test_CFE_TBL_ValidationFunc);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS, "CFE_TBL_Register",
              "Critical double buffered table (setup - part 3)");

    /* d. Perform an initial load on the critical single buffered table */
    UT_InitData();
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType     = CFE_FS_SubType_TBL_IMG;
    strncpy(TblFileHeader.TableName, "ut_cfe_tbl.UT_Table1", sizeof(TblFileHeader.TableName) - 1);
    TblFileHeader.TableName[sizeof(TblFileHeader.TableName) - 1] = '\0';
    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(UT_Table1_t));

    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, 0);
    RtnCode       = CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_FILE, "TblSrcFileName.dat");
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_LOAD_SUCCESS_INF_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_Load",
              "Critical single buffered table (setup - part 4)");

    /* e. Update CDS for single buffered table */
    UT_InitData();
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType     = CFE_FS_SubType_TBL_IMG;
    strncpy(TblFileHeader.TableName, "ut_cfe_tbl.UT_Table1", sizeof(TblFileHeader.TableName) - 1);
    TblFileHeader.TableName[sizeof(TblFileHeader.TableName) - 1] = '\0';
    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(UT_Table1_t));

    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, 0);
    RtnCode       = CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_FILE, "TblSrcFileName.dat");
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_LOAD_SUCCESS_INF_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_Load",
              "Update CDS for single buffered table (setup - part 5)");

    /* f. Perform an initial load on the critical double buffered table */
    UT_InitData();
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType     = CFE_FS_SubType_TBL_IMG;
    strncpy(TblFileHeader.TableName, "ut_cfe_tbl.UT_Table2", sizeof(TblFileHeader.TableName) - 1);
    TblFileHeader.TableName[sizeof(TblFileHeader.TableName) - 1] = '\0';
    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(UT_Table1_t));

    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, 0);
    RtnCode       = CFE_TBL_Load(App1TblHandle2, CFE_TBL_SRC_FILE, "TblSrcFileName.dat");
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_LOAD_SUCCESS_INF_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_Load",
              "Critical single buffered table (setup - part 6)");

    /* g. Update CDS for double buffered table */
    UT_InitData();
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType     = CFE_FS_SubType_TBL_IMG;
    strncpy(TblFileHeader.TableName, "ut_cfe_tbl.UT_Table2", sizeof(TblFileHeader.TableName) - 1);
    TblFileHeader.TableName[sizeof(TblFileHeader.TableName) - 1] = '\0';
    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(UT_Table1_t));

    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, 0);
    CFE_TBL_GetAddress(&TblPtr, App1TblHandle2);
    RtnCode       = CFE_TBL_Load(App1TblHandle2, CFE_TBL_SRC_FILE, "TblSrcFileName.dat");
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_LOAD_SUCCESS_INF_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_Load",
              "Update CDS for single buffered table (setup - part 7)");

    /* Test CFE_TBL_GetWorkingBuffer response when both double buffered table
     * buffers are locked
     */
    UT_InitData();
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType     = CFE_FS_SubType_TBL_IMG;
    strncpy(TblFileHeader.TableName, "ut_cfe_tbl.UT_Table2", sizeof(TblFileHeader.TableName) - 1);
    TblFileHeader.TableName[sizeof(TblFileHeader.TableName) - 1] = '\0';
    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(UT_Table1_t));

    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, 0);
    RtnCode       = CFE_TBL_Load(App1TblHandle2, CFE_TBL_SRC_FILE, "TblSrcFileName.dat");
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_NO_WORK_BUFFERS_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_NO_BUFFER_AVAIL && EventsCorrect, "CFE_TBL_Load",
              "Both double buffered table buffers locked");

    /* Release buffer for error writing to CDS test */
    /* a. Get table address */
    UT_InitData();
    RtnCode       = CFE_TBL_GetAddress(&TblPtr, App1TblHandle2);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_INFO_UPDATED && EventsCorrect, "CFE_TBL_GetAddress",
              "Error writing to CDS (setup - part 1)");

    /* b. Release table address */
    UT_ClearEventHistory();
    RtnCode       = CFE_TBL_ReleaseAddress(App1TblHandle2);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_ReleaseAddress",
              "Error writing to CDS (setup - part 2)");

    /* c. Perform test */
    UT_ClearEventHistory();
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType     = CFE_FS_SubType_TBL_IMG;
    strncpy(TblFileHeader.TableName, "ut_cfe_tbl.UT_Table2", sizeof(TblFileHeader.TableName) - 1);
    TblFileHeader.TableName[sizeof(TblFileHeader.TableName) - 1] = '\0';
    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(UT_Table1_t));

    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, 0);
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CopyToCDS), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    RtnCode       = CFE_TBL_Load(App1TblHandle2, CFE_TBL_SRC_FILE, "TblSrcFileName.dat");
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_LOAD_SUCCESS_INF_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, ((RtnCode == CFE_SUCCESS) && EventsCorrect), "CFE_TBL_Load", "Error writing to CDS");

    /* Release buffer for error writing to CDS (second time) test */
    /* a. Get table address */
    UT_InitData();
    RtnCode       = CFE_TBL_GetAddress(&TblPtr, App1TblHandle2);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_INFO_UPDATED && EventsCorrect, "CFE_TBL_GetAddress",
              "Error writing to CDS second time (setup - part 1)");

    /* b. Release table address */
    UT_ClearEventHistory();
    RtnCode       = CFE_TBL_ReleaseAddress(App1TblHandle2);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_ReleaseAddress",
              "Error writing to CDS second time (setup - part 2)");

    /* c. Perform test */
    UT_ClearEventHistory();
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType     = CFE_FS_SubType_TBL_IMG;
    strncpy(TblFileHeader.TableName, "ut_cfe_tbl.UT_Table2", sizeof(TblFileHeader.TableName) - 1);
    TblFileHeader.TableName[sizeof(TblFileHeader.TableName) - 1] = '\0';
    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(UT_Table1_t));

    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, 0);
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CopyToCDS), 2, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    RtnCode       = CFE_TBL_Load(App1TblHandle2, CFE_TBL_SRC_FILE, "TblSrcFileName.dat");
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_LOAD_SUCCESS_INF_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_Load",
              "Error writing to CDS (second time)");

    /* Release buffer for failure to find the CDS handle in the CDS
     * registry test
     */
    /* a. Get table address */
    UT_InitData();
    RtnCode       = CFE_TBL_GetAddress(&TblPtr, App1TblHandle2);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_INFO_UPDATED && EventsCorrect, "CFE_TBL_GetAddress",
              "Failure to find CDS handle in CDS registry (setup - part 1)");

    /* b. Release table address */
    UT_ClearEventHistory();
    RtnCode       = CFE_TBL_ReleaseAddress(App1TblHandle2);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_ReleaseAddress",
              "Failure to find CDS handle in CDS registry (setup - part 2)");

    /* c. Perform test */
    UT_ClearEventHistory();
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType     = CFE_FS_SubType_TBL_IMG;
    strncpy(TblFileHeader.TableName, "ut_cfe_tbl.UT_Table2", sizeof(TblFileHeader.TableName) - 1);
    TblFileHeader.TableName[sizeof(TblFileHeader.TableName) - 1] = '\0';
    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(UT_Table1_t));

    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, 0);
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CopyToCDS), 2, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    AccessDescPtr = &CFE_TBL_Global.Handles[App1TblHandle2];
    RegRecPtr     = &CFE_TBL_Global.Registry[AccessDescPtr->RegIndex];

    for (i = 0; i < CFE_PLATFORM_TBL_MAX_CRITICAL_TABLES; i++)
    {
        if (CFE_RESOURCEID_TEST_EQUAL(CFE_TBL_Global.CritReg[i].CDSHandle, RegRecPtr->CDSHandle))
        {
            CFE_TBL_Global.CritReg[i].CDSHandle = CFE_ES_CDSHANDLE_C(CFE_RESOURCEID_RESERVED);
        }
    }

    RtnCode       = CFE_TBL_Load(App1TblHandle2, CFE_TBL_SRC_FILE, "TblSrcFileName.dat");
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_LOAD_SUCCESS_INF_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_Load",
              "Failure to find CDS handle in CDS registry");

    /* Test unregistering a shared table */
    /* a. Share table */
    UT_InitData();
    CFE_TBL_Global.CritReg[0].CDSHandle = RegRecPtr->CDSHandle;
    UT_SetAppID(UT_TBL_APPID_2);
    RtnCode = CFE_TBL_Share(&App2TblHandle1, "ut_cfe_tbl.UT_Table1");
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS, "CFE_TBL_Share", "Unregister shared table (setup)");

    /* b. Perform test */
    UT_ClearEventHistory();
    RtnCode       = CFE_TBL_Unregister(App2TblHandle1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_Unregister",
              "Unregister shared table");

    /* Test successful application cleanup */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_PutPoolBuf), -1);
    AccessDescPtr                                 = &CFE_TBL_Global.Handles[App1TblHandle1];
    RegRecPtr                                     = &CFE_TBL_Global.Registry[AccessDescPtr->RegIndex];
    CFE_TBL_Global.DumpControlBlocks[3].State     = CFE_TBL_DUMP_PENDING;
    CFE_TBL_Global.DumpControlBlocks[3].RegRecPtr = RegRecPtr;
    RegRecPtr->LoadInProgress                     = 1;
    CFE_TBL_Global.LoadBuffs[1].Taken             = true;
    CFE_TBL_CleanUpApp(UT_TBL_APPID_1);
    UT_Report(__FILE__, __LINE__,
              CFE_TBL_Global.DumpControlBlocks[3].State == CFE_TBL_DUMP_FREE &&
                  CFE_RESOURCEID_TEST_EQUAL(RegRecPtr->OwnerAppId, CFE_TBL_NOT_OWNED) &&
                  CFE_TBL_Global.LoadBuffs[RegRecPtr->LoadInProgress].Taken == false &&
                  RegRecPtr->LoadInProgress == CFE_TBL_NO_LOAD_IN_PROGRESS,
              "CFE_TBL_CleanUpApp", "Execute clean up - success");

    /* Test response to an attempt to use an invalid table handle */
    UT_InitData();
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType     = CFE_FS_SubType_TBL_IMG;
    strncpy(TblFileHeader.TableName, "ut_cfe_tbl.UT_Table2", sizeof(TblFileHeader.TableName) - 1);
    TblFileHeader.TableName[sizeof(TblFileHeader.TableName) - 1] = '\0';
    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(UT_Table1_t));

    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, 0);
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CopyToCDS), 2, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    RtnCode       = CFE_TBL_Load(App1TblHandle2, CFE_TBL_SRC_FILE, "TblSrcFileName.dat");
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_HANDLE_ACCESS_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_INVALID_HANDLE && EventsCorrect, "CFE_TBL_Load",
              "Attempt to use an invalid handle");

    /* Test CFE_TBL_EarlyInit response where the CDS already exists and
     * restore succeeds
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RegisterCDSEx), 1, CFE_ES_CDS_ALREADY_EXISTS);
    RtnCode       = CFE_TBL_EarlyInit();
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_EarlyInit",
              "CDS already exists and restore succeeds");

    /* Test CFE_TBL_CheckAccessRights response when the application ID matches
     * the table task application ID
     */
    UT_InitData();
    CFE_TBL_Global.TableTaskAppId = UT_TBL_APPID_1;
    RtnCode                       = CFE_TBL_CheckAccessRights(App2TblHandle1, UT_TBL_APPID_1);
    EventsCorrect                 = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_EarlyInit",
              "Application ID matches table task application ID");

    /* Test CFE_TBL_FindFreeRegistryEntry response when the registry entry is
     * not owned but is not at the end of the list
     */
    UT_InitData();
    CFE_TBL_Global.Registry[0].OwnerAppId       = CFE_TBL_NOT_OWNED;
    CFE_TBL_Global.Registry[0].HeadOfAccessList = CFE_TBL_END_OF_LIST + 1;
    RtnCode                                     = CFE_TBL_FindFreeRegistryEntry();
    EventsCorrect                               = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == 1 && EventsCorrect, "CFE_TBL_FindFreeRegistryEntry",
              "Registry entry not owned but not at end of list");

    /* Test CFE_TBL_LockRegistry response when an error occurs taking the mutex
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemTake), 1, OS_ERROR);
    RtnCode       = CFE_TBL_LockRegistry();
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == OS_ERROR && EventsCorrect, "CFE_TBL_LockRegistry", "Mutex take error");

    /* Test CFE_TBL_UnlockRegistry response when an error occurs giving the
     * mutex
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemGive), 1, OS_ERROR);
    RtnCode       = CFE_TBL_UnlockRegistry();
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == OS_ERROR && EventsCorrect, "CFE_TBL_UnlockRegistry", "Mutex give error");

    /* Test CFE_TBL_LoadFromFile response to an invalid header length */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_ReadHeader), 1, sizeof(CFE_FS_Header_t) - 1);
    RtnCode       = CFE_TBL_LoadFromFile("UT", WorkingBufferPtr, RegRecPtr, Filename);
    EventsCorrect = (UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_NO_STD_HEADER && EventsCorrect, "CFE_TBL_LoadFromFile",
              "No standard header");

    /* Test CFE_TBL_UpdateInternal response when an inactive buffer is ready to
     * be copied but a load is in progress
     */
    UT_InitData();
    AccessDescPtr             = &CFE_TBL_Global.Handles[App1TblHandle2];
    RegRecPtr                 = &CFE_TBL_Global.Registry[AccessDescPtr->RegIndex];
    RegRecPtr->LoadPending    = true;
    RegRecPtr->LoadInProgress = CFE_TBL_NO_LOAD_IN_PROGRESS;
    RtnCode                   = CFE_TBL_UpdateInternal(App1TblHandle2, RegRecPtr, AccessDescPtr);
    EventsCorrect             = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_INFO_NO_UPDATE_PENDING && EventsCorrect, "CFE_TBL_UpdateInternal",
              "Inactive buffer ready while load in progress");

    /* Test CFE_TBL_UpdateInternal response when an inactive buffer is ready to
     * be copied but a load is in progress
     */
    UT_InitData();
    AccessDescPtr             = &CFE_TBL_Global.Handles[App1TblHandle2];
    RegRecPtr                 = &CFE_TBL_Global.Registry[AccessDescPtr->RegIndex];
    RegRecPtr->LoadPending    = true;
    RegRecPtr->LoadInProgress = CFE_TBL_NO_LOAD_IN_PROGRESS + 1;
    RegRecPtr->CriticalTable  = false;
    RegRecPtr->DoubleBuffered = true;
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CopyToCDS), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    RtnCode       = CFE_TBL_UpdateInternal(App1TblHandle2, RegRecPtr, AccessDescPtr);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_UpdateInternal",
              "Active buffer ready, no load in progress, double buffered "
              "non-critical table");

    /* Test CFE_TBL_UpdateInternal single buffer memcpy when
     * source and dest are not equal
     */
    UT_InitData();
    AccessDescPtr             = &CFE_TBL_Global.Handles[App1TblHandle2];
    RegRecPtr                 = &CFE_TBL_Global.Registry[AccessDescPtr->RegIndex];
    RegRecPtr->LoadPending    = true;
    RegRecPtr->LoadInProgress = CFE_TBL_NO_LOAD_IN_PROGRESS + 1;
    RegRecPtr->DoubleBuffered = false;
    RtnCode                   = CFE_TBL_UpdateInternal(App1TblHandle2, RegRecPtr, AccessDescPtr);
    EventsCorrect             = (UT_GetNumEventsSent() == 0);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_SUCCESS && EventsCorrect, "CFE_TBL_UpdateInternal",
              "Update single buffer table memcpy test with src != dest");

    /* Test application cleanup where there are no dumped tables to delete and
     * the application doesn't own the table
     */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_PutPoolBuf), -1);
    CFE_TBL_Global.Handles[0].AppId               = UT_TBL_APPID_1;
    AccessDescPtr                                 = &CFE_TBL_Global.Handles[App1TblHandle2];
    RegRecPtr                                     = &CFE_TBL_Global.Registry[AccessDescPtr->RegIndex];
    RegRecPtr->OwnerAppId                         = CFE_TBL_NOT_OWNED;
    CFE_TBL_Global.DumpControlBlocks[3].State     = CFE_TBL_DUMP_PENDING;
    CFE_TBL_Global.DumpControlBlocks[3].RegRecPtr = RegRecPtr;
    CFE_TBL_CleanUpApp(UT_TBL_APPID_1);
    UT_Report(__FILE__, __LINE__,
              CFE_TBL_Global.DumpControlBlocks[3].State == CFE_TBL_DUMP_PENDING &&
                  CFE_RESOURCEID_TEST_EQUAL(RegRecPtr->OwnerAppId, CFE_TBL_NOT_OWNED),
              "CFE_TBL_CleanUpApp",
              "Execute clean up - no dumped tables to delete, application "
              "doesn't own table");

#if (CFE_PLATFORM_TBL_VALID_SCID_COUNT > 0)
    /* Test CFE_TBL_ReadHeaders response to an invalid spacecraft ID */
    UT_InitData();
    StdFileHeader.ContentType  = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType      = CFE_FS_SubType_TBL_IMG;
    StdFileHeader.SpacecraftID = -1;
    StdFileHeader.ProcessorID  = CFE_PLATFORM_TBL_VALID_PRID_1;
    strncpy(TblFileHeader.TableName, "ut_cfe_tbl.UT_Table1", sizeof(TblFileHeader.TableName) - 1);
    TblFileHeader.TableName[sizeof(TblFileHeader.TableName) - 1] = '\0';
    TblFileHeader.NumBytes                                       = sizeof(UT_Table1_t) - 1;
    TblFileHeader.Offset                                         = 0;

    if (UT_Endianess == UT_LITTLE_ENDIAN)
    {
        CFE_TBL_ByteSwapUint32(&TblFileHeader.NumBytes);
        CFE_TBL_ByteSwapUint32(&TblFileHeader.Offset);
    }

    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    strncpy(Filename, "MyTestInputFilename", sizeof(Filename) - 1);
    Filename[sizeof(Filename) - 1] = '\0';
    RtnCode                        = CFE_TBL_ReadHeaders(FileDescriptor, &StdFileHeader, &TblFileHeader, Filename);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_SPACECRAFT_ID_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_BAD_SPACECRAFT_ID && EventsCorrect, "CFE_TBL_ReadHeaders",
              "Invalid spacecraft ID");
#else
    UT_Report(__FILE__, __LINE__, true, "CFE_TBL_ReadHeaders", "*Not tested* Invalid spacecraft ID ");
#endif

#if (CFE_PLATFORM_TBL_VALID_PRID_COUNT > 0)
    /* Test CFE_TBL_ReadHeaders response to an invalid processor ID */
    UT_InitData();
    StdFileHeader.ContentType  = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType      = CFE_FS_SubType_TBL_IMG;
    StdFileHeader.SpacecraftID = CFE_PLATFORM_TBL_VALID_SCID_1;
    StdFileHeader.ProcessorID  = -1;
    strncpy(TblFileHeader.TableName, "ut_cfe_tbl.UT_Table1", sizeof(TblFileHeader.TableName) - 1);
    TblFileHeader.TableName[sizeof(TblFileHeader.TableName) - 1] = '\0';
    TblFileHeader.NumBytes                                       = sizeof(UT_Table1_t) - 1;
    TblFileHeader.Offset                                         = 0;

    if (UT_Endianess == UT_LITTLE_ENDIAN)
    {
        CFE_TBL_ByteSwapUint32(&TblFileHeader.NumBytes);
        CFE_TBL_ByteSwapUint32(&TblFileHeader.Offset);
    }

    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    strncpy(Filename, "MyTestInputFilename", sizeof(Filename) - 1);
    Filename[sizeof(Filename) - 1] = '\0';
    RtnCode                        = CFE_TBL_ReadHeaders(FileDescriptor, &StdFileHeader, &TblFileHeader, Filename);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_PROCESSOR_ID_ERR_EID) == true && UT_GetNumEventsSent() == 1);
    UT_Report(__FILE__, __LINE__, RtnCode == CFE_TBL_ERR_BAD_PROCESSOR_ID && EventsCorrect, "CFE_TBL_ReadHeaders",
              "Invalid processor ID");
#else
    UT_Report(__FILE__, __LINE__, true, "CFE_TBL_ReadHeaders", "*Not tested* Invalid processor ID ");
#endif
}

/*
** Test function executed when the contents of a table need to be validated
*/
int32 Test_CFE_TBL_ValidationFunc(void *TblPtr)
{
    return UT_DEFAULT_IMPL(Test_CFE_TBL_ValidationFunc);
}
