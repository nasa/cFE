project(CFE_ASSERT C)

# Create the app module
add_cfe_app(cfe_assert
    src/cfe_assert_io.c
    src/cfe_assert_init.c
    src/cfe_assert_runner.c
    $<TARGET_OBJECTS:ut_assert_pic>
)

# publicize the interface to cfe_assert (and ut_assert)
target_include_directories(cfe_assert PUBLIC
    ${CFE_ASSERT_SOURCE_DIR}/inc
    $<TARGET_PROPERTY:ut_assert,INTERFACE_INCLUDE_DIRECTORIES>
)
