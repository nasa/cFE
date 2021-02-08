###########################################################
#
# Resource ID arch build setup
#
# This file is evaluated as part of the "prepare" stage
# and can be used to set up prerequisites for the build, 
# such as generating header files
# 
###########################################################

# allow all other code to refer to the public API in the "inc" dir
# ideally this should use an interface library instead
include_directories(${CMAKE_CURRENT_LIST_DIR}/inc)
