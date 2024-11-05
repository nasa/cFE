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

/*
**  File:
**    cfe_es_cds.c
**
**  Purpose:
**    This file implements the cFE Executive Services Critical Data Store functions.
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**     cFE Flight Software Application Developers Guide
**
**  Notes:
**
**  Modification History:
**
*/

/*
** Required header files.
*/
#include "cfe_es_module_all.h"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_CDS_EarlyInit(void)
{
    CFE_ES_CDS_Instance_t *CDS = &CFE_ES_Global.CDSVars;
    uint32                 PlatformSize;
    size_t                 MinRequiredSize;
    int32                  OsStatus;
    int32                  Status;
    int32                  PspStatus;

    CFE_ES_Global.CDSIsAvailable = false;

    /* Create CDS general access mutex */
    OsStatus = OS_MutSemCreate(&CDS->GenMutex, CFE_ES_CDS_MUT_REG_NAME, CFE_ES_CDS_MUT_REG_VALUE);
    if (OsStatus != OS_SUCCESS)
    {
        CFE_ES_SysLogWrite_Unsync("%s: Failed to create mutex with error %ld\n", __func__, (long)OsStatus);
        return CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
    }

    CDS->LastCDSBlockId = CFE_ResourceId_FromInteger(CFE_ES_CDSBLOCKID_BASE);

    /* Get CDS size from PSP.  Note that the PSP interface
     * uses "uint32" for size here. */
    PspStatus = CFE_PSP_GetCDSSize(&PlatformSize);
    if (PspStatus != CFE_PSP_SUCCESS)
    {
        /* Error getting the size of the CDS from the BSP */
        CFE_ES_WriteToSysLog("%s: Unable to obtain CDS Size from BSP (Err=0x%08X)\n", __func__,
                             (unsigned int)PspStatus);
        return CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
    }

    /* Always truncate the size to the nearest 4 byte boundary */
    CDS->TotalSize = PlatformSize & 0xfffffffc;

    /* Compute the minimum size required for the CDS with the current configuration of the cFE */
    MinRequiredSize = CDS_RESERVED_MIN_SIZE;
    MinRequiredSize += CFE_ES_CDSReqdMinSize(CFE_PLATFORM_ES_CDS_MAX_NUM_ENTRIES); /* Max # of Min Sized Blocks */

    if (CDS->TotalSize < MinRequiredSize)
    {
        CFE_ES_WriteToSysLog("%s: CDS Size (%lu) less than required (%lu)\n", __func__, (unsigned long)CDS->TotalSize,
                             (unsigned long)MinRequiredSize);
        Status = CFE_SUCCESS;
    }
    else
    {
        CDS->DataSize = CDS->TotalSize;
        CDS->DataSize -= CDS_RESERVED_MIN_SIZE;

        /* If the size was obtained successfully and meets the minimum requirements, then check its contents */
        Status = CFE_ES_ValidateCDS();

        if (Status == CFE_SUCCESS)
        {
            /* If a valid CDS was found, rebuild the memory pool */
            Status = CFE_ES_RebuildCDS();
        }

        /* If the CDS is accessible but invalid, then create a new one */
        if (Status == CFE_ES_CDS_INVALID)
        {
            /* First wipe the entire CDS area */
            Status = CFE_ES_ClearCDS();

            if (Status == CFE_SUCCESS)
            {
                Status = CFE_ES_InitCDSSignatures();
            }

            if (Status == CFE_SUCCESS)
            {
                /* Initialize the variables for managing the CDS Memory Pool */
                Status = CFE_ES_CreateCDSPool(CDS->DataSize, CDS_POOL_OFFSET);
            }

            if (Status == CFE_SUCCESS)
            {
                Status = CFE_ES_InitCDSRegistry();
            }
        }

        if (Status != CFE_SUCCESS)
        {
            /* Unrecoverable error while reading the CDS */
            CFE_ES_WriteToSysLog("%s: Error validating/initializing CDS (0x%08lX)\n", __func__, (unsigned long)Status);
        }
        else
        {
            /* Set the CDS Overall flag to be present/valid */
            CFE_ES_Global.CDSIsAvailable = true;
        }
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_CDSHandle_ToIndex(CFE_ES_CDSHandle_t BlockID, uint32 *Idx)
{
    return CFE_ResourceId_ToIndex(CFE_RESOURCEID_UNWRAP(BlockID), CFE_ES_CDSBLOCKID_BASE,
                                  CFE_PLATFORM_ES_CDS_MAX_NUM_ENTRIES, Idx);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
bool CFE_ES_CheckCDSHandleSlotUsed(CFE_ResourceId_t CheckId)
{
    CFE_ES_CDS_RegRec_t *CDSRegRecPtr;
    /*
     * Note - The pointer here should never be NULL because the ID should always be
     * within the expected range, but if it ever is NULL, this should return true
     * such that the caller will _not_ attempt to use the record.
     */
    CDSRegRecPtr = CFE_ES_LocateCDSBlockRecordByID(CFE_ES_CDSHANDLE_C(CheckId));
    return (CDSRegRecPtr == NULL || CFE_ES_CDSBlockRecordIsUsed(CDSRegRecPtr));
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_ES_CDS_RegRec_t *CFE_ES_LocateCDSBlockRecordByID(CFE_ES_CDSHandle_t BlockID)
{
    CFE_ES_CDS_Instance_t *CDS = &CFE_ES_Global.CDSVars;
    CFE_ES_CDS_RegRec_t *  CDSRegRecPtr;
    uint32                 Idx;

    if (CFE_ES_CDSHandle_ToIndex(BlockID, &Idx) == CFE_SUCCESS)
    {
        CDSRegRecPtr = &CDS->Registry[Idx];
    }
    else
    {
        CDSRegRecPtr = NULL;
    }

    return CDSRegRecPtr;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_CDS_CacheFetch(CFE_ES_CDS_AccessCache_t *Cache, size_t Offset, size_t Size)
{
    int32 Status;

    if (Size > 0 && Size <= sizeof(Cache->Data))
    {
        Cache->AccessStatus = CFE_PSP_ReadFromCDS(&Cache->Data, Offset, Size);

        if (Cache->AccessStatus == CFE_PSP_SUCCESS)
        {
            Cache->Offset = Offset;
            Cache->Size   = Size;
            Status        = CFE_SUCCESS;
        }
        else
        {
            Status = CFE_ES_CDS_ACCESS_ERROR;
        }
    }
    else
    {
        Status = CFE_ES_CDS_INVALID_SIZE;
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_CDS_CacheFlush(CFE_ES_CDS_AccessCache_t *Cache)
{
    int32 Status;

    if (Cache->Size > 0 && Cache->Size <= sizeof(Cache->Data))
    {
        Cache->AccessStatus = CFE_PSP_WriteToCDS(&Cache->Data, Cache->Offset, Cache->Size);

        if (Cache->AccessStatus == CFE_PSP_SUCCESS)
        {
            Status = CFE_SUCCESS;
        }
        else
        {
            Status = CFE_ES_CDS_ACCESS_ERROR;
        }
    }
    else
    {
        Status = CFE_ES_CDS_INVALID_SIZE;
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_CDS_CachePreload(CFE_ES_CDS_AccessCache_t *Cache, const void *Source, size_t Offset, size_t Size)
{
    int32 Status;

    if (Size > 0 && Size <= sizeof(Cache->Data))
    {
        if (Source == NULL)
        {
            /* just zero it out */
            memset(&Cache->Data, 0, Size);
        }
        else if (Source != &Cache->Data)
        {
            /* copy from the user-supplied preload data */
            memcpy(&Cache->Data, Source, Size);
        }
        Cache->Size   = Size;
        Cache->Offset = Offset;
        Status        = CFE_SUCCESS;
    }
    else
    {
        Status = CFE_ES_CDS_INVALID_SIZE;
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_RegisterCDSEx(CFE_ES_CDSHandle_t *HandlePtr, size_t UserBlockSize, const char *Name, bool CriticalTbl)
{
    CFE_ES_CDS_Instance_t *CDS = &CFE_ES_Global.CDSVars;
    int32                  Status;
    int32                  RegUpdateStatus;
    CFE_ES_CDS_RegRec_t *  RegRecPtr;
    size_t                 BlockOffset;
    size_t                 OldBlockSize;
    size_t                 NewBlockSize;
    CFE_ResourceId_t       PendingBlockId;
    bool                   IsNewEntry;
    bool                   IsNewOffset;

    Status          = CFE_SUCCESS;
    RegUpdateStatus = CFE_SUCCESS;
    IsNewEntry      = false;
    IsNewOffset     = false;

    if (UserBlockSize == 0 || UserBlockSize > CDS_ABS_MAX_BLOCK_SIZE)
    {
        /* Block size is not supportable */
        return CFE_ES_CDS_INVALID_SIZE;
    }

    /* Lock Registry for update.  This prevents two applications from */
    /* trying to register CDSs at the same location at the same time  */
    CFE_ES_LockCDS();

    /*
     * Check for an existing entry with the same name.
     */
    RegRecPtr = CFE_ES_LocateCDSBlockRecordByName(Name);
    if (RegRecPtr != NULL)
    {
        /* in CDS a duplicate name is not necessarily an error, we
         * may reuse/resize the existing entry */
        PendingBlockId = CFE_RESOURCEID_UNWRAP(CFE_ES_CDSBlockRecordGetID(RegRecPtr));
    }
    else
    {
        /* scan for a free slot */
        PendingBlockId = CFE_ResourceId_FindNext(CDS->LastCDSBlockId, CFE_PLATFORM_ES_CDS_MAX_NUM_ENTRIES,
                                                 CFE_ES_CheckCDSHandleSlotUsed);
        RegRecPtr      = CFE_ES_LocateCDSBlockRecordByID(CFE_ES_CDSHANDLE_C(PendingBlockId));

        if (RegRecPtr != NULL)
        {
            /* Fully clear the entry, just in case of stale data */
            memset(RegRecPtr, 0, sizeof(*RegRecPtr));
            CDS->LastCDSBlockId = PendingBlockId;
            IsNewEntry          = true;
            Status              = CFE_SUCCESS;
        }
        else
        {
            Status         = CFE_ES_NO_RESOURCE_IDS_AVAILABLE;
            PendingBlockId = CFE_RESOURCEID_UNDEFINED;
        }
    }

    if (RegRecPtr != NULL)
    {
        /* Account for the extra header which will be added */
        NewBlockSize = UserBlockSize;
        NewBlockSize += sizeof(CFE_ES_CDS_BlockHeader_t);

        /* If a reallocation is needed, the old block may need to be freed first */
        if (Status == CFE_SUCCESS && RegRecPtr->BlockOffset != 0 && NewBlockSize != RegRecPtr->BlockSize)
        {
            /* If the new size is different, the old CDS must be deleted first  */
            Status = CFE_ES_GenPoolPutBlock(&CDS->Pool, &OldBlockSize, RegRecPtr->BlockOffset);

            /*
             * Note because CDS puts a signature at the very beginning of the memory,
             * valid data offsets are never zero.
             */
            if (Status == CFE_SUCCESS)
            {
                RegRecPtr->BlockOffset = 0;
                RegRecPtr->BlockSize   = 0;
            }
        }

        /* If a new allocation is needed, do it now */
        if (Status == CFE_SUCCESS && RegRecPtr->BlockOffset == 0)
        {
            /* Allocate the block for the CDS */
            Status = CFE_ES_GenPoolGetBlock(&CDS->Pool, &BlockOffset, NewBlockSize);
            if (Status == CFE_SUCCESS)
            {
                /* Save the size of the CDS */
                RegRecPtr->BlockOffset = BlockOffset;
                RegRecPtr->BlockSize   = NewBlockSize;
                IsNewOffset            = true;
            }
        }

        if (Status == CFE_SUCCESS && IsNewEntry)
        {
            /* Save flag indicating whether it is a Critical Table or not */
            RegRecPtr->Table = CriticalTbl;

            /* Save CDS Name in Registry */
            strncpy(RegRecPtr->Name, Name, sizeof(RegRecPtr->Name) - 1);
            RegRecPtr->Name[sizeof(RegRecPtr->Name) - 1] = 0;
            CFE_ES_CDSBlockRecordSetUsed(RegRecPtr, PendingBlockId);
        }

        if (Status == CFE_SUCCESS && IsNewOffset)
        {
            /* If we succeeded at creating a CDS, save updated registry in the CDS */
            RegUpdateStatus = CFE_ES_UpdateCDSRegistry();
        }
    }

    /* Unlock Registry for update */
    CFE_ES_UnlockCDS();

    /* Log any failures AFTER releasing the lock */
    if (RegUpdateStatus != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: Failed to update CDS Registry (Stat=0x%08X)\n", __func__,
                             (unsigned int)RegUpdateStatus);

        Status = RegUpdateStatus;
    }

    if (Status == CFE_SUCCESS && !IsNewOffset)
    {
        /*
         * For backward compatibility, return the
         * special non-success success code when
         * reallocating an existing CDS.
         *
         * Note this intentionally needs to return CFE_SUCCESS
         * when reusing an exiting entry but changing the size.
         */
        Status = CFE_ES_CDS_ALREADY_EXISTS;
    }

    *HandlePtr = CFE_ES_CDSHANDLE_C(PendingBlockId);

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_ValidateCDS(void)
{
    CFE_ES_CDS_Instance_t *CDS = &CFE_ES_Global.CDSVars;
    size_t                 TrailerOffset;
    const size_t           SIG_CDS_SIZE = {CFE_ES_CDS_SIGNATURE_LEN};
    int32                  Status;

    /* Perform 2 checks to validate the CDS Memory Pool */
    /* First, determine if the first validity check field is correct */
    Status = CFE_ES_CDS_CacheFetch(&CDS->Cache, CDS_SIG_BEGIN_OFFSET, SIG_CDS_SIZE);
    if (Status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: 1st ReadFromCDS Failed. Status=0x%X\n", __func__, (unsigned int)Status);
        return Status;
    }

    if (strncmp(CDS->Cache.Data.Sig, CFE_ES_CDS_SIGNATURE_BEGIN, CFE_ES_CDS_SIGNATURE_LEN) != 0)
    {
        /* Beginning Validity Field failed */
        return CFE_ES_CDS_INVALID;
    }

    TrailerOffset = CDS->TotalSize;
    TrailerOffset -= sizeof(CFE_ES_CDS_PersistentTrailer_t);

    Status = CFE_ES_CDS_CacheFetch(&CDS->Cache, TrailerOffset, SIG_CDS_SIZE);
    if (Status != CFE_SUCCESS)
    {
        /* BSP reported an error reading from CDS */
        CFE_ES_WriteToSysLog("%s: 2nd ReadFromCDS Failed. Status=0x%X\n", __func__, (unsigned int)Status);
        return Status;
    }

    if (strncmp(CDS->Cache.Data.Sig, CFE_ES_CDS_SIGNATURE_END, CFE_ES_CDS_SIGNATURE_LEN) != 0)
    {
        /* Ending Validity Field failed */
        return CFE_ES_CDS_INVALID;
    }

    /* All sanity checks passed */
    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_ClearCDS(void)
{
    CFE_ES_CDS_Instance_t *CDS = &CFE_ES_Global.CDSVars;
    size_t                 RemainSize;
    int32                  Status;

    Status = CFE_SUCCESS;

    /* Clear the CDS to ensure everything is gone */
    /* Create a block of zeros to write to the CDS */
    CFE_ES_CDS_CachePreload(&CDS->Cache, NULL, 0, sizeof(CDS->Cache.Data.Zero));

    /* While there is space to write another block of zeros, then do so */
    while (CDS->Cache.Offset < CDS->TotalSize)
    {
        RemainSize = CDS->TotalSize - CDS->Cache.Offset;
        if (RemainSize < sizeof(CDS->Cache.Data.Zero))
        {
            /* partial size */
            CDS->Cache.Size = RemainSize;
        }
        Status = CFE_ES_CDS_CacheFlush(&CDS->Cache);
        if (Status != CFE_SUCCESS)
        {
            break;
        }

        CDS->Cache.Offset += CDS->Cache.Size;
    }

    if (Status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: Clear CDS failed @ Offset=%lu Status=0x%08X\n", __func__,
                             (unsigned long)CDS->Cache.Offset, (unsigned int)CDS->Cache.AccessStatus);
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_InitCDSSignatures(void)
{
    CFE_ES_CDS_Instance_t *CDS = &CFE_ES_Global.CDSVars;
    size_t                 SigOffset;
    int32                  Status;

    /* Initialize the Validity Check strings */
    SigOffset = 0;
    CFE_ES_CDS_CachePreload(&CDS->Cache, CFE_ES_CDS_SIGNATURE_BEGIN, SigOffset, CFE_ES_CDS_SIGNATURE_LEN);
    Status = CFE_ES_CDS_CacheFlush(&CDS->Cache);
    if (Status != CFE_SUCCESS)
    {
        /* BSP reported an error writing to CDS */
        CFE_ES_WriteToSysLog("%s: '_CDSBeg_' write failed. Status=0x%08X\n", __func__,
                             (unsigned int)CDS->Cache.AccessStatus);
        return Status;
    }

    SigOffset = CDS->TotalSize;
    SigOffset -= sizeof(CFE_ES_CDS_PersistentTrailer_t);

    CFE_ES_CDS_CachePreload(&CDS->Cache, CFE_ES_CDS_SIGNATURE_END, SigOffset, CFE_ES_CDS_SIGNATURE_LEN);
    Status = CFE_ES_CDS_CacheFlush(&CDS->Cache);
    if (Status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: '_CDSEnd_' write failed. Status=0x%08X\n", __func__,
                             (unsigned int)CDS->Cache.AccessStatus);
        return Status;
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_InitCDSRegistry(void)
{
    CFE_ES_CDS_Instance_t *CDS = &CFE_ES_Global.CDSVars;
    int32                  Status;
    uint32                 RegSize;

    /* Initialize the local CDS Registry */
    RegSize = CFE_PLATFORM_ES_CDS_MAX_NUM_ENTRIES;
    CFE_ES_CDS_CachePreload(&CDS->Cache, &RegSize, CDS_REG_SIZE_OFFSET, sizeof(RegSize));
    /* Copy the number of registry entries to the CDS */
    Status = CFE_ES_CDS_CacheFlush(&CDS->Cache);
    if (Status == CFE_SUCCESS)
    {
        memset(CDS->Registry, 0, sizeof(CDS->Registry));

        Status = CFE_ES_UpdateCDSRegistry();
    }
    else
    {
        CFE_ES_WriteToSysLog("%s: Failed to write Reg Size. Status=0x%08X\n", __func__,
                             (unsigned int)CDS->Cache.AccessStatus);
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_UpdateCDSRegistry(void)
{
    CFE_ES_CDS_Instance_t *CDS = &CFE_ES_Global.CDSVars;
    int32                  PspStatus;

    /* Copy the contents of the local registry to the CDS */
    PspStatus = CFE_PSP_WriteToCDS(CDS->Registry, CDS_REG_OFFSET, sizeof(CDS->Registry));

    if (PspStatus != CFE_PSP_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: Failed to write CDS Registry. Status=0x%08X\n", __func__, (unsigned int)PspStatus);
        return CFE_ES_CDS_ACCESS_ERROR;
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_ES_FormCDSName(char *FullCDSName, const char *CDSName, CFE_ES_AppId_t ThisAppId)
{
    char AppName[OS_MAX_API_NAME];

    CFE_ES_GetAppName(AppName, ThisAppId, sizeof(AppName));

    /* Ensure that AppName is null terminated */
    AppName[OS_MAX_API_NAME - 1] = '\0';

    /* Complete formation of processor specific table name */
    sprintf(FullCDSName, "%s.%s", AppName, CDSName);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_LockCDS(void)
{
    CFE_ES_CDS_Instance_t *CDS = &CFE_ES_Global.CDSVars;
    int32                  OsStatus;
    int32                  Status;

    OsStatus = OS_MutSemTake(CDS->GenMutex);

    /* Convert to CFE return code */
    if (OsStatus == OS_SUCCESS)
    {
        Status = CFE_SUCCESS;
    }
    else
    {
        Status = CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_UnlockCDS(void)
{
    CFE_ES_CDS_Instance_t *CDS = &CFE_ES_Global.CDSVars;
    int32                  OsStatus;
    int32                  Status;

    OsStatus = OS_MutSemGive(CDS->GenMutex);

    /* Convert to CFE return code */
    if (OsStatus == OS_SUCCESS)
    {
        Status = CFE_SUCCESS;
    }
    else
    {
        Status = CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_ES_CDS_RegRec_t *CFE_ES_LocateCDSBlockRecordByName(const char *CDSName)
{
    CFE_ES_CDS_Instance_t *CDS = &CFE_ES_Global.CDSVars;
    CFE_ES_CDS_RegRec_t *  CDSRegRecPtr;
    uint32                 NumReg;

    CDSRegRecPtr = CDS->Registry;
    NumReg       = CFE_PLATFORM_ES_CDS_MAX_NUM_ENTRIES;
    while (true)
    {
        if (NumReg == 0)
        {
            CDSRegRecPtr = NULL; /* not found */
            break;
        }

        if (CFE_ES_CDSBlockRecordIsUsed(CDSRegRecPtr))
        {
            /* Perform a case sensitive name comparison */
            if (strcmp(CDSName, CDSRegRecPtr->Name) == 0)
            {
                /* If the names match, then stop */
                break;
            }
        }

        ++CDSRegRecPtr;
        --NumReg;
    }

    return CDSRegRecPtr;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_RebuildCDS(void)
{
    CFE_ES_CDS_Instance_t *CDS = &CFE_ES_Global.CDSVars;
    int32                  Status;
    int32                  PspStatus;

    /* First, determine if the CDS registry stored in the CDS is smaller or equal */
    /* in size to the CDS registry we are currently configured for                */
    /* Copy the number of registry entries to the CDS */
    Status = CFE_ES_CDS_CacheFetch(&CDS->Cache, CDS_REG_SIZE_OFFSET, sizeof(CDS->Cache.Data.RegistrySize));
    if (Status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: PSP Error reading Registry size (%lx)\n", __func__,
                             (unsigned long)CDS->Cache.AccessStatus);
        return CFE_ES_CDS_INVALID;
    }

    if (CDS->Cache.Data.RegistrySize != CFE_PLATFORM_ES_CDS_MAX_NUM_ENTRIES)
    {
        /* Registry in CDS is incompatible size to recover */
        CFE_ES_WriteToSysLog("%s: Registry in CDS incorrect size (%lu)\n", __func__,
                             (unsigned long)CDS->Cache.Data.RegistrySize);
        return CFE_ES_CDS_INVALID;
    }

    PspStatus = CFE_PSP_ReadFromCDS(&CDS->Registry, CDS_REG_OFFSET, sizeof(CDS->Registry));

    if (PspStatus == CFE_PSP_SUCCESS)
    {
        /* Scan the memory pool and identify the created but currently unused memory blocks */
        Status = CFE_ES_RebuildCDSPool(CDS->DataSize, CDS_POOL_OFFSET);
    }
    else
    {
        /* Registry in CDS is unreadable */
        CFE_ES_WriteToSysLog("%s: Registry in CDS is unreadable, PSP error %lx\n", __func__, (unsigned long)PspStatus);
        Status = CFE_ES_CDS_INVALID;
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_DeleteCDS(const char *CDSName, bool CalledByTblServices)
{
    CFE_ES_CDS_Instance_t *CDS = &CFE_ES_Global.CDSVars;
    int32                  Status;
    CFE_ES_CDS_RegRec_t *  RegRecPtr;
    char                   OwnerName[OS_MAX_API_NAME];
    CFE_ES_AppId_t         AppId;
    uint32                 i;
    char                   LogMessage[CFE_ES_MAX_SYSLOG_MSG_SIZE];
    size_t                 OldBlockSize;

    LogMessage[0] = 0;

    /* Lock Registry for update.  This prevents two applications from */
    /* trying to change the CDS registry at the same time  */
    CFE_ES_LockCDS();

    /* Find CDS name in registry */
    RegRecPtr = CFE_ES_LocateCDSBlockRecordByName(CDSName);

    /* Check to see if CDS is already in the registry */
    if (RegRecPtr != NULL)
    {
        /* Critical tables are not allowed to be deleted via an ES Command. */
        /* They must be deleted by a Table Services Command */
        if (RegRecPtr->Table != CalledByTblServices)
        {
            Status = CFE_ES_CDS_WRONG_TYPE_ERR;
        }
        else
        {
            /* Check to see if the owning application is still active */
            /* First, extract the owning application name */
            i = 0;
            while ((i < (OS_MAX_API_NAME - 1) && (RegRecPtr->Name[i] != '.')))
            {
                OwnerName[i] = RegRecPtr->Name[i];
                i++;
            }

            /* Null terminate the application name */
            OwnerName[i] = '\0';

            /* Check to see if the Application Name is in the Registered Apps list */
            Status = CFE_ES_GetAppIDByName(&AppId, OwnerName);

            /* If we can't find the name, then things are good */
            if (Status != CFE_SUCCESS)
            {
                /* Free the registry entry and the CDS memory block associated with it */
                Status = CFE_ES_GenPoolPutBlock(&CDS->Pool, &OldBlockSize, RegRecPtr->BlockOffset);

                /* Report any errors incurred while freeing the CDS Memory Block */
                if (Status != CFE_SUCCESS)
                {
                    snprintf(LogMessage, sizeof(LogMessage),
                             "Failed to free CDS Mem Block (Handle=0x%08lX)(Stat=0x%08X)\n",
                             (unsigned long)RegRecPtr->BlockOffset, (unsigned int)Status);
                }
                else
                {
                    /* Remove entry from the CDS Registry */
                    CFE_ES_CDSBlockRecordSetFree(RegRecPtr);

                    Status = CFE_ES_UpdateCDSRegistry();

                    if (Status != CFE_SUCCESS)
                    {
                        snprintf(LogMessage, sizeof(LogMessage), "Failed to update CDS Registry (Stat=0x%08X)\n",
                                 (unsigned int)Status);
                    }
                }
            }
            else
            {
                Status = CFE_ES_CDS_OWNER_ACTIVE_ERR;
            }
        }
    }
    else /* Error - CDS not in registry */
    {
        Status = CFE_ES_ERR_NAME_NOT_FOUND;
    }

    /* Unlock Registry for future updates */
    CFE_ES_UnlockCDS();

    /* Output the message to syslog once the CDS registry resource is unlocked */
    if (LogMessage[0] != 0)
    {
        CFE_ES_WriteToSysLog("%s: %s", __func__, LogMessage);
    }

    return Status;
}
