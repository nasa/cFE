# Core Flight Software Contributing Guide

So you'd like to contribute to Core Flight Software? Below are some guidelines for contributors to follow:

* Submit a bug/feature ticket.

* Follow GitHub's fork-branch-pull request pattern.

* Ensure that your ticket is relevant, and not redundant or in conflict with other tickets.

* If you are tackling an existing ticket, check that it's still open and still not redundant or in conflict.

* Name your branch "fix-###-<whatyoufix>"
    _TODO_: "feature-###-<whatyoufeature>"?

* Follow cFS code conventions (formatting, symbol naming, file naming, etc) but do *not* change/reformat existing code except to address your changes.

* For any new API's, add unit tests to cover nominal and off-nominal conditions.

* Add/edit stubs to the unit test codebase for any new/modified functions.

* For any changes to existing API's, alter the unit tests to cover the changes (and remove tests made irrelevant due to your changes).

* Test your code, on a Linux platform (at minimum) --
    _TODO:_ Specific distros/versions/CPU architectures of Linux?

* Build your code, including unit tests.
    _TODO:_ need "standard" build process.

* Run the unit tests (all passed, yes?)
    _TODO:_ need "standard" test procedure.

* Run the main cfs executable (no errors reported, yes?)
    _TODO:_ need "standard" test procedure, including targets.cmake recommendations.

* ?? Do we expect contributors to run coverage ?? Guessing no.
