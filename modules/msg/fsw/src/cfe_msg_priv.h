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
 * Message private header
 *  - Avoid including outside this module
 */

#ifndef CFE_MSG_PRIV_H
#define CFE_MSG_PRIV_H

/*
 * Includes
 */
#include "common_types.h"
#include "cfe_msg_hdr.h"

/*---------------------------------------------------------------------------------------*/
/**
 * \brief get generic header field (uint8 array[2])
 *
 * \par DESCRIPTION
 *     Big endian get of header field given mask.  Only sets bits
 *     in value that are part of mask.
 *
 * \param[in]      Word Header value to set
 * \param[out]     Val  Value to set
 * \param[in]      Mask Mask used for set
 */
static inline void CFE_MSG_GetHeaderField(const uint8 *Word, uint16 *Val, uint16 Mask)
{
    *Val = (Word[0] << 8 | Word[1]) & Mask;
}

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Set generic header field (uint8 array[2])
 *
 * \par DESCRIPTION
 *     Big endian set of header field given value and mask.  Only sets bits
 *     from value that are part of mask.
 *
 * \param[in, out] Word Header value to set
 * \param[in]      Val  Value to set
 * \param[in]      Mask Mask used for set
 */
static inline void CFE_MSG_SetHeaderField(uint8 *Word, uint16 Val, uint16 Mask)
{
    Word[0] = (Word[0] & ~(Mask >> 8)) | ((Val & Mask) >> 8);
    Word[1] = ((Word[1] & ~Mask) | (Val & Mask)) & 0xFF;
}

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Initialize default header - implemented based on selected header format
 *
 * \par DESCRIPTION
 *     Sets the constant defaults for the entire header. Internal function
 *     assumes pointer is valid.
 *
 * \param[out]     MsgPtr  Message to set
 */
void CFE_MSG_InitDefaultHdr(CFE_MSG_Message_t *MsgPtr);

#endif /* CFE_MSG_PRIV_H */
