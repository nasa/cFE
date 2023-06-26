/************************************************************************
 * NASA Docket No. GSC-18,719-1, and identified as “core Flight System: Bootes”
 *
 * Copyright (c) 2020 United States Government as represented by the
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

/**
 * @file
 *
 * Auto-Generated stub implementations for functions defined in cfe_time header
 */

#include "cfe_time.h"
#include "utgenstub.h"

void UT_DefaultHandler_CFE_TIME_Add(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_TIME_GetMET(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_TIME_GetSTCF(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_TIME_GetTAI(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_TIME_GetTime(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_TIME_GetUTC(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_TIME_MET2SCTime(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_TIME_Print(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_TIME_Subtract(void *, UT_EntryKey_t, const UT_StubContext_t *);

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_TIME_Add()
 * ----------------------------------------------------
 */
CFE_TIME_SysTime_t CFE_TIME_Add(CFE_TIME_SysTime_t Time1, CFE_TIME_SysTime_t Time2)
{
    UT_GenStub_SetupReturnBuffer(CFE_TIME_Add, CFE_TIME_SysTime_t);

    UT_GenStub_AddParam(CFE_TIME_Add, CFE_TIME_SysTime_t, Time1);
    UT_GenStub_AddParam(CFE_TIME_Add, CFE_TIME_SysTime_t, Time2);

    UT_GenStub_Execute(CFE_TIME_Add, Basic, UT_DefaultHandler_CFE_TIME_Add);

    return UT_GenStub_GetReturnValue(CFE_TIME_Add, CFE_TIME_SysTime_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_TIME_Compare()
 * ----------------------------------------------------
 */
CFE_TIME_Compare_t CFE_TIME_Compare(CFE_TIME_SysTime_t TimeA, CFE_TIME_SysTime_t TimeB)
{
    UT_GenStub_SetupReturnBuffer(CFE_TIME_Compare, CFE_TIME_Compare_t);

    UT_GenStub_AddParam(CFE_TIME_Compare, CFE_TIME_SysTime_t, TimeA);
    UT_GenStub_AddParam(CFE_TIME_Compare, CFE_TIME_SysTime_t, TimeB);

    UT_GenStub_Execute(CFE_TIME_Compare, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_TIME_Compare, CFE_TIME_Compare_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_TIME_ExternalGPS()
 * ----------------------------------------------------
 */
void CFE_TIME_ExternalGPS(CFE_TIME_SysTime_t NewTime, int16 NewLeaps)
{
    UT_GenStub_AddParam(CFE_TIME_ExternalGPS, CFE_TIME_SysTime_t, NewTime);
    UT_GenStub_AddParam(CFE_TIME_ExternalGPS, int16, NewLeaps);

    UT_GenStub_Execute(CFE_TIME_ExternalGPS, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_TIME_ExternalMET()
 * ----------------------------------------------------
 */
void CFE_TIME_ExternalMET(CFE_TIME_SysTime_t NewMET)
{
    UT_GenStub_AddParam(CFE_TIME_ExternalMET, CFE_TIME_SysTime_t, NewMET);

    UT_GenStub_Execute(CFE_TIME_ExternalMET, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_TIME_ExternalTime()
 * ----------------------------------------------------
 */
void CFE_TIME_ExternalTime(CFE_TIME_SysTime_t NewTime)
{
    UT_GenStub_AddParam(CFE_TIME_ExternalTime, CFE_TIME_SysTime_t, NewTime);

    UT_GenStub_Execute(CFE_TIME_ExternalTime, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_TIME_ExternalTone()
 * ----------------------------------------------------
 */
void CFE_TIME_ExternalTone(void)
{

    UT_GenStub_Execute(CFE_TIME_ExternalTone, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_TIME_GetClockInfo()
 * ----------------------------------------------------
 */
uint16 CFE_TIME_GetClockInfo(void)
{
    UT_GenStub_SetupReturnBuffer(CFE_TIME_GetClockInfo, uint16);

    UT_GenStub_Execute(CFE_TIME_GetClockInfo, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_TIME_GetClockInfo, uint16);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_TIME_GetClockState()
 * ----------------------------------------------------
 */
CFE_TIME_ClockState_Enum_t CFE_TIME_GetClockState(void)
{
    UT_GenStub_SetupReturnBuffer(CFE_TIME_GetClockState, CFE_TIME_ClockState_Enum_t);

    UT_GenStub_Execute(CFE_TIME_GetClockState, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_TIME_GetClockState, CFE_TIME_ClockState_Enum_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_TIME_GetLeapSeconds()
 * ----------------------------------------------------
 */
int16 CFE_TIME_GetLeapSeconds(void)
{
    UT_GenStub_SetupReturnBuffer(CFE_TIME_GetLeapSeconds, int16);

    UT_GenStub_Execute(CFE_TIME_GetLeapSeconds, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_TIME_GetLeapSeconds, int16);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_TIME_GetMET()
 * ----------------------------------------------------
 */
CFE_TIME_SysTime_t CFE_TIME_GetMET(void)
{
    UT_GenStub_SetupReturnBuffer(CFE_TIME_GetMET, CFE_TIME_SysTime_t);

    UT_GenStub_Execute(CFE_TIME_GetMET, Basic, UT_DefaultHandler_CFE_TIME_GetMET);

    return UT_GenStub_GetReturnValue(CFE_TIME_GetMET, CFE_TIME_SysTime_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_TIME_GetMETseconds()
 * ----------------------------------------------------
 */
uint32 CFE_TIME_GetMETseconds(void)
{
    UT_GenStub_SetupReturnBuffer(CFE_TIME_GetMETseconds, uint32);

    UT_GenStub_Execute(CFE_TIME_GetMETseconds, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_TIME_GetMETseconds, uint32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_TIME_GetMETsubsecs()
 * ----------------------------------------------------
 */
uint32 CFE_TIME_GetMETsubsecs(void)
{
    UT_GenStub_SetupReturnBuffer(CFE_TIME_GetMETsubsecs, uint32);

    UT_GenStub_Execute(CFE_TIME_GetMETsubsecs, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_TIME_GetMETsubsecs, uint32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_TIME_GetSTCF()
 * ----------------------------------------------------
 */
CFE_TIME_SysTime_t CFE_TIME_GetSTCF(void)
{
    UT_GenStub_SetupReturnBuffer(CFE_TIME_GetSTCF, CFE_TIME_SysTime_t);

    UT_GenStub_Execute(CFE_TIME_GetSTCF, Basic, UT_DefaultHandler_CFE_TIME_GetSTCF);

    return UT_GenStub_GetReturnValue(CFE_TIME_GetSTCF, CFE_TIME_SysTime_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_TIME_GetTAI()
 * ----------------------------------------------------
 */
CFE_TIME_SysTime_t CFE_TIME_GetTAI(void)
{
    UT_GenStub_SetupReturnBuffer(CFE_TIME_GetTAI, CFE_TIME_SysTime_t);

    UT_GenStub_Execute(CFE_TIME_GetTAI, Basic, UT_DefaultHandler_CFE_TIME_GetTAI);

    return UT_GenStub_GetReturnValue(CFE_TIME_GetTAI, CFE_TIME_SysTime_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_TIME_GetTime()
 * ----------------------------------------------------
 */
CFE_TIME_SysTime_t CFE_TIME_GetTime(void)
{
    UT_GenStub_SetupReturnBuffer(CFE_TIME_GetTime, CFE_TIME_SysTime_t);

    UT_GenStub_Execute(CFE_TIME_GetTime, Basic, UT_DefaultHandler_CFE_TIME_GetTime);

    return UT_GenStub_GetReturnValue(CFE_TIME_GetTime, CFE_TIME_SysTime_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_TIME_GetUTC()
 * ----------------------------------------------------
 */
CFE_TIME_SysTime_t CFE_TIME_GetUTC(void)
{
    UT_GenStub_SetupReturnBuffer(CFE_TIME_GetUTC, CFE_TIME_SysTime_t);

    UT_GenStub_Execute(CFE_TIME_GetUTC, Basic, UT_DefaultHandler_CFE_TIME_GetUTC);

    return UT_GenStub_GetReturnValue(CFE_TIME_GetUTC, CFE_TIME_SysTime_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_TIME_Local1HzISR()
 * ----------------------------------------------------
 */
void CFE_TIME_Local1HzISR(void)
{

    UT_GenStub_Execute(CFE_TIME_Local1HzISR, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_TIME_MET2SCTime()
 * ----------------------------------------------------
 */
CFE_TIME_SysTime_t CFE_TIME_MET2SCTime(CFE_TIME_SysTime_t METTime)
{
    UT_GenStub_SetupReturnBuffer(CFE_TIME_MET2SCTime, CFE_TIME_SysTime_t);

    UT_GenStub_AddParam(CFE_TIME_MET2SCTime, CFE_TIME_SysTime_t, METTime);

    UT_GenStub_Execute(CFE_TIME_MET2SCTime, Basic, UT_DefaultHandler_CFE_TIME_MET2SCTime);

    return UT_GenStub_GetReturnValue(CFE_TIME_MET2SCTime, CFE_TIME_SysTime_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_TIME_Micro2SubSecs()
 * ----------------------------------------------------
 */
uint32 CFE_TIME_Micro2SubSecs(uint32 MicroSeconds)
{
    UT_GenStub_SetupReturnBuffer(CFE_TIME_Micro2SubSecs, uint32);

    UT_GenStub_AddParam(CFE_TIME_Micro2SubSecs, uint32, MicroSeconds);

    UT_GenStub_Execute(CFE_TIME_Micro2SubSecs, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_TIME_Micro2SubSecs, uint32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_TIME_Print()
 * ----------------------------------------------------
 */
void CFE_TIME_Print(char *PrintBuffer, CFE_TIME_SysTime_t TimeToPrint)
{
    UT_GenStub_AddParam(CFE_TIME_Print, char *, PrintBuffer);
    UT_GenStub_AddParam(CFE_TIME_Print, CFE_TIME_SysTime_t, TimeToPrint);

    UT_GenStub_Execute(CFE_TIME_Print, Basic, UT_DefaultHandler_CFE_TIME_Print);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_TIME_RegisterSynchCallback()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_TIME_RegisterSynchCallback(CFE_TIME_SynchCallbackPtr_t CallbackFuncPtr)
{
    UT_GenStub_SetupReturnBuffer(CFE_TIME_RegisterSynchCallback, CFE_Status_t);

    UT_GenStub_AddParam(CFE_TIME_RegisterSynchCallback, CFE_TIME_SynchCallbackPtr_t, CallbackFuncPtr);

    UT_GenStub_Execute(CFE_TIME_RegisterSynchCallback, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_TIME_RegisterSynchCallback, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_TIME_Sub2MicroSecs()
 * ----------------------------------------------------
 */
uint32 CFE_TIME_Sub2MicroSecs(uint32 SubSeconds)
{
    UT_GenStub_SetupReturnBuffer(CFE_TIME_Sub2MicroSecs, uint32);

    UT_GenStub_AddParam(CFE_TIME_Sub2MicroSecs, uint32, SubSeconds);

    UT_GenStub_Execute(CFE_TIME_Sub2MicroSecs, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_TIME_Sub2MicroSecs, uint32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_TIME_Subtract()
 * ----------------------------------------------------
 */
CFE_TIME_SysTime_t CFE_TIME_Subtract(CFE_TIME_SysTime_t Time1, CFE_TIME_SysTime_t Time2)
{
    UT_GenStub_SetupReturnBuffer(CFE_TIME_Subtract, CFE_TIME_SysTime_t);

    UT_GenStub_AddParam(CFE_TIME_Subtract, CFE_TIME_SysTime_t, Time1);
    UT_GenStub_AddParam(CFE_TIME_Subtract, CFE_TIME_SysTime_t, Time2);

    UT_GenStub_Execute(CFE_TIME_Subtract, Basic, UT_DefaultHandler_CFE_TIME_Subtract);

    return UT_GenStub_GetReturnValue(CFE_TIME_Subtract, CFE_TIME_SysTime_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_TIME_UnregisterSynchCallback()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_TIME_UnregisterSynchCallback(CFE_TIME_SynchCallbackPtr_t CallbackFuncPtr)
{
    UT_GenStub_SetupReturnBuffer(CFE_TIME_UnregisterSynchCallback, CFE_Status_t);

    UT_GenStub_AddParam(CFE_TIME_UnregisterSynchCallback, CFE_TIME_SynchCallbackPtr_t, CallbackFuncPtr);

    UT_GenStub_Execute(CFE_TIME_UnregisterSynchCallback, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_TIME_UnregisterSynchCallback, CFE_Status_t);
}
