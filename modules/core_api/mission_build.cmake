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
# these must come from mission config
generate_config_includefile(
    FILE_NAME           "cfe_mission_cfg.h" 
    MATCH_SUFFIX        "mission_cfg.h"
    PREFIXES            ${MISSIONCONFIG}
)

generate_config_includefile(
    FILE_NAME           "cfe_perfids.h" 
    MATCH_SUFFIX        "perfids.h"
    PREFIXES            ${MISSIONCONFIG}
)

