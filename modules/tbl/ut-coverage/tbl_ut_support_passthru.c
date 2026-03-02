/************************************************************************
 * NASA Docket No. GSC-19,200-1, and identified as "cFS Draco"
 *
 * Copyright (c) 2023 United States Government as represented by the
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
 *   Unit test support for pass-through (non-eds) default implementation
 */

/*
** Includes
*/
#include "tbl_UT.h"
#include "tbl_ut_helpers.h"
#include "cfe_core_resourceid_basevalues.h"
#include "cfe_config.h"

/* Normal dispatching registers the MsgID+CC in order to follow a
 * certain path through a series of switch statements */

/* NOTE: Automatic formatting of this table tends to make it harder to read. */
/* clang-format off */

#define TBL_UT_MID_DISPATCH(intf) \
    .Method = UT_TaskPipeDispatchMethod_MSG_ID_CC, \
    .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_TBL_##intf##_MID)

const UT_TaskPipeDispatchId_t UT_TPID_CFE_TBL_MSG_HK =
    { TBL_UT_MID_DISPATCH(SEND_HK), UT_TPD_SETSIZE(CFE_TBL_SendHkCmd) };

const UT_TaskPipeDispatchId_t UT_TPID_CFE_TBL_CMD_NOOP_CC =
    { TBL_UT_MID_DISPATCH(CMD), UT_TPD_SETSIZE(CFE_TBL_NoopCmd), UT_TPD_SETCC(CFE_TBL_NOOP_CC) };

const UT_TaskPipeDispatchId_t UT_TPID_CFE_TBL_CMD_INVALID_LENGTH =
    { TBL_UT_MID_DISPATCH(CMD), UT_TPD_SETERR(CFE_STATUS_WRONG_MSG_LENGTH), UT_TPD_SETCC(CFE_TBL_NOOP_CC) };

const UT_TaskPipeDispatchId_t UT_TPID_CFE_TBL_CMD_RESET_COUNTERS_CC =
    { TBL_UT_MID_DISPATCH(CMD), UT_TPD_SETSIZE(CFE_TBL_ResetCountersCmd), UT_TPD_SETCC(CFE_TBL_RESET_COUNTERS_CC) };

const UT_TaskPipeDispatchId_t UT_TPID_CFE_TBL_INVALID_MID =
    { .Method = UT_TaskPipeDispatchMethod_MSG_ID_CC, UT_TPD_SETERR(CFE_STATUS_UNKNOWN_MSG_ID) };

const UT_TaskPipeDispatchId_t UT_TPID_CFE_TBL_CMD_INVALID_CC =
    { TBL_UT_MID_DISPATCH(CMD), UT_TPD_SETCC(-1), UT_TPD_SETERR(CFE_STATUS_BAD_COMMAND_CODE) };



void UT_TBL_SetupHeader(CFE_TBL_File_Hdr_t *TblFileHeader, size_t Offset, size_t NumBytes, const char *Name)
{
    UT_TBL_SetName(TblFileHeader->TableName, sizeof(TblFileHeader->TableName), Name);
    TblFileHeader->Offset   = Offset;
    TblFileHeader->NumBytes = NumBytes;

    if (UT_Endianess == UT_LITTLE_ENDIAN)
    {
        CFE_TBL_ByteSwapUint32(&TblFileHeader->NumBytes);
        CFE_TBL_ByteSwapUint32(&TblFileHeader->Offset);
    }

    /* In this mode the file binary data is exactly the same as the struct data */
    UT_SetReadBuffer(TblFileHeader, sizeof(*TblFileHeader));
}


void UT_TBL_SetupCodec(size_t ByteSize)
{
    /* classic build has a passthru layer here, this is a no-op */
}

void UT_TBL_RegisterCodecTests(void)
{
    /* classic build has a passthru layer here, this is a no-op */
}
