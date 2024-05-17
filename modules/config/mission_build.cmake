###########################################################
#
# CFE configuration mission build setup
#
# This file is evaluated as part of the "prepare" stage
# and can be used to set up prerequisites for the build,
# such as generating header files
#
###########################################################

# Generate the complete list of configuration ids
# Start with an empty list - will be appended by various files
set(CFE_CONFIG_IDS)

# Get the base set of config IDs from a separate file
# This baseline set is based on build time config and will be the same
# on all CFE instances produced by this build.
include(${CMAKE_CURRENT_LIST_DIR}/config_ids_base.cmake)

# Get the set of config IDs that are generated via the platform config tool.
# These are set to values based on platform config and may be different on
# the different CFE instances.  To accomplish this, an intermediate translation
# tool must be used - this resides in the "tool" directory.
include(${CMAKE_CURRENT_LIST_DIR}/tool/config_ids_dynamic.cmake)

# Append any mission-specific config IDs.  This is done last so it does
# not change the order of items in the first two lists.
include(${MISSIONCONFIG}/config_ids_custom.cmake OPTIONAL)

# Now generate header and source files containing the ID tables
# This includes an enum, and enum to string name table, and a set
# of constants to use in runtime FSW code.
set(GENERATED_ID_MACRO_LIST)
set(GENERATED_CONSTANT_DEFINE_LIST)

foreach(CFGID ${CFE_CONFIG_IDS})
  list(APPEND GENERATED_ID_MACRO_LIST "CFE_CONFIGID_DEFINE(${CFGID})\n")
  list(APPEND GENERATED_CONSTANT_DEFINE_LIST "#define CFE_CONFIGID_${CFGID} CFE_CONFIGID_FROM_OFFSET(${CFGID})\n")
endforeach()

string(CONCAT GENERATED_ID_MACRO_LIST ${GENERATED_ID_MACRO_LIST})
string(CONCAT GENERATED_CONSTANT_DEFINE_LIST ${GENERATED_CONSTANT_DEFINE_LIST})

# Write header file for config offsets - this is the basic enumeration (symbols mapped to 0-based integers)
# This is a simple list and does not need any other headers, and can be used by the platform config tool
configure_file(${CMAKE_CURRENT_LIST_DIR}/cmake/cfe_configid_offset.h.in ${CMAKE_BINARY_DIR}/inc/cfe_configid_offset.h @ONLY)

# Write source file for enum to string mapping - this allows the software to print the config item by name
# this is a simple lookup table based on the enum, so it can also be used by FSW and the platform config tool
configure_file(${CMAKE_CURRENT_LIST_DIR}/cmake/cfe_configid_nametable.c.in ${CMAKE_BINARY_DIR}/src/cfe_configid_nametable.c @ONLY)

# Write header file for config IDs - this is the FSW runtime identifier (resourceID) based on the enum above
# however - it requires the full set of FSW headers, so it cannot be used by the platform config tool
configure_file(${CMAKE_CURRENT_LIST_DIR}/cmake/cfe_config_ids.h.in ${CMAKE_BINARY_DIR}/inc/cfe_config_ids.h @ONLY)

# Add the subdirectory for the intermediate tool that allows platform-speicific data to included with the config
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/tool cfeconfig_platformdata_tool)
