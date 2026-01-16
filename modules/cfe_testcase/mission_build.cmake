###########################################################
#
# TEST Core Module mission build setup
#
# This file is evaluated as part of the "prepare" stage
# and can be used to set up prerequisites for the build,
# such as generating header files
#
###########################################################

# The list of header files that control the TEST configuration
set(TESTCASE_MISSION_CONFIG_FILE_LIST
  cfe_test_msg.h
  cfe_test_msgdefs.h
  cfe_test_msgstruct.h
  cfe_test_msgids.h
  cfe_test_msgid_values.h
  cfe_test_tbl.h
  cfe_test_tblstruct.h
  cfe_test_topicid_values.h
)

generate_configfile_set(${TESTCASE_MISSION_CONFIG_FILE_LIST})
