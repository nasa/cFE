##################################################################
#
# cFE message module CMake build recipe
#
# This CMakeLists.txt adds source files for
# message module included in the cFE distribution.  Selected
# files are built into a static library that in turn
# is linked into the final executable.
#
# Note this is different than applications which are dynamically
# linked to support runtime loading.  The core applications all
# use static linkage.
#
##################################################################

# Add the basic set of files which are always built
# Defined as absolute so this list can also be used to build unit tests
set(${DEP}_SRC
    fsw/src/cfe_msg_ccsdspri.c
    fsw/src/cfe_msg_init.c
    fsw/src/cfe_msg_integrity.c
    fsw/src/cfe_msg_msgid_shared.c
    fsw/src/cfe_msg_sechdr_checksum.c
    fsw/src/cfe_msg_sechdr_fc.c
    fsw/src/cfe_msg_sechdr_time.c
)

# Source selection for if CCSDS extended header is included, and MsgId version use
if (MISSION_INCLUDE_CCSDSEXT_HEADER)
    message(STATUS "CCSDS primary and extended header included in message header")
    list(APPEND ${DEP}_SRC
        fsw/src/cfe_msg_ccsdsext.c
        fsw/src/cfe_msg_initdefaulthdr_priext.c)
    if (MISSION_MSGID_V2)  # MsgId v2 or v1 can be used with extended headers
      message(STATUS "Message Id version 2 in use (MsgId V2)")
      list(APPEND ${DEP}_SRC
           fsw/src/cfe_msg_msgid_v2.c)
    else (MISSION_MSGID_V2)
      message(STATUS "Message Id version 1 in use (MsgId V1)")
      list(APPEND ${DEP}_SRC
           fsw/src/cfe_msg_msgid_v1.c)
    endif (MISSION_MSGID_V2)
else (MISSION_INCLUDE_CCSDSEXT_HEADER)
    message(STATUS "CCSDS primary header included in message header (not including CCSDS extended header)")
    message(STATUS "Message Id version 1 in use (MsgId V1)")
    list(APPEND ${DEP}_SRC
        fsw/src/cfe_msg_initdefaulthdr_pri.c
        fsw/src/cfe_msg_msgid_v1.c)
    if (MISSION_MSGID_V2)
        message(FATAL_ERROR "Message Id (MsgId) version 2 can only be used if MISSION_INCLUDE_CCSDSEXT_HEADER is set")
    endif (MISSION_MSGID_V2)
endif (MISSION_INCLUDE_CCSDSEXT_HEADER)

# Module library
add_library(${DEP} STATIC ${${DEP}_SRC})

target_include_directories(${DEP} PUBLIC fsw/inc)

target_link_libraries(${DEP} PRIVATE core_private)

# Add unit test coverage subdirectory
if(ENABLE_UNIT_TESTS)
    add_subdirectory(ut-coverage)
endif(ENABLE_UNIT_TESTS)

cfs_app_check_intf(${DEP}
    ccsds_hdr.h
    cfe_msg_api_typedefs.h
)
