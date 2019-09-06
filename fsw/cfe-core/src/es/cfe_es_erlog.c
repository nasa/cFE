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
**    cfe_es_erlog.c
**
**  Purpose:  
**    This file implements the cFE Executive Services Exception and Reset Log functions.
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**     cFE Flight Software Application Developers Guide
**
**  Notes:
** 
**  Modification History:
**
*/

/*
** Required header files.
*/
#include "private/cfe_private.h"
#include "cfe_es.h"
#include "cfe_es_apps.h"
#include "cfe_es_global.h"
#include "cfe_es_log.h"
#include "cfe_psp.h"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

/*
 * Confirm the size of the error log context buffer is at least what the user asked for.
 *
 * This is to catch errors such as if the CFE_PLATFORM_ES_ER_LOG_MAX_CONTEXT_SIZE was set to a value
 * that is _not_ a multiple of sizeof(uint32) -- in this case the final size of the context
 * buffer would end up being less than what the macro was set to.
 */
CompileTimeAssert(sizeof(CFE_ES_ResetDataPtr->ERLog[0].Context) == CFE_PLATFORM_ES_ER_LOG_MAX_CONTEXT_SIZE,  CfeEsErLogContextSizeError);


/*
** Function: CFE_ES_WriteToERLog
**
** Purpose:  Create an entry in the ES Exception and Reset Log. 
**
*/
int32 CFE_ES_WriteToERLog( uint32 EntryType,   uint32  ResetType, uint32 ResetSubtype,
                           const char  *Description, const uint32 *Context,   uint32 ContextSize )
{
   uint32 LogIdx;

   /*
   ** Code
   */

   /*
   ** Try to clean up an invalid ER log index variable.
   */
   if ( CFE_ES_ResetDataPtr->ERLogIndex >= CFE_PLATFORM_ES_ER_LOG_ENTRIES )
   {
      CFE_ES_ResetDataPtr->ERLogIndex = 0;
   }
   LogIdx = CFE_ES_ResetDataPtr->ERLogIndex;

   /*
   ** Now that the Local Index variable is set, increment the index for the next entry.
   */
   CFE_ES_ResetDataPtr->ERLogIndex++;
   if ( CFE_ES_ResetDataPtr->ERLogIndex >= CFE_PLATFORM_ES_ER_LOG_ENTRIES )
   {
      CFE_ES_ResetDataPtr->ERLogIndex = 0;
   }


   /*
   ** Clear out the log entry we are about to use.
   */
   memset( &(CFE_ES_ResetDataPtr->ERLog[LogIdx]), 0, 
            sizeof (CFE_ES_ERLog_t));

   /*
   ** Fill out the log fields
   */
   CFE_ES_ResetDataPtr->ERLog[LogIdx].LogEntryType = EntryType;
   CFE_ES_ResetDataPtr->ERLog[LogIdx].ResetType    = ResetType;
   CFE_ES_ResetDataPtr->ERLog[LogIdx].ResetSubtype = ResetSubtype;
   CFE_ES_ResetDataPtr->ERLog[LogIdx].BootSource   = CFE_ES_ResetDataPtr->ResetVars.BootSource;
   CFE_ES_ResetDataPtr->ERLog[LogIdx].ProcessorResetCount = 
                               CFE_ES_ResetDataPtr->ResetVars.ProcessorResetCount;
   CFE_ES_ResetDataPtr->ERLog[LogIdx].MaxProcessorResetCount = 
                               CFE_ES_ResetDataPtr->ResetVars.MaxProcessorResetCount;

   /*
   ** Copy the ES Reset variables to the log (before they are modified by the log entry).
   */
   memcpy(&(CFE_ES_ResetDataPtr->ERLog[LogIdx].DebugVars), 
          (void *)&(CFE_ES_Global.DebugVars), 
          sizeof(CFE_ES_DebugVariables_t ));

   /*
   ** Time Stamp the log entry with the system time 
   */
   CFE_ES_ResetDataPtr->ERLog[LogIdx].TimeCode = CFE_TIME_GetTime(); 

   /*
   ** Copy the Description string to the log.
   */
   if ( Description  == NULL)
   {
      strncpy(CFE_ES_ResetDataPtr->ERLog[LogIdx].Description, "No Description String Given.", 80);
   }
   else
   {
      strncpy(CFE_ES_ResetDataPtr->ERLog[LogIdx].Description, Description, 80);
   }

   /*
   ** In the case of an exception, copy the processor context data to the log.
   */
   if (Context != NULL && ContextSize > 0)
   {
      /*
      ** Copy the processor context data (i.e. register dump).  Make sure that
      ** the passed-in context_size is not greater than the declared size in
      ** the ER Log log entry.
      */
      if ( ContextSize <= CFE_PLATFORM_ES_ER_LOG_MAX_CONTEXT_SIZE )
      {
         memcpy ( (CFE_ES_ResetDataPtr->ERLog[LogIdx].Context),
                  (void *)Context,
                  ContextSize);
      }
      else
      {
         memcpy ( (CFE_ES_ResetDataPtr->ERLog[LogIdx].Context),
                  (void *)Context,
                  CFE_PLATFORM_ES_ER_LOG_MAX_CONTEXT_SIZE);
      }   
      /*
      ** Indicate that context is valid.
      ** Using the original context size (not the truncated size) so it will be
      ** evident if the context information was truncated.
      */
      CFE_ES_ResetDataPtr->ERLog[LogIdx].ContextSize = ContextSize;
   }
   else
   {
      /*
      ** Context is not valid
      */
      CFE_ES_ResetDataPtr->ERLog[LogIdx].ContextSize = 0;
   } /* end if */

   /*
   ** Increment the number of ER log entries made
   */
   CFE_ES_ResetDataPtr->ERLogEntries++;

   return(CFE_SUCCESS);
    
} /* End of CFE_ES_WriteToERLog() */

/* end of file */
