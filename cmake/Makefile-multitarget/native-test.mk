# At a minimum the variable O must be set by the caller
ifeq ($(O),)
$(error O must be set prior to native-test.mk)
endif

include $(O)/test-results/test-command-deps.mk
include $(O)/test-results/all-test-deps.mk

ARCH   ?= native
OBJDIR := $(O)/$(ARCH)

# Check for dirs which have a "CTestTestfile.cmake" - this means we can run ctest there
TEST_TARGETS := $(addsuffix ctest.log,$(dir $(wildcard $(OBJDIR)/*/CTestTestfile.cmake)))

.PHONY: all_tests all_lcov clean_lcov force

#all_tests: | clean_lcov
#all_logs: | clean_lcov

$(OBJDIR)/coverage_test.info: force
	lcov --capture --rc branch_coverage=1 --directory "$(OBJDIR)" --output-file "$(OBJDIR)/coverage_test.info"

clean_lcov:
	find "$(OBJDIR)" -type f -name '*.gcno' -o -name '*.gcda' -print0 | xargs -0 rm -f
	-rm -f $(OBJDIR)/coverage_test.info

# It is important NOT to create the ".log" file unless it was successful
# The intent is if the test failed it will stop at the temp file and not rename it
# This relies on the exit code of the test process - it returns nonzero on error
%.log:
	(cd $(dir $(<)) && ./$(notdir $(<))) > "$(@).tmp"
	@mv -v "$(@).tmp" "$(@)"

#%.check: %.log
#	touch $(@)

all_lcov: $(OBJDIR)/coverage_test.info
	genhtml "$(OBJDIR)/coverage_test.info" --branch-coverage --output-directory "$(OBJDIR)/lcov"
	@/bin/echo -e "\n\nCoverage Report Link: file://$(abspath $(OBJDIR))/lcov/index.html\n"
