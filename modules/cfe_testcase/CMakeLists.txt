# Filenames based on doxygen groups.
# Create the app module
add_cfe_app(cfe_testcase
    src/cfe_test.c
    src/es_info_test.c
    src/es_task_test.c
    src/es_cds_test.c
    src/fs_header_test.c
    src/time_current_test.c
)

# register the dependency on cfe_assert
add_cfe_app_dependency(cfe_testcase cfe_assert)
