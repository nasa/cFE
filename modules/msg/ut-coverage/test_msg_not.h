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
