---
title: core Flight Software Release Notes
tags: cFE
abstract: |    
    ![cFS Logo](./media/image1.png){width="2.75625in" height="2.0243in"}
    
    \begin{center}
    \LARGE
    core Flight Executive (cFE)
    \normalsize
    \end{center}
    \newpage                            
---

\newpage

# cFE 6.6.0

Deploying CFE 6.6 as shipped into a project that uses the most recenlty shipped versions of the other
components will require a few changes outside the CFE subtree.

## Update C header files in ${MISSIONCONFIG}_defs

This can be accomplished by transferring headers from the cmake/sample_defs subdirectory of CFE into 
your top-level ${MISSIONCONFIG}_defs directory.

Existing projects will want to make this update, then incorporate any existing customizations into 
the new files.

The following modifications were checked by incorporating Git Commit **0bbdbea** in cfs_test into the 
automatic test system:

### ${MISSIONCONFIG}_defs/cpu1_msgids.h

The names of a number of MISSION specific macros have been changed, globally, to include the term 
MISSION in the name of the macro. This emphasizes that these values which may differ from mission 
to mission are expected to be consistant across all target platforms that are part of the 
same mission.

This file no longer provides a Message ID for the "FAKE" Time Command, as this is no longer present 
in the TIME application.

The EVENT facility now supports both a LONG form (which matches the prior format of EVENT messages),
and a SHORT form. This file provides distinct Message ID numbers for each, with new names that indicate 
LONG or SHORT form respectively. CFE_EVS_EVENT_MSG_MID is now considered to be deprecated, but will be
provided unless specific steps are taken during the project build to omit deprecated things.

### ${MISSIONCONFIG}_defs/cpu1_platform_cfg.h

The names of a number of PLATFORM specific macros have been changed, globally, to include the term PLATFORM 
in the name of the macro. This emphasizes that these values may differ from platform to platform within a 
single mission, and generic code that does not relate directly to a specific platform should not
reference them.

COMPATIBILITY definitions are provided for the original names defining them in terms of the new names; these 
old names are deprecated, but are provided unless specific steps are taken to omit deprecated items from the 
CFE build.

Comments have been updated to provide additional clarity about the supported ranges of a number of the 
configuration macros. Operations outside these limits may work perfectly well, but missions that need to work 
beyond the documented limits are responsible for assuring operational correctness.

Several new macros have been added; refer to commentary in the header file for operational details:

+   CFE_PLATFORM_ENDIAN
+   CFE_PLATFORM_TIME_CFG_BIGENDIAN
+   CFE_PLATFORM_ES_MEMPOOL_ALIGN_SIZE
+   CFE_PLATFORM_ES_SHELL_OS_DELAY_MILLISEC

Several macros have been removed, as the code that previously used them no longer has such 
configuration options (or now uses one of the new options given above):

+   CFE_TIME_MAX_NUM_SYNCH_FUNCS
+   CFE_TIME_ENA_1HZ_CMD_PKT
+   CFE_ES_MEMPOOL_ALIGNED

The "CFE_TIME_ENA_1HZ_CMD_PKT" configuration item is no longer configurable. The header continues to define
this, hardwiring it to TRUE, in order to trigger a message during a project build if the project attempts to
define it as anything else.

### ${MISSIONCONFIG}_defs/sample_mission_cfg.h

The names of a number of MISSION specific macros have been changed, globally, to include the term MISSION in 
the name of the macro. This emphasizes that these values which may differ from mission to mission are expected 
to be consistant across all target platforms that are part of the same mission.

COMPATIBILITY definitions are provided for the original names defining them in terms of the new names; these 
old names are deprecated, but are provided unless specific steps are taken to omit deprecated items from the 
CFE build.

A new MESSAGE_FORMAT_IS_CCSDS_VER_2 option has been added, which can be set to compile the system to use 
modified CCSDS message headers that extend the Message ID space; leave this macro undefined to continue using 
existing message formats.

Based on the setting (or not) of the above macro, this file provides Message ID base values; these values 
change when going from Version 1 to Version 2.

A number of additional MISSION configuration items are added, consult comments in the header file 
for details:

+   CFE_MISSION_ES_MAX_APPLICATIONS
+   CFE_MISSION_ES_MAX_SHELL_CMD
+   CFE_MISSION_ES_MAX_SHELL_PKT
+   CFE_MISSION_ES_PERF_MAX_IDS
+   CFE_MISSION_TBL_MAX_FULL_NAME_LEN
+   CFE_MISSION_SB_MAX_PIPES
+   CFE_MISSION_MAX_PATH_LEN
+   CFE_MISSION_MAX_API_LEN
+   CFE_MISSION_ES_CDS_MAX_NAME_LEN

### ${MISSIONCONFIG}_defs/sample_perfids.h

This new file provides MISSION specific definitions for the Performance Monitor ID values for the 
CFE applications.

## Include "cfe_platform_cfg.h" where needed

CFE 6.6 has a stronger demarcation of the PLATFORM specific configuration, and the cfe_platform_cfg.h 
header is no longer implicitly included. Applications that require this data must explicitly include 
this header now.

This is needed in the CF, CI_LAB, CS, SBN, and SCH applications.

These changes can be seen in Git Commit **f97ab21** in cfs_apps.

## Add cfe_sb_msg_id_util.o to CFE SB Unit Test Build Scripts

Modifications to the CFE SB application included adding a new source file, and the build scripts for 
the unit tests for this module (when built with the classic build) are owned by the CFS TOOLS project. 
The new file must be added to these build rules (specifically to the SUBSYS_OBJS list) for unit tests 
to build.

This change can be seen in Git Commit **1fb51c7** in cfs_tools.

## Add cfe_es_syslog.o to CFE ES Unit Test Build Scripts

Modifications to the CFE SB application included adding a new source file, and the build scripts for the 
unit tests for this module (when built with the classic build) are owned by the CFS TOOLS project. The new 
file must be added to these build rules (specifically to the SUBSYS_OBJS list in both assignments) for 
unit tests to build.

This change can be seen in Git Commit **3314349** in cfs_tools.
