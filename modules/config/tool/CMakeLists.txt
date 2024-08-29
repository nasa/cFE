##########################################################
#
# CFE Platform Configuration tool
#
# The purpose of this tool is to act as an intermediate step
# to generate platform-specific config item definitions.  This
# is built for the dev host (not cross compiled) and executed
# on the build host during the build to generate objects that
# are then used later during the target (cross) builds.
#
##########################################################

include_directories(${CMAKE_BINARY_DIR}/inc)
include_directories(../fsw/inc)

set(SYM_PREFIX CFECONFIG_PLATFORMDATA)

set(PLATFORM_CONFIG_LIST)
set(PLATFORM_OBJ_NAMES)

foreach(SYSVAR ${TGTSYS_LIST})

  # Decode the SYSVAR value to get the arch-specific build dir
  # (note this gets compiled at mission scope, so CMAKE_BUILD_DIR
  # refers to the top level build, not the cross build)
  decode_targetsystem(${SYSVAR}
    OUTPUT_ARCH_BINARY_DIR  TEMP_ARCH_BINARY_DIR
  )

  set(OBJECT_NAME ${SYM_PREFIX}_${SYSVAR})

  # add an OBJECT library for the platform-specific config data
  # This always uses the same .c file for every platform, but the
  # include path is changed for each one so it gets the appropriate
  # cfe_platform_cfg.h file.
  string(TOLOWER ${OBJECT_NAME} OBJLIB_NAME)
  add_library(${OBJLIB_NAME} OBJECT
    cfeconfig_platformdata_table.c
  )
  target_compile_definitions(${OBJLIB_NAME} PRIVATE
    MAP_ENTRY_LIST=${OBJECT_NAME}
  )
  target_include_directories(${OBJLIB_NAME} PRIVATE
    ${TEMP_ARCH_BINARY_DIR}/inc
  )

  set(PLATFORM_DEFINE_FILE ${MISSION_BINARY_DIR}/src/${OBJLIB_NAME}.c)
  set(PLATFORM_LIST_FILE ${MISSION_BINARY_DIR}/src/${OBJLIB_NAME}.list)
  list(APPEND PLATFORM_CONFIG_LIST $<TARGET_OBJECTS:${OBJLIB_NAME}>)
  string(APPEND PLATFORM_OBJ_NAMES "CFE_PLATFORM(${SYSVAR})\n")

  add_custom_command(
    OUTPUT ${PLATFORM_DEFINE_FILE}
    COMMAND cfeconfig_platformdata_tool initcode ${SYSVAR} > ${PLATFORM_DEFINE_FILE}
    DEPENDS cfeconfig_platformdata_tool
  )

  add_custom_command(
    OUTPUT ${PLATFORM_LIST_FILE}
    COMMAND cfeconfig_platformdata_tool entrylist ${SYSVAR} > ${PLATFORM_LIST_FILE}
    DEPENDS cfeconfig_platformdata_tool
  )

  add_custom_target(cfgtool-execute-${SYSVAR} DEPENDS ${PLATFORM_DEFINE_FILE} ${PLATFORM_LIST_FILE})
  add_dependencies(mission-prebuild cfgtool-execute-${SYSVAR})

endforeach(SYSVAR ${TGTSYS_LIST})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/cfe_platform_list.c.in ${CMAKE_BINARY_DIR}/src/cfe_platform_list.c)

add_executable(cfeconfig_platformdata_tool
  cfeconfig_platformdata_tool.c
  ${CMAKE_BINARY_DIR}/src/cfe_configid_nametable.c
  ${CMAKE_BINARY_DIR}/src/cfe_platform_list.c
  ${PLATFORM_CONFIG_LIST}
)

target_include_directories(cfeconfig_platformdata_tool PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

add_dependencies(mission-prebuild cfeconfig_platformdata_tool)