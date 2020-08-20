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
 * Message header test utilities
 */
#ifndef test_msg_utils_
#define test_msg_utils_

/*
 * Includes
 */
#include "cfe_msg_typedefs.h"

/*
 * Defines
 */

/* Subtest macro */
#define MSG_UT_ADD_SUBTEST(Func) UT_AddSubTest(Func, NULL, NULL, __func__, #Func)

/* Prints the message, 0 length uses length from message */
void Test_MSG_PrintMsg(const CFE_MSG_Message_t *MsgPtr, CFE_MSG_Size_t Size);

/* Sums the message for checking, generically check the entire message */
unsigned long long int Test_MSG_Sum(const CFE_MSG_Message_t *MsgPtr);

#endif /* test_msg_utils_ */
