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
# The "intf" modules are headers only, and define the interface(s) between components
set(MISSION_CORE_MODULES
    "core_api"
    "core_private"
    "es"
    "evs"
    "fs"
    "sb"
    "tbl"
    "time"
    "osal"
    "psp"
    "msg"
    "sbr"
    "resourceid"
)

# The "MISSION_GLOBAL_APPLIST" is a set of apps/libs that will be built
# for every defined and target.  These are built as dynamic modules
# and must be loaded explicitly via startup script or command.
# This list is effectively appended to every TGTx_APPLIST in targets.cmake.
set(MISSION_GLOBAL_APPLIST)

# The "MISSION_GLOBAL_STATIC_APPLIST" is similar to MISSION_GLOBAL_APPLIST
# but the apps are statically linked.
# This list is effectively appended to every TGTx_STATIC_APPLIST in targets.cmake.
set(MISSION_GLOBAL_STATIC_APPLIST)

# The "MISSION_MODULE_SEARCH_PATH" is a list of subdirectories
# which will be searched for modules (apps and libs) specified in
# the targets.cmake file.  It may also be locally extended by setting
# the environment variable "CFS_APP_PATH"
set(MISSION_MODULE_SEARCH_PATH
    "apps"                  # general purpose $[top}/apps directory
    "libs"                  # general purpose $[top}/libs directory
    "psp/fsw/modules"       # modules for optional platform abstraction, associated with PSP
    "cfe/modules"           # modules for optional core functions, associated with CFE
)

# The path for specific components can also be set via
# a variable named "<component>_SEARCH_PATH".  This is
# used for locating other modules that are not part
# of the standard search path.
set(osal_SEARCH_PATH ".")
set(psp_SEARCH_PATH ".")

# If ENABLE_UNIT_TEST is enabled, then include the cfe_assert library in
# all targets.  This can still be overridden in targets.cmake.
if (ENABLE_UNIT_TESTS)
    list(APPEND MISSION_GLOBAL_APPLIST cfe_assert cfe_testrunner cfe_testcase)
endif (ENABLE_UNIT_TESTS)

