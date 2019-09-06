##################################################################
#
# cFE unit test build recipe
#
# This CMake file contains the recipe for building the cFE unit tests.
# It is invoked from the parent directory when unit tests are enabled.
#
##################################################################
 
add_definitions(-DCFE_LINUX)
include_directories(${osal_MISSION_DIR}/ut_assert/inc)

# The parent build may have specified extra C flags for use when unit testing
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${UT_C_FLAGS}")

# Because the object code is dependent on settings in cfe_platform_cfg.h,
# we must include the LIBNAME in all binary targets, as they must be rebuilt
# for each different platform config.
set (STUBNAME stub) 
if (CFE_CORE_TARGET)
  set(STUBNAME "${CFE_CORE_TARGET}_${STUBNAME}")
endif (CFE_CORE_TARGET)

set(STUBFILES configdata osprintf ${CFE_CORE_MODULES})

# Allow direct inclusion of all private CFE header files
foreach(MODULE ${CFE_CORE_MODULES})
  get_filename_component(CFEDIR ${CMAKE_CURRENT_SOURCE_DIR}/../src/${MODULE} ABSOLUTE)
  if (IS_DIRECTORY "${CFEDIR}")
    include_directories(${CFEDIR})
  endif()
endforeach()

set(CFE_MODULE_FILES)
foreach(MODULE ${CFE_CORE_MODULES})
    list(APPEND CFE_MODULE_FILES ut_${MODULE}_stubs.c)
endforeach(MODULE ${CFE_CORE_MODULES})

#
# Create the generic stubs library that ANY other UT target can link to
# to provide stubs for all CFE API calls.
#
add_library(ut_${CFE_CORE_TARGET}_stubs STATIC ${CFE_MODULE_FILES})

# For each core module, generate the associated unit test
# This is done by linking the stubs of every OTHER module with the
# UT version of the real module (compiled with coverage flags)
foreach(MODULE ${CFE_CORE_MODULES})
  set(CFE_MODULE_FILES 
    ut_support.c
    ut_osprintf_stubs.c)
  aux_source_directory(../src/${MODULE} CFE_MODULE_FILES)
  aux_source_directory(../src/shared CFE_MODULE_FILES)
  add_executable(${CFE_CORE_TARGET}_${MODULE}_UT ${MODULE}_UT.c ${CFE_MODULE_FILES})
  target_link_libraries(${CFE_CORE_TARGET}_${MODULE}_UT
        ut_${CFE_CORE_TARGET}_stubs
        ut_psp-${CFE_SYSTEM_PSPNAME}_stubs
        ut_osapi_stubs 
        ut_bsp)
  add_test(${CFE_CORE_TARGET}_${MODULE}_UT ${CFE_CORE_TARGET}_${MODULE}_UT)
  install(TARGETS ${CFE_CORE_TARGET}_${MODULE}_UT DESTINATION ${TGTNAME}/${UT_INSTALL_SUBDIR})
endforeach(MODULE ${CFE_CORE_MODULES})

# Generate the FS test input files
# As these are just arbitrary data, they only have to be present - they do not need to be updated 
execute_process(COMMAND gzip -c ${CMAKE_CURRENT_SOURCE_DIR}/fs_UT.c OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/fs_test.gz)
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/fs_test.nongz "This is an ordinary file\n")


