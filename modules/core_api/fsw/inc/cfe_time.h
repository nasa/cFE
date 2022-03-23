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
 * Purpose:  cFE Time Services (TIME) library API header file
 *
 * Author:   S.Walling/Microtel
 *
 * Notes:
 *
 */

#ifndef CFE_TIME_H
#define CFE_TIME_H

/*
** Includes
*/
#include "common_types.h"
#include "cfe_error.h"
#include "cfe_time_api_typedefs.h"
#include "cfe_es_api_typedefs.h"

/**
** \brief Time Copy
**
** Macro to copy systime into another systime.
** Preferred to use this macro as it does not require the two arguments to be exactly the same type,
** it will work with any two structures that define "Seconds" and "Subseconds" members.
*/
#define CFE_TIME_Copy(m, t)                \
    {                                      \
        (m)->Seconds    = (t)->Seconds;    \
        (m)->Subseconds = (t)->Subseconds; \
    }

/*****************************************************************************/
/*
** Exported Functions
*/

/** @defgroup CFEAPITIMEGetCurrent cFE Get Current Time APIs
 * @{
 */

/*****************************************************************************/
/**
** \brief Get the current spacecraft time
**
** \par Description
**        This routine returns the current spacecraft time, which is the amount of
**        time elapsed since the epoch as set in mission configuration.  The time returned
**        is either TAI (no leap seconds) or UTC (including leap seconds).  This choice
**        is made in the mission configuration file by defining either #CFE_MISSION_TIME_CFG_DEFAULT_TAI
**        or #CFE_MISSION_TIME_CFG_DEFAULT_UTC as true at compile time.  To maintain re-usability
**        across missions, most applications should be using this function
**        rather than the specific routines for getting UTC/TAI directly.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \return The current spacecraft time in default format
**
** \sa #CFE_TIME_GetTAI, #CFE_TIME_GetUTC, #CFE_TIME_GetMET,
**     #CFE_TIME_GetMETseconds, #CFE_TIME_GetMETsubsecs
**
******************************************************************************/
CFE_TIME_SysTime_t CFE_TIME_GetTime(void);

/*****************************************************************************/
/**
** \brief Get the current TAI (MET + SCTF) time
**
** \par Description
**        This routine returns the current TAI time to the caller.  TAI is an
**        international time standard that does not include leap seconds.
**        This routine should only be used in situations where TAI is absolutely
**        required.  Applications that call #CFE_TIME_GetTAI may not be portable
**        to all missions.  Maintenance of correct TAI in flight is not guaranteed
**        under all mission operations scenarios.  To maintain re-usability across
**        missions, most applications should be using #CFE_TIME_GetTime, rather
**        than the specific routines for getting UTC/TAI directly.
**
** \par Assumptions, External Events, and Notes:
**          -# The "TAI" time returned is referenced to the mission-defined time epoch,
**             which may or may not be the same as the standard TAI epoch.
**          -# Even though TAI does not include leap seconds, the time returned by this
**             function can still jump forward or backward without warning when the
**             spacecraft clock is set or adjusted by operators.  Applications using
**             this function must be able to handle these time discontinuities gracefully.
**
** \return The current spacecraft time in TAI
**
** \sa #CFE_TIME_GetTime, #CFE_TIME_GetUTC, #CFE_TIME_GetMET,
**     #CFE_TIME_GetMETseconds, #CFE_TIME_GetMETsubsecs
**
******************************************************************************/
CFE_TIME_SysTime_t CFE_TIME_GetTAI(void);

/*****************************************************************************/
/**
** \brief Get the current UTC (MET + SCTF - Leap Seconds) time
**
** \par Description
**        This routine returns the current UTC time to the caller.  This routine
**        should only be used in situations where UTC is absolutely required.
**        Applications that call #CFE_TIME_GetUTC may not be portable to all
**        missions.  Maintenance of correct UTC in flight is not guaranteed under
**        all mission operations scenarios.  If UTC is maintained in flight, it will
**        jump backwards occasionally due to leap second adjustments.  To maintain
**        re-usability across missions, most applications should be using
**        #CFE_TIME_GetTime, rather than the specific routines for getting
**        UTC/TAI directly.
**
** \par Assumptions, External Events, and Notes:
**          Note: The "UTC" time returned is referenced to the mission-defined time epoch,
**                which may or may not be the same as the standard UTC epoch.
**
** \return The current spacecraft time in UTC
**
** \sa #CFE_TIME_GetTime, #CFE_TIME_GetTAI, #CFE_TIME_GetMET,
**     #CFE_TIME_GetMETseconds, #CFE_TIME_GetMETsubsecs
**
******************************************************************************/
CFE_TIME_SysTime_t CFE_TIME_GetUTC(void);

/*****************************************************************************/
/**
** \brief Get the current value of the Mission Elapsed Time (MET).
**
** \par Description
**        This routine returns the current mission-elapsed time (MET).  MET is
**        usually derived from a hardware-based clock that is not adjusted
**        during normal operations.  Callers of this routine should not assume
**        that the MET return value has any specific relationship to any
**        ground-based time standard.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \return The current MET
**
** \sa #CFE_TIME_GetTime, #CFE_TIME_GetTAI, #CFE_TIME_GetUTC,
**     #CFE_TIME_GetMETseconds, #CFE_TIME_GetMETsubsecs, #CFE_TIME_MET2SCTime
**
******************************************************************************/
CFE_TIME_SysTime_t CFE_TIME_GetMET(void);

/*****************************************************************************/
/**
** \brief Get the current seconds count of the mission-elapsed time.
**
** \par Description
**        This routine is the same as #CFE_TIME_GetMET, except that it
**        returns only the integer seconds portion of the MET time.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \return The current MET seconds
**
** \sa #CFE_TIME_GetTime, #CFE_TIME_GetTAI, #CFE_TIME_GetUTC, #CFE_TIME_GetMET,
**     #CFE_TIME_GetMETsubsecs, #CFE_TIME_MET2SCTime
**
******************************************************************************/
uint32 CFE_TIME_GetMETseconds(void);

/*****************************************************************************/
/**
** \brief Get the current sub-seconds count of the mission-elapsed time.
**
** \par Description
**        This routine is the same as #CFE_TIME_GetMET, except that it
**        returns only the integer sub-seconds portion of the MET time.
**        Each count is equal to 2^(-32) seconds.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \return The current MET sub-seconds
**
** \sa #CFE_TIME_GetTime, #CFE_TIME_GetTAI, #CFE_TIME_GetUTC, #CFE_TIME_GetMET,
**     #CFE_TIME_GetMETseconds, #CFE_TIME_MET2SCTime
**
******************************************************************************/
uint32 CFE_TIME_GetMETsubsecs(void);
/**@}*/

/** @defgroup CFEAPITIMEGetInfo cFE Get Time Information APIs
 * @{
 */

/*****************************************************************************/
/**
** \brief Get the current value of the spacecraft time correction factor (STCF).
**
** \par Description
**        This routine returns the current value of the spacecraft time correction
**        factor.  This is the delta time between the MET and the TAI time.
**        There is no API provided to set or adjust leap seconds or SCTF, those
**        actions should be done by command only.  This API is provided for
**        applications to be able to include STCF in their data products to
**        aid in time correlation during downstream science data processing.
**
** \par Assumptions, External Events, and Notes:
**        Does not include leap seconds
**
** \return The current SCTF
**
** \sa #CFE_TIME_GetLeapSeconds, #CFE_TIME_GetClockState, #CFE_TIME_GetClockInfo
**
******************************************************************************/
CFE_TIME_SysTime_t CFE_TIME_GetSTCF(void);

/*****************************************************************************/
/**
** \brief Get the current value of the leap seconds counter.
**
** \par Description
**        This routine returns the current value of the leap seconds counter.
**        This is the delta seconds between international atomic time (TAI)
**        and universal coordinated time (UTC).
**        There is no API provided to set or adjust leap seconds or SCTF, those
**        actions should be done by command only.  This API is provided for
**        applications to be able to include leap seconds in their data products to
**        aid in time correlation during downstream science data processing.
**        Note that some mission operations
**        teams do not maintain the leap seconds count, preferring to adjust the
**        STCF instead.  Users of this function should check with their mission
**        ops team to see how they are planning to handle leap seconds.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \returns The current spacecraft leap seconds.
**
** \sa #CFE_TIME_GetSTCF, #CFE_TIME_GetClockState, #CFE_TIME_GetClockInfo
**
******************************************************************************/
int16 CFE_TIME_GetLeapSeconds(void);

/*****************************************************************************/
/**
** \brief Get the current state of the spacecraft clock.
**
** \par Description
**        This routine returns the spacecraft clock state.  Applications that
**        are highly dependent on valid time may want to call this routine
**        before taking actions based on the times returned by the various
**        clock routines
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \return The current spacecraft clock state
**
** \sa #CFE_TIME_GetSTCF, #CFE_TIME_GetLeapSeconds, #CFE_TIME_GetClockInfo
**
******************************************************************************/
CFE_TIME_ClockState_Enum_t CFE_TIME_GetClockState(void);

/*****************************************************************************/
/**
** \brief Provides information about the spacecraft clock.
**
** \par Description
**        This routine returns information on the spacecraft clock in a bit mask.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \return Spacecraft clock information, \ref CFETIMEClkStates.
**         To extract the information from the
**         returned value, the flags can be used as in the following: <br>
**         <tt> if ((ReturnValue & CFE_TIME_FLAG_xxxxxx) == CFE_TIME_FLAG_xxxxxx)</tt> then
**            the following definition of the \c CFE_TIME_FLAG_xxxxxx is true. <br>
**
** \sa #CFE_TIME_GetSTCF, #CFE_TIME_GetLeapSeconds, #CFE_TIME_GetClockState
**
******************************************************************************/
uint16 CFE_TIME_GetClockInfo(void);
/**@}*/

/** @defgroup CFEAPITIMEArithmetic cFE Time Arithmetic APIs
 * @{
 */

/*****************************************************************************/
/**
** \brief Adds two time values
**
** \par Description
**        This routine adds the two specified times and returns the result.
**        Normally, at least one of the input times should be a value representing
**        a delta time.  Adding two absolute times together will not cause an error,
**        but the result will probably be meaningless.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in] Time1   The first time to be added.
**
** \param[in] Time2   The second time to be added.
**
** \return The sum of the two times.
**         If the sum is greater than the maximum value that can be stored in a
**         #CFE_TIME_SysTime_t, the result will roll over (this is not considered an error).
**
** \sa #CFE_TIME_Subtract, #CFE_TIME_Compare
**
******************************************************************************/
CFE_TIME_SysTime_t CFE_TIME_Add(CFE_TIME_SysTime_t Time1, CFE_TIME_SysTime_t Time2);

/*****************************************************************************/
/**
** \brief Subtracts two time values
**
** \par Description
**        This routine subtracts time2 from time1 and returns the result.  The
**        time values can represent either absolute or delta times, but not all
**        combinations make sense.
**           -  AbsTime - AbsTime = DeltaTime
**           -  AbsTime - DeltaTime = AbsTime
**           -  DeltaTime - DeltaTime = DeltaTime
**           -  DeltaTime - AbsTime = garbage
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in] Time1   The base time.
**
** \param[in] Time2   The time to be subtracted from the base time.
**
** \return The result of subtracting the two times.
**         If the subtraction results in an underflow, the result will
**         roll over (this is not considered an error).
**
** \sa #CFE_TIME_Add, #CFE_TIME_Compare
**
******************************************************************************/
CFE_TIME_SysTime_t CFE_TIME_Subtract(CFE_TIME_SysTime_t Time1, CFE_TIME_SysTime_t Time2);

/*****************************************************************************/
/**
** \brief Compares two time values
**
** \par Description
**        This routine compares two time values to see which is "greater".  It
**        is important that applications use this function rather than trying
**        to directly compare the component pieces of times.  This function will
**        handle roll-over cases seamlessly, which may not be intuitively obvious.
**        The cFE's internal representation of time "rolls over" when the 32 bit
**        seconds count reaches 0xFFFFFFFF.  Also, subtracting a delta time from
**        an absolute time close to the epoch could result in "roll under".  The
**        strange cases that result from these situations can be handled by defining
**        the comparison function for times as follows:
**        Plot the two times on the circumference of a circle where 0 is at the
**        top and 0x80000000 is at the bottom.  If the shortest arc from time A
**        to time B runs clockwise around the circle, then time A is less than
**        time B.  If the shortest arc from A to B runs counter-clockwise, then
**        time A is greater than time B.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in] TimeA   The first time to compare.
**
** \param[in] TimeB   The second time to compare.
**
** \return The result of comparing the two times.
** \retval #CFE_TIME_EQUAL  \copybrief CFE_TIME_EQUAL
** \retval #CFE_TIME_A_GT_B \copybrief CFE_TIME_A_GT_B
** \retval #CFE_TIME_A_LT_B \copybrief CFE_TIME_A_LT_B
**
** \sa #CFE_TIME_Add, #CFE_TIME_Subtract
**
******************************************************************************/
CFE_TIME_Compare_t CFE_TIME_Compare(CFE_TIME_SysTime_t TimeA, CFE_TIME_SysTime_t TimeB);
/**@}*/

/** @defgroup CFEAPITIMEConvert cFE Time Conversion APIs
 * @{
 */

/*****************************************************************************/
/**
** \brief Convert specified MET into Spacecraft Time
**
** \par Description
**        This function returns Spacecraft Time given MET.  Note that Spacecraft
**        Time is returned as either UTC or TAI depending on whether the mission
**        configuration parameter #CFE_MISSION_TIME_CFG_DEFAULT_UTC or #CFE_MISSION_TIME_CFG_DEFAULT_TAI
**        was set to true at compile time.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in] METTime      The MET to be converted.
**
** \return Spacecraft Time (UTC or TAI) corresponding to the specified MET
**
** \sa #CFE_TIME_GetMET, #CFE_TIME_GetMETseconds, #CFE_TIME_GetMETsubsecs,
**     #CFE_TIME_Sub2MicroSecs, #CFE_TIME_Micro2SubSecs
**
******************************************************************************/
CFE_TIME_SysTime_t CFE_TIME_MET2SCTime(CFE_TIME_SysTime_t METTime);

/*****************************************************************************/
/**
** \brief Converts a sub-seconds count to an equivalent number of microseconds
**
** \par Description
**        This routine converts from a sub-seconds count
**        (each tick is 1 / 2^32 seconds) to microseconds (each tick is 1e-06 seconds).
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in] SubSeconds   The sub-seconds count to convert.
**
** \return The equivalent number of microseconds.
**
** \sa #CFE_TIME_MET2SCTime, #CFE_TIME_Micro2SubSecs,
**
******************************************************************************/
uint32 CFE_TIME_Sub2MicroSecs(uint32 SubSeconds);

/*****************************************************************************/
/**
** \brief Converts a number of microseconds to an equivalent sub-seconds count.
**
** \par Description
**        This routine converts from microseconds (each tick is 1e-06 seconds)
**        to a subseconds count (each tick is 1 / 2^32 seconds).
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in] MicroSeconds   The sub-seconds count to convert.
**
** \return The equivalent number of subseconds.  If the number of microseconds
**         passed in is greater than one second, (i.e. > 999,999), the return
**         value is equal to \c 0xffffffff.
**
** \sa #CFE_TIME_MET2SCTime, #CFE_TIME_Sub2MicroSecs,
**
******************************************************************************/
uint32 CFE_TIME_Micro2SubSecs(uint32 MicroSeconds);

/**@}*/

/** @defgroup CFEAPITIMEExternSource cFE External Time Source APIs
 * @{
 */

/*****************************************************************************/
/**
** \brief Provides the 1 Hz signal from an external source
**
** \par Description
**        This routine provides a method for cFE TIME software to be notified
**        of the occurrence of the 1Hz tone signal without knowledge of the
**        specific hardware design.  Regardless of the source of the tone,
**        this routine should be called as soon as possible after detection
**        to allow cFE TIME software the opportunity to latch the local clock
**        as close as possible to the instant of the tone.
**
** \par Assumptions, External Events, and Notes:
**          - This routine may be called directly from within the context of an
**            interrupt handler.
**
** \sa #CFE_TIME_ExternalMET, #CFE_TIME_ExternalGPS, #CFE_TIME_ExternalTime
**
******************************************************************************/
void CFE_TIME_ExternalTone(void);

/*
** Function prototypes (external time source)...
**
** If a Time Server has been configured to accept external time
**    data, then one of the following functions will be enabled.
**
** If the Time Server has also been commanded to use the external
**    time data (as opposed to getting time data from a local MET)
**    then the Time Server will use the external data in computing
**    time.
**
** However, regardless whether the external time data is accepted
**    by the Time Server, these functions also act as the signal to
**    create and distribute the "time at the tone" command packet.
*/

/*****************************************************************************/
/**
** \brief Provides the Mission Elapsed Time from an external source
**
** \par Description
**        This routine provides a method to provide cFE TIME with MET acquired
**        from an external source.  There is a presumption that this function
**        will be called at the appropriate time (relative to the tone) such
**        that this call may be used by cFE TIME as the signal to generate the
**        "time at the tone" data command.  The "time at the tone" data command
**        must arrive within the configuration parameter specified window for
**        tone signal and data packet verification. <br> <br>
**        The MET value at the tone "should" have zero subseconds.  Although the
**        interface accepts non-zero values for sub-seconds, it may be harmful
**        to other applications that expect zero subseconds at the moment of the
**        tone.  Any decision to use non-zero subseconds should be carefully considered.
**
** \par Assumptions, External Events, and Notes:
**          - This routine is included in the API only when 3 specific configuration
**            parameters are set to true.  The first is #CFE_PLATFORM_TIME_CFG_SERVER which defines
**            this instantiation of cFE TIME as a time server (not a client).  The
**            second required configuration parameter is #CFE_PLATFORM_TIME_CFG_SOURCE which
**            enables time source selection commands to the cFE TIME task, and further
**            enables configuration definitions for the selected type of external time
**            data.  The third configuration parameter required for this routine is
**            #CFE_PLATFORM_TIME_CFG_SRC_MET, which indicates that the external time data consists
**            of MET.
**
** \param[in]  NewMET   The MET value at the next (or previous) 1 Hz tone signal.
**
** \sa #CFE_TIME_ExternalTone, #CFE_TIME_ExternalGPS, #CFE_TIME_ExternalTime
**
******************************************************************************/
void CFE_TIME_ExternalMET(CFE_TIME_SysTime_t NewMET);

/*****************************************************************************/
/**
** \brief Provide the time from an external source that has data common to GPS receivers.
**
** \par Description
**        This routine provides a method to provide cFE TIME with current time
**        data acquired from an external source.  There is a presumption that
**        this function will be called at the appropriate time (relative to the
**        tone) such that this call may be used by cFE TIME as the signal to
**        generate the "time at the tone" data command.  The "time at the tone"
**        data command must arrive within the configuration parameter specified
**        window for tone signal and data packet verification. <br> <br>
**        Internally, cFE TIME will calculate a new STCF as the difference between
**        this new time value and the spacecraft MET value at the tone.  This allows
**        cFE TIME to always calculate time as the sum of MET and STCF.  The value
**        of STCF will change only as much as the drift factor between spacecraft
**        MET and the external time source.
**
** \par Assumptions, External Events, and Notes:
**          - This routine is included in the API only when 3 specific configuration
**            parameters are set to true.  The first is #CFE_PLATFORM_TIME_CFG_SERVER which defines this
**            instantiation of cFE TIME as a time server (not a client).  The second
**            required configuration parameter is #CFE_PLATFORM_TIME_CFG_SOURCE which enables
**            time source selection commands to the cFE TIME task, and further enables
**            configuration definitions for the selected type of external time data.
**            The third configuration parameter required for this routine is
**            #CFE_PLATFORM_TIME_CFG_SRC_GPS, which indicates that the external time data consists
**            of a time value relative to a known epoch, plus a leap seconds value.
**
** \param[in]  NewTime   The MET value at the next (or previous) 1 Hz tone signal.
**
** \param[in]  NewLeaps The Leap Seconds value used to calculate time as UTC.
**
** \sa #CFE_TIME_ExternalTone, #CFE_TIME_ExternalMET, #CFE_TIME_ExternalTime
**
******************************************************************************/
void CFE_TIME_ExternalGPS(CFE_TIME_SysTime_t NewTime, int16 NewLeaps);

/*****************************************************************************/
/**
** \brief Provide the time from an external source that measures time relative to a known epoch.
**
** \par Description
**        This routine provides a method to provide cFE TIME with current time
**        data acquired from an external source.  There is a presumption that
**        this function will be called at the appropriate time (relative to the
**        tone) such that this call may be used by cFE TIME as the signal to
**        generate the "time at the tone" data command.  The "time at the tone"
**        data command must arrive within the configuration specified window for
**        tone signal and data packet verification. <br> <br>
**        Internally, cFE TIME will calculate a new STCF as the difference between
**        this new time value and the spacecraft MET value at the tone.  This allows
**        cFE TIME to always calculate time as the sum of MET and STCF.  The value
**        of STCF will change only as much as the drift factor between spacecraft
**        MET and the external time source.
**
** \par Assumptions, External Events, and Notes:
**          - This routine is included in the API only when 3 specific configuration
**            parameters are set to true.  The first is #CFE_PLATFORM_TIME_CFG_SERVER which defines this
**            instantiation of cFE TIME as a time server (not a client).  The second
**            required configuration parameter is #CFE_PLATFORM_TIME_CFG_SOURCE which enables
**            time source selection commands to the cFE TIME task, and further enables
**            configuration definitions for the selected type of external time data.
**            The third configuration parameter required for this routine is
**            #CFE_PLATFORM_TIME_CFG_SRC_TIME, which indicates that the external time data consists
**            of a time value relative to a known epoch.
**
** \param[in]  NewTime   The MET value at the next (or previous) 1 Hz tone signal.
**
** \sa #CFE_TIME_ExternalTone, #CFE_TIME_ExternalMET, #CFE_TIME_ExternalGPS
**
******************************************************************************/
void CFE_TIME_ExternalTime(CFE_TIME_SysTime_t NewTime);

/*****************************************************************************/
/**
** \brief Registers a callback function that is called whenever time synchronization occurs
**
** \par Description
**        This routine passes a callback function pointer for an Application that wishes to
**        be notified whenever a legitimate time synchronization signal (typically a 1 Hz)
**        is received.
**
** \par Assumptions, External Events, and Notes:
**        Only a single callback per application is supported, and this function should only
**        be called from a single thread within each application (typically the apps main thread).
**        If an application requires triggering multiple child tasks at 1Hz, it should distribute
**        the timing signal internally, rather than registering for multiple callbacks.
**
** \param[in]  CallbackFuncPtr   Function to call at synchronization interval @nonnull
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS                       \copybrief CFE_SUCCESS
** \retval #CFE_TIME_TOO_MANY_SYNCH_CALLBACKS \copybrief CFE_TIME_TOO_MANY_SYNCH_CALLBACKS
** \retval #CFE_TIME_BAD_ARGUMENT             \copybrief CFE_TIME_BAD_ARGUMENT
**
** \sa #CFE_TIME_UnregisterSynchCallback
**
******************************************************************************/
CFE_Status_t CFE_TIME_RegisterSynchCallback(CFE_TIME_SynchCallbackPtr_t CallbackFuncPtr);

/*****************************************************************************/
/**
** \brief Unregisters a callback function that is called whenever time synchronization occurs
**
** \par Description
**        This routine removes the specified callback function pointer from the list
**        of Callback functions that are called whenever a time synchronization (typically
**        the 1Hz signal) is received.
**
** \par Assumptions, External Events, and Notes:
**        Only a single callback per application is supported, and this function should only
**        be called from a single thread within each application (typically the apps main thread).
**
** \param[in]  CallbackFuncPtr   Function to remove from synchronization call list @nonnull
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS                      \copybrief CFE_SUCCESS
** \retval #CFE_TIME_CALLBACK_NOT_REGISTERED \copybrief CFE_TIME_CALLBACK_NOT_REGISTERED
** \retval #CFE_TIME_BAD_ARGUMENT            \copybrief CFE_TIME_BAD_ARGUMENT
**
** \sa #CFE_TIME_RegisterSynchCallback
**
******************************************************************************/
CFE_Status_t CFE_TIME_UnregisterSynchCallback(CFE_TIME_SynchCallbackPtr_t CallbackFuncPtr);
/**@}*/

/** @defgroup CFEAPITIMEMisc cFE Miscellaneous Time APIs
 * @{
 */

/*****************************************************************************/
/**
** \brief Print a time value as a string
**
** \par Description
**        This routine prints the specified time to the specified string buffer
**        in the following format: <br> <br>
**           \c yyyy-ddd-hh:mm:ss.xxxxx\\0 <br> <br>
**        where:
**           - \c yyyy = year
**           - \c ddd = Julian day of the year
**           - \c hh = hour of the day (0 to 23)
**           - \c mm = minute (0 to 59)
**           - \c ss = second (0 to 59)
**           - \c xxxxx = subsecond formatted as a decimal fraction (1/4 second = 0.25000)
**           - \c \\0 = trailing null
**
** \par Assumptions, External Events, and Notes:
**        - The value of the time argument is simply added to the configuration
**          definitions for the ground epoch and converted into a fixed length
**          string in the buffer provided by the caller.
**        - A loss of data during the string conversion will occur if the
**          computed year exceeds 9999.  However, a year that large would
**          require an unrealistic definition for the ground epoch since
**          the maximum amount of time represented by a CFE_TIME_SysTime
**          structure is approximately 136 years.
**
** \param[out]  PrintBuffer   Pointer to a character array @nonnull of at least
**                           #CFE_TIME_PRINTED_STRING_SIZE characters in length. *PrintBuffer is the time as a
**                           character string as described above.
**
** \param[in]  TimeToPrint   The time to print into the character array.
**
******************************************************************************/
void CFE_TIME_Print(char *PrintBuffer, CFE_TIME_SysTime_t TimeToPrint);

/*****************************************************************************/
/**
** \brief This function is called via a timer callback set up at initialization
**        of the TIME service.
**
** \par Description
**        Drives the time processing logic from the system PSP layer.  This must be called
**        once per second based on a hardware interrupt or OS kernel signal.
**
** \par Assumptions, External Events, and Notes:
**        This will update the global data structures accordingly, incrementing each
**        by the 1Hz amount.
**
******************************************************************************/
void CFE_TIME_Local1HzISR(void);
/**@}*/

#endif /* CFE_TIME_H */
