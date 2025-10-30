###########################################################
#
# CFE arch/platform build setup
#
# This file is evaluated as part of the "prepare" stage
# and can be used to set up prerequisites for the build,
# such as generating header files
#
###########################################################

# Generate the platform-specific header files
# these usually come from user config, but CFE provides defaults

set(CORE_API_PLATFORM_CONFIG_FILE_LIST
    cfe_core_api_base_msgid_values.h
    cfe_msgids.h
)

generate_configfile_set(${CORE_API_PLATFORM_CONFIG_FILE_LIST})
