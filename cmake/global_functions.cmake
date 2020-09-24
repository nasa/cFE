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
#   FILE_NAME - the name of the file to write
#   FALLBACK_FILE - if no files are found in "defs" using the name match, this file will be used instead.
#   MATCH_SUFFIX - the suffix to match in the "defs" directory (optional)
#   PREFIXES - a list of prefixes to match in the "defs" directory (optional)
#
function(generate_config_includefile)

    cmake_parse_arguments(GENCONFIG_ARG "" "OUTPUT_DIRECTORY;FILE_NAME;FALLBACK_FILE;MATCH_SUFFIX" "PREFIXES" ${ARGN} )
    if (NOT GENCONFIG_ARG_OUTPUT_DIRECTORY)
        set(GENCONFIG_ARG_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/inc")
    endif (NOT GENCONFIG_ARG_OUTPUT_DIRECTORY)
    
    set(WRAPPER_FILE_CONTENT)
    set(ITEM_FOUND FALSE)
    
    # Assemble a list of file names to test for
    # Check for existence of a file in defs directory with an exact matching name
    # Then Check for existence of file(s) with a matching prefix+suffix
    set(CHECK_PATH_LIST "${MISSION_DEFS}/${GENCONFIG_ARG_FILE_NAME}")
    if (GENCONFIG_ARG_MATCH_SUFFIX)
        foreach(PREFIX ${GENCONFIG_ARG_PREFIXES} ${GENCONFIG_ARG_UNPARSED_ARGUMENTS})
            list(APPEND CHECK_PATH_LIST "${MISSION_DEFS}/${PREFIX}_${GENCONFIG_ARG_MATCH_SUFFIX}")
        endforeach()
    endif(GENCONFIG_ARG_MATCH_SUFFIX)
    
    # Check for existence of files, and add to content if present
    # Note that all files are appended/concatenated together.
    foreach(SRC_LOCAL_PATH ${CHECK_PATH_LIST})
        if (EXISTS "${SRC_LOCAL_PATH}")
            file(TO_NATIVE_PATH "${SRC_LOCAL_PATH}" SRC_NATIVE_PATH)
            list(APPEND WRAPPER_FILE_CONTENT "#include \"${SRC_NATIVE_PATH}\"\n")
            set(ITEM_FOUND TRUE)
        else()
            list(APPEND WRAPPER_FILE_CONTENT "/* ${SRC_LOCAL_PATH} does not exist */\n")
        endif (EXISTS "${SRC_LOCAL_PATH}")
    endforeach()

    # If _no_ files were found in the above loop,
    # then check for and use the fallback file. 
    # (if specified by the caller it should always exist)
    # Also produce a message on the console showing whether mission config or fallback was used
    if (ITEM_FOUND)
        message(STATUS "Generated ${GENCONFIG_ARG_FILE_NAME} from ${MISSION_DEFS} configuration")
    elseif (GENCONFIG_ARG_FALLBACK_FILE)
        file(TO_NATIVE_PATH "${GENCONFIG_ARG_FALLBACK_FILE}" SRC_NATIVE_PATH)
        list(APPEND WRAPPER_FILE_CONTENT 
            "\n\n/* No configuration for ${GENCONFIG_ARG_FILE_NAME}, using fallback */\n"
            "#include \"${GENCONFIG_ARG_FALLBACK_FILE}\"\n")
        message(STATUS "Using ${GENCONFIG_ARG_FALLBACK_FILE} for ${GENCONFIG_ARG_FILE_NAME}")
    else()
        message("ERROR: No implementation for ${GENCONFIG_ARG_FILE_NAME} found")
        message(FATAL_ERROR "Tested: ${CHECK_PATH_LIST}")
    endif()
    
    # Generate a header file
    generate_c_headerfile("${GENCONFIG_ARG_OUTPUT_DIRECTORY}/${GENCONFIG_ARG_FILE_NAME}" ${WRAPPER_FILE_CONTENT})
  
endfunction(generate_config_includefile)


##################################################################
#
# FUNCTION: read_targetconfig
#
# Scan the list of targets and organize by target system type.
# This function sets up the following variables in the global scope:
#   TGTSYS_LIST: list of CPU architectures used in the build.  Note this
#       will always contain a "native" target (for tools at least) which 
#       is forced to be last.
#   MISSION_APPS: full list of applications specified in the whole mission 
#   SYSID_<arch>: set for each entry of TGTSYS_LIST, and indicates the
#       toolchain specified in the target file for that CPU arch.
#   TGTSYS_<arch>: set to a list of CPU numbers that utilize the same arch 
#   TGTSYS_<arch>_APPS: set for each entry of TGTSYS_LIST, and indicates the
#       full set of applications that need to built for that target architecture
#   TGTSYS_<arch>_DRIVERS: set for each entry of TGTSYS_LIST, and indicates the
#       full set of device drivers that need to built for that target architecture
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
    
    # convert to a the string which is safe for a variable name
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

##################################################################
#
# FUNCTION: get_current_cflags
#
# Convert the input string, which is a simple text string of compiler flags such
# as CMAKE_C_FLAGS or CMAKE_CXX_FLAGS, and convert it to a list of individual options
#
# In addition, the "-I" options from include_directories() and -D options from 
# add_definitions() will be added to the output list.  The contents of these will be
# obtained via the properities of the current source directory. 
#
function(get_current_cflags OUTPUT_LIST)

  # Start by converting the supplied string to a list 
  set(FLAGLIST)
  foreach (FLGSTR ${ARGN})
    string(REGEX REPLACE " +" ";" TEMPFLG ${FLGSTR})
    list(APPEND FLAGLIST ${TEMPFLG})
  endforeach (FLGSTR ${ARGN})
  
  # Append any compile definitions from the directory properties
  get_directory_property(CURRENT_DEFS COMPILE_DEFINITIONS)
  foreach(DEF ${CURRENT_DEFS})
    list(APPEND FLAGLIST "-D${DEF}")
  endforeach(DEF ${CURRENT_DEFS})
  
  # Append any include directories from the directory properties
  get_directory_property(CURRENT_INCDIRS INCLUDE_DIRECTORIES)
  foreach(INC ${CURRENT_INCDIRS})
    list(APPEND FLAGLIST "-I${INC}")
  endforeach(INC ${CURRENT_INCDIRS})

  set(${OUTPUT_LIST} ${FLAGLIST} PARENT_SCOPE)
  
endfunction(get_current_cflags OUTPUT_LIST INPUT_FLAGS)
