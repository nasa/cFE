##################################################################
#
# cFS Mission default values
#
# This file provides default values for mission applications
# and module/library selection.
#
##################################################################

# The "MISSION_CORE_MODULES" will be built and statically linked as part
# of the CFE core executable on every target.  These can be used to amend
# or override parts of the CFE core on a mission-specific basis.
set(MISSION_CORE_MODULES)

# The "MISSION_GLOBAL_APPLIST" is a set of apps/libs that will be built
# for every defined and target.  These are built as dynamic modules
# and must be loaded explicitly via startup script or command.
# This list is effectively appended to every TGTx_APPLIST in targets.cmake.  
set(MISSION_GLOBAL_APPLIST)

# The "MISSION_GLOBAL_STATIC_APPLIST" is similar to MISSION_GLOBAL_APPLIST
# but the apps are statically linked.  
# This list is effectively appended to every TGTx_STATIC_APPLIST in targets.cmake.  
set(MISSION_GLOBAL_STATIC_APPLIST)
