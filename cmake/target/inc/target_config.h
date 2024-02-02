/************************************************************************
 * NASA Docket No. GSC-18,719-1, and identified as “core Flight System: Bootes”
 *
 * Copyright (c) 2020 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

/**
 * @file
 *
 * Defines structures for the global system-wide configuration data.
 * These structures can be accessed at runtime and are an alternative to
 * setting tunable parameters with preprocessor macros.  By putting the
 * tunable parameter in here and accessing it at runtime, the object
 * code becomes more portable.
 */

#ifndef TARGET_CONFIG_H
#define TARGET_CONFIG_H

#include "common_types.h"

/**
 * Prototype for the main system entry function implemented in CFE ES
 * The PSP should call this at start up.
 */
typedef void (*System_MainFunc_t)(uint32 StartType, uint32 StartSubtype, uint32 ModeId, const char *StartFilePath);

/**
 * Prototype for 1Hz ISR function implemented in CFE TIME
 * The PSP should call this at 1Hz.
 */
typedef void (*System_1HzISRFunc_t)(void);

/**
 * Prototype for notification function implemented in CFE ES
 * The PSP should call this when exceptions occur.
 *
 * NOTE: the PSP must call this routine only from a context where
 * it is possible to use OSAL primitives.  This means it must _not_
 * be called from an ISR/signal context on systems where these are
 * restricted.
 */
typedef void (*System_NotifyFunc_t)(void);

/**
 * Abstract pointer to a module API
 * Contents are not defined at this level - the PSP/CFE implementation actually defines this.
 */
typedef const void CFE_StaticModuleApi_t;

/**
 * Combines a module API with a name to create the static module list.
 */
typedef const struct
{
    const char *           Name;
    CFE_StaticModuleApi_t *Api;
} CFE_StaticModuleLoadEntry_t;

/**
 * A record containing a configuration name
 * This is a single entry in a list of simple strings.
 */
typedef const struct
{
    const char *Name;
} CFE_ConfigName_t;

/**
 * A record containing a configuration key-value pair
 * The data are simple strings defined at compile time, and
 * do not change until next build (always const).
 */
typedef const struct
{
    const char *Key;
    const void *Value;
} CFE_ConfigKeyValue_t;

/**
 * The "EdsLib_DatabaseObject" is an abstract structure here.
 */
typedef struct EdsLib_DatabaseObject                CFE_EdsDbObject_t;
typedef struct CFE_MissionLib_SoftwareBus_Interface CFE_SbIntfDbObject_t;

/**
 * Core Flight Executive configuration information.
 */
typedef const struct
{
    /**
     * 1Hz ISR entry point.  Called from PSP once per second on HW clock.
     */
    System_1HzISRFunc_t System1HzISR;

    /**
     * Main CFE entry point.  Called from PSP startup code.
     */
    System_MainFunc_t SystemMain;

    /**
     * Notification function. Called from PSP after async event handling.
     */
    System_NotifyFunc_t SystemNotify;

    /*
     * Sizes of memory segments required by the CFE based on the current config
     */
    uint32 CdsSize;             /***< CDS segment size */
    uint32 ResetAreaSize;       /***< Reset area segment size */
    uint32 UserReservedSize;    /***< User reserved area segment size */
    uint32 RamDiskSectorSize;   /***< RAM disk sector size */
    uint32 RamDiskTotalSectors; /***< RAM disk number of sectors */

    /**
     * Default value for nonvolatile file system mount point
     */
    const char *NonvolMountPoint;

    /**
     * Default value for volatile file system mount point
     */
    const char *RamdiskMountPoint;

    /**
     * File name of startup script
     */
    const char *NonvolStartupFile;
} Target_CfeConfigData;

/**
 * Extern reference to cfe config struct.
 * Allows the actual instantiation to be done outside this module
 */
extern Target_CfeConfigData GLOBAL_CFE_CONFIGDATA;

/**
 * Build system information
 * This data will be auto-populated by the build scripts
 *
 * Note that with some PSP's (such as the linux ones) the values
 * for CPU name and number may be overridden, so the correct value
 * for these should be acquired through the PSP via the normal API.
 * The values in this structure are only the defaults.
 */
typedef const struct
{
    const char *MissionName; /**< The Mission Name from configuration */

    /*
     * Note: the version strings in these fields should reflect the administratively-assigned
     * "semantic version" identifiers, typically from a "version.h" header file of
     * some type which is manually updated at various points in the development cycle.
     *
     * This is separate/distinct from the source control based revision
     * information, although it may be similar/related.  All automatically
     * obtained source revision info is in the "ModuleVersionList" below.
     */
    const char *MissionVersion; /**< Version string from mission source tree at build time (currently unused/unset) */
    const char *CfeVersion;     /**< Version string from CFE source tree at build time */
    const char *OsalVersion;    /**< Version string from OSAL source tree at build time */

    const char *Config; /**< Configuration used for build */

    /*
     * Note: date and user info have been moved into the BuildEnvironment below,
     * but these members must exist in this structure (for now) for compatibility.
     * These pointers will be NULL at runtime.
     */
    const char *Date; /**< Not set.  Get value from BuildEnvironment instead. */
    const char *User; /**< Not set.  Get value from BuildEnvironment instead. */

    /*
     * Default values for CPU ID and CPU Name
     */
    const char *Default_CpuName;      /**< Compile-time value for CPU name */
    uint16      Default_CpuId;        /**< Compile-time value for CPU number */
    uint16      Default_SpacecraftId; /**< Compile-time value for Spacecraft ID (mission-wide) */

    const char *Default_ModuleExtension; /**< Default system extension for dynamic modules */
    const char *Default_CoreFilename;    /**< Default file name for CFE core executable/library */

    Target_CfeConfigData *CfeConfig; /**< CFE configuration sub-structure */
    CFE_StaticModuleLoadEntry_t
        *PspModuleList; /**< List of PSP modules (API structures) statically linked into the core EXE */

    CFE_ConfigKeyValue_t
        *BuildEnvironment; /**< Environment details of build system at the last time CFE core was built */
    CFE_ConfigKeyValue_t *ModuleVersionList; /**< List of module versions at the last time CFE core was built */
    CFE_ConfigName_t *    CoreModuleList;    /**< List of CFE core support module names that are statically linked */
    CFE_ConfigName_t
        *StaticAppList; /**< List of additional CFS Applications that are statically linked into this binary */

    /**
     * Normal read-only EDS Database object
     *
     * This EDS DB object pointer is always initialized to be non-null.
     * It is qualified as "const" and used for all EDS query requests.
     */
    const CFE_EdsDbObject_t *EdsDb;

    /**
     * Dynamic EDS Database object
     *
     * This provides a writable (non-const) pointer to the same EDS object as above,
     * but only when when dynamic EDS link mode is selected.  It will be set NULL
     * when static EDS link mode is selected.
     *
     * This can checked by runtime code to determine the EDS link mode.  If it is
     * NULL this means the EDS DB is fixed/static and no runtime registration is needed.
     * If this pointer is non-null then the EDS DB is dynamic and runtime registration
     * is needed.
     */
    CFE_EdsDbObject_t *DynamicEdsDb;

    /**
     * Software bus interface EDS object (MsgId mappings)
     */
    const CFE_SbIntfDbObject_t *SbIntfDb;

} Target_ConfigData;

/**
 * Extern reference to global config struct.
 * This object is always instantiated within this module
 */
extern Target_ConfigData GLOBAL_CONFIGDATA;

#endif /* TARGET_CONFIG_H */
