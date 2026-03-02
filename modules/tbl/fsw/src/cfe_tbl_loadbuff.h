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
    CFE_TBL_LoadBuffId_t LoadBufferId;
    CFE_TBL_RegId_t      OwnerRegId;

    void *             BufferPtr;      /**< \brief Pointer to Load Buffer */
    size_t             AllocationSize; /**< \brief Allocated size of the memory to which BufferPtr points */
    size_t             ContentSize;    /**< \brief Current content size */
    CFE_TIME_SysTime_t FileTime;       /**< \brief Time stamp from last file loaded into table */
    uint32             Crc;            /**< \brief Last calculated CRC for this buffer's contents */
    bool               Validated;      /**< \brief Flag indicating whether the buffer has been successfully validated */

    char DataSource[OS_MAX_PATH_LEN]; /**< \brief Source of data put into buffer (filename or memory address) */
};

/**
 * \brief Helper struct for incrementing load buffer serial numbers
 *
 * This is intended for use with CFE_TBL_LoadBuffIncrementSerial(), and should not be used outside
 * of this context.  It needs to be exposed in this header for unit testing purposes only.
 */
typedef struct
{
    uint32 BaseCategory;
    uint32 RemainingCount;

} CFE_TBL_LoadBuffFindNextState_t;

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
 * @param[out] Category     Output buffer to store the category (0 for global >0 regrec index)
 * @param[out] Idx     Output buffer to store the index
 * @returns    #CFE_SUCCESS if conversion successful. @copydoc CFE_SUCCESS
 *             #CFE_ES_ERR_RESOURCEID_NOT_VALID if ID is outside valid range
 */
CFE_Status_t CFE_TBL_LoadBuffId_ToIndex(CFE_TBL_LoadBuffId_t BuffId, uint32 *Category, uint32 *Idx);

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
 * @param[in]   BufferId   the buffer ID to locate
 * @return pointer to the buffer for the given ID, or NULL if out of range
 */
CFE_TBL_LoadBuff_t *CFE_TBL_LocateLoadBufferByID(CFE_TBL_LoadBuffId_t BufferId);

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
static inline CFE_TBL_LoadBuffId_t CFE_TBL_LoadBufferGetID(const CFE_TBL_LoadBuff_t *LoadBuffPtr)
{
    return LoadBuffPtr->LoadBufferId;
}

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
    return CFE_TBL_LOADBUFFID_IS_VALID(BuffPtr->LoadBufferId);
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Marks a validation result table entry as in use (not avaliable)
 *
 * This sets the internal field(s) within this entry, and marks
 * it as being associated with the given validation result ID.
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   BuffPtr     pointer to validation result table entry
 * @param[in]   PendingId   the ID of this entry that will be set
 * @param[in]   OwnerRegId  the ID of the registry entry that owns this buffer
 */
static inline void CFE_TBL_LoadBuffSetUsed(CFE_TBL_LoadBuff_t *BuffPtr, CFE_ResourceId_t PendingId,
                                           CFE_TBL_RegId_t OwnerRegId)
{
    BuffPtr->OwnerRegId   = OwnerRegId;
    BuffPtr->LoadBufferId = CFE_TBL_LOADBUFFID_C(PendingId);
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
    BuffPtr->LoadBufferId = CFE_TBL_LOADBUFFID_UNDEFINED;
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Sets the memory block associated with the load buffer
 *
 * @param[inout]   BuffPtr   pointer to buffer
 * @param[in]      MemPtr    pointer to memory block
 * @param[in]      MemSize   size of memory block
 */
static inline void CFE_TBL_LoadBuffSetAllocatedBlock(CFE_TBL_LoadBuff_t *BuffPtr, void *MemPtr, size_t MemSize)
{
    BuffPtr->BufferPtr      = MemPtr;
    BuffPtr->AllocationSize = MemSize;
    BuffPtr->ContentSize    = 0;
    BuffPtr->Crc            = 0;
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Sets the memory block associated with the load buffer
 *
 * @param[inout]   BuffPtr   pointer to buffer
 * @param[in]      MemPtr    pointer to memory block
 * @param[in]      MemSize   size of memory block
 */
static inline void CFE_TBL_LoadBuffSetExternalBlock(CFE_TBL_LoadBuff_t *BuffPtr, void *MemPtr, size_t MemSize)
{
    BuffPtr->BufferPtr      = MemPtr;
    BuffPtr->ContentSize    = MemSize;
    BuffPtr->AllocationSize = 0;
    BuffPtr->Crc            = 0;
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Checks if the load buffer is allocated
 *
 * @param[in]   BuffPtr   pointer to buffer
 */
static inline bool CFE_TBL_LoadBuffIsAllocated(const CFE_TBL_LoadBuff_t *BuffPtr)
{
    return (BuffPtr->AllocationSize != 0);
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Gets the pointer to buffer data for reading
 *
 * @param[in]   BuffPtr   pointer to buffer
 */
static inline const void *CFE_TBL_LoadBuffGetReadPointer(const CFE_TBL_LoadBuff_t *BuffPtr)
{
    return BuffPtr->BufferPtr;
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Gets the pointer to buffer data for writing
 *
 * Note that Table services does not write to data buffers which it did not allocate itself
 * If this is an external buffer (such as from a dump-only table) then table services should
 * not modify it, however the owning application still could modify it.
 *
 * @param[in]   BuffPtr   pointer to buffer
 */
static inline void *CFE_TBL_LoadBuffGetWritePointer(const CFE_TBL_LoadBuff_t *BuffPtr)
{
    if (!CFE_TBL_LoadBuffIsAllocated(BuffPtr))
    {
        /* Not writeable */
        return NULL;
    }

    return BuffPtr->BufferPtr;
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Gets the allocation size of the buffer
 *
 * This returns the maximum amount of valid data, in bytes, that the buffer is able to hold
 *
 * @param[in]   BuffPtr   pointer to buffer
 */
static inline size_t CFE_TBL_LoadBuffGetAllocSize(const CFE_TBL_LoadBuff_t *BuffPtr)
{
    return BuffPtr->AllocationSize;
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Gets the content size of the buffer
 *
 * This returns the amount of valid data, in bytes, pointed to by BufferPtr
 *
 * @param[in]   BuffPtr   pointer to buffer
 */
static inline size_t CFE_TBL_LoadBuffGetContentSize(const CFE_TBL_LoadBuff_t *BuffPtr)
{
    return BuffPtr->ContentSize;
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Sets the content size of the buffer
 *
 * This sets the amount of valid data, in bytes, pointed to by BufferPtr
 *
 * @param[in]   BuffPtr      pointer to buffer
 * @param[in]   ContentSize  size of content
 */
static inline void CFE_TBL_LoadBuffSetContentSize(CFE_TBL_LoadBuff_t *BuffPtr, size_t ContentSize)
{
    BuffPtr->ContentSize = ContentSize;
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Check if a validation result entry is a match for the given ID
 *
 * This routine confirms that the previously-located result record is valid
 * and matches the expected validation result ID.
 *
 * As this dereferences fields within the record, global data must be
 * locked prior to invoking this function.
 *
 * This function may be used in conjunction with CFE_TBL_LocateLoadBuffByID()
 * to confirm that the located record is a positive match to the expected ID.
 * As such, the record pointer is also permitted to be NULL, to alleviate the
 * need for the caller to handle this possibility explicitly.
 *
 * Once a record pointer has been successfully validated using this routine,
 * it may be safely passed to all other internal functions.
 *
 * @sa CFE_TBL_LocateLoadBuffByID
 *
 * @param[in]   BuffPtr   pointer to validation result table entry, or NULL
 * @param[in]   BufferId  expected load buffer ID
 * @returns true if the entry matches the given ID
 */
static inline bool CFE_TBL_LoadBuffIsMatch(const CFE_TBL_LoadBuff_t *BuffPtr, CFE_TBL_LoadBuffId_t BufferId)
{
    return (BuffPtr != NULL && CFE_TBL_LOADBUFFID_EQ(BuffPtr->LoadBufferId, BufferId));
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Gets the registry record that owns a given load buffer ID
 *
 * Determines the registry record that owns a given load buffer.  For shared/temporary
 * buffers, this is the table that is currently utilizing the shared buffer.  For dedicated
 * buffers, this is the parent registry record.
 *
 * @param[in] BuffId The buffer ID
 * @returns Pointer to registry record that owns the given buffer
 * @retval  NULL if the ID is not valid or it has no owner
 */
CFE_TBL_RegistryRec_t *CFE_TBL_LoadBuffGetRegRecFromId(CFE_TBL_LoadBuffId_t BuffId);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Gets the ID of the next buffer to use on a double-buffered table
**
** \par Description
**        This returns the identifier for the local table buffer that should be
**        loaded next.
**
** \par Assumptions, External Events, and Notes:
**        This is not applicable to single-buffered tables.
**
** \param RegRecPtr The table registry record
** \returns Identifier of next buffer to use
*/
CFE_ResourceId_t CFE_TBL_GetNextLocalBufferId(const CFE_TBL_RegistryRec_t *RegRecPtr);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Compatibility function for Load buffer identifiers in telemetry
 *
 * When identifying load buffers in the TLM stream, table services historically used
 * 0-based positive integers to identify global (shared) buffers, and negative numbers
 * to identify local buffers (e.g. a double buffered table).
 *
 * This translates the Active Buffer ID (which internally is no longer zero based) into
 * a backward compatible integer value for publishing in TLM.
 *
 * @param[in]   BufferId  Buffer ID
 * @returns integer value representing buffer ID in a backward compatible scheme
 */
int32 CFE_TBL_LoadBuffIdToTlmIndex(CFE_TBL_LoadBuffId_t BufferId);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Allocate and set up a temporary load buffer
 *
 * Obtains a table load buffer from the shared buffer pool.  This is intended for
 * temporary storage while loading or dumping a table.  Once complete the buffer must be
 * returned back to the pool for other uses.
 *
 * @param[inout]   RegRecPtr   pointer to Registry table entry
 * @returns Pointer to allocated load buffer
 * @retval  NULL if no buffers are available
 */
CFE_TBL_LoadBuff_t *CFE_TBL_PrepareNewLoadBuff(CFE_TBL_RegistryRec_t *RegRecPtr);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Find the ID of the next available shared load buffer
 *
 * Gets a new shared load buffer identifier.  Note this only finds a suitable ID, it does
 * not actually allocate and initialize a buffer to make it usable.  The related function
 * CFE_TBL_PrepareNewLoadBuff() is the complete operation get a usable load buffer.
 *
 * @sa CFE_TBL_PrepareNewLoadBuff
 *
 * @returns Suitable identifier to use for shared load buffer
 * @retval  CFE_RESOURCEID_UNDEFINED if no identifiers are available
 */
CFE_ResourceId_t CFE_TBL_FindNextSharedBufferId(void);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Decompose a buffer serial number into its constituent parts
 *
 * Convert the serial number into a category and index value.
 *
 * @note This is an internal function for the load buffer management and generally should
 * not be used outside of the loadbuff unit.  It is declared here for unit testing purposes.
 *
 */
void CFE_TBL_LoadBuffSerialDecompose(uint32 Serial, uint32 *CategoryOut, uint32 *IdxOut);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Compose a buffer serial number from its constituent parts
 *
 * Convert the category and index value into a serial number.
 *
 * @note This is an internal function for the load buffer management and generally should
 * not be used outside of the loadbuff unit.  It is declared here for unit testing purposes.
 *
 * @param[inout] SerialInOut Buffer holding previous serial number, will be updated
 * @param[in]    ReqCategory Required category
 * @param[in]    ReqIdx      Required index
 */
void CFE_TBL_LoadBuffSerialCompose(uint32 *SerialInOut, uint32 ReqCategory, uint32 ReqIdx);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Increment a buffer serial number
 *
 * Increment to the next serial number.  This preserves the category value and finds the next
 * serial number that has the same category (only the index value potentially changes).  This
 * function is compatible with CFE_ResourceId_FindNextEx().
 *
 * @sa CFE_ResourceId_FindNextEx()
 *
 * @note This is an internal function for the load buffer management and generally should
 * not be used outside of the loadbuff unit.  It is declared here for unit testing purposes.
 *
 * @param[in]    Id   Initial/current resource ID value
 * @param[inout] Arg  Pointer to a CFE_TBL_LoadBuffFindNextState_t structure
 * @returns ID value that is a logical "++Id" of the initial value
 */
CFE_ResourceId_t CFE_TBL_LoadBuffIncrementSerial(CFE_ResourceId_t Id, void *Arg);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Checks if a buffer slot is used or available
 *
 * Determines if a given resource ID maps to an available load buffer slot. This
 * function is compatible with CFE_ResourceId_FindNextEx().
 *
 * @sa CFE_ResourceId_FindNextEx()
 *
 * @note This is an internal function for the load buffer management and generally should
 * not be used outside of the loadbuff unit.  It is declared here for unit testing purposes.
 *
 * @param[in]    CheckId   number
 * @retval true if slot is used (unavailable)
 * @retval false if slot is unused (available)
 */
bool CFE_TBL_CheckLoadBuffSlotUsed(CFE_ResourceId_t CheckId);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Clears the entire load buffer
 *
 * This will zero out the memory block associated with the load buffer
 *
 * @param[inout] BufferPtr Pointer to load buffer
 */
void CFE_TBL_LoadBuffClearData(CFE_TBL_LoadBuff_t *BufferPtr);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Copy data into the load buffer
 *
 * Copy data from the source address into the load buffer
 *
 * @param[inout] BufferPtr  Pointer to load buffer
 * @param[in]    SourcePtr  Pointer to source data
 * @param[in]    SourceSize Size of source data
 */
void CFE_TBL_LoadBuffCopyData(CFE_TBL_LoadBuff_t *BufferPtr, const void *SourcePtr, size_t SourceSize);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Compute the CRC of the data in the load buffer
 *
 * Calculates the CRC value over the data in the load buffer, and sets the result
 * to the CRC field within the buffer
 *
 * @param[inout] BufferPtr  Pointer to load buffer
 */
void CFE_TBL_LoadBuffRecomputeCRC(CFE_TBL_LoadBuff_t *BufferPtr);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Checks if the given load buffer is private to this registry record
 *
 * Determines if this load buffer is dedicated for use only with this registry record.
 *
 * @param[in] BuffId The buffer ID
 * @param[in] RegId  The Registry ID
 * @retval  true if that buffer is private to that registry entry
 * @retval  false if that buffer is not private to that registry entry
 */
bool CFE_TBL_LoadBuffIsPrivate(CFE_TBL_LoadBuffId_t BuffId, CFE_TBL_RegId_t RegId);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Checks if the given load buffer is part of the shared/global set
 *
 * Determines if this load buffer is a temporary buffer that came from the shared pool
 *
 * @param[in] BuffId The buffer ID
 * @retval  true if that buffer is from the shared pool
 * @retval  false if that buffer is not from the shared pool
 */
bool CFE_TBL_LoadBuffIsShared(CFE_TBL_LoadBuffId_t BuffId);

#endif /* CFE_TBL_LOADBUFF_H */
