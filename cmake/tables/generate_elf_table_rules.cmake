##################################################################
#
# Sub-script to capture the table compile/generation environment
#
# This small script runs at build time (as opposed to prep time)
# which captures a set of environment metadata
#
# It must be done this way such that generator expressions will
# be evaluated now, during the arch build process, rather than
# deferring the evaluation to the parent build where they may
# have different values.
#
##################################################################

set(STAGING_DIR staging ${TARGET_NAME} ${INSTALL_SUBDIR})
string(REPLACE ";" "/" STAGING_DIR  "${STAGING_DIR}")

set(TABLE_BINARY "${STAGING_DIR}/${TABLE_NAME}.tbl")
set(TMP_DIR "elf/${TARGET_NAME}")
set(TABLE_RULES)

foreach(TBL_SRC ${SOURCES})

    get_filename_component(DEP_FILE ${TBL_SRC} NAME)
    set(DEP_FILE "${TMP_DIR}/${DEP_FILE}${OBJEXT}")
    string(APPEND TABLE_RULES
        "${DEP_FILE}: ${ARCHIVE_FILE}\n"
        "${TABLE_BINARY}: ${DEP_FILE}\n"
        "\n"
    )

endforeach()

configure_file(${TEMPLATE_FILE} ${OUTPUT_FILE})
