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

cmake_minimum_required(VERSION 2.6.4)
project(CFECORE C)

# The CFE core target to generate MUST be given
# this allows more than one cfe core target per architecture 
if (NOT CFE_CORE_TARGET)
  message(FATAL_ERROR "CFE_CORE_TARGET must be defined to build cFE core")
endif (NOT CFE_CORE_TARGET)

# The cmake CURRENT binary include dir will have the generated platform config file (cfe_platform_cfg.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/inc)

# Define the "_CFE_CORE_" macro during compilation of all CFE core sources
add_definitions(-D_CFE_CORE_)

set(CFE_CORE_MODULES es sb evs tbl time fs)
set(CFE_ALL_MODULE_SRCS)

foreach(MODULE ${CFE_CORE_MODULES} config shared)
  aux_source_directory(src/${MODULE} CFE_ALL_MODULE_SRCS)
endforeach(MODULE ${CFE_CORE_MODULES})

add_library(${CFE_CORE_TARGET} STATIC ${CFE_ALL_MODULE_SRCS})

if (ENABLE_UNIT_TESTS)
  add_subdirectory(unit-test)
endif (ENABLE_UNIT_TESTS)


