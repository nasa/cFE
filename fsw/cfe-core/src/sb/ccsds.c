/*
**  GSC-18128-1, "Core Flight Executive Version 6.6"
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

/******************************************************************************
** File:  ccsds.c
**
** Purpose:
**      Functions for working with CCSDS headers.
**
******************************************************************************/

/*
** Include Files
*/

#include "common_types.h"
#include "ccsds.h"
#include "osapi.h"
#include "cfe_psp.h"

/******************************************************************************
**  Function:  CCSDS_LoadCheckSum()
**
**  Purpose:
**    Compute and load a checksum for a CCSDS command packet that has a
**    secondary header.
**
**  Arguments:
**    PktPtr   : Pointer to header of command packet.  The packet must
**               have a secondary header and the length in the primary
**               header must be correct.  The checksum field in the packet
**               will be modified.
**
**  Return:
**    (none)
*/

void CCSDS_LoadCheckSum (CCSDS_CommandPacket_t *PktPtr)
{
   uint8    CheckSum;

   /* Clear the checksum field so the new checksum is correct. */
   CCSDS_WR_CHECKSUM(PktPtr->Sec, 0);

   /* Compute and load new checksum. */
   CheckSum = CCSDS_ComputeCheckSum(PktPtr);
   CCSDS_WR_CHECKSUM(PktPtr->Sec, CheckSum);

} /* END CCSDS_LoadCheckSum() */


/******************************************************************************
**  Function:  CCSDS_ValidCheckSum()
**
**  Purpose:
**    Determine whether a checksum in a command packet is valid.
**
**  Arguments:
**    PktPtr   : Pointer to header of command packet.  The packet must
**               have a secondary header and the length in the primary
**               header must be correct.
**
**  Return:
**    true if checksum of packet is valid; false if not.
**    A valid checksum is 0.
*/

bool CCSDS_ValidCheckSum (CCSDS_CommandPacket_t *PktPtr)
{

   return (CCSDS_ComputeCheckSum(PktPtr) == 0);

} /* END CCSDS_ValidCheckSum() */


/******************************************************************************
**  Function:  CCSDS_ComputeCheckSum()
**
**  Purpose:
**    Compute the checksum for a command packet.  The checksum is the XOR of
**    all bytes in the packet; a valid checksum is zero.
**
**  Arguments:
**    PktPtr   : Pointer to header of command packet.  The packet must
**               have a secondary header and the length in the primary
**               header must be correct.
**
**  Return:
**    true if checksum of packet is valid; false if not.
*/

uint8 CCSDS_ComputeCheckSum (CCSDS_CommandPacket_t *PktPtr)
{
   uint16   PktLen   = CCSDS_RD_LEN(PktPtr->SpacePacket.Hdr);
   uint8   *BytePtr  = (uint8 *)PktPtr;
   uint8    CheckSum;

   CheckSum = 0xFF;
   while (PktLen--)  CheckSum ^= *(BytePtr++);

   return CheckSum;

} /* END CCSDS_ComputeCheckSum() */

/*****************************************************************************/
