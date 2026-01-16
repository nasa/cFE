###########################################################
#
# CFE mission build setup
#
# This file is evaluated as part of the "prepare" stage
# and can be used to set up prerequisites for the build,
# such as generating header files
#
###########################################################

# Generate the "cfe_mission_cfg.h" and "cfe_perfids.h" header files
# these usually come from user config, but CFE provides defaults

set(CORE_API_MISSION_CONFIG_FILE_LIST
    cfe_core_api_interface_cfg_values.h
    cfe_core_api_msgid_mapping.h
    cfe_mission_cfg.h
    cfe_perfids.h
)

generate_configfile_set(${CORE_API_MISSION_CONFIG_FILE_LIST})
