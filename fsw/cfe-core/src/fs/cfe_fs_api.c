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
    CFE_ES_ResourceID_t AppID;

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
          Hdr->ApplicationID = CFE_ES_ResourceID_ToInteger(AppID);

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


/************************/
/*  End of File Comment */
/************************/
