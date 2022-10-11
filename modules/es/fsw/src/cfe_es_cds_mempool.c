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
**  cfe_es_cds_mempool.c
**
**  Purpose:
**  Set of services for management of the CDS discrete sized memory pools.
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**     cFE Flight Software Application Developers Guide
**
*/

/*
** Includes
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cfe_es_module_all.h"

/*****************************************************************************/
/*
** Type Definitions
*/

/*****************************************************************************/
/*
** File Global Data
*/

const size_t CFE_ES_CDSMemPoolDefSize[CFE_ES_CDS_NUM_BLOCK_SIZES] = {
    CFE_PLATFORM_ES_CDS_MAX_BLOCK_SIZE,    CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_16, CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_15,
    CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_14, CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_13, CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_12,
    CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_11, CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_10, CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_09,
    CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_08, CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_07, CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_06,
    CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_05, CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_04, CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_03,
    CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_02, CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_01};

/*****************************************************************************/
/*
** Functions
*/

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 * Obtains a block descriptor from CDS storage.
 * This is a bridge between the generic pool implementation and the CDS cache.
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_CDS_PoolRetrieve(CFE_ES_GenPoolRecord_t *GenPoolRecPtr, size_t Offset, CFE_ES_GenPoolBD_t **BdPtr)
{
    CFE_ES_CDS_Instance_t *CDS = (CFE_ES_CDS_Instance_t *)GenPoolRecPtr;

    *BdPtr = &CDS->Cache.Data.Desc;

    return CFE_ES_CDS_CacheFetch(&CDS->Cache, Offset, sizeof(CFE_ES_GenPoolBD_t));
}

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 * Writes a block descriptor to CDS storage.
 * This is a bridge between the generic pool implementation and the CDS cache.
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_CDS_PoolCommit(CFE_ES_GenPoolRecord_t *GenPoolRecPtr, size_t Offset, const CFE_ES_GenPoolBD_t *BdPtr)
{
    CFE_ES_CDS_Instance_t *CDS = (CFE_ES_CDS_Instance_t *)GenPoolRecPtr;

    CFE_ES_CDS_CachePreload(&CDS->Cache, BdPtr, Offset, sizeof(CFE_ES_GenPoolBD_t));

    return CFE_ES_CDS_CacheFlush(&CDS->Cache);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_CreateCDSPool(size_t CDSPoolSize, size_t StartOffset)
{
    CFE_ES_CDS_Instance_t *CDS = &CFE_ES_Global.CDSVars;
    int32                  Status;
    size_t                 SizeCheck;
    size_t                 ActualSize;

    SizeCheck  = CFE_ES_GenPoolCalcMinSize(CFE_ES_CDS_NUM_BLOCK_SIZES, CFE_ES_CDSMemPoolDefSize, 1);
    ActualSize = CDSPoolSize;

    if (ActualSize < SizeCheck)
    {
        /* Must be able make Pool verification, block descriptor and at least one of the smallest blocks  */
        CFE_ES_SysLogWrite_Unsync("%s: Pool size(%lu) too small for one CDS Block, need >=%lu\n", __func__,
                                  (unsigned long)ActualSize, (unsigned long)SizeCheck);
        return CFE_ES_CDS_INVALID_SIZE;
    }

    Status = CFE_ES_GenPoolInitialize(&CDS->Pool, StartOffset, /* starting offset */
                                      ActualSize,              /* total size */
                                      4,                       /* alignment */
                                      CFE_ES_CDS_NUM_BLOCK_SIZES, CFE_ES_CDSMemPoolDefSize, CFE_ES_CDS_PoolRetrieve,
                                      CFE_ES_CDS_PoolCommit);

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_RebuildCDSPool(size_t CDSPoolSize, size_t StartOffset)
{
    CFE_ES_CDS_Instance_t *CDS = &CFE_ES_Global.CDSVars;
    int32                  Status;

    /*
     * Start by creating the pool in a clean state, as it would be in a non-rebuild.
     */
    Status = CFE_ES_CreateCDSPool(CDSPoolSize, StartOffset);
    if (Status != CFE_SUCCESS)
    {
        return Status;
    }

    /* Now walk through the CDS memory and attempt to recover existing CDS blocks */
    Status = CFE_ES_GenPoolRebuild(&CDS->Pool);

    if (Status != CFE_SUCCESS)
    {
        CFE_ES_SysLogWrite_Unsync("%s: Err rebuilding CDS (Stat=0x%08x)\n", __func__, (unsigned int)Status);
        Status = CFE_ES_CDS_ACCESS_ERROR;
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_CDSBlockWrite(CFE_ES_CDSHandle_t Handle, const void *DataToWrite)
{
    CFE_ES_CDS_Instance_t *CDS = &CFE_ES_Global.CDSVars;
    char                   LogMessage[CFE_ES_MAX_SYSLOG_MSG_SIZE];
    int32                  Status;
    int32                  PspStatus;
    size_t                 BlockSize;
    size_t                 UserDataSize;
    size_t                 UserDataOffset;
    CFE_ES_CDS_RegRec_t *  CDSRegRecPtr;

    /* Ensure the log message is an empty string in case it is never written to */
    LogMessage[0] = 0;

    CDSRegRecPtr = CFE_ES_LocateCDSBlockRecordByID(Handle);

    /*
     * A CDS block ID must be accessed by only one thread at a time.
     * Checking the validity of the block requires access to the registry.
     */
    CFE_ES_LockCDS();

    if (CFE_ES_CDSBlockRecordIsMatch(CDSRegRecPtr, Handle))
    {
        /*
         * Getting the buffer size via this function retrieves it from the
         * internal descriptor, and validates the descriptor as part of the operation.
         * This should always agree with the size in the registry for this block.
         */
        Status = CFE_ES_GenPoolGetBlockSize(&CDS->Pool, &BlockSize, CDSRegRecPtr->BlockOffset);
        if (Status != CFE_SUCCESS)
        {
            snprintf(LogMessage, sizeof(LogMessage), "Invalid Handle or Block Descriptor.\n");
        }
        else if (BlockSize <= sizeof(CFE_ES_CDS_BlockHeader_t) || BlockSize != CDSRegRecPtr->BlockSize)
        {
            snprintf(LogMessage, sizeof(LogMessage), "Block size %lu invalid, expected %lu\n", (unsigned long)BlockSize,
                     (unsigned long)CDSRegRecPtr->BlockSize);
            Status = CFE_ES_CDS_INVALID_SIZE;
        }
        else
        {
            UserDataSize = CDSRegRecPtr->BlockSize;
            UserDataSize -= sizeof(CFE_ES_CDS_BlockHeader_t);
            UserDataOffset = CDSRegRecPtr->BlockOffset;
            UserDataOffset += sizeof(CFE_ES_CDS_BlockHeader_t);

            CDS->Cache.Data.BlockHeader.Crc =
                CFE_ES_CalculateCRC(DataToWrite, UserDataSize, 0, CFE_MISSION_ES_DEFAULT_CRC);
            CDS->Cache.Offset = CDSRegRecPtr->BlockOffset;
            CDS->Cache.Size   = sizeof(CFE_ES_CDS_BlockHeader_t);

            /* Write the new block descriptor for the data coming from the Application */
            Status = CFE_ES_CDS_CacheFlush(&CDS->Cache);
            if (Status != CFE_SUCCESS)
            {
                snprintf(LogMessage, sizeof(LogMessage),
                         "Err writing header data to CDS (Stat=0x%08x) @Offset=0x%08lx\n",
                         (unsigned int)CDS->Cache.AccessStatus, (unsigned long)CDSRegRecPtr->BlockOffset);
            }
            else
            {
                PspStatus = CFE_PSP_WriteToCDS(DataToWrite, UserDataOffset, UserDataSize);
                if (PspStatus != CFE_PSP_SUCCESS)
                {
                    snprintf(LogMessage, sizeof(LogMessage),
                             "Err writing user data to CDS (Stat=0x%08x) @Offset=0x%08lx\n", (unsigned int)PspStatus,
                             (unsigned long)UserDataOffset);

                    Status = CFE_ES_CDS_ACCESS_ERROR;
                }
            }
        }
    }
    else
    {
        Status = CFE_ES_ERR_RESOURCEID_NOT_VALID;
    }

    CFE_ES_UnlockCDS();

    /* Do the actual syslog if something went wrong */
    if (LogMessage[0] != 0)
    {
        CFE_ES_WriteToSysLog("%s: %s", __func__, LogMessage);
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_CDSBlockRead(void *DataRead, CFE_ES_CDSHandle_t Handle)
{
    CFE_ES_CDS_Instance_t *CDS = &CFE_ES_Global.CDSVars;
    int32                  Status;
    int32                  PspStatus;
    uint32                 CrcOfCDSData;
    size_t                 BlockSize;
    size_t                 UserDataSize;
    size_t                 UserDataOffset;
    CFE_ES_CDS_RegRec_t *  CDSRegRecPtr;

    CDSRegRecPtr = CFE_ES_LocateCDSBlockRecordByID(Handle);

    /*
     * A CDS block ID must be accessed by only one thread at a time.
     * Checking the validity of the block requires access to the registry.
     */
    CFE_ES_LockCDS();

    if (CFE_ES_CDSBlockRecordIsMatch(CDSRegRecPtr, Handle))
    {
        /*
         * Getting the buffer size via this function retrieves it from the
         * internal descriptor, and validates the descriptor as part of the operation.
         * This should always agree with the size in the registry for this block.
         */
        Status = CFE_ES_GenPoolGetBlockSize(&CDS->Pool, &BlockSize, CDSRegRecPtr->BlockOffset);
        if (Status == CFE_SUCCESS)
        {
            if (BlockSize <= sizeof(CFE_ES_CDS_BlockHeader_t) || BlockSize != CDSRegRecPtr->BlockSize)
            {
                Status = CFE_ES_CDS_INVALID_SIZE;
            }
            else
            {
                UserDataSize = CDSRegRecPtr->BlockSize;
                UserDataSize -= sizeof(CFE_ES_CDS_BlockHeader_t);
                UserDataOffset = CDSRegRecPtr->BlockOffset;
                UserDataOffset += sizeof(CFE_ES_CDS_BlockHeader_t);

                /* Read the header */
                Status =
                    CFE_ES_CDS_CacheFetch(&CDS->Cache, CDSRegRecPtr->BlockOffset, sizeof(CFE_ES_CDS_BlockHeader_t));

                if (Status == CFE_SUCCESS)
                {
                    /* Read the data block */
                    PspStatus = CFE_PSP_ReadFromCDS(DataRead, UserDataOffset, UserDataSize);
                    if (PspStatus == CFE_PSP_SUCCESS)
                    {
                        /* Compute the CRC for the data read from the CDS and determine if the data is still valid */
                        CrcOfCDSData = CFE_ES_CalculateCRC(DataRead, UserDataSize, 0, CFE_MISSION_ES_DEFAULT_CRC);

                        /* If the CRCs do not match, report an error */
                        if (CrcOfCDSData != CDS->Cache.Data.BlockHeader.Crc)
                        {
                            Status = CFE_ES_CDS_BLOCK_CRC_ERR;
                        }
                        else
                        {
                            Status = CFE_SUCCESS;
                        }
                    }
                    else
                    {
                        Status = CFE_ES_CDS_ACCESS_ERROR;
                    }
                }
            }
        }
    }
    else
    {
        Status = CFE_ES_ERR_RESOURCEID_NOT_VALID;
    }

    CFE_ES_UnlockCDS();

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
size_t CFE_ES_CDSReqdMinSize(uint32 MaxNumBlocksToSupport)
{
    size_t ReqSize;

    ReqSize = CFE_ES_GenPoolCalcMinSize(CFE_ES_CDS_NUM_BLOCK_SIZES, CFE_ES_CDSMemPoolDefSize, MaxNumBlocksToSupport);

    return ReqSize;
}
