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
 *   Specification for the CFE Time Services (CFE_TIME) command and telemetry
 *   message constant definitions.
 *
 *  For CFE_TIME this is only the function/command code definitions
 */
#ifndef CFE_TIME_MSGDEFS_H
#define CFE_TIME_MSGDEFS_H

#include "cfe_time_fcncodes.h"

/** \defgroup CFETIMEClkStates cFE Clock State Flag Defines
 * \{
 */
#define CFE_TIME_FLAG_CLKSET 0x8000 /**< \brief The spacecraft time has been set */
#define CFE_TIME_FLAG_FLYING 0x4000 /**< \brief This instance of Time Services is flywheeling */
#define CFE_TIME_FLAG_SRCINT 0x2000 /**< \brief The clock source is set to "internal" */
#define CFE_TIME_FLAG_SIGPRI 0x1000 /**< \brief The clock signal is set to "primary" */
#define CFE_TIME_FLAG_SRVFLY 0x0800 /**< \brief The Time Server is in flywheel mode */
#define CFE_TIME_FLAG_CMDFLY 0x0400 /**< \brief This instance of Time Services was commanded into flywheel mode */
#define CFE_TIME_FLAG_ADDADJ 0x0200 /**< \brief One time STCF Adjustment is to be done in positive direction */
#define CFE_TIME_FLAG_ADD1HZ 0x0100 /**< \brief 1 Hz STCF Adjustment is to be done in a positive direction */
#define CFE_TIME_FLAG_ADDTCL 0x0080 /**< \brief Time Client Latency is applied in a positive direction */
#define CFE_TIME_FLAG_SERVER 0x0040 /**< \brief This instance of Time Services is a Time Server */
#define CFE_TIME_FLAG_GDTONE 0x0020 /**< \brief The tone received is good compared to the last tone received */
#define CFE_TIME_FLAG_REFERR \
    0x0010 /**< \brief GetReference read error, will be set if unable to get a consistent ref value */
#define CFE_TIME_FLAG_UNUSED 0x000F /**< \brief Reserved flags - should be zero */
/** \} */

#endif
