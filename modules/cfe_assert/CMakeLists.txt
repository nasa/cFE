project(CFE_ASSERT C)

include_directories("${CFE_ASSERT_SOURCE_DIR}/inc")
include_directories("${UT_ASSERT_SOURCE_DIR}/inc")

# Create the app module
add_cfe_app(cfe_assert
    src/cfe_assert_io.c
    src/cfe_assert_init.c
    $<TARGET_OBJECTS:ut_assert_pic>
)

