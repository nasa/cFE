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

/**
 * @file
 *
 * Purpose:
 *   This header file contains the mission configuration parameters and
 *   typedefs with mission scope.
 */

#ifndef EDS_CFE_CORE_API_INTERFACE_CFG_VALUES_H
#define EDS_CFE_CORE_API_INTERFACE_CFG_VALUES_H

#include "cfe_mission_eds_designparameters.h"

/* Use the default configuration value for all */
#define CFE_MISSION_CORE_API_CFGVAL(x) EdsParam_CFE_MISSION_CORE_API_##x

#endif /* SAMPLE_MISSION_CFG_H */
