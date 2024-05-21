
file(READ ${LIST_FILE} PLATCFG_LIST)
file(READ ${CONTENT_FILE} PLATCFG_CONTENT)

configure_file(cfe_platform_cfg.c.in ${OUTPUT_FILE} @ONLY)
