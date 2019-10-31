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

/* File: 
**   cfe_es_shell.c
**
**  Purpose:
**  cFE Executive Services (ES) Shell Commanding System
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**     cFE Flight Software Application Developers Guide
**
*/

/*
 ** Includes
 */
#include "private/cfe_private.h"
#include "cfe_es_global.h"
#include "cfe_es_apps.h"
#include "cfe_es_shell.h"
#include "cfe_es_task.h"
#include "cfe_es_log.h"
#include "cfe_psp.h"


#include <string.h>

#define  CFE_ES_CHECKSIZE 3
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* CFE_ES_ShellOutputCommand() -- Pass thru string to O/S shell or to ES */
/*                                                                       */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CFE_ES_ShellOutputCommand(const char * CmdString, const char *Filename)
{
    int32 Result;
    int32 ReturnCode = CFE_SUCCESS;
    int32 fd;
    int32 FileSize;
    int32 CurrFilePtr;
    uint32 i;

    /* Use default filename if not provided */
    if (Filename == NULL || Filename[0] == '\0')
    {   
        Filename = CFE_PLATFORM_ES_DEFAULT_SHELL_FILENAME;
    }

    /* Remove previous version of output file */
    OS_remove(Filename); 

    fd = OS_creat(Filename, OS_READ_WRITE);

    if (fd < OS_FS_SUCCESS)
    {
        Result = OS_FS_ERROR;
    }

    else
    {
        /* We need to check if this command is directed at ES, or at the 
        operating system */
    
        if (strncmp(CmdString,"ES_",CFE_ES_CHECKSIZE) == 0)
        {
            /* This list can be expanded to include other ES functionality */
            if ( strcmp(CmdString,CFE_ES_LIST_APPS_CMD) == 0)
            {
                Result = CFE_ES_ListApplications(fd);
            }
            else if ( strcmp(CmdString,CFE_ES_LIST_TASKS_CMD) == 0)
            {
                Result = CFE_ES_ListTasks(fd);
            }
            else if ( strcmp(CmdString,CFE_ES_LIST_RESOURCES_CMD) == 0)
            {
                Result = CFE_ES_ListResources(fd);
            }

            /* default if there is not an ES command that matches */
            else
            {
                Result = CFE_ES_ERR_SHELL_CMD;
                CFE_ES_WriteToSysLog("There is no ES Shell command that matches %s \n",CmdString);
            }            

        }
        /* if the command is not directed at ES, pass it through to the 
        * underlying OS */
        else
        {
            Result = OS_ShellOutputToFile(CmdString,fd);
        }

        /* seek to the end of the file to get it's size */
        FileSize = OS_lseek(fd,0,OS_SEEK_END);

        if (FileSize == OS_FS_ERROR)
        {
            OS_close(fd);
            CFE_ES_WriteToSysLog("OS_lseek call failed from CFE_ES_ShellOutputCmd 1\n");
            Result =  OS_FS_ERROR;
        }



        /* We want to add 3 characters at the end of the telemetry,'\n','$','\0'.
         * To do this we need to make sure there are at least 3 empty characters at
         * the end of the last CFE_MISSION_ES_MAX_SHELL_PKT so we don't over write any data. If
         * the current file has only 0,1, or 2 free spaces at the end, we want to 
         * make the file longer to start a new tlm packet of size CFE_MISSION_ES_MAX_SHELL_PKT.
         * This way we will get a 'blank' packet with the correct 3 characters at the end.
         */

        else
        {
            /* if we are within 2 bytes of the end of the packet*/
            if ( FileSize % CFE_MISSION_ES_MAX_SHELL_PKT > (CFE_MISSION_ES_MAX_SHELL_PKT - 3))
            {
                /* add enough bytes to start a new packet */
                for (i = 0; i < CFE_MISSION_ES_MAX_SHELL_PKT - (FileSize % CFE_MISSION_ES_MAX_SHELL_PKT) + 1 ; i++)
                {
                    OS_write(fd," ",1);
                }
            }
            else
            {
                /* we are exactly at the end */
                if( FileSize % CFE_MISSION_ES_MAX_SHELL_PKT == 0)
                {
                    OS_write(fd," ",1);
                }
            }

            /* seek to the end of the file again to get it's new size */
            FileSize = OS_lseek(fd,0,OS_SEEK_END);

            if (FileSize == OS_FS_ERROR)
            {
                OS_close(fd);
                CFE_ES_WriteToSysLog("OS_lseek call failed from CFE_ES_ShellOutputCmd 2\n");
                Result =  OS_FS_ERROR;
            }


            else
            {
                /* set the file back to the beginning */
                OS_lseek(fd,0,OS_SEEK_SET);


                /* start processing the chunks. We want to have one packet left so we are sure this for loop
                * won't run over */
        
                for (CurrFilePtr=0; CurrFilePtr < (FileSize - CFE_MISSION_ES_MAX_SHELL_PKT); CurrFilePtr += CFE_MISSION_ES_MAX_SHELL_PKT)
                {
                    OS_read(fd, CFE_ES_TaskData.ShellPacket.Payload.ShellOutput, CFE_MISSION_ES_MAX_SHELL_PKT);

                    /* Send the packet */
                    CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &CFE_ES_TaskData.ShellPacket);
                    CFE_SB_SendMsg((CFE_SB_Msg_t *) &CFE_ES_TaskData.ShellPacket);
                    /* delay to not flood the pipe on large messages */
                    OS_TaskDelay(CFE_PLATFORM_ES_SHELL_OS_DELAY_MILLISEC);
                }

                /* finish off the last portion of the file */
                /* over write the last packet with spaces, then it will get filled
               * in with the correct info below. This assures that the last non full
               * part of the packet will be spaces */
                for (i =0; i < CFE_MISSION_ES_MAX_SHELL_PKT; i++)
                {
                    CFE_ES_TaskData.ShellPacket.Payload.ShellOutput[i] = ' ';
                }
  
                OS_read(fd, CFE_ES_TaskData.ShellPacket.Payload.ShellOutput, ( FileSize - CurrFilePtr));

                /* From our check above, we are assured that there are at least 3 free
                 * characters to write our data into at the end of this last packet 
                 * 
                 * The \n assures we are on a new line, the $ gives us our prompt, and the 
                 * \0 assures we are null terminalted.
                 */

        
                CFE_ES_TaskData.ShellPacket.Payload.ShellOutput[ CFE_MISSION_ES_MAX_SHELL_PKT - 3] = '\n';
                CFE_ES_TaskData.ShellPacket.Payload.ShellOutput[ CFE_MISSION_ES_MAX_SHELL_PKT - 2] = '$';
                CFE_ES_TaskData.ShellPacket.Payload.ShellOutput[ CFE_MISSION_ES_MAX_SHELL_PKT - 1] = '\0';

                /* Send the last packet */
                CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &CFE_ES_TaskData.ShellPacket);
                CFE_SB_SendMsg((CFE_SB_Msg_t *) &CFE_ES_TaskData.ShellPacket);
   
                /* Close the file descriptor */
                OS_close(fd);
            } /* if FilseSize == OS_FS_ERROR */
        } /* if FileSeize == OS_FS_ERROR */
    }/* if fd < OS_FS_SUCCESS */


    /* cppcheck-suppress duplicateExpression */
    if (Result != OS_SUCCESS && Result != CFE_SUCCESS )
    {
        ReturnCode = CFE_ES_ERR_SHELL_CMD;
        CFE_ES_WriteToSysLog("OS_ShellOutputToFile call failed from CFE_ES_ShellOutputCommand\n");
    }
    else
    {
        ReturnCode = CFE_SUCCESS;
    }
    
    return ReturnCode;
}  
    
    
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_ListApplications() -- List All ES Applications,put in fd */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CFE_ES_ListApplications(int32 fd)
{
    uint32 i;
    char Line [OS_MAX_API_NAME +2];
    int32 Result = CFE_SUCCESS;
    
    /* Make sure we start at the beginning of the file */
    OS_lseek(fd,0, OS_SEEK_SET);
    
    for ( i = 0; i < CFE_PLATFORM_ES_MAX_APPLICATIONS; i++ )
    {
        if ( CFE_ES_Global.AppTable[i].AppState != CFE_ES_AppState_UNDEFINED && (Result == CFE_SUCCESS) )
        {
            /* We found an in use app. Write it to the file */
            strcpy(Line, (char*) CFE_ES_Global.AppTable[i].StartParams.Name);
            strcat(Line,"\n");             
            Result = OS_write(fd, Line, strlen(Line));
            
            if (Result == strlen(Line))
            {
                Result = CFE_SUCCESS;
            }
            /* if not success, returns whatever OS_write failire was */
            
        }
    } /* end for */

    return Result;
} /* end ES_ListApplications */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_ListTasks() -- List All ES Tasks,put in fd               */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CFE_ES_ListTasks(int32 fd)
{
    uint32                i;
    /*
     * The "Line" buffer is used for temporary storage of each output line
     * prior to writing to the file.
     *
     * 96 bytes are reserved for the constant/fixed length content, plus 
     * up to 2 instances of Task/Application names which are of a maximum
     * length defined by osconfig.
     */
    char                 Line [96 + (OS_MAX_API_NAME * 2)];
    int32                Result = CFE_SUCCESS;
    CFE_ES_TaskInfo_t    TaskInfo;
    
    /* Make sure we start at the beginning of the file */
    Result = OS_lseek(fd, 0, OS_SEEK_SET);
    if ( Result == 0 ) 
    {
       snprintf(Line,sizeof(Line),"---- ES Task List ----\n");
       Result = OS_write(fd, Line, strlen(Line));
       if (Result == strlen(Line))
       {
          Result = CFE_SUCCESS;
          for ( i = 0; i < OS_MAX_TASKS; i++ )
          {
             if ((CFE_ES_Global.TaskTable[i].RecordUsed == true) && (Result == CFE_SUCCESS))
             {      
                /* 
                ** zero out the local entry 
                */
                memset(&TaskInfo,0,sizeof(CFE_ES_TaskInfo_t));

                /*
                ** Populate the AppInfo entry 
                */
                Result = CFE_ES_GetTaskInfo(&TaskInfo,CFE_ES_Global.TaskTable[i].TaskId);

                if ( Result == CFE_SUCCESS )
                {
                   snprintf(Line,sizeof(Line),"Task ID: %08d, Task Name: %20s, Prnt App ID: %08d, Prnt App Name: %20s\n",
                         (int) TaskInfo.TaskId, TaskInfo.TaskName, 
                         (int)TaskInfo.AppId, TaskInfo.AppName);
                   Result = OS_write(fd, Line, strlen(Line));
            
                   if (Result == strlen(Line))
                   {
                      Result = CFE_SUCCESS;
                   }
                   /* if not success, returns whatever OS_write failire was */
                }
             }
          } /* end for */
       } /* End if OS_write */
    } /* End if OS_lseek */ 
    return Result;
} /* end ES_ListTasks */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_ShellCountObjectCallback() -- Helper function            */
/*  (used by CFE_ES_ListResources() below)                         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
static void CFE_ES_ShellCountObjectCallback(uint32 object_id, void *arg)
{
    uint32                 *CountState;
    uint32                 idtype;

    CountState = (uint32 *)arg;
    idtype = OS_IdentifyObject(object_id);
    if (idtype < OS_OBJECT_TYPE_USER)
    {
        ++CountState[idtype];
    }
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_ListResources() -- List All OS Resources, put in fd      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CFE_ES_ListResources(int32 fd)
{
    int32 Result = CFE_SUCCESS;
    int32 NumSemaphores = 0;
    int32 NumCountSems =0;
    int32 NumMutexes = 0;
    int32 NumQueues = 0;
    int32 NumTasks = 0;
    int32 NumFiles = 0;
    char Line[35];
    uint32 CountState[OS_OBJECT_TYPE_USER];

    memset(&CountState,0,sizeof(CountState));
    OS_ForEachObject (0, CFE_ES_ShellCountObjectCallback, CountState);

    NumSemaphores = CountState[OS_OBJECT_TYPE_OS_BINSEM];
    NumCountSems = CountState[OS_OBJECT_TYPE_OS_COUNTSEM];
    NumMutexes = CountState[OS_OBJECT_TYPE_OS_MUTEX];
    NumQueues = CountState[OS_OBJECT_TYPE_OS_QUEUE];
    NumTasks = CountState[OS_OBJECT_TYPE_OS_TASK];
    NumFiles = CountState[OS_OBJECT_TYPE_OS_STREAM];
    snprintf(Line,sizeof(Line),"OS Resources in Use:\n");
    Result = OS_write(fd, Line, strlen(Line));
    
    if( Result == strlen(Line))
    {   
        snprintf(Line,sizeof(Line),"Number of Tasks: %d\n", (int) NumTasks);
        Result = OS_write(fd, Line, strlen(Line));

        if (Result == strlen(Line))
        {
            snprintf(Line,sizeof(Line),"Number of Queues: %d\n", (int) NumQueues);
            Result = OS_write(fd, Line, strlen(Line));
            
            if (Result == strlen(Line))
            {
                snprintf(Line,sizeof(Line),"Number of Binary Semaphores: %d\n",(int) NumSemaphores);
                Result = OS_write(fd, Line, strlen(Line));
                if (Result == strlen(Line))
                {
                
                   
                    snprintf(Line,sizeof(Line),"Number of Counting Semaphores: %d\n",(int) NumCountSems);
                    Result = OS_write(fd, Line, strlen(Line));
                 
                    if (Result == strlen(Line))
                    {
                        snprintf(Line,sizeof(Line),"Number of Mutexes: %d\n", (int) NumMutexes);
                        Result = OS_write(fd, Line, strlen(Line));
                        if (Result == strlen(Line))
                        {
                            snprintf(Line,sizeof(Line),"Number of Open Files: %d\n",(int) NumFiles);
                            Result = OS_write(fd, Line, strlen(Line));
                            if ( Result == strlen(Line))
                            {
                               Result = CFE_SUCCESS;
                            }
                        }
                    }
                }   
            }
        }
    }
            
    /* 
    ** If any of the writes failed, return the OS_write 
    **  failure 
    */
    return Result;
}
