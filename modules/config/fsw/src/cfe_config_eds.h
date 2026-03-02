/************************************************************************
 * NASA Docket No. GSC-19,200-1, and identified as "cFS Draco"
 *
 * Copyright (c) 2023 United States Government as represented by the
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
 * \brief  Return the state of EDS for the given component
 *
 * Determines if the component/module includes EDS-based interface definitions, and whether
 * those definitions are being utilized in the running CFE/CFS build.
 *
 * CFS applications may or may not provide an EDS file to describe their respective
 * interface (this includes commands, telemetry, tables, and anything else that is externally
 * visible).  In order to utlize the EDS-provided information, EDS support must be enabled
 * at the build level (via compile time switch) and the generated database must be linked in with
 * the application.
 *
 * The result is returned as a free form string that is intended to be included in event
 * message or system log text that is visible to the operator.  A string value allows the implementation
 * to better capture nuances of cases where the EDS support is only paritally implemented in a component.
 *
 * - If EDS is globally disabled at the mission scope, then this returns the string "disabled"
 * - If EDS is enabled globally and but the component did not provide an EDS, this returns "inactive"
 * - If EDS is enabled globally and the component has an EDS that defines all interfaces, this returns "active"
 *
 * \note Other string return values are also possible, depending on the level of EDS that is implemented
 * and active (such as EDS for commands only, or tables only, etc).  The list of strings specified here
 * are just examples of possible outputs, it is not indended to mean these are the only outputs.
 *
 * \return String indicating the state of EDS support in the given component
 * \retval "disabled" if EDS support is disabled at the global level
 * \retval "inactive" if the EDS is enabled but the component does not have/provide EDS information
 * \retval "active" if the EDS is enabled and the component provides the full set of EDS information
 *
 */
const char *CFE_Config_EdsState(const char *ComponentName);

#endif // CFE_CONFIG_EDS_H