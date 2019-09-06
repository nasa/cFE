###############################################################################
#
# File: cfe_fs.mak
#
# Purpose:  cFE File Services (FS) make file
#
# Author:   S.Walling/Microtel
#
# Notes:    Creates a combination of the File Services task and API library
#
###############################################################################

# Subsystem produced by this makefile.
TARGET = fs.o

#==============================================================================
# Object files required to build subsystem.

OBJS = cfe_fs_priv.o cfe_fs_api.o cfe_fs_decompress.o

#==============================================================================
# Source files required to build subsystem; used to generate dependencies.

SOURCES = $(OBJS:.o=.c)

