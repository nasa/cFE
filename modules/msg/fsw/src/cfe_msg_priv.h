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
