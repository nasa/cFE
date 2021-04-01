project(CFE_TESTRUNNER C)

# Create the app module
add_cfe_app(cfe_testrunner
    src/cfe_testrunner_main.c
)

target_include_directories(cfe_testrunner PUBLIC
    ${CFE_TESTRUNNER_SOURCE_DIR}/inc
)

# register the dependency on cfe_assert
add_cfe_app_dependency(cfe_testrunner cfe_assert)
