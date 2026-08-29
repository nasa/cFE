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

/*
**  File:
**    cfe_es_startupscript.c
**
**  Purpose:
**    This file contains functions for parsing the CFE ES startup script.
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**     cFE Flight Software Application Developers Guide
**
**  Notes:
**
*/

/*
** Includes
*/
#include "cfe_es_module_all.h"
#include "cfe_es_startupscript.h"

#include <stdio.h>
#include <string.h> /* memset() */
#include <fcntl.h>
#include <ctype.h>

#include "target_config.h"

/*----------------------------------------------------------------
 *
 * Local helper function
 * Opens the startup script file
 *
 *-----------------------------------------------------------------*/
static osal_id_t CFE_ES_OpenStartupScript(uint32 ResetType, const char *StartFilePath)
{
    char      ScriptFileName[OS_MAX_PATH_LEN];
    osal_id_t AppFile;
    int32     Status;
    int32     OsStatus;
    bool      FileOpened;

    FileOpened = false;

    /*
    ** Get the ES startup script filename.
    ** If this is a Processor Reset, try to open the file in the volatile disk first.
    */
    if (ResetType == CFE_PSP_RST_TYPE_PROCESSOR)
    {
        /*
        ** First Attempt to parse as file in the volatile disk (temp area).
        */
        Status = CFE_FS_ParseInputFileName(ScriptFileName,
                                           CFE_PLATFORM_ES_VOLATILE_STARTUP_FILE,
                                           sizeof(ScriptFileName),
                                           CFE_FS_FileCategory_TEMP);

        if (Status == CFE_SUCCESS)
        {
            OsStatus = OS_OpenCreate(&AppFile, ScriptFileName, OS_FILE_FLAG_NONE, OS_READ_ONLY);
            if (OsStatus == OS_SUCCESS)
            {
                FileOpened = true;
            }
            else
            {
                CFE_ES_WriteToSysLog("%s: Cannot Open Volatile Startup file: %s, Trying Nonvolatile.\n",
                                     __func__,
                                     ScriptFileName);
            }
        }
        else
        {
            /* not expected -- likely a misconfiguration in CFE_PLATFORM_ES_VOLATILE_STARTUP_FILE setting */
            CFE_ES_WriteToSysLog("%s: CFE_FS_ParseInputFileName() RC=%08x parsing volatile script file name.\n",
                                 __func__,
                                 (unsigned int)Status);
        }
    }

    /*
    ** This if block covers two cases: A Power on reset, and a Processor reset when
    ** the startup file on the volatile file system could not be opened.
    */
    if (!FileOpened)
    {
        /*
        ** Try to Open the file passed in to the cFE start.
        */
        Status = CFE_FS_ParseInputFileName(ScriptFileName,
                                           StartFilePath,
                                           sizeof(ScriptFileName),
                                           CFE_FS_FileCategory_SCRIPT);

        if (Status == CFE_SUCCESS)
        {
            OsStatus = OS_OpenCreate(&AppFile, ScriptFileName, OS_FILE_FLAG_NONE, OS_READ_ONLY);
            if (OsStatus == OS_SUCCESS)
            {
                FileOpened = true;
            }
            else
            {
                CFE_ES_WriteToSysLog("%s: Error, Can't Open ES App Startup file: %s, EC = %ld\n",
                                     __func__,
                                     ScriptFileName,
                                     (long)OsStatus);
            }
        }
        else
        {
            /* not expected -- likely a misconfiguration in the user-supplied StartFilePath */
            CFE_ES_WriteToSysLog("%s: CFE_FS_ParseInputFileName() RC=%08x parsing StartFilePath.\n",
                                 __func__,
                                 (unsigned int)Status);
        }
    }

    if (FileOpened)
    {
        CFE_ES_WriteToSysLog("%s: Opened ES App Startup file: %s\n", __func__, ScriptFileName);
    }
    else
    {
        /* be sure to return undefined if this did not work */
        AppFile = OS_OBJECT_ID_UNDEFINED;
    }

    return AppFile;
}

/*----------------------------------------------------------------
 *
 * Local helper function
 * Reads a single line from the startup script
 *
 *-----------------------------------------------------------------*/
static int32 CFE_ES_ReadStartupLine(osal_id_t   AppFile,
                                    const char *TokenList[],
                                    size_t     *NumTokensPtr,
                                    char       *BuffPtr,
                                    size_t     *BuffSizePtr)
{
    int32        OsStatus;
    char         c;
    char        *CurrPtr;
    char        *EndPtr;
    const char **CurrTokenPtr;
    const char **LastTokenPtr;

    /* placeholder values - overwritten later */
    OsStatus = OS_ERROR;

    CurrPtr = BuffPtr;
    EndPtr  = CurrPtr + *BuffSizePtr;

    CurrTokenPtr = TokenList;
    LastTokenPtr = CurrTokenPtr + *NumTokensPtr;

    --EndPtr; /* Save 1 char for the terminating NULL, always required */

    while (1)
    {
        c        = 0;
        OsStatus = OS_read(AppFile, &c, 1);
        if (OsStatus < OS_SUCCESS)
        {
            /* Leave OsStatus with error code - will be returned */
            break;
        }

        if (OsStatus == 0 || c == '!')
        {
            /* End of file marker */
            /* EOF reported from OSAL is treated the same as the EOF from a ! char */
            OsStatus = 0;
            break;
        }

        if (c == ';')
        {
            /* End of Line Reached, OsStatus will be left >0 */
            break;
        }

        /* process all other chars, if buffer space permits */
        if (CurrPtr < EndPtr)
        {
            if (c == ',')
            {
                /*
                ** replace the field delimiter with a null
                ** This is used to separate the tokens
                */
                *CurrPtr = 0;
                ++CurrPtr;

                if (CurrTokenPtr < LastTokenPtr)
                {
                    *CurrTokenPtr = CurrPtr;
                    ++CurrTokenPtr;
                }
            }
            else if (isgraph((int)c))
            {
                /* store normal (non-space) chars */
                *CurrPtr = c;
                ++CurrPtr;
            }
        }
    }

    /* space for this terminating NUL was reserved at the beginning */
    *CurrPtr      = 0;
    *NumTokensPtr = CurrTokenPtr - TokenList;
    *BuffSizePtr  = CurrPtr - BuffPtr;

    return OsStatus;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_ES_StartApplications(uint32 ResetType, const char *StartFilePath)
{
    char        ES_AppLoadBuffer[CFE_ES_START_BUFF_SIZE + 1]; /* A buffer of for a line in a file */
    const char *TokenList[CFE_ES_STARTSCRIPT_MAX_TOKENS_PER_LINE + 1];
    size_t      NumTokens;
    size_t      NumLines;
    size_t      BuffLen; /* Length of the current buffer */
    osal_id_t   AppFile = OS_OBJECT_ID_UNDEFINED;
    int32       OsStatus;

    /* There may be more than one startup script - this finds and opens the correct one */
    AppFile = CFE_ES_OpenStartupScript(ResetType, StartFilePath);

    /*
    ** If the file is opened in either the Nonvolatile or the Volatile disk, process it.
    */
    if (OS_ObjectIdDefined(AppFile))
    {
        memset(ES_AppLoadBuffer, 0x0, sizeof(ES_AppLoadBuffer));
        NumLines = 0;

        /*
        ** Parse the lines from the file. If it has an error
        ** or reaches EOF, then abort the loop.
        */
        do
        {
            /* The first token is always assumed to be at the beginning of the line */
            TokenList[0] = ES_AppLoadBuffer;
            NumTokens    = CFE_ES_STARTSCRIPT_MAX_TOKENS_PER_LINE;
            BuffLen      = sizeof(ES_AppLoadBuffer);

            OsStatus = CFE_ES_ReadStartupLine(AppFile, &TokenList[1], &NumTokens, ES_AppLoadBuffer, &BuffLen);
            if (OsStatus < OS_SUCCESS)
            {
                CFE_ES_WriteToSysLog("%s: Error Reading Startup file. EC = %ld\n", __func__, (long)OsStatus);
                break;
            }

            /* Note: because both of these buffers are sized +1 from the allowed amount,
             * it means if we filled it, then the input was too long */
            if (BuffLen >= CFE_ES_START_BUFF_SIZE || NumTokens >= CFE_ES_STARTSCRIPT_MAX_TOKENS_PER_LINE)
            {
                /* The line was not formed correctly */
                CFE_ES_WriteToSysLog("%s: **WARNING** File Line %u is malformed: %u bytes, %u tokens.\n",
                                     __func__,
                                     (unsigned int)NumLines + 1,
                                     (unsigned int)BuffLen,
                                     (unsigned int)NumTokens);
            }
            else if (NumTokens > 0)
            {
                /* Send the line to the file parser */
                CFE_ES_ParseFileEntry(TokenList, 1 + NumTokens);
                ++NumLines;
            }
        } while (OsStatus > 0);

        /*
        ** close the file
        */
        OS_close(AppFile);
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_ParseFileEntry(const char **TokenList, uint32 NumTokens)
{
    const char   *ModuleName;
    const char   *EntryType;
    unsigned long ParsedValue;
    union
    {
        CFE_ES_AppId_t AppId;
        CFE_ES_LibId_t LibId;
    } IdBuf;
    int32                   Status;
    CFE_ES_AppStartParams_t ParamBuf;

    memset(&ParamBuf, 0, sizeof(ParamBuf));

    /*
    ** Check to see if the correct number of items were parsed
    */
    if (NumTokens < 8)
    {
        CFE_ES_WriteToSysLog("%s: Invalid ES Startup file entry: %u\n", __func__, (unsigned int)NumTokens);
        return CFE_ES_BAD_ARGUMENT;
    }

    /* Get pointers to specific tokens that are simple strings used as-is */
    EntryType  = TokenList[0];
    ModuleName = TokenList[3];

    /*
     * Other tokens will need to be scrubbed/converted.
     * Both Libraries and Apps use File Name (1) and Symbol Name (2) fields so copy those now
     */
    memset(&ParamBuf, 0, sizeof(ParamBuf));
    Status = CFE_FS_ParseInputFileName(ParamBuf.BasicInfo.FileName,
                                       TokenList[1],
                                       sizeof(ParamBuf.BasicInfo.FileName),
                                       CFE_FS_FileCategory_DYNAMIC_MODULE);
    if (Status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: Invalid ES Startup script file name: %s\n", __func__, TokenList[1]);
        return Status;
    }

    strncpy(ParamBuf.BasicInfo.InitSymbolName, TokenList[2], sizeof(ParamBuf.BasicInfo.InitSymbolName) - 1);

    if (strcmp(EntryType, "CFE_APP") == 0)
    {
        CFE_ES_WriteToSysLog("%s: Loading file: %s, APP: %s\n", __func__, ParamBuf.BasicInfo.FileName, ModuleName);

        /*
         * Priority and Exception action have limited ranges, which is checked here
         * Task priority cannot be bigger than OS_MAX_TASK_PRIORITY
         */
        ParsedValue = strtoul(TokenList[4], NULL, 0);
        if (ParsedValue > OS_MAX_TASK_PRIORITY)
        {
            ParamBuf.MainTaskInfo.Priority = OS_MAX_TASK_PRIORITY;
        }
        else
        {
            /* convert parsed value to correct type */
            ParamBuf.MainTaskInfo.Priority = (CFE_ES_TaskPriority_Atom_t)ParsedValue;
        }

        /* No specific upper/lower limit for stack size - will pass value through */
        ParamBuf.MainTaskInfo.StackSize = strtoul(TokenList[5], NULL, 0);

        /*
        ** Validate Some parameters
        ** Exception action should be 0 ( Restart App ) or
        ** 1 ( Processor reset ). If it's non-zero, assume it means
        ** reset CPU.
        */
        ParsedValue = strtoul(TokenList[7], NULL, 0);
        if (ParsedValue > CFE_ES_ExceptionAction_RESTART_APP)
        {
            ParamBuf.ExceptionAction = CFE_ES_ExceptionAction_PROC_RESTART;
        }
        else
        {
            /* convert parsed value to correct type */
            ParamBuf.ExceptionAction = (CFE_ES_ExceptionAction_Enum_t)ParsedValue;
        }

        /*
        ** Now create the application
        */
        Status = CFE_ES_AppCreate(&IdBuf.AppId, ModuleName, &ParamBuf);
    }
    else if (strcmp(EntryType, "CFE_LIB") == 0)
    {
        CFE_ES_WriteToSysLog("%s: Loading shared library: %s\n", __func__, ParamBuf.BasicInfo.FileName);

        /*
        ** Now load the library
        */
        Status = CFE_ES_LoadLibrary(&IdBuf.LibId, ModuleName, &ParamBuf.BasicInfo);
    }
    else
    {
        CFE_ES_WriteToSysLog("%s: Unexpected EntryType %s in startup file.\n", __func__, EntryType);
        Status = CFE_ES_ERR_APP_CREATE;
    }

    return Status;
}
