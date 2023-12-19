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

    if ($ENV{VERBOSE})
      message("inside get_version for ${DEP}")
    endif()

    execute_process(
        COMMAND ${GIT_EXECUTABLE} describe --tags --always --dirty
        WORKING_DIRECTORY ${DIR}
        OUTPUT_VARIABLE GIT_DESC_OUTPUT
        RESULT_VARIABLE GIT_RESULT
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    # Export output to parent scope
    set(${DEP}_NAME "${NAME}" PARENT_SCOPE)

    # If result was successful, then string-ify it, otherwise use NULL
    if (GIT_RESULT EQUAL 0)
      set(${DEP}_VERSION "\"git:${GIT_DESC_OUTPUT}\"" PARENT_SCOPE)
    else()
      set(${DEP}_VERSION "NULL" PARENT_SCOPE)
    endif()

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

# Get version for all mission apps/dependencies (they may be different)
foreach(DEP "MISSION" ${MISSION_DEPS})
  get_version(${DEP})
endforeach()


# Use configure_file() command to generate the final output file because this can detect
# and only update it if it changes.
set(CFE_KEYVALUE_TABLE_NAME "CFE_MODULE_VERSION_TABLE")
configure_file(${BIN}/cfe_module_version.in ${BIN}/src/cfe_module_version_table.c @ONLY)
