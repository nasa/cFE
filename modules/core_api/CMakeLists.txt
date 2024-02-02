##################################################################
#
# cFE Global/Public Application Interface
#
##################################################################

# NOTE: This module is shared headers and configuration only, it has no
# source files of its own.  It just defines the interfaces between the CFE core
# modules and other private headers that define CFE internal shared data objects.
add_library(core_api INTERFACE)

# The fsw/inc here defines global/shared structures and interfaces
target_include_directories(core_api INTERFACE fsw/inc)

# Propagate the setting to a C preprocessor define of the same name
# The CFE_EDS_ENABLED_BUILD switch indicates that any
# compile-time preprocessor blocks should be enabled in this build
if (CFE_EDS_ENABLED_BUILD)
    target_compile_definitions(core_api INTERFACE CFE_EDS_ENABLED_BUILD)
endif()

# Propagate any INTERFACE-level include dirs and compile definitions from
# the modules into this abstract interface target
foreach(MOD ${MISSION_CORE_MODULES})
    target_include_directories(core_api INTERFACE
        $<TARGET_PROPERTY:${MOD},INTERFACE_INCLUDE_DIRECTORIES>
    )
    target_compile_definitions(core_api INTERFACE
        $<TARGET_PROPERTY:${MOD},INTERFACE_COMPILE_DEFINITIONS>
    )
endforeach(MOD ${MISSION_CORE_MODULES})

# Add unit test coverage subdirectory
# This provides stubs for functions declared in fsw/inc
if (ENABLE_UNIT_TESTS)
  add_subdirectory(ut-stubs)
endif (ENABLE_UNIT_TESTS)

cfs_app_check_intf(core_api
    cfe_resourceid.h
    cfe_endian.h
    cfe_msg.h
    cfe_error.h
    cfe.h
    cfe_version.h

    cfe_es_extern_typedefs.h
    cfe_evs_extern_typedefs.h
    cfe_fs_extern_typedefs.h
    cfe_sb_extern_typedefs.h
    cfe_tbl_extern_typedefs.h
    cfe_time_extern_typedefs.h

    cfe_tbl_filedef.h
)
