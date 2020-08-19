project(CFE_TESTRUNNER C)

include_directories("inc")
include_directories("${UT_ASSERT_SOURCE_DIR}/inc")

# Create the app module
add_cfe_app(cfe_testrunner
    src/cfe_testrunner_main.c
)

