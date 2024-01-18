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
 *   CFE Test app (CFE_TEST) Application Message Definitions
 */
#ifndef CFE_TEST_MSGSTRUCT_H
#define CFE_TEST_MSGSTRUCT_H

#include "cfe_msg_hdr.h"
#include "cfe_test_msgdefs.h"

/* A simple command message with a 64 bit payload */
typedef struct CFE_TEST_TestCmdMessage
{
    CFE_MSG_CommandHeader_t  CommandHeader;
    CFE_TEST_TestPayload64_t Payload;
} CFE_TEST_TestCmdMessage64_t;

/* A simple telemetry message with a 64 bit payload */
typedef struct CFE_TEST_TestTlmMessage
{
    CFE_MSG_TelemetryHeader_t TelemetryHeader;
    CFE_TEST_TestPayload64_t  Payload;
} CFE_TEST_TestTlmMessage64_t;

/* A simple command message with a 32 bit payload */
typedef struct
{
    CFE_MSG_CommandHeader_t  CommandHeader;
    CFE_TEST_TestPayload32_t Payload;
} CFE_TEST_TestCmdMessage32_t;

/* A simple telemetry message with a 32 bit payload */
typedef struct
{
    CFE_MSG_TelemetryHeader_t TelemetryHeader;
    CFE_TEST_TestPayload32_t  Payload;
} CFE_TEST_TestTlmMessage32_t;

#endif
