##################################################################
#
# cFE software bus routing module CMake build recipe
#
# This CMakeLists.txt adds source files for the
# SBR module included in the cFE distribution.  Selected
# files are built into a static library that in turn
# is linked into the final executable.
#
# Note this is different than applications which are dynamically
# linked to support runtime loading.  The core applications all
# use static linkage.
#
##################################################################

project(CFE_SBR C)

if (NOT MISSION_MSGMAP_IMPLEMENTATION)
    set(MISSION_MSGMAP_IMPLEMENTATION "DIRECT")
endif (NOT MISSION_MSGMAP_IMPLEMENTATION)

if (MISSION_MSGMAP_IMPLEMENTATION STREQUAL "DIRECT")
    message(STATUS "Using direct map software bus routing implementation")
    set(${DEP}_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/fsw/src/cfe_sbr_map_direct.c
        ${CMAKE_CURRENT_SOURCE_DIR}/fsw/src/cfe_sbr_route_unsorted.c)
elseif (MISSION_MSGMAP_IMPLEMENTATION STREQUAL "HASH")
    message(STATUS "Using hashed map software bus routing implementation")
    set(${DEP}_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/fsw/src/cfe_sbr_map_hash.c
        ${CMAKE_CURRENT_SOURCE_DIR}/fsw/src/cfe_sbr_route_unsorted.c)
else()
    message(ERROR "Invalid software bus routing implementation selected:" MISSION_MSGMAP_IMPLEMENTATION)
endif()

# Module library
add_library(${DEP} STATIC ${${DEP}_SRC})

# Add private include
target_include_directories(${DEP} PRIVATE private_inc)
target_link_libraries(sbr PRIVATE core_private)

# Add unit test coverage subdirectory
if(ENABLE_UNIT_TESTS)
    add_subdirectory(ut-coverage)
endif(ENABLE_UNIT_TESTS)

