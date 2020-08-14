include_directories("${CFE_ASSERT_SOURCE_DIR}/inc")
include_directories("${UT_ASSERT_SOURCE_DIR}/inc")

# Create the app module
add_cfe_app(cfe_testcase
    src/cfe_test.c
    src/es_test.c
)
