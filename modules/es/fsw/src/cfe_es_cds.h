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
 *  Purpose:
 *  This file contains the Internal interface for the cFE Critical Data Store functions.
 *  These functions and data structures manage the Critical Data Store in the cFE.
 *
 *  References:
 *     Flight Software Branch C Coding Standard Version 1.0a
 *     cFE Flight Software Application Developers Guide
 *
 *  Notes:
 *
 */

#ifndef CFE_ES_CDS_H
#define CFE_ES_CDS_H

/*
** Include Files
*/
#include "common_types.h"
#include "cfe_es_generic_pool.h"

/*
** Macro Definitions
*/

/** \name Registry Mutex Definitions */
/**  \{ */
#define CFE_ES_CDS_MUT_REG_NAME  "CDS_MUTEX" /**< \brief Name of Mutex controlling CDS Access */
#define CFE_ES_CDS_MUT_REG_VALUE 0           /**< \brief Initial Value of CDS Access Mutex */
/** \} */

/** \name Registry Signature Definitions */
/**  \{ */
#define CFE_ES_CDS_SIGNATURE_LEN   8          /**< \brief Length of CDS signature field. */
#define CFE_ES_CDS_SIGNATURE_BEGIN "_CDSBeg_" /**< \brief Fixed signature at beginning of CDS */
#define CFE_ES_CDS_SIGNATURE_END   "_CDSEnd_" /**< \brief Fixed signature at end of CDS */
/** \} */

/*
 * Space in CDS should be aligned to a multiple of uint32
 * These helper macros round up to a whole number of words
 */
#define CDS_SIZE_TO_U32WORDS(x)       (((x) + 3) / sizeof(uint32))
#define CDS_RESERVE_SPACE(name, size) uint32 name[CDS_SIZE_TO_U32WORDS(size)]

/* Define offset addresses for CDS data segments */
#define CDS_SIG_BEGIN_OFFSET offsetof(CFE_ES_CDS_PersistentHeader_t, SignatureBegin)
#define CDS_REG_SIZE_OFFSET  offsetof(CFE_ES_CDS_PersistentHeader_t, RegistrySize)
#define CDS_REG_OFFSET       offsetof(CFE_ES_CDS_PersistentHeader_t, RegistryContent)
#define CDS_POOL_OFFSET      sizeof(CFE_ES_CDS_PersistentHeader_t)

/*
 * Absolute Minimum CDS size conceivably supportable by the implementation.
 * This is the space required for the basic signatures and registry information.
 * It is not possible to create a CDS with a storage area smaller than this.
 */
#define CDS_RESERVED_MIN_SIZE sizeof(CFE_ES_CDS_PersistentHeader_t) + sizeof(CFE_ES_CDS_PersistentTrailer_t)

/*
 * Absolute Maximum Block size conceivably supportable by the implementation.
 * User-defined platform limits (in cfe_platform_cfg.h) may be lower,
 * but this is a hard limit to avoid overflow of a 32 bit integer.
 *
 * This ensures the size is safe for a PSP that uses 32 bit CDS offsets.
 * (It is not anticipated that a CDS would need to exceed this size)
 */
#define CDS_ABS_MAX_BLOCK_SIZE ((size_t)(1 << 30) - sizeof(CFE_ES_CDS_BlockHeader_t))

/*
** Type Definitions
*/

/**
 * The structure cached in RAM for each block within the CDS non-volatile memory
 * This has the basic runtime info without having to go to CDS.
 */
typedef struct
{
    /*
     * Note that the block size and offset stored here are for the
     * total block size.  The CDS code adds is own extra metadata
     * which has a CRC, and therefore the actual user data size is
     * less than this.
     */
    CFE_ES_CDSHandle_t BlockID;     /**< Abstract ID associated with this CDS block */
    size_t             BlockOffset; /**< Start offset of the block in CDS memory */
    size_t             BlockSize;   /**< Size, in bytes, of the CDS memory block */
    char               Name[CFE_MISSION_ES_CDS_MAX_FULL_NAME_LEN];
    bool               Table; /**< \brief Flag that indicates whether CDS contains a Critical Table */
} CFE_ES_CDS_RegRec_t;

typedef struct CFE_ES_CDSBlockHeader
{
    uint32 Crc; /**< CRC of content */
} CFE_ES_CDS_BlockHeader_t;

/*
 * A generic buffer to hold the various objects that need
 * to be cached in RAM from the CDS non-volatile storage.
 */
typedef union CFE_ES_CDS_AccessCacheData
{
    char                     Sig[CFE_ES_CDS_SIGNATURE_LEN]; /**< A signature field (beginning or end) */
    uint32                   RegistrySize;                  /**< Registry Size Field */
    uint32                   Zero[4];                       /**< Used when clearing CDS content */
    CFE_ES_GenPoolBD_t       Desc;                          /**< A generic block descriptor */
    CFE_ES_CDS_BlockHeader_t BlockHeader;                   /**< A user block header */
    CFE_ES_CDS_RegRec_t      RegEntry;                      /**< A registry entry */
} CFE_ES_CDS_AccessCacheData_t;

typedef struct CFE_ES_CDS_AccessCache
{
    CFE_ES_CDS_AccessCacheData_t Data;         /**< Cached data (varies in size) */
    size_t                       Offset;       /**< The offset where Data is cached from */
    size_t                       Size;         /**< The size of cached Data */
    int32                        AccessStatus; /**< The PSP status of the last read/write from CDS memory */
} CFE_ES_CDS_AccessCache_t;

/**
 * Instance data associated with a CDS
 *
 * Currently there is just one global CDS instance (i.e. a singleton)
 * stored in the CFE_ES_Global structure.
 */
typedef struct
{
    /*
     * The generic pool structure
     * This must be the first entry in this structure.
     */
    CFE_ES_GenPoolRecord_t Pool;

    /*
     * Cache of last accessed data block
     * Because CDS memory is not memory mapped, this serves
     * as temporary holding location for data being actively accessed.
     */
    CFE_ES_CDS_AccessCache_t Cache;

    osal_id_t           GenMutex;       /**< \brief Mutex that controls access to CDS and registry */
    size_t              TotalSize;      /**< \brief Total size of the CDS as reported by BSP */
    size_t              DataSize;       /**< \brief Size of actual user data pool */
    CFE_ResourceId_t    LastCDSBlockId; /**< \brief Last issued CDS block ID */
    CFE_ES_CDS_RegRec_t Registry[CFE_PLATFORM_ES_CDS_MAX_NUM_ENTRIES]; /**< \brief CDS Registry (Local Copy) */
} CFE_ES_CDS_Instance_t;

/*
 * structs representing the intended layout of data
 * in the actual CDS/PSP-provided non-volatile memory
 *
 * All blocks should be multiples of uint32
 *
 * NOTE: these aren't necessarily instantiated in RAM,
 * just in CDS.  Mainly interested in the size of these
 * elements, and offset of the various members within.
 */
typedef struct CFE_ES_CDS_PersistentHeader
{
    CDS_RESERVE_SPACE(SignatureBegin, CFE_ES_CDS_SIGNATURE_LEN);
    CDS_RESERVE_SPACE(RegistrySize, sizeof(uint32));
    CDS_RESERVE_SPACE(RegistryContent, CFE_PLATFORM_ES_CDS_MAX_NUM_ENTRIES * sizeof(CFE_ES_CDS_RegRec_t));
} CFE_ES_CDS_PersistentHeader_t;

typedef struct CFE_ES_CDS_PersistentTrailer
{
    CDS_RESERVE_SPACE(SignatureEnd, CFE_ES_CDS_SIGNATURE_LEN);
} CFE_ES_CDS_PersistentTrailer_t;

/*****************************************************************************/
/*
** Function prototypes
*/

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Fetch data from the non-volatile storage and store in RAM cache
 *
 * This fetches a data segment from the PSP and loads it into the
 * local CDS cache buffer.  The content can be accessed via the
 * "Data" member inside the cache structure.
 *
 * Only one thread can use CDS cache at a given time, so the CDS access
 * control mutex must be obtained before calling this function.
 *
 * @param[inout] Cache  the global CDS cache buffer
 * @param[in]    Offset the CDS offset to fetch
 * @param[in]    Size   the CDS data size to fetch
 * @returns #CFE_SUCCESS on success, or appropriate error code.
 */
int32 CFE_ES_CDS_CacheFetch(CFE_ES_CDS_AccessCache_t *Cache, size_t Offset, size_t Size);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Write data from the RAM cache back to non-volatile storage
 *
 * This stores a data segment from the cache into the PSP for
 * permanent storage.  Data should be loaded into the cache
 * prior to invoking this function, either via CFE_ES_CDS_CacheFetch()
 * or CFE_ES_CDS_CachePreload().
 *
 * Only one thread can use CDS cache at a given time, so the CDS access
 * control mutex must be obtained before calling this function.
 *
 * @param[inout] Cache  the global CDS cache buffer
 * @returns #CFE_SUCCESS on success, or appropriate error code.
 */
int32 CFE_ES_CDS_CacheFlush(CFE_ES_CDS_AccessCache_t *Cache);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Preload the cache data from a local buffer
 *
 * This loads the CDS cache directly from a provided object/buffer to
 * prepare for writing to PSP.  The data can then be committed to PSP
 * at a later time using CFE_ES_CDS_CacheFlush().
 *
 * If Source is NULL, then the cache data will be initialized to zero.
 *
 * If Source refers to the cache buffer, then no copying will take place, because
 * source and destination are the same.  No copy is performed, and the data will be
 * unchanged.  In this mode only the size and offset are updated.
 *
 * Only one thread can use CDS cache at a given time, so the CDS access
 * control mutex must be obtained before calling this function.
 *
 * @param[inout] Cache  the global CDS cache buffer
 * @param[in]    Source the local object to load into cache
 * @param[in]    Offset the CDS offset to fetch
 * @param[in]    Size   the CDS data size to fetch
 * @returns #CFE_SUCCESS on success, or appropriate error code.
 */
int32 CFE_ES_CDS_CachePreload(CFE_ES_CDS_AccessCache_t *Cache, const void *Source, size_t Offset, size_t Size);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Get the registry array index correlating with a CDS block ID
 *
 * Calculates the array position/index of the CDS registry entry for
 * the given block ID.
 *
 * @param[in]  BlockID the ID/handle of the CDS block to retrieve
 * @param[out] Idx     Output buffer to store the index
 * @returns    #CFE_SUCCESS if conversion successful. @copydoc CFE_SUCCESS
 *             #CFE_ES_ERR_RESOURCEID_NOT_VALID if block ID is outside valid range
 */
int32 CFE_ES_CDSHandle_ToIndex(CFE_ES_CDSHandle_t BlockID, uint32 *Idx);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Get a registry record within the CDS, given a block ID/handle
 *
 * This only returns a pointer to the table entry where the record
 * should reside, but does _not_ actually check/validate the entry.
 *
 * If the passed-in ID parameter is not within the acceptable range of ID
 * values for CDS blocks, such that it could never be valid under
 * any circumstances, then NULL is returned.  Otherwise, a pointer to the
 * corresponding table entry is returned, indicating the location where
 * that ID _should_ reside, if it is currently in use.
 *
 * @note This only returns where the ID should reside, not that it actually
 * resides there.  If looking up an existing ID, then caller must additionally
 * confirm that the returned record is a match to the expected ID before using
 * or modifying the data within the returned record pointer.
 *
 * The CFE_ES_CDSBlockRecordIsMatch() function can be used to check/confirm
 * if the returned table entry is a positive match for the given ID.
 *
 * @sa CFE_ES_CDSBlockRecordIsMatch()
 *
 * @param[in] BlockID the ID/handle of the CDS block to retrieve
 * @returns   Pointer to registry record, or NULL if ID/handle invalid.
 */
CFE_ES_CDS_RegRec_t *CFE_ES_LocateCDSBlockRecordByID(CFE_ES_CDSHandle_t BlockID);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Check if a Memory Pool record is in use or free/empty
 *
 * This routine checks if the Pool table entry is in use or if it is free
 *
 * As this dereferences fields within the record, global data must be
 * locked prior to invoking this function.
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   CDSBlockRecPtr   pointer to Pool table entry
 * @returns true if the entry is in use/configured, or false if it is free/empty
 */
static inline bool CFE_ES_CDSBlockRecordIsUsed(const CFE_ES_CDS_RegRec_t *CDSBlockRecPtr)
{
    return CFE_RESOURCEID_TEST_DEFINED(CDSBlockRecPtr->BlockID);
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Get the ID value from a Memory Pool table entry
 *
 * This routine converts the table entry back to an abstract ID.
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   CDSBlockRecPtr   pointer to Pool table entry
 * @returns BlockID of entry
 */
static inline CFE_ES_CDSHandle_t CFE_ES_CDSBlockRecordGetID(const CFE_ES_CDS_RegRec_t *CDSBlockRecPtr)
{
    return CDSBlockRecPtr->BlockID;
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Marks a Memory Pool table entry as used (not free)
 *
 * This sets the internal field(s) within this entry, and marks
 * it as being associated with the given Pool ID.
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   CDSBlockRecPtr   pointer to Pool table entry
 * @param[in]   PendingId        the Pool ID of this entry
 */
static inline void CFE_ES_CDSBlockRecordSetUsed(CFE_ES_CDS_RegRec_t *CDSBlockRecPtr, CFE_ResourceId_t PendingId)
{
    CDSBlockRecPtr->BlockID = CFE_ES_CDSHANDLE_C(PendingId);
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Set a Memory Pool record table entry free (not used)
 *
 * This clears the internal field(s) within this entry, and allows the
 * memory to be re-used in the future.
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   CDSBlockRecPtr   pointer to Pool table entry
 */
static inline void CFE_ES_CDSBlockRecordSetFree(CFE_ES_CDS_RegRec_t *CDSBlockRecPtr)
{
    CDSBlockRecPtr->BlockID = CFE_ES_CDS_BAD_HANDLE;
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Check if a CDS block record is a match for the given BlockID
 *
 * This routine confirms that the previously-located record is valid
 * and matches the expected block ID.
 *
 * As this dereferences fields within the record, CDS access mutex must be
 * locked prior to invoking this function.
 *
 * This function may be used in conjunction with CFE_ES_LocateCDSBlockRecordByID()
 * to confirm that the located record is a positive match to the expected ID.
 * As such, the record pointer is also permitted to be NULL, to alleviate the
 * need for the caller to handle this possibility explicitly.
 *
 * Once a record pointer has been successfully validated using this routine,
 * it may be safely passed to all other internal functions.
 *
 * @sa CFE_ES_LocateCDSBlockRecordByID
 *
 * @param[in]   CDSBlockRecPtr   pointer to registry table entry
 * @param[in]   BlockID          expected block ID
 * @returns true if the entry matches the given block ID
 */
static inline bool CFE_ES_CDSBlockRecordIsMatch(const CFE_ES_CDS_RegRec_t *CDSBlockRecPtr, CFE_ES_CDSHandle_t BlockID)
{
    return (CDSBlockRecPtr != NULL && CFE_RESOURCEID_TEST_EQUAL(CDSBlockRecPtr->BlockID, BlockID));
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Gets the data size from a given registry record
 *
 * This computes the usable data size of the CDS registry entry
 *
 * As this dereferences fields within the record, CDS access mutex must be
 * locked prior to invoking this function.
 *
 * @note CDS entries include an extra header in addition to the data,
 * which contains error checking information.  Therefore the usable data
 * size is less than the raw block size.
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   CDSBlockRecPtr   pointer to registry table entry
 * @returns     Usable size of the CDS
 */
static inline size_t CFE_ES_CDSBlockRecordGetUserSize(const CFE_ES_CDS_RegRec_t *CDSBlockRecPtr)
{
    return (CDSBlockRecPtr->BlockSize - sizeof(CFE_ES_CDS_BlockHeader_t));
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Check if a CDS Block ID table slot is used
 *
 * Checks if a table slot is available for a potential new ID
 * This is a helper function intended to be used with
 * CFE_ResourceId_FindNext() for allocating new IDs
 *
 * As this dereferences fields within the record, global data must be
 * locked prior to invoking this function.
 *
 * @param[in]   CheckId       pending/candidate Block ID to check
 * @returns true if the table slot for the ID is occupied, false if available
 */
bool CFE_ES_CheckCDSHandleSlotUsed(CFE_ResourceId_t CheckId);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Initializes CDS data constructs
**
** \par Description
**        Locates and validates any pre-existing CDS memory or initializes the
**        memory as a fresh CDS.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \par SysLog Messages
**
** \return None
**
******************************************************************************/
int32 CFE_ES_CDS_EarlyInit(void);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Determines whether a CDS currently exists
**
** \par Description
**        Reads a set of bytes from the beginning and end of the CDS memory
**        area and determines if a fixed pattern is present, thus determining
**        whether the CDS still likely contains valid data or not.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \return #CFE_SUCCESS         \copydoc CFE_SUCCESS
** \return #CFE_ES_CDS_INVALID  \copydoc CFE_ES_CDS_INVALID
** \return Any of the return values from #CFE_PSP_ReadFromCDS
**
******************************************************************************/
int32 CFE_ES_ValidateCDS(void);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Initializes the CDS Registry
**
** \par Description
**        Initializes the data structure used to keep track of CDS blocks and
**        who they belong to.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \retval #CFE_SUCCESS         \copydoc CFE_SUCCESS
**
******************************************************************************/
int32 CFE_ES_InitCDSRegistry(void);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Rebuilds memory pool for CDS and recovers existing registry
**
** \par Description
**        Scans memory for existing CDS and initializes memory pool and registry
**        settings accordingly
**
** \par Assumptions, External Events, and Notes:
**        -# Assumes the validity of the CDS has already been determined
**
** \return #CFE_SUCCESS         \copydoc CFE_SUCCESS
** \return Any of the return values from #CFE_PSP_ReadFromCDS
**
******************************************************************************/
int32 CFE_ES_RebuildCDS(void);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Copies the local version of the CDS Registry to the actual CDS
**
** \par Description
**        Copies the local working copy of the CDS Registry to the CDS.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \return #CFE_SUCCESS                     \copydoc CFE_SUCCESS
** \return Any of the return values from #CFE_PSP_WriteToCDS
**
******************************************************************************/
int32 CFE_ES_UpdateCDSRegistry(void);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Creates a Full CDS name from application name and CDS name
**
** \par Description
**        Takes a given CDS Name and combines it with the calling
**        Application's name to make a processor specific name of the
**        form: "AppName.CDSName"
**
** \par Assumptions, External Events, and Notes:
**        Note: AppName portion will be truncated to OS_MAX_API_NAME.
**
** \param[in, out]  FullCDSName pointer to character buffer of #CFE_MISSION_ES_CDS_MAX_FULL_NAME_LEN size
**                  that will be filled with the processor specific CDS Name. *FullCDSName is the processor
**                  specific CDS Name of the form "AppName.CDSName".
**
** \param[in]  CDSName pointer to character string containing the Application's local name for
**                     the CDS.
**
** \param[in]  ThisAppId the Application ID of the Application making the call.
**
******************************************************************************/
void CFE_ES_FormCDSName(char *FullCDSName, const char *CDSName, CFE_ES_AppId_t ThisAppId);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Returns the Registry Record for the specified CDS Name
**
** \par Description
**        Locates given CDS Name in the CDS Registry and
**        returns the appropriate Registry Index.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  CDSName - Pointer to character string containing complete
**                       CDS Name (of the format "AppName.CDSName").
**
** \retval NULL if not found, Non null entry pointer on success
**
******************************************************************************/
CFE_ES_CDS_RegRec_t *CFE_ES_LocateCDSBlockRecordByName(const char *CDSName);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Locks access to the CDS
**
** \par Description
**        Locks the CDS to prevent multiple tasks/threads
**        from modifying it at once.
**
**        This lock covers both the registry and the data access.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \retval #CFE_SUCCESS                     \copydoc CFE_SUCCESS
******************************************************************************/
int32 CFE_ES_LockCDS(void);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Unlocks access to the CDS
**
** \par Description
**        Unlocks CDS to allow other tasks/threads to
**        modify the CDS contents.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \retval #CFE_SUCCESS                     \copydoc CFE_SUCCESS
**
******************************************************************************/
int32 CFE_ES_UnlockCDS(void);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Rebuilds memory pool for CDS and recovers existing registry
**
** \par Description
**        Scans memory for existing CDS and initializes memory pool and registry
**        settings accordingly
**
** \par Assumptions, External Events, and Notes:
**        -# Assumes the validity of the CDS has already been determined
**
** \return #CFE_SUCCESS         \copydoc CFE_SUCCESS
** \return Any of the return values from #CFE_PSP_ReadFromCDS
**
******************************************************************************/
int32 CFE_ES_RebuildCDS(void);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Initializes the CDS Registry
**
** \par Description
**        Initializes the data structure used to keep track of CDS blocks and
**        who they belong to.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \retval #CFE_SUCCESS         \copydoc CFE_SUCCESS
**
******************************************************************************/
int32 CFE_ES_InitCDSRegistry(void);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Determines whether a CDS currently exists
**
** \par Description
**        Reads a set of bytes from the beginning and end of the CDS memory
**        area and determines if a fixed pattern is present, thus determining
**        whether the CDS still likely contains valid data or not.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \return #CFE_SUCCESS         \copydoc CFE_SUCCESS
** \return #CFE_ES_CDS_INVALID  \copydoc CFE_ES_CDS_INVALID
** \return Any of the return values from #CFE_PSP_ReadFromCDS
**
******************************************************************************/
int32 CFE_ES_ValidateCDS(void);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Clears the contents of the CDS
**
** \par Description
**        Writes zeros to the entire CDS storage area
**
**        This prevents any stale data that may exist in the
**        memory area from being potentially interpreted as valid
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \return #CFE_SUCCESS          \copydoc CFE_SUCCESS
** \return Any of the return values from #CFE_ES_CDS_CacheFlush
**
******************************************************************************/
int32 CFE_ES_ClearCDS(void);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Initializes the signatures of the CDS area
**
** \par Description
**        Stores a fixed pattern at the beginning and end of the CDS memory
**        to tag it for future verification following a reset.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \return #CFE_SUCCESS          \copydoc CFE_SUCCESS
** \return Any of the return values from #CFE_ES_CDS_CacheFlush
**
******************************************************************************/
int32 CFE_ES_InitCDSSignatures(void);

#endif /* CFE_ES_CDS_H */
