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
**  File:  
**    cfe_es_start.c
**
**  Purpose:  
**      Provides the load-time configuration of which shared libraries and applications should
**      be loaded into the cFS instance.
**
**  References:
**
**  Notes:
** 
**  Modification History:
**
*/

/*
** Required header files.
*/

#include "cfe_tbl_filedef.h" /* Required for the CFE_TBL_FILEDEF macro */
#include "cfe_es_table.h"

CFE_ES_AppLibTbl_t CFE_ES_AppLibTbl = {
    /** Libraries **/
    .Libs = {
        /** { <name> <shared library file path>, <entry point> } **/
        { /* Lib #0 */
            .Name =             "SAMPLE_LIB",
            .FileName =         "/cf/sample_lib.so",
            .EntryPoint =       "SAMPLE_LibInit"
        },
    },
    /** Applications **/
    {
        /** { <name>, <shared library file path>, <entry point>, <stack size>, <priority>,
                <1 == reset processor on err> } **/
        { /* App #0 */
            .Name =             "SAMPLE_APP",
            .FileName =         "/cf/sample_app.so",
            .EntryPoint =       "SAMPLE_AppMain",
            .StackSize =        16384,
            .Priority =         50,
            .ExceptionAction =  0
        },
        { /* App #1 */
            .Name =             "CI_LAB",
            .FileName =         "/cf/ci_lab.so",
            .EntryPoint =       "CI_Lab_AppMain",
            .StackSize =        16384,
            .Priority =         60,
            .ExceptionAction =  0
        },
        { /* App #2 */
            .Name =             "TO_LAB",
            .FileName =         "/cf/to_lab.so",
            .EntryPoint =       "TO_Lab_AppMain",
            .StackSize =        16384,
            .Priority =         70,
            .ExceptionAction =  0
        },
        { /* App #3 */
            .Name =             "SCH_LAB_APP",
            .FileName =         "/cf/sch_lab.so",
            .EntryPoint =       "SCH_Lab_AppMain",
            .StackSize =        16384,
            .Priority =         80,
            .ExceptionAction =  0
        },
    }
};

CFE_TBL_FILEDEF(CFE_ES_AppLibTbl, CFE_ES.CFE_ES_AppLibTbl, ES App and Lib Table, cfe_es_start.tbl)
