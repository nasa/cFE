###############################################################################
#
# $Id: cfe_time.mak 1.1 2008/04/17 07:05:38GMT-05:00 ruperera Exp  $
#
# Purpose:  cFE Time Services (TIME) make file
#
# Author:   S.Walling/Microtel
#
# Notes:    Creates a combination of the Time Services task and API library
#
###############################################################################

# Subsystem produced by this makefile.
TARGET = time.o

#==============================================================================
# Object files required to build subsystem.

OBJS =\
  cfe_time_task.o cfe_time_api.o cfe_time_utils.o cfe_time_tone.o

#==============================================================================
# Source files required to build subsystem; used to generate dependencies.

SOURCES = $(OBJS:.o=.c)

