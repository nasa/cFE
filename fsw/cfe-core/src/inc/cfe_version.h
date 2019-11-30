/*
**  GSC-18128-1, "Core Flight Executive Version 6.6"
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
**
**     Version Number Semantics:
**
**     The version number is a sequence of four numbers, generally separated by dots when written. These are, in order,
**     the Major number, the Minor number, the Implementation Revision number, and the Mission Revision number. At
**     their option, Missions may modify the Mission Revision information as needed to suit their needs.
**
**     The Major number shall be incremented to indicate when there is a change to an API. Specifically when the API
**     changes in ways that will cause existing correctly-written cFS components to stop working. It may also be
**     incremented for a release that contains changes deemed to be of similar impact, even if there are no actual
**     changes to the API.  The Major number may increment when there is no breaking change to the API, if the changes
**     are significant enough to warrant the same level of attention as a breaking API change.  The Major number is
**     provided as a simple macro defined in this header file as part of the API definition; this macro must expand
**     to a simple integer value, so that it can be used in simple #if directives by the macro preprocessor.
**
**     The Minor number shall be incremented to indicate the addition of features to the API, which do not break
**     existing code. It may also be incremented for a release that contains changes deemed to be of similar impact,
**     even if there are no actual updates to the API.  The Minor number may increment  when there have been no
**     augmentations to the API, if changes are as significant as additions to the public API.  The Minor number is
**     provided as a simple macro defined in this header file as part of the API definition; this macro must expand
**     to a simple integer value, so that it can be used in simple #if directives by the macro preprocessor.
**
**     The Implementation Revision Version number shall be incremented when updates are made to an implementation that
**     do not have consequences visible to external components. It may also be updated if there are other changes
**     contained within a release that make it desirable for applications to distinguish one release from another.  The
**     Revision number may update in implementations where no actual implementation-specific code has changed, if
**     there are other changes within the release with similar significance.  The Revision number is provided as a
**     simple macro defined in this header file as part of the API definition; this macro must expand to a simple
**     integer value, so that it can be used in simple #if directives by the macro preprocessor.
**
**     The Mission Version number shall be set to zero in all officially released packages, and is entirely reserved
**     for the use of the mission.  The Mission Version is provided as a simple macro defined in the cfe_platform_cfg.h
**     header file.
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**     cFE Flight Software Application Developers Guide
**
**  Notes:
**
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
#define CFE_REVISION              3


#endif  /* _cfe_version_ */

