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

/**
 * @file
 *
 * Provide version identifiers for the cFE core. See @ref cfsversions for further details.
 */

#ifndef CFE_VERSION_H
#define CFE_VERSION_H

/* Development Build Macro Definitions */
#define CFE_BUILD_NUMBER   1030         /**< @brief Development: Number of development commits since baseline */
#define CFE_BUILD_BASELINE "v6.8.0-rc1" /**< @brief Development: Reference git tag for build number */

/* Version Macro Definitions updated for official releases only */
#define CFE_MAJOR_VERSION 6  /**< @brief Major release version (Former for Revision == 99) */
#define CFE_MINOR_VERSION 7  /**< @brief Minor release version (Former for Revision == 99) */
#define CFE_REVISION      99 /**< @brief Revision, 99 indicates development */
#define CFE_MISSION_REV   0  /**< @brief Mission revision, reserved for mission use */

#define CFE_STR_HELPER(x) #x                /**< @brief Convert argument to string */
#define CFE_STR(x)        CFE_STR_HELPER(x) /**< @brief Expand macro before conversion */

/**
 * @brief Short Build Version String
 *
 * Short string identifying the build, see @ref cfsversions for suggested format for development
 * and official releases.
 */
#define CFE_SRC_VERSION CFE_BUILD_BASELINE "+dev" CFE_STR(CFE_BUILD_NUMBER)

/**
 * @brief Long Build Version String
 *
 * Long freeform string identifying the build, see @ref cfsversions for suggested format for development
 * and official releases.
 */
#define CFE_VERSION_STRING \
    " cFE DEVELOPMENT BUILD " CFE_SRC_VERSION " (Codename: Bootes), Last Official Release: cfe v6.7.0"

#endif /* CFE_VERSION_H */
