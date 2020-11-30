##################################################################
#
# cFE unit test build recipe
#
# This CMake file contains the recipe for building the cFE unit tests.
# It is invoked from the parent directory when unit tests are enabled.
#
##################################################################
 
include_directories(${osal_MISSION_DIR}/ut_assert/inc)

# allow direct inclusion of module-private header files by UT code
# NOTE: this should be minimized and moved to a more targeted
# approach, where only each specific UT module does this.
include_directories(
      ${cfe-core_MISSION_DIR}/src/es
      ${cfe-core_MISSION_DIR}/src/evs
      ${cfe-core_MISSION_DIR}/src/sb
      ${cfe-core_MISSION_DIR}/src/tbl
      ${cfe-core_MISSION_DIR}/src/time
      ${cfe-core_MISSION_DIR}/src/fs
)

# CFE needs a supplemental test support hook library
add_library(ut_cfe-core_support STATIC
    ut_support.c
    ut_osprintf_stubs.c
)
target_include_directories(ut_cfe-core_support PUBLIC
    ${cfe-core_MISSION_DIR}/src/es
    ${cfe-core_MISSION_DIR}/src/evs
    ${cfe-core_MISSION_DIR}/src/time
    ${CMAKE_CURRENT_SOURCE_DIR})

# For each core module, generate the associated unit test
# This is done by linking the stubs of every OTHER module with the
# UT version of the real module (compiled with coverage flags)
foreach(MODULE ${CFE_CORE_MODULES})

  # The "UT_TARGET_NAME" is a concatenation of the configuration name with the current module
  # This avoids target name duplication in case more than one configuration is used
  # (All  targets should be based on this name)
  set(UT_TARGET_NAME "cfe-core_${MODULE}")
  
  set(CFE_MODULE_FILES)
  aux_source_directory(${cfe-core_MISSION_DIR}/src/${MODULE} CFE_MODULE_FILES)
  
  # Compile the unit(s) under test as an object library
  # this allows easy configuration of special flags and include paths
  # in particular this should use the UT_COVERAGE_COMPILE_FLAGS for coverage instrumentation
  add_library(ut_${UT_TARGET_NAME}_object OBJECT 
      ${CFE_MODULE_FILES})
  
  # Apply the UT_COVERAGE_COMPILE_FLAGS to the units under test
  # This should enable coverage analysis on platforms that support this
  target_compile_options(ut_${UT_TARGET_NAME}_object PRIVATE ${UT_COVERAGE_COMPILE_FLAGS})
        
  # For this object target only, the "override" includes should be injected
  # into the include path BEFORE any other include path.  This is so the
  # override will take precedence over any system-provided version.
  target_include_directories(ut_${UT_TARGET_NAME}_object BEFORE PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/modules/inc/overrides)
        
  add_executable(${UT_TARGET_NAME}_UT 
    ${MODULE}_UT.c 
    $<TARGET_OBJECTS:ut_${UT_TARGET_NAME}_object>)
  
  # Also add the UT_COVERAGE_LINK_FLAGS to the link command    
  # This should enable coverage analysis on platforms that support this
  target_link_libraries(${UT_TARGET_NAME}_UT
        ${UT_COVERAGE_LINK_FLAGS}
        ut_cfe-core_support
        ut_cfe-core_stubs
        sbr  # Will be removed when sbr stubs are implemented
        ut_assert)

  add_test(${UT_TARGET_NAME}_UT ${UT_TARGET_NAME}_UT)
  foreach(TGTNAME ${INSTALL_TARGET_LIST})
      install(TARGETS ${UT_TARGET_NAME}_UT DESTINATION ${TGTNAME}/${UT_INSTALL_SUBDIR})
  endforeach(TGTNAME ${INSTALL_TARGET_LIST})
endforeach(MODULE ${CFE_CORE_MODULES})

# Generate the FS test input files
# As these are just arbitrary data, they only have to be present - they do not need to be updated 
execute_process(COMMAND gzip -c ${CMAKE_CURRENT_SOURCE_DIR}/fs_UT.c OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/fs_test.gz)
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/fs_test.nongz "This is an ordinary file\n")


