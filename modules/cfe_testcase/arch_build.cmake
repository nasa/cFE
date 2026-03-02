###########################################################
#
# TEST Core Module platform build setup
#
# This file is evaluated as part of the "prepare" stage
# and can be used to set up prerequisites for the build,
# such as generating header files
#
###########################################################

# The list of header files that control the TEST configuration
set(TEST_PLATFORM_CONFIG_FILE_LIST
  cfe_test_msgids.h
  cfe_test_msgid_values.h
)

generate_configfile_set(${TEST_PLATFORM_CONFIG_FILE_LIST})

