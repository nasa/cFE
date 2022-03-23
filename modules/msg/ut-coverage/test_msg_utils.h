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
 * Message header test utilities
 */

#ifndef TEST_MSG_UTILS_H
#define TEST_MSG_UTILS_H

/*
 * Includes
 */
#include "cfe_msg_api_typedefs.h"

/*
 * Defines
 */

/* Subtest macro */
#define MSG_UT_ADD_SUBTEST(Func) UtTest_AddSubTest(Func, NULL, NULL, __func__, #Func)

#endif /* TEST_MSG_UTILS_H */
