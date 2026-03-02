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
 * Header definining the table services validation result buffer and related methods
 */

#ifndef CFE_TBL_VALRESULT_H
#define CFE_TBL_VALRESULT_H

/*
** Required header files
*/
#include "common_types.h"
#include "cfe_error.h"
#include "cfe_tbl_api_typedefs.h"
#include "cfe_tbl_resource.h"

/*************************************************************************/

/** \brief Value indicating when no Validation is Pending */
/**
**  This macro is used to indicate no Validation is Pending by assigning it to
**  #CFE_TBL_RegistryRec_t::ValidateActiveId or #CFE_TBL_RegistryRec_t::ValidateInactiveId
*/
#define CFE_TBL_NO_VALIDATION_PENDING CFE_TBL_VALRESULTID_UNDEFINED

/************************  Internal Structure Definitions  *****************************/

/*******************************************************************************/
/**  \brief Identifies the current state of a validation sequence.
 */
typedef enum
{
    CFE_TBL_VALIDATION_FREE = 0, /**< \brief Validation Result Block is Free */
    CFE_TBL_VALIDATION_PENDING,  /**< \brief Validation Result Block waiting for Application */
    CFE_TBL_VALIDATION_PERFORMED /**< \brief Validation Result Block contains Validation Results */
} CFE_TBL_ValidationState_t;

/*******************************************************************************/
/**   \brief Validation Result Block
**
**    This structure holds the data to be returned to the Operator via telemetry
**    on the results of a Validation request.
*/
struct CFE_TBL_ValidationResult
{
    CFE_TBL_ValidationResultId_t ValId;

    CFE_TBL_ValidationState_t State;      /**< \brief Current state of this block of data */
    int32                     Result;     /**< \brief Result returned by Application's Validation function */
    uint32                    CrcOfTable; /**< \brief Data Integrity Value computed on Table Buffer */
    bool ActiveBuffer;                    /**< \brief Flag indicating whether Validation is on Active/Inactive Buffer */
    char TableName[CFE_TBL_MAX_FULL_NAME_LEN]; /**< \brief Name of Table being Validated */
};

/*
 * ---------------------------------------------------------------------------------------
 *
 *     ~~~  VALIDATION RESULT TABLE ACCESSORS ~~~
 *
 * These operate on CFE_TBL_ValidationResult_t* and CFE_TBL_ValidationResultId_t types
 *
 * ---------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Locate the validation result table entry correlating with a given registry ID.
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
 * The CFE_TBL_ValidationResultIsMatch() function can be used to check/confirm
 * if the returned table entry is a positive match for the given ID.
 *
 * @sa CFE_TBL_ValidationResultIsMatch()
 *
 * @param[in]   ValResultId   the registry ID to locate
 * @return pointer to Validation Result Table entry for the given registry ID, or NULL if out of range
 */
CFE_TBL_ValidationResult_t *CFE_TBL_LocateValidationResultByID(CFE_TBL_ValidationResultId_t ValResultId);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Get the array index correlating with a Validation Result ID
 *
 * Calculates the array position/index of the global array entry for
 * the given result ID.
 *
 * @param[in]  ValResultId the ID of the validation result entry to retrieve
 * @param[out] Idx     Output buffer to store the index
 * @returns    #CFE_SUCCESS if conversion successful. @copydoc CFE_SUCCESS
 *             #CFE_ES_ERR_RESOURCEID_NOT_VALID if ID is outside valid range
 */
CFE_Status_t CFE_TBL_ValidationResultId_ToIndex(CFE_TBL_ValidationResultId_t ValResultId, uint32 *Idx);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Check if a validation result table entry is in use or free/empty
 *
 * This routine checks if the table entry is in use or if it is free
 *
 * As this dereferences fields within the record, global data must be
 * locked prior to invoking this function.
 *
 * @note This internal helper function must only be used on result pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   BuffPtr   pointer to validation result table entry
 * @returns true if the entry is in use/configured, or false if it is free/empty
 */
static inline bool CFE_TBL_ValidationResultIsUsed(const CFE_TBL_ValidationResult_t *BuffPtr)
{
    return (CFE_TBL_VALRESULTID_IS_VALID(BuffPtr->ValId));
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
 * @param[in]   BuffPtr   pointer to validation result table entry
 * @param[in]   PendingId        the ID of this entry that will be set
 */
static inline void CFE_TBL_ValidationResultSetUsed(CFE_TBL_ValidationResult_t *BuffPtr, CFE_ResourceId_t PendingId)
{
    BuffPtr->ValId = CFE_TBL_VALRESULTID_C(PendingId);
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Marks a validation result table entry as available (not in use)
 *
 * This clears the internal field(s) within this entry, and marks
 * it as not being associated with any validation result ID.
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   BuffPtr   pointer to validation result table entry
 */
static inline void CFE_TBL_ValidationResultSetFree(CFE_TBL_ValidationResult_t *BuffPtr)
{
    BuffPtr->State = CFE_TBL_VALIDATION_FREE; /* for backward compatibility; not part of "IsUsed" check anymore */
    BuffPtr->ValId = CFE_TBL_VALRESULTID_UNDEFINED;
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Get the ID value from a validation result table entry
 *
 * This routine converts the table entry pointer to its corresponding ID.
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   BuffPtr   pointer to table entry
 * @returns ID of entry
 */
static inline CFE_TBL_ValidationResultId_t CFE_TBL_ValidationResultGetId(const CFE_TBL_ValidationResult_t *BuffPtr)
{
    return BuffPtr->ValId;
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
 * This function may be used in conjunction with CFE_TBL_LocateValidationResultByID()
 * to confirm that the located record is a positive match to the expected ID.
 * As such, the record pointer is also permitted to be NULL, to alleviate the
 * need for the caller to handle this possibility explicitly.
 *
 * Once a record pointer has been successfully validated using this routine,
 * it may be safely passed to all other internal functions.
 *
 * @sa CFE_TBL_LocateValidationResultByID
 *
 * @param[in]   BuffPtr   pointer to validation result table entry, or NULL
 * @param[in]   ValId     expected validation result ID
 * @returns true if the entry matches the given ID
 */
static inline bool CFE_TBL_ValidationResultIsMatch(const CFE_TBL_ValidationResult_t *BuffPtr,
                                                   CFE_TBL_ValidationResultId_t      ValId)
{
    return (BuffPtr != NULL && CFE_RESOURCEID_TEST_EQUAL(BuffPtr->ValId, ValId));
}

/**
 * @brief Determine the next ID to use for validation results
 *
 * Obtains an ID value that is usable for a new validation result.  If no validation
 * result entries are available, then UNDEFINED is returned.
 *
 * @returns ID to use for next result, or UNDEFINED if no slots available
 */
CFE_ResourceId_t CFE_TBL_GetNextValResultBlock(void);

/**
 * Test if a slot corresponding to a pending ID is used
 *
 * This is an internal helper function for CFE_ResourceId_FindNext(), and not
 * typically called directly. It is prototyped here for unit testing.
 *
 * @returns True if used, False if available
 */
bool CFE_TBL_CheckValidationResultSlotUsed(CFE_ResourceId_t CheckId);

#endif /* CFE_TBL_VALRESULT_H */
