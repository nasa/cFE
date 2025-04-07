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
 * Header definining the table services load buffer and related methods
 */

#ifndef CFE_TBL_LOADBUFF_H
#define CFE_TBL_LOADBUFF_H

/*
** Required header files
*/
#include "common_types.h"
#include "cfe_error.h"
#include "cfe_platform_cfg.h"

#include "cfe_tbl_resource.h"

#include "cfe_tbl_api_typedefs.h"
#include "cfe_time_api_typedefs.h"

/*************************************************************************/

/*******************************************************************************/
/**   \brief Load Buffer Description Data
**
**     This structure holds a pointer to a table buffer along with its associated
**     data such as the time from the file that was loaded into the buffer, whether
**     the buffer has been allocated and a string describing the source of the data.
*/
struct CFE_TBL_LoadBuff
{
    void *             BufferPtr;     /**< \brief Pointer to Load Buffer */
    CFE_TIME_SysTime_t FileTime;      /**< \brief Time stamp from last file loaded into table */
    uint32             Crc;           /**< \brief Last calculated CRC for this buffer's contents */
    bool               Taken;         /**< \brief Flag indicating whether buffer is in use */
    bool               Validated;     /**< \brief Flag indicating whether the buffer has been successfully validated */
    char DataSource[OS_MAX_PATH_LEN]; /**< \brief Source of data put into buffer (filename or memory address) */
};

/*
 * ---------------------------------------------------------------------------------------
 *
 *     ~~~  LOAD BUFFER ACCESSORS ~~~
 *
 * These operate on CFE_TBL_LoadBuff_t* and buffer ID
 *
 * ---------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Get the array index correlating with a load buffer ID
 *
 * Calculates the array position/index of the global array entry for
 * the given load buffer ID.
 *
 * @param[in]  BuffId the ID/handle of the load buffer to retrieve
 * @param[out] Idx     Output buffer to store the index
 * @returns    #CFE_SUCCESS if conversion successful. @copydoc CFE_SUCCESS
 *             #CFE_ES_ERR_RESOURCEID_NOT_VALID if ID is outside valid range
 */
CFE_Status_t CFE_TBL_LoadBuffId_ToIndex(CFE_TBL_LoadBuffId_t BuffId, uint32 *Idx);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Locate the load buffer table entry correlating with a given load buffer ID.
 *
 * This only returns a pointer to the table entry where the record
 * should reside, but does _not_ actually check/validate the entry.
 *
 * If the passed-in ID parameter is not within the acceptable range of ID
 * values for applications, such that it could never be valid under
 * any circumstances, then NULL is returned.  Otherwise, a pointer to the
 * corresponding table entry is returned, indicating the location where
 * that ID should reside, if it is currently in use.
 *
 * @note This only returns where the ID should reside, not that it actually
 * resides there.  If looking up an existing ID, then caller must additionally
 * confirm that the returned record is a match to the expected ID before using
 * or modifying the data within the returned record pointer.
 *
 * @param[in]   ContextPtr The registry entry this buffer is associated with (NULL for temporary buffers)
 * @param[in]   BufferId   the buffer ID to locate
 * @return pointer to the buffer for the given ID, or NULL if out of range
 */
CFE_TBL_LoadBuff_t *CFE_TBL_LocateLoadBufferByID(CFE_TBL_RegistryRec_t *ContextPtr, CFE_TBL_LoadBuffId_t BufferId);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Get the ID value from a table buffer
 *
 * This routine converts the load buffer pointer to its corresponding ID.
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   LoadBuffPtr   pointer to buffer entry
 * @returns ID of entry
 */
CFE_TBL_LoadBuffId_t CFE_TBL_LoadBufferGetID(const CFE_TBL_LoadBuff_t *LoadBuffPtr);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Check if a load buffer is in use or free/empty
 *
 * This routine checks if the buffer is in use or if it is free
 *
 * As this dereferences fields within the record, global data must be
 * locked prior to invoking this function.
 *
 * @note This internal helper function must only be used on result pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   BuffPtr   pointer to load buffer
 * @returns true if the buffer is in use/configured, or false if it is free/empty
 */
static inline bool CFE_TBL_LoadBuffIsUsed(const CFE_TBL_LoadBuff_t *BuffPtr)
{
    return BuffPtr->Taken;
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Marks a buffer as available (not in use)
 *
 * This clears the internal field(s) within this buffer, and marks
 * it as being available for future use.
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   BuffPtr   pointer to buffer
 */
static inline void CFE_TBL_LoadBuffSetFree(CFE_TBL_LoadBuff_t *BuffPtr)
{
    BuffPtr->Taken = false;
}

#endif /* CFE_TBL_LOADBUFF_H */
