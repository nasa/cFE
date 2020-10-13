##################################################################
#
# cFS executable target recipe
#
# This CMake script integrates the CFE core, PSP, and selected CFS 
# apps into a final executable target.
#
# No additional source code is built here, it only serves as a final build 
# stage to link the libraries together.
#
##################################################################

project(CFETARGET C)

# Sanity check on inputs - these should be set in the parent script(s)
if (NOT DEFINED TGTNAME)
  message(FATAL_ERROR "TGTNAME must be defined to link a final exe")
endif (NOT DEFINED TGTNAME)

if (NOT DEFINED ${TGTNAME}_PROCESSORID)
  message(FATAL_ERROR "${TGTNAME}_PROCESSORID must be defined to link a final exe")
endif (NOT DEFINED ${TGTNAME}_PROCESSORID)

# Create a file for the statically-linked module list for this target
# do this for both PSP and CFS static modules
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/psp_module_list.inc.tmp" 
    "/* Automatically generated based on target config */\n")
foreach(PSPMOD ${${TGTNAME}_PSP_MODULELIST})
  file(APPEND "${CMAKE_CURRENT_BINARY_DIR}/psp_module_list.inc.tmp" "LOAD_PSP_MODULE(${PSPMOD})\n")
endforeach(PSPMOD ${${TGTNAME}_PSP_MODULELIST})
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different 
    "${CMAKE_CURRENT_BINARY_DIR}/psp_module_list.inc.tmp" 
    "${CMAKE_CURRENT_BINARY_DIR}/psp_module_list.inc")
file (REMOVE "${CMAKE_CURRENT_BINARY_DIR}/psp_module_list.inc.tmp")

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/cfs_static_symbol_list.inc.tmp" 
    "/* Automatically generated based on target config */\n")
foreach(CFSSYM ${${TGTNAME}_STATIC_SYMLIST})
  file(APPEND "${CMAKE_CURRENT_BINARY_DIR}/cfs_static_symbol_list.inc.tmp" "STATIC_CFS_SYMBOL(${CFSSYM})\n")
endforeach(CFSSYM ${${TGTNAME}_STATIC_SYMLIST})
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different 
    "${CMAKE_CURRENT_BINARY_DIR}/cfs_static_symbol_list.inc.tmp" 
    "${CMAKE_CURRENT_BINARY_DIR}/cfs_static_symbol_list.inc")
file (REMOVE "${CMAKE_CURRENT_BINARY_DIR}/cfs_static_symbol_list.inc.tmp")

# Target for the final executable
add_executable(core-${TGTNAME} src/target_config.c)

target_compile_definitions(core-${TGTNAME} PRIVATE 
    CFE_DEFAULT_MODULE_EXTENSION="${CMAKE_SHARED_MODULE_SUFFIX}"
    CFE_DEFAULT_CORE_FILENAME="$<TARGET_FILE_NAME:core-${TGTNAME}>"
    CFE_CPU_NAME_VALUE="${TGTNAME}"
    CFE_SPACECRAFT_ID_VALUE=${SPACECRAFT_ID}
    CFE_CPU_ID_VALUE=${${TGTNAME}_PROCESSORID}
)

target_include_directories(core-${TGTNAME} PRIVATE 
    "${CMAKE_CURRENT_BINARY_DIR}"
    "${CMAKE_BINARY_DIR}/${CFE_CORE_TARGET}/inc"
)

# This next section provides a method for adding the "-u" switch to the 
# linker in order to make sure the linker includes certain symbols in the link.
# This problem may exist for the entry point or other symbols if the 
# linker doesn't know that they are unresolved when it links those files.
set (TARGET_LINK_FLAGS)
foreach(SYM ${CFE_ENTRY_SYM})
  set (TARGET_LINK_FLAGS "${TARGET_LINK_FLAGS} -u ${SYM}")
endforeach()

set_target_properties(core-${TGTNAME} PROPERTIES LINK_FLAGS "${TARGET_LINK_FLAGS}")

# We may need to use the "--whole-archive" flags to CFE, OSAL, and PSP to ensure that ALL
# symbols make their way into the final executable.  The "--enable-exports" 
# (enabled by the ENABLE_EXPORTS property) does not apply to static libs on the link line
# This is only a concern when relying on the dynamic module loader, if we are statically
# linking the entire CFE system into a single binary then no special help is needed.
if (${TGTNAME}_APPLIST)

  set_target_properties(core-${TGTNAME} PROPERTIES ENABLE_EXPORTS TRUE)
  
  if ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
    # The option pair for GNU gcc/ld tools
    set(START_WHOLE_ARCHIVE "--whole-archive")
    set(STOP_WHOLE_ARCHIVE  "--no-whole-archive")
    
    # the linker option prefix may or may not be needed, see below
    set(COMPILER_LINKER_OPTION_PREFIX "-Wl,")  
  else()
    # Other toolchain options may be added here
  endif()

  # Determine if a pass-through prefix is needed for a linker option.
  # This prefix is required if the link is invoked through gcc,
  # but will fail if calling the linker directory 
  # Unfortunately there is no way to directly get this info,
  # but the platform should set CMAKE_EXE_EXPORTS_C_FLAG
  # for the ENABLE_EXPORTS property, so this just checks
  # if this starts with -Wl, and if so, use it here too.
  if (DEFINED COMPILER_LINKER_OPTION_PREFIX AND 
        "${CMAKE_EXE_EXPORTS_C_FLAG}" MATCHES "^${COMPILER_LINKER_OPTION_PREFIX}")
    set(START_WHOLE_ARCHIVE "${COMPILER_LINKER_OPTION_PREFIX}${START_WHOLE_ARCHIVE}")
    set(STOP_WHOLE_ARCHIVE "${COMPILER_LINKER_OPTION_PREFIX}${STOP_WHOLE_ARCHIVE}")
  endif()
                
endif (${TGTNAME}_APPLIST)

# Collect any additional libraries that should be included on the link line
# This depends on whether any special features are included or not
set(CFE_LINK_WHOLE_LIBS
    ${MISSION_CORE_MODULES}
)
set(CFE_LINK_NORMAL_LIBS 
    ${${TGTNAME}_PSP_MODULELIST}
    ${${TGTNAME}_STATIC_APPLIST}
)

# Handle the list of "embedded files" that should be linked into CFE.
# These are arbitrary files in the mission config that are converted
# into C data structures and linked with the executable.  This is 
# a helpful feature for use when statically linking the CFE.
if (DEFINED ${TGTNAME}_EMBED_FILELIST)

  set(EMBFILE_GENSRC_LIST)  # a list of C source files to compile
  set(EMBFILE_CONTENT_LIST) # a list of content targets for dependencies
  file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/embed") # work area
    
  # Loop through each entry, which should be of the form:
  #    VARIABLE_NAME,FILE_NAME
  # Where "VARIABLE_NAME" indicates the name to use for the 
  # generated C data structure, and FILE_NAME is the data file
  # that it is sourced from (no paths, it will be searched).
  foreach(LISTENT ${${TGTNAME}_EMBED_FILELIST})
    string(REPLACE "," ";" LISTENT ${LISTENT}) # split on the comma
    list(GET LISTENT 0 EMBNAME)  # EMBNAME => C variable name
    list(GET LISTENT 1 EMBFILE)  # EMBFILE => File name
     
    # Find the real source of the file and store in FILESRC
    if(EXISTS ${MISSION_DEFS}/${TGTNAME}_${EMBFILE})
      set(FILESRC ${MISSION_DEFS}/${TGTNAME}_${EMBFILE})
    elseif(EXISTS ${MISSION_DEFS}/${EMBFILE})
      set(FILESRC ${MISSION_DEFS}/${EMBFILE})
    else()
      message(FATAL_ERROR "ERROR: Embed file ${EMBFILE} for ${TGTNAME} not found")
    endif()

    # Use the "xxd" utility to convert the binary file to a C array
    # Note that this pipes the original file to stdin of xxd, so it
    # does not create a usable C file, just array content.
    add_custom_command(
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/embed/${EMBNAME}.inc"
        COMMAND xxd -i < "${FILESRC}" > "${CMAKE_CURRENT_BINARY_DIR}/embed/${EMBNAME}.inc"
        DEPENDS "${FILESRC}")

    # Create a ".c" file that wraps the binary file, using the
    # configure_file() command.  This is done this way (rather than
    # using xxd entirely) for two reasons:
    #  a) it can use our preferred VARIABLE_NAME
    #  b) it can be qualified as "const"
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/embed/${EMBNAME}.c"
            "const unsigned char ${EMBNAME}_DATA[] = {\n"
            "#include \"${EMBNAME}.inc\"\n"
            "};\n"
            "const unsigned long ${EMBNAME}_SIZE = sizeof(${EMBNAME}_DATA);\n")
            
    list(APPEND EMBFILE_GENSRC_LIST "${CMAKE_CURRENT_BINARY_DIR}/embed/${EMBNAME}.c")
    list(APPEND EMBFILE_CONTENT_LIST "${CMAKE_CURRENT_BINARY_DIR}/embed/${EMBNAME}.inc")
       
  endforeach(LISTENT ${${TGTNAME}_EMBED_FILELIST})

  # Finally, generate a static library that contains all embeded binary files
  # and add this to the list of libraries that the CFE will be linked with.
  # add a dependency so that the "inc" files are regenerated and the library
  # is rebuilt before the static library is built.
  add_library(${TGTNAME}_embed_files STATIC ${EMBFILE_GENSRC_LIST})
  add_custom_target(${TGTNAME}_embed_content DEPENDS ${EMBFILE_CONTENT_LIST})
  add_dependencies(${TGTNAME}_embed_files ${TGTNAME}_embed_content)
  list(APPEND CFE_LINK_NORMAL_LIBS ${TGTNAME}_embed_files)
  
endif (DEFINED ${TGTNAME}_EMBED_FILELIST)    

target_link_libraries(core-${TGTNAME}
    # The following libs should be included whole, even if they
    # do not necessarily resolve any symbols, because they may be needed by apps 
    ${START_WHOLE_ARCHIVE} 
    ${CFE_LINK_WHOLE_LIBS}
    ${STOP_WHOLE_ARCHIVE}
    
    # The remainder are static libraries that should only be 
    # included if they resolve an undefined symbol (normal link logic)
    ${CFE_LINK_NORMAL_LIBS}
    ${OSAL_LINK_LIBS}
)

# Install the final executable
# This is implemented in a separate function so
# it may be overridden in an OS-specific manner if necessary.
cfe_exec_do_install(${TGTNAME})

