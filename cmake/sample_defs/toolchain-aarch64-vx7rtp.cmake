#
# Cross compiler toolchain file for aarch64 Raspberry Pi 4 VxWorks 7 RTP
#

#
# This toolchain depends on the VxWorks 7 environment setup script
# It should work for both the Public SDKs and the Licensed SDKs
#
# Note that the SDK setup script redefines standard compiler environment variables such as
# CC, CPP, CXX, LD, AS, etc
# These redefined variables will cause the cFS build to fail when it builds elf2cfetbl.
# The solution for now is to comment out the CC environment variable in the VxWorks SDK environment
# setup script.
#

set(VXWORKS_RTP TRUE)
set(CMAKE_SYSTEM_NAME VxWorks-CFE)
set(CMAKE_SYSTEM_VERSION 7)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

add_definitions("-D__aarch64__")
add_definitions("-DVXSDK_RPI4")

# Common VxWorks RTP system configuration
include("${CMAKE_CURRENT_LIST_DIR}/toolchain-common-vx7rtp.cmake" REQUIRED)

MESSAGE("Toolchain Selected: aarch64-vx7rtp")
MESSAGE("CMAKE_SYSTEM_NAME = ${CMAKE_SYSTEM_NAME}")
MESSAGE("WIND_HOME = ${WIND_HOME}")
MESSAGE("CC = $ENV{CC} - should not be wr-cc!")
