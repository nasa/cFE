/*
** $Id: cfe_time_verify.h 1.6 2014/04/14 10:51:41GMT-05:00 lwalling Exp  $
**
**      GSC-18128-1, "Core Flight Executive Version 6.6"
**
**      Copyright (c) 2006-2019 United States Government as represented by
**      the Administrator of the National Aeronautics and Space Administration.
**      All Rights Reserved.
**
**      Licensed under the Apache License, Version 2.0 (the "License");
**      you may not use this file except in compliance with the License.
**      You may obtain a copy of the License at
**
**        http://www.apache.org/licenses/LICENSE-2.0
**
**      Unless required by applicable law or agreed to in writing, software
**      distributed under the License is distributed on an "AS IS" BASIS,
**      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**      See the License for the specific language governing permissions and
**      limitations under the License.
**
** Purpose:  cFE Time Services (TIME) configuration verification
**
** Author:   S.Walling/Microtel
**
** Notes:
**
**
*/

/*
** Ensure that header is included only once...
*/
#ifndef _cfe_time_verify_
#define _cfe_time_verify_

/*************************************************************************/

/*
** Validate default time client/server selection...
*/
#if (CFE_PLATFORM_TIME_CFG_SERVER == TRUE)
  #if (CFE_PLATFORM_TIME_CFG_CLIENT == TRUE)
    #error Cannot define both CFE_PLATFORM_TIME_CFG_SERVER and CFE_PLATFORM_TIME_CFG_CLIENT as TRUE!
  #endif
#else
  #if (CFE_PLATFORM_TIME_CFG_CLIENT != TRUE)
    #error Must define either CFE_PLATFORM_TIME_CFG_SERVER or CFE_PLATFORM_TIME_CFG_CLIENT as TRUE!
  #endif
#endif

/*
** Validate default time format selection...
*/
#if (CFE_MISSION_TIME_CFG_DEFAULT_TAI == TRUE)
  #if (CFE_MISSION_TIME_CFG_DEFAULT_UTC == TRUE)
    #error Cannot define both CFE_MISSION_TIME_CFG_DEFAULT_UTC and CFE_MISSION_TIME_CFG_DEFAULT_TAI as TRUE!
  #endif
#else
  #if (CFE_MISSION_TIME_CFG_DEFAULT_UTC != TRUE)
    #error Must define either CFE_MISSION_TIME_CFG_DEFAULT_UTC or CFE_MISSION_TIME_CFG_DEFAULT_TAI as TRUE!
  #endif
#endif

/*
** Validate time source selection...
*/
#if (CFE_PLATFORM_TIME_CFG_CLIENT == TRUE)
  #if (CFE_PLATFORM_TIME_CFG_SOURCE == TRUE)
    #error Cannot define both CFE_PLATFORM_TIME_CFG_CLIENT and CFE_PLATFORM_TIME_CFG_SOURCE as TRUE!
  #endif
#endif

#if (CFE_PLATFORM_TIME_CFG_SOURCE == TRUE)
  #if (CFE_PLATFORM_TIME_CFG_VIRTUAL != TRUE)
    #error Cannot define CFE_PLATFORM_TIME_CFG_SOURCE as TRUE without defining CFE_PLATFORM_TIME_CFG_VIRTUAL as TRUE!
  #endif
#endif

/*
** Validate local MET selections...
*/
#if (CFE_PLATFORM_TIME_CFG_CLIENT == TRUE)
  #if (CFE_PLATFORM_TIME_CFG_VIRTUAL != TRUE)
    #error Cannot define CFE_PLATFORM_TIME_CFG_CLIENT as TRUE without defining CFE_PLATFORM_TIME_CFG_VIRTUAL as TRUE!
  #endif
#endif

/*
** Validate time source type selection...
*/
#if (CFE_PLATFORM_TIME_CFG_SRC_MET == TRUE)
  #if (CFE_PLATFORM_TIME_CFG_SOURCE != TRUE)
    #error Cannot define CFE_PLATFORM_TIME_CFG_SRC_MET as TRUE without defining CFE_PLATFORM_TIME_CFG_SOURCE as TRUE!
  #endif
  #if (CFE_PLATFORM_TIME_CFG_SRC_GPS == TRUE)
    #error Cannot define both CFE_PLATFORM_TIME_CFG_SRC_MET and CFE_PLATFORM_TIME_CFG_SRC_GPS as TRUE!
  #endif
  #if (CFE_PLATFORM_TIME_CFG_SRC_TIME == TRUE)
    #error Cannot define both CFE_PLATFORM_TIME_CFG_SRC_MET and CFE_PLATFORM_TIME_CFG_SRC_TIME as TRUE!
  #endif
#endif

#if (CFE_PLATFORM_TIME_CFG_SRC_GPS == TRUE)
  #if (CFE_PLATFORM_TIME_CFG_SOURCE != TRUE)
    #error Cannot define CFE_PLATFORM_TIME_CFG_SRC_GPS as TRUE without defining CFE_PLATFORM_TIME_CFG_SOURCE as TRUE!
  #endif
  #if (CFE_PLATFORM_TIME_CFG_SRC_TIME == TRUE)
    #error Cannot define both CFE_PLATFORM_TIME_CFG_SRC_GPS and CFE_PLATFORM_TIME_CFG_SRC_TIME as TRUE!
  #endif
#endif

#if (CFE_PLATFORM_TIME_CFG_SRC_TIME == TRUE)
  #if (CFE_PLATFORM_TIME_CFG_SOURCE != TRUE)
    #error Cannot define CFE_PLATFORM_TIME_CFG_SRC_TIME as TRUE without defining CFE_PLATFORM_TIME_CFG_SOURCE as TRUE!
  #endif
#endif


/*
** Validate tone signal and data packet arrival selection...
*/
#if (CFE_MISSION_TIME_AT_TONE_WAS == TRUE)
  #if (CFE_MISSION_TIME_AT_TONE_WILL_BE == TRUE)
    #error Both CFE_MISSION_TIME_AT_TONE_WAS and CFE_MISSION_TIME_AT_TONE_WILL_BE have been defined as TRUE!
  #endif
#else
  #if (CFE_MISSION_TIME_AT_TONE_WILL_BE != TRUE)
    #error Either CFE_MISSION_TIME_AT_TONE_WAS or CFE_MISSION_TIME_AT_TONE_WILL_BE must be defined as TRUE!
  #endif
#endif

/*
** Validate simulated tone signal and external time source selection...
*/
#if (CFE_MISSION_TIME_CFG_FAKE_TONE == TRUE)
  #if (CFE_PLATFORM_TIME_CFG_SOURCE == TRUE)
    #error Cannot define both CFE_MISSION_TIME_CFG_FAKE_TONE and CFE_PLATFORM_TIME_CFG_SOURCE as TRUE!
  #endif
#endif

/*
** Validate simulated tone signal and data packet arrival selection...
*/
#if (CFE_MISSION_TIME_CFG_FAKE_TONE == TRUE)
  #if (CFE_MISSION_TIME_AT_TONE_WILL_BE == TRUE)
    #error Cannot define both CFE_MISSION_TIME_CFG_FAKE_TONE and CFE_MISSION_TIME_AT_TONE_WILL_BE as TRUE!
  #endif
#endif

/*
** Validate task priorities...
*/
#if CFE_PLATFORM_TIME_START_TASK_PRIORITY < 0    
    #error CFE_PLATFORM_TIME_START_TASK_PRIORITY must be greater than or equal to zero
#elif CFE_PLATFORM_TIME_START_TASK_PRIORITY > 255    
    #error CFE_PLATFORM_TIME_START_TASK_PRIORITY must be less than or equal to 255
#endif    
#if CFE_PLATFORM_TIME_TONE_TASK_PRIORITY < 0    
    #error CFE_PLATFORM_TIME_TONE_TASK_PRIORITY must be greater than or equal to zero
#elif CFE_PLATFORM_TIME_TONE_TASK_PRIORITY > 255    
    #error CFE_PLATFORM_TIME_TONE_TASK_PRIORITY must be less than or equal to 255
#endif    
#if CFE_PLATFORM_TIME_1HZ_TASK_PRIORITY < 0    
    #error CFE_PLATFORM_TIME_1HZ_TASK_PRIORITY must be greater than or equal to zero
#elif CFE_PLATFORM_TIME_1HZ_TASK_PRIORITY > 255    
    #error CFE_PLATFORM_TIME_1HZ_TASK_PRIORITY must be less than or equal to 255
#endif    

/*
** Validate task stack sizes...
*/
#if CFE_PLATFORM_TIME_START_TASK_STACK_SIZE < 2048    
    #error CFE_PLATFORM_TIME_START_TASK_STACK_SIZE must be greater than or equal to 2048
#endif

#if CFE_PLATFORM_TIME_TONE_TASK_STACK_SIZE < 2048
    #error CFE_PLATFORM_TIME_TONE_TASK_STACK_SIZE must be greater than or equal to 2048
#endif

#if CFE_PLATFORM_TIME_1HZ_TASK_STACK_SIZE < 2048
    #error CFE_PLATFORM_TIME_1HZ_TASK_STACK_SIZE must be greater than or equal to 2048
#endif    

/*************************************************************************/

#endif /* _cfe_time_verify_ */

/************************/
/*  End of File Comment */
/************************/
