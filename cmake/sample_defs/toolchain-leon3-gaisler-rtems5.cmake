# This example toolchain file describes the cross compiler to use for
# the target architecture indicated in the configuration file.

# In this sample application, the cross toolchain is configured to
# use a compiler for the RTEMS operating system targeting the "pc686" BSP

# Note that to use this, the "RTEMS" platform module may need to be added
# to the system-wide CMake installation as a default CMake does not yet
# recognize RTEMS as a system name.  An example of this is distributed with
# the pc-rtems PSP.

# Basic cross system configuration
set(CMAKE_SYSTEM_NAME       RTEMS)
set(CMAKE_SYSTEM_PROCESSOR  sparc)
set(CMAKE_SYSTEM_VERSION    5)

# The BSP that will be used for this build
set(RTEMS_BSP "leon3")

# these settings are specific to cFE/OSAL and determines which
# abstraction layers are built when using this toolchain
SET(CFE_SYSTEM_PSPNAME                  pc-rtems)
SET(OSAL_SYSTEM_BSPTYPE                 generic-rtems)
SET(OSAL_SYSTEM_OSTYPE                  rtems)

# This is for version specific RTEMS ifdefs needed by the OSAL and PSP
ADD_DEFINITIONS(-DOS_RTEMS_5)

# RTEMS_DYNAMIC_LOAD definition:
# - Set to FALSE for platforms that create a RTEMS executable and link it
#   to the cFE core.
# - Set to TRUE for platforms that expect the cFE core to be dynamically
#   loaded into an existing runtime image.
# This is tied to the OSAL-BSP and PSP implementation so generally cannot
# be switched on a specific OSAL/PSP platform without modifications.
set(RTEMS_DYNAMIC_LOAD      TRUE)
set(RTEMS_INCLUDE_TARFS     TRUE)
set(RTEMS_NO_CMDLINE        TRUE)

# Default to no shell (flight like and batch testing) but allow override from make line
# Example: make TARGET=devboard RTEMS_NO_SHELL=FALSE install
if (NOT DEFINED ENV{RTEMS_NO_SHELL})
  set(RTEMS_NO_SHELL TRUE)
else ()
  set(RTEMS_NO_SHELL $ENV{RTEMS_NO_SHELL})
endif ()

# TODO Only remaining dependency is for network setup, switch to osal network config and remove
if (RTEMS_INCLUDE_TARFS)
    ADD_DEFINITIONS(-DRTEMS_INCLUDE_TARFS)
endif ()

set(RTEMS_BSP_C_FLAGS       "-mcpu=leon3 -fno-common")
set(RTEMS_BSP_CXX_FLAGS     ${RTEMS_BSP_C_FLAGS})
set(RTEMS_BSP_SPECS_FLAGS   "-qbsp=leon3")

# Exception handling is very iffy.  These two options disable eh_frame creation.
set(CMAKE_C_COMPILE_OPTIONS_PIC -fno-exceptions -fno-asynchronous-unwind-tables)

# The TOOLS and BSP are allowed to be installed in different locations.
# If the README was followed they will both be installed under $HOME
# By default it is assumed the BSP is installed to the same directory as the tools
SET(RTEMS_TOOLS_PREFIX "/opt/rcc-1.3.1-gcc" CACHE PATH
    "RTEMS tools install directory")
SET(RTEMS_BSP_PREFIX "/opt/rcc-1.3.1-gcc/sparc-gaisler-rtems${CMAKE_SYSTEM_VERSION}" CACHE PATH
    "RTEMS BSP install directory")

#+---------------------------------------------------------------------------+
#| Common RTEMS toolchain statements
#+---------------------------------------------------------------------------+

# specify the cross compiler - adjust accord to compiler installation
# This uses the compiler-wrapper toolchain that buildroot produces
SET(SDKHOSTBINDIR               "${RTEMS_TOOLS_PREFIX}/bin")
set(TARGETPREFIX                "${CMAKE_SYSTEM_PROCESSOR}-gaisler-rtems${CMAKE_SYSTEM_VERSION}-")

SET(CMAKE_C_COMPILER            "${RTEMS_TOOLS_PREFIX}/bin/${TARGETPREFIX}gcc")
SET(CMAKE_CXX_COMPILER          "${RTEMS_TOOLS_PREFIX}/bin/${TARGETPREFIX}g++")
SET(CMAKE_LINKER                "${RTEMS_TOOLS_PREFIX}/bin/${TARGETPREFIX}ld")
SET(CMAKE_ASM_COMPILER          "${RTEMS_TOOLS_PREFIX}/bin/${TARGETPREFIX}as")
SET(CMAKE_STRIP                 "${RTEMS_TOOLS_PREFIX}/bin/${TARGETPREFIX}strip")
SET(CMAKE_NM                    "${RTEMS_TOOLS_PREFIX}/bin/${TARGETPREFIX}nm")
SET(CMAKE_AR                    "${RTEMS_TOOLS_PREFIX}/bin/${TARGETPREFIX}ar")
SET(CMAKE_OBJDUMP               "${RTEMS_TOOLS_PREFIX}/bin/${TARGETPREFIX}objdump")
SET(CMAKE_OBJCOPY               "${RTEMS_TOOLS_PREFIX}/bin/${TARGETPREFIX}objcopy")
SET(CMAKE_SIZE                  "${RTEMS_TOOLS_PREFIX}/bin/${TARGETPREFIX}size")

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM   NEVER)

# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY   ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE   ONLY)

SET(CMAKE_PREFIX_PATH                   /)
