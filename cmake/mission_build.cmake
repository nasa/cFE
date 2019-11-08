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
      get_filename_component(MISSION_SOURCE_DIR "${CMAKE_SOURCE_DIR}/.." ABSOLUTE)
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
# FUNCTION: add_static_dependencies
#
# Adds an entry to the depedency list during app search
# (Note apps can depend on other apps/libs)
#
function(add_static_dependencies TGT_DEPS)

  set(FULLDEPS ${MISSION_DEPS} ${FIND_DEP_LIST})
  set(NEWDEPS)    
  foreach(DEP ${TGT_DEPS} ${ARGN})
    list(FIND FULLDEPS ${DEP} DEPIDX)
    if (DEPIDX LESS 0)
      list(APPEND NEWDEPS ${DEP})
      list(APPEND FULLDEPS ${DEP})
    endif()  
  endforeach()
  
  set(FIND_DEP_LIST ${FIND_DEP_LIST} ${NEWDEPS} PARENT_SCOPE)
  
endfunction(add_static_dependencies)



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

  # Generate the cfe_mission_cfg.h wrapper file  
  generate_config_includefile("inc/cfe_mission_cfg.h" mission_cfg.h ${MISSIONCONFIG})
  generate_config_includefile("inc/cfe_perfids.h" perfids.h ${MISSIONCONFIG} )
  
  # Create custom targets for building and cleaning all architectures
  # This is required particularly for doing extra stuff in the clean step
  add_custom_target(mission-all COMMAND $(MAKE) all)
  add_custom_target(mission-install COMMAND $(MAKE) install)
  add_custom_target(mission-clean COMMAND $(MAKE) clean)
  add_custom_target(mission-prebuild)
  
  # Locate the source location for all the apps found within the target file
  # Each of those may in turn have a "mission_build" file that calls out additional dependencies for that app,
  # so this is run in a loop until the list of unfound apps is empty
  string(REPLACE ":" ";" CFS_APP_PATH "$ENV{CFS_APP_PATH}")
  list(APPEND CFS_APP_PATH "apps" "apps/CFS" "libs" "psp/fsw/modules")
  set(MISSION_DEPS "cfe-core" "osal")
  set(APP_MISSING_COUNT 0)
  
  # Set the search path of those dependency components which are fixed
  # All other components/dependencies are subject to the search path
  # In particular this is OSAL and the CFE core itself
  set(cfe-core_SEARCH_PATH "cfe/fsw")
  set(osal_SEARCH_PATH ".")
  
  # Now search for the rest of CFS applications/libraries/modules - these may exist in
  # a variety of places and also may in turn add more dependencies onto the list.
  # Repeat this loop until the list becomes empty.
  set(FIND_DEP_LIST ${MISSION_APPS} ${MISSION_DEPS} ${MISSION_PSPMODULES})
  while(1)
    set(FIND_DEP_CURRSET ${FIND_DEP_LIST})
    set(FIND_DEP_LIST)
    foreach(APP ${FIND_DEP_CURRSET})
      set (APPFOUND)
      foreach(APPSRC ${${APP}_SEARCH_PATH} ${CFS_APP_PATH})
        if(IS_DIRECTORY ${MISSION_SOURCE_DIR}/${APPSRC}/${APP})
          get_filename_component(APPFOUND "${MISSION_SOURCE_DIR}/${APPSRC}/${APP}" ABSOLUTE)
          break()
        endif()
      endforeach(APPSRC ${CFS_APP_PATH})
      if (APPFOUND)
        set(${APP}_MISSION_DIR ${APPFOUND})
        include(${APPFOUND}/mission_build.cmake OPTIONAL)
        message(STATUS "Module '${APP}' found at ${APPFOUND}")
      else()
        message("** Module ${APP} NOT found **")
        math(EXPR APP_MISSING_COUNT "${APP_MISSING_COUNT} + 1")
      endif()
    endforeach(APP ${FIND_DEP_CURRSET})
    if (NOT FIND_DEP_LIST)
      break()
    endif (NOT FIND_DEP_LIST)
    list(APPEND MISSION_DEPS ${FIND_DEP_LIST})
  endwhile(1)
  
  if (APP_MISSING_COUNT GREATER 0)
    message(FATAL_ERROR "Target build incomplete, source for ${APP_MISSING_COUNT} module(s) not found.")
  endif (APP_MISSING_COUNT GREATER 0)

  # Export the full set of dependencies to the parent build
  # including the individual dependency paths to each component
  set(MISSION_DEPS ${MISSION_DEPS} PARENT_SCOPE)
  foreach(DEP ${MISSION_DEPS} ${MISSION_PSPMODULES})
     set(${DEP}_MISSION_DIR ${${DEP}_MISSION_DIR} PARENT_SCOPE)
  endforeach(DEP ${MISSION_DEPS})
  
  # Doxygen-based documentation generation targets
  # Create a directory for documentation output
  file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/doc")
  
  # Generate a customized Doxyfile file for the Doxygen docs.
  # This file must be present in the directory where "doxygen" is executed
  # If the user has provided a "Doxyfile" in their top level documentation directory,
  # then assume they have also set PROJECT_NAME and PROJECT_BRIEF in that.
  # Otherwise, generate reasonable strings for these values.
  set(MISSION_DOXYFILE_USER_CONTENT)
  if (EXISTS "${MISSION_SOURCE_DIR}/doc/Doxyfile")
    list(APPEND MISSION_DOXYFILE_USER_CONTENT "@INCLUDE = ${MISSION_SOURCE_DIR}/doc/Doxyfile\n")
  endif (EXISTS "${MISSION_SOURCE_DIR}/doc/Doxyfile")
  
  foreach(APP ${MISSION_APPS} ${MISSION_DEPS} ${MISSION_PSPMODULES})
    # OSAL is handled specially, as only part of it is used
    if (NOT APP STREQUAL "osal" AND NOT APP STREQUAL "cfe-core")
      if (EXISTS "${${APP}_MISSION_DIR}/docs/${APP}.doxyfile.in")
        # If the module provides its own doxyfile, then include it directly
        # This allows for app-specific fine-tuning of the sources, based on its own source tree 
        configure_file("${${APP}_MISSION_DIR}/docs/${APP}.doxyfile.in"
            "${CMAKE_BINARY_DIR}/doc/${APP}.doxyfile")
        list(APPEND MISSION_DOXYFILE_USER_CONTENT "@INCLUDE = ${CMAKE_BINARY_DIR}/doc/${APP}.doxyfile\n")
      else()
        # Otherwise just add this entire directory to the "INPUT" list
        list(APPEND MISSION_DOXYFILE_USER_CONTENT "INPUT += ${${APP}_MISSION_DIR}\n")
      endif()
    endif()
  endforeach()
  
  # In all cases it is assumed to include the CFE documentation as well (could be configurable?)  
  file(WRITE "${CMAKE_BINARY_DIR}/doc/mission-content.doxyfile"
      ${MISSION_DOXYFILE_USER_CONTENT})
      
  configure_file("${CMAKE_SOURCE_DIR}/cmake/cfe-common.doxyfile.in"
    "${CMAKE_BINARY_DIR}/doc/cfe-common.doxyfile")
    
  configure_file("${CMAKE_SOURCE_DIR}/cmake/mission-detaildesign.doxyfile.in"
    "${CMAKE_BINARY_DIR}/doc/mission-detaildesign.doxyfile")
    
  # The user guide should include the doxygen from the _public_ API files from CFE + OSAL
  file(GLOB MISSION_USERGUIDE_HEADERFILES 
    "${cfe-core_MISSION_DIR}/src/inc/*.h"
    "${osal_MISSION_DIR}/src/os/inc/*.h"
    "${MISSION_SOURCE_DIR}/psp/fsw/inc/*.h")
  string(REPLACE ";" " \\\n" MISSION_USERGUIDE_HEADERFILES "${MISSION_USERGUIDE_HEADERFILES}") 

  # Addition to usersguide
  file(GLOB USERGUIDE_MISC_ADDITION
       "${cfe-core_MISSION_DIR}/src/inc/private/*.h"
       "${cfe-core_MISSION_DIR}/src/sb/*"
       "${cfe-core_MISSION_DIR}/src/es/*"
       "${cfe-core_MISSION_DIR}/src/evs/*")
  string(REPLACE ";" " \\\n" USERGUIDE_MISC_ADDITION "${USERGUIDE_MISC_ADDITION}")

  # PREDEFINED
  set(USERGUIDE_PREDEFINED 
      "MESSAGE_FORMAT_IS_CCSDS")
 
  configure_file("${CMAKE_SOURCE_DIR}/cmake/cfe-usersguide.doxyfile.in"
    "${CMAKE_BINARY_DIR}/doc/cfe-usersguide.doxyfile")
    
  add_custom_target(mission-doc 
    doxygen mission-detaildesign.doxyfile 
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/doc") 

  add_custom_target(cfe-usersguide 
    doxygen cfe-usersguide.doxyfile 
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/doc") 

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
  foreach(APP ${MISSION_APPS} ${MISSION_PSPMODULES} ${MISSION_DEPS})
    list(APPEND VARLIST "${APP}_MISSION_DIR")
  endforeach(APP ${MISSION_APPS})

  set(MISSION_VARCACHE)
  foreach(VARL ${VARLIST})
    # It is important to avoid putting any blank lines in the output,
    # This will cause the reader to misinterpret the data
    if (NOT "${${VARL}}" STREQUAL "")
      set(MISSION_VARCACHE "${MISSION_VARCACHE}${VARL}\n${${VARL}}\n")
    endif (NOT "${${VARL}}" STREQUAL "")
  endforeach(VARL ${VARLIST})
  file(WRITE "${CMAKE_BINARY_DIR}/mission_vars.cache" "${MISSION_VARCACHE}")
  
  # Generate version information for the executable file.  This is done by executing a small CMAKE
  # at _build_ time  (not at prep time since it might change between now and then) that collects
  # the info out of the version control system in use (git is currently assumed). 
  add_custom_target(mission-version
    COMMAND 
        ${CMAKE_COMMAND} -D BIN=${CMAKE_BINARY_DIR}
                         -P ${CMAKE_SOURCE_DIR}/cmake/version.cmake
    WORKING_DIRECTORY 
      ${CMAKE_SOURCE_DIR}                             
  )

  # Generate the tools for the native (host) arch
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

  set(CURRSYS "${SYSID_${TARGETSYSTEM}}")
  set(ARCH_BINARY_DIR "${CMAKE_BINARY_DIR}/${CURRSYS}")
  file(MAKE_DIRECTORY "${ARCH_BINARY_DIR}")

  message(STATUS "Configuring for system arch: ${CURRSYS}")

  # Note - A warning is issued if you pass CMAKE_TOOLCHAIN_FILE to an already-configured build area
  # so an extra check is added to see if this is an initial run or a re-run by checking for a CMakeCache file.
  if (NOT CURRSYS STREQUAL "native" AND NOT EXISTS "${ARCH_BINARY_DIR}/CMakeCache.txt")
    # Find the toolchain file - allow a file in the mission defs dir to supercede one in the compile dir
    if (EXISTS ${MISSION_DEFS}/toolchain-${CURRSYS}.cmake)
      set(TOOLCHAIN_FILE ${MISSION_DEFS}/toolchain-${CURRSYS}.cmake)
    elseif(EXISTS ${CMAKE_SOURCE_DIR}/cmake/toolchain-${CURRSYS}.cmake)
      set(TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/toolchain-${CURRSYS}.cmake)
    else()
      message(FATAL_ERROR "Unable to find toolchain file for ${CURRSYS}")
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
        ${CMAKE_SOURCE_DIR} 
    WORKING_DIRECTORY 
        "${ARCH_BINARY_DIR}" 
    RESULT_VARIABLE 
        RESULT
  )
  if (NOT RESULT EQUAL 0)
    message(FATAL_ERROR "Failed to configure ${CURRSYS}")
  endif (NOT RESULT EQUAL 0)  
  
  # Hook the "make all", "make clean", and "make install" targets for the subordinate build
  # to top-level build targets prefixed by the CPU architecture.
  add_custom_target(${CURRSYS}-all 
   COMMAND 
      $(MAKE) all 
   WORKING_DIRECTORY 
      "${ARCH_BINARY_DIR}"
  )
  add_custom_target(${CURRSYS}-clean 
   COMMAND 
      $(MAKE) clean 
   WORKING_DIRECTORY 
      "${ARCH_BINARY_DIR}"
  )
  add_custom_target(${CURRSYS}-install 
   COMMAND 
      $(MAKE) install 
   WORKING_DIRECTORY 
      "${ARCH_BINARY_DIR}"
  )

  # All subordinate builds depend on the generated files being present first
  add_dependencies(${CURRSYS}-install mission-prebuild)
  add_dependencies(${CURRSYS}-all mission-prebuild) 

  add_dependencies(mission-all ${CURRSYS}-all)
  add_dependencies(mission-clean ${CURRSYS}-clean)
  add_dependencies(mission-install ${CURRSYS}-install)
  
endfunction(process_arch TARGETSYSTEM)


