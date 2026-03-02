###########################################################
#
# ES Core Module platform build setup
#
# This file is evaluated as part of the "prepare" stage
# and can be used to set up prerequisites for the build,
# such as generating header files
#
###########################################################

# The list of header files that control the ES configuration
set(ES_PLATFORM_CONFIG_FILE_LIST
  cfe_es_internal_cfg_values.h
  cfe_es_msgid_values.h
  cfe_es_msgids.h
  cfe_es_platform_cfg.h
)

generate_configfile_set(${ES_PLATFORM_CONFIG_FILE_LIST})

