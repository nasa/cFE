###############################################################################
#
# $Id: cfe_tbl.mak 1.1 2008/04/17 07:05:33GMT-05:00 ruperera Exp  $
#
# Purpose:  cFE Table Services (TBL) make file
#
# Author:   K.Audra(Microtel-LLC)
#
# Notes:    
# 
###############################################################################

# Subsystem produced by this makefile.
TARGET = tbl.o

#==============================================================================
# Object files required to build subsystem.

OBJS =\
  cfe_tbl_internal.o cfe_tbl_api.o cfe_tbl_task.o cfe_tbl_task_cmds.o

#==============================================================================
# Source files required to build subsystem; used to generate dependencies.

SOURCES = $(OBJS:.o=.c)

