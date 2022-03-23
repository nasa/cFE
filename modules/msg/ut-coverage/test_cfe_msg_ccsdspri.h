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
 * cfe_msg_ccsdspri test header
 */

#ifndef TEST_CFE_MSG_CCSDSPRI_H
#define TEST_CFE_MSG_CCSDSPRI_H

/*
 * Defines
 */

#define TEST_MSG_SIZE_OFFSET 7 /* CCSDS Message length offset */

/*
 * Functions
 */
/* Test CCSDS Primary header accessor functions */
void Test_MSG_CCSDSPri(void);

#endif /* TEST_CFE_MSG_CCSDSPRI_H */
