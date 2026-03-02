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
 * Header definining the table services dump control block and related methods
 */

#ifndef CFE_TBL_DUMPCTL_H
#define CFE_TBL_DUMPCTL_H

/*
** Required header files
*/
#include "common_types.h"
#include "cfe_error.h"
#include "cfe_platform_cfg.h"

#include "cfe_tbl_resource.h"

#include "cfe_tbl_loadbuff.h"
#include "cfe_tbl_regrec.h"

/** \brief Value indicating when no Dump is Pending on a Dump-Only Table */
/**
**  This macro is used to indicate no Dump is Pending by assigning it to
**  #CFE_TBL_RegistryRec_t::DumpControlId
*/
#define CFE_TBL_NO_DUMP_PENDING CFE_TBL_DUMPCTRLID_UNDEFINED

/*******************************************************************************/
/**  \brief Identifies the current state of a dump request.
 */
typedef enum
{
    CFE_TBL_DUMP_FREE = 0, /**< \brief Dump Request Block is Free */
    CFE_TBL_DUMP_PENDING,  /**< \brief Dump Request Block waiting for Application */
    CFE_TBL_DUMP_PERFORMED /**< \brief Dump Request Block processed by Application */
} CFE_TBL_DumpState_t;

/*******************************************************************************/
/**   \brief Dump Control Block
**
**    This structure holds the data associated with a dump request.
*/
struct CFE_TBL_DumpControl
{
    CFE_TBL_DumpCtrlId_t BlockId;

    CFE_TBL_DumpState_t  State;         /**< \brief Current state of this block of data */
    CFE_TBL_LoadBuff_t * DumpBufferPtr; /**< \brief Address where dumped data is to be stored temporarily */
    CFE_TBL_LoadBuffId_t SourceBuffId;  /**< \brief Identifier of buffer to dump */
    char                 TableName[CFE_TBL_MAX_FULL_NAME_LEN]; /**< \brief Name of Table being Dumped */
};

/*
 * ---------------------------------------------------------------------------------------
 *
 *     ~~~  DUMP CONTROL BLOCK ACCESSORS ~~~
 *
 * These operate on CFE_TBL_DumpControl_t* and CFE_TBL_DumpCtrlId_t types
 *
 * ---------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Get the array index correlating with a Dump Control Block ID
 *
 * Calculates the array position/index of the global array entry for
 * the given block ID.
 *
 * @param[in]  DumpCtrlId the ID/handle of the control block to retrieve
 * @param[out] Idx     Output buffer to store the index
 * @returns    #CFE_SUCCESS if conversion successful. @copydoc CFE_SUCCESS
 *             #CFE_ES_ERR_RESOURCEID_NOT_VALID if ID is outside valid range
 */
CFE_Status_t CFE_TBL_DumpCtrlId_ToIndex(CFE_TBL_DumpCtrlId_t DumpCtrlId, uint32 *Idx);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Locate the registry table entry correlating with a given registry ID.
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
 * The CFE_TBL_DumpCtrlBlockIsMatch() function can be used to check/confirm
 * if the returned table entry is a positive match for the given ID.
 *
 * @sa CFE_TBL_DumpCtrlBlockIsMatch()
 *
 * @param[in]   BlockId   the block ID to locate
 * @return pointer to dump control block for the given ID, or NULL if out of range
 */
CFE_TBL_DumpControl_t *CFE_TBL_LocateDumpCtrlByID(CFE_TBL_DumpCtrlId_t BlockId);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Check if a dump control block is a match for the given ID
 *
 * This routine confirms that the previously-located block pointer is valid
 * and matches the expected block ID.
 *
 * As this dereferences fields within the record, global data must be
 * locked prior to invoking this function.
 *
 * This function may be used in conjunction with CFE_TBL_LocateDumpCtrlByID()
 * to confirm that the located record is a positive match to the expected ID.
 * As such, the record pointer is also permitted to be NULL, to alleviate the
 * need for the caller to handle this possibility explicitly.
 *
 * Once a record pointer has been successfully validated using this routine,
 * it may be safely passed to all other internal functions.
 *
 * @sa CFE_TBL_LocateDumpCtrlByID
 *
 * @param[in]   BlockPtr   pointer to validation result table entry, or NULL
 * @param[in]   BlockId     expected validation result ID
 * @returns true if the entry matches the given ID
 */
static inline bool CFE_TBL_DumpCtrlBlockIsMatch(const CFE_TBL_DumpControl_t *BlockPtr, CFE_TBL_DumpCtrlId_t BlockId)
{
    return (BlockPtr != NULL && CFE_RESOURCEID_TEST_EQUAL(BlockPtr->BlockId, BlockId));
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Check if a dump control block is in use or free/empty
 *
 * This routine checks if the block is in use or if it is free
 *
 * As this dereferences fields within the record, global data must be
 * locked prior to invoking this function.
 *
 * @note This internal helper function must only be used on result pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   BlockPtr   pointer to dump control block
 * @returns true if the entry is in use/configured, or false if it is free/empty
 */
static inline bool CFE_TBL_DumpCtrlBlockIsUsed(const CFE_TBL_DumpControl_t *BlockPtr)
{
    return (CFE_TBL_DUMPCTRLID_IS_VALID(BlockPtr->BlockId));
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Marks a dump control block as in use (not avaliable)
 *
 * This sets the internal field(s) within this entry, and marks
 * it as being associated with the given block ID.
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   BlockPtr   pointer to dump control block
 * @param[in]   PendingId        the ID of this entry that will be set
 */
static inline void CFE_TBL_DumpCtrlBlockSetUsed(CFE_TBL_DumpControl_t *BlockPtr, CFE_ResourceId_t PendingId)
{
    BlockPtr->BlockId = CFE_TBL_DUMPCTRLID_C(PendingId);
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Marks a dump control block as available (not in use)
 *
 * This clears the internal field(s) within this entry, and marks
 * it as not being associated with any block ID.
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   BlockPtr   pointer to dump control block
 */
static inline void CFE_TBL_DumpCtrlBlockSetFree(CFE_TBL_DumpControl_t *BlockPtr)
{
    BlockPtr->State   = CFE_TBL_DUMP_FREE;
    BlockPtr->BlockId = CFE_TBL_DUMPCTRLID_UNDEFINED;
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Get the ID value from a dump control block
 *
 * This routine converts the block pointer to its corresponding ID.
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   BlockPtr   pointer to block
 * @returns ID of entry
 */
static inline CFE_TBL_DumpCtrlId_t CFE_TBL_DumpCtrlBlockGetId(const CFE_TBL_DumpControl_t *BlockPtr)
{
    return BlockPtr->BlockId;
}

/**
 * @brief Determine the next ID to use for a dump control block
 *
 * Obtains an ID value that is usable for a new dump control block.  If no blocks
 * are available, then UNDEFINED is returned.
 *
 * @returns ID to use for next control block, or UNDEFINED if no slots available
 */
CFE_ResourceId_t CFE_TBL_GetNextDumpCtrlBlock(void);

/**
 * Test if a slot corresponding to a pending ID is used
 *
 * This is an internal helper function for CFE_ResourceId_FindNext(), and not
 * typically called directly. It is prototyped here for unit testing.
 *
 * @returns True if used, False if available
 */
bool CFE_TBL_CheckDumpCtrlSlotUsed(CFE_ResourceId_t CheckId);

#endif /* CFE_TBL_DUMPCTL_H */
