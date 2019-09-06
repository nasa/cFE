/*
**  GSC-18128-1, "Core Flight Executive Version 6.6"
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


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_FS_ReadHeader() -- read cFE file header structure                   */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 CFE_FS_ReadHeader(CFE_FS_Header_t *Hdr, int32 FileDes)
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

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_FS_InitHeader() -- intialize cFE file header structure              */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CFE_FS_InitHeader(CFE_FS_Header_t *Hdr, const char *Description, uint32 SubType)
{
   memset(Hdr, 0, sizeof(CFE_FS_Header_t));
   strncpy((char *)Hdr->Description, Description, sizeof(Hdr->Description) - 1);
   Hdr->SubType = SubType;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_FS_WriteHeader() -- write cFE file header structure                 */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 CFE_FS_WriteHeader(int32 FileDes, CFE_FS_Header_t *Hdr)
{
    CFE_TIME_SysTime_t Time;
    int32   Result;
    int32   EndianCheck = 0x01020304;

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
          CFE_ES_GetAppID((uint32 *)&(Hdr->ApplicationID));

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


int32 CFE_FS_SetTimestamp(int32 FileDes, CFE_TIME_SysTime_t NewTimestamp)
{
    int32              Result = OS_FS_SUCCESS;
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
                Result = OS_FS_SUCCESS;
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


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* Function: CFE_FS_ExtractFilenameFromPath                                */
/*                                                                         */
/*  This function extracts the filename from a given path/filename         */
/*   combination.                                                          */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
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


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* Function: CFE_FS_BSPIsGzFile                                            */
/*                                                                         */
/*  This function will check to see if the file to load                    */
/*  is compressed with gzip. Currently it is based on the                  */
/*  file name. The file name must end in ".gz".                            */ 
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
bool CFE_FS_IsGzFile(const char *FileName)
{
   size_t    StringLength;
   
   if ( FileName == NULL )
   {
      return(false);
   }
   /*
   ** Get the string length of the SourceFile name
   */
   StringLength = strlen(FileName);
   
   /*
   ** The compressed filename must be named "something.gz"
   ** Because of that, the shortest compressed filename is "a.gz"
   ** Check for the length before determining if the file is zipped
   */
   if ( StringLength < 4 )
   {
      return(false);
   }

   /*
   ** if the last 3 characters are ".gz"
   ** Then assume the file is zipped.
   */
   if ( (FileName[StringLength - 3] == '.') &&
        (FileName[StringLength - 2] == 'g') &&
        (FileName[StringLength - 1] == 'z'))
   {
       return(true);       
   }
   else
   {
       return(false);
   }
  
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* Function: CFE_FS_GetUncompressedFile                                    */
/*                                                                         */
/*  Decompress a Loadable file to prepare for loading it                   */
/*                                                                         */
/*  A temporary filename on the ramdisk is generated, and the file is      */
/*  decompressed to it. The filename of the temporary file is output back  */
/*  to the caller.                                                         */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CFE_FS_GetUncompressedFile(char *OutputNameBuffer, uint32 OutputNameBufferSize,
        const char *GzipFileName, const char *TempDir)
{
    char    FileNameOnly[OS_MAX_PATH_LEN];
    int32   Status;
    int     RequiredLen;    /* using "int" per snprintf API */

    /*
    ** Extract the filename from the path
    */
    Status = CFE_FS_ExtractFilenameFromPath(GzipFileName, FileNameOnly);
    if ( Status != CFE_SUCCESS )
    {
       CFE_ES_WriteToSysLog("FS_GetUncompressedFile: Unable to extract filename from path: %s.\n",GzipFileName);
    }
    else
    {
        /*
        ** Build up the destination path in the RAM disk
        */
        RequiredLen = snprintf(OutputNameBuffer, OutputNameBufferSize,
                "%s/%s", TempDir, FileNameOnly);

        /*
        ** Remove the ".gz" prefix from the filename
        ** (Any input file name to this function must have a .gz extension,
        ** i.e. it passed the CFE_FS_IsGzFile() test, of which .gz is the only possibility)
        */
        if (RequiredLen >= 3)
        {
            RequiredLen -= 3;
        }

        /*
         * check if LoadNameBuffer got truncated
         * (snprintf returns the _required_ length, whether or not it actually fit)
         */
        if (RequiredLen < 0 || RequiredLen >= OutputNameBufferSize)
        {
            /* Can't include the name string since it could be too long for the message */
            CFE_ES_WriteToSysLog("FS_GetUncompressedFile: Temporary path plus file name length (%d) exceeds max allowed (%lu)\n",
                                 RequiredLen, (unsigned long)(OutputNameBufferSize-1));
            Status = CFE_FS_FNAME_TOO_LONG;
        }
        else
        {
            /*
             * Actually truncate the .gz from the string
             * (had to wait until after length verification)
             */
            OutputNameBuffer[RequiredLen] = 0;

            /*
            ** Decompress the file:
            */
            Status =  CFE_FS_Decompress( GzipFileName, OutputNameBuffer );
            if ( Status != CFE_SUCCESS )
            {
                CFE_ES_WriteToSysLog("FS_GetUncompressedFile: Unable to decompress %s, error=%08lx\n",
                        GzipFileName, (unsigned long)Status);
            }
        }
    }

    return Status;
}

/************************/
/*  End of File Comment */
/************************/
