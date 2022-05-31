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
 * Primary message header field not Zero or F
 */

/*
 * Includes
 */
#include "test_cfe_msg_ccsdspri.h" /* For TEST_MSG_SIZE_OFFSET */
#include "cfe_msg.h"
#include "test_msg_not.h"

unsigned int Test_MSG_Pri_NotZero(const CFE_MSG_Message_t *MsgPtr)
{
    unsigned int               bits    = 0;
    CFE_MSG_HeaderVersion_t    hdrver  = 0;
    bool                       hassec  = false;
    CFE_MSG_Type_t             type    = 0;
    CFE_MSG_ApId_t             apid    = 0;
    CFE_MSG_SegmentationFlag_t segflag = 0;
    CFE_MSG_SequenceCount_t    seqcnt  = 0;
    CFE_MSG_Size_t             size    = 0;

    CFE_MSG_GetHeaderVersion(MsgPtr, &hdrver);
    if (hdrver != 0)
        bits |= MSG_HDRVER_FLAG;
    CFE_MSG_GetType(MsgPtr, &type);
    if (type != CFE_MSG_Type_Tlm)
        bits |= MSG_TYPE_FLAG;
    CFE_MSG_GetHasSecondaryHeader(MsgPtr, &hassec);
    if (hassec)
        bits |= MSG_HASSEC_FLAG;
    CFE_MSG_GetApId(MsgPtr, &apid);
    if (apid != 0)
        bits |= MSG_APID_FLAG;
    CFE_MSG_GetSegmentationFlag(MsgPtr, &segflag);
    if (segflag != CFE_MSG_SegFlag_Continue)
        bits |= MSG_SEGMENT_FLAG;
    CFE_MSG_GetSequenceCount(MsgPtr, &seqcnt);
    if (seqcnt != 0)
        bits |= MSG_SEQUENCE_FLAG;
    CFE_MSG_GetSize(MsgPtr, &size);
    if (size != TEST_MSG_SIZE_OFFSET)
        bits |= MSG_LENGTH_FLAG;

    return bits;
}

unsigned int Test_MSG_Pri_NotF(const CFE_MSG_Message_t *MsgPtr)
{
    unsigned int               bits    = 0;
    CFE_MSG_HeaderVersion_t    hdrver  = 0;
    bool                       hassec  = false;
    CFE_MSG_Type_t             type    = 0;
    CFE_MSG_ApId_t             apid    = 0;
    CFE_MSG_SegmentationFlag_t segflag = 0;
    CFE_MSG_SequenceCount_t    seqcnt  = 0;
    CFE_MSG_Size_t             size    = 0;

    CFE_MSG_GetHeaderVersion(MsgPtr, &hdrver);
    if (hdrver != 0x7)
        bits |= MSG_HDRVER_FLAG;
    CFE_MSG_GetType(MsgPtr, &type);
    if (type != CFE_MSG_Type_Cmd)
        bits |= MSG_TYPE_FLAG;
    CFE_MSG_GetHasSecondaryHeader(MsgPtr, &hassec);
    if (!hassec)
        bits |= MSG_HASSEC_FLAG;
    CFE_MSG_GetApId(MsgPtr, &apid);
    if (apid != 0x7FF)
        bits |= MSG_APID_FLAG;
    CFE_MSG_GetSegmentationFlag(MsgPtr, &segflag);
    if (segflag != CFE_MSG_SegFlag_Unsegmented)
        bits |= MSG_SEGMENT_FLAG;
    CFE_MSG_GetSequenceCount(MsgPtr, &seqcnt);
    if (seqcnt != 0x3FFF)
        bits |= MSG_SEQUENCE_FLAG;
    CFE_MSG_GetSize(MsgPtr, &size);
    if (size != 0xFFFF + TEST_MSG_SIZE_OFFSET)
        bits |= MSG_LENGTH_FLAG;

    return bits;
}
