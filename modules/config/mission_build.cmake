###########################################################
#
# CFE configuration mission build setup
#
# This file is evaluated as part of the "prepare" stage
# and can be used to set up prerequisites for the build,
# such as generating header files
#
###########################################################

# Generate the complete list of configuration ids
set(CFE_CONFIG_IDS)
set(GENERATED_ENUM_OFFSET_LIST)
set(GENERATED_CONSTANT_DEFINE_LIST)
set(GENERATED_IDNAME_MAP_LIST)

# Append the set of version description config keys
list(APPEND CFE_CONFIG_IDS
  MISSION_NAME
  MISSION_SRCVER

  CORE_VERSION_MAJOR
  CORE_VERSION_MINOR
  CORE_VERSION_REVISION
  CORE_VERSION_MISSION_REV
  CORE_VERSION_BUILDNUM
  CORE_VERSION_BASELINE
  CORE_VERSION_DESCRIPTION

  CORE_BUILDINFO_DATE
  CORE_BUILDINFO_USER
  CORE_BUILDINFO_HOST
)

# Generate config ID for source version of modules that are included in the build
# NOTE: the presence in this list does not necesarily mean it will have a value at runtime,
# which may be the case for dynamic apps which are not loaded, for instance.
foreach(DEP ${MISSION_CORE_INTERFACES} ${MISSION_APPS} ${MISSION_CORE_MODULES} ${MISSION_PSPMODULES})
  string(TOUPPER "${DEP}" DEPNAME)
  list(APPEND CFE_CONFIG_IDS MOD_SRCVER_${DEPNAME})
endforeach()

# Append any mission-defined config keys
# this may further extend the list of IDs
include(${MISSIONCONFIG}/config_ids_custom.cmake OPTIONAL)

foreach(CFGID ${CFE_CONFIG_IDS})
  list(APPEND GENERATED_ENUM_OFFSET_LIST "  CFE_ConfigIdOffset_${CFGID},\n")
  list(APPEND GENERATED_IDNAME_MAP_LIST "  [CFE_ConfigIdOffset_${CFGID}] = { \"${CFGID}\" },\n")
  list(APPEND GENERATED_CONSTANT_DEFINE_LIST "#define CFE_CONFIGID_${CFGID} CFE_CONFIGID_C(CFE_ResourceId_FromInteger(CFE_CONFIGID_BASE + CFE_ConfigIdOffset_${CFGID}))\n")
endforeach()

string(CONCAT GENERATED_ENUM_OFFSET_LIST ${GENERATED_ENUM_OFFSET_LIST})
string(CONCAT GENERATED_CONSTANT_DEFINE_LIST ${GENERATED_CONSTANT_DEFINE_LIST})
string(CONCAT GENERATED_IDNAME_MAP_LIST ${GENERATED_IDNAME_MAP_LIST})

# Write header file for config IDs
configure_file(${CMAKE_CURRENT_LIST_DIR}/cmake/cfe_config_ids.h.in ${CMAKE_BINARY_DIR}/inc/cfe_config_ids.h)

# Write constant map list for config IDs
configure_file(${CMAKE_CURRENT_LIST_DIR}/cmake/cfe_config_map.c.in ${CMAKE_BINARY_DIR}/src/cfe_config_map.c)
