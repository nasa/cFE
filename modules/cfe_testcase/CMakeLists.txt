include_directories(inc)

# Filenames based on doxygen groups.
# Create the app module
add_cfe_app(cfe_testcase
    src/cfe_test.c
    src/cfe_test_table.c
    src/es_application_control_test.c
    src/es_behavior_test.c
    src/es_error_test.c
    src/es_info_test.c
    src/es_task_test.c
    src/es_cds_test.c
    src/es_counter_test.c
    src/es_misc_test.c
    src/es_mempool_test.c
    src/es_perf_test.c
    src/es_resource_id_test.c
    src/evs_filters_test.c
    src/evs_send_test.c
    src/fs_header_test.c
    src/fs_util_test.c
    src/message_id_test.c
    src/msg_api_test.c
    src/resource_id_misc_test.c
    src/sb_performance_test.c
    src/sb_pipe_mang_test.c
    src/sb_sendrecv_test.c
    src/sb_subscription_test.c
    src/tbl_content_access_test.c
    src/tbl_content_mang_test.c
    src/tbl_information_test.c
    src/tbl_registration_test.c
    src/time_arithmetic_test.c
    src/time_current_test.c
    src/time_conversion_test.c
    src/time_external_test.c
    src/time_misc_test.c
)

# register the dependency on cfe_assert
add_cfe_app_dependency(cfe_testcase cfe_assert)
add_cfe_tables(cfeTestAppTable tables/cfe_test_tbl.c)
