# Multi-Target makefile

The files in this directory may be used in lieu of the original "Makefile.sample" in the parent directory.

The key advantage is that it allows use of multiple build directories in parallel, as opposed to a 
single build directory as is the default in the original makefile wrapper.

To use it, copy all the files in this directory to the top level of the project.

The files ending in `-test.mk` are used to facilitate the testing targets on each platform.

The targets in this makefile follow the pattern `<CONFIG>.<GOAL>` where config is one of:

 - native
 - eds
 - qemurtems (RTEMS 5 with pc686 running in QEMU)
 - osal (standalone)
 - gr712 (for the GR712 board using gaisler compiler, requires separate toolchain)
 - flight (cross compile with optimization, requires separte toolchain)


 The goals generally the same as traditional makefile, can be one of:

 - prep
 - compile
 - install
 - test
 - lcov
 - docs

 For example, to simply build CFE for the host system do this command:

 `make native.install`

 (the prep and compile steps are automatically handled as dependencies)
 