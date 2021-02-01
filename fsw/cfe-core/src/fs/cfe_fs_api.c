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
** File: cfe_fs_api.c
**
** Purpose:  cFE File Services (FS) library API source file
**
** Author:   S.Walling/Microtel
**
** Notes:
**
*/


/*
** Required header files...
*/
#include "private/cfe_private.h"
#include "cfe_fs_priv.h"
#include "cfe_fs.h"
#include "cfe_time.h"
#include "osapi.h"
#include "cfe_psp.h"
#include "cfe_es.h"
#include <string.h>


/*
** CFE_FS_ReadHeader() - See API and header file for details
*/
int32 CFE_FS_ReadHeader(CFE_FS_Header_t *Hdr, osal_id_t FileDes)
{
    int32   Result;
    int32   EndianCheck = 0x01020304;
    
    /*
    ** Ensure that we are at the start of the file...
    */
    Result = OS_lseek(FileDes, 0, OS_SEEK_SET);

    if (Result == OS_SUCCESS)
    {
        /*
        ** Read header structure into callers buffer...
        */
        Result = OS_read(FileDes, Hdr, sizeof(CFE_FS_Header_t));

	    /* Determine if this processor is a little endian processor */
    	if ((*(char *)(&EndianCheck)) == 0x04)
    	{
    	    /* If this is a little endian processor, then convert the header data structure from */
    	    /* its standard big-endian format into a little endian format to ease user access    */
    	    CFE_FS_ByteSwapCFEHeader(Hdr);
    	}
    }
    
    return(Result);

} /* End of CFE_FS_ReadHeader() */

/*
** CFE_FS_InitHeader() - See API and header file for details
*/
void CFE_FS_InitHeader(CFE_FS_Header_t *Hdr, const char *Description, uint32 SubType)
{
   memset(Hdr, 0, sizeof(CFE_FS_Header_t));
   strncpy((char *)Hdr->Description, Description, sizeof(Hdr->Description) - 1);
   Hdr->SubType = SubType;
}

/*
** CFE_FS_WriteHeader() - See API and header file for details
*/
int32 CFE_FS_WriteHeader(osal_id_t FileDes, CFE_FS_Header_t *Hdr)
{
    CFE_TIME_SysTime_t Time;
    int32   Result;
    int32   EndianCheck = 0x01020304;
    CFE_ES_AppId_t AppID;

    /*
    ** Ensure that we are at the start of the file...
    */
    Result = OS_lseek(FileDes, 0, OS_SEEK_SET);

    if (Result == OS_SUCCESS)
    {
        /*
        ** Fill in the ID fields...
        */
          Hdr->SpacecraftID  = CFE_PSP_GetSpacecraftId();
          Hdr->ProcessorID   = CFE_PSP_GetProcessorId();
          CFE_ES_GetAppID(&AppID);
          Hdr->ApplicationID = CFE_RESOURCEID_TO_ULONG(AppID);

          /* Fill in length field */

          Hdr->Length = sizeof(CFE_FS_Header_t);

          /* put the header, 'cfe1' in hex, in to the content type */
          Hdr->ContentType = 0x63464531;

          
        /*
        ** Fill in the timestamp fields...
        */
          Time = CFE_TIME_GetTime();
          Hdr->TimeSeconds = Time.Seconds;
          Hdr->TimeSubSeconds = Time.Subseconds;

        /*
        ** Determine if this is a little endian processor
        */
        if ((*(char *)(&EndianCheck)) == 0x04)
        {
            /* If this is a little endian processor, then convert the header data structure from */
            /* the native little endian format to the required CFE standard big endian format    */
            CFE_FS_ByteSwapCFEHeader(Hdr);
        }

        /*
        ** Write header structure from callers buffer...
        */
        Result = OS_write(FileDes, Hdr, sizeof(CFE_FS_Header_t));

        /*
        ** Determine if this is a little endian processor
        */
        if ((*(char *)(&EndianCheck)) == 0x04)
        {
            /* If this is a little endian processor, then convert the header data structure back */
            /* from the required CFE standard big endian format to the little endian format      */
            CFE_FS_ByteSwapCFEHeader(Hdr);
        }

    }

    return(Result);

} /* End of CFE_FS_WriteHeader() */

/*
** CFE_FS_SetTimestamp - See API and header file for details
*/
int32 CFE_FS_SetTimestamp(osal_id_t FileDes, CFE_TIME_SysTime_t NewTimestamp)
{
    int32              Result;
    CFE_FS_Header_t    TempHdr;
    int32              EndianCheck = 0x01020304;
    CFE_TIME_SysTime_t OutTimestamp = NewTimestamp;
    int32              FileOffset = 0;
    
    FileOffset = ((char *)&TempHdr.TimeSeconds - (char *)&TempHdr.ContentType);
    Result = OS_lseek(FileDes, FileOffset, OS_SEEK_SET);
    
    if (Result == FileOffset)
    {
        /*
        **  Determine if this is a little endian processor
        */
        if ((*(char *)(&EndianCheck)) == 0x04)
        {
            /* If this processor is a little endian processor, then convert the timestamp to a big */
            /* endian format so that it is compatible with the standard cFE File Header format     */
            CFE_FS_ByteSwapUint32(&OutTimestamp.Seconds);
            CFE_FS_ByteSwapUint32(&OutTimestamp.Subseconds);
        }
        
        Result = OS_write(FileDes, &OutTimestamp.Seconds, sizeof(OutTimestamp.Seconds));
        
        /* On a good write, the value returned will equal the number of bytes written */
        if (Result == sizeof(OutTimestamp.Seconds))
        {
            Result = OS_write(FileDes, &OutTimestamp.Subseconds, sizeof(OutTimestamp.Subseconds));
            
            if (Result == sizeof(OutTimestamp.Subseconds))
            {
                Result = OS_SUCCESS;
            }
            else
            {
                CFE_ES_WriteToSysLog("CFE_FS:SetTime-Failed to write Seconds (Status=0x%08X)\n", (unsigned int)Result);
            }
        }
        else
        {
            CFE_ES_WriteToSysLog("CFE_FS:SetTime-Failed to write Seconds (Status=0x%08X)\n", (unsigned int)Result);
        }
    }
    else
    {
        CFE_ES_WriteToSysLog("CFE_FS:SetTime-Failed to lseek time fields (Status=0x%08X)\n", (unsigned int)Result);
    }
    
    return(Result);
} /* End of CFE_FS_SetTimestamp() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_FS_ByteSwapCFEHeader() -- byte swap cFE file header structure       */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_FS_ByteSwapCFEHeader(CFE_FS_Header_t *Hdr)
{
    CFE_FS_ByteSwapUint32(&Hdr->ContentType);
    CFE_FS_ByteSwapUint32(&Hdr->SubType);
    CFE_FS_ByteSwapUint32(&Hdr->Length);
    CFE_FS_ByteSwapUint32(&Hdr->SpacecraftID);
    CFE_FS_ByteSwapUint32(&Hdr->ProcessorID);
    CFE_FS_ByteSwapUint32(&Hdr->ApplicationID);
    CFE_FS_ByteSwapUint32(&Hdr->TimeSeconds);
    CFE_FS_ByteSwapUint32(&Hdr->TimeSubSeconds);

} /* End of CFE_FS_ByteSwapCFEHeader() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_FS_ByteSwapUint32() -- byte swap an uint32                          */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_FS_ByteSwapUint32(uint32 *Uint32ToSwapPtr)
{
    int32 Temp = *Uint32ToSwapPtr;
    char *InPtr = (char *)&Temp;
    char *OutPtr = (char *)Uint32ToSwapPtr;
    
    OutPtr[0] = InPtr[3];
    OutPtr[1] = InPtr[2];
    OutPtr[2] = InPtr[1];
    OutPtr[3] = InPtr[0];    
} /* End of CFE_FS_ByteSwapUint32() */


/*
** CFE_FS_ExtractFilenameFromPath - See API and header file for details
*/
int32 CFE_FS_ExtractFilenameFromPath(const char *OriginalPath, char *FileNameOnly)
{
   uint32 i,j;
   int    StringLength;
   int    DirMarkIdx;
   int32   ReturnCode;
   
   if ( OriginalPath == NULL || FileNameOnly == NULL )
   {
      ReturnCode = CFE_FS_BAD_ARGUMENT;
   }
   else
   {
      
       /*
       ** Get the string length of the original file path
       */
       StringLength = strlen(OriginalPath);
   
       /*
       ** Extract the filename from the Path
       */
    
       /* 
       ** Find the last '/' Character 
       */
       DirMarkIdx = -1;
       for ( i = 0; i < StringLength; i++ )
       {
          if ( OriginalPath[i] == '/' )
          {
             DirMarkIdx = i;
          }
       }
    
       /*
       ** Verify the filename isn't too long
       */
       if ((StringLength - (DirMarkIdx + 1)) < OS_MAX_PATH_LEN)
       {       
          /* 
          ** Extract the filename portion 
          */
          if ( DirMarkIdx > 0 )
          {    
             /* 
             ** Extract the filename portion 
             */
             j = 0;
             for ( i = DirMarkIdx + 1; i < StringLength; i++ )
             {
                FileNameOnly[j] = OriginalPath[i];
                j++;
             }
             FileNameOnly[j] = '\0';
    
             ReturnCode = CFE_SUCCESS;       
          }
          else
          { 
             ReturnCode = CFE_FS_INVALID_PATH;
          }
       }
       else 
       {
           ReturnCode = CFE_FS_FNAME_TOO_LONG;
       }
    }
   
    return(ReturnCode);
}

/*
** CFE_FS_RunBackgroundFileDump - See API and header file for details
*/
bool CFE_FS_RunBackgroundFileDump(uint32 ElapsedTime, void *Arg)
{
    CFE_FS_CurrentFileState_t *State;
    CFE_FS_BackgroundFileDumpEntry_t *Curr;
    CFE_FS_FileWriteMetaData_t *Meta;
    int32               Status;
    CFE_FS_Header_t     FileHdr;
    void *RecordPtr;
    size_t RecordSize;
    bool IsEOF;

    State = &CFE_FS_Global.FileDump.Current;
    Curr = NULL;
    IsEOF = false;
    RecordPtr = NULL;
    RecordSize = 0;

    State->Credit += (ElapsedTime * CFE_FS_BACKGROUND_CREDIT_PER_SECOND) / 1000;
    if (State->Credit > CFE_FS_BACKGROUND_MAX_CREDIT)
    {
        State->Credit = CFE_FS_BACKGROUND_MAX_CREDIT;
    }

    /*
     * Lock shared data.
     * Not strictly necessary as the "CompleteCount" is only updated
     * by this task but this helps in case the access isn't atomic.
     */
    CFE_FS_LockSharedData(__func__);

    if (CFE_FS_Global.FileDump.CompleteCount != CFE_FS_Global.FileDump.RequestCount)
    {
        Curr = &CFE_FS_Global.FileDump.Entries[CFE_FS_Global.FileDump.CompleteCount & (CFE_FS_MAX_BACKGROUND_FILE_WRITES - 1)];
    }

    CFE_FS_UnlockSharedData(__func__);

    if (Curr == NULL)
    {
        return false;
    }

    Meta = Curr->Meta;

    if (!OS_ObjectIdDefined(State->Fd) && Meta->IsPending)
    {
        /* First time processing this entry - open the file */
        Status = OS_OpenCreate(&State->Fd, Meta->FileName, OS_FILE_FLAG_CREATE | OS_FILE_FLAG_TRUNCATE, OS_WRITE_ONLY);
        if(Status < 0)
        {
            State->Fd = OS_OBJECT_ID_UNDEFINED;
            Meta->OnEvent(Meta, CFE_FS_FileWriteEvent_CREATE_ERROR, Status, 0, 0, 0);
        }
        else
        {
            CFE_FS_InitHeader(&FileHdr, Meta->Description, Meta->FileSubType);

            /* write the cFE header to the file */
            Status = CFE_FS_WriteHeader(State->Fd, &FileHdr);
            if (Status != sizeof(CFE_FS_Header_t))
            {
                OS_close(State->Fd);
                State->Fd = OS_OBJECT_ID_UNDEFINED;
                Meta->OnEvent(Meta, CFE_FS_FileWriteEvent_HEADER_WRITE_ERROR, Status, State->RecordNum, sizeof(CFE_FS_Header_t), State->FileSize);
            }
            else
            {
                State->FileSize = sizeof(CFE_FS_Header_t);
                State->Credit -= sizeof(CFE_FS_Header_t);
                State->RecordNum = 0;
            }
        }
    }

    while (OS_ObjectIdDefined(State->Fd) && State->Credit > 0 && !IsEOF)
    {
        /*
         * Getter should return false on EOF (last record), true if more data is still waiting
         */
        IsEOF = Meta->GetData(Meta, State->RecordNum, &RecordPtr, &RecordSize);

        /* 
         * if the getter outputs a record size of 0, this means there is no data for
         * this entry, but the cycle keeps going (in case of "holes" or unused table entries
         * in the database).
         */
        if (RecordSize > 0)
        {
            State->Credit -= RecordSize;

            /*
            * Now write to file
            */
            Status = OS_write(State->Fd,RecordPtr,RecordSize);

            if (Status != RecordSize)
            {
                /* end the file early (cannot set "IsEOF" as this would cause the complete event to be generated too) */
                OS_close(State->Fd);
                State->Fd = OS_OBJECT_ID_UNDEFINED;

                /* generate write error event */
                Meta->OnEvent(Meta, CFE_FS_FileWriteEvent_RECORD_WRITE_ERROR, Status, State->RecordNum, RecordSize, State->FileSize);
                break;
            }
            else
            {
                State->FileSize += RecordSize;
            }
        }

        ++State->RecordNum;

    } /* end if */

    /* On normal EOF close the file and generate the complete event */
    if (IsEOF)
    {
        OS_close(State->Fd);
        State->Fd = OS_OBJECT_ID_UNDEFINED;

        /* generate complete event */
        Meta->OnEvent(Meta, CFE_FS_FileWriteEvent_COMPLETE, CFE_SUCCESS, State->RecordNum, 0, State->FileSize);
    }

    /* 
     * if the file is not open, consider this file complete, and advance the head position.
     * (done this way so it also catches the case where the file failed to create, not just EOF)
     */
    if (!OS_ObjectIdDefined(State->Fd))
    {
        CFE_FS_LockSharedData(__func__);

        /* Wipe the entry structure, as it will be reused */
        memset(Curr, 0, sizeof(*Curr));
        ++CFE_FS_Global.FileDump.CompleteCount;

        /* Set the "IsPending" flag to false - this indicates that the originator may re-post now */
        Meta->IsPending = false;

        CFE_FS_UnlockSharedData(__func__);

    } /* end if */

    return !IsEOF;
}

/*
** CFE_FS_BackgroundFileDumpRequest - See API and header file for details
*/
int32 CFE_FS_BackgroundFileDumpRequest(CFE_FS_FileWriteMetaData_t *Meta)
{
    CFE_FS_BackgroundFileDumpEntry_t *Curr;
    int32 Status;
    uint32 PendingRequestCount;

    /* Pre-validate inputs */
    if (Meta == NULL)
    {
        return CFE_FS_BAD_ARGUMENT;
    }

    /* getter and event functions must be set */
    if (Meta->GetData == NULL || Meta->OnEvent == NULL)
    {
        return CFE_FS_BAD_ARGUMENT;
    }

    /* filename cannot be empty */
    if (Meta->FileName[0] == 0)
    {
        return CFE_FS_INVALID_PATH;
    }

    /* request must not already be pending */
    if (Meta->IsPending)
    {
        return CFE_STATUS_REQUEST_ALREADY_PENDING;
    }


    CFE_FS_LockSharedData(__func__);

    PendingRequestCount = CFE_FS_Global.FileDump.RequestCount + 1;

    /* Check if queue is full before writing to tail position */
    if (PendingRequestCount == (CFE_FS_Global.FileDump.CompleteCount + CFE_FS_MAX_BACKGROUND_FILE_WRITES))
    {
        Status = CFE_STATUS_REQUEST_ALREADY_PENDING;
    }
    else
    {
        Curr = &CFE_FS_Global.FileDump.Entries[CFE_FS_Global.FileDump.RequestCount & (CFE_FS_MAX_BACKGROUND_FILE_WRITES - 1)];

        /* 
         * store the meta object - note this retains the pointer that was submitted 
         * (caller must not reuse/change this object until request is completed)
         */
        Curr->Meta = Meta;

        /* 
         * The "IsPending" Flag will be set true whenever while this is waiting in the request queue.
         * It will be set false when the file is done.
         * 
         * The requester can check this flag to determine if/when the request is complete
         */
        Meta->IsPending = true;

        /* update tail position */
        CFE_FS_Global.FileDump.RequestCount = PendingRequestCount;

        Status = CFE_SUCCESS;
    }

    CFE_FS_UnlockSharedData(__func__);

    if (Status == CFE_SUCCESS)
    {
        /* 
         * If successfully added to write queue, then wake the ES background task to get started.
         * 
         * This may reduce the overall latency between request and completion (depending on other
         * background task work).  If this is the only pending job, this should get it started faster.
         */
        CFE_ES_BackgroundWakeup();
    }

    return Status;
}

/*
** CFE_FS_ExtractFilenameFromPath - See API and header file for details
*/
bool CFE_FS_BackgroundFileDumpIsPending(const CFE_FS_FileWriteMetaData_t *Meta)
{
    if (Meta == NULL)
    {
        return false;
    }

    return Meta->IsPending;
}

/************************/
/*  End of File Comment */
/************************/
