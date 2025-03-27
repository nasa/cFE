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
 * Provide version identifiers for the cFE core. See @ref cfsversions for further details.
 */

#ifndef CFE_VERSION_H
#define CFE_VERSION_H

/* Development Build Macro Definitions */
#define CFE_BUILD_NUMBER    239 /**< @brief Development: Number of development git commits since CFE_BUILD_BASELINE */
#define CFE_BUILD_BASELINE  "equuleus-rc1" /**< @brief Development: Reference git tag for build number */
#define CFE_BUILD_DEV_CYCLE "equuleus-rc2" /**< @brief Development: Release name for current development cycle */
#define CFE_BUILD_CODENAME  "Equuleus"     /**< @brief: Development: Code name for the current build */

/* See \ref cfsversions for definitions */
#define CFE_MAJOR_VERSION 6 /*!< @brief Major version number */
#define CFE_MINOR_VERSION 7 /*!< @brief Minor version number */
#define CFE_REVISION      0 /*!< @brief Revision version number. Value of 0 indicates a development version.*/

/**
 * @brief Last official release.
 */
#define CFE_LAST_OFFICIAL "v6.7.0"

/*!
 * @brief Mission revision.
 *
 * Values 1-254 are reserved for mission use to denote patches/customizations as needed. NOTE: Reserving 0 and 0xFF for
 * cFS open-source development use (pending resolution of nasa/cFS#440)
 *
 */
#define CFE_MISSION_REV 0xFF

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
 * @brief Max Version String length.
 *
 * Maximum length that a cFE version string can be.
 */
#define CFE_CFG_MAX_VERSION_STR_LEN 256

#endif /* CFE_VERSION_H */
