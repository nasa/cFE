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

/**
 * Definition of the CFE_SB_DestinationD_t type.
 * This was moved into its own header file since it is referenced by multiple CFE modules.
 */

#ifndef CFE_SB_DESTINATION_TYPEDEF_H_
#define CFE_SB_DESTINATION_TYPEDEF_H_

#include "common_types.h"
#include "cfe_sb.h"  /* Required for CFE_SB_PipeId_t definition */

/******************************************************************************
 * This structure defines a DESTINATION DESCRIPTOR used to specify
 * each destination pipe for a message.
 *
 * Note: Changing the size of this structure may require the memory pool
 * block sizes to change.
 */
typedef struct
{
     CFE_SB_PipeId_t PipeId;
     uint8           Active;
     uint16          MsgId2PipeLim;
     uint16          BuffCount;
     uint16          DestCnt;
     uint8           Scope;
     uint8           Spare[3];
     void            *Prev;
     void            *Next;
} CFE_SB_DestinationD_t;

#endif /* CFE_SB_DESTINATION_TYPEDEF_H_ */
