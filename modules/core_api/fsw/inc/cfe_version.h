/**
 * @file
 *
 * Provide version identifiers for the cFE core. See @ref cfsversions for further details.
 */

#ifndef CFE_VERSION_H
#define CFE_VERSION_H

/* Development Build Macro Definitions */
#define CFE_BUILD_NUMBER   80           /**< @brief Development: Number of development commits since baseline */
#define CFE_BUILD_BASELINE "v7.0.0-rc4" /**< @brief Development: Reference git tag for build number */

/* Version Macro Definitions updated for official releases only */
#define CFE_MAJOR_VERSION 6 /**< @brief Major release version (Former for Revision == 99) */
#define CFE_MINOR_VERSION 7 /**< @brief Minor release version (Former for Revision == 99) */
#define CFE_REVISION \
    99 /*!< @brief * Set to 0 on OFFICIAL releases, and set to 99 on development versions. Revision number.      */

/*!
 * @brief Mission revision.
 *
 * Reserved for mission use to denote patches/customizations as needed.
 */
#define CFE_MISSION_REV 0

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
    " cFE DEVELOPMENT BUILD " CFE_SRC_VERSION " (Codename: Draco), Last Official Release: cfe v6.7.0"

#endif /* CFE_VERSION_H */
