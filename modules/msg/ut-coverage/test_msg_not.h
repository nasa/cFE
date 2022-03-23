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
 * Message header fields not (Zero or F's)
 */

#ifndef TEST_MSG_NOT_H
#define TEST_MSG_NOT_H

/*
 * Defines
 */

/* Field flags */
/* CCSDS Primary */
#define MSG_HDRVER_FLAG   0x00001
#define MSG_TYPE_FLAG     0x00002
#define MSG_HASSEC_FLAG   0x00004
#define MSG_APID_FLAG     0x00008
#define MSG_SEGMENT_FLAG  0x00010
#define MSG_SEQUENCE_FLAG 0x00020
#define MSG_LENGTH_FLAG   0x00040

/* Extended */
#define MSG_EDSVER_FLAG 0x00080
#define MSG_ENDIAN_FLAG 0x00100
#define MSG_PBACK_FLAG  0x00200
#define MSG_SUBSYS_FLAG 0x00400
#define MSG_SYSTEM_FLAG 0x00800

/* Secondary */
#define MSG_CKSUM_FLAG   0x01000
#define MSG_FCNCODE_FLAG 0x02000
#define MSG_TIME_FLAG    0x04000

/*
 * Prototypes
 */

/* Returns flags for fields that are 0 */
unsigned int Test_MSG_NotZero(const CFE_MSG_Message_t *MsgPtr);
unsigned int Test_MSG_Pri_NotZero(const CFE_MSG_Message_t *MsgPtr);
unsigned int Test_MSG_Ext_NotZero(const CFE_MSG_Message_t *MsgPtr);

/* Returns flags for fields that are fully set (aka all F's */
unsigned int Test_MSG_NotF(const CFE_MSG_Message_t *MsgPtr);
unsigned int Test_MSG_Pri_NotF(const CFE_MSG_Message_t *MsgPtr);
unsigned int Test_MSG_Ext_NotF(const CFE_MSG_Message_t *MsgPtr);

#endif /* TEST_MSG_NOT_H */
