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
**  File: cfe_version.h
**
**  Purpose:
**     Provide version identifiers for the cFE core.
*/

/**
 * \page cfeversion Version Numbers
 *
 * <H2> Version Number Semantics </H2>
 *
 * The version number is a sequence of four numbers, generally separated by dots when written. These are, in order,
 * the Major number, the Minor number, the Implementation Revision number, and the Mission Revision number. At
 * their option, Missions may modify the Mission Revision information as needed to suit their needs.
 *
 * The Major number shall be incremented on release to indicate when there is a change to an API
 * that may cause existing correctly-written cFS components to stop working. It may also be incremented for a
 * release that contains changes deemed to be of similar impact, even if there are no actual changes to the API.
 *
 * The Minor number shall be incremented on release to indicate the addition of features to the API,
 * which do not break the existing code.  It may also be incremented for a release that contains changes deemed
 * to be of similar impact, even if there are no actual updates to the API.
 *
 * The Implementation Revision Version number shall be incremented on changes to software in the master branch,
 * or other changes that benefit from unique identification. It is used for identifying open source development
 * versions.  It is important to note that Major and Minor numbers are only updated upon official releases
 * of tagged versions (see the release tab), \b NOT on development version updates in the master branch.
 *
 * The Major, Minor, and Implementation Revision numbers are provided in this header file as part of the API
 * definition; this macro must expand to a simple integer value, so that it can be used in simple if directives
 * by the macro preprocessor.
 *
 * The Mission Version number shall be set to zero in all officially released packages, and is entirely reserved
 * for the use of the mission.  The Mission Version is provided as a simple macro defined in the cfe_platform_cfg.h
 * header file.
 *
 * <H2> Version Number Flexibility </H2>
 *
 * The major number may increment when there is no breaking change to the API, if the changes are significant enough to
 * warrant the same level of attention as a breaking API change.
 *
 * The minor number may increment when there have been no augmentations to the API, if changes are as significant as
 * additions to the public API.
 *
 * The revision numbers may update in implementations where no actual implementation-specific code has changed, if
 * there are other changes within the release with similar significance.
 *
 * <H2> How and Where Defined </H2>
 *
 * The Major, Minor, and Revision components of the version are provided as simple macros defined in the cfe_version.h
 * header file as part of the API definition; these macros must expand to simple integer values, so that they can be used
 * in simple if directives by the macro preprocessor.
 *
 * The Mission Version is provided as a simple macro defined in the cfe_platform_cfg.h header file. As delivered in
 * official releases, these macros must expand to simple integer values, so that they can be used in simple macro
 * preprocessor conditions, but delivered code should not prevent a mission from, for example, deciding that the Mission
 * Version is actually a text string.
 *
 */

#ifndef _cfe_version_
#define _cfe_version_

/*
 * The target config contains extended version information within it.
 * This information is generated automatically by the build using
 * git to determine the most recent tag and commit id.
 */
#include <target_config.h>


/*
** Macro Definitions
*/
#define CFE_MAJOR_VERSION         6
#define CFE_MINOR_VERSION         7
#define CFE_REVISION              11


#endif  /* _cfe_version_ */
