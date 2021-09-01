##################################################################
#
# cFS project top-level mission build recipes
#
# This manages the overall top-level build environment
#
# Note that the target CPUs may use different architectures, therefore each
# architecture must be done as a separate sub-build since none of the binaries
# can be shared.
#
# This is a normal (non-cross) build recipe and a custom target
# is generated per-cpu which is typically a cross (arch-specific) build.
#
# In addition to those custom targets, targets for tools that execute directly
# on the development host may also be created via this recipe.
#
##################################################################


##################################################################
#
# FUNCTION: initialize_globals
#
# Set up global mission configuration variables.
# In the top level mode (this file)t reads extracts  state info from
# configuration files within the project source tree (the _defs directory)
#
function(initialize_globals)

    # Entry point for the "mission" (top level) build
    # Determine the values for the cache variables
    if (NOT DEFINED MISSION_BINARY_DIR)
      set(MISSION_BINARY_DIR ${CMAKE_BINARY_DIR} CACHE PATH "Top level mission binary directory")
    endif (NOT DEFINED MISSION_BINARY_DIR)

    # this is the parent (mission) build and variable values must be determined
    # Obtain the "real" top-level source directory and set it in parent scope
    if (NOT DEFINED MISSION_SOURCE_DIR)
      get_filename_component(MISSION_SOURCE_DIR "${CFE_SOURCE_DIR}/.." ABSOLUTE)
      set(MISSION_SOURCE_DIR ${MISSION_SOURCE_DIR} CACHE PATH "Top level mission source directory")
    endif(NOT DEFINED MISSION_SOURCE_DIR)

    # The configuration should be in a subdirectory named "<mission>_defs".  If there is one
    # and only one of these, this is assumed to be it.  If there is more than one then the
    # user MUST specify which one is intended to be used by setting MISSIONCONFIG in the environment
    if (NOT MISSIONCONFIG)
      set(MCTEMP $ENV{MISSIONCONFIG})
      if ("${MCTEMP}" STREQUAL "")
        file(GLOB DEFDIRS RELATIVE "${MISSION_SOURCE_DIR}" "${MISSION_SOURCE_DIR}/*_defs")
        list(LENGTH DEFDIRS DDLEN)
        if (NOT DDLEN EQUAL 1)
          message(FATAL_ERROR "Unable to automatically determine the mission config directory.  Specify it with the MISSIONCONFIG variable.")
        endif (NOT DDLEN EQUAL 1)
        string(REPLACE "_defs" "" MCTEMP ${DEFDIRS})
        message(STATUS "Mission configuration ${MCTEMP} automatically selected")
      endif ("${MCTEMP}" STREQUAL "")
      # Set the MISSIONCONFIG as a CMake cache variable so it will be preserved for future runs
      set(MISSIONCONFIG ${MCTEMP} CACHE STRING "Mission configuration selection")
      unset(MCTEMP)
    endif(NOT MISSIONCONFIG)

    # Cache the values of certain environment variables used during the setup process
    # The issue with environment variables is that they are transient and may change at any time,
    # such as if the build is invoked from a different shell.  Whenever the build system is regenerated
    # in the future, we need to use the same values for these options even if the user has modified them
    # in the local environment.
    set(SIMULATION $ENV{SIMULATION} CACHE STRING "Enable simulation mode using specified toolchain")
    set(ENABLE_UNIT_TESTS $ENV{ENABLE_UNIT_TESTS} CACHE BOOL "Enable build of unit tests")

    # Export values to parent level
    set(MISSION_DEFS ${MISSION_SOURCE_DIR}/${MISSIONCONFIG}_defs CACHE PATH "Full path to mission definitions directory")

endfunction(initialize_globals)

##################################################################
#
# FUNCTION: generate_build_version_templates
#
# Generates file templates for use with configure_file() which is
# invoked at build time to get the required information.
#
# Note that some information may change between generation and build
# times, hence why only a template can be generated here, the final
# file content must be generated via a build rule.
#
function(generate_build_version_templates)

    # File header for build info template (tag file as auto-generated)
    string(CONCAT GENERATED_FILE_HEADER
        "/* This file is auto-generated from CMake build system.  Do not manually edit! */\n"
        "#include \"target_config.h\"\n"
        "const CFE_ConfigKeyValue_t @CFE_KEYVALUE_TABLE_NAME@[] = {\n"
    )

    # File trailer for build info template
    string(CONCAT GENERATED_FILE_TRAILER
        "{ NULL, NULL } /* End of list */\n"
        "};\n"
        "/* End of file */\n"
    )

    # These variables are deferred until build time
    foreach (VAR BUILDDATE BUILDUSER BUILDHOST)
        list (APPEND GENERATED_FILE_CONTENT "{ \"${VAR}\", \"@${VAR}@\" },")
    endforeach ()
    string(REPLACE ";" "\n" GENERATED_FILE_CONTENT "${GENERATED_FILE_CONTENT}")

    # Write a template for build/config information
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/cfe_generated_file.h.in ${CMAKE_BINARY_DIR}/cfe_build_env.in)

    # Content for version info - all are deferred until build time
    set(GENERATED_FILE_CONTENT)
    foreach(DEP "MISSION" ${MISSION_DEPS})
        list (APPEND GENERATED_FILE_CONTENT "{ \"${DEP}\", @${DEP}_VERSION@ },")
    endforeach()
    string(REPLACE ";" "\n" GENERATED_FILE_CONTENT "${GENERATED_FILE_CONTENT}")

    # Write a template for version information
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/cfe_generated_file.h.in ${CMAKE_BINARY_DIR}/cfe_module_version.in)

    # The actual version information (to fill out the template above) is obtained at build time
    # via a script that is executed as a build target.  If this script exists in the mission defs
    # directory (user-supplied) then use that.  Otherwise a pre-canned "git" version is included
    # as a fallback, which should work for source trees assembled via git submodules or subtrees.
    if (EXISTS "${MISSION_DEFS}/generate_module_version.cmake")
        set(VERSION_SCRIPT "${MISSION_DEFS}/generate_module_version.cmake")
    else()
        set(VERSION_SCRIPT "${CFE_SOURCE_DIR}/cmake/generate_git_module_version.cmake")
    endif()

    add_custom_target(cfe-module-version
        COMMAND
            ${CMAKE_COMMAND} -D BIN=${CMAKE_BINARY_DIR}
                -P "${VERSION_SCRIPT}"
        WORKING_DIRECTORY
            ${CMAKE_SOURCE_DIR}
        VERBATIM
    )

    add_custom_target(cfe-build-env
        COMMAND
            ${CMAKE_COMMAND} -D BIN=${CMAKE_BINARY_DIR}
                -P "${CFE_SOURCE_DIR}/cmake/generate_build_env.cmake"
        WORKING_DIRECTORY
            ${CMAKE_SOURCE_DIR}
        VERBATIM
    )

    # Content for build info - these vars can be evaluated right now, no need to defer
    set(GENERATED_FILE_HEADER "/* Automatically generated from CMake build system */")
    string(CONCAT GENERATED_FILE_CONTENT
        "const char CFE_MISSION_NAME[] = \"${MISSION_NAME}\";\n"
        "const char CFE_MISSION_CONFIG[] = \"${MISSIONCONFIG}\";\n"
    )
    set(GENERATED_FILE_TRAILER "/* End of file */")
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/cfe_generated_file.h.in ${CMAKE_BINARY_DIR}/src/cfe_mission_strings.c)

    add_custom_target(mission-version
        DEPENDS cfe-module-version cfe-build-env
    )

endfunction(generate_build_version_templates)

##################################################################
#
# FUNCTION: prepare
#
# Called by the top-level CMakeLists.txt to set up prerequisites
#
function(prepare)

  # Propagate the SIMULATION variable if set
  if (SIMULATION)
    add_definitions(-DSIMULATION=${SIMULATION})
  endif (SIMULATION)

  # Create custom targets for building and cleaning all architectures
  # This is required particularly for doing extra stuff in the clean step
  add_custom_target(mission-all COMMAND $(MAKE) all)
  add_custom_target(mission-install COMMAND $(MAKE) install)
  add_custom_target(mission-clean COMMAND $(MAKE) clean)
  add_custom_target(mission-prebuild)

  # Locate the source location for all the apps found within the target file
  # This is done by searching through the list of paths to find a matching name
  # The environment variable is cached so it will be retained across runs.
  set(CFS_APP_PATH "$ENV{CFS_APP_PATH}"
     CACHE STRING "Extra search path for code modules"
  )
  string(REPLACE ":" ";" CFS_APP_PATH "${CFS_APP_PATH}")
  set(MISSION_MODULE_SEARCH_PATH ${CFS_APP_PATH} ${MISSION_MODULE_SEARCH_PATH})

  # The "MISSION_DEPS" list is the complete set of all dependencies used in the build.
  # This reflects all modules for all CPUs.  It is set as a usage convenience
  # for iterating through the full set of dependencies regardless of which level
  # or context each dependency relates to (CFE, PSP, apps, etc).
  set(MISSION_DEPS ${MISSION_CORE_INTERFACES} ${MISSION_APPS} ${MISSION_CORE_MODULES} ${MISSION_PSPMODULES})
  set(APP_MISSING_COUNT 0)

  message(STATUS "Search path for modules: ${MISSION_MODULE_SEARCH_PATH}")

  # Now search for the rest of CFS applications/libraries/modules - these may exist in
  # any directory within the search path.
  foreach(APP ${MISSION_DEPS})
    set (APPFOUND FALSE)
    foreach(APPSRC ${MISSION_MODULE_SEARCH_PATH} ${${APP}_SEARCH_PATH})
      if (NOT IS_ABSOLUTE "${APPSRC}")
        set(APPSRC "${MISSION_SOURCE_DIR}/${APPSRC}")
      endif()
      if(IS_DIRECTORY "${APPSRC}/${APP}")
        set(APPFOUND "${APPSRC}/${APP}")
        break()
      endif()
    endforeach()
    if (APPFOUND)
      get_filename_component(${APP}_MISSION_DIR "${APPFOUND}" ABSOLUTE)
      message(STATUS "Module '${APP}' found at ${${APP}_MISSION_DIR}")
    else()
      message("** Module ${APP} NOT found **")
      math(EXPR APP_MISSING_COUNT "${APP_MISSING_COUNT} + 1")
    endif()
  endforeach()

  if (APP_MISSING_COUNT GREATER 0)
    message(FATAL_ERROR "Target build incomplete, source for ${APP_MISSING_COUNT} module(s) not found.")
  endif (APP_MISSING_COUNT GREATER 0)

  # Export the full set of dependencies to the parent build
  # including the individual dependency paths to each component
  set(MISSION_DEPS ${MISSION_DEPS} PARENT_SCOPE)
  foreach(DEP ${MISSION_DEPS})
     set(${DEP}_MISSION_DIR ${${DEP}_MISSION_DIR} PARENT_SCOPE)
  endforeach(DEP ${MISSION_DEPS})

  # Doxygen-based documentation generation targets
  # Create a directory for documentation output
  file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/docs")

  # Generate a customized Doxyfile file for the Doxygen docs.
  # This file must be present in the directory where "doxygen" is executed
  # If the user has provided a "Doxyfile" in their top level documentation directory,
  # then assume they have also set PROJECT_NAME and PROJECT_BRIEF in that.
  # Otherwise, generate reasonable strings for these values.
  set(DETAILDESIGN_DOXYFILE_USER_CONTENT)
  if (EXISTS "${MISSION_SOURCE_DIR}/docs/Doxyfile")
    list(APPEND DETAILDESIGN_DOXYFILE_USER_CONTENT "@INCLUDE = ${MISSION_SOURCE_DIR}/docs/Doxyfile\n")
  endif (EXISTS "${MISSION_SOURCE_DIR}/docs/Doxyfile")

  # Add any detail design information from each app/module
  foreach(APP ${MISSION_DEPS})

    set(APP_DOC_DIR "${${APP}_MISSION_DIR}/docs")

    # Now add the app-specific documentation to the detail design input set.
    # This depends on whether the app has provided any guidance on this.
    # The "doxyfile.in" should provide specific directives for what to include.  If
    # not specified, then the entire app source tree will be used as a catch-all.
    if (EXISTS "${APP_DOC_DIR}/${APP}-detaildesign.doxyfile.in")

      # a "doxyfile.in" is a template that should be passed through configure_file()
      # (which will expand any variable refs) then the result of this is included in the doxyfile
      configure_file("${APP_DOC_DIR}/${APP}-detaildesign.doxyfile.in"
        "${CMAKE_BINARY_DIR}/docs/${APP}-detaildesign.doxyfile")

      list(APPEND DETAILDESIGN_DOXYFILE_USER_CONTENT "@INCLUDE = ${CMAKE_BINARY_DIR}/docs/${APP}-detaildesign.doxyfile\n")
    else()

      # Otherwise just add this entire directory to the "INPUT" list
      list(APPEND DETAILDESIGN_DOXYFILE_USER_CONTENT "INPUT += ${${APP}_MISSION_DIR}\n")
    endif()

  endforeach()

  set(TGTSYSTEM_DOXYFILE_USER_CONTENT)
  foreach(SYSVAR ${TGTSYS_LIST})
    list(APPEND TGTSYSTEM_DOXYFILE_USER_CONTENT "@INCLUDE = ${CMAKE_BINARY_DIR}/docs/tgtsystem-content-${SYSVAR}.doxyfile\n")
  endforeach()

  # Create single/unified strings from the lists
  string(CONCAT DETAILDESIGN_DOXYFILE_USER_CONTENT ${DETAILDESIGN_DOXYFILE_USER_CONTENT})
  string(CONCAT TGTSYSTEM_DOXYFILE_USER_CONTENT ${TGTSYSTEM_DOXYFILE_USER_CONTENT})

  configure_file("${CFE_SOURCE_DIR}/cmake/cfe-common.doxyfile.in"
    "${CMAKE_BINARY_DIR}/docs/cfe-common.doxyfile"
    @ONLY)

  # Generate an "empty" osconfig.h file for doxygen purposes
  # this does not have the actual user-defined values, but will
  # have the documentation associated with each macro definition.
  configure_file("${osal_MISSION_DIR}/osconfig.h.in"
    "${CMAKE_BINARY_DIR}/docs/osconfig-example.h")

  # The user guide should include the doxygen from the _public_ API files from CFE + OSAL
  # NOTE: the userguide is built against the headers of the default core apps. Even if
  # an alternate version of the module is in use, it should adhere to the same interface.
  set(SUBMODULE_HEADER_PATHS
    "${osal_MISSION_DIR}/src/os/inc/*.h"
    "${psp_MISSION_DIR}/psp/fsw/inc/*.h"
  )
  foreach(MODULE core_api ${MISSION_CORE_MODULES})
    list(APPEND SUBMODULE_HEADER_PATHS "${${MODULE}_MISSION_DIR}/fsw/inc/*.h")
  endforeach()
  file(GLOB MISSION_USERGUIDE_HEADERFILES
    ${SUBMODULE_HEADER_PATHS}
    "${CMAKE_BINARY_DIR}/docs/osconfig-example.h"
  )

  string(REPLACE ";" " \\\n" MISSION_USERGUIDE_HEADERFILES "${MISSION_USERGUIDE_HEADERFILES}")

  file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/docs/detaildesign")
  configure_file("${CFE_SOURCE_DIR}/cmake/mission-detaildesign.doxyfile.in"
    "${CMAKE_BINARY_DIR}/docs/detaildesign/Doxyfile"
    @ONLY)
  add_custom_target(mission-doc doxygen
    COMMAND echo "Detail Design: file://${CMAKE_BINARY_DIR}/docs/detaildesign/html/index.html"
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/docs/detaildesign")

  file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/docs/users_guide")
  configure_file("${CFE_SOURCE_DIR}/cmake/cfe-usersguide.doxyfile.in"
    "${CMAKE_BINARY_DIR}/docs/users_guide/Doxyfile"
    @ONLY)
  add_custom_target(cfe-usersguide doxygen
    COMMAND echo "Users Guide: file://${CMAKE_BINARY_DIR}/docs/users_guide/html/index.html"
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/docs/users_guide")

  # OSAL API GUIDE include PUBLIC API
  set(OSAL_API_INCLUDE_DIRECTORIES
    "${osal_MISSION_DIR}/src/os/inc"
    "${CMAKE_BINARY_DIR}/docs"
  )
  add_subdirectory(${osal_MISSION_DIR}/docs/src ${CMAKE_BINARY_DIR}/docs/osalguide)
  add_custom_target(osalguide)
  add_dependencies(osalguide osal-apiguide)

  # Pull in any application-specific mission-scope configuration
  # This may include user configuration files such as cfe_mission_cfg.h,
  # msgid definitions, or any other configuration/preparation that needs to
  # happen at mission/global scope.
  foreach(DEP_NAME ${MISSION_DEPS})
    include("${${DEP_NAME}_MISSION_DIR}/mission_build.cmake" OPTIONAL)
  endforeach(DEP_NAME ${MISSION_DEPS})

  # Certain runtime variables need to be "exported" to the subordinate build, such as
  # the specific arch settings and the location of all the apps.  This is done by creating
  # a temporary file within the dir and then the subprocess will read that file and re-create
  # variables out of them.  The alternative to this is to specify many "-D" parameters to the
  # subordinate build but that would not scale well to many vars.
  set(VARLIST
    "MISSION_NAME"
    "SIMULATION"
    "MISSION_DEFS"
    "MISSION_SOURCE_DIR"
    "MISSION_BINARY_DIR"
    "MISSIONCONFIG"
    "MISSION_APPS"
    "MISSION_PSPMODULES"
    "MISSION_DEPS"
    "ENABLE_UNIT_TESTS"
  )
  foreach(APP ${MISSION_DEPS})
    list(APPEND VARLIST "${APP}_MISSION_DIR")
  endforeach(APP ${MISSION_APPS})

  foreach(SYSVAR ${TGTSYS_LIST})
    list(APPEND VARLIST "BUILD_CONFIG_${SYSVAR}")
  endforeach(SYSVAR ${TGTSYS_LIST})

  set(MISSION_VARCACHE)
  foreach(VARL ${VARLIST})
    # It is important to avoid putting any blank lines in the output,
    # This will cause the reader to misinterpret the data
    if (NOT "${${VARL}}" STREQUAL "")
      set(MISSION_VARCACHE "${MISSION_VARCACHE}${VARL}\n${${VARL}}\n")
    endif (NOT "${${VARL}}" STREQUAL "")
  endforeach(VARL ${VARLIST})
  file(WRITE "${CMAKE_BINARY_DIR}/mission_vars.cache" "${MISSION_VARCACHE}")

  generate_build_version_templates()

  # Generate the tools for the native (host) arch
  # Add all public include dirs for core components to include path for tools
  include_directories(
    ${core_api_MISSION_DIR}/fsw/inc
    ${osal_MISSION_DIR}/src/os/inc
    ${psp_MISSION_DIR}/psp/fsw/inc
  )
  add_subdirectory(${MISSION_SOURCE_DIR}/tools tools)

  # Add a dependency on the table generator tool as this is required for table builds
  # The "elf2cfetbl" target should have been added by the "tools" above
  add_dependencies(mission-prebuild elf2cfetbl)

  # Build version information should be generated as part of the pre-build process
  add_dependencies(mission-prebuild mission-version)

  # Install the functional test code into the host directory
  if (IS_DIRECTORY ${MISSION_DEFS}/functional-test AND DEFINED FT_INSTALL_SUBDIR)
    install(DIRECTORY ${MISSION_DEFS}/functional-test/ DESTINATION ${FT_INSTALL_SUBDIR})
  endif()

endfunction(prepare)

##################################################################
#
# FUNCTION: process_arch
#
# Called by the top-level CMakeLists.txt to set up targets for this arch
# This is where the real work is done
#
function(process_arch TARGETSYSTEM)

  # The "BUILD_CONFIG" is a list of items to uniquely identify this build
  # The first element in the list is the toolchain name, followed by config name(s)
  set(BUILD_CONFIG ${BUILD_CONFIG_${TARGETSYSTEM}})
  list(GET BUILD_CONFIG 0 ARCH_TOOLCHAIN_NAME)
  list(REMOVE_AT BUILD_CONFIG 0)
  # convert to a string which is safe for a directory name
  string(REGEX REPLACE "[^A-Za-z0-9]" "_" ARCH_CONFIG_NAME "${BUILD_CONFIG}")
  set(ARCH_BINARY_DIR "${CMAKE_BINARY_DIR}/${ARCH_TOOLCHAIN_NAME}/${ARCH_CONFIG_NAME}")
  file(MAKE_DIRECTORY "${ARCH_BINARY_DIR}" "${ARCH_BINARY_DIR}/inc")

  message(STATUS "Configuring for system arch: ${ARCH_TOOLCHAIN_NAME}/${ARCH_CONFIG_NAME}")

  # Note - A warning is issued if you pass CMAKE_TOOLCHAIN_FILE to an already-configured build area
  # so an extra check is added to see if this is an initial run or a re-run by checking for a CMakeCache file.
  if (NOT ARCH_TOOLCHAIN_NAME STREQUAL "native" AND NOT EXISTS "${ARCH_BINARY_DIR}/CMakeCache.txt")
    # Find the toolchain file - allow a file in the mission defs dir to supersede one in the compile dir
    if (EXISTS "${MISSION_DEFS}/toolchain-${ARCH_TOOLCHAIN_NAME}.cmake")
      set(TOOLCHAIN_FILE "${MISSION_DEFS}/toolchain-${ARCH_TOOLCHAIN_NAME}.cmake")
    elseif(EXISTS "${CFE_SOURCE_DIR}/cmake/toolchain-${ARCH_TOOLCHAIN_NAME}.cmake")
      set(TOOLCHAIN_FILE "${CFE_SOURCE_DIR}/cmake/toolchain-${ARCH_TOOLCHAIN_NAME}.cmake")
    else()
      message(FATAL_ERROR "Unable to find toolchain file for ${ARCH_TOOLCHAIN_NAME}")
    endif()
    set(SELECTED_TOOLCHAIN_FILE "-DCMAKE_TOOLCHAIN_FILE=${TOOLCHAIN_FILE}")
  else()
    # Do not supply any toolchain file option to the subprocess
    set(SELECTED_TOOLCHAIN_FILE)
  endif ()

  # Execute CMake subprocess to create a binary build tree for the specific CPU architecture
  execute_process(
    COMMAND ${CMAKE_COMMAND}
        -G "Unix Makefiles"
        -DTARGETSYSTEM=${TARGETSYSTEM}
        -DMISSION_BINARY_DIR=${MISSION_BINARY_DIR}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        ${SELECTED_TOOLCHAIN_FILE}
        ${CFE_SOURCE_DIR}
    WORKING_DIRECTORY
        "${ARCH_BINARY_DIR}"
    RESULT_VARIABLE
        RESULT
  )
  if (NOT RESULT EQUAL 0)
    message(FATAL_ERROR "Failed to configure ${TARGETSYSTEM}")
  endif (NOT RESULT EQUAL 0)

  # Hook the "make all", "make clean", and "make install" targets for the subordinate build
  # to top-level build targets prefixed by the CPU architecture.
  add_custom_target(${TARGETSYSTEM}-all
   COMMAND
      $(MAKE) all
   WORKING_DIRECTORY
      "${ARCH_BINARY_DIR}"
  )
  add_custom_target(${TARGETSYSTEM}-clean
   COMMAND
      $(MAKE) clean
   WORKING_DIRECTORY
      "${ARCH_BINARY_DIR}"
  )
  add_custom_target(${TARGETSYSTEM}-install
   COMMAND
      $(MAKE) install
   WORKING_DIRECTORY
      "${ARCH_BINARY_DIR}"
  )

  # All subordinate builds depend on the generated files being present first
  add_dependencies(${TARGETSYSTEM}-install mission-prebuild)
  add_dependencies(${TARGETSYSTEM}-all mission-prebuild)

  add_dependencies(mission-all ${TARGETSYSTEM}-all)
  add_dependencies(mission-clean ${TARGETSYSTEM}-clean)
  add_dependencies(mission-install ${TARGETSYSTEM}-install)

endfunction(process_arch TARGETSYSTEM)


