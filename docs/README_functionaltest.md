# Mini-HOWTO guide for running CFE functional tests

This version of CFE includes additional modular libraries and an application
to execute functional tests within an unmodified CFE environment.  These are
distinct from the unit tests, which are executed separately from CFE in a
confined (non-CFE, test-specific) environment.

## Building the modules

These modules are built automatically when `ENABLE_UNIT_TESTS` is enabled
in the build.  However, they can be added to any CFE build by adding the
following to the `APPLIST` of any target in `targets.cmake`:

- `cfe_assert`: a CFE-compatible library wrapping the basic UT assert library.  This
is the same library that all other unit tests use, but configured to be
dynamically loaded into the CFE environment, and using CFE syslog for its output.
This must be the first library loaded for any functional test.

- `cfe_testcase`: a CFE-compatible library implementing test cases for CFE core apps.
This must be loaded after `cfe_assert`.

- `cfe_testrunner`: a CFE application that actually executes the tests.  This is a very
simple app that waits for CFE startup to complete, then executes all registered test
cases.  It also must be loaded after `cfe_assert`.


## Example startup script entries

To execute tests at startup, the following lines can be added to `cfe_es_startup.scr` on the
designated test target:

    CFE_LIB, cfe_assert,     CFE_Assert_LibInit, ASSERT_LIB,     0,     0, 0x0, 0;
    CFE_APP, cfe_testcase,   CFE_TestMain,       CFE_TEST_APP, 100, 16384, 0x0, 0;

It is important that `cfe_assert` is loaded first, as all other test applications depend on
symbols provided in this library.  The order of loading other test cases should not
matter with respect to symbol resolution, but note that test apps may be executed in
a different order than they are listed in the startup script.
