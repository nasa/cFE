##################################################################
#
# cFE core module CMake build recipe
#
# This CMakeLists.txt file contains recipes for building the 6
# core applications included in the cFE distribution.  The entire
# set of source code is built into a static library that in turn 
# is linked into the final executable.  
#
# Note this is different than applications which are dynamically 
# linked to support runtime loading.  The core applications all
# use static linkage.
#
##################################################################

project(CFECORE C)

# Define the "_CFE_CORE_" macro during compilation of all CFE core sources
add_definitions(-D_CFE_CORE_)

set(CFE_CORE_MODULES es sb evs tbl time fs)
set(CFE_ALL_MODULE_SRCS)

foreach(MODULE ${CFE_CORE_MODULES})
  aux_source_directory(src/${MODULE} CFE_ALL_MODULE_SRCS)
endforeach(MODULE ${CFE_CORE_MODULES})

add_library(cfe-core STATIC ${CFE_ALL_MODULE_SRCS})

if (ENABLE_UNIT_TESTS)
  add_subdirectory(ut-stubs)
  add_subdirectory(unit-test)
endif (ENABLE_UNIT_TESTS)

