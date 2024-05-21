##########################################################
#
# CFE Configuration key names - dynamic set
#
# These configuration keys are set via the FSW code
# with the aid of the platform configuration tool, and
# are set according to the platform config parameters.
# They may be set differently on each CPU.
#
##########################################################

# These "List" items are generated from platform config
# These need to be set dynamically from the tool based on
# the configurable list size.  The list size and the initializers
# have to match, so they cannot be hardcoded.
list(APPEND CFE_CONFIG_IDS
  PLATFORM_TBL_VALID_SCID
  PLATFORM_TBL_VALID_PRID
  PLATFORM_ES_MEM_BLOCK_SIZE
  PLATFORM_ES_CDS_MEM_BLOCK_SIZE
  PLATFORM_SB_MEM_BLOCK_SIZE
)
