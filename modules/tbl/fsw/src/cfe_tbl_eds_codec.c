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
 * \file
 *  This file contains the source code for the TO lab application
 */

#include <ctype.h>

#include "cfe_tbl_module_all.h"
#include "cfe_tbl_codec.h"
#include "cfe_config.h"

#include "edslib_datatypedb.h"
#include "edslib_intfdb.h"
#include "edslib_global.h"

#include "cfe_missionlib_api.h"
#include "cfe_mission_eds_parameters.h"
#include "cfe_mission_eds_interface_parameters.h"

#include "cfe_tbl_eds_datatypes.h"

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_DecodeHeadersFromFile(CFE_TBL_TxnState_t *Txn, osal_id_t FileDescriptor,
                                           CFE_TBL_File_Hdr_t *HeaderPtr)
{
    CFE_Status_t                       ReturnCode;
    EdsLib_Id_t                        EdsId;
    int32                              EdsStatus;
    int32                              OsStatus;
    const EdsLib_DatabaseObject_t *    EDS_DB;
    EdsPackedBuffer_CFE_TBL_File_Hdr_t Buffer;

    EDS_DB = CFE_Config_GetObjPointer(CFE_CONFIGID_MISSION_EDS_DB);
    EdsId  = EDSLIB_MAKE_ID(EDS_INDEX(CFE_TBL), EdsContainer_CFE_TBL_File_Hdr_DATADICTIONARY);

    OsStatus = OS_read(FileDescriptor, &Buffer, sizeof(Buffer));

    /* Verify successful read of cFE Table File Header */
    if (OsStatus != sizeof(Buffer))
    {
        CFE_TBL_TxnAddEvent(Txn, CFE_TBL_FILE_TBL_HDR_ERR_EID, OsStatus, sizeof(Buffer));
        ReturnCode = CFE_TBL_ERR_NO_TBL_HEADER;
    }
    else
    {
        EdsStatus = EdsLib_DataTypeDB_UnpackCompleteObject(EDS_DB, &EdsId, HeaderPtr, &Buffer, sizeof(*HeaderPtr),
                                                           8 * sizeof(Buffer));
        if (EdsStatus == EDSLIB_SUCCESS)
        {
            ReturnCode = CFE_SUCCESS;
        }
        else
        {
            CFE_TBL_TxnAddEvent(Txn, CFE_TBL_CODEC_ERROR_ERR_EID, EdsStatus, EdsId);
            ReturnCode = CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
        }
    }

    return ReturnCode;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_EncodeHeadersToFile(CFE_TBL_TxnState_t *Txn, osal_id_t FileDescriptor,
                                         const CFE_TBL_File_Hdr_t *HeaderPtr)
{
    CFE_Status_t                       ReturnCode;
    EdsLib_Id_t                        EdsId;
    int32                              EdsStatus;
    int32                              OsStatus;
    const EdsLib_DatabaseObject_t *    EDS_DB;
    EdsPackedBuffer_CFE_TBL_File_Hdr_t Buffer;

    EDS_DB = CFE_Config_GetObjPointer(CFE_CONFIGID_MISSION_EDS_DB);
    EdsId  = EDSLIB_MAKE_ID(EDS_INDEX(CFE_TBL), EdsContainer_CFE_TBL_File_Hdr_DATADICTIONARY);

    EdsStatus = EdsLib_DataTypeDB_PackPartialObject(EDS_DB, &EdsId, &Buffer, HeaderPtr, 8 * sizeof(Buffer),
                                                    sizeof(*HeaderPtr), 0);
    if (EdsStatus == EDSLIB_SUCCESS)
    {
        OsStatus = OS_write(FileDescriptor, &Buffer, sizeof(Buffer));

        /* Verify successful write of cFE Table File Header */
        if (OsStatus != sizeof(Buffer))
        {
            CFE_TBL_TxnAddEvent(Txn, CFE_TBL_WRITE_TBL_HDR_ERR_EID, OsStatus, sizeof(Buffer));
            ReturnCode = CFE_TBL_ERR_ACCESS;
        }
        else
        {
            ReturnCode = CFE_SUCCESS;
        }
    }
    else
    {
        CFE_TBL_TxnAddEvent(Txn, CFE_TBL_CODEC_ERROR_ERR_EID, EdsStatus, EdsId);
        ReturnCode = CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
    }

    return ReturnCode;
}

/*----------------------------------------------------------------
 *
 * Local Helper Function, not called outside of this unit
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_FindAppTableInterface(const char *TableFullName, EdsLib_Id_t *EdsIdBuf)
{
    CFE_Status_t ReturnCode;
    EdsLib_Id_t  AppComponentEdsId;
    int32        EdsStatus;
    uint8_t      AppIdx;
    char         AppNameBuffer[OS_MAX_API_NAME];
    char         TableNameBuffer[CFE_MISSION_TBL_MAX_NAME_LENGTH];
    const char * TableNamePtr;
    char *       TempPtr;
    size_t       AppNameLen;

    const EdsLib_DatabaseObject_t *EDS_DB;

    EDS_DB = CFE_Config_GetObjPointer(CFE_CONFIGID_MISSION_EDS_DB);

    /* The Name in ReqCfg is stored as <AppName>.<TableName>, need to isolate the app name */
    TableNamePtr = strchr(TableFullName, '.');
    if (TableNamePtr != NULL)
    {
        AppNameLen = TableNamePtr - TableFullName;
        ++TableNamePtr;
    }
    else
    {
        AppNameLen   = 0;
        TableNamePtr = TableFullName;
    }

    if (AppNameLen >= sizeof(AppNameBuffer))
    {
        AppNameLen = sizeof(AppNameBuffer) - 1;
    }

    memcpy(AppNameBuffer, TableFullName, AppNameLen);
    AppNameBuffer[AppNameLen] = 0;

    strncpy(TableNameBuffer, TableNamePtr, sizeof(TableNameBuffer) - 1);
    TableNameBuffer[sizeof(TableNameBuffer) - 1] = 0;

    /*
     * All apps with tables should include those tables in their respective EDS file
     * as an interface that inherits from the table interface defined by table services,
     * and this should include the type mapping for the table data.
     */
    EdsStatus = EdsLib_FindPackageIdxByName(EDS_DB, AppNameBuffer, &AppIdx);
    if (EdsStatus != EDSLIB_SUCCESS)
    {
        /* This means that this app/component does not have an EDS. */
        /* We can still permit the binary to be loaded as a passthrough (traditional method) */
        *EdsIdBuf = EDSLIB_ID_INVALID;
        EdsStatus = EDSLIB_SUCCESS;
    }
    else
    {
        EdsStatus = EdsLib_IntfDB_FindComponentByLocalName(EDS_DB, AppIdx, "Application", &AppComponentEdsId);

        /* Look for a direct match to the table name in the application interfaces */
        if (EdsStatus == EDSLIB_SUCCESS)
        {
            EdsStatus =
                EdsLib_IntfDB_FindComponentInterfaceByLocalName(EDS_DB, AppComponentEdsId, TableNameBuffer, EdsIdBuf);
        }

        /* Fallback - if no exact match, check for a numeric suffix.  Some apps (such as MD)
         * have multiple instances of the same table and they attach a numeric suffix to the name */
        if (EdsStatus != EDSLIB_SUCCESS)
        {
            TempPtr = &TableNameBuffer[strlen(TableNameBuffer)];
            while (TempPtr != &TableNameBuffer[0])
            {
                --TempPtr;
                if (!isdigit((int)(*TempPtr)))
                {
                    break;
                }

                *TempPtr = 0;
            }
            EdsStatus =
                EdsLib_IntfDB_FindComponentInterfaceByLocalName(EDS_DB, AppComponentEdsId, TableNameBuffer, EdsIdBuf);
        }
    }

    if (EdsStatus != EDSLIB_SUCCESS)
    {
        ReturnCode = CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
    }
    else
    {
        ReturnCode = CFE_SUCCESS;
    }

    return ReturnCode;
}

/*----------------------------------------------------------------
 *
 * Local Helper Function, not called outside of this unit
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_TranslateArgumentType(EdsLib_Id_t TblIntfEdsId, EdsLib_Id_t LoadCmdEdsId, EdsLib_Id_t *EdsIdBuf)
{
    CFE_Status_t                   ReturnCode;
    int32                          EdsStatus;
    const EdsLib_DatabaseObject_t *EDS_DB;

    EDS_DB = CFE_Config_GetObjPointer(CFE_CONFIGID_MISSION_EDS_DB);

    EdsStatus = EdsLib_IntfDB_FindAllArgumentTypes(EDS_DB, LoadCmdEdsId, TblIntfEdsId, EdsIdBuf, 1);

    /* Translate the status code */
    if (EdsStatus != EDSLIB_SUCCESS)
    {
        ReturnCode = CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
    }
    else
    {
        ReturnCode = CFE_SUCCESS;
    }

    return ReturnCode;
}

/*----------------------------------------------------------------
 *
 * Local Helper Function, not called outside of this unit
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_ValidateEdsObjectSize(CFE_TBL_TableConfig_t *ReqCfg)
{
    CFE_Status_t                        ReturnCode;
    EdsLib_DataTypeDB_DerivedTypeInfo_t DerivInfo;
    int32_t                             EdsStatus;

    const EdsLib_DatabaseObject_t *EDS_DB;

    EDS_DB = CFE_Config_GetObjPointer(CFE_CONFIGID_MISSION_EDS_DB);

    /* Note that this is checking the in-memory size here, not the encoded size */
    /* Use the derived info here - if the table type is a base type, this gets the max size */
    EdsStatus = EdsLib_DataTypeDB_GetDerivedInfo(EDS_DB, ReqCfg->EdsId, &DerivInfo);
    if (EdsStatus != EDSLIB_SUCCESS)
    {
        ReturnCode = CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
    }
    else if (DerivInfo.MaxSize.Bytes > ReqCfg->Size)
    {
        /* The size does not agree with what the user is trying to register */
        ReturnCode = CFE_TBL_ERR_INVALID_SIZE;
    }
    else
    {
        /* Everything looks OK */
        ReturnCode = CFE_SUCCESS;
    }

    return ReturnCode;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_ValidateCodecConfig(CFE_TBL_TableConfig_t *ReqCfg)
{
    CFE_Status_t ReturnCode;
    EdsLib_Id_t  LoadCmdEdsId;
    EdsLib_Id_t  TblIntfEdsId;

    /* Locate the relevant entries in the EDS DB */

    /* The table load command is a fixed definition, does not need a lookup */
    LoadCmdEdsId = EDSLIB_INTF_ID(EDS_INDEX(CFE_TBL), EdsCommand_CFE_TBL_Table_load_DECLARATION);

    /* The type mapping depends on the application use of this service */
    /* This assumes that the CFE application name matches the EDS package name */
    ReturnCode = CFE_TBL_FindAppTableInterface(ReqCfg->Name, &TblIntfEdsId);

    /* Determine the argument data type for "load" command */
    if (ReturnCode == CFE_SUCCESS)
    {
        if (EdsLib_Is_Valid(TblIntfEdsId))
        {
            ReturnCode = CFE_TBL_TranslateArgumentType(TblIntfEdsId, LoadCmdEdsId, &ReqCfg->EdsId);
        }
        else
        {
            ReqCfg->EdsId = EDSLIB_ID_INVALID;
        }
    }

    /* Confirm that the size matches the user-indicated size */
    if (ReturnCode == CFE_SUCCESS && EdsLib_Is_Valid(ReqCfg->EdsId))
    {
        ReturnCode = CFE_TBL_ValidateEdsObjectSize(ReqCfg);
    }

    return ReturnCode;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_GetEncodedTableSize(CFE_TBL_TxnState_t *Txn, uint32 *NumBytes)
{
    EdsLib_DataTypeDB_DerivedTypeInfo_t DerivInfo;
    const EdsLib_DatabaseObject_t *     EDS_DB;
    EdsLib_Id_t                         EdsId;
    int32                               EdsStatus;
    CFE_Status_t                        Status;
    CFE_TBL_RegistryRec_t *             RegRecPtr;
    const CFE_TBL_TableConfig_t *       Config;

    EDS_DB    = CFE_Config_GetObjPointer(CFE_CONFIGID_MISSION_EDS_DB);
    RegRecPtr = CFE_TBL_TxnRegRec(Txn);
    if (RegRecPtr == NULL)
    {
        Config = NULL;
    }
    else
    {
        Config = CFE_TBL_RegRecGetConfig(RegRecPtr);
    }

    if (Config == NULL)
    {
        Status = CFE_TBL_ERR_INVALID_HANDLE;
    }
    else
    {
        EdsId = Config->EdsId;
        if (EdsLib_Is_Valid(EdsId))
        {
            EdsStatus = EdsLib_DataTypeDB_GetDerivedInfo(EDS_DB, EdsId, &DerivInfo);

            if (EdsStatus != EDSLIB_SUCCESS)
            {
                CFE_TBL_TxnAddEvent(Txn, CFE_TBL_CODEC_ERROR_ERR_EID, EdsStatus, EdsId);
                Status = CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
            }
            else
            {
                Status    = CFE_SUCCESS;
                *NumBytes = (DerivInfo.MaxSize.Bits + 7) / 8;
            }
        }
        else
        {
            /* Use the registration size (passthrough) */
            Status    = CFE_SUCCESS;
            *NumBytes = Config->Size;
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
CFE_Status_t CFE_TBL_ValidateCodecLoadSize(CFE_TBL_TxnState_t *Txn, const CFE_TBL_File_Hdr_t *HeaderPtr)
{
    uint32       ProjectedSize;
    uint32       ActualSize;
    CFE_Status_t Status;

    Status = CFE_TBL_GetEncodedTableSize(Txn, &ActualSize);

    if (Status == CFE_SUCCESS)
    {
        ProjectedSize = HeaderPtr->Offset + HeaderPtr->NumBytes;
        if (ProjectedSize > ActualSize)
        {
            Status = CFE_TBL_ERR_FILE_TOO_LARGE;
            CFE_TBL_TxnAddEvent(Txn, CFE_TBL_LOAD_EXCEEDS_SIZE_ERR_EID, ProjectedSize, ActualSize);
        }
        else
        {
            Status = CFE_SUCCESS;
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
CFE_Status_t CFE_TBL_CodecGetFinalStatus(CFE_TBL_TxnState_t *Txn, const CFE_TBL_File_Hdr_t *HeaderPtr)
{
    uint32       ActualSize;
    CFE_Status_t Status;

    Status = CFE_TBL_GetEncodedTableSize(Txn, &ActualSize);

    if (Status == CFE_SUCCESS)
    {
        /* Any Table load that starts beyond the first byte is a "partial load" */
        /* But a file that starts with the first byte and ends before filling   */
        /* the whole table is just considered "short".                          */
        if (HeaderPtr->Offset > 0)
        {
            Status = CFE_TBL_WARN_PARTIAL_LOAD;
        }
        else if (HeaderPtr->NumBytes < ActualSize)
        {
            Status = CFE_TBL_WARN_SHORT_FILE;
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
CFE_TBL_LoadBuff_t *CFE_TBL_AcquireCodecBuffer(CFE_TBL_RegistryRec_t *RegRecPtr)
{
    return CFE_TBL_AcquireGlobalLoadBuff(CFE_TBL_RegRecGetID(RegRecPtr));
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_ReleaseCodecBuffer(CFE_TBL_LoadBuff_t *BufferPtr)
{
    CFE_TBL_LoadBuffSetFree(BufferPtr);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_EncodeOutputData(CFE_TBL_TxnState_t *Txn, const CFE_TBL_LoadBuff_t *SourceBuffer,
                                      CFE_TBL_LoadBuff_t *DestBuffer)
{
    CFE_Status_t                 ReturnCode;
    EdsLib_Id_t                  EdsId;
    int32                        EdsStatus;
    EdsLib_DataTypeDB_TypeInfo_t TypeInfo;
    CFE_TBL_RegistryRec_t *      RegRecPtr;

    const EdsLib_DatabaseObject_t *EDS_DB;

    EDS_DB    = CFE_Config_GetObjPointer(CFE_CONFIGID_MISSION_EDS_DB);
    RegRecPtr = CFE_TBL_TxnRegRec(Txn);

    EdsId = CFE_TBL_RegRecGetConfig(RegRecPtr)->EdsId;

    EdsStatus = EdsLib_DataTypeDB_PackCompleteObject(EDS_DB, &EdsId, DestBuffer, SourceBuffer->BufferPtr,
                                                     8 * CFE_PLATFORM_TBL_MAX_SNGL_TABLE_SIZE,
                                                     CFE_TBL_RegRecGetSize(RegRecPtr));

    if (EdsStatus == EDSLIB_SUCCESS)
    {
        EdsStatus = EdsLib_DataTypeDB_GetTypeInfo(EDS_DB, EdsId, &TypeInfo);
        if (EdsStatus == EDSLIB_SUCCESS)
        {
            CFE_TBL_LoadBuffSetContentSize(DestBuffer, (TypeInfo.Size.Bits + 7) / 8);
            ReturnCode = CFE_SUCCESS;
        }
        else
        {
            OS_printf("%s(): EdsLib_DataTypeDB_GetTypeInfo(): %d\n", __func__, (int)EdsStatus);
            ReturnCode = CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
        }
    }
    else
    {
        OS_printf("%s(): EdsLib_DataTypeDB_PackCompleteObject(): %d\n", __func__, (int)EdsStatus);
        ReturnCode = CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
    }

    return ReturnCode;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_DecodeInputData(CFE_TBL_TxnState_t *Txn, const CFE_TBL_LoadBuff_t *SourceBuffer,
                                     CFE_TBL_LoadBuff_t *DestBuffer)
{
    CFE_Status_t                 ReturnCode;
    EdsLib_Id_t                  EdsId;
    int32                        EdsStatus;
    EdsLib_DataTypeDB_TypeInfo_t TypeInfo;
    CFE_TBL_RegistryRec_t *      RegRecPtr;

    const EdsLib_DatabaseObject_t *EDS_DB;

    EDS_DB    = CFE_Config_GetObjPointer(CFE_CONFIGID_MISSION_EDS_DB);
    RegRecPtr = CFE_TBL_TxnRegRec(Txn);

    EdsId = CFE_TBL_RegRecGetConfig(RegRecPtr)->EdsId;

    if (!EdsLib_Is_Valid(EdsId))
    {
        /* Passthrough mode, just copy the data */
        memcpy(DestBuffer->BufferPtr, SourceBuffer->BufferPtr, CFE_TBL_RegRecGetSize(RegRecPtr));
        CFE_TBL_LoadBuffSetContentSize(DestBuffer, CFE_TBL_RegRecGetSize(RegRecPtr));
        ReturnCode = CFE_SUCCESS;
    }
    else
    {
        EdsStatus = EdsLib_DataTypeDB_UnpackCompleteObject(EDS_DB, &EdsId, DestBuffer->BufferPtr,
                                                           SourceBuffer->BufferPtr, CFE_TBL_RegRecGetSize(RegRecPtr),
                                                           8 * CFE_PLATFORM_TBL_MAX_SNGL_TABLE_SIZE);

        if (EdsStatus == EDSLIB_SUCCESS)
        {
            EdsStatus = EdsLib_DataTypeDB_GetTypeInfo(EDS_DB, EdsId, &TypeInfo);
            if (EdsStatus == EDSLIB_SUCCESS)
            {
                CFE_TBL_LoadBuffSetContentSize(DestBuffer, TypeInfo.Size.Bytes);
                ReturnCode = CFE_SUCCESS;
            }
            else
            {
                OS_printf("%s(): EdsLib_DataTypeDB_GetTypeInfo(): %d\n", __func__, (int)EdsStatus);
                ReturnCode = CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
            }
        }
        else
        {
            OS_printf("%s(): EdsLib_DataTypeDB_UnpackCompleteObject(): %d\n", __func__, (int)EdsStatus);
            ReturnCode = CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
        }
    }

    return ReturnCode;
}
