/*
**  GSC-18128-1, "Core Flight Executive Version 6.7"
**
**  Copyright (c) 2006-2019 United States Government as represented by
**  the Administrator of the National Aeronautics and Space Administration.
**  All Rights Reserved.
**
**  Licensed under the Apache License, Version 2.0 (the "License");
**  you may not use this file except in compliance with the License.
**  You may obtain a copy of the License at
**
**    http://www.apache.org/licenses/LICENSE-2.0
**
**  Unless required by applicable law or agreed to in writing, software
**  distributed under the License is distributed on an "AS IS" BASIS,
**  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**  See the License for the specific language governing permissions and
**  limitations under the License.
*/

/*
** File: ut_fs_stubs.c
**
** Purpose:
** Unit test stubs for File Service routines
** 
** Notes:
** Minimal work is done, only what is required for unit testing
**
*/

/*
** Includes
*/
#include <string.h>
#include "cfe.h"
#include "utstubs.h"


/*
** Functions
*/
/*****************************************************************************/
/**
** \brief CFE_FS_InitHeader stub function
**
** \par Description
**        This function is used to mimic the response of the cFE FS function
**        CFE_FS_InitHeader.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        None
**
******************************************************************************/
void CFE_FS_InitHeader(CFE_FS_Header_t *Hdr, const char *Description, uint32 SubType)
{
    UT_Stub_RegisterContext(UT_KEY(CFE_FS_InitHeader), Hdr);
    UT_Stub_RegisterContext(UT_KEY(CFE_FS_InitHeader), Description);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_FS_InitHeader), SubType);

    memset(Hdr,0,sizeof(CFE_FS_Header_t));
    UT_DEFAULT_IMPL(CFE_FS_InitHeader);
}

/*****************************************************************************/
/**
** \brief CFE_FS_WriteHeader stub function
**
** \par Description
**        This function is used to mimic the response of the cFE FS function
**        CFE_FS_WriteHeader.  The user can adjust the response by setting
**        the values in the FSWriteHdrRtn structure prior to this function
**        being called.  If the value FSWriteHdrRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value FSWriteHdrRtn.value.
**        The size of the CFE_FS_Header_t structure, in bytes, is returned
**        otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or the size of the
**        CFE_FS_Header_t structure in bytes.
**
******************************************************************************/
int32 CFE_FS_WriteHeader(osal_id_t FileDes, CFE_FS_Header_t *Hdr)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_FS_WriteHeader), FileDes);
    UT_Stub_RegisterContext(UT_KEY(CFE_FS_WriteHeader), Hdr);

    int32 status;

    status = UT_DEFAULT_IMPL_RC(CFE_FS_WriteHeader, sizeof(CFE_FS_Header_t));

    if (status > 0)
    {
        UT_Stub_CopyFromLocal(UT_KEY(CFE_FS_WriteHeader), (const uint8*)Hdr, status);
    }

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_FS_ReadHeader stub function
**
** \par Description
**        This function is used to mimic the response of the cFE FS function
**        CFE_FS_ReadHeader.  The user can adjust the response by setting
**        the values in the FSReadHdrRtn structure prior to this function
**        being called.  If the value FSReadHdrRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value FSReadHdrRtn.value.
**        The size of the CFE_FS_Header_t structure, in bytes, is returned
**        otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or the size of the
**        CFE_FS_Header_t structure in bytes.
**
******************************************************************************/
int32 CFE_FS_ReadHeader(CFE_FS_Header_t *Hdr, osal_id_t FileDes)
{
    UT_Stub_RegisterContext(UT_KEY(CFE_FS_ReadHeader), Hdr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_FS_ReadHeader), FileDes);

    int32 status;

    status = UT_DEFAULT_IMPL_RC(CFE_FS_ReadHeader, sizeof(CFE_FS_Header_t));

    if (status > 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(CFE_FS_ReadHeader), (uint8*)Hdr, status);
    }

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_FS_SetTimestamp stub function
**
** \par Description
**        This function is used to mimic the response of the cFE FS function
**        CFE_FS_SetTimestamp.  The user can adjust the response by setting
**        the values in the FSSetTimestampRtn structure prior to this function
**        being called.  If the value FSSetTimestampRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value
**        FSSetTimestampRtn.value.  OS_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or OS_SUCCESS.
**
******************************************************************************/
int32 CFE_FS_SetTimestamp(osal_id_t FileDes, CFE_TIME_SysTime_t NewTimestamp)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_FS_SetTimestamp), FileDes);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_FS_SetTimestamp), NewTimestamp);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_FS_SetTimestamp);

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_FS_EarlyInit stub function
**
** \par Description
**        This function is used to mimic the response of the cFE FS function
**        CFE_FS_EarlyInit.  It always returns CFE_SUCCESS when called.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_FS_EarlyInit(void)
{
    int32 status;

    status = UT_DEFAULT_IMPL(CFE_FS_EarlyInit);

    return status;
}


/*****************************************************************************/
/**
** \brief CFE_FS_ExtractFilenameFromPath stub function
**
** \par Description
**        This function is used to mimic the response of the cFE FS function
**        CFE_FS_ExtractFilenameFromPath.  The user can adjust the response by
**        setting the values in the FSExtractRtn structure prior to this
**        function being called.  If the value FSExtractRtn.count is greater
**        than zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value FSExtractRtn.value.
**        Otherwise returns CFE_SUCCESS or an error flag dependent on the
**        function's input values.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag, CFE_FS_BAD_ARGUMENT,
**        CFE_FS_INVALID_PATH, CFE_FS_FNAME_TOO_LONG, or CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_FS_ExtractFilenameFromPath(const char *OriginalPath, char *FileNameOnly)
{
    UT_Stub_RegisterContext(UT_KEY(CFE_FS_ExtractFilenameFromPath), OriginalPath);
    UT_Stub_RegisterContext(UT_KEY(CFE_FS_ExtractFilenameFromPath), FileNameOnly);

    int   i,j;
    int   StringLength;
    int   DirMarkIdx;
    int32 status;
    uint32 UserBuffLen;

    status = UT_DEFAULT_IMPL(CFE_FS_ExtractFilenameFromPath);

    if (status >= 0)
    {
        if (OriginalPath == NULL || FileNameOnly == NULL)
        {
            status = CFE_FS_BAD_ARGUMENT;
        }
        else
        {
            UserBuffLen = UT_Stub_CopyToLocal(UT_KEY(CFE_FS_ExtractFilenameFromPath), (uint8*)FileNameOnly, OS_MAX_FILE_NAME);

            if (UserBuffLen >= OS_MAX_FILE_NAME)
            {
                FileNameOnly[OS_MAX_FILE_NAME - 1] = 0;
            }
            else if (UserBuffLen > 0)
            {
                /* Just ensure that the output is null terminated */
                FileNameOnly[UserBuffLen] = 0;
            }
            else
            {
                /* Get the string length of the original file path */
                StringLength = strlen(OriginalPath);

                /* Extract the filename from the Path:
                     Find the last '/' Character */
                DirMarkIdx = -1;

                for (i = 0; i < StringLength; i++)
                {
                    if (OriginalPath[i] == '/')
                    {
                        DirMarkIdx = i;
                    }
                }

                /* Verify the filename isn't too long */
                if ((StringLength - (DirMarkIdx + 1)) < OS_MAX_PATH_LEN)
                {
                    /* Extract the filename portion */
                    if (DirMarkIdx > 0)
                    {
                        /* Extract the filename portion */
                        j = 0;

                        for (i = DirMarkIdx + 1; i < StringLength; i++)
                        {
                            FileNameOnly[j] = OriginalPath[i];
                            j++;
                        }

                        FileNameOnly[j] = '\0';
                    }
                    else
                    {
                        status = CFE_FS_INVALID_PATH;
                    }
                }
                else
                {
                   status = CFE_FS_FNAME_TOO_LONG;
                }
            }
        }
    }
   
    return status;
}

