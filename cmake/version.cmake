##################################################################
#
# cFS version metadata collection script
#
# This small script runs at build time (as opposed to prep time)
# and is intended to extract version metadata from the current source
# tree.  It is done each time that the code is built, since the 
# metadata could change at any time (i.e. a different branch could
# be checked out, or additional changes committed)
#
# This generates two header files: 
#   cfecfs_version_info.h -- contains version control metadata
#   cfecfs_build_info.h -- contains build information (user,host,etc) 
#
# Currently only git is supported as a version control source, however
# it could be extended to others by adding the appropriate command
#
##################################################################

set(GIT_EXECUTABLE git)

function(get_version DEP)
    if (DEP STREQUAL "MISSION")
      set(NAME ${MISSION_NAME})
      set(DIR ${MISSION_SOURCE_DIR})
    else()
      if(EXISTS ${${DEP}_MISSION_DIR}/version_info.cmake)
        include(${${DEP}_MISSION_DIR}/version_info.cmake)
      else()
        set(NAME ${DEP})
      endif()
      set(DIR ${${DEP}_MISSION_DIR})
    endif()
    execute_process(
        COMMAND ${GIT_EXECUTABLE} describe --tags --always --dirty
        WORKING_DIRECTORY ${DIR} 
        OUTPUT_VARIABLE GIT_DESC_OUTPUT 
        RESULT_VARIABLE GIT_RESULT
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if (NOT GIT_RESULT EQUAL 0)
      set(GIT_DESC_OUTPUT "error/unknown")
    endif()
    set(${DEP}_NAME "${NAME}" PARENT_SCOPE)
    set(${DEP}_VERSION "${GIT_DESC_OUTPUT}" PARENT_SCOPE)
endfunction()


# First read in any variables that are passed in from the parent process
# There may be many of these and they may not all be passable via -D options
file(STRINGS "${BIN}/mission_vars.cache" PARENTVARS)
set(VARNAME)
foreach(PV ${PARENTVARS})
  if (VARNAME)
    set(${VARNAME} ${PV})
    set(VARNAME)
  else()
    set(VARNAME ${PV})
  endif()
endforeach(PV ${PARENTVARS})

# Set other well-known locations
set(OSAL_MISSION_DIR ${MISSION_SOURCE_DIR}/osal)

# Start the template for configure_file() -- 
# see below why it is done this way and not written directly
file(WRITE ${BIN}/cfecfs_version_info.h.in 
    "/* Auto-generated version information file */\n"
    "#define MISSION_CONFIG      \"\@MISSIONCONFIG\@\"\n" 
    "#define MISSION_NAME        \"\@MISSION_NAME\@\"\n") 
    
# Get version for all mission apps/dependencies (they may be different)
foreach(DEP "MISSION" ${MISSION_DEPS})
  get_version(${DEP})
  string(TOUPPER ${${DEP}_NAME} MACRONAME)
  string(REGEX REPLACE "[^A-Z0-9_]+" "_" MACRONAME "${MACRONAME}")
  file(APPEND ${BIN}/cfecfs_version_info.h.in "#define ${MACRONAME}_VERSION \"\@${DEP}_VERSION\@\"\n") 
endforeach()

string(TOUPPER ${MISSION_NAME} MACRONAME)
file(APPEND ${BIN}/cfecfs_version_info.h.in "\n#define MISSION_VERSION ${MACRONAME}_VERSION\n\n /* end */\n")
   
# Same for build information -
# note this is done separately because the build time might change often -
# only the final target EXE should include build.h (nothing else in the mission) to minimize rebuilds
file(WRITE ${BIN}/cfecfs_build_info.h.in 
    "/* Auto-generated build information file */\n"
    "#define MISSION_BUILDDATE   \"\@BUILDDATE\@\"\n"
    "#define MISSION_BUILDUSER   \"\@USERNAME\@\"\n"
    "#define MISSION_BUILDHOST   \"\@HOSTNAME\@\"\n\n")

# OVERRIDE NOTE: 
# All 3 of these may be passed via environment variables to force a particular
# date, user, or hostname i.e. if hoping to reproduce an exact binary of a prior build
# They are actually free-form strings so they can be anything up to 64 chars.

# Get the current date and time
set(BUILDDATE $ENV{BUILDDATE})
if (NOT BUILDDATE)
    execute_process(
        COMMAND date "+%Y%m%d%H%M"
        OUTPUT_VARIABLE BUILDDATE
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif(NOT BUILDDATE)

# Get the build host
set(HOSTNAME $ENV{HOSTNAME})
if (NOT HOSTNAME)
    execute_process(
        COMMAND hostname
        OUTPUT_VARIABLE HOSTNAME
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif (NOT HOSTNAME)

# Get the user ID
set(USERNAME $ENV{USER})
if (NOT USERNAME)
    execute_process(
        COMMAND whoami
        OUTPUT_VARIABLE USERNAME
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif (NOT USERNAME)

# Use configure_file() command to generate the final output file because this can detect
# and only update it if it changes.
configure_file(${BIN}/cfecfs_version_info.h.in ${BIN}/inc/cfecfs_version_info.h @ONLY)
configure_file(${BIN}/cfecfs_build_info.h.in ${BIN}/inc/cfecfs_build_info.h @ONLY)


