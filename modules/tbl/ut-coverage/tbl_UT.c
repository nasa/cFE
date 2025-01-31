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

/* Set up buffer to provide to CFE_ES_GetPoolBuf handler */
#define UT_TBL_LOAD_BUFFER_SIZE \
    (CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS * (CFE_PLATFORM_TBL_MAX_SNGL_TABLE_SIZE + sizeof(CFE_ES_PoolAlign_t)))
static union
{
    CFE_ES_PoolAlign_t Align;
    uint8              Bytes[UT_TBL_LOAD_BUFFER_SIZE];
} UT_TBL_LoadBuffer;

void * Tbl1Ptr = NULL;
void * Tbl2Ptr = NULL;
void **ArrayOfPtrsToTblPtrs[2];

/* Normal dispatching registers the MsgID+CC in order to follow a
 * certain path through a series of switch statements */
#define TBL_UT_MID_DISPATCH(intf) \
    .Method = UT_TaskPipeDispatchMethod_MSG_ID_CC, .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_TBL_##intf##_MID)

#define TBL_UT_MSG_DISPATCH(intf, cmd)       TBL_UT_MID_DISPATCH(intf), UT_TPD_SETSIZE(CFE_TBL_##cmd)
#define TBL_UT_CC_DISPATCH(intf, cc, cmd)    TBL_UT_MSG_DISPATCH(intf, cmd), UT_TPD_SETCC(cc)
#define TBL_UT_ERROR_DISPATCH(intf, cc, err) TBL_UT_MID_DISPATCH(intf), UT_TPD_SETCC(cc), UT_TPD_SETERR(err)

/* NOTE: Automatic formatting of this table tends to make it harder to read. */
/* clang-format off */
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TBL_MSG_HK =
    { TBL_UT_MSG_DISPATCH(SEND_HK, SendHkCmd) };
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TBL_CMD_NOOP_CC =
    { TBL_UT_CC_DISPATCH(CMD, CFE_TBL_NOOP_CC, NoopCmd) };
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TBL_CMD_RESET_COUNTERS_CC =
    { TBL_UT_CC_DISPATCH(CMD, CFE_TBL_RESET_COUNTERS_CC, ResetCountersCmd) };

static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TBL_INVALID_MID =
    { .Method = UT_TaskPipeDispatchMethod_MSG_ID_CC, UT_TPD_SETERR(CFE_STATUS_UNKNOWN_MSG_ID) };
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TBL_CMD_INVALID_CC =
    { TBL_UT_ERROR_DISPATCH(CMD, -1, CFE_STATUS_BAD_COMMAND_CODE) };
/* clang-format on */

CFE_TBL_RegistryRec_t Original[CFE_PLATFORM_TBL_MAX_NUM_TABLES];

/*
 * UT helper routines
 */
void UT_TBL_SetupHeader(CFE_TBL_File_Hdr_t *TblFileHeader, size_t Offset, size_t NumBytes)
{
    TblFileHeader->Offset   = Offset;
    TblFileHeader->NumBytes = NumBytes;

    if (UT_Endianess == UT_LITTLE_ENDIAN)
    {
        CFE_TBL_ByteSwapUint32(&TblFileHeader->Offset);
        CFE_TBL_ByteSwapUint32(&TblFileHeader->NumBytes);
    }
}

/* Sets up the indicated validation request/result buffer as VALIDATION_PENDING */
void UT_TBL_SetupPendingValidation(uint32 ArrayIndex, bool UseActive, CFE_TBL_RegistryRec_t *RegRecPtr,
                                   CFE_TBL_ValidationResult_t **ValResultOut)
{
    CFE_TBL_ValidationResult_t *ValResultPtr;
    CFE_ResourceId_t            PendingId;

    ValResultPtr = &CFE_TBL_Global.ValidationResults[ArrayIndex];
    PendingId    = CFE_ResourceId_FromInteger(CFE_TBL_VALRESULTID_BASE + ArrayIndex);

    memset(ValResultPtr, 0, sizeof(*ValResultPtr));

    ValResultPtr->State = CFE_TBL_VALIDATION_PENDING;

    ValResultPtr->ValId        = CFE_TBL_VALRESULTID_C(PendingId);
    ValResultPtr->ActiveBuffer = UseActive;

    snprintf(ValResultPtr->TableName, sizeof(ValResultPtr->TableName), "ut_cfe_tbl.UT_Table%u",
             (unsigned int)ArrayIndex + 1);

    if (RegRecPtr != NULL)
    {
        if (UseActive)
        {
            RegRecPtr->ValidateActiveId = ValResultPtr->ValId;
        }
        else
        {
            RegRecPtr->ValidateInactiveId = ValResultPtr->ValId;
        }
    }

    if (ValResultOut != NULL)
    {
        *ValResultOut = ValResultPtr;
    }
}

/* Resets the indicated validation request/result buffer to the free/unused state */
void UT_TBL_ResetValidationState(uint32 ArrayIndex)
{
    CFE_TBL_ValidationResult_t *ValResultPtr;
    ValResultPtr = &CFE_TBL_Global.ValidationResults[ArrayIndex];
    memset(ValResultPtr, 0, sizeof(*ValResultPtr));
}

void UT_TBL_SetupPendingDump(uint32 ArrayIndex, CFE_TBL_LoadBuff_t *DumpBufferPtr, CFE_TBL_RegistryRec_t *RegRecPtr,
                             CFE_TBL_DumpControl_t **DumpCtrlOut)
{
    CFE_TBL_DumpControl_t *DumpCtrlPtr;
    CFE_ResourceId_t       PendingId;

    DumpCtrlPtr = &CFE_TBL_Global.DumpControlBlocks[ArrayIndex];

    PendingId = CFE_ResourceId_FromInteger(CFE_TBL_DUMPCTRLID_BASE + ArrayIndex);

    memset(DumpCtrlPtr, 0, sizeof(*DumpCtrlPtr));

    DumpCtrlPtr->State         = CFE_TBL_DUMP_PENDING;
    DumpCtrlPtr->BlockId       = CFE_TBL_DUMPCTRLID_C(PendingId);
    DumpCtrlPtr->RegRecPtr     = RegRecPtr;
    DumpCtrlPtr->DumpBufferPtr = DumpBufferPtr;
    DumpCtrlPtr->Size          = 1;

    snprintf(DumpCtrlPtr->TableName, sizeof(DumpCtrlPtr->TableName), "ut_cfe_tbl.UT_Table%u",
             (unsigned int)ArrayIndex + 1);

    if (RegRecPtr != NULL)
    {
        RegRecPtr->DumpControlId = DumpCtrlPtr->BlockId;
    }

    if (DumpCtrlOut != NULL)
    {
        *DumpCtrlOut = DumpCtrlPtr;
    }
}

/* Resets the indicated dump control block to the free/unused state */
void UT_TBL_ResetDumpCtrlState(uint32 ArrayIndex)
{
    CFE_TBL_DumpControl_t *DumpCtrlPtr;
    DumpCtrlPtr = &CFE_TBL_Global.DumpControlBlocks[ArrayIndex];
    memset(DumpCtrlPtr, 0, sizeof(*DumpCtrlPtr));
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

    /*
     * Shared resource access patterns
     * (do this early because many other APIs depend on these working correctly)
     */
    UT_ADD_TEST(Test_CFE_TBL_ResourceID_ValidationResult);
    UT_ADD_TEST(Test_CFE_TBL_ResourceID_RegistryRecord);
    UT_ADD_TEST(Test_CFE_TBL_ResourceID_AccessDescriptor);
    UT_ADD_TEST(Test_CFE_TBL_ResourceID_DumpControl);

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
    UT_ADD_TEST(Test_CFE_TBL_SendHkCmd);

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
    UT_ADD_TEST(Test_CFE_TBL_DumpToBuffer);
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
        CFE_TBL_Global.Handles[i].AppId    = CFE_TBL_NOT_OWNED;
        CFE_TBL_Global.Handles[i].RegIndex = 0;
        CFE_TBL_HandleLinkInit(&CFE_TBL_Global.Handles[i].Link);
        CFE_TBL_Global.Handles[i].UsedFlag    = false;
        CFE_TBL_Global.Handles[i].LockFlag    = false;
        CFE_TBL_Global.Handles[i].Updated     = false;
        CFE_TBL_Global.Handles[i].BufferIndex = 0;
    }

    /* Initialize the table validation results records */
    for (i = 0; i < CFE_PLATFORM_TBL_MAX_NUM_VALIDATIONS; i++)
    {
        UT_TBL_ResetValidationState(i);
    }

    /* Initialize the dump-only table dump control blocks */
    for (i = 0; i < CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS; i++)
    {
        UT_TBL_ResetDumpCtrlState(i);

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
        CFE_TBL_NoopCmd_t          NoopCmd;
        CFE_TBL_SendHkCmd_t        SendHkCmd;
        CFE_TBL_ResetCountersCmd_t ResetCountersCmd;
        CFE_MSG_Message_t          Msg;
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
    UtAssert_VOIDCALL(CFE_TBL_TaskMain());
    UtAssert_INT32_EQ(ExitCode, CFE_ES_RunStatus_CORE_APP_RUNTIME_ERROR);
    UtAssert_STUB_COUNT(CFE_ES_ExitApp, 1);

    /* Main task initialization failure */
    UT_InitData();
    ExitCode = 0;
    UT_SetDataBuffer(UT_KEY(CFE_ES_ExitApp), &ExitCode, sizeof(ExitCode), false);
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_Register), 1, -1);
    UtAssert_VOIDCALL(CFE_TBL_TaskMain());
    UtAssert_INT32_EQ(ExitCode, CFE_ES_RunStatus_CORE_APP_INIT_ERROR);
    /* Since stub doesn't actually cause an exit, will get called twice */
    UtAssert_STUB_COUNT(CFE_ES_ExitApp, 2);

    /* Test successful table services core application initialization */
    UT_InitData();
    CFE_UtAssert_SUCCESS(CFE_TBL_TaskInit());

    /* Test table services core application initialization response to a pipe
     * creation failure
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_CreatePipe), 1, -2);
    UtAssert_INT32_EQ(CFE_TBL_TaskInit(), -2);

    /* Test table services core application initialization response to a
     * housekeeping request subscription error
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_Subscribe), 1, -3);
    UtAssert_INT32_EQ(CFE_TBL_TaskInit(), -3);

    /* Test table services core application initialization response to a
     * ground command subscription error
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_Subscribe), 2, -4);
    UtAssert_INT32_EQ(CFE_TBL_TaskInit(), -4);

    /* Test table services core application initialization response to a
     * send initialization event error
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_SendEvent), 1, -5);
    UtAssert_INT32_EQ(CFE_TBL_TaskInit(), -5);

    /* Test table services core application initialization response to an
     * EVS register failure
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_Register), 1, -6);
    UtAssert_INT32_EQ(CFE_TBL_TaskInit(), -6);

    /* Test command pipe messages handler response to a valid command */
    UT_InitData();
    UT_CallTaskPipe(CFE_TBL_TaskPipe, CFE_MSG_PTR(CmdBuf), sizeof(CmdBuf.NoopCmd), UT_TPID_CFE_TBL_CMD_NOOP_CC);
    CFE_UtAssert_EVENTSENT(CFE_TBL_NOOP_INF_EID);

    /* Test command pipe messages handler response to an invalid
     * message length
     */
    UT_InitData();
    UT_CallTaskPipe(CFE_TBL_TaskPipe, CFE_MSG_PTR(CmdBuf), sizeof(CmdBuf.NoopCmd) - 1, UT_TPID_CFE_TBL_CMD_NOOP_CC);
    CFE_UtAssert_EVENTSENT(CFE_TBL_LEN_ERR_EID);

    /* Test command pipe messages handler response to an invalid
     * command code
     */
    UT_InitData();
    UT_CallTaskPipe(CFE_TBL_TaskPipe, CFE_MSG_PTR(CmdBuf), sizeof(CmdBuf.NoopCmd), UT_TPID_CFE_TBL_CMD_INVALID_CC);
    CFE_UtAssert_EVENTSENT(CFE_TBL_CC1_ERR_EID);

    /* Test command pipe messages handler response to other errors */
    UT_InitData();
    CFE_TBL_Global.CommandCounter      = 0;
    CFE_TBL_Global.CommandErrorCounter = 0;
    UT_CallTaskPipe(CFE_TBL_TaskPipe, CFE_MSG_PTR(CmdBuf), sizeof(CmdBuf.NoopCmd), UT_TPID_CFE_TBL_INVALID_MID);
    CFE_UtAssert_EVENTSENT(CFE_TBL_MID_ERR_EID);
    UtAssert_ZERO(CFE_TBL_Global.CommandCounter);
    UtAssert_ZERO(CFE_TBL_Global.CommandErrorCounter);

    /* Test command pipe messages handler response to "message type" message */
    UT_InitData();
    CFE_TBL_Global.CommandCounter      = 0;
    CFE_TBL_Global.CommandErrorCounter = 0;
    UT_CallTaskPipe(CFE_TBL_TaskPipe, CFE_MSG_PTR(CmdBuf), sizeof(CmdBuf.SendHkCmd), UT_TPID_CFE_TBL_MSG_HK);
    UtAssert_ZERO(CFE_TBL_Global.CommandCounter);
    UtAssert_ZERO(CFE_TBL_Global.CommandErrorCounter);

    /* Test command pipe messages handler response to "command type" message */
    UT_InitData();
    UT_CallTaskPipe(CFE_TBL_TaskPipe, CFE_MSG_PTR(CmdBuf), sizeof(CmdBuf.ResetCountersCmd),
                    UT_TPID_CFE_TBL_CMD_RESET_COUNTERS_CC);
    CFE_UtAssert_EVENTSENT(CFE_TBL_RESET_INF_EID);
    UtAssert_ZERO(CFE_TBL_Global.CommandCounter);
    UtAssert_ZERO(CFE_TBL_Global.CommandErrorCounter);
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
    UtAssert_STUB_COUNT(CFE_MSG_Init, 3);
}

/*
** Test command handler table message ID (or command code) search function
*/
void Test_CFE_TBL_SearchCmdHndlrTbl(void)
{
    uint16         CmdCode;
    CFE_SB_MsgId_t MsgID;

    UtPrintf("Begin Test Search Command Handler Table");

    /* Test successfully finding a matching message ID and command code */
    UT_InitData();
    MsgID   = CFE_SB_ValueToMsgId(CFE_TBL_CMD_MID);
    CmdCode = CFE_TBL_NOOP_CC;
    UtAssert_INT32_EQ(CFE_TBL_SearchCmdHndlrTbl(MsgID, CmdCode), 1);

    /* Test using a message that is not a command message with specific
     * command code
     */
    UT_InitData();
    MsgID = CFE_SB_ValueToMsgId(CFE_TBL_SEND_HK_MID);
    UtAssert_INT32_EQ(CFE_TBL_SearchCmdHndlrTbl(MsgID, CmdCode), 0);

    /* Test with a message ID that matches but the command code does
     * not match
     */
    UT_InitData();
    MsgID   = CFE_SB_ValueToMsgId(CFE_TBL_CMD_MID);
    CmdCode = 0xffff;
    UtAssert_INT32_EQ(CFE_TBL_SearchCmdHndlrTbl(MsgID, CmdCode), CFE_TBL_BAD_CMD_CODE);

    /* Test with a message ID that does not match */
    UT_InitData();
    MsgID = CFE_SB_INVALID_MSG_ID;
    UtAssert_INT32_EQ(CFE_TBL_SearchCmdHndlrTbl(MsgID, CmdCode), CFE_TBL_BAD_MSG_ID);
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
    UtAssert_INT32_EQ(CFE_TBL_DeleteCDSCmd(&DelCDSCmd), CFE_TBL_INC_ERR_CTR);

    /* Test failure to find table in the critical table registry */
    UT_InitData();
    k = CFE_PLATFORM_TBL_MAX_CRITICAL_TABLES + CFE_PLATFORM_TBL_MAX_NUM_TABLES;

    for (j = CFE_PLATFORM_TBL_MAX_NUM_TABLES; j < k; j++)
    {
        snprintf(CFE_TBL_Global.CritReg[j - CFE_PLATFORM_TBL_MAX_NUM_TABLES].Name, CFE_TBL_MAX_FULL_NAME_LEN, "%d", j);
    }

    strncpy(DelCDSCmd.Payload.TableName, "-1", sizeof(DelCDSCmd.Payload.TableName) - 1);
    DelCDSCmd.Payload.TableName[sizeof(DelCDSCmd.Payload.TableName) - 1] = '\0';
    UtAssert_INT32_EQ(CFE_TBL_DeleteCDSCmd(&DelCDSCmd), CFE_TBL_INC_ERR_CTR);

    /* Test finding the table in the critical table registry, but CDS is not
     * tagged as a table
     */
    UT_InitData();
    snprintf(DelCDSCmd.Payload.TableName, sizeof(DelCDSCmd.Payload.TableName), "%d",
             CFE_PLATFORM_TBL_MAX_CRITICAL_TABLES + CFE_PLATFORM_TBL_MAX_NUM_TABLES - 1);
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_DeleteCDS), 1, CFE_ES_CDS_WRONG_TYPE_ERR);
    UtAssert_INT32_EQ(CFE_TBL_DeleteCDSCmd(&DelCDSCmd), CFE_TBL_INC_ERR_CTR);

    /* Test deletion when CDS owning application is still active */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_DeleteCDS), 1, CFE_ES_CDS_OWNER_ACTIVE_ERR);
    UtAssert_INT32_EQ(CFE_TBL_DeleteCDSCmd(&DelCDSCmd), CFE_TBL_INC_ERR_CTR);

    /* Test deletion where the table cannot be located in the CDS registry */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_DeleteCDS), 1, CFE_ES_ERR_NAME_NOT_FOUND);
    UtAssert_INT32_EQ(CFE_TBL_DeleteCDSCmd(&DelCDSCmd), CFE_TBL_INC_ERR_CTR);

    /* Test deletion error while deleting table from the CDS */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_DeleteCDS), 1, CFE_SUCCESS - 1);
    UtAssert_INT32_EQ(CFE_TBL_DeleteCDSCmd(&DelCDSCmd), CFE_TBL_INC_ERR_CTR);

    /* Test successful removal of the table from the CDS */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_DeleteCDS), 1, CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_TBL_DeleteCDSCmd(&DelCDSCmd), CFE_TBL_INC_CMD_CTR);
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
    UtAssert_INT32_EQ(CFE_TBL_SendRegistryCmd(&TlmRegCmd), CFE_TBL_INC_CMD_CTR);

    /* Test when table name does not exist */
    UT_InitData();

    snprintf(TlmRegCmd.Payload.TableName, sizeof(TlmRegCmd.Payload.TableName), "%d",
             CFE_PLATFORM_TBL_MAX_NUM_TABLES + 1);
    UtAssert_INT32_EQ(CFE_TBL_SendRegistryCmd(&TlmRegCmd), CFE_TBL_INC_ERR_CTR);
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
    UtAssert_INT32_EQ(CFE_TBL_AbortLoadCmd(&AbortLdCmd), CFE_TBL_INC_CMD_CTR);

    /* Test when table name does exist but no table load is in progress */
    UT_InitData();
    CFE_TBL_Global.Registry[0].LoadInProgress = CFE_TBL_NO_LOAD_IN_PROGRESS;
    UtAssert_INT32_EQ(CFE_TBL_AbortLoadCmd(&AbortLdCmd), CFE_TBL_INC_ERR_CTR);

    /* Test when table name does exist, a table load is in progress, and the
     * table is dump only
     */
    UT_InitData();
    CFE_TBL_Global.Registry[0].LoadInProgress = CFE_TBL_NO_LOAD_IN_PROGRESS + 1;
    CFE_TBL_Global.Registry[0].DumpOnly       = true;
    UtAssert_INT32_EQ(CFE_TBL_AbortLoadCmd(&AbortLdCmd), CFE_TBL_INC_ERR_CTR);

    /* Test when table name not found in the registry */
    UT_InitData();
    snprintf(AbortLdCmd.Payload.TableName, sizeof(AbortLdCmd.Payload.TableName), "%d",
             CFE_PLATFORM_TBL_MAX_NUM_TABLES + 1);
    UtAssert_INT32_EQ(CFE_TBL_AbortLoadCmd(&AbortLdCmd), CFE_TBL_INC_ERR_CTR);

    /* Test when table is double buffered */
    UT_InitData();
    CFE_TBL_Global.Registry[0].DoubleBuffered = true;
    CFE_TBL_Global.LoadBuffs[0].Taken         = true;
    CFE_TBL_AbortLoad(&CFE_TBL_Global.Registry[0]);
    UtAssert_BOOL_TRUE(CFE_TBL_Global.LoadBuffs[0].Taken);

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
    UtAssert_INT32_EQ(CFE_TBL_ActivateCmd(&ActivateCmd), CFE_TBL_INC_ERR_CTR);

    /* Test when table name exists, the table is not a dump-only, a load is in
     * progress, and the table is double-buffered
     */
    UT_InitData();
    CFE_TBL_Global.Registry[0].DumpOnly       = false;
    CFE_TBL_Global.Registry[0].LoadInProgress = CFE_TBL_NO_LOAD_IN_PROGRESS + 1;
    CFE_TBL_Global.Registry[0].DoubleBuffered = true;
    UtAssert_INT32_EQ(CFE_TBL_ActivateCmd(&ActivateCmd), CFE_TBL_INC_ERR_CTR);

    /* Test when table name exists, the table is not a dump-only, a load is in
     * progress, the table isn't double-buffered, and ValidationStatus = true
     */
    UT_InitData();
    CFE_TBL_Global.Registry[0].DoubleBuffered                                     = false;
    CFE_TBL_Global.LoadBuffs[CFE_TBL_Global.Registry[0].LoadInProgress].Validated = true;
    UtAssert_INT32_EQ(CFE_TBL_ActivateCmd(&ActivateCmd), CFE_TBL_INC_CMD_CTR);

    /* Test when table name exists, the table is not a dump-only, no load is in
     * progress, and no notification message should be sent
     */
    UT_InitData();
    CFE_TBL_Global.Registry[0].NotifyByMsg    = false;
    CFE_TBL_Global.Registry[0].LoadInProgress = CFE_TBL_NO_LOAD_IN_PROGRESS;
    UtAssert_INT32_EQ(CFE_TBL_ActivateCmd(&ActivateCmd), CFE_TBL_INC_ERR_CTR);

    /* Test when table name exists, the table is not a dump-only, no load in in
     * progress, and a notification message should be sent
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_TransmitMsg), 1, CFE_SB_INTERNAL_ERR);
    CFE_TBL_Global.Registry[0].NotifyByMsg    = true;
    CFE_TBL_Global.Registry[0].LoadInProgress = CFE_TBL_NO_LOAD_IN_PROGRESS + 1;
    UtAssert_INT32_EQ(CFE_TBL_ActivateCmd(&ActivateCmd), CFE_TBL_INC_CMD_CTR);

    /* Test when the table name doesn't exist */
    UT_InitData();
    snprintf(ActivateCmd.Payload.TableName, sizeof(ActivateCmd.Payload.TableName), "%d",
             CFE_PLATFORM_TBL_MAX_NUM_TABLES + 1);
    UtAssert_INT32_EQ(CFE_TBL_ActivateCmd(&ActivateCmd), CFE_TBL_INC_ERR_CTR);

    /* Restore original values */
    CFE_TBL_Global.Registry[0].LoadInProgress = load;
    CFE_TBL_Global.Registry[0].DumpOnly       = dump;
}

/*
** Test the write table data to a file function
*/
void Test_CFE_TBL_DumpToFile(void)
{
    size_t TblSizeInBytes = 9;

    UtPrintf("Begin Test Dump to File");

    /* Test with an error creating the dump file */
    UT_InitData();
    UT_SetDefaultReturnValue(UT_KEY(OS_OpenCreate), OS_ERROR);
    UtAssert_INT32_EQ(CFE_TBL_DumpToFile("filename", "tablename", "dumpaddress", TblSizeInBytes), CFE_TBL_INC_ERR_CTR);

    /* Test with an error writing the cFE file header */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_WriteHeader), 1, sizeof(CFE_FS_Header_t) - 1);
    UtAssert_INT32_EQ(CFE_TBL_DumpToFile("filename", "tablename", "dumpaddress", TblSizeInBytes), CFE_TBL_INC_ERR_CTR);

    /* Test with an error writing the table file header */
    UT_InitData();

    /* Set the count for the FSWriteHdrRtn return code variable to a large
     * enough value to pass through every time
     */
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_WriteHeader), 6, sizeof(CFE_FS_Header_t));
    UT_SetDeferredRetcode(UT_KEY(OS_write), 1, sizeof(CFE_TBL_File_Hdr_t) - 1);
    UtAssert_INT32_EQ(CFE_TBL_DumpToFile("filename", "tablename", "dumpaddress", TblSizeInBytes), CFE_TBL_INC_ERR_CTR);

    /* Test with an error writing the table to a file */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(OS_write), 2, TblSizeInBytes - 1);
    UtAssert_INT32_EQ(CFE_TBL_DumpToFile("filename", "tablename", "dumpaddress", TblSizeInBytes), CFE_TBL_INC_ERR_CTR);

    /* Test successful file creation and data dumped */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(OS_OpenCreate), 1, OS_ERROR);
    UtAssert_INT32_EQ(CFE_TBL_DumpToFile("filename", "tablename", "dumpaddress", TblSizeInBytes), CFE_TBL_INC_CMD_CTR);

    /* Test where file already exists so data is overwritten */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_DumpToFile("filename", "tablename", "dumpaddress", TblSizeInBytes), CFE_TBL_INC_CMD_CTR);
}

/*
** Test the processing reset counters command message function
*/
void Test_CFE_TBL_ResetCmd(void)
{
    UtPrintf("Begin Test Reset Command");

    /* Test run through function (there are no additional paths) */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_ResetCountersCmd(NULL), CFE_TBL_DONT_INC_CTR);
}

/*
** Test the validate table command message function
*/
void Test_CFE_TBL_ValidateCmd(void)
{
    uint8                     Buff;
    uint8 *                   BuffPtr = &Buff;
    CFE_TBL_ValidateCmd_t     ValidateCmd;
    CFE_TBL_CallbackFuncPtr_t ValFuncPtr = (CFE_TBL_CallbackFuncPtr_t)((unsigned long)&UT_InitializeTableRegistryNames);

    UtPrintf("Begin Test Validate Command");

    /* Test when table name is not found in the registry */
    UT_InitData();
    snprintf(ValidateCmd.Payload.TableName, sizeof(ValidateCmd.Payload.TableName), "%d",
             CFE_PLATFORM_TBL_MAX_NUM_TABLES + 1);
    UtAssert_INT32_EQ(CFE_TBL_ValidateCmd(&ValidateCmd), CFE_TBL_INC_ERR_CTR);

    /* Test where the active buffer has data, but too many table validations
     * have been requested
     */
    UT_InitData();
    strncpy(ValidateCmd.Payload.TableName, CFE_TBL_Global.Registry[0].Name, sizeof(ValidateCmd.Payload.TableName) - 1);
    ValidateCmd.Payload.TableName[sizeof(ValidateCmd.Payload.TableName) - 1] = '\0';
    ValidateCmd.Payload.ActiveTableFlag                                      = CFE_TBL_BufferSelect_ACTIVE;
    CFE_TBL_Global.Registry[0].Buffers[CFE_TBL_Global.Registry[0].ActiveBufferIndex].BufferPtr = BuffPtr;

    UT_SetDeferredRetcode(UT_KEY(CFE_ResourceId_FindNext), 1, -1);
    UtAssert_INT32_EQ(CFE_TBL_ValidateCmd(&ValidateCmd), CFE_TBL_INC_ERR_CTR);

    /* Test where the active buffer has data, but there is no validation
     * function pointer
     */
    UT_InitData();
    UT_TBL_ResetValidationState(0);
    CFE_TBL_Global.Registry[0].ValidationFuncPtr = NULL;
    UtAssert_INT32_EQ(CFE_TBL_ValidateCmd(&ValidateCmd), CFE_TBL_INC_CMD_CTR);

    /* Test where the active buffer has data, the validation function pointer
     * exists, and the active table flag is set
     */
    UT_InitData();
    UT_TBL_ResetValidationState(0);
    CFE_TBL_Global.Registry[0].ValidationFuncPtr = ValFuncPtr;
    ValidateCmd.Payload.ActiveTableFlag          = true;
    UtAssert_INT32_EQ(CFE_TBL_ValidateCmd(&ValidateCmd), CFE_TBL_INC_CMD_CTR);

    /* Test with the buffer inactive, the table is double-buffered, and the
     * validation function pointer exists
     */
    UT_InitData();
    UT_TBL_ResetValidationState(0);
    ValidateCmd.Payload.ActiveTableFlag       = CFE_TBL_BufferSelect_INACTIVE;
    CFE_TBL_Global.Registry[0].DoubleBuffered = true;
    CFE_TBL_Global.Registry[0].Buffers[1 - CFE_TBL_Global.Registry[0].ActiveBufferIndex].BufferPtr = BuffPtr;
    CFE_TBL_Global.Registry[0].ValidationFuncPtr                                                   = ValFuncPtr;
    UtAssert_INT32_EQ(CFE_TBL_ValidateCmd(&ValidateCmd), CFE_TBL_INC_CMD_CTR);

    /* Test with the buffer inactive, the table is single-buffered with a
     * load in progress, the validation function pointer exists, and no
     * notification message should be sent
     */
    UT_InitData();
    UT_TBL_ResetValidationState(0);
    CFE_TBL_Global.Registry[0].NotifyByMsg                                        = false;
    CFE_TBL_Global.Registry[0].DoubleBuffered                                     = false;
    CFE_TBL_Global.LoadBuffs[CFE_TBL_Global.Registry[0].LoadInProgress].BufferPtr = BuffPtr;
    CFE_TBL_Global.Registry[0].LoadInProgress                                     = CFE_TBL_NO_LOAD_IN_PROGRESS + 1;
    UtAssert_INT32_EQ(CFE_TBL_ValidateCmd(&ValidateCmd), CFE_TBL_INC_CMD_CTR);

    /* Test with the buffer inactive, the table is single-buffered with a
     * load in progress, the validation function pointer exists, and a
     * notification message should be sent
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_TransmitMsg), 1, CFE_SB_INTERNAL_ERR);
    UT_TBL_ResetValidationState(0);
    CFE_TBL_Global.Registry[0].NotifyByMsg                                        = true;
    CFE_TBL_Global.Registry[0].DoubleBuffered                                     = false;
    CFE_TBL_Global.LoadBuffs[CFE_TBL_Global.Registry[0].LoadInProgress].BufferPtr = BuffPtr;
    CFE_TBL_Global.Registry[0].LoadInProgress                                     = CFE_TBL_NO_LOAD_IN_PROGRESS + 1;
    UtAssert_INT32_EQ(CFE_TBL_ValidateCmd(&ValidateCmd), CFE_TBL_INC_CMD_CTR);

    /* Test where no inactive buffer is present (single-buffered table without
     * load in progress)
     */
    UT_InitData();
    CFE_TBL_Global.Registry[0].LoadInProgress = CFE_TBL_NO_LOAD_IN_PROGRESS;
    UtAssert_INT32_EQ(CFE_TBL_ValidateCmd(&ValidateCmd), CFE_TBL_INC_ERR_CTR);

    /* Test with an illegal buffer */
    UT_InitData();
    ValidateCmd.Payload.ActiveTableFlag = 0xffff;
    UtAssert_INT32_EQ(CFE_TBL_ValidateCmd(&ValidateCmd), CFE_TBL_INC_ERR_CTR);
}

/*
** Test the processing no-operation command message function
*/
void Test_CFE_TBL_NoopCmd(void)
{
    UtPrintf("Begin Test No-Op Command");

    /* Test run through function (there are no additional paths) */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_NoopCmd(NULL), CFE_TBL_INC_CMD_CTR);
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
    UtAssert_NOT_NULL(CFE_ES_MEMADDRESS_TO_PTR(CFE_TBL_Global.TblRegPacket.Payload.InactiveBufferAddr));

    /* Test using a single buffered table and the buffer is inactive */
    UT_InitData();
    CFE_TBL_Global.TblRegPacket.Payload.InactiveBufferAddr                  = CFE_ES_MEMADDRESS_C(0);
    CFE_TBL_Global.Registry[CFE_TBL_Global.HkTlmTblRegIndex].DoubleBuffered = false;
    CFE_TBL_Global.Registry[CFE_TBL_Global.HkTlmTblRegIndex].LoadInProgress = CFE_TBL_NO_LOAD_IN_PROGRESS + 1;
    CFE_TBL_GetTblRegData();
    UtAssert_NOT_NULL(CFE_ES_MEMADDRESS_TO_PTR(CFE_TBL_Global.TblRegPacket.Payload.InactiveBufferAddr));

    /* Test with no inactive buffer */
    UT_InitData();
    CFE_TBL_Global.TblRegPacket.Payload.InactiveBufferAddr                  = CFE_ES_MEMADDRESS_C(0);
    CFE_TBL_Global.Registry[CFE_TBL_Global.HkTlmTblRegIndex].LoadInProgress = CFE_TBL_NO_LOAD_IN_PROGRESS;
    CFE_TBL_GetTblRegData();
    UtAssert_NULL(CFE_ES_MEMADDRESS_TO_PTR(CFE_TBL_Global.TblRegPacket.Payload.InactiveBufferAddr));
}

/*
** Test the function that collects data and stores it in the housekeeping
** message
*/
void Test_CFE_TBL_GetHkData(void)
{
    int                         i;
    int32                       NumLoadPendingIndex = CFE_PLATFORM_TBL_MAX_NUM_TABLES - 1;
    int32                       FreeSharedBuffIndex = CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS - 1;
    int32                       ValTableIndex       = CFE_PLATFORM_TBL_MAX_NUM_VALIDATIONS - 1;
    CFE_ES_AppId_t              AppID;
    CFE_TBL_ValidationResult_t *ValResultPtr;

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
    UtAssert_UINT32_EQ(CFE_TBL_Global.HkPacket.Payload.NumLoadPending, 1);

    /* Test lowering the count of free shared buffers */
    UT_InitData();
    CFE_TBL_Global.LoadBuffs[FreeSharedBuffIndex].Taken = true;
    CFE_TBL_GetHkData();
    UtAssert_UINT32_EQ(CFE_TBL_Global.HkPacket.Payload.NumFreeSharedBufs, CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS - 1);

    /* Test making a ValPtr with result = CFE_SUCCESS */
    UT_InitData();
    CFE_TBL_Global.SuccessValCounter = 0;
    UT_TBL_SetupPendingValidation(ValTableIndex, false, NULL, &ValResultPtr);
    ValResultPtr->State = CFE_TBL_VALIDATION_PERFORMED;
    CFE_TBL_GetHkData();
    UtAssert_UINT32_EQ(CFE_TBL_Global.SuccessValCounter, 1);

    /* Test making a ValPtr without result = CFE_SUCCESS */
    UT_InitData();
    CFE_TBL_Global.FailedValCounter = 0;
    UT_TBL_SetupPendingValidation(ValTableIndex, false, NULL, &ValResultPtr);
    ValResultPtr->State  = CFE_TBL_VALIDATION_PERFORMED;
    ValResultPtr->Result = CFE_SUCCESS - 1;
    CFE_TBL_GetHkData();
    UtAssert_UINT32_EQ(CFE_TBL_Global.FailedValCounter, 1);

    /* Test with an invalid registry entry */
    UT_InitData();
    CFE_TBL_Global.Registry[CFE_TBL_Global.LastTblUpdated].OwnerAppId = CFE_TBL_NOT_OWNED;
    CFE_TBL_Global.HkPacket.Payload.LastUpdateTime.Seconds            = 19283;
    CFE_TBL_GetHkData();
    UtAssert_UINT32_EQ(CFE_TBL_Global.HkPacket.Payload.LastUpdateTime.Seconds, 19283);

    /* Test with invalid last valid table updated out of range (low) */
    UT_InitData();
    CFE_TBL_Global.LastTblUpdated                          = -1;
    CFE_TBL_Global.HkPacket.Payload.LastUpdateTime.Seconds = 12345;
    CFE_TBL_GetHkData();
    UtAssert_UINT32_EQ(CFE_TBL_Global.HkPacket.Payload.LastUpdateTime.Seconds, 12345);

    /* Test with invalid last valid table updated out of range (high) */
    UT_InitData();
    CFE_TBL_Global.LastTblUpdated                          = CFE_PLATFORM_TBL_MAX_NUM_TABLES;
    CFE_TBL_Global.HkPacket.Payload.LastUpdateTime.Seconds = 54321;
    CFE_TBL_GetHkData();
    UtAssert_UINT32_EQ(CFE_TBL_Global.HkPacket.Payload.LastUpdateTime.Seconds, 54321);
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

    /* Get the AppID being used for UT */
    CFE_ES_GetAppID(&AppID);

    UtPrintf("Begin Test Dump Register Command");

    for (q = 0; q < CFE_PLATFORM_TBL_MAX_NUM_TABLES; q++)
    {
        CFE_TBL_HandleLinkInit(&CFE_TBL_Global.Registry[q].AccessList);
    }

    /* Test command using the default dump file name (nominal path) */
    UT_InitData();
    UT_SetDefaultReturnValue(UT_KEY(CFE_FS_BackgroundFileDumpIsPending), false);
    strncpy(DumpRegCmd.Payload.DumpFilename, "X", sizeof(DumpRegCmd.Payload.DumpFilename) - 1);
    DumpRegCmd.Payload.DumpFilename[sizeof(DumpRegCmd.Payload.DumpFilename) - 1] = '\0';
    UtAssert_INT32_EQ(CFE_TBL_DumpRegistryCmd(&DumpRegCmd), CFE_TBL_INC_CMD_CTR);

    /* Test command with a bad file name */
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_ParseInputFileNameEx), 1, CFE_FS_INVALID_PATH);
    UtAssert_INT32_EQ(CFE_TBL_DumpRegistryCmd(&DumpRegCmd), CFE_TBL_INC_ERR_CTR);
    UT_ResetState(UT_KEY(CFE_FS_ParseInputFileNameEx));

    /* Test command with the dump file already pending (max requests pending) */
    UT_SetDefaultReturnValue(UT_KEY(CFE_FS_BackgroundFileDumpIsPending), true);
    UT_SetDefaultReturnValue(UT_KEY(CFE_FS_BackgroundFileDumpRequest), CFE_STATUS_REQUEST_ALREADY_PENDING);
    UtAssert_INT32_EQ(CFE_TBL_DumpRegistryCmd(&DumpRegCmd), CFE_TBL_INC_ERR_CTR);
    UT_ResetState(UT_KEY(CFE_FS_BackgroundFileDumpRequest));

    /* Test command with the dump file already pending (local) */
    UT_SetDefaultReturnValue(UT_KEY(CFE_FS_BackgroundFileDumpIsPending), false);
    UT_SetDefaultReturnValue(UT_KEY(CFE_FS_BackgroundFileDumpRequest), CFE_STATUS_REQUEST_ALREADY_PENDING);
    UtAssert_INT32_EQ(CFE_TBL_DumpRegistryCmd(&DumpRegCmd), CFE_TBL_INC_ERR_CTR);

    /* Check event generators */
    UT_ClearEventHistory();
    CFE_TBL_Global.RegDumpState.FileExisted = true;
    CFE_TBL_DumpRegistryEventHandler(&CFE_TBL_Global.RegDumpState, CFE_FS_FileWriteEvent_COMPLETE, CFE_SUCCESS, 10, 0,
                                     1000);
    CFE_UtAssert_EVENTSENT(CFE_TBL_OVERWRITE_REG_DUMP_INF_EID);

    UT_ClearEventHistory();
    CFE_TBL_Global.RegDumpState.FileExisted = false;
    CFE_TBL_DumpRegistryEventHandler(&CFE_TBL_Global.RegDumpState, CFE_FS_FileWriteEvent_COMPLETE, CFE_SUCCESS, 10, 0,
                                     1000);
    CFE_UtAssert_EVENTSENT(CFE_TBL_WRITE_REG_DUMP_INF_EID);

    UT_ClearEventHistory();
    CFE_TBL_DumpRegistryEventHandler(&CFE_TBL_Global.RegDumpState, CFE_FS_FileWriteEvent_RECORD_WRITE_ERROR,
                                     CFE_SUCCESS, 10, 10, 1000);
    CFE_UtAssert_EVENTSENT(CFE_TBL_WRITE_TBL_REG_ERR_EID);

    UT_ClearEventHistory();
    CFE_TBL_DumpRegistryEventHandler(&CFE_TBL_Global.RegDumpState, CFE_FS_FileWriteEvent_HEADER_WRITE_ERROR,
                                     CFE_SUCCESS, 10, 10, 1000);
    CFE_UtAssert_EVENTSENT(CFE_TBL_WRITE_CFE_HDR_ERR_EID);

    UT_ClearEventHistory();
    CFE_TBL_DumpRegistryEventHandler(&CFE_TBL_Global.RegDumpState, CFE_FS_FileWriteEvent_CREATE_ERROR, OS_ERROR, 10, 0,
                                     0);
    CFE_UtAssert_EVENTSENT(CFE_TBL_CREATING_DUMP_FILE_ERR_EID);

    UT_ClearEventHistory();
    CFE_TBL_DumpRegistryEventHandler(&CFE_TBL_Global.RegDumpState, CFE_FS_FileWriteEvent_UNDEFINED, OS_ERROR, 0, 0, 0);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test where the table is owned, the file doesn't already exist, and the
     * table is successfully dumped
     */
    UT_InitData();
    CFE_TBL_Global.Registry[0].OwnerAppId = AppID;
    CFE_TBL_HandleLinkInit(&CFE_TBL_Global.Registry[0].AccessList);
    CFE_TBL_Global.Registry[1].OwnerAppId     = CFE_TBL_NOT_OWNED;
    CFE_TBL_Global.Registry[0].LoadInProgress = CFE_TBL_NO_LOAD_IN_PROGRESS + 1;
    CFE_TBL_Global.Registry[0].DoubleBuffered = true;
    LocalBuf                                  = NULL;
    LocalSize                                 = 0;
    UtAssert_BOOL_FALSE(CFE_TBL_DumpRegistryGetter(&CFE_TBL_Global.RegDumpState, 0, &LocalBuf, &LocalSize));
    UtAssert_NOT_NULL(LocalBuf);
    UtAssert_NONZERO(LocalSize);

    /* Same but not double buffered */
    UT_InitData();
    CFE_TBL_Global.Registry[0].OwnerAppId = AppID;
    CFE_TBL_HandleLinkInit(&CFE_TBL_Global.Registry[0].AccessList);
    CFE_TBL_Global.Registry[1].OwnerAppId     = CFE_TBL_NOT_OWNED;
    CFE_TBL_Global.Registry[0].LoadInProgress = CFE_TBL_NO_LOAD_IN_PROGRESS + 1;
    CFE_TBL_Global.Registry[0].DoubleBuffered = false;
    LocalBuf                                  = NULL;
    LocalSize                                 = 0;
    UtAssert_BOOL_FALSE(CFE_TBL_DumpRegistryGetter(&CFE_TBL_Global.RegDumpState, 0, &LocalBuf, &LocalSize));
    UtAssert_NOT_NULL(LocalBuf);
    UtAssert_NONZERO(LocalSize);

    /* Hit last entry, no load in progress */
    CFE_TBL_Global.Registry[CFE_PLATFORM_TBL_MAX_NUM_TABLES - 1].OwnerAppId      = CFE_TBL_NOT_OWNED;
    CFE_TBL_Global.Registry[CFE_PLATFORM_TBL_MAX_NUM_TABLES - 1].AccessList.Next = 2;
    CFE_TBL_Global.Registry[CFE_PLATFORM_TBL_MAX_NUM_TABLES - 1].AccessList.Prev = 2;
    CFE_TBL_Global.Registry[CFE_PLATFORM_TBL_MAX_NUM_TABLES - 1].LoadInProgress  = CFE_TBL_NO_LOAD_IN_PROGRESS;
    CFE_TBL_HandleLinkInit(&CFE_TBL_Global.Handles[2].Link);
    LocalBuf  = NULL;
    LocalSize = 0;
    UtAssert_BOOL_TRUE(CFE_TBL_DumpRegistryGetter(&CFE_TBL_Global.RegDumpState, CFE_PLATFORM_TBL_MAX_NUM_TABLES - 1,
                                                  &LocalBuf, &LocalSize));
    UtAssert_NOT_NULL(LocalBuf);
    UtAssert_NONZERO(LocalSize);

    /* Test with record numb beyond EOF (should be ignored, return null) */
    UtAssert_BOOL_TRUE(CFE_TBL_DumpRegistryGetter(&CFE_TBL_Global.RegDumpState, CFE_PLATFORM_TBL_MAX_NUM_TABLES + 1,
                                                  &LocalBuf, &LocalSize));
    UtAssert_NULL(LocalBuf);
    UtAssert_ZERO(LocalSize);

    /* Test empty registry */
    CFE_TBL_Global.Registry[0].OwnerAppId = CFE_TBL_NOT_OWNED;
    CFE_TBL_HandleLinkInit(&CFE_TBL_Global.Registry[0].AccessList);
    LocalBuf  = NULL;
    LocalSize = 0;
    UtAssert_BOOL_FALSE(CFE_TBL_DumpRegistryGetter(&CFE_TBL_Global.RegDumpState, 0, &LocalBuf, &LocalSize));
    UtAssert_NULL(LocalBuf);
    UtAssert_ZERO(LocalSize);
}

/*
** Test the function that processes dump table to file command message
*/
void Test_CFE_TBL_DumpCmd(void)
{
    int                i, u;
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
    UtAssert_INT32_EQ(CFE_TBL_DumpCmd(&DumpCmd), CFE_TBL_INC_ERR_CTR);

    /* Test where the active buffer has data, but no dump control buffer is available
     */
    UT_InitData();
    strncpy(CFE_TBL_Global.Registry[2].Name, "DumpCmdTest", sizeof(CFE_TBL_Global.Registry[2].Name) - 1);
    strncpy(DumpCmd.Payload.TableName, CFE_TBL_Global.Registry[2].Name, sizeof(DumpCmd.Payload.TableName) - 1);
    CFE_TBL_Global.Registry[2].DumpOnly                              = true;
    DumpCmd.Payload.TableName[sizeof(DumpCmd.Payload.TableName) - 1] = '\0';
    DumpCmd.Payload.ActiveTableFlag                                  = CFE_TBL_BufferSelect_ACTIVE;
    CFE_TBL_Global.Registry[2].Buffers[CFE_TBL_Global.Registry[2].ActiveBufferIndex].BufferPtr = BuffPtr;

    UT_SetDeferredRetcode(UT_KEY(CFE_ResourceId_FindNext), 1, -1);
    UtAssert_INT32_EQ(CFE_TBL_DumpCmd(&DumpCmd), CFE_TBL_INC_ERR_CTR);

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

    UT_TBL_SetupPendingDump(2, NULL, NULL, NULL);
    UT_TBL_ResetDumpCtrlState(3);
    CFE_TBL_Global.Registry[2].DumpControlId                            = CFE_TBL_NO_DUMP_PENDING;
    CFE_TBL_Global.Registry[2].LoadInProgress                           = CFE_TBL_NO_LOAD_IN_PROGRESS + 1;
    CFE_TBL_Global.Registry[2].DoubleBuffered                           = false;
    CFE_TBL_Global.LoadBuffs[CFE_TBL_Global.Registry[2].LoadInProgress] = Load;
    CFE_TBL_Global.Registry[2].NotifyByMsg                              = true;
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_TransmitMsg), 1, CFE_SB_INTERNAL_ERR);
    UtAssert_INT32_EQ(CFE_TBL_DumpCmd(&DumpCmd), CFE_TBL_INC_CMD_CTR);

    /* Test with an active buffer, a pointer is created, the table is dump
     * only, no dump is already progress, and fails to get a working buffer;
     * no load in progress, TableLoadedOnce = true, single-buffered, no buffer
     * available
     */
    UT_InitData();
    UT_TBL_ResetDumpCtrlState(2);
    CFE_TBL_Global.Registry[2].DumpControlId   = CFE_TBL_NO_DUMP_PENDING;
    CFE_TBL_Global.Registry[2].LoadInProgress  = CFE_TBL_NO_LOAD_IN_PROGRESS;
    CFE_TBL_Global.Registry[2].TableLoadedOnce = true;
    CFE_TBL_Global.Registry[2].DoubleBuffered  = false;

    for (u = 0; u < CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS; u++)
    {
        CFE_TBL_Global.LoadBuffs[u].Taken = true;
    }

    CFE_TBL_Global.Registry[2].NotifyByMsg = true;
    UtAssert_INT32_EQ(CFE_TBL_DumpCmd(&DumpCmd), CFE_TBL_INC_ERR_CTR);

    /* Test with an active buffer, a pointer is created, the table is dump
     * only, and no dump fails to find a free dump control block; too many
     * dump only table dumps have been requested
     */
    UT_InitData();
    CFE_TBL_Global.Registry[2].DumpControlId = CFE_TBL_NO_DUMP_PENDING;
    CFE_TBL_Global.Registry[2].NotifyByMsg   = true;
    UT_SetDeferredRetcode(UT_KEY(CFE_ResourceId_FindNext), 1, 0);
    UtAssert_INT32_EQ(CFE_TBL_DumpCmd(&DumpCmd), CFE_TBL_INC_ERR_CTR);

    /* Test with an inactive buffer, double-buffered, dump already in progress;
     * dump is already pending
     */
    UT_InitData();
    DumpCmd.Payload.ActiveTableFlag           = CFE_TBL_BufferSelect_INACTIVE;
    CFE_TBL_Global.Registry[2].DoubleBuffered = true;
    CFE_TBL_Global.Registry[2].Buffers[(1 - CFE_TBL_Global.Registry[2].ActiveBufferIndex)].BufferPtr = BuffPtr;
    CFE_TBL_Global.Registry[2].DumpControlId = CFE_TBL_DUMPCTRLID_C(CFE_ResourceId_FromInteger(1));
    UtAssert_INT32_EQ(CFE_TBL_DumpCmd(&DumpCmd), CFE_TBL_INC_ERR_CTR);

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
    UtAssert_INT32_EQ(CFE_TBL_DumpCmd(&DumpCmd), CFE_TBL_INC_CMD_CTR);

    /* Test with an inactive buffer, single-buffered: No inactive buffer for
     * table due to load in progress
     */
    UT_InitData();
    CFE_TBL_Global.Registry[2].LoadInProgress = CFE_TBL_NO_LOAD_IN_PROGRESS;
    UtAssert_INT32_EQ(CFE_TBL_DumpCmd(&DumpCmd), CFE_TBL_INC_ERR_CTR);

    /* Test with an inactive buffer, single-buffered: No inactive buffer for
     * table due to user defined address
     */
    UT_InitData();
    CFE_TBL_Global.Registry[2].LoadInProgress = CFE_TBL_NO_LOAD_IN_PROGRESS + 1;
    CFE_TBL_Global.Registry[2].UserDefAddr    = true;
    UtAssert_INT32_EQ(CFE_TBL_DumpCmd(&DumpCmd), CFE_TBL_INC_ERR_CTR);

    /* Test with an illegal buffer parameter */
    UT_InitData();
    DumpCmd.Payload.ActiveTableFlag = CFE_TBL_BufferSelect_ACTIVE + 1;
    UtAssert_INT32_EQ(CFE_TBL_DumpCmd(&DumpCmd), CFE_TBL_INC_ERR_CTR);
}

/*
** Test the function that processes load table file to buffer command
** message
*/
void Test_CFE_TBL_LoadCmd(void)
{
    int                i;
    CFE_TBL_File_Hdr_t TblFileHeader;
    CFE_FS_Header_t    StdFileHeader;
    uint8              LoadBuffer[sizeof(UT_Table1_t)];
    CFE_TBL_LoadCmd_t  LoadCmd;
    CFE_ES_AppId_t     AppID;

    CFE_ES_GetAppID(&AppID);

    UtPrintf("Begin Test Load Command");

    /* Clear all local structures */
    memset(&TblFileHeader, 0, sizeof(TblFileHeader));
    memset(&StdFileHeader, 0, sizeof(StdFileHeader));
    memset(&LoadCmd, 0, sizeof(LoadCmd));

    /* Start with a cleared global (no tables registered) */
    memset(&CFE_TBL_Global, 0, sizeof(CFE_TBL_Global));

    /* Set up the headers */
    strncpy(StdFileHeader.Description, "FS header description", sizeof(StdFileHeader.Description) - 1);
    StdFileHeader.Description[sizeof(StdFileHeader.Description) - 1] = '\0';
    StdFileHeader.ContentType                                        = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType                                            = CFE_FS_SubType_TBL_IMG;
    StdFileHeader.SpacecraftID                                       = CFE_PLATFORM_TBL_VALID_SCID_1;
    StdFileHeader.ProcessorID                                        = CFE_PLATFORM_TBL_VALID_PRID_1;

    /* Test response to inability to open file */
    UT_InitData();
    UT_SetDefaultReturnValue(UT_KEY(OS_OpenCreate), OS_ERROR);
    UtAssert_INT32_EQ(CFE_TBL_LoadCmd(&LoadCmd), CFE_TBL_INC_ERR_CTR);

    /* Test response to inability to find the table in the registry */
    UT_InitData();
    strncpy(TblFileHeader.TableName, CFE_TBL_Global.Registry[0].Name, sizeof(TblFileHeader.TableName) - 1);
    TblFileHeader.TableName[sizeof(TblFileHeader.TableName) - 1] = '\0';
    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UtAssert_INT32_EQ(CFE_TBL_LoadCmd(&LoadCmd), CFE_TBL_INC_ERR_CTR);

    /* The rest of the tests will use registry 0, note empty name matches */
    CFE_TBL_Global.Registry[0].OwnerAppId = AppID;

    /* Test attempt to load a dump only table */
    UT_InitData();
    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    CFE_TBL_Global.Registry[0].DumpOnly = true;
    UtAssert_INT32_EQ(CFE_TBL_LoadCmd(&LoadCmd), CFE_TBL_INC_ERR_CTR);
    CFE_TBL_Global.Registry[0].DumpOnly = false;

    /* Test attempt to load a table with a load already pending */
    UT_InitData();
    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    CFE_TBL_Global.Registry[0].LoadPending = true;
    UtAssert_INT32_EQ(CFE_TBL_LoadCmd(&LoadCmd), CFE_TBL_INC_ERR_CTR);
    CFE_TBL_Global.Registry[0].LoadPending = false;

    /* Test where the file isn't dump only and passes table checks, get a
     * working buffer, and there is an extra byte (more data than header
     * indicates)
     * Works because OS_read stub assumes success (returns dummy extra byte even though it isn't in the buffer)
     */
    UT_InitData();
    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(UT_Table1_t));
    CFE_TBL_Global.Registry[0].TableLoadedOnce                                    = true;
    CFE_TBL_Global.Registry[0].Size                                               = sizeof(UT_Table1_t);
    CFE_TBL_Global.Registry[0].LoadInProgress                                     = CFE_TBL_NO_LOAD_IN_PROGRESS + 1;
    CFE_TBL_Global.LoadBuffs[CFE_TBL_Global.Registry[0].LoadInProgress].BufferPtr = &LoadBuffer;
    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UtAssert_INT32_EQ(CFE_TBL_LoadCmd(&LoadCmd), CFE_TBL_INC_ERR_CTR);

    /* Test with no extra byte => successful load */
    UT_InitData();
    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(UT_Table1_t));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, 0);
    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UtAssert_INT32_EQ(CFE_TBL_LoadCmd(&LoadCmd), CFE_TBL_INC_CMD_CTR);

    /* Test with differing amount of data from header's claim */
    UT_InitData();
    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(UT_Table1_t));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 2, 0);
    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UtAssert_INT32_EQ(CFE_TBL_LoadCmd(&LoadCmd), CFE_TBL_INC_ERR_CTR);

    /* Test with no working buffers available */
    UT_InitData();
    CFE_TBL_Global.Registry[0].LoadInProgress  = CFE_TBL_NO_LOAD_IN_PROGRESS;
    CFE_TBL_Global.Registry[0].TableLoadedOnce = true;
    CFE_TBL_Global.Registry[0].DoubleBuffered  = false;

    for (i = 0; i < CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS; i++)
    {
        CFE_TBL_Global.LoadBuffs[i].Taken = true;
    }

    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UtAssert_INT32_EQ(CFE_TBL_LoadCmd(&LoadCmd), CFE_TBL_INC_ERR_CTR);

    /* Test with interal CFE_TBL_GetWorkingBuffer error (memcpy with matching address */
    UT_InitData();
    CFE_TBL_Global.Registry[0].LoadInProgress                                        = CFE_TBL_NO_LOAD_IN_PROGRESS;
    CFE_TBL_Global.Registry[0].TableLoadedOnce                                       = true;
    CFE_TBL_Global.Registry[0].DoubleBuffered                                        = false;
    CFE_TBL_Global.Registry[0].Buffers[CFE_TBL_Global.Registry[0].ActiveBufferIndex] = CFE_TBL_Global.LoadBuffs[0];
    CFE_TBL_Global.LoadBuffs[0].Taken                                                = false;
    strncpy(TblFileHeader.TableName, CFE_TBL_Global.Registry[0].Name, sizeof(TblFileHeader.TableName) - 1);
    TblFileHeader.TableName[sizeof(TblFileHeader.TableName) - 1] = '\0';
    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UtAssert_INT32_EQ(CFE_TBL_LoadCmd(&LoadCmd), CFE_TBL_INC_ERR_CTR);

    /* Test with table header indicating data beyond size of the table */
    UT_InitData();
    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(UT_Table1_t));
    CFE_TBL_Global.Registry[0].Size = sizeof(UT_Table1_t) - 1;
    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UtAssert_INT32_EQ(CFE_TBL_LoadCmd(&LoadCmd), CFE_TBL_INC_ERR_CTR);

    /* Test with table header indicating no data in the file */
    UT_InitData();
    UT_TBL_SetupHeader(&TblFileHeader, 0, 0);
    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UtAssert_INT32_EQ(CFE_TBL_LoadCmd(&LoadCmd), CFE_TBL_INC_ERR_CTR);

    /* Test where file has partial load for uninitialized table and offset
     * is non-zero
     */
    UT_InitData();
    UT_TBL_SetupHeader(&TblFileHeader, 1, 1);
    CFE_TBL_Global.Registry[0].TableLoadedOnce = false;
    CFE_TBL_Global.Registry[0].Size            = sizeof(UT_Table1_t);
    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UtAssert_INT32_EQ(CFE_TBL_LoadCmd(&LoadCmd), CFE_TBL_INC_ERR_CTR);

    /* Test where file has partial load for uninitialized table and offset
     * is zero
     */
    UT_InitData();
    UT_TBL_SetupHeader(&TblFileHeader, 0, 1);
    CFE_TBL_Global.Registry[0].TableLoadedOnce = false;
    CFE_TBL_Global.Registry[0].Size            = sizeof(UT_Table1_t);
    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UtAssert_INT32_EQ(CFE_TBL_LoadCmd(&LoadCmd), CFE_TBL_INC_ERR_CTR);

    /* Test response to inability to read the file header */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_ReadHeader), 1, sizeof(CFE_FS_Header_t) - 1);
    UtAssert_INT32_EQ(CFE_TBL_LoadCmd(&LoadCmd), CFE_TBL_INC_ERR_CTR);
}

/*
** Test the function that processes housekeeping request message
*/
void Test_CFE_TBL_SendHkCmd(void)
{
    int                    i;
    CFE_TBL_LoadBuff_t     DumpBuff;
    CFE_TBL_LoadBuff_t *   DumpBuffPtr = &DumpBuff;
    CFE_TBL_RegistryRec_t *RegRecPtr;
    uint8                  Buff;
    void *                 BuffPtr    = &Buff;
    int32                  LoadInProg = 0;
    CFE_TBL_DumpControl_t *DumpCtrlPtr;

    UtPrintf("Begin Test Housekeeping Command");

    /* Test response to inability to update timestamp in dump file + inability
     * to send Hk packet
     */
    UT_InitData();
    RegRecPtr = &CFE_TBL_Global.Registry[0];
    UT_TBL_SetupPendingDump(0, DumpBuffPtr, RegRecPtr, &DumpCtrlPtr);
    LoadInProg                = CFE_TBL_NO_LOAD_IN_PROGRESS + 1;
    RegRecPtr->LoadInProgress = LoadInProg;
    DumpBuffPtr->Taken        = true;
    DumpBuffPtr->Validated    = true;
    DumpBuffPtr->BufferPtr    = BuffPtr;
    DumpBuffPtr->FileTime     = CFE_TIME_ZERO_VALUE;
    strncpy(DumpBuffPtr->DataSource, "hkSource", sizeof(DumpBuffPtr->DataSource) - 1);
    DumpBuffPtr->DataSource[sizeof(DumpBuffPtr->DataSource) - 1] = '\0';
    DumpCtrlPtr->State                                           = CFE_TBL_DUMP_PERFORMED;

    for (i = 1; i < CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS; i++)
    {
        UT_TBL_SetupPendingDump(i, NULL, NULL, NULL);
    }

    UT_SetDeferredRetcode(UT_KEY(CFE_SB_TransmitMsg), 1, CFE_SUCCESS - 1);
    CFE_TBL_Global.HkTlmTblRegIndex = CFE_TBL_NOT_FOUND + 1;
    UtAssert_INT32_EQ(CFE_TBL_SendHkCmd(NULL), CFE_TBL_DONT_INC_CTR);

    for (i = 1; i < CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS; i++)
    {
        UT_TBL_SetupPendingDump(i, NULL, NULL, NULL);
    }

    RegRecPtr->LoadInProgress = LoadInProg;

    /* Test response to inability to open dump file */
    UT_InitData();
    UT_TBL_SetupPendingDump(0, DumpBuffPtr, RegRecPtr, &DumpCtrlPtr);
    DumpCtrlPtr->State              = CFE_TBL_DUMP_PERFORMED;
    CFE_TBL_Global.HkTlmTblRegIndex = CFE_TBL_NOT_FOUND + 1;
    UT_SetDefaultReturnValue(UT_KEY(OS_OpenCreate), OS_ERROR);
    UtAssert_INT32_EQ(CFE_TBL_SendHkCmd(NULL), CFE_TBL_DONT_INC_CTR);

    /* Test response to an invalid table and a dump file create failure */
    UT_InitData();
    UT_TBL_SetupPendingDump(0, DumpBuffPtr, RegRecPtr, &DumpCtrlPtr);
    CFE_TBL_Global.HkTlmTblRegIndex = CFE_TBL_NOT_FOUND;
    DumpCtrlPtr->State              = CFE_TBL_DUMP_PERFORMED;
    UT_SetDefaultReturnValue(UT_KEY(OS_OpenCreate), OS_ERROR);
    UtAssert_INT32_EQ(CFE_TBL_SendHkCmd(NULL), CFE_TBL_DONT_INC_CTR);

    /* Test response to a file time stamp failure */
    UT_InitData();
    UT_TBL_SetupPendingDump(0, DumpBuffPtr, RegRecPtr, &DumpCtrlPtr);
    DumpCtrlPtr->State = CFE_TBL_DUMP_PERFORMED;
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_SetTimestamp), 1, OS_SUCCESS - 1);
    UtAssert_INT32_EQ(CFE_TBL_SendHkCmd(NULL), CFE_TBL_DONT_INC_CTR);

    /* Test response to OS_OpenCreate failure */
    UT_InitData();
    UT_TBL_SetupPendingDump(0, DumpBuffPtr, RegRecPtr, &DumpCtrlPtr);
    DumpCtrlPtr->State = CFE_TBL_DUMP_PERFORMED;
    UT_SetDeferredRetcode(UT_KEY(OS_OpenCreate), 3, -1);
    UtAssert_INT32_EQ(CFE_TBL_SendHkCmd(NULL), CFE_TBL_DONT_INC_CTR);
}

/*
** Function to prepare for test table API functions
*/
void Test_CFE_TBL_ApiInit(void)
{
    UT_ResetCDS();

    /* Provide a big enough pool for the load buffers */
    UT_ResetState(UT_KEY(CFE_ES_GetPoolBuf));
    UT_SetDataBuffer(UT_KEY(CFE_ES_GetPoolBuf), &UT_TBL_LoadBuffer, sizeof(UT_TBL_LoadBuffer), false);

    CFE_TBL_EarlyInit();
    CFE_TBL_Global.TableTaskAppId = UT_TBL_APPID_10;
}

/*
** Test function that registers a table with cFE to obtain table management
** services
*/
void Test_CFE_TBL_Register(void)
{
    CFE_TBL_Handle_t            TblHandle1;
    CFE_TBL_Handle_t            TblHandle2;
    CFE_TBL_Handle_t            TblHandle3;
    char                        TblName[CFE_MISSION_TBL_MAX_NAME_LENGTH + 2];
    int16                       i;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr;
    CFE_TBL_RegistryRec_t *     RegRecPtr;

    UtPrintf("Begin Test Register");

    /* Test response to a null table handle and null table name */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_Register(NULL, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_DEFAULT, NULL),
                      CFE_TBL_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle1, NULL, sizeof(UT_Table1_t), CFE_TBL_OPT_DEFAULT, NULL),
                      CFE_TBL_BAD_ARGUMENT);

    /* Test response to an invalid application ID */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetAppID), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_DEFAULT, NULL),
                      CFE_ES_ERR_RESOURCEID_NOT_VALID);
    CFE_UtAssert_EVENTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test response to a table name longer than the maximum allowed */
    UT_InitData();

    /* Create a table name longer than allowed */
    for (i = 0; i < CFE_MISSION_TBL_MAX_NAME_LENGTH + 1; i++)
    {
        TblName[i] = 'A';
    }

    TblName[i] = '\0';
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle1, TblName, sizeof(UT_Table1_t), CFE_TBL_OPT_DEFAULT, NULL),
                      CFE_TBL_ERR_INVALID_NAME);
    CFE_UtAssert_EVENTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test response to a table name shorter than the minimum allowed */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle1, "", sizeof(UT_Table1_t), CFE_TBL_OPT_DEFAULT, NULL),
                      CFE_TBL_ERR_INVALID_NAME);
    CFE_UtAssert_EVENTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /*  Test response to a table size of zero */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle1, "UT_Table1", 0, CFE_TBL_OPT_DEFAULT, NULL),
                      CFE_TBL_ERR_INVALID_SIZE);
    CFE_UtAssert_EVENTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test response to a table size larger than the maximum allowed */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle1, "UT_Table1", (CFE_PLATFORM_TBL_MAX_SNGL_TABLE_SIZE + 1),
                                       CFE_TBL_OPT_DEFAULT, NULL),
                      CFE_TBL_ERR_INVALID_SIZE);
    CFE_UtAssert_EVENTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test response at single size limit */
    UT_ClearEventHistory();
    CFE_UtAssert_SUCCESS(
        CFE_TBL_Register(&TblHandle1, "UT_Table1", CFE_PLATFORM_TBL_MAX_SNGL_TABLE_SIZE, CFE_TBL_OPT_DEFAULT, NULL));
    CFE_UtAssert_EVENTNOTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_SUCCESS(CFE_TBL_Unregister(TblHandle1));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test response to a double-buffered table size larger than the
     * maximum allowed
     */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle1, "UT_Table1", (CFE_PLATFORM_TBL_MAX_DBL_TABLE_SIZE + 1),
                                       CFE_TBL_OPT_DBL_BUFFER, NULL),
                      CFE_TBL_ERR_INVALID_SIZE);
    CFE_UtAssert_EVENTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test response at double size limit */
    UT_ClearEventHistory();
    CFE_UtAssert_SUCCESS(
        CFE_TBL_Register(&TblHandle1, "UT_Table1", CFE_PLATFORM_TBL_MAX_DBL_TABLE_SIZE, CFE_TBL_OPT_DBL_BUFFER, NULL));
    CFE_UtAssert_EVENTNOTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_SUCCESS(CFE_TBL_Unregister(TblHandle1));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test response to an invalid table option combination
     * (CFE_TBL_OPT_USR_DEF_ADDR | CFE_TBL_OPT_DBL_BUFFER)
     */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t),
                                       ((CFE_TBL_OPT_USR_DEF_ADDR & ~CFE_TBL_OPT_LD_DMP_MSK) | CFE_TBL_OPT_DBL_BUFFER),
                                       NULL),
                      CFE_TBL_ERR_INVALID_OPTIONS);
    CFE_UtAssert_EVENTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test response to an invalid table option combination
     * (CFE_TBL_OPT_USR_DEF_ADDR | CFE_TBL_OPT_LOAD_DUMP)
     */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t),
                                       (CFE_TBL_OPT_USR_DEF_ADDR & ~CFE_TBL_OPT_LD_DMP_MSK), NULL),
                      CFE_TBL_ERR_INVALID_OPTIONS);
    CFE_UtAssert_EVENTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test response to an invalid table option combination
     * (CFE_TBL_OPT_USR_DEF_ADDR | CFE_TBL_OPT_CRITICAL)
     */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t),
                                       ((CFE_TBL_OPT_USR_DEF_ADDR & ~CFE_TBL_OPT_LD_DMP_MSK) | CFE_TBL_OPT_CRITICAL),
                                       NULL),
                      CFE_TBL_ERR_INVALID_OPTIONS);
    CFE_UtAssert_EVENTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test response to an invalid table option combination
     * (CFE_TBL_OPT_DUMP_ONLY | CFE_TBL_OPT_DBL_BUFFER)
     */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t),
                                       (CFE_TBL_OPT_DUMP_ONLY | CFE_TBL_OPT_DBL_BUFFER), NULL),
                      CFE_TBL_ERR_INVALID_OPTIONS);
    CFE_UtAssert_EVENTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test response to a memory handle error */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetPoolBuf), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_DEFAULT, NULL),
                      CFE_ES_ERR_RESOURCEID_NOT_VALID);
    CFE_UtAssert_EVENTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test response to a memory block size error */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetPoolBuf), 1, CFE_ES_ERR_MEM_BLOCK_SIZE);
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_DEFAULT, NULL),
                      CFE_ES_ERR_MEM_BLOCK_SIZE);
    CFE_UtAssert_EVENTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test response to a memory block size error (for a second buffer) */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetPoolBuf), 2, CFE_ES_ERR_MEM_BLOCK_SIZE);
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_DBL_BUFFER, NULL),
                      CFE_ES_ERR_MEM_BLOCK_SIZE);
    CFE_UtAssert_EVENTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test successfully getting a double buffered table */
    UT_InitData();
    CFE_UtAssert_SUCCESS(CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_DBL_BUFFER, NULL));
    CFE_UtAssert_EVENTNOTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test attempt to register table owned by another application */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_2);
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle3, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_DBL_BUFFER, NULL),
                      CFE_TBL_ERR_DUPLICATE_NOT_OWNED);
    CFE_UtAssert_EVENTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test attempt to register existing table with a different size */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle3, "UT_Table1", sizeof(UT_Table2_t), CFE_TBL_OPT_DBL_BUFFER, NULL),
                      CFE_TBL_ERR_DUPLICATE_DIFF_SIZE);
    CFE_UtAssert_EVENTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test attempt to register a table with the same size and name */
    /* a. Test setup */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_2);
    CFE_UtAssert_SUCCESS(CFE_TBL_Share(&TblHandle3, "ut_cfe_tbl.UT_Table1"));
    CFE_UtAssert_EVENTCOUNT(0);

    /* b. Perform test */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1); /* Restore AppID to proper value */
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle2, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_DBL_BUFFER, NULL),
                      CFE_TBL_WARN_DUPLICATE);
    CFE_UtAssert_EVENTCOUNT(0);
    UtAssert_INT32_EQ(TblHandle1, TblHandle2);

    /* c. Test cleanup: unregister tables */
    UT_ClearEventHistory();
    UT_SetAppID(UT_TBL_APPID_1);
    CFE_UtAssert_SUCCESS(CFE_TBL_Unregister(TblHandle2));
    UT_SetAppID(UT_TBL_APPID_2);
    CFE_UtAssert_SUCCESS(CFE_TBL_Unregister(TblHandle3));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test registering a single buffered table */
    /* a. Perform test */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    CFE_UtAssert_SUCCESS(CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_DEFAULT, NULL));
    CFE_UtAssert_EVENTNOTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(0);

    /* b. Test cleanup: unregister table */
    UT_ClearEventHistory();
    CFE_UtAssert_SUCCESS(CFE_TBL_Unregister(TblHandle1));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test registering a single buffered dump-only table */
    /* a. Perform test */
    UT_InitData();
    CFE_UtAssert_SUCCESS(CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t),
                                          (CFE_TBL_OPT_SNGL_BUFFER | CFE_TBL_OPT_DUMP_ONLY), NULL));
    CFE_UtAssert_EVENTNOTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(0);

    /* b. Test cleanup: unregister table */
    UT_ClearEventHistory();
    CFE_UtAssert_SUCCESS(CFE_TBL_Unregister(TblHandle1));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test registering a user defined address table */
    /* a. Perform test */
    UT_InitData();
    CFE_UtAssert_SUCCESS(
        CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_USR_DEF_ADDR, NULL));
    CFE_UtAssert_EVENTNOTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(0);

    /* b. Test cleanup: unregister table */
    UT_ClearEventHistory();
    CFE_UtAssert_SUCCESS(CFE_TBL_Unregister(TblHandle1));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test registering a critical table */
    /* a. Perform test */
    UT_InitData();
    CFE_UtAssert_SUCCESS(CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_CRITICAL, NULL));
    CFE_UtAssert_EVENTNOTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(0);

    /* b. Test cleanup: unregister table */
    UT_ClearEventHistory();
    CFE_UtAssert_SUCCESS(CFE_TBL_Unregister(TblHandle1));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test registering a critical table that already has an allocated CDS */
    /* a. Perform test */
    UT_ClearEventHistory();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RegisterCDSEx), 1, CFE_ES_CDS_ALREADY_EXISTS);
    CFE_TBL_Global.CritReg[0].TableLoadedOnce = true;
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_CRITICAL, NULL),
                      CFE_TBL_INFO_RECOVERED_TBL);
    CFE_UtAssert_EVENTNOTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(0);

    /* b. Test cleanup: unregister table */
    UT_ClearEventHistory();
    CFE_UtAssert_SUCCESS(CFE_TBL_Unregister(TblHandle1));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test registering a critical table that already has an allocated CDS
     * and recovery fails
     */
    /* a. Perform test */
    UT_ClearEventHistory();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RegisterCDSEx), 1, CFE_ES_CDS_ALREADY_EXISTS);
    CFE_TBL_Global.CritReg[0].TableLoadedOnce = false;
    CFE_UtAssert_SUCCESS(CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_CRITICAL, NULL));
    CFE_UtAssert_EVENTNOTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(0);

    /* b. Test cleanup: unregister table */
    UT_ClearEventHistory();
    CFE_UtAssert_SUCCESS(CFE_TBL_Unregister(TblHandle1));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test registering a critical table that already has an allocated CDS but
     * fails recovery
     */
    /* a. Perform test */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RegisterCDSEx), 1, CFE_ES_CDS_ALREADY_EXISTS);
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RestoreFromCDS), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    CFE_UtAssert_SUCCESS(CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_CRITICAL, NULL));
    CFE_UtAssert_EVENTNOTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(0);

    /* b. Test cleanup: unregister table */
    UT_ClearEventHistory();
    CFE_UtAssert_SUCCESS(CFE_TBL_Unregister(TblHandle1));
    CFE_UtAssert_EVENTCOUNT(0);

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

    CFE_UtAssert_SUCCESS(CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_CRITICAL, NULL));
    CFE_UtAssert_EVENTNOTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(0);

    /* b. Test cleanup: unregister table */
    UT_ClearEventHistory();
    CFE_UtAssert_SUCCESS(CFE_TBL_Unregister(TblHandle1));
    CFE_UtAssert_EVENTCOUNT(0);

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

    CFE_UtAssert_SUCCESS(CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_CRITICAL, NULL));
    CFE_UtAssert_EVENTNOTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(0);

    /* b. Test cleanup: unregister table */
    UT_ClearEventHistory();
    CFE_UtAssert_SUCCESS(CFE_TBL_Unregister(TblHandle1));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test attempt to register a critical table when the CDS registry
     * is full
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RegisterCDSEx), 1, CFE_ES_NO_RESOURCE_IDS_AVAILABLE);
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_CRITICAL, NULL),
                      CFE_TBL_WARN_NOT_CRITICAL);
    CFE_UtAssert_EVENTNOTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test attempt to register a table when the registry is full */
    /* a. Test setup */
    UT_InitData();
    UT_ResetTableRegistry();

    for (i = 0; i < CFE_PLATFORM_TBL_MAX_NUM_TABLES; ++i)
    {
        snprintf(TblName, CFE_MISSION_TBL_MAX_NAME_LENGTH, "UT_Table%d", i + 1);
        if (!CFE_UtAssert_SETUP(CFE_TBL_Register(&TblHandle1, TblName, sizeof(UT_Table1_t), CFE_TBL_OPT_DEFAULT, NULL)))
        {
            break;
        }
    }

    /* b. Perform test */
    UT_ClearEventHistory();
    snprintf(TblName, CFE_MISSION_TBL_MAX_NAME_LENGTH, "UT_Table%d", i + 1);
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle2, TblName, sizeof(UT_Table1_t), CFE_TBL_OPT_DEFAULT, NULL),
                      CFE_TBL_ERR_REGISTRY_FULL);
    CFE_UtAssert_EVENTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* c. Test cleanup: unregister table */
    UT_ClearEventHistory();
    CFE_UtAssert_SUCCESS(CFE_TBL_Unregister(TblHandle1));
    CFE_UtAssert_EVENTNOTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test registering a critical table with no space in the critical table registry */
    /* a. Setup test */
    UT_InitData();
    for (i = 0; i < CFE_PLATFORM_TBL_MAX_CRITICAL_TABLES; i++)
    {
        CFE_TBL_Global.CritReg[i].CDSHandle = CFE_ES_CDSHANDLE_C(CFE_ResourceId_FromInteger(i + 1));
    }
    /* b. Perform test */
    CFE_UtAssert_SUCCESS(CFE_TBL_Register(&TblHandle1, "NOTABLE", sizeof(UT_Table1_t), CFE_TBL_OPT_CRITICAL, NULL));
    CFE_UtAssert_EVENTNOTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(0);

    /* c. Test cleanup: unregister table */
    UT_ClearEventHistory();
    CFE_UtAssert_SUCCESS(CFE_TBL_Unregister(TblHandle1));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test response to no available handles */
    /* a. Test setup */
    UT_InitData();

    do
    {
        if (!CFE_UtAssert_SETUP(CFE_TBL_Share(&TblHandle1, "ut_cfe_tbl.UT_Table2")))
        {
            break;
        }
    } while (TblHandle1 < CFE_PLATFORM_TBL_MAX_NUM_HANDLES - 1);

    /* b. Perform test */
    UT_ClearEventHistory();
    snprintf(TblName, CFE_MISSION_TBL_MAX_NAME_LENGTH, "UT_Table%d", CFE_PLATFORM_TBL_MAX_NUM_TABLES);
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle1, TblName, sizeof(UT_Table1_t), CFE_TBL_OPT_DEFAULT, NULL),
                      CFE_TBL_ERR_HANDLES_FULL);
    CFE_UtAssert_EVENTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test response to an invalid table option combination
     * (CFE_TBL_OPT_USR_DEF_ADDR | CFE_TBL_OPT_CRITICAL)
     */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t),
                                       CFE_TBL_OPT_USR_DEF_ADDR | CFE_TBL_OPT_CRITICAL, NULL),
                      CFE_TBL_ERR_INVALID_OPTIONS);
    CFE_UtAssert_EVENTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test response to an invalid table option combination
     * (CFE_TBL_OPT_DUMP_ONLY | CFE_TBL_OPT_CRITICAL)
     */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t),
                                       CFE_TBL_OPT_DUMP_ONLY | CFE_TBL_OPT_CRITICAL, NULL),
                      CFE_TBL_ERR_INVALID_OPTIONS);
    CFE_UtAssert_EVENTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test attempt to register a table with UsedFlag = false */
    UT_InitData();
    CFE_TBL_Global.Handles[0].UsedFlag = false;
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle2, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_DBL_BUFFER, NULL),
                      CFE_TBL_WARN_DUPLICATE);
    CFE_UtAssert_EVENTCOUNT(0);
    UtAssert_INT32_EQ(TblHandle1, TblHandle2);

    /* Test attempt to register a table with an invalid registry index */
    UT_InitData();
    CFE_TBL_Global.Handles[0].UsedFlag = true;
    CFE_TBL_Global.Handles[0].RegIndex = -1;
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle2, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_DBL_BUFFER, NULL),
                      CFE_TBL_WARN_DUPLICATE);
    CFE_UtAssert_EVENTCOUNT(0);
    UtAssert_INT32_EQ(TblHandle1, TblHandle2);

    /* Test attempt to register a table with access index at end of list */
    UT_InitData();

    for (i = 0; i < CFE_PLATFORM_TBL_MAX_NUM_TABLES; i++)
    {
        CFE_TBL_HandleLinkInit(&CFE_TBL_Global.Registry[i].AccessList);
    }

    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle2, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_DBL_BUFFER, NULL),
                      CFE_TBL_WARN_DUPLICATE);
    CFE_UtAssert_EVENTCOUNT(0);
    UtAssert_INT32_EQ(TblHandle1, TblHandle2);

    /* Test attempt to register a double buffered table with a pool buffer
     * error */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetPoolBuf), 1, CFE_SEVERITY_ERROR);
    snprintf(TblName, CFE_MISSION_TBL_MAX_NAME_LENGTH, "UT_Table%d", CFE_PLATFORM_TBL_MAX_NUM_TABLES);
    CFE_TBL_Global.Handles[0].UsedFlag = false;
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle2, TblName, sizeof(UT_Table1_t) + 1, CFE_TBL_OPT_DBL_BUFFER, NULL),
                      CFE_SEVERITY_ERROR);
    AccessDescPtr = &CFE_TBL_Global.Handles[TblHandle2];
    RegRecPtr     = &CFE_TBL_Global.Registry[AccessDescPtr->RegIndex];
    UtAssert_BOOL_FALSE(RegRecPtr->DoubleBuffered);
    UtAssert_ZERO(RegRecPtr->ActiveBufferIndex);
    CFE_TBL_Global.Handles[0].UsedFlag = true;
}

/*
** Test function that obtains the handle of table registered by another
** application
*/
void Test_CFE_TBL_Share(void)
{
    CFE_FS_Header_t    StdFileHeader;
    CFE_TBL_File_Hdr_t TblFileHeader;

    UtPrintf("Begin Test Share");

    StdFileHeader.SpacecraftID = CFE_PLATFORM_TBL_VALID_SCID_1;
    StdFileHeader.ProcessorID  = CFE_PLATFORM_TBL_VALID_PRID_1;

    /* Test response to a null table handle and null table name */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_Share(NULL, "ut_cfe_tbl.UT_Table2"), CFE_TBL_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_TBL_Share(&App2TblHandle1, NULL), CFE_TBL_BAD_ARGUMENT);

    /* Test response to an invalid application ID */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetAppID), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_TBL_Share(&App2TblHandle1, "ut_cfe_tbl.UT_Table2"), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    CFE_UtAssert_EVENTSENT(CFE_TBL_SHARE_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test response when table name is not in the registry */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    UtAssert_INT32_EQ(CFE_TBL_Share(&App1TblHandle1, "ut_cfe_tbl.NOT_Table2"), CFE_TBL_ERR_INVALID_NAME);
    CFE_UtAssert_EVENTSENT(CFE_TBL_SHARE_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test response when there are no available table handles */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_Share(&App1TblHandle1, "ut_cfe_tbl.UT_Table3"), CFE_TBL_ERR_HANDLES_FULL);
    CFE_UtAssert_EVENTSENT(CFE_TBL_SHARE_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test unregistering tables to free handles */
    UT_InitData();
    CFE_UtAssert_SUCCESS(CFE_TBL_Unregister(CFE_PLATFORM_TBL_MAX_NUM_HANDLES / 2));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test unregister response to a PutPoolBuf error */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_PutPoolBuf), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    CFE_UtAssert_SUCCESS(CFE_TBL_Unregister(CFE_PLATFORM_TBL_MAX_NUM_HANDLES / 2 + 1));
    CFE_UtAssert_EVENTCOUNT(0);

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
    CFE_UtAssert_SUCCESS(CFE_TBL_Load(3, CFE_TBL_SRC_FILE, "TblSrcFileName.dat"));
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOAD_SUCCESS_INF_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test successful share of a table that has not been loaded once */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_2);
    CFE_UtAssert_SUCCESS(CFE_TBL_Share(&App2TblHandle1, "ut_cfe_tbl.UT_Table3"));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test successful share of a table that has been loaded once */
    UT_InitData();
    CFE_UtAssert_SUCCESS(CFE_TBL_Share(&App2TblHandle2, "ut_cfe_tbl.UT_Table4"));
    CFE_UtAssert_EVENTCOUNT(0);
}

/*
** Function to test unregistering a previously registered table and freeing
** associated resources
*/
void Test_CFE_TBL_Unregister(void)
{
    UtPrintf("Begin Test Unregister");

    /* Test response to unregistering a table with an invalid handle */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    UtAssert_INT32_EQ(CFE_TBL_Unregister(CFE_PLATFORM_TBL_MAX_NUM_HANDLES), CFE_TBL_ERR_INVALID_HANDLE);
    CFE_UtAssert_EVENTSENT(CFE_TBL_UNREGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Unregister a shared table to make it unowned */
    UT_InitData();
    CFE_UtAssert_SUCCESS(CFE_TBL_Unregister(3));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test response to unregistering an unowned table */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_2);
    UtAssert_INT32_EQ(CFE_TBL_Unregister(App2TblHandle2), CFE_TBL_ERR_NO_ACCESS);
    CFE_UtAssert_EVENTSENT(CFE_TBL_UNREGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);
}

/*
** Test function that instructs table services to notify calling application
** whenever the specified table requires management
*/
void Test_CFE_TBL_NotifyByMessage(void)
{
    UtPrintf("Begin Test Notify by Message");

    /* Set up notify by message tests */
    UT_InitData();
    Test_CFE_TBL_ApiInit();
    UT_SetAppID(UT_TBL_APPID_1);
    UT_ResetPoolBufferIndex();

    CFE_UtAssert_SUCCESS(
        CFE_TBL_Register(&App1TblHandle1, "NBMsg_Tbl", sizeof(UT_Table1_t), CFE_TBL_OPT_CRITICAL, NULL));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test successful notification */
    UT_InitData();
    CFE_UtAssert_SUCCESS(CFE_TBL_NotifyByMessage(App1TblHandle1, CFE_SB_ValueToMsgId(1), 1, 1));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test response to notification request when the application doesn't
     * own the table handle
     */
    UT_InitData();
    CFE_TBL_Global.Registry[0].OwnerAppId = CFE_TBL_NOT_OWNED;
    UtAssert_INT32_EQ(CFE_TBL_NotifyByMessage(App1TblHandle1, CFE_SB_ValueToMsgId(1), 1, 1), CFE_TBL_ERR_NO_ACCESS);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test response to notification request when the application ID is bad */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetAppID), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_TBL_NotifyByMessage(App1TblHandle1, CFE_SB_ValueToMsgId(1), 1, 1),
                      CFE_ES_ERR_RESOURCEID_NOT_VALID);
    CFE_UtAssert_EVENTCOUNT(0);
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
    CFE_FS_Header_t             StdFileHeader;
    CFE_TBL_File_Hdr_t          TblFileHeader;
    void *                      App2TblPtr;
    CFE_TBL_RegistryRec_t *     RegRecPtr;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr;

    memset(&TestTable1, 0, sizeof(TestTable1));

    UtPrintf("Begin Test Load");

    StdFileHeader.SpacecraftID = CFE_PLATFORM_TBL_VALID_SCID_1;
    StdFileHeader.ProcessorID  = CFE_PLATFORM_TBL_VALID_PRID_1;

    /* Set up for table load test */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    UT_ResetTableRegistry();
    CFE_UtAssert_SUCCESS(CFE_TBL_Register(&App1TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_DEFAULT,
                                          Test_CFE_TBL_ValidationFunc));
    CFE_UtAssert_EVENTNOTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test response to a null source data pointer */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_ADDRESS, NULL), CFE_TBL_BAD_ARGUMENT);

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
    UtAssert_INT32_EQ(CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_FILE, "TblSrcFileName.dat"), CFE_TBL_ERR_PARTIAL_LOAD);
    CFE_UtAssert_EVENTSENT(CFE_TBL_PARTIAL_LOAD_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

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
    CFE_UtAssert_SUCCESS(CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_FILE, "TblSrcFileName.dat"));
    CFE_UtAssert_EVENTNOTSENT(CFE_TBL_PARTIAL_LOAD_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

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
    UtAssert_INT32_EQ(CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_FILE, "TblSrcFileName.dat"),
                      CFE_TBL_ERR_FILE_FOR_WRONG_TABLE);
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOAD_TBLNAME_MISMATCH_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Set up for double buffer table load test */
    /* Test setup - register a double buffered table */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    CFE_UtAssert_SUCCESS(CFE_TBL_Register(&App1TblHandle2, "UT_Table2x", sizeof(UT_Table1_t), CFE_TBL_OPT_DBL_BUFFER,
                                          Test_CFE_TBL_ValidationFunc));
    CFE_UtAssert_EVENTNOTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(0);

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
    UtAssert_INT32_EQ(CFE_TBL_Load(App1TblHandle2, CFE_TBL_SRC_FILE, "TblSrcFileName.dat"),
                      CFE_TBL_ERR_FILE_FOR_WRONG_TABLE);
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOAD_TBLNAME_MISMATCH_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test performing a Load from memory */
    UT_InitData();
    CFE_UtAssert_SUCCESS(CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_ADDRESS, &TestTable1));
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOAD_SUCCESS_INF_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test attempt to load from an illegal source type (not a file or
     * from memory)
     */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_Load(App1TblHandle1, (CFE_TBL_SrcEnum_t)99, &TestTable1), CFE_TBL_ERR_ILLEGAL_SRC_TYPE);
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOAD_TYPE_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test failure of validation function on table load using a negative
     * return code
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(Test_CFE_TBL_ValidationFunc), 1, -1);
    UtAssert_INT32_EQ(CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_ADDRESS, &TestTable1), -1);
    CFE_UtAssert_EVENTSENT(CFE_TBL_VALIDATION_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test failure of validation function on table load using a positive
     * return code
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(Test_CFE_TBL_ValidationFunc), 1, 1);
    UtAssert_INT32_EQ(CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_ADDRESS, &TestTable1), -1);
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOAD_VAL_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(2);

    /* Test attempt to load a table with a bad handle */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_Load(CFE_PLATFORM_TBL_MAX_NUM_HANDLES, CFE_TBL_SRC_ADDRESS, &TestTable1),
                      CFE_TBL_ERR_INVALID_HANDLE);
    CFE_UtAssert_EVENTSENT(CFE_TBL_HANDLE_ACCESS_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test attempt to load a dump-only table */
    /* a. Test setup */
    UT_InitData();
    CFE_UtAssert_SUCCESS(
        CFE_TBL_Register(&DumpOnlyTblHandle, "UT_Table2", sizeof(UT_Table1_t), CFE_TBL_OPT_DUMP_ONLY, NULL));
    CFE_UtAssert_EVENTNOTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(0);

    /* b. Perform test */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_Load(DumpOnlyTblHandle, CFE_TBL_SRC_ADDRESS, &TestTable1), CFE_TBL_ERR_DUMP_ONLY);
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOADING_A_DUMP_ONLY_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test attempt to load a dump-only table with the table already loaded */
    UT_InitData();
    AccessDescPtr              = &CFE_TBL_Global.Handles[DumpOnlyTblHandle];
    RegRecPtr                  = &CFE_TBL_Global.Registry[AccessDescPtr->RegIndex];
    RegRecPtr->UserDefAddr     = true;
    RegRecPtr->TableLoadedOnce = true;
    UtAssert_INT32_EQ(CFE_TBL_Load(DumpOnlyTblHandle, CFE_TBL_SRC_ADDRESS, &TestTable1), CFE_TBL_ERR_DUMP_ONLY);
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOADING_A_DUMP_ONLY_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test specifying a table address for a user defined table */
    /* a. Test setup */
    UT_InitData();
    CFE_UtAssert_SUCCESS(
        CFE_TBL_Register(&UserDefTblHandle, "UT_Table3", sizeof(UT_Table1_t), CFE_TBL_OPT_USR_DEF_ADDR, NULL));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Perform test */
    UT_InitData();
    CFE_UtAssert_SUCCESS(CFE_TBL_Load(UserDefTblHandle, CFE_TBL_SRC_ADDRESS, &TestTable1));
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOAD_SUCCESS_INF_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test attempt to load a locked shared table */
    /* a. Test setup part 1 */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_2);
    CFE_UtAssert_SUCCESS(CFE_TBL_Share(&App2TblHandle1, "ut_cfe_tbl.UT_Table1"));
    CFE_UtAssert_EVENTCOUNT(0);

    /* a. Test setup part 2 */
    UtAssert_INT32_EQ(CFE_TBL_GetAddress(&App2TblPtr, App2TblHandle1), CFE_TBL_INFO_UPDATED);
    CFE_UtAssert_EVENTCOUNT(0);

    /* c. Perform test */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    UtAssert_INT32_EQ(CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_ADDRESS, &TestTable1), CFE_TBL_INFO_TABLE_LOCKED);
    CFE_UtAssert_EVENTCOUNT(1);

    /* d. Test cleanup */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_2);
    CFE_UtAssert_SUCCESS(CFE_TBL_ReleaseAddress(App2TblHandle1));
    CFE_UtAssert_EVENTCOUNT(0);
}

/*
** Function to test obtaining the current address of the contents
** of the specified table
*/
void Test_CFE_TBL_GetAddress(void)
{
    void *App3TblPtr;
    void *App2TblPtr;

    UtPrintf("Begin Test Get Address");

    /* Test response to a null table pointer */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_GetAddress(NULL, App1TblHandle1), CFE_TBL_BAD_ARGUMENT);

    /* Test attempt to get the address of a table for which the application
     * does not have access
     */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_3);
    UtAssert_INT32_EQ(CFE_TBL_GetAddress(&App3TblPtr, App2TblHandle1), CFE_TBL_ERR_NO_ACCESS);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test attempt to get the address with an invalid application ID */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetAppID), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_TBL_GetAddress(&App3TblPtr, App2TblHandle1), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test attempt to get the address with an invalid handle */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_GetAddress(&App3TblPtr, CFE_PLATFORM_TBL_MAX_NUM_HANDLES), CFE_TBL_ERR_INVALID_HANDLE);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Attempt to get the address of an unregistered (unowned) table */
    /* a. Test setup */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    CFE_UtAssert_SUCCESS(CFE_TBL_Unregister(App1TblHandle1));
    CFE_UtAssert_EVENTCOUNT(0);

    /* b. Perform test */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_2);
    UtAssert_INT32_EQ(CFE_TBL_GetAddress(&App2TblPtr, App2TblHandle1), CFE_TBL_ERR_UNREGISTERED);
    CFE_UtAssert_EVENTCOUNT(0);
}

/*
** Function to test release of a previously obtained pointer to the
** contents of the specified table
*/
void Test_CFE_TBL_ReleaseAddress(void)
{
    UtPrintf("Begin Test Release Address");

    /* Test address release using an invalid application ID */
    /* a. Test setup - register single buffered table */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    UT_ResetTableRegistry();
    CFE_UtAssert_SUCCESS(CFE_TBL_Register(&App1TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_DEFAULT,
                                          Test_CFE_TBL_ValidationFunc));
    CFE_UtAssert_EVENTNOTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(0);

    /* b. Perform test */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetAppID), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_TBL_ReleaseAddress(App1TblHandle1), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    CFE_UtAssert_EVENTCOUNT(0);
}

/*
** Test function that obtains the current addresses of the contents of a
** collection of tables
*/
void Test_CFE_TBL_GetAddresses(void)
{
    UtPrintf("Begin Test Get Addresses");

    /* Test setup - register a double buffered table */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    CFE_UtAssert_SUCCESS(CFE_TBL_Register(&App1TblHandle2, "UT_Table2", sizeof(UT_Table1_t), CFE_TBL_OPT_DBL_BUFFER,
                                          Test_CFE_TBL_ValidationFunc));
    CFE_UtAssert_EVENTNOTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(0);

    /* b. Perform test */
    UT_InitData();

    /* Test getting the addresses of two tables that have not been loaded */
    ArrayOfHandles[0]       = App1TblHandle1;
    ArrayOfHandles[1]       = App1TblHandle2;
    ArrayOfPtrsToTblPtrs[0] = &Tbl1Ptr;
    ArrayOfPtrsToTblPtrs[1] = &Tbl2Ptr;

    UtAssert_INT32_EQ(CFE_TBL_GetAddresses(ArrayOfPtrsToTblPtrs, 2, ArrayOfHandles), CFE_TBL_ERR_NEVER_LOADED);
    CFE_UtAssert_EVENTCOUNT(0);
    UtAssert_NOT_NULL(Tbl1Ptr);
    UtAssert_NOT_NULL(Tbl2Ptr);

    /* Test getting the addresses of zero tables */
    CFE_UtAssert_SUCCESS(CFE_TBL_GetAddresses(ArrayOfPtrsToTblPtrs, 0, ArrayOfHandles));

    /* Test response to a null table pointer and null table handle */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_GetAddresses(NULL, 2, ArrayOfHandles), CFE_TBL_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_TBL_GetAddresses(ArrayOfPtrsToTblPtrs, 2, NULL), CFE_TBL_BAD_ARGUMENT);

    /* Test attempt to get addresses of tables that the application is not
     * allowed to see
     */
    UT_InitData();
    UT_SetAppID(CFE_ES_APPID_UNDEFINED);
    UtAssert_INT32_EQ(CFE_TBL_GetAddresses(ArrayOfPtrsToTblPtrs, 2, ArrayOfHandles), CFE_TBL_ERR_NO_ACCESS);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test attempt to get addresses of tables with a bad app ID */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetAppID), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_TBL_GetAddresses(ArrayOfPtrsToTblPtrs, 2, ArrayOfHandles), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    CFE_UtAssert_EVENTCOUNT(0);
}

/*
** Test function that releases previously obtained pointers to the contents
** of the specified tables
*/
void Test_CFE_TBL_ReleaseAddresses(void)
{
    CFE_FS_Header_t  FileHeader;
    UT_TempFile_t    File;
    CFE_TBL_Handle_t TblHandle1;
    UtPrintf("Begin Test Release Addresses");

    /* Test response to a null table handle pointer */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_ReleaseAddresses(2, NULL), CFE_TBL_BAD_ARGUMENT);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test response to releasing two tables that have not been loaded */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    UtAssert_INT32_EQ(CFE_TBL_ReleaseAddresses(2, ArrayOfHandles), CFE_TBL_ERR_NEVER_LOADED);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test releasing 0 then 1 addresses */
    UT_InitData();

    memset(&File, 0, sizeof(File));

    /* a. Configure for successful file read to initialize table */
    strncpy(FileHeader.Description, "FS header description", sizeof(FileHeader.Description) - 1);
    FileHeader.Description[sizeof(FileHeader.Description) - 1] = '\0';
    FileHeader.ContentType                                     = CFE_FS_FILE_CONTENT_ID;
    FileHeader.SubType                                         = CFE_FS_SubType_TBL_IMG;
    FileHeader.TimeSeconds                                     = 1704;
    FileHeader.TimeSubSeconds                                  = 104;
    strncpy(File.TblHeader.TableName, "ut_cfe_tbl.UT_RELEASE_TEST", sizeof(File.TblHeader.TableName) - 1);
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

    /* b. Perform load */
    CFE_UtAssert_SUCCESS(
        CFE_TBL_Register(&TblHandle1, "UT_RELEASE_TEST", sizeof(UT_Table1_t), CFE_TBL_OPT_DEFAULT, NULL));
    CFE_UtAssert_SUCCESS(CFE_TBL_Load(TblHandle1, CFE_TBL_SRC_FILE, "MyInputFile"));

    /* c. Get addresses */
    ArrayOfHandles[0]       = TblHandle1;
    ArrayOfPtrsToTblPtrs[0] = &Tbl1Ptr;
    CFE_UtAssert_SUCCESS(CFE_TBL_GetAddresses(ArrayOfPtrsToTblPtrs, 1, ArrayOfHandles));
    UT_ClearEventHistory();

    /* d. Release 0 addresses */
    CFE_UtAssert_SUCCESS(CFE_TBL_ReleaseAddresses(0, ArrayOfHandles));
    CFE_UtAssert_EVENTCOUNT(0);

    /* e. Release 1 addresses */
    CFE_UtAssert_SUCCESS(CFE_TBL_ReleaseAddresses(1, ArrayOfHandles));
    CFE_UtAssert_EVENTCOUNT(0);
}

/*
** Test function for validating the table image for a specified table
*/
void Test_CFE_TBL_Validate(void)
{
    int16                       RegIndex;
    CFE_TBL_RegistryRec_t *     RegRecPtr;
    CFE_TBL_ValidationResult_t *ValResultPtr;

    UtPrintf("Begin Test Validate");

    /* Test setup */
    RegIndex  = CFE_TBL_FindTableInRegistry("ut_cfe_tbl.UT_Table1");
    RegRecPtr = &CFE_TBL_Global.Registry[RegIndex];

    /* Refer to the test validation function */
    RegRecPtr->ValidationFuncPtr = Test_CFE_TBL_ValidationFunc;

    /* Test response to attempt to validate a table that an application is
     * not allowed to see
     */
    UT_InitData();
    UT_SetAppID(CFE_ES_APPID_UNDEFINED);
    UtAssert_INT32_EQ(CFE_TBL_Validate(App1TblHandle1), CFE_TBL_ERR_NO_ACCESS);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test response to attempt to validate a table when no validation is
     * pending
     */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    UtAssert_INT32_EQ(CFE_TBL_Validate(App1TblHandle1), CFE_TBL_INFO_NO_VALIDATION_PENDING);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test failed validation */
    UT_InitData();

    /* a. Configure table for validation */
    UT_TBL_SetupPendingValidation(0, false, RegRecPtr, &ValResultPtr);

    RegRecPtr->LoadInProgress = 0;

    /* b. Perform failed validation */
    UT_SetDeferredRetcode(UT_KEY(Test_CFE_TBL_ValidationFunc), 1, -1);
    CFE_UtAssert_SUCCESS(CFE_TBL_Validate(App1TblHandle1));
    CFE_UtAssert_EVENTSENT(CFE_TBL_VALIDATION_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);
    UtAssert_INT32_EQ(ValResultPtr->Result, -1);

    /* Test validation on table w/user-defined address (this is not a valid combo) */
    UT_InitData();

    /* a. Configure table for validation and set UserDefAddr flag */
    UT_TBL_SetupPendingValidation(0, false, RegRecPtr, &ValResultPtr);
    RegRecPtr->UserDefAddr = true;

    /* b. Perform validation */
    CFE_UtAssert_SUCCESS(CFE_TBL_Validate(App1TblHandle1));
    CFE_UtAssert_EVENTSENT(CFE_TBL_VALIDATION_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);
    UtAssert_INT32_EQ(ValResultPtr->Result, -1);
    RegRecPtr->UserDefAddr = false;

    /* Test case where validation request is stale */
    UT_InitData();

    /* a. Configure table for validation and modify the ID so it will not match */
    UT_TBL_SetupPendingValidation(0, false, RegRecPtr, &ValResultPtr);
    ValResultPtr->ValId = CFE_TBL_VALRESULTID_C(CFE_ResourceId_FromInteger(1));

    /* b. Perform validation */
    UtAssert_INT32_EQ(CFE_TBL_Validate(App1TblHandle1), CFE_TBL_INFO_NO_VALIDATION_PENDING);
    UtAssert_INT32_EQ(ValResultPtr->Result, CFE_SUCCESS);

    /* Test successful validation */
    UT_InitData();

    /* a. Configure table for validation */
    UT_TBL_SetupPendingValidation(0, false, RegRecPtr, &ValResultPtr);

    /* b. Perform validation */
    UT_SetDeferredRetcode(UT_KEY(Test_CFE_TBL_ValidationFunc), 1, CFE_SUCCESS);
    CFE_UtAssert_SUCCESS(CFE_TBL_Validate(App1TblHandle1));
    UtAssert_INT32_EQ(ValResultPtr->Result, CFE_SUCCESS);

    /* Set up a case where the entry does not have a validation function ptr */
    UT_TBL_SetupPendingValidation(0, false, RegRecPtr, &ValResultPtr);
    ValResultPtr->Result         = -1;
    RegRecPtr->ValidationFuncPtr = NULL;

    /* b. Perform failed validation */
    CFE_UtAssert_SUCCESS(CFE_TBL_Validate(App1TblHandle1));
    UtAssert_INT32_EQ(ValResultPtr->Result, CFE_SUCCESS);
}

/*
** Test function for performing standard table maintenance on the
** specified table
*/
void Test_CFE_TBL_Manage(void)
{
    int16                       RegIndex;
    CFE_TBL_RegistryRec_t *     RegRecPtr;
    CFE_TBL_LoadBuff_t *        WorkingBufferPtr;
    UT_Table1_t                 TestTable1;
    void *                      App2TblPtr;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr;
    CFE_TBL_Handle_t            AccessIterator;
    CFE_TBL_ValidationResult_t *ValResultPtr;
    CFE_TBL_DumpControl_t *     DumpCtrlPtr;

    memset(&TestTable1, 0, sizeof(TestTable1));

    UtPrintf("Begin Test Manage");

    /* Test response to attempt to manage a table that doesn't need managing */
    UT_InitData();
    CFE_UtAssert_SUCCESS(CFE_TBL_Manage(App1TblHandle1));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test response to attempt to load while a load is in progress */
    UT_InitData();

    /* "Load" image into inactive buffer for table */
    RegIndex  = CFE_TBL_FindTableInRegistry("ut_cfe_tbl.UT_Table1");
    RegRecPtr = &CFE_TBL_Global.Registry[RegIndex];

    /* Refer to the test validation function */
    RegRecPtr->ValidationFuncPtr = Test_CFE_TBL_ValidationFunc;

    CFE_UtAssert_SUCCESS(CFE_TBL_GetWorkingBuffer(&WorkingBufferPtr, RegRecPtr, false));
    UT_SetAppID(UT_TBL_APPID_1);
    UtAssert_INT32_EQ(CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_ADDRESS, &TestTable1), CFE_TBL_ERR_LOAD_IN_PROGRESS);
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOAD_IN_PROGRESS_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test response to processing an unsuccessful validation request on
     * inactive buffer; validation function return code is valid
     */
    UT_InitData();

    /* Configure table for validation */
    UT_TBL_SetupPendingValidation(0, false, RegRecPtr, &ValResultPtr);

    /* Perform validation via manage call */
    UT_SetDeferredRetcode(UT_KEY(Test_CFE_TBL_ValidationFunc), 1, -1);
    CFE_UtAssert_SUCCESS(CFE_TBL_Manage(App1TblHandle1));
    CFE_UtAssert_EVENTSENT(CFE_TBL_VALIDATION_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);
    UtAssert_INT32_EQ(ValResultPtr->Result, -1);

    /* Test response to processing an unsuccessful validation request on
     * inactive buffer ; validation function return code is invalid
     */
    UT_InitData();

    /* Configure table for validation */
    UT_TBL_SetupPendingValidation(0, false, RegRecPtr, &ValResultPtr);

    /* Perform validation via manage call */
    UT_SetDeferredRetcode(UT_KEY(Test_CFE_TBL_ValidationFunc), 1, 1);
    CFE_UtAssert_SUCCESS(CFE_TBL_Manage(App1TblHandle1));
    CFE_UtAssert_EVENTSENT(CFE_TBL_VALIDATION_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);
    UtAssert_INT32_EQ(ValResultPtr->Result, 1);

    /* Test response to processing an unsuccessful validation request;
     * CFE_TBL_Validate does not return CFE_SUCCESS
     */
    UT_InitData();

    /* Configure table for validation */
    UT_TBL_SetupPendingValidation(0, false, RegRecPtr, &ValResultPtr);

    /* Perform validation via manage call */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetAppID), 2, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_TBL_Manage(App1TblHandle1), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    CFE_UtAssert_EVENTCOUNT(0);
    UtAssert_INT32_EQ(ValResultPtr->Result, 0);

    /* Test response to processing a successful validation request on an
     * inactive buffer
     */
    UT_InitData();

    /* Configure table for validation */
    UT_TBL_SetupPendingValidation(0, false, RegRecPtr, &ValResultPtr);
    ValResultPtr->Result = 1;

    /* Perform validation via manage call */
    UT_SetDeferredRetcode(UT_KEY(Test_CFE_TBL_ValidationFunc), 1, CFE_SUCCESS);
    CFE_UtAssert_SUCCESS(CFE_TBL_Manage(App1TblHandle1));
    CFE_UtAssert_EVENTSENT(CFE_TBL_VALIDATION_INF_EID);
    CFE_UtAssert_EVENTCOUNT(1);
    UtAssert_INT32_EQ(ValResultPtr->Result, 0);

    /* Test response to processing an unsuccessful validation request on an
     * active buffer
     */
    UT_InitData();

    /* Configure table for validation */
    UT_TBL_SetupPendingValidation(0, true, RegRecPtr, &ValResultPtr);

    /* Perform validation via manage call */
    UT_SetDeferredRetcode(UT_KEY(Test_CFE_TBL_ValidationFunc), 1, -1);
    CFE_UtAssert_SUCCESS(CFE_TBL_Manage(App1TblHandle1));
    CFE_UtAssert_EVENTSENT(CFE_TBL_VALIDATION_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);
    UtAssert_INT32_EQ(ValResultPtr->Result, -1);

    /* Test response to processing an unsuccessful validation request on
     * an active buffer
     */
    UT_InitData();

    /* Configure table for validation */
    UT_TBL_SetupPendingValidation(0, true, RegRecPtr, &ValResultPtr);

    /* Perform validation via manage call */
    UT_SetDeferredRetcode(UT_KEY(Test_CFE_TBL_ValidationFunc), 1, 1);
    CFE_UtAssert_SUCCESS(CFE_TBL_Manage(App1TblHandle1));
    CFE_UtAssert_EVENTSENT(CFE_TBL_VALIDATION_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);
    UtAssert_INT32_EQ(ValResultPtr->Result, 1);

    /* Test response to processing a successful validation request on an
     * active buffer
     */
    UT_InitData();

    /* Configure table for validation */
    UT_TBL_SetupPendingValidation(0, true, RegRecPtr, &ValResultPtr);
    ValResultPtr->Result = 1;

    /* Perform validation via manage call */
    UT_SetDeferredRetcode(UT_KEY(Test_CFE_TBL_ValidationFunc), 1, CFE_SUCCESS);
    CFE_UtAssert_SUCCESS(CFE_TBL_Manage(App1TblHandle1));
    CFE_UtAssert_EVENTSENT(CFE_TBL_VALIDATION_INF_EID);
    CFE_UtAssert_EVENTCOUNT(1);
    UtAssert_INT32_EQ(ValResultPtr->Result, 0);

    /* Test response to processing an update request on a locked table */
    /* a. Test setup - part 1 */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_2);
    CFE_UtAssert_SUCCESS(CFE_TBL_Share(&App2TblHandle1, "ut_cfe_tbl.UT_Table1"));
    CFE_UtAssert_EVENTCOUNT(0);

    /* a. Test setup - part 2 */
    UtAssert_INT32_EQ(CFE_TBL_GetAddress(&App2TblPtr, App2TblHandle1), CFE_TBL_ERR_NEVER_LOADED);
    CFE_UtAssert_EVENTCOUNT(0);

    /* c. Perform test */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);

    /* Configure table for update */
    RegRecPtr->LoadPending = true;
    UtAssert_INT32_EQ(CFE_TBL_Manage(App1TblHandle1), CFE_TBL_INFO_TABLE_LOCKED);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Save the previous table's information for a subsequent test */
    AccessDescPtr  = &CFE_TBL_Global.Handles[App1TblHandle1];
    RegRecPtr      = &CFE_TBL_Global.Registry[AccessDescPtr->RegIndex];
    AccessIterator = RegRecPtr->AccessList.Next;

    /* Test unlocking a table by releasing the address */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_2);
    UtAssert_INT32_EQ(CFE_TBL_ReleaseAddress(App2TblHandle1), CFE_TBL_ERR_NEVER_LOADED);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test response to processing an update request on a single
     * buffered table
     */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);

    /* Configure table for Update */
    RegRecPtr->LoadPending = true;
    UtAssert_INT32_EQ(CFE_TBL_Manage(App1TblHandle1), CFE_TBL_INFO_UPDATED);
    CFE_UtAssert_EVENTSENT(CFE_TBL_UPDATE_SUCCESS_INF_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test response to processing an unsuccessful validation request on an
     * inactive buffer (double buffered)
     */
    UT_InitData();

    /* Reset the current table entry pointer to a previous table in order to
     * exercise the path where no buffer is available
     */
    AccessDescPtr                                      = &CFE_TBL_Global.Handles[App1TblHandle2];
    RegRecPtr                                          = &CFE_TBL_Global.Registry[AccessDescPtr->RegIndex];
    CFE_TBL_Global.Handles[AccessIterator].Link.Next   = RegRecPtr->AccessList.Next;
    CFE_TBL_Global.Handles[AccessIterator].AppId       = UT_TBL_APPID_2;
    RegRecPtr->AccessList.Next                         = AccessIterator;
    CFE_TBL_Global.Handles[AccessIterator].BufferIndex = 1;
    CFE_TBL_Global.Handles[AccessIterator].LockFlag    = true;

    /* Attempt to "load" image into inactive buffer for table */
    RegIndex  = CFE_TBL_FindTableInRegistry("ut_cfe_tbl.UT_Table2");
    RegRecPtr = &CFE_TBL_Global.Registry[RegIndex];
    UtAssert_INT32_EQ(CFE_TBL_GetWorkingBuffer(&WorkingBufferPtr, RegRecPtr, false), CFE_TBL_ERR_NO_BUFFER_AVAIL);

    /* Reset the table information for subsequent tests */
    CFE_TBL_Global.Handles[AccessIterator].BufferIndex = 1;
    CFE_TBL_Global.Handles[AccessIterator].LockFlag    = false;

    /* Successfully "load" image into inactive buffer for table */
    CFE_UtAssert_SUCCESS(CFE_TBL_GetWorkingBuffer(&WorkingBufferPtr, RegRecPtr, false));

    /* Configure table for validation */
    UT_TBL_SetupPendingValidation(1, false, RegRecPtr, &ValResultPtr);

    /* Perform validation via manage call */
    UT_SetDeferredRetcode(UT_KEY(Test_CFE_TBL_ValidationFunc), 1, -1);
    CFE_UtAssert_SUCCESS(CFE_TBL_Manage(App1TblHandle2));
    CFE_UtAssert_EVENTSENT(CFE_TBL_VALIDATION_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);
    UtAssert_INT32_EQ(ValResultPtr->Result, -1);

    /* Test successfully processing a validation request on an inactive buffer
     * (double buffered)
     */
    UT_InitData();

    /* Configure table for validation */
    UT_TBL_SetupPendingValidation(1, false, RegRecPtr, &ValResultPtr);

    /* Perform validation via manage call */
    UT_SetDeferredRetcode(UT_KEY(Test_CFE_TBL_ValidationFunc), 1, CFE_SUCCESS);
    CFE_UtAssert_SUCCESS(CFE_TBL_Manage(App1TblHandle2));
    CFE_UtAssert_EVENTSENT(CFE_TBL_VALIDATION_INF_EID);
    CFE_UtAssert_EVENTCOUNT(1);
    UtAssert_INT32_EQ(ValResultPtr->Result, 0);

    /* Test processing an unsuccessful validation request on an active buffer
     * (double buffered)
     */
    UT_InitData();

    /* Configure table for validation */
    UT_TBL_SetupPendingValidation(1, true, RegRecPtr, &ValResultPtr);

    /* Perform validation via manage call */
    UT_SetDeferredRetcode(UT_KEY(Test_CFE_TBL_ValidationFunc), 1, -1);
    CFE_UtAssert_SUCCESS(CFE_TBL_Manage(App1TblHandle2));
    CFE_UtAssert_EVENTSENT(CFE_TBL_VALIDATION_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);
    UtAssert_INT32_EQ(ValResultPtr->Result, -1);

    /* Test successfully processing a validation request on active buffer
     * (double buffered)
     */
    UT_InitData();

    /* Configure table for validation */
    UT_TBL_SetupPendingValidation(1, true, RegRecPtr, &ValResultPtr);
    ValResultPtr->Result = 1;

    /* Perform validation via manage call */
    UT_SetDeferredRetcode(UT_KEY(Test_CFE_TBL_ValidationFunc), 1, CFE_SUCCESS);
    CFE_UtAssert_SUCCESS(CFE_TBL_Manage(App1TblHandle2));
    CFE_UtAssert_EVENTSENT(CFE_TBL_VALIDATION_INF_EID);
    CFE_UtAssert_EVENTCOUNT(1);
    UtAssert_INT32_EQ(ValResultPtr->Result, 0);

    /* Test successfully processing a table dump request */
    UT_InitData();
    CFE_UtAssert_SUCCESS(CFE_TBL_GetWorkingBuffer(&WorkingBufferPtr, RegRecPtr, false));
    UT_TBL_SetupPendingDump(0, WorkingBufferPtr, RegRecPtr, &DumpCtrlPtr);

    /* Save the name of the desired dump filename, table name, and size for
     * later
     */
    strncpy(WorkingBufferPtr->DataSource, "MyDumpFilename", sizeof(WorkingBufferPtr->DataSource) - 1);
    WorkingBufferPtr->DataSource[sizeof(WorkingBufferPtr->DataSource) - 1] = 0;
    CFE_UtAssert_SUCCESS(CFE_TBL_Manage(App1TblHandle2));
    CFE_UtAssert_EVENTCOUNT(0);
}

/*
** Test function for dumping to a buffer
*/
void Test_CFE_TBL_DumpToBuffer(void)
{
    UtPrintf("Begin Test Dump To Buffer");

    /* Test successfully dumping to a buffer */
    UT_InitData();
    CFE_UtAssert_SUCCESS(CFE_TBL_DumpToBuffer(App1TblHandle1));

    /* Test response to an attempt to dump the buffer on a table that the
     * application is not allowed to see
     */
    UT_InitData();
    UT_SetAppID(CFE_ES_APPID_UNDEFINED);
    UtAssert_INT32_EQ(CFE_TBL_DumpToBuffer(App1TblHandle1), CFE_TBL_ERR_NO_ACCESS);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test response to an attempt to dump the buffer on a bad table handle */
    UT_InitData();
    UT_SetAppID(CFE_ES_APPID_UNDEFINED);
    UtAssert_INT32_EQ(CFE_TBL_DumpToBuffer(CFE_TBL_BAD_TABLE_HANDLE), CFE_TBL_ERR_INVALID_HANDLE);
    CFE_UtAssert_EVENTCOUNT(0);
}

/*
** Test function that updates the contents of a table if an update is pending
*/
void Test_CFE_TBL_Update(void)
{
    int16                       RegIndex;
    CFE_TBL_LoadBuff_t *        WorkingBufferPtr;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr;
    CFE_TBL_RegistryRec_t *     RegRecPtr;
    AccessDescPtr = &CFE_TBL_Global.Handles[App1TblHandle1];
    RegRecPtr     = &CFE_TBL_Global.Registry[AccessDescPtr->RegIndex];

    UtPrintf("Begin Test Update");

    /* Test a successful update */
    UT_InitData();

    /* a. Configure table for update */
    RegIndex  = CFE_TBL_FindTableInRegistry("ut_cfe_tbl.UT_Table1");
    RegRecPtr = &CFE_TBL_Global.Registry[RegIndex];
    CFE_UtAssert_SUCCESS(CFE_TBL_GetWorkingBuffer(&WorkingBufferPtr, RegRecPtr, false));
    UT_SetAppID(UT_TBL_APPID_1);
    RegRecPtr->LoadPending = true;

    /* b. Perform update test */
    UtAssert_INT32_EQ(CFE_TBL_Update(App1TblHandle1), CFE_SUCCESS);
    CFE_UtAssert_EVENTSENT(CFE_TBL_UPDATE_SUCCESS_INF_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test processing an update on a single buffered table without
     * privileges
     */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_2);
    UtAssert_INT32_EQ(CFE_TBL_Update(App1TblHandle1), CFE_TBL_ERR_NO_ACCESS);
    CFE_UtAssert_EVENTSENT(CFE_TBL_UPDATE_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test processing an update on a single buffered table when no update
     * is pending
     */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    UtAssert_INT32_EQ(CFE_TBL_Update(App1TblHandle1), CFE_TBL_INFO_NO_UPDATE_PENDING);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test processing an update on an application with a bad ID
     */
    UT_InitData();
    UT_SetAppID(CFE_ES_APPID_UNDEFINED);
    UtAssert_INT32_EQ(CFE_TBL_Update(App1TblHandle1), CFE_TBL_ERR_NO_ACCESS);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test with resource not valid */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetAppID), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_TBL_Update(App1TblHandle1), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    CFE_UtAssert_EVENTCOUNT(1);
}

/*
** Test function that obtains the pending action status for specified table
*/
void Test_CFE_TBL_GetStatus(void)
{
    UtPrintf("Begin Test Get Status");

    /* Test response to successfully getting status of a table */
    UT_InitData();
    CFE_UtAssert_SUCCESS(CFE_TBL_GetStatus(App1TblHandle1));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test response to an attempt to get the status on a table that the
     * application is not allowed to see
     */
    UT_InitData();
    UT_SetAppID(CFE_ES_APPID_UNDEFINED);
    UtAssert_INT32_EQ(CFE_TBL_GetStatus(App1TblHandle1), CFE_TBL_ERR_NO_ACCESS);
    CFE_UtAssert_EVENTCOUNT(0);
}

/*
** Test function that obtains characteristics/information for a specified table
*/
void Test_CFE_TBL_GetInfo(void)
{
    CFE_TBL_Info_t TblInfo;

    UtPrintf("Begin Test Get Info");

    /* Test response to a null table info and null table name */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_GetInfo(NULL, "ut_cfe_tbl.UT_Table1"), CFE_TBL_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_TBL_GetInfo(&TblInfo, NULL), CFE_TBL_BAD_ARGUMENT);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test successfully getting information on a table */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    CFE_UtAssert_SUCCESS(CFE_TBL_GetInfo(&TblInfo, "ut_cfe_tbl.UT_Table1"));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test response to attempt to get information on a non-existent table */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_GetInfo(&TblInfo, "ut_cfe_tbl.UT_Table_Not"), CFE_TBL_ERR_INVALID_NAME);
    CFE_UtAssert_EVENTCOUNT(0);
}

/*
** Test function that loads a specified table with data from the
** specified source
*/
void Test_CFE_TBL_TblMod(void)
{
    CFE_FS_Header_t             FileHeader;
    UT_TempFile_t               File;
    uint32                      Index;
    CFE_TBL_Info_t              TblInfo1;
    void *                      TblDataAddr;
    UT_Table1_t *               TblDataPtr;
    char                        MyFilename[OS_MAX_PATH_LEN];
    CFE_TBL_AccessDescriptor_t *AccessDescPtr;
    CFE_TBL_RegistryRec_t *     RegRecPtr;
    CFE_TBL_Handle_t            AccessIterator;
    uint8                       CDS_Data[sizeof(UT_Table1_t)];
    uint32                      ExpectedCrc;
    int                         maxPathLenDiff = (int)CFE_MISSION_MAX_PATH_LEN - (int)OS_MAX_PATH_LEN;

    memset(&TblInfo1, 0, sizeof(TblInfo1));

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
    CFE_UtAssert_SUCCESS(
        CFE_TBL_Register(&App1TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_CRITICAL, NULL));
    CFE_UtAssert_EVENTCOUNT(0);

    /* b. Perform test */
    UT_ClearEventHistory();

    memset(&File, 0, sizeof(File));

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
    CFE_UtAssert_SUCCESS(CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_FILE, "MyInputFile"));
    CFE_UtAssert_EVENTCOUNT(1);
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOAD_SUCCESS_INF_EID);

    /* Modify the contents of the table */
    CFE_TBL_GetAddress(&TblDataAddr, App1TblHandle1);
    TblDataPtr = TblDataAddr;
    TblDataPtr->TblElement1 ^= 0xFFFFFFFF;
    File.TblData.TblElement1 ^= 0xFFFFFFFF;

    /* Notify Table Services that the table has been modified */
    UT_SetDataBuffer(UT_KEY(CFE_ES_CopyToCDS), CDS_Data, sizeof(CDS_Data), false);
    CFE_UtAssert_SUCCESS(CFE_TBL_Modified(App1TblHandle1));
    CFE_UtAssert_SUCCESS(CFE_TBL_GetInfo(&TblInfo1, "ut_cfe_tbl.UT_Table1"));
    UtAssert_UINT32_EQ(TblInfo1.TimeOfLastUpdate.Seconds, TblInfo1.TimeOfLastUpdate.Subseconds);
    UtAssert_StrCmp(TblInfo1.LastFileLoaded, "MyInputFile(*)", "TblInfo1.LastFileLoaded (%s)", TblInfo1.LastFileLoaded);
    UtAssert_MemCmp(CDS_Data, &File.TblData, sizeof(CDS_Data), "Table Data");

    /* Save the previous table's information for a subsequent test */
    AccessDescPtr  = &CFE_TBL_Global.Handles[App1TblHandle1];
    RegRecPtr      = &CFE_TBL_Global.Registry[AccessDescPtr->RegIndex];
    AccessIterator = RegRecPtr->AccessList.Next;

    /* Test response to adding a TBL API for notifying table services that
     * the table has been updated by application
     */
    /* a. Test setup */
    UT_InitData();

    /* Register a non critical table */
    CFE_UtAssert_SUCCESS(
        CFE_TBL_Register(&App1TblHandle1, "UT_Table2", sizeof(UT_Table1_t), CFE_TBL_OPT_DEFAULT, NULL));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Reset the current table entry pointer to a previous table in order to
     * exercise the path where one of the application IDs don't match
     */
    AccessDescPtr                                    = &CFE_TBL_Global.Handles[App1TblHandle1];
    RegRecPtr                                        = &CFE_TBL_Global.Registry[AccessDescPtr->RegIndex];
    CFE_TBL_Global.Handles[AccessIterator].Link.Next = RegRecPtr->AccessList.Next;
    CFE_TBL_Global.Handles[AccessIterator].AppId     = UT_TBL_APPID_2;
    RegRecPtr->AccessList.Next                       = AccessIterator;

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

    CFE_UtAssert_SUCCESS(CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_FILE, MyFilename));
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOAD_SUCCESS_INF_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /*
     * Notify Table Services that the table has been modified. Verify CRC has been
     * calculated and table has been flagged as Updated
     */
    ExpectedCrc = 0x0000F00D;
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CalculateCRC), 1, ExpectedCrc);
    CFE_TBL_Global.Handles[AccessIterator].Updated = false;
    CFE_UtAssert_SUCCESS(CFE_TBL_Modified(App1TblHandle1));
    UtAssert_BOOL_TRUE(CFE_TBL_Global.Handles[AccessIterator].Updated);
    CFE_UtAssert_SUCCESS(CFE_TBL_GetInfo(&TblInfo1, "ut_cfe_tbl.UT_Table2"));
    UtAssert_INT32_EQ(TblInfo1.TimeOfLastUpdate.Seconds, TblInfo1.TimeOfLastUpdate.Subseconds);
    UtAssert_UINT32_EQ(TblInfo1.Crc, ExpectedCrc);
    UtAssert_INT32_EQ(CFE_TBL_GetAddress(&TblDataAddr, App1TblHandle1), CFE_TBL_INFO_UPDATED);

    /*
     * LastFileLoaded (limited by mission) can be bigger than MyFilename (limited by osal)
     */
    UtAssert_StrnCmp(TblInfo1.LastFileLoaded, MyFilename, sizeof(TblInfo1.LastFileLoaded) - 4, "%s == %s, %ld",
                     TblInfo1.LastFileLoaded, MyFilename, (long)sizeof(TblInfo1.LastFileLoaded) - 4);

    if (maxPathLenDiff >= 0)
    {
        UtAssert_StrCmp(&TblInfo1.LastFileLoaded[sizeof(MyFilename) - 4], "(*)", "%s == (*)",
                        &TblInfo1.LastFileLoaded[sizeof(MyFilename) - 4]);
    }
    else if (maxPathLenDiff > -3)
    {
        int modIndicatorStart = (int)CFE_MISSION_MAX_PATH_LEN - 4 - maxPathLenDiff;
        UtAssert_StrCmp(&TblInfo1.LastFileLoaded[modIndicatorStart], "(*)", "%s == (*)",
                        &TblInfo1.LastFileLoaded[modIndicatorStart]);
    }

    /* Test response to an invalid handle */
    UtAssert_INT32_EQ(CFE_TBL_Modified(CFE_TBL_BAD_TABLE_HANDLE), CFE_TBL_ERR_INVALID_HANDLE);
}

/*
** Tests for the remaining functions in cfe_tbl_internal.c
*/
void Test_CFE_TBL_Internal(void)
{
    CFE_TBL_TxnState_t          Txn;
    CFE_TBL_LoadBuff_t *        WorkingBufferPtr;
    CFE_TBL_RegistryRec_t *     RegRecPtr;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr;
    CFE_TBL_DumpControl_t *     DumpCtrlPtr;
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
    CFE_UtAssert_SUCCESS(CFE_TBL_Register(&App1TblHandle2, "UT_Table3", sizeof(UT_Table1_t), CFE_TBL_OPT_DBL_BUFFER,
                                          Test_CFE_TBL_ValidationFunc));
    CFE_UtAssert_EVENTNOTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test successful initial load of double buffered table */
    UT_InitData();
    AccessDescPtr = &CFE_TBL_Global.Handles[App1TblHandle2];
    RegRecPtr     = &CFE_TBL_Global.Registry[AccessDescPtr->RegIndex];
    strncpy(RegRecPtr->Name, "ut_cfe_tbl.UT_Table3", sizeof(RegRecPtr->Name) - 1);
    RegRecPtr->Name[sizeof(RegRecPtr->Name) - 1] = '\0';
    RegRecPtr->TableLoadedOnce                   = false;
    RegRecPtr->LoadInProgress                    = CFE_TBL_NO_LOAD_IN_PROGRESS;
    CFE_UtAssert_SUCCESS(CFE_TBL_GetWorkingBuffer(&WorkingBufferPtr, RegRecPtr, true));
    CFE_UtAssert_EVENTCOUNT(0);
    UtAssert_ADDRESS_EQ(WorkingBufferPtr, &RegRecPtr->Buffers[RegRecPtr->ActiveBufferIndex]);

    /* Test response to a single buffered table with a mutex sem take
     * failure
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemTake), 1, OS_ERROR);
    AccessDescPtr = &CFE_TBL_Global.Handles[App1TblHandle1];
    RegRecPtr     = &CFE_TBL_Global.Registry[AccessDescPtr->RegIndex];
    CFE_UtAssert_SUCCESS(CFE_TBL_GetWorkingBuffer(&WorkingBufferPtr, RegRecPtr, false));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test CFE_TBL_LoadFromFile response to a file name that is too long */
    UT_InitData();

    for (i = 0; i < sizeof(FilenameLong) - 1; i++)
    {
        FilenameLong[i] = 'a';
    }
    FilenameLong[i] = '\0'; /* Null terminate file name string */

    UtAssert_INT32_EQ(CFE_TBL_LoadFromFile("UT", WorkingBufferPtr, RegRecPtr, FilenameLong),
                      CFE_TBL_ERR_FILENAME_TOO_LONG);
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOAD_FILENAME_LONG_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

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
    UtAssert_INT32_EQ(CFE_TBL_LoadFromFile("UT", WorkingBufferPtr, RegRecPtr, Filename), CFE_TBL_ERR_FILE_TOO_LARGE);
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOAD_EXCEEDS_SIZE_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

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
    UtAssert_INT32_EQ(CFE_TBL_LoadFromFile("UT", WorkingBufferPtr, RegRecPtr, Filename), CFE_TBL_ERR_FILE_TOO_LARGE);
    CFE_UtAssert_EVENTSENT(CFE_TBL_FILE_TOO_BIG_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

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
    UtAssert_INT32_EQ(CFE_TBL_LoadFromFile("UT", WorkingBufferPtr, RegRecPtr, Filename), CFE_TBL_ERR_LOAD_INCOMPLETE);
    CFE_UtAssert_EVENTSENT(CFE_TBL_FILE_INCOMPLETE_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test CFE_TBL_LoadFromFile with failure of data OS_read */
    UT_InitData();
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType     = CFE_FS_SubType_TBL_IMG;
    strncpy(TblFileHeader.TableName, "ut_cfe_tbl.UT_Table2", sizeof(TblFileHeader.TableName) - 1);
    TblFileHeader.TableName[sizeof(TblFileHeader.TableName) - 1] = '\0';
    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(UT_Table1_t));

    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 2, -1);
    UtAssert_INT32_EQ(CFE_TBL_LoadFromFile("UT", WorkingBufferPtr, RegRecPtr, Filename), CFE_TBL_ERR_LOAD_INCOMPLETE);
    CFE_UtAssert_EVENTSENT(CFE_TBL_FILE_INCOMPLETE_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test CFE_TBL_LoadFromFile with failure of extra byte OS_read */
    UT_InitData();
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType     = CFE_FS_SubType_TBL_IMG;
    strncpy(TblFileHeader.TableName, "ut_cfe_tbl.UT_Table2", sizeof(TblFileHeader.TableName) - 1);
    TblFileHeader.TableName[sizeof(TblFileHeader.TableName) - 1] = '\0';
    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(UT_Table1_t));

    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, -1);
    CFE_UtAssert_SUCCESS(CFE_TBL_LoadFromFile("UT", WorkingBufferPtr, RegRecPtr, Filename));
    CFE_UtAssert_EVENTCOUNT(0);

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
    UtAssert_INT32_EQ(CFE_TBL_LoadFromFile("UT", WorkingBufferPtr, RegRecPtr, Filename),
                      CFE_TBL_ERR_FILE_FOR_WRONG_TABLE);
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOAD_TBLNAME_MISMATCH_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

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
    UtAssert_INT32_EQ(CFE_TBL_LoadFromFile("UT", WorkingBufferPtr, RegRecPtr, Filename), CFE_TBL_ERR_ACCESS);
    CFE_UtAssert_EVENTSENT(CFE_TBL_FILE_ACCESS_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

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
    UtAssert_INT32_EQ(CFE_TBL_LoadFromFile("UT", WorkingBufferPtr, RegRecPtr, Filename), CFE_TBL_WARN_SHORT_FILE);
    CFE_UtAssert_EVENTCOUNT(0);

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
    UtAssert_INT32_EQ(CFE_TBL_ReadHeaders(FileDescriptor, &StdFileHeader, &TblFileHeader, Filename),
                      CFE_TBL_ERR_NO_STD_HEADER);
    CFE_UtAssert_EVENTSENT(CFE_TBL_FILE_STD_HDR_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

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
    UtAssert_INT32_EQ(CFE_TBL_ReadHeaders(FileDescriptor, &StdFileHeader, &TblFileHeader, Filename),
                      CFE_TBL_ERR_BAD_CONTENT_ID);
    CFE_UtAssert_EVENTSENT(CFE_TBL_FILE_TYPE_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test CFE_TBL_ReadHeaders response to a wrong cFE file subtype */
    UT_InitData();
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType     = CFE_FS_SubType_TBL_IMG - 1;
    strncpy(TblFileHeader.TableName, "ut_cfe_tbl.UT_Table1", sizeof(TblFileHeader.TableName) - 1);
    TblFileHeader.TableName[sizeof(TblFileHeader.TableName) - 1] = '\0';
    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(UT_Table1_t) - 1);

    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UtAssert_INT32_EQ(CFE_TBL_ReadHeaders(FileDescriptor, &StdFileHeader, &TblFileHeader, Filename),
                      CFE_TBL_ERR_BAD_SUBTYPE_ID);
    CFE_UtAssert_EVENTSENT(CFE_TBL_FILE_SUBTYPE_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

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
    UtAssert_INT32_EQ(CFE_TBL_ReadHeaders(FileDescriptor, &StdFileHeader, &TblFileHeader, Filename),
                      CFE_TBL_ERR_NO_TBL_HEADER);
    CFE_UtAssert_EVENTSENT(CFE_TBL_FILE_TBL_HDR_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test CFE_TBL_TxnRemoveAccessLink response to a failure to put back the
     * memory buffer for a double buffered table
     * Note: CFE_TBL_Unregister() does not propagate this error to the caller,
     * as there is no recourse and the table is still unregistered.  However, it
     * is invoked here for internal coverage paths.
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_PutPoolBuf), 2, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_TBL_Unregister(App1TblHandle2), CFE_SUCCESS);
    CFE_UtAssert_EVENTCOUNT(0);

    /* EarlyInit - Table Registry Mutex Create Failure */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemCreate), 1, OS_ERROR);
    UtAssert_INT32_EQ(CFE_TBL_EarlyInit(), CFE_STATUS_EXTERNAL_RESOURCE_FAIL);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test CFE_TBL_EarlyInit response to a work buffer mutex create failure */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemCreate), 2, OS_ERROR);
    UtAssert_INT32_EQ(CFE_TBL_EarlyInit(), CFE_STATUS_EXTERNAL_RESOURCE_FAIL);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test CFE_TBL_EarlyInit response to a memory pool create failure */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_PoolCreate), 1, CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_TBL_EarlyInit(), CFE_ES_BAD_ARGUMENT);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test CFE_TBL_EarlyInit response to a get pool buffer failure */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetPoolBuf), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_TBL_EarlyInit(), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test CFE_TBL_EarlyInit response where the CDS already exists but
     * restore fails
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RegisterCDSEx), 1, CFE_ES_CDS_ALREADY_EXISTS);
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RestoreFromCDS), 1, CFE_ES_CDS_BLOCK_CRC_ERR);
    UT_ResetState(UT_KEY(CFE_ES_GetPoolBuf));
    UT_SetDataBuffer(UT_KEY(CFE_ES_GetPoolBuf), &UT_TBL_LoadBuffer, sizeof(UT_TBL_LoadBuffer), false);
    CFE_UtAssert_SUCCESS(CFE_TBL_EarlyInit());
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test CFE_TBL_EarlyInit response when no CDS is available */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RegisterCDSEx), 1, CFE_ES_NOT_IMPLEMENTED);
    UT_ResetState(UT_KEY(CFE_ES_GetPoolBuf));
    UT_SetDataBuffer(UT_KEY(CFE_ES_GetPoolBuf), &UT_TBL_LoadBuffer, sizeof(UT_TBL_LoadBuffer), false);
    CFE_UtAssert_SUCCESS(CFE_TBL_EarlyInit());
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test CFE_TBL_EarlyInit response to a failure to save a critical table
     * registry to the CDS
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CopyToCDS), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UT_ResetState(UT_KEY(CFE_ES_GetPoolBuf));
    UT_SetDataBuffer(UT_KEY(CFE_ES_GetPoolBuf), &UT_TBL_LoadBuffer, sizeof(UT_TBL_LoadBuffer), false);
    CFE_UtAssert_SUCCESS(CFE_TBL_EarlyInit());
    CFE_UtAssert_EVENTCOUNT(0);

    /* Reset, then register tables for subsequent tests */
    /* a. Reset tables */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    UT_ResetState(UT_KEY(CFE_ES_GetPoolBuf));
    UT_SetDataBuffer(UT_KEY(CFE_ES_GetPoolBuf), &UT_TBL_LoadBuffer, sizeof(UT_TBL_LoadBuffer), false);
    CFE_UtAssert_SUCCESS(CFE_TBL_EarlyInit());

    /* b. Register critical single buffered table */
    UT_InitData();
    CFE_UtAssert_SUCCESS(CFE_TBL_Register(&App1TblHandle1, "UT_Table1", sizeof(UT_Table1_t),
                                          CFE_TBL_OPT_DEFAULT | CFE_TBL_OPT_CRITICAL, Test_CFE_TBL_ValidationFunc));

    /* c. Register critical double buffered table */
    UT_InitData();
    CFE_UtAssert_SUCCESS(CFE_TBL_Register(&App1TblHandle2, "UT_Table2", sizeof(UT_Table1_t),
                                          CFE_TBL_OPT_DBL_BUFFER | CFE_TBL_OPT_CRITICAL, Test_CFE_TBL_ValidationFunc));

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
    CFE_UtAssert_SUCCESS(CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_FILE, "TblSrcFileName.dat"));
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOAD_SUCCESS_INF_EID);
    CFE_UtAssert_EVENTCOUNT(1);

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
    CFE_UtAssert_SUCCESS(CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_FILE, "TblSrcFileName.dat"));
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOAD_SUCCESS_INF_EID);
    CFE_UtAssert_EVENTCOUNT(1);

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
    CFE_UtAssert_SUCCESS(CFE_TBL_Load(App1TblHandle2, CFE_TBL_SRC_FILE, "TblSrcFileName.dat"));
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOAD_SUCCESS_INF_EID);
    CFE_UtAssert_EVENTCOUNT(1);

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
    CFE_UtAssert_SUCCESS(CFE_TBL_Load(App1TblHandle2, CFE_TBL_SRC_FILE, "TblSrcFileName.dat"));
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOAD_SUCCESS_INF_EID);
    CFE_UtAssert_EVENTCOUNT(1);

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
    UtAssert_INT32_EQ(CFE_TBL_Load(App1TblHandle2, CFE_TBL_SRC_FILE, "TblSrcFileName.dat"),
                      CFE_TBL_ERR_NO_BUFFER_AVAIL);
    CFE_UtAssert_EVENTSENT(CFE_TBL_NO_WORK_BUFFERS_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Release buffer for error writing to CDS test */
    /* a. Get table address */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_GetAddress(&TblPtr, App1TblHandle2), CFE_TBL_INFO_UPDATED);
    CFE_UtAssert_EVENTCOUNT(0);

    /* b. Release table address */
    UT_ClearEventHistory();
    CFE_UtAssert_SUCCESS(CFE_TBL_ReleaseAddress(App1TblHandle2));
    CFE_UtAssert_EVENTCOUNT(0);

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
    CFE_UtAssert_SUCCESS(CFE_TBL_Load(App1TblHandle2, CFE_TBL_SRC_FILE, "TblSrcFileName.dat"));
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOAD_SUCCESS_INF_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Release buffer for error writing to CDS (second time) test */
    /* a. Get table address */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_GetAddress(&TblPtr, App1TblHandle2), CFE_TBL_INFO_UPDATED);
    CFE_UtAssert_EVENTCOUNT(0);

    /* b. Release table address */
    UT_ClearEventHistory();
    CFE_UtAssert_SUCCESS(CFE_TBL_ReleaseAddress(App1TblHandle2));
    CFE_UtAssert_EVENTCOUNT(0);

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
    CFE_UtAssert_SUCCESS(CFE_TBL_Load(App1TblHandle2, CFE_TBL_SRC_FILE, "TblSrcFileName.dat"));
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOAD_SUCCESS_INF_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Release buffer for failure to find the CDS handle in the CDS
     * registry test
     */
    /* a. Get table address */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_GetAddress(&TblPtr, App1TblHandle2), CFE_TBL_INFO_UPDATED);
    CFE_UtAssert_EVENTCOUNT(0);

    /* b. Release table address */
    UT_ClearEventHistory();
    CFE_UtAssert_SUCCESS(CFE_TBL_ReleaseAddress(App1TblHandle2));
    CFE_UtAssert_EVENTCOUNT(0);

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

    CFE_UtAssert_SUCCESS(CFE_TBL_Load(App1TblHandle2, CFE_TBL_SRC_FILE, "TblSrcFileName.dat"));
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOAD_SUCCESS_INF_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test unregistering a shared table */
    /* a. Share table */
    UT_InitData();
    CFE_TBL_Global.CritReg[0].CDSHandle = RegRecPtr->CDSHandle;
    UT_SetAppID(UT_TBL_APPID_2);
    CFE_UtAssert_SUCCESS(CFE_TBL_Share(&App2TblHandle1, "ut_cfe_tbl.UT_Table1"));

    /* b. Perform test */
    UT_ClearEventHistory();
    CFE_UtAssert_SUCCESS(CFE_TBL_Unregister(App2TblHandle1));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test successful application cleanup */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_PutPoolBuf), -1);
    AccessDescPtr    = &CFE_TBL_Global.Handles[App1TblHandle1];
    RegRecPtr        = &CFE_TBL_Global.Registry[AccessDescPtr->RegIndex];
    WorkingBufferPtr = &CFE_TBL_Global.LoadBuffs[1];

    UT_TBL_SetupPendingDump(3, WorkingBufferPtr, RegRecPtr, &DumpCtrlPtr);

    RegRecPtr->LoadInProgress = 1;
    WorkingBufferPtr->Taken   = true;
    CFE_UtAssert_SUCCESS(CFE_TBL_CleanUpApp(UT_TBL_APPID_1));
    UtAssert_INT32_EQ(DumpCtrlPtr->State, CFE_TBL_DUMP_FREE);
    CFE_UtAssert_RESOURCEID_EQ(RegRecPtr->OwnerAppId, CFE_TBL_NOT_OWNED);
    UtAssert_BOOL_FALSE(WorkingBufferPtr->Taken);
    UtAssert_INT32_EQ(RegRecPtr->LoadInProgress, CFE_TBL_NO_LOAD_IN_PROGRESS);

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
    UtAssert_INT32_EQ(CFE_TBL_Load(App1TblHandle2, CFE_TBL_SRC_FILE, "TblSrcFileName.dat"), CFE_TBL_ERR_INVALID_HANDLE);
    CFE_UtAssert_EVENTSENT(CFE_TBL_HANDLE_ACCESS_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test CFE_TBL_EarlyInit response where the CDS already exists and
     * restore succeeds
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RegisterCDSEx), 1, CFE_ES_CDS_ALREADY_EXISTS);
    UT_ResetState(UT_KEY(CFE_ES_GetPoolBuf));
    UT_SetDataBuffer(UT_KEY(CFE_ES_GetPoolBuf), &UT_TBL_LoadBuffer, sizeof(UT_TBL_LoadBuffer), false);
    CFE_UtAssert_SUCCESS(CFE_TBL_EarlyInit());
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test starting a transaction where the handle is OK but the underlying registry record is invalid */
    UT_InitData();
    memset(&Txn, 0, sizeof(Txn));
    CFE_TBL_Global.Handles[2].UsedFlag = true;
    CFE_TBL_Global.Handles[2].RegIndex = CFE_TBL_END_OF_LIST;
    UtAssert_INT32_EQ(CFE_TBL_TxnStartFromHandle(&Txn, App2TblHandle1, 0), CFE_TBL_ERR_UNREGISTERED);

    UT_InitData();
    memset(&Txn, 0, sizeof(Txn));
    CFE_TBL_Global.Handles[2].UsedFlag = true;
    CFE_TBL_Global.Handles[2].RegIndex = 1 + CFE_PLATFORM_TBL_MAX_NUM_TABLES;
    UtAssert_INT32_EQ(CFE_TBL_TxnStartFromHandle(&Txn, App2TblHandle1, 0), CFE_TBL_ERR_UNREGISTERED);
    CFE_TBL_Global.Handles[2].UsedFlag = false;

    /* Test CFE_TBL_TxnAllocateRegistryEntry response when the registry entry is
     * not owned but is not at the end of the list
     */
    UT_InitData();
    memset(&Txn, 0, sizeof(Txn));
    CFE_TBL_Global.Registry[0].OwnerAppId      = CFE_TBL_NOT_OWNED;
    CFE_TBL_Global.Registry[0].AccessList.Next = CFE_TBL_END_OF_LIST + 1;
    CFE_UtAssert_SUCCESS(CFE_TBL_TxnAllocateRegistryEntry(&Txn));
    UtAssert_INT32_EQ(CFE_TBL_TxnRegId(&Txn), 1);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test CFE_TBL_LockRegistry response when an error occurs taking the mutex
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemTake), 1, OS_ERROR);
    UtAssert_INT32_EQ(CFE_TBL_LockRegistry(), CFE_STATUS_EXTERNAL_RESOURCE_FAIL);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test CFE_TBL_UnlockRegistry response when an error occurs giving the
     * mutex
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemGive), 1, OS_ERROR);
    UtAssert_INT32_EQ(CFE_TBL_UnlockRegistry(), CFE_STATUS_EXTERNAL_RESOURCE_FAIL);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test CFE_TBL_LoadFromFile response to an invalid header length */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_ReadHeader), 1, sizeof(CFE_FS_Header_t) - 1);
    UtAssert_INT32_EQ(CFE_TBL_LoadFromFile("UT", WorkingBufferPtr, RegRecPtr, Filename), CFE_TBL_ERR_NO_STD_HEADER);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test CFE_TBL_UpdateInternal response when an inactive buffer is ready to
     * be copied but a load is in progress
     */
    UT_InitData();
    AccessDescPtr             = &CFE_TBL_Global.Handles[App1TblHandle2];
    RegRecPtr                 = &CFE_TBL_Global.Registry[AccessDescPtr->RegIndex];
    RegRecPtr->LoadPending    = true;
    RegRecPtr->LoadInProgress = CFE_TBL_NO_LOAD_IN_PROGRESS;
    UtAssert_INT32_EQ(CFE_TBL_UpdateInternal(App1TblHandle2, RegRecPtr, AccessDescPtr), CFE_TBL_INFO_NO_UPDATE_PENDING);
    CFE_UtAssert_EVENTCOUNT(0);

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
    CFE_UtAssert_SUCCESS(CFE_TBL_UpdateInternal(App1TblHandle2, RegRecPtr, AccessDescPtr));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test CFE_TBL_UpdateInternal single buffer memcpy when
     * source and dest are not equal
     */
    UT_InitData();
    AccessDescPtr             = &CFE_TBL_Global.Handles[App1TblHandle2];
    RegRecPtr                 = &CFE_TBL_Global.Registry[AccessDescPtr->RegIndex];
    RegRecPtr->LoadPending    = true;
    RegRecPtr->LoadInProgress = CFE_TBL_NO_LOAD_IN_PROGRESS + 1;
    RegRecPtr->DoubleBuffered = false;
    CFE_UtAssert_SUCCESS(CFE_TBL_UpdateInternal(App1TblHandle2, RegRecPtr, AccessDescPtr));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test CFE_TBL_UpdateInternal with overlapping memcopy (bug) */
    UT_InitData();
    AccessDescPtr                   = &CFE_TBL_Global.Handles[App1TblHandle2];
    RegRecPtr                       = &CFE_TBL_Global.Registry[AccessDescPtr->RegIndex];
    RegRecPtr->LoadPending          = true;
    RegRecPtr->LoadInProgress       = CFE_TBL_NO_LOAD_IN_PROGRESS + 1;
    RegRecPtr->DoubleBuffered       = false;
    RegRecPtr->Buffers[0].BufferPtr = CFE_TBL_Global.LoadBuffs[RegRecPtr->LoadInProgress].BufferPtr;
    CFE_UtAssert_SUCCESS(CFE_TBL_UpdateInternal(App1TblHandle2, RegRecPtr, AccessDescPtr));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test application cleanup where there are no dumped tables to delete,
     * one unused handle and one used but not owned table
     */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_PutPoolBuf), -1);
    RegRecPtr = &CFE_TBL_Global.Registry[0];
    UT_TBL_SetupPendingDump(3, NULL, RegRecPtr, &DumpCtrlPtr);

    CFE_TBL_Global.Handles[0].AppId    = UT_TBL_APPID_1;
    CFE_TBL_Global.Handles[0].UsedFlag = true;
    CFE_TBL_Global.Handles[0].RegIndex = 0;
    RegRecPtr->OwnerAppId              = UT_TBL_APPID_2;
    CFE_TBL_Global.Handles[1].AppId    = UT_TBL_APPID_1;
    CFE_TBL_Global.Handles[1].UsedFlag = false;

    CFE_UtAssert_SUCCESS(CFE_TBL_CleanUpApp(UT_TBL_APPID_1));
    UtAssert_INT32_EQ(DumpCtrlPtr->State, CFE_TBL_DUMP_PENDING);
    CFE_UtAssert_RESOURCEID_EQ(RegRecPtr->OwnerAppId, UT_TBL_APPID_2);

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
    UtAssert_INT32_EQ(CFE_TBL_ReadHeaders(FileDescriptor, &StdFileHeader, &TblFileHeader, Filename),
                      CFE_TBL_ERR_BAD_SPACECRAFT_ID);
    CFE_UtAssert_EVENTSENT(CFE_TBL_SPACECRAFT_ID_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);
#else
    UtAssert_NA("*Not tested* Invalid spacecraft ID ");
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
    UtAssert_INT32_EQ(CFE_TBL_ReadHeaders(FileDescriptor, &StdFileHeader, &TblFileHeader, Filename),
                      CFE_TBL_ERR_BAD_PROCESSOR_ID);
    CFE_UtAssert_EVENTSENT(CFE_TBL_PROCESSOR_ID_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);
#else
    UtAssert_NA("*Not tested* Invalid processor ID ");
#endif

    /* Test CFE_TBL_RestoreTableDataFromCDS() when failed to get a working buffer */
    UT_InitData();

    RegRecPtr = &CFE_TBL_Global.Registry[0];

    RegRecPtr->DoubleBuffered  = false;
    RegRecPtr->TableLoadedOnce = true;

    for (i = 0; i < CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS; i++)
    {
        CFE_TBL_Global.LoadBuffs[i].Taken = true;
    }

    UtAssert_INT32_EQ(CFE_TBL_RestoreTableDataFromCDS(RegRecPtr, "UT", "UT1", NULL), CFE_TBL_ERR_NO_BUFFER_AVAIL);

    UT_ClearEventHistory();

    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_ValidateTableSize("UT", 0, 0), CFE_TBL_ERR_INVALID_SIZE);
    UtAssert_INT32_EQ(CFE_TBL_ValidateTableSize("UT", CFE_PLATFORM_TBL_MAX_SNGL_TABLE_SIZE, CFE_TBL_OPT_SNGL_BUFFER),
                      CFE_SUCCESS);
    UtAssert_INT32_EQ(
        CFE_TBL_ValidateTableSize("UT", CFE_PLATFORM_TBL_MAX_SNGL_TABLE_SIZE + 1, CFE_TBL_OPT_SNGL_BUFFER),
        CFE_TBL_ERR_INVALID_SIZE);
    UtAssert_INT32_EQ(CFE_TBL_ValidateTableSize("UT", CFE_PLATFORM_TBL_MAX_DBL_TABLE_SIZE + 1, 0),
                      CFE_TBL_ERR_INVALID_SIZE);
    UtAssert_INT32_EQ(CFE_TBL_ValidateTableSize("UT", CFE_PLATFORM_TBL_MAX_DBL_TABLE_SIZE, CFE_TBL_OPT_DBL_BUFFER),
                      CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_TBL_ValidateTableSize("UT", CFE_PLATFORM_TBL_MAX_DBL_TABLE_SIZE + 1, CFE_TBL_OPT_DBL_BUFFER),
                      CFE_TBL_ERR_INVALID_SIZE);
}

/*
 * Tests the resource accessors for Validation Results
 */
void Test_CFE_TBL_ResourceID_ValidationResult(void)
{
    uint32                       Idx;
    CFE_TBL_ValidationResultId_t InvalidResultId;
    CFE_TBL_ValidationResultId_t ValidResultId;
    CFE_ResourceId_t             PendingId;

    UT_InitData();

    InvalidResultId = CFE_TBL_VALRESULTID_UNDEFINED;
    UT_SetDefaultReturnValue(UT_KEY(CFE_ResourceId_ToIndex), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_TBL_ValidationResultId_ToIndex(InvalidResultId, &Idx), CFE_ES_ERR_RESOURCEID_NOT_VALID);

    /* by definition, looking up the undefined value should always be NULL */
    UtAssert_NULL(CFE_TBL_LocateValidationResultByID(InvalidResultId));
    UT_ResetState(UT_KEY(CFE_ResourceId_ToIndex));

    ValidResultId = CFE_TBL_VALRESULTID_C(CFE_ResourceId_FromInteger(CFE_TBL_VALRESULTID_BASE + 1));
    UtAssert_INT32_EQ(CFE_TBL_ValidationResultId_ToIndex(ValidResultId, &Idx), CFE_SUCCESS);

    UtAssert_VOIDCALL(PendingId = CFE_TBL_GetNextValResultBlock());
    UtAssert_BOOL_TRUE(CFE_ResourceId_IsDefined(PendingId));

    /* The slot should be available right now */
    UtAssert_BOOL_FALSE(CFE_TBL_CheckValidationResultSlotUsed(PendingId));

    /* Make it used and confirm it is reported as not available */
    CFE_TBL_ValidationResultSetUsed(CFE_TBL_LocateValidationResultByID(CFE_TBL_VALRESULTID_C(PendingId)), PendingId);
    UtAssert_BOOL_TRUE(CFE_TBL_CheckValidationResultSlotUsed(PendingId));

    /* Test case where no ID is available */
    UT_SetDefaultReturnValue(UT_KEY(CFE_ResourceId_FindNext), -1);
    UtAssert_VOIDCALL(PendingId = CFE_TBL_GetNextValResultBlock());
    UtAssert_BOOL_FALSE(CFE_ResourceId_IsDefined(PendingId));

    /* A nonexistent slot is always "unavailable" */
    UtAssert_BOOL_TRUE(CFE_TBL_CheckValidationResultSlotUsed(PendingId));
    UT_ResetState(UT_KEY(CFE_ResourceId_FindNext));
}

/*
 * Tests the resource accessors for Table Registry Records
 */
void Test_CFE_TBL_ResourceID_RegistryRecord(void)
{
    uint32                 Idx;
    CFE_TBL_RegId_t        InvalidRegId;
    CFE_TBL_RegId_t        ValidRegId;
    CFE_TBL_RegistryRec_t *RegRecPtr;

    UT_InitData();

    InvalidRegId = (CFE_TBL_RegId_t)(-1);
    UtAssert_INT32_EQ(CFE_TBL_RegId_ToIndex(InvalidRegId, &Idx), CFE_TBL_ERR_INVALID_HANDLE);

    /* by definition, looking up the undefined value should always be NULL */
    UtAssert_NULL(CFE_TBL_LocateRegistryRecordByID(InvalidRegId));

    InvalidRegId = (CFE_TBL_RegId_t)(CFE_PLATFORM_TBL_MAX_NUM_TABLES + 1);
    UtAssert_INT32_EQ(CFE_TBL_RegId_ToIndex(InvalidRegId, &Idx), CFE_TBL_ERR_INVALID_HANDLE);

    /* Now with a valid ID */
    ValidRegId = (CFE_TBL_RegId_t)(1);
    UtAssert_INT32_EQ(CFE_TBL_RegId_ToIndex(ValidRegId, &Idx), CFE_SUCCESS);
    UtAssert_UINT32_EQ(Idx, 1);
    UtAssert_NOT_NULL(RegRecPtr = CFE_TBL_LocateRegistryRecordByID(ValidRegId));

    UtAssert_UINT32_EQ(CFE_TBL_RegistryRecordGetID(RegRecPtr), ValidRegId);
}

/*
 * Tests the resource accessors for Table Access Descriptors
 */
void Test_CFE_TBL_ResourceID_AccessDescriptor(void)
{
    uint32           Idx;
    CFE_TBL_Handle_t InvalidHandle;
    CFE_TBL_Handle_t ValidHandle;

    UT_InitData();

    InvalidHandle = (CFE_TBL_Handle_t)(-1);
    UtAssert_INT32_EQ(CFE_TBL_Handle_ToIndex(InvalidHandle, &Idx), CFE_TBL_ERR_INVALID_HANDLE);

    /* by definition, looking up the undefined value should always be NULL */
    UtAssert_NULL(CFE_TBL_LocateRegistryRecordByID(InvalidHandle));

    InvalidHandle = (CFE_TBL_Handle_t)(CFE_PLATFORM_TBL_MAX_NUM_HANDLES + 1);
    UtAssert_INT32_EQ(CFE_TBL_Handle_ToIndex(InvalidHandle, &Idx), CFE_TBL_ERR_INVALID_HANDLE);

    /* Now with a valid ID */
    ValidHandle = (CFE_TBL_Handle_t)(1);
    UtAssert_INT32_EQ(CFE_TBL_RegId_ToIndex(ValidHandle, &Idx), CFE_SUCCESS);
    UtAssert_UINT32_EQ(Idx, 1);
}

/*
 * Tests the resource accessors for Dump Control Blocks
 */
void Test_CFE_TBL_ResourceID_DumpControl(void)
{
    uint32               Idx;
    CFE_TBL_DumpCtrlId_t InvalidBlockId;
    CFE_TBL_DumpCtrlId_t ValidBlockId;
    CFE_ResourceId_t     PendingId;

    UT_InitData();

    InvalidBlockId = CFE_TBL_DUMPCTRLID_UNDEFINED;
    UT_SetDefaultReturnValue(UT_KEY(CFE_ResourceId_ToIndex), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_TBL_DumpCtrlId_ToIndex(InvalidBlockId, &Idx), CFE_ES_ERR_RESOURCEID_NOT_VALID);

    /* by definition, looking up the undefined value should always be NULL */
    UtAssert_NULL(CFE_TBL_LocateDumpCtrlByID(InvalidBlockId));
    UT_ResetState(UT_KEY(CFE_ResourceId_ToIndex));

    ValidBlockId = CFE_TBL_DUMPCTRLID_C(CFE_ResourceId_FromInteger(CFE_TBL_DUMPCTRLID_BASE + 1));
    UtAssert_INT32_EQ(CFE_TBL_DumpCtrlId_ToIndex(ValidBlockId, &Idx), CFE_SUCCESS);

    UtAssert_VOIDCALL(PendingId = CFE_TBL_GetNextDumpCtrlBlock());
    UtAssert_BOOL_TRUE(CFE_ResourceId_IsDefined(PendingId));

    /* The slot should be available right now */
    UtAssert_BOOL_FALSE(CFE_TBL_CheckDumpCtrlSlotUsed(PendingId));

    /* Make it used and confirm it is reported as not available */
    CFE_TBL_DumpCtrlBlockSetUsed(CFE_TBL_LocateDumpCtrlByID(CFE_TBL_DUMPCTRLID_C(PendingId)), PendingId);
    UtAssert_BOOL_TRUE(CFE_TBL_CheckDumpCtrlSlotUsed(PendingId));

    /* Test case where no ID is available */
    UT_SetDefaultReturnValue(UT_KEY(CFE_ResourceId_FindNext), -1);
    UtAssert_VOIDCALL(PendingId = CFE_TBL_GetNextDumpCtrlBlock());
    UtAssert_BOOL_FALSE(CFE_ResourceId_IsDefined(PendingId));

    /* A nonexistent slot is always "unavailable" */
    UtAssert_BOOL_TRUE(CFE_TBL_CheckDumpCtrlSlotUsed(PendingId));
    UT_ResetState(UT_KEY(CFE_ResourceId_FindNext));
}

/*
** Test function executed when the contents of a table need to be validated
*/
int32 Test_CFE_TBL_ValidationFunc(void *TblPtr)
{
    return UT_DEFAULT_IMPL(Test_CFE_TBL_ValidationFunc);
}
