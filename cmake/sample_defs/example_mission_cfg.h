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
 * This header file contains the mission configuration parameters and
 * typedefs with mission scope.
 *
 * This provides values for configurable items that affect
 * the interface(s) of this module.  This includes the CMD/TLM message
 * interface, tables definitions, and any other data products that
 * serve to exchange information with other entities.
 *
 * @note It is no longer necessary to provide this file directly in the defs
 * directory, but if present, this file is still supported/usable for backward
 * compatibility.  To use this file, is should be called "cfe_mission_cfg.h".
 *
 * Going forward, more fine-grained (module/purposes-specific) header files are
 * included with each submodule.  These may be overridden as necessary, but only
 * if a definition within that file needs to be changed from the default.  This
 * approach will reduce the amount of duplicate/cloned definitions and better
 * support alternative build configurations in the future.
 *
 * Note that if this file is present, the fine-grained header files noted above
 * will _not_ be used.
 */

#ifndef EXAMPLE_MISSION_CFG_H
#define EXAMPLE_MISSION_CFG_H

/**
**  \cfemissioncfg cFE Maximum length for pathnames within data exchange structures
**
**  \par Description:
**       The value of this constant dictates the size of pathnames within all structures
**       used for external data exchange, such as Software bus messages and table definitions.
**       This is typically the same as OS_MAX_PATH_LEN but that is OSAL dependent --
**       and as such it definable on a per-processor/OS basis and hence may be different
**       across multiple processors.  It is recommended to set this to the value of the
**       largest OS_MAX_PATH_LEN in use on any CPU on the mission.
**
**       This affects only the layout of command/telemetry messages and table definitions;
**       internal allocation may use the platform-specific OS_MAX_PATH_LEN value.
**
**       This length must include an extra character for NULL termination.
**
**  \par Limits
**       All CPUs within the same SB domain (mission) and ground tools must share the
**       same definition.
**       Note this affects the size of messages, so it must not cause any message
**       to exceed the max length.
**
**       This value should be kept as a multiple of 4, to maintain alignment of
**       any possible neighboring fields without implicit padding.
*/
#define CFE_MISSION_MAX_PATH_LEN 64

/**
**  \cfemissioncfg cFE Maximum length for filenames within data exchange structures
**
**  \par Description:
**       The value of this constant dictates the size of filenames within all structures
**       used for external data exchange, such as Software bus messages and table definitions.
**       This is typically the same as OS_MAX_FILE_LEN but that is OSAL dependent --
**       and as such it definable on a per-processor/OS basis and hence may be different
**       across multiple processors.  It is recommended to set this to the value of the
**       largest OS_MAX_FILE_LEN in use on any CPU on the mission.
**
**       This affects only the layout of command/telemetry messages and table definitions;
**       internal allocation may use the platform-specific OS_MAX_FILE_LEN value.
**
**       This length must include an extra character for NULL termination.
**
**  \par Limits
**       All CPUs within the same SB domain (mission) and ground tools must share the
**       same definition.
**       Note this affects the size of messages, so it must not cause any message
**       to exceed the max length.
**
**       This value should be kept as a multiple of 4, to maintain alignment of
**       any possible neighboring fields without implicit padding.
*/
#define CFE_MISSION_MAX_FILE_LEN 20

/**
**  \cfemissioncfg cFE Maximum length for API names within data exchange structures
**
**  \par Description:
**       The value of this constant dictates the size of filenames within all structures
**       used for external data exchange, such as Software bus messages and table definitions.
**       This is typically the same as OS_MAX_API_LEN but that is OSAL dependent --
**       and as such it definable on a per-processor/OS basis and hence may be different
**       across multiple processors.  It is recommended to set this to the value of the
**       largest OS_MAX_API_LEN in use on any CPU on the mission.
**
**       This affects only the layout of command/telemetry messages and table definitions;
**       internal allocation may use the platform-specific OS_MAX_API_LEN value.
**
**       This length must include an extra character for NULL termination.
**
**  \par Limits
**       All CPUs within the same SB domain (mission) must share the same definition
**       Note this affects the size of messages, so it must not cause any message
**       to exceed the max length.
**
**       This value should be kept as a multiple of 4, to maintain alignment of
**       any possible neighboring fields without implicit padding.
*/
#define CFE_MISSION_MAX_API_LEN 20

/**
**  \cfemissioncfg cFE Maximum number of files in a message/data exchange
**
**  \par Description:
**       The value of this constant dictates the maximum number of files within all structures
**       used for external data exchange, such as Software bus messages and table definitions.
**       This is typically the same as OS_MAX_NUM_OPEN_FILES but that is OSAL dependent --
**       and as such it definable on a per-processor/OS basis and hence may be different
**       across multiple processors.  It is recommended to set this to the value of the
**       largest OS_MAX_NUM_OPEN_FILES in use on any CPU on the mission.
**
**       This affects only the layout of command/telemetry messages and table definitions;
**       internal allocation may use the platform-specific OS_MAX_NUM_OPEN_FILES value.
**
**  \par Limits
**       All CPUs within the same SB domain (mission) must share the same definition
**       Note this affects the size of messages, so it must not cause any message
**       to exceed the max length.
**
*/
#define CFE_MISSION_MAX_NUM_FILES 50

/******************************************************************************
 *   CFE Executive Services (CFE_ES) Application Public Definitions
 *
 * This provides default values for configurable items that affect
 * the interface(s) of this module.  This includes the CMD/TLM message
 * interface, tables definitions, and any other data products that
 * serve to exchange information with other entities.
 *
 */

/**
**  \cfeescfg Mission Max Apps in a message
**
**  \par Description:
**      Indicates the maximum number of apps in a telemetry housekeeping message
**
**      This affects the layout of command/telemetry messages but does not affect run
**      time behavior or internal allocation.
**
**  \par Limits
**       All CPUs within the same SB domain (mission) must share the same definition
**       Note this affects the size of messages, so it must not cause any message
**       to exceed the max length.
*/
#define CFE_MISSION_ES_MAX_APPLICATIONS 16

/**
**  \cfeescfg Define Max Number of Performance IDs for messages
**
**  \par Description:
**       Defines the maximum number of perf ids allowed in command/telemetry messages
**
**      This affects the layout of command/telemetry messages but does not affect run
**      time behavior or internal allocation.
**
**  \par Limits
**       All CPUs within the same SB domain (mission) must share the same definition
**       Note this affects the size of messages, so it must not cause any message
**       to exceed the max length.
**
*/
#define CFE_MISSION_ES_PERF_MAX_IDS 128

/** \cfeescfg Maximum number of block sizes in pool structures
**
**  \par Description:
**      The upper limit for the number of block sizes supported in the generic
**      pool implementation, which in turn implements the memory pools and CDS.
**      This definition is used as the array size with the pool stats structure,
**      and therefore should be consistent across all CPUs in a mission, as well
**      as with the ground station.
**
**      There is also a platform-specific limit which may be fewer than this
**      value.
**
**  \par Limits:
**       Must be at least one.  No specific upper limit, but the number is
**       anticipated to be reasonably small (i.e. tens, not hundreds).  Large
**       values have not been tested.
**
**
*/
#define CFE_MISSION_ES_POOL_MAX_BUCKETS 17

/**
**  \cfeescfg Maximum Length of CDS Name
**
**  \par Description:
**      Indicates the maximum length (in characters) of the CDS name ('CDSName')
**      portion of a Full CDS Name of the following form:
**      "ApplicationName.CDSName"
**
**      This length does not need to include an extra character for NULL termination.
**
**  \par Limits
**       This value should be kept as a multiple of 4, to maintain alignment of
**       any possible neighboring fields without implicit padding.
**
*/
#define CFE_MISSION_ES_CDS_MAX_NAME_LENGTH 16

/**
**  \cfeescfg Mission Default CRC algorithm
**
**  \par Description:
**      Indicates the which CRC algorithm should be used as the default
**      for verifying the contents of Critical Data Stores and when calculating
**      Table Image data integrity values.
**
**  \par Limits
**      Currently only CFE_ES_CrcType_CRC_16 is supported (see brief in CFE_ES_CrcType_Enum
**      definition in cfe_es_api_typedefs.h)
*/
#define CFE_MISSION_ES_DEFAULT_CRC CFE_ES_CrcType_CRC_16

/**
**  \cfeescfg Maximum Length of Full CDS Name in messages
**
**  \par Description:
**      Indicates the maximum length (in characters) of the entire CDS name
**      of the following form: "ApplicationName.CDSName"
**
**       This affects the layout of command/telemetry messages but does not affect run
**       time behavior or internal allocation.
**
**  \par Limits
**       All CPUs within the same SB domain (mission) must share the same definition
**       Note this affects the size of messages, so it must not cause any message
**       to exceed the max length.
**
**       This value should be kept as a multiple of 4, to maintain alignment of
**       any possible neighboring fields without implicit padding.
*/
#define CFE_MISSION_ES_CDS_MAX_FULL_NAME_LEN (CFE_MISSION_ES_CDS_MAX_NAME_LENGTH + CFE_MISSION_MAX_API_LEN + 4)

#ifndef CFE_OMIT_DEPRECATED_6_8
/* These names have been converted to an enum in cfe_es_api_typedefs.h */

/** \name Checksum/CRC algorithm identifiers */

#define CFE_MISSION_ES_CRC_8  CFE_ES_CrcType_CRC_8  /* 1 */
#define CFE_MISSION_ES_CRC_16 CFE_ES_CrcType_CRC_16 /* 2 */
#define CFE_MISSION_ES_CRC_32 CFE_ES_CrcType_CRC_32 /* 3 */

#endif

/******************************************************************************
 *   CFE Event Services (CFE_EVS) Application Public Definitions
 *
 * This provides default values for configurable items that affect
 * the interface(s) of this module.  This includes the CMD/TLM message
 * interface, tables definitions, and any other data products that
 * serve to exchange information with other entities.
 *
 */

/**
**  \cfeevscfg Maximum Event Message Length
**
**  \par Description:
**      Indicates the maximum length (in characters) of the formatted text
**      string portion of an event message
**
**      This length does not need to include an extra character for NULL termination.
**
**  \par Limits
**      Not Applicable
*/
#define CFE_MISSION_EVS_MAX_MESSAGE_LENGTH 122

/******************************************************************************
 *   CFE File Services (CFE_FS) Public Definitions
 *
 */

/*
 * NOTE: the value of CFE_FS_HDR_DESC_MAX_LEN, if modified, should
 * be constrained to multiples of 4, as it is used within a structure that
 * also contains uint32 types.  This ensures that the entire structure
 * remains 32-bit aligned without the need for implicit padding bytes.
 */

#define CFE_FS_HDR_DESC_MAX_LEN 32 /**< \brief Max length of description field in a standard cFE File Header */

#define CFE_FS_FILE_CONTENT_ID 0x63464531 /**< \brief Magic Number for cFE-compliant files (= 'cFE1') */


/******************************************************************************
 *   CFE Software Bus (CFE_SB) Application Public Definitions
 *
 * This provides default values for configurable items that affect
 * the interface(s) of this module.  This includes the CMD/TLM message
 * interface, tables definitions, and any other data products that
 * serve to exchange information with other entities.
 */

/**
**  \cfesbcfg Maximum SB Message Size
**
**  \par Description:
**      The following definition dictates the maximum message size allowed on
**      the software bus. SB checks the pkt length field in the header of all
**      messages sent. If the pkt length field indicates the message is larger
**      than this define, SB sends an event and rejects the send.
**
**  \par Limits
**      This parameter has a lower limit of 6 (CCSDS primary header size).  There
**      are no restrictions on the upper limit however, the maximum message size is
**      system dependent and should be verified.  Total message size values that are
**      checked against this configuration are defined by a 16 bit data word.
*/
#define CFE_MISSION_SB_MAX_SB_MSG_SIZE 32768

/**
**  \cfesbcfg Maximum Number of pipes that SB command/telemetry messages may hold
**
**  \par Description:
**       Dictates the maximum number of unique Pipes the SB message definitions will hold.
**
**       This affects the layout of command/telemetry messages but does not affect run
**       time behavior or internal allocation.
**
**  \par Limits
**       All CPUs within the same SB domain (mission) must share the same definition
**       Note this affects the size of messages, so it must not cause any message
**       to exceed the max length.
**
*/
#define CFE_MISSION_SB_MAX_PIPES 64

/******************************************************************************
 *   CFE Table Services (CFE_TBL) Application Public Definitions
 *
 * This provides default values for configurable items that affect
 * the interface(s) of this module.  This includes the CMD/TLM message
 * interface, tables definitions, and any other data products that
 * serve to exchange information with other entities.
 */

/**
**  \cfetblcfg Maximum Table Name Length
**
**  \par Description:
**      Indicates the maximum length (in characters) of the table name
**      ('TblName') portion of a Full Table Name of the following
**      form: "ApplicationName.TblName"
**
**      This length does not need to include an extra character for NULL termination.
**
**  \par Limits
**       This value should be kept as a multiple of 4, to maintain alignment of
**       any possible neighboring fields without implicit padding.
*/
#define CFE_MISSION_TBL_MAX_NAME_LENGTH 16

/**
**  \cfetblcfg Maximum Length of Full Table Name in messages
**
**  \par Description:
**      Indicates the maximum length (in characters) of the entire table name
**      within software bus messages, in "AppName.TableName" notation.
**
**      This affects the layout of command/telemetry messages but does not affect run
**      time behavior or internal allocation.
**
**  \par Limits
**       All CPUs within the same SB domain (mission) must share the same definition
**       Note this affects the size of messages, so it must not cause any message
**       to exceed the max length.
**
**       This value should be kept as a multiple of 4, to maintain alignment of
**       any possible neighboring fields without implicit padding.
*/
#define CFE_MISSION_TBL_MAX_FULL_NAME_LEN (CFE_MISSION_TBL_MAX_NAME_LENGTH + CFE_MISSION_MAX_API_LEN + 4)


/******************************************************************************
 *   CFE Time Services (CFE_TIME) Application Public Definitions
 *
 * This provides default values for configurable items that affect
 * the interface(s) of this module.  This includes the CMD/TLM message
 * interface, tables definitions, and any other data products that
 * serve to exchange information with other entities.
 *
 */

/**
**  \cfetimecfg Default Time Format
**
**  \par Description:
**      The following definitions select either UTC or TAI as the default
**      (mission specific) time format.  Although it is possible for an
**      application to request time in a specific format, most callers
**      should use CFE_TIME_GetTime(), which returns time in the default
**      format.  This avoids having to modify each individual caller
**      when the default choice is changed.
**
**  \par Limits
**      if CFE_MISSION_TIME_CFG_DEFAULT_TAI is defined as true then CFE_MISSION_TIME_CFG_DEFAULT_UTC must be
**      defined as false.
**      if CFE_MISSION_TIME_CFG_DEFAULT_TAI is defined as false then CFE_MISSION_TIME_CFG_DEFAULT_UTC must be
**      defined as true.
*/
#define CFE_MISSION_TIME_CFG_DEFAULT_TAI true
#define CFE_MISSION_TIME_CFG_DEFAULT_UTC false

/**
**  \cfetimecfg Default Time Format
**
**  \par Description:
**      The following definition enables the use of a simulated time at
**      the tone signal using a software bus message.
**
**  \par Limits
**      Not Applicable
*/
#define CFE_MISSION_TIME_CFG_FAKE_TONE true

/**
**  \cfetimecfg Default Time and Tone Order
**
**  \par Description:
**      Time Services may be configured to expect the time at the tone
**      data packet to either precede or follow the tone signal.  If the
**      time at the tone data packet follows the tone signal, then the
**      data within the packet describes what the time "was" at the tone.
**      If the time at the tone data packet precedes the tone signal, then
**      the data within the packet describes what the time "will be" at
**      the tone.  One, and only one, of the following symbols must be set to true:
**      - CFE_MISSION_TIME_AT_TONE_WAS
**      - CFE_MISSION_TIME_AT_TONE_WILL_BE
**      Note: If Time Services is defined as using a simulated tone signal
**            (see #CFE_MISSION_TIME_CFG_FAKE_TONE above), then the tone data packet
**            must follow the tone signal.
**
**  \par Limits
**      Either CFE_MISSION_TIME_AT_TONE_WAS or CFE_MISSION_TIME_AT_TONE_WILL_BE must be set to true.
**      They may not both be true and they may not both be false.
*/
#define CFE_MISSION_TIME_AT_TONE_WAS     true
#define CFE_MISSION_TIME_AT_TONE_WILL_BE false

/**
**  \cfetimecfg Min and Max Time Elapsed
**
**  \par Description:
**      Based on the definition of Time and Tone Order
**      (CFE_MISSION_TIME_AT_TONE_WAS/WILL_BE) either the "time at the tone" signal or
**      data packet will follow the other. This definition sets the valid window
**      of time for the second of the pair to lag behind the first. Time
**      Services will invalidate both the tone and packet if the second does not
**      arrive within this window following the first.
**
**      For example, if the data packet follows the tone, it might be valid for
**      the data packet to arrive between zero and 100,000 micro-seconds after
**      the tone.  But, if the tone follows the packet, it might be valid
**      only if the packet arrived between 200,000 and 700,000 micro-seconds
**      before the tone.
**
**      Note: units are in micro-seconds
**
**  \par Limits
**       0 to 999,999 decimal
*/
#define CFE_MISSION_TIME_MIN_ELAPSED 0
#define CFE_MISSION_TIME_MAX_ELAPSED 200000

/**
**  \cfetimecfg Default Time Values
**
**  \par Description:
**      Default time values are provided to avoid problems due to time
**      calculations performed after startup but before commands can be
**      processed.  For example, if the default time format is UTC then
**      it is important that the sum of MET and STCF always exceed the
**      value of Leap Seconds to prevent the UTC time calculation
**     <tt>(time = MET + STCF - Leap Seconds) </tt> from resulting in a negative
**     (very large) number.<BR><BR>
**     Some past missions have also created known (albeit wrong) default
**     timestamps.  For example, assume the epoch is defined as Jan 1, 1970
**     and further assume the default time values are set to create a timestamp
**     of Jan 1, 2000.  Even though the year 2000 timestamps are wrong, it
**     may be of value to keep the time within some sort of bounds acceptable
**     to the software.<BR><BR>
**     Note: Sub-second units are in micro-seconds (0 to 999,999) and
**           all values must be defined
**
**  \par Limits
**       Not Applicable
*/
#define CFE_MISSION_TIME_DEF_MET_SECS 1000
#define CFE_MISSION_TIME_DEF_MET_SUBS 0

#define CFE_MISSION_TIME_DEF_STCF_SECS 1000000
#define CFE_MISSION_TIME_DEF_STCF_SUBS 0

#define CFE_MISSION_TIME_DEF_LEAPS 37

#define CFE_MISSION_TIME_DEF_DELAY_SECS 0
#define CFE_MISSION_TIME_DEF_DELAY_SUBS 1000

/**
**  \cfetimecfg Default EPOCH Values
**
**  \par Description:
**      Default ground time epoch values
**      Note: these values are used only by the CFE_TIME_Print() API function
**
**  \par Limits
**      Year - must be within 136 years
**      Day - Jan 1 = 1, Feb 1 = 32, etc.
**      Hour - 0 to 23
**      Minute - 0 to 59
**      Second - 0 to 59
**      Micros - 0 to 999999
*/
#define CFE_MISSION_TIME_EPOCH_YEAR   1980
#define CFE_MISSION_TIME_EPOCH_DAY    1
#define CFE_MISSION_TIME_EPOCH_HOUR   0
#define CFE_MISSION_TIME_EPOCH_MINUTE 0
#define CFE_MISSION_TIME_EPOCH_SECOND 0
#define CFE_MISSION_TIME_EPOCH_MICROS 0

/**
**  \cfetimecfg Time File System Factor
**
**  \par Description:
**      Define the s/c vs file system time conversion constant...
**
**      Note: this value is intended for use only by CFE TIME API functions to
**      convert time values based on the ground system epoch (s/c time) to
**      and from time values based on the file system epoch (fs time).
**
**      FS time  = S/C time + factor
**      S/C time = FS time - factor
**
**      Worksheet:
**
**      S/C epoch = Jan 1, 2005  (LRO ground system epoch)
**      FS epoch  = Jan 1, 1980  (vxWorks DOS file system epoch)
**
**      Delta = 25 years, 0 days, 0 hours, 0 minutes, 0 seconds
**
**      Leap years = 1980, 1984, 1988, 1992, 1996, 2000, 2004
**      (divisible by 4 -- except if by 100 -- unless also by 400)
**
**      1 year   =  31,536,000 seconds
**      1 day    =      86,400 seconds
**      1 hour   =       3,600 seconds
**      1 minute =          60 seconds
**
**      25 years = 788,400,000 seconds
**      7 extra leap days = 604,800 seconds
**
**      total delta = 789,004,800 seconds
**
**  \par Limits
**      Not Applicable
*/
#define CFE_MISSION_TIME_FS_FACTOR 789004800

#endif /* EXAMPLE_MISSION_CFG_H */
