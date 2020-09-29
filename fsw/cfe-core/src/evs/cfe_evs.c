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
**
**  File: cfe_evs.c
**
**  Title: Event Services API library
**
**  Purpose: This module defines the library functions of the
**           Event Services API
**
*/

/* Include Files */
#include "cfe_evs.h"         /* EVS library function definitions */
#include "cfe_evs_task.h"    /* EVS internal definitions */
#include "cfe_evs_utils.h"   /* EVS utility function definitions */
#include "common_types.h"    /* Basic data types */
#include "cfe_es.h"          /* Executive Service definitions */
#include "cfe_error.h"       /* cFE error code definitions */

#include <stdarg.h>
#include <string.h>

/* External Data */

/* Local Function Prototypes */


/* Function Definitions */

/*
** Function: CFE_EVS_Register - See API and header file for details
*/
int32 CFE_EVS_Register (void *Filters, uint16 NumEventFilters, uint16 FilterScheme)
{
   uint16 FilterLimit;
   uint16 i;
   int32  Status;
   CFE_ES_ResourceID_t    AppID;
   CFE_EVS_BinFilter_t   *AppFilters;
   EVS_AppData_t         *AppDataPtr;

   /* Query and verify the caller's AppID */
   Status = EVS_GetCurrentContext(&AppDataPtr, &AppID);
   if (Status == CFE_SUCCESS)
   {
      /* Clear and configure entry */
      memset(AppDataPtr, 0, sizeof(EVS_AppData_t));

      /* Verify filter arguments */
      if (FilterScheme != CFE_EVS_EventFilter_BINARY)
      {
         Status = CFE_EVS_UNKNOWN_FILTER;
      }
      else if ((NumEventFilters != 0) && (Filters == NULL))
      {
         Status = CFE_ES_ERR_BUFFER;
      }
      else
      {
         /* Initialize application event data */
         AppDataPtr->ActiveFlag   = true;
         AppDataPtr->EventTypesActiveFlag = CFE_PLATFORM_EVS_DEFAULT_TYPE_FLAG;

         /* Set limit for number of provided filters */
         if (NumEventFilters < CFE_PLATFORM_EVS_MAX_EVENT_FILTERS)
         {
            FilterLimit = NumEventFilters;
         }
         else
         {
            FilterLimit = CFE_PLATFORM_EVS_MAX_EVENT_FILTERS;
         }

         if (Filters != NULL)
         {
            AppFilters = (CFE_EVS_BinFilter_t *) Filters;

            /* Copy provided filters */
            for (i = 0; i < FilterLimit; i++)
            {
               AppDataPtr->BinFilters[i].EventID = AppFilters[i].EventID;
               AppDataPtr->BinFilters[i].Mask    = AppFilters[i].Mask;
               AppDataPtr->BinFilters[i].Count   = 0;
            }
         }

         /* Initialize remainder of filters as unused */
         for (i = FilterLimit; i < CFE_PLATFORM_EVS_MAX_EVENT_FILTERS; i++)
         {
            AppDataPtr->BinFilters[i].EventID = CFE_EVS_FREE_SLOT;
            AppDataPtr->BinFilters[i].Mask    = 0;
            AppDataPtr->BinFilters[i].Count   = 0;
         }

         EVS_AppDataSetUsed(AppDataPtr, AppID);
      }
   }

   return(Status);

} /* End CFE_EVS_Register */

/*
** Function: CFE_EVS_Unregister - See API and header file for details
*/
int32 CFE_EVS_Unregister(void)
{
   int32 Status;
   CFE_ES_ResourceID_t AppID;
   EVS_AppData_t *AppDataPtr;

   /* Query and verify the caller's AppID */
   Status = EVS_GetCurrentContext(&AppDataPtr, &AppID);
   if (Status == CFE_SUCCESS &&
           EVS_AppDataIsMatch(AppDataPtr, AppID))
   {
       EVS_AppDataSetFree(AppDataPtr);
   }

   return(Status);

} /* End CFE_EVS_Unregister */

/*
** Function: CFE_EVS_SendEvent - See API and header file for details
*/
int32 CFE_EVS_SendEvent (uint16 EventID, uint16 EventType, const char *Spec, ... )
{
   int32              Status;
   CFE_ES_ResourceID_t AppID;
   CFE_TIME_SysTime_t Time;
   va_list            Ptr;
   EVS_AppData_t     *AppDataPtr;

   /* Query and verify the caller's AppID */
   Status = EVS_GetCurrentContext(&AppDataPtr, &AppID);
   if (Status == CFE_SUCCESS)
   {
      if (!EVS_AppDataIsMatch(AppDataPtr, AppID))
      {
         /* Handler for events from apps not registered with EVS */
         Status = EVS_NotRegistered(AppDataPtr, AppID);
      }
      else if (EVS_IsFiltered(AppDataPtr, EventID, EventType) == false)
      {
          /* Get current spacecraft time */
          Time = CFE_TIME_GetTime();

          /* Send the event packets */
          va_start(Ptr, Spec);
          EVS_GenerateEventTelemetry(AppDataPtr, EventID, EventType, &Time, Spec, Ptr);
          va_end(Ptr);
      }
   }

   return(Status);

} /* End CFE_EVS_SendEvent */


/*
** Function: CFE_EVS_SendEventWithAppID - See API and header file for details
*/
int32 CFE_EVS_SendEventWithAppID (uint16 EventID, uint16 EventType, CFE_ES_ResourceID_t AppID, const char *Spec, ... )
{
   int32              Status = CFE_SUCCESS;
   CFE_TIME_SysTime_t Time;
   va_list            Ptr;
   EVS_AppData_t     *AppDataPtr;

   AppDataPtr = EVS_GetAppDataByID (AppID);
   if (AppDataPtr == NULL)
   {
      Status = CFE_EVS_APP_ILLEGAL_APP_ID;
   }
   else if (!EVS_AppDataIsMatch(AppDataPtr, AppID))
   {
      /* Handler for events from apps not registered with EVS */
      Status = EVS_NotRegistered(AppDataPtr, AppID);
   }
   else if (EVS_IsFiltered(AppDataPtr, EventID, EventType) == false)
   {
      /* Get current spacecraft time */
      Time = CFE_TIME_GetTime();

      /* Send the event packets */
      va_start(Ptr, Spec);
      EVS_GenerateEventTelemetry(AppDataPtr, EventID, EventType, &Time, Spec, Ptr);
      va_end(Ptr);
   }

   return Status;

} /* End CFE_EVS_SendEventWithAppID */

/*
** Function: CFE_EVS_SendTimedEvent - See API and header file for details
*/
int32 CFE_EVS_SendTimedEvent (CFE_TIME_SysTime_t Time, uint16 EventID, uint16 EventType, const char *Spec, ... )
{
   int32              Status;
   CFE_ES_ResourceID_t AppID;
   va_list            Ptr;
   EVS_AppData_t     *AppDataPtr;

   /* Query and verify the caller's AppID */
   Status = EVS_GetCurrentContext(&AppDataPtr, &AppID);
   if (Status == CFE_SUCCESS)
   {
      if (!EVS_AppDataIsMatch(AppDataPtr, AppID))
      {
         /* Handler for events from apps not registered with EVS */
         Status = EVS_NotRegistered(AppDataPtr, AppID);
      }
      else if (EVS_IsFiltered(AppDataPtr, EventID, EventType) == false)
      {
         /* Send the event packets */
         va_start(Ptr, Spec);
         EVS_GenerateEventTelemetry(AppDataPtr, EventID, EventType, &Time, Spec, Ptr);
         va_end(Ptr);
      }
   }

   return(Status);

} /* End CFE_EVS_SendTimedEvent */

/*
** Function: CFE_EVS_ResetFilter - See API and header file for details
*/
int32 CFE_EVS_ResetFilter (int16 EventID)
{
   int32            Status;
   EVS_BinFilter_t *FilterPtr = NULL;
   CFE_ES_ResourceID_t AppID;
   EVS_AppData_t   *AppDataPtr;

   /* Query and verify the caller's AppID */
   Status = EVS_GetCurrentContext(&AppDataPtr, &AppID);
   if (Status == CFE_SUCCESS)
   {
      if (!EVS_AppDataIsMatch(AppDataPtr, AppID))
      {
         Status = CFE_EVS_APP_NOT_REGISTERED;
      }
      else
      {
         FilterPtr = EVS_FindEventID(EventID, AppDataPtr->BinFilters);

         if (FilterPtr != NULL)
         {
            FilterPtr->Count = 0;
         }
         else
         {
            Status = CFE_EVS_EVT_NOT_REGISTERED;
         }
      }
   }

   return(Status);

} /* End CFE_EVS_ResetFilter */


/*
** Function: CFE_EVS_ResetAllFilters - See API and header file for details
*/
int32 CFE_EVS_ResetAllFilters ( void )
{
   int32    Status;
   CFE_ES_ResourceID_t AppID;
   uint32   i;
   EVS_AppData_t *AppDataPtr;

   /* Query and verify the caller's AppID */
   Status = EVS_GetCurrentContext(&AppDataPtr, &AppID);
   if (Status == CFE_SUCCESS)
   {
      if (!EVS_AppDataIsMatch(AppDataPtr, AppID))
      {
         Status = CFE_EVS_APP_NOT_REGISTERED;
      }
      else
      {
         for (i = 0; i < CFE_PLATFORM_EVS_MAX_EVENT_FILTERS; i++)
         {
            AppDataPtr->BinFilters[i].Count = 0;
         }
      }
   }

   return(Status);

} /* End CFE_EVS_ResetAllFilters */


/* End CFE_EVS.C */
