##################################################################
#
# cFS Mission global CMake function definitions
#
# This is included by the top-level script and can define
# common routines and variables that may be referenced in both
# the mission (non-arch) and arch-specific builds
#
##################################################################

include(CMakeParseArguments)

# This is done here at the global level so this definition is used for
# ALL code on ALL targets, including host-side tools.  Ideally, this should
# only be necessary on the core_api interface, but it does not fully propagate
# to all unit test targets.  If/when that issue is resolved, this can be removed.
if (CFE_EDS_ENABLED_BUILD)

    # Propagate the setting to a C preprocessor define of the same name
    # The CFE_EDS_ENABLED_BUILD switch indicates that any
    # compile-time preprocessor blocks should be enabled in this build
    add_definitions(-DCFE_EDS_ENABLED_BUILD)

endif(CFE_EDS_ENABLED_BUILD)


##################################################################
#
# FUNCTION: cfe_locate_implementation_file
#
# The CFE/CFS build permits users to modify or tune system behavior by
# providing customized versions of configuration files and tables.
#
# This function locates the preferred version of a file to use, by following
# a priority-based search scheme.  Users can put their preferred version of
# a file in their MISSION_DEFS directory, based on a file naming convention.
#
# If no version of the file is found there, a default/fallback version can
# be specified as well, which can be part of the module source.
#
function(cfe_locate_implementation_file OUTPUT_VAR FILE_NAME)

  cmake_parse_arguments(LOCATEIMPL_ARG "OPTIONAL;ALLOW_LIST" "FALLBACK_FILE" "PREFIX;SUBDIR" ${ARGN})

  set(IMPL_SEARCH_BASEDIRS)
  # The prefix could also be a subdir, but do not repeat the mission name
  foreach (PREFIX ${LOCATEIMPL_ARG_PREFIX})
    if (NOT "${MISSIONCONFIG}" STREQUAL "${PREFIX}")
      list(APPEND IMPL_SEARCH_BASEDIRS "${MISSION_DEFS}/${PREFIX}/")
    endif()
  endforeach()
  # Always check for filename matches directly in the MISSION_DEFS dir
  list(APPEND IMPL_SEARCH_BASEDIRS "${MISSION_DEFS}/")
  list(REVERSE IMPL_SEARCH_BASEDIRS)

  set(ADD_SUBDIRS)
  foreach (SUBDIR ${LOCATEIMPL_ARG_SUBDIR})
    foreach (BASEDIR ${IMPL_SEARCH_BASEDIRS})
      list(APPEND ADD_SUBDIRS "${BASEDIR}${SUBDIR}/")
    endforeach()
  endforeach()
  list(APPEND IMPL_SEARCH_BASEDIRS ${ADD_SUBDIRS})
  list(REMOVE_DUPLICATES IMPL_SEARCH_BASEDIRS)

  # Build the list of possible locations for this file in REVERSE priority order
  set(IMPL_SEARCH_PATH)
  # Check for the existence of the file in each of the dirs
  foreach(BASEDIR ${IMPL_SEARCH_BASEDIRS})
    list(APPEND IMPL_SEARCH_PATH "${BASEDIR}${FILE_NAME}")

    # Get relative directory
    string(REPLACE ${MISSION_SOURCE_DIR} "" RELATIVEDIR ${BASEDIR})

    # A target-specific prefixed filename gets priority over a direct filename match
    # But do not include this variant if the prefix is already part of the relative search path
    foreach (PREFIX ${LOCATEIMPL_ARG_PREFIX})
      if (NOT "${RELATIVEDIR}" MATCHES "/${PREFIX}/")
        list(APPEND IMPL_SEARCH_PATH "${BASEDIR}${PREFIX}_${FILE_NAME}")
      endif()
    endforeach()
  endforeach()

  set(SELECTED_FILE)
  foreach (CHECK_FILE ${IMPL_SEARCH_PATH})
    if ($ENV{VERBOSE})
      message("Testing for: ${CHECK_FILE}")
    endif()
    if (EXISTS "${CHECK_FILE}")
      list(APPEND SELECTED_FILE ${CHECK_FILE})
    endif()
  endforeach()

  if (NOT SELECTED_FILE AND LOCATEIMPL_ARG_FALLBACK_FILE)
    if (IS_ABSOLUTE "${LOCATEIMPL_ARG_FALLBACK_FILE}")
      set(SELECTED_FILE "${LOCATEIMPL_ARG_FALLBACK_FILE}")
    else()
      set(SELECTED_FILE "${CMAKE_CURRENT_LIST_DIR}/${LOCATEIMPL_ARG_FALLBACK_FILE}")
    endif()
    if (NOT EXISTS "${SELECTED_FILE}" AND NOT LOCATEIMPL_ARG_OPTIONAL)
      message(FATAL_ERROR "No implementation for ${FILE_NAME} found")
    endif()
  endif()

  if (SELECTED_FILE)
    message(STATUS "Using file: ${SELECTED_FILE} for ${FILE_NAME}")
  endif()

  # Export the result to the caller
  if (NOT LOCATEIMPL_ARG_ALLOW_LIST AND SELECTED_FILE)
    list(GET SELECTED_FILE -1 SELECTED_FILE)
  endif()
  set(${OUTPUT_VAR} ${SELECTED_FILE} PARENT_SCOPE)

endfunction()

##################################################################
#
# FUNCTION: generate_c_headerfile
#
# Generates a C header file in the build directory.
# First argument is the file name to write.  All remaining arguments will be
# concatenated and written to the file.
#
function(generate_c_headerfile FILE_NAME)

    # Determine the absolute location for this wrapper file
    # If it is relative path then assume it is relative to ${CMAKE_BINARY_DIR}
    # This should not write generated files to ${CMAKE_SOURCE_DIR}
    if (NOT IS_ABSOLUTE "${FILE_NAME}")
        set(FILE_NAME "${CMAKE_BINARY_DIR}/${FILE_NAME}")
    endif (NOT IS_ABSOLUTE "${FILE_NAME}")

    # Generate an include guard
    get_filename_component(FILE_GUARD "${FILE_NAME}" NAME)
    string(REGEX REPLACE "[^A-Za-z0-9]" "_" FILE_GUARD "${FILE_GUARD}")
    string(TOUPPER "GENERATED_INCLUDE_${FILE_GUARD}" FILE_GUARD)
    set(GENERATED_FILE_HEADER
        "/* Generated header file.  Do not edit */\n\n"
        "#ifndef ${FILE_GUARD}\n"
        "#define ${FILE_GUARD}\n\n"
    )
    set(GENERATED_FILE_TRAILER
        "#endif /* ${FILE_GUARD} */\n"
    )

    # Use configure_file() to write the file, as this automatically
    # has the logic to not update the timestamp on the file unless it changes.
    string(REPLACE ";" "" GENERATED_FILE_CONTENT "${ARGN}")
    string(REPLACE ";" "" GENERATED_FILE_HEADER "${GENERATED_FILE_HEADER}")
    string(REPLACE ";" "" GENERATED_FILE_TRAILER "${GENERATED_FILE_TRAILER}")
    configure_file(
        "${CFE_SOURCE_DIR}/cmake/cfe_generated_file.h.in"
        "${FILE_NAME}"
        @ONLY)

endfunction(generate_c_headerfile)

##################################################################
#
# FUNCTION: generate_config_includefile
#
# A function to create safe include file wrappers
#
# Rather than symlinking to the include file (which might not work the same on all platforms)
# we can create a build-specific include file that just #include's the real file
#
# This also supports "stacking" multiple component files together by specifying more than one
# source file for the wrapper.
#
# This function now accepts named parameters:
#   OUTPUT_DIRECTORY - non-default directory to write the file to (optional)
#   FILE_NAME - the name of the file to write
#   FALLBACK_FILE - if no files are found in "defs" using the name match, this file will be used instead.
#   MATCH_SUFFIX - the suffix to match in the "defs" directory (optional)
#   PREFIXES - a list of prefixes to match in the "defs" directory (optional)
#
function(generate_config_includefile)

    cmake_parse_arguments(GENCONFIG_ARG "" "OUTPUT_DIRECTORY;FILE_NAME;FALLBACK_FILE;MATCH_SUFFIX" "PREFIXES;GENERATED_FILE" ${ARGN} )
    if (NOT GENCONFIG_ARG_OUTPUT_DIRECTORY)
        set(GENCONFIG_ARG_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/inc")
    endif (NOT GENCONFIG_ARG_OUTPUT_DIRECTORY)

    if (IS_CFS_ARCH_BUILD AND NOT GENCONFIG_ARG_PREFIXES)
      set(ARCH_PREFIXES ${BUILD_CONFIG})
    else()
      set(ARCH_PREFIXES)
    endif()

    if (GENCONFIG_ARG_MATCH_SUFFIX)
      set(TGTFILE ${GENCONFIG_ARG_MATCH_SUFFIX})
    else()
      set(TGTFILE ${GENCONFIG_ARG_FILE_NAME})
    endif()

    if (GENCONFIG_ARG_GENERATED_FILE)
      # A generated file may not yet exist at the time this runs, so just use
      # what the caller said, no searching.
      set(SRC_LOCAL_PATH_LIST ${GENCONFIG_ARG_GENERATED_FILE})
      message(STATUS "Using file: [generated] ${SRC_LOCAL_PATH_LIST} for ${TGTFILE}")
    else()
      # Use the common search function to find the candidate(s)
      cfe_locate_implementation_file(SRC_LOCAL_PATH_LIST "${TGTFILE}"
        FALLBACK_FILE "${GENCONFIG_ARG_FALLBACK_FILE}"
        PREFIX ${GENCONFIG_ARG_PREFIXES} ${ARCH_PREFIXES}
        SUBDIR config
      )
    endif()

    set(WRAPPER_FILE_CONTENT)
    foreach(SELECTED_FILE ${SRC_LOCAL_PATH_LIST})
      file(TO_NATIVE_PATH "${SELECTED_FILE}" SRC_NATIVE_PATH)
      list(APPEND WRAPPER_FILE_CONTENT "#include \"${SRC_NATIVE_PATH}\"\n")
    endforeach()

    # Generate a header file
    generate_c_headerfile("${GENCONFIG_ARG_OUTPUT_DIRECTORY}/${GENCONFIG_ARG_FILE_NAME}" ${WRAPPER_FILE_CONTENT})

endfunction(generate_config_includefile)


##################################################################
#
# FUNCTION: read_targetconfig
#
# Scan the list of targets and organize by target system type.
#
# If the historical TGT<x> variables are defined, they are translated to name
# based list of MISSION_CPUNAMES (from TGT<x>_NAMEs).  The historical settings
# are then translated to the new cpuname based settings as defined in the
# sample_defs/targets.cmake and sets them as global scope.
#
# This function then sets up the following variables in the global scope:
#   TGTSYS_LIST: list of CPU architectures used in the build.  Note this
#       will always contain a "native" target (for tools at least) which
#       is forced to be last.
#   MISSION_APPS: list of all applications in this build
#   MISSION_PSPMODULES: list of all psp modules in this build
#
# Additionally for each architecture in TGTSYS_LIST:
#   TGTSYS_<arch>: list of CPU names that utilize the same architecture
#   TGTSYS_<arch>_APPS: list of apps for the architecture
#   TGTSYS_<arch>_STATICAPPS: list of static apps for the architecture
#   TGTSYS_<arch>_PSPMODULES: list of psp modules for the architecture
#
function(read_targetconfig)

  set(TGTSYS_LIST)
  set(MISSION_APPS)
  set(MISSION_PSPMODULES)

  # This while loop checks for a sequential set of variables prefixed with TGT<x>_
  # where <x> is a sequence number starting with 1.  The first "gap" (undefined name)
  # is treated as the end of list.
  # This is the historical way of specifying CPU configs.  New/future development should
  # prefer the name-based specification. This translates the sequential TGT<x> variable
  # to a name-based variable.
  set(TGTID 0)
  while(1)
    math(EXPR TGTID "${TGTID} + 1")
    if (NOT DEFINED TGT${TGTID}_NAME)
      break()
    endif()
    set(CPUNAME ${TGT${TGTID}_NAME})
    # by default if PROCESSORID isn't specified, then use TGTID number.
    if(NOT DEFINED TGT${TGTID}_PROCESSORID)
        set(TGT${TGTID}_PROCESSORID ${TGTID})
    endif()

    # Translate the TGT<x> prefix to the CPU name prefix
    # also propagate the value to parent scope
    foreach(PROP PROCESSORID
                 APPLIST
                 STATIC_APPLIST
                 STATIC_SYMLIST
                 PSP_MODULELIST
                 FILELIST
                 EMBED_FILELIST
                 SYSTEM
                 PLATFORM)
         set(${CPUNAME}_${PROP} ${TGT${TGTID}_${PROP}})
         set(${CPUNAME}_${PROP} ${${CPUNAME}_${PROP}} PARENT_SCOPE)
    endforeach()
    list(APPEND MISSION_CPUNAMES ${CPUNAME})
  endwhile()

  foreach(CPUNAME ${MISSION_CPUNAMES})
    if (SIMULATION)
      # if simulation use simulation system architecture for all targets
      set(TOOLCHAIN_NAME "${SIMULATION}")
    elseif (${CPUNAME}_SYSTEM)
      # get the target system arch identifier string
      set(TOOLCHAIN_NAME "${${CPUNAME}_SYSTEM}")
    else()
      # assume a toolchain name matching the CPU name
      set(TOOLCHAIN_NAME "${CPUNAME}")
      set(${CPUNAME}_SYSTEM ${TOOLCHAIN_NAME} PARENT_SCOPE)
    endif ()

    if (NOT DEFINED ${CPUNAME}_PLATFORM)
      set(${CPUNAME}_PLATFORM "default" "${CPUNAME}")
      set(${CPUNAME}_PLATFORM "${${CPUNAME}_PLATFORM}" PARENT_SCOPE)
    endif()

    set(BUILD_CONFIG ${TOOLCHAIN_NAME} ${${CPUNAME}_PLATFORM})

    # convert to a string which is safe for a variable name
    string(REGEX REPLACE "[^A-Za-z0-9]" "_" SYSVAR "${BUILD_CONFIG}")

    # save the unmodified name for future reference
    set(BUILD_CONFIG_${SYSVAR} "${BUILD_CONFIG}" PARENT_SCOPE)

    # if the "global" applist is not empty, append to every CPU applist
    if (MISSION_GLOBAL_APPLIST)
      list(APPEND ${CPUNAME}_APPLIST ${MISSION_GLOBAL_APPLIST})
      set(${CPUNAME}_APPLIST ${${CPUNAME}_APPLIST} PARENT_SCOPE)
    endif (MISSION_GLOBAL_APPLIST)

    if (MISSION_GLOBAL_STATIC_APPLIST)
      list(APPEND ${CPUNAME}_STATIC_APPLIST ${MISSION_GLOBAL_STATIC_APPLIST})
      set(${CPUNAME}_STATIC_APPLIST ${${CPUNAME}_STATIC_APPLIST} PARENT_SCOPE)
    endif (MISSION_GLOBAL_STATIC_APPLIST)

    # Append to global lists
    list(APPEND TGTSYS_LIST "${SYSVAR}")
    list(APPEND TGTSYS_${SYSVAR} "${CPUNAME}")
    list(APPEND TGTSYS_${SYSVAR}_APPS ${${CPUNAME}_APPLIST})
    list(APPEND TGTSYS_${SYSVAR}_STATICAPPS ${${CPUNAME}_STATIC_APPLIST})
    list(APPEND TGTSYS_${SYSVAR}_PSPMODULES ${${CPUNAME}_PSP_MODULELIST})
    list(APPEND MISSION_APPS ${${CPUNAME}_APPLIST} ${${CPUNAME}_STATIC_APPLIST})
    list(APPEND MISSION_PSPMODULES ${${CPUNAME}_PSP_MODULELIST})

  endforeach()

  # Remove duplicate entries in the generated lists
  list(REMOVE_DUPLICATES TGTSYS_LIST)
  if (MISSION_APPS)
    list(REMOVE_DUPLICATES MISSION_APPS)
  endif (MISSION_APPS)
  if (MISSION_PSPMODULES)
    list(REMOVE_DUPLICATES MISSION_PSPMODULES)
  endif(MISSION_PSPMODULES)

  # Set the final variable value(s) in the parent scope
  set(TGTSYS_LIST ${TGTSYS_LIST} PARENT_SCOPE)
  set(MISSION_APPS ${MISSION_APPS} PARENT_SCOPE)
  set(MISSION_PSPMODULES ${MISSION_PSPMODULES} PARENT_SCOPE)
  set(MISSION_CPUNAMES ${MISSION_CPUNAMES} PARENT_SCOPE)

  foreach(SYSVAR ${TGTSYS_LIST})
    set(TGTSYS_${SYSVAR} ${TGTSYS_${SYSVAR}} PARENT_SCOPE)
    if(TGTSYS_${SYSVAR}_APPS)
      list(REMOVE_DUPLICATES TGTSYS_${SYSVAR}_APPS)
      set(TGTSYS_${SYSVAR}_APPS ${TGTSYS_${SYSVAR}_APPS} PARENT_SCOPE)
    endif(TGTSYS_${SYSVAR}_APPS)
    if(TGTSYS_${SYSVAR}_STATICAPPS)
      list(REMOVE_DUPLICATES TGTSYS_${SYSVAR}_STATICAPPS)
      set(TGTSYS_${SYSVAR}_STATICAPPS ${TGTSYS_${SYSVAR}_STATICAPPS} PARENT_SCOPE)
    endif(TGTSYS_${SYSVAR}_STATICAPPS)
    if(TGTSYS_${SYSVAR}_PSPMODULES)
      list(REMOVE_DUPLICATES TGTSYS_${SYSVAR}_PSPMODULES)
      set(TGTSYS_${SYSVAR}_PSPMODULES ${TGTSYS_${SYSVAR}_PSPMODULES} PARENT_SCOPE)
    endif(TGTSYS_${SYSVAR}_PSPMODULES)
  endforeach(SYSVAR ${TGTSYS_LIST})

endfunction(read_targetconfig)
