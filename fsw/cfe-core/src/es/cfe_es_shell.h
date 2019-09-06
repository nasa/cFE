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
**  File:  
**    cfe_es_shell.h
**
**  Purpose:  
**    This file contains definitions needed for the internal shell functions.
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**     cFE Flight Software Application Developers Guide
**
**  Notes:
** 
*/

#ifndef _cfe_es_shell_
#define _cfe_es_shell_

/*
** Include Files
*/
#include "cfe.h"

/*
** Macro Definitions
*/
#define CFE_ES_LIST_APPS_CMD        "ES_ListApps"
#define CFE_ES_LIST_RESOURCES_CMD   "ES_ListResources"
#define CFE_ES_LIST_TASKS_CMD       "ES_ListTasks"

/*
** Type Definitions
*/


/*
** Function prototypes
*/
/*
** Operating System Shell function
*/

int32 CFE_ES_ShellOutputCommand(const char * CmdString, const char *Filename);

int32 CFE_ES_ListApplications(int32 fd);
int32 CFE_ES_ListTasks(int32 fd);
int32 CFE_ES_ListResources(int32 fd);
#endif  /* cfe_es_shell_ */
