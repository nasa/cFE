##########################################################
#
# CFE Configuration key names - base set
#
# These configuration keys are set via the FSW code
# as part of the basic init routine.  They will be set
# to the same values on all CPUs.
#
##########################################################

# Append the set of version description config keys
list(APPEND CFE_CONFIG_IDS
  MISSION_NAME
  MISSION_SRCVER
  MISSION_EDS_DB
  MISSION_SBINTF_DB

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
