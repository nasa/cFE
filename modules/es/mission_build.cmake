###########################################################
#
# ES Core Module mission build setup
#
# This file is evaluated as part of the "prepare" stage
# and can be used to set up prerequisites for the build,
# such as generating header files
#
###########################################################

# The list of header files that control the ES configuration
set(ES_MISSION_CONFIG_FILE_LIST
  cfe_es_mission_cfg.h
  cfe_es_interface_cfg_values.h
  cfe_es_memaddress.h
  cfe_es_extern_typedefs.h
  cfe_es_fcncode_values.h
  cfe_es_msgdefs.h
  cfe_es_msg.h
  cfe_es_msgstruct.h
  cfe_es_topicid_values.h
)


generate_configfile_set(${ES_MISSION_CONFIG_FILE_LIST})
