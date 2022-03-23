/*
 * Extended message header field not Zero or F
 */

/*
 * Includes
 */
#include "cfe_msg.h"
#include "test_msg_not.h"

unsigned int Test_MSG_Ext_NotZero(const CFE_MSG_Message_t *MsgPtr)
{
    unsigned int           bits = 0;
    CFE_MSG_EDSVersion_t   edsver;
    CFE_MSG_Endian_t       endian;
    CFE_MSG_PlaybackFlag_t playflag;
    CFE_MSG_Subsystem_t    subsystem;
    CFE_MSG_System_t       system;

    CFE_MSG_GetEDSVersion(MsgPtr, &edsver);
    if (edsver != 0)
        bits |= MSG_EDSVER_FLAG;
    CFE_MSG_GetEndian(MsgPtr, &endian);
    if (endian != CFE_MSG_Endian_Big)
        bits |= MSG_ENDIAN_FLAG;
    CFE_MSG_GetPlaybackFlag(MsgPtr, &playflag);
    if (playflag != CFE_MSG_PlayFlag_Original)
        bits |= MSG_PBACK_FLAG;
    CFE_MSG_GetSubsystem(MsgPtr, &subsystem);
    if (subsystem != 0)
        bits |= MSG_SUBSYS_FLAG;
    CFE_MSG_GetSystem(MsgPtr, &system);
    if (system != 0)
        bits |= MSG_SYSTEM_FLAG;

    return bits;
}

unsigned int Test_MSG_Ext_NotF(const CFE_MSG_Message_t *MsgPtr)
{
    unsigned int           bits = 0;
    CFE_MSG_EDSVersion_t   edsver;
    CFE_MSG_Endian_t       endian;
    CFE_MSG_PlaybackFlag_t playflag;
    CFE_MSG_Subsystem_t    subsystem;
    CFE_MSG_System_t       system;

    CFE_MSG_GetEDSVersion(MsgPtr, &edsver);
    if (edsver != 0x1F)
        bits |= MSG_EDSVER_FLAG;
    CFE_MSG_GetEndian(MsgPtr, &endian);
    if (endian != CFE_MSG_Endian_Little)
        bits |= MSG_ENDIAN_FLAG;
    CFE_MSG_GetPlaybackFlag(MsgPtr, &playflag);
    if (playflag != CFE_MSG_PlayFlag_Playback)
        bits |= MSG_PBACK_FLAG;
    CFE_MSG_GetSubsystem(MsgPtr, &subsystem);
    if (subsystem != 0x1FF)
        bits |= MSG_SUBSYS_FLAG;
    CFE_MSG_GetSystem(MsgPtr, &system);
    if (system != 0xFFFF)
        bits |= MSG_SYSTEM_FLAG;

    return bits;
}
