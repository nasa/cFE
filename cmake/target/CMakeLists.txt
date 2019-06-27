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

# The CFE core target to link to MUST be given 
if (NOT CFE_CORE_TARGET)
  message(FATAL_ERROR "CFE_CORE_TARGET must be defined to link a final exe")
endif (NOT CFE_CORE_TARGET)

# Create a file for the statically-linked module list for this target
# do this for both PSP and CFS static modules
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/psp_module_list.inc.tmp" 
    "/* Automatically generated based on target config */\n")
foreach(PSPMOD ${TGT${TGTID}_PSP_MODULELIST})
  file(APPEND "${CMAKE_CURRENT_BINARY_DIR}/psp_module_list.inc.tmp" "LOAD_PSP_MODULE(${PSPMOD})\n")
endforeach(PSPMOD ${TGT${TGTID}_PSP_MODULELIST})
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different 
    "${CMAKE_CURRENT_BINARY_DIR}/psp_module_list.inc.tmp" 
    "${CMAKE_CURRENT_BINARY_DIR}/psp_module_list.inc")
file (REMOVE "${CMAKE_CURRENT_BINARY_DIR}/psp_module_list.inc.tmp")

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/cfs_module_list.inc.tmp" 
    "/* Automatically generated based on target config */\n")
foreach(CFSMOD ${TGT${TGTID}_STATIC_APPLIST})
  file(APPEND "${CMAKE_CURRENT_BINARY_DIR}/cfs_module_list.inc.tmp" "LOAD_CFS_MODULE(${CFSMOD})\n")
endforeach(CFSMOD ${TGT${TGTID}_STATIC_APPLIST})
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different 
    "${CMAKE_CURRENT_BINARY_DIR}/cfs_module_list.inc.tmp" 
    "${CMAKE_CURRENT_BINARY_DIR}/cfs_module_list.inc")
file (REMOVE "${CMAKE_CURRENT_BINARY_DIR}/cfs_module_list.inc.tmp")


# Include the table of symbols that should be explicitly linked into the exe
# This file may be empty, but this ensures the file at least exists so the build works.
generate_config_includefile("${CMAKE_CURRENT_BINARY_DIR}/export_symbols.inc" export_symbols.inc ${TGTPLATFORM} ${TGTNAME})

# Include the current binary dir for the driver list
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Include the CFE core-specific configuration
include_directories(${CMAKE_BINARY_DIR}/${CFE_CORE_TARGET}/inc)

# The CPU ID and name are defined by the build scripts for this target
if (DEFINED TGTID)
  add_definitions(-DCFE_CPU_ID_VALUE=${TGTID})
endif()
if (DEFINED SPACECRAFT_ID)
  add_definitions(-DCFE_SPACECRAFT_ID_VALUE=${SPACECRAFT_ID})
endif()
if (DEFINED TGTNAME)
  add_definitions(-DCFE_CPU_NAME_VALUE="${TGTNAME}")
endif()

# Target for the final executable
add_executable(core-${TGTNAME} src/target_config.c)
set_target_properties(core-${TGTNAME} PROPERTIES ENABLE_EXPORTS TRUE)

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
if (NOT TGT${TGTID}_STATIC_LINK)

  if ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
    # The option pair for GNU gcc/ld tools
    set(START_WHOLE_ARCHIVE "-Wl,--whole-archive")
    set(STOP_WHOLE_ARCHIVE  "-Wl,--no-whole-archive")
  else()
    # Other toolchain options may be added here
  endif()
  
endif (NOT TGT${TGTID}_STATIC_LINK)

# Collect any additional libraries that should be included on the link line
# This depends on whether any special features are included or not
set(CFE_LINK_WHOLE_LIBS
    ${CFE_CORE_TARGET}
    psp-${CFE_SYSTEM_PSPNAME} 
    osal
)
set(CFE_LINK_NORMAL_LIBS 
    ${TGT${TGTID}_PSP_MODULELIST} 
    ${TGT${TGTID}_STATIC_APPLIST}
    ${OSAL_LINK_LIBS}
)

target_link_libraries(core-${TGTNAME}
    # The following libs should be included whole, even if they
    # do not necessarily resolve any symbols, because they may be needed by apps 
    ${START_WHOLE_ARCHIVE} 
    ${CFE_LINK_WHOLE_LIBS}
    ${STOP_WHOLE_ARCHIVE}
    
    # The remainder are static libraries that should only be 
    # included if they resolve an undefined symbol (normal link logic)
    ${CFE_LINK_NORMAL_LIBS}
)

install(TARGETS core-${TGTNAME} DESTINATION ${TGTNAME})

# Some target types have extra steps for making a bootable executable
# If so execute them here (defined in the toolchain file if needed) 
if (COMMAND target_add_hook)
  target_add_hook(${TGTNAME})
endif (COMMAND target_add_hook)

