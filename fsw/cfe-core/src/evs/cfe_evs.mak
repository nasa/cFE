###############################################################################
# File: evs.mak
# $Id: cfe_evs.mak 1.1 2008/04/17 07:05:12GMT-05:00 ruperera Exp  $
#
# Purpose:
#   Compile the Event Services (EVS) task.
#
###############################################################################

# Subsystem produced by this makefile.
TARGET = evs.o

#==============================================================================
# Object files required to build subsystem.

OBJS=cfe_evs.o cfe_evs_task.o cfe_evs_log.o cfe_evs_utils.o

#==============================================================================
# Source files required to build subsystem; used to generate dependencies.

SOURCES = $(OBJS:.o=.c)

