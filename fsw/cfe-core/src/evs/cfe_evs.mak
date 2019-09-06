###############################################################################
# File: cfe_evs.mak
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

