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

#ifndef CFE_CONFIG_EDS_H
#define CFE_CONFIG_EDS_H

/* =================== */
/* Function Prototypes */
/* =================== */

/**
 * \brief  Return the state of EDS as defined by CFE_EDS_ENABLED_BUILD
 * \note   This effective converts the CFE_EDS_ENABLED_BUILD macro
 *         into a string. The difference is that the determination is
 *         made by CMake rather than a conditional compilation within
 *         C, which can cause problems
 * \return String Indicating the State of EDS
 * \retval "enabled"
 * \retval "disabled"
 */
const char *CFE_Config_EdsState(void);

#endif // CFE_CONFIG_EDS_H