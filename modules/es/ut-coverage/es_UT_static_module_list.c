#include "target_config.h"
#include "target_objtab.h"

/* Forward declarations */
extern Target_ObjectTable_t CFE_CONFIG_ModuleEntry;
extern Target_ObjectTable_t CFE_ES_ModuleEntry;
extern Target_ObjectTable_t CFE_EVS_ModuleEntry;
extern Target_ObjectTable_t CFE_FS_ModuleEntry;
extern Target_ObjectTable_t CFE_SB_ModuleEntry;
extern Target_ObjectTable_t CFE_TBL_ModuleEntry;
extern Target_ObjectTable_t CFE_TIME_ModuleEntry;
extern Target_ObjectTable_t CFE_MSG_ModuleEntry;
extern Target_ObjectTable_t CFE_SBR_ModuleEntry;
extern Target_ObjectTable_t CFE_RESOURCEID_ModuleEntry;

/* UT version of the core module object table */
static const Target_ObjectTable_t *UT_CoreObjectTable[] = {
    &CFE_CONFIG_ModuleEntry,
    &CFE_ES_ModuleEntry,
    &CFE_EVS_ModuleEntry,
    &CFE_FS_ModuleEntry,
    &CFE_SB_ModuleEntry,
    &CFE_TBL_ModuleEntry,
    &CFE_TIME_ModuleEntry,
    &CFE_MSG_ModuleEntry,
    &CFE_SBR_ModuleEntry,
    &CFE_RESOURCEID_ModuleEntry,
    NULL /* null terminator */
};

const Target_ConfigData GLOBAL_CONFIGDATA = {
    .CoreObjectTable = UT_CoreObjectTable,
    /* other fields left as zero/NULL - UT stubs handle them */
};
