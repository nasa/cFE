#
# Core Flight Software CMake / GNU make wrapper
#
# ABOUT THIS MAKEFILE:
# This is actually part of the CMake alternative build system.
# It is a GNU-make wrapper that calls the CMake tools appropriately
# so that setting up a new build is fast and easy with no need to
# learn the CMake commands.  It also makes it easier to integrate
# the build with IDE tools such as Eclipse by providing a default
# makefile that has the common targets such as all/clean/etc.
#
# Use of this file is optional.
#
# This file is intended to be placed at the TOP-MOST level of the mission
# source tree, i.e. a level above "cfe".  Note this is outside the cfe
# repository which is why it cannot be delivered directly in place.
# To use it, simply copy it to the top directory.  As this just contains
# wrappers for the CMake targets, it is unlikely to change.  Projects
# are also free to customize this file and add their own targets after
# copying it to the top of the source tree.
#
# For _ALL_ targets defined in this file the build tree location may
# be specified via the "O" variable (i.e. make O=<my-build-dir> all).
# If not specified then the "build" subdirectory will be assumed.
#
# This wrapper defines the following major targets:
#  prep -- Runs CMake to create a new or re-configure an existing build tree
#    Note that multiple build trees can exist from a single source
#    Other control options (such as "SIMULATION") may be passed to CMake via
#    make variables depending on the mission build scripts.  These will be
#    cached in the build tree so they do not need to be set again thereafter.
#
#  all -- Build all targets in the CMake build tree
#
#  install -- Copy all files to the installation tree and run packaging scripts
#     The "DESTDIR" environment variable controls where the files are copied
#
#  clean -- Clean all targets in the CMake build tree, but not the build tree itself.
#
#  distclean -- Entirely remove the build directory specified by "O"
#      Note that after this the "prep" step must be run again in order to build.
#      Use caution with this as it does an rm -rf - don't set O to your home dir!
#
#  docs -- Build all doxygen source documentation.  The HTML documentation will be
#      generated under the build tree specified by "O".
#
#  test -- Run all unit tests defined in the build.  Unit tests will typically only
#      be executable when building with the "SIMULATION=native" option.  Otherwise
#      it is up to the user to copy the executables to the target and run them.
#
#  lcov -- Runs the "lcov" tool on the build tree to collect all code coverage
#      analysis data and build the reports.  Code coverage data may be output by
#      the "make test" target above.
#

# Provide default values for all important vars, which ensures they are never unset.
# However most are overridden on a per-target basis
O             ?= placeholder
ARCH          ?= native
CFG           ?= none
INSTALLPREFIX ?= /exe
DESTDIR       ?= $(abspath $(O))
ENABLE_TESTS  ?= true
BUILDTYPE     ?= debug
ENABLE_ASAN   ?=

# If using containers it is possible that the "native" gcc version
# might refer to a different version, if this is run in a container vs.
# outside the container.
NATIVE_GCC_VER := $(shell gcc --version | head -1 | awk -F\  '{print $$NF}')

# Define the separate output dir per config
# Each config listed in CONFIG_NAMES should have a corresponding O variable here
O_native    ?= build-native-$(NATIVE_GCC_VER)
O_eds       ?= build-eds-$(NATIVE_GCC_VER)
O_qemurtems ?= build-qemurtems
O_rpi       ?= build-rpi
O_gr712     ?= build-gr712
O_flight    ?= build-flight
O_osal      ?= build-osal
O_bplib_p   ?= build-bplib_p
O_bplib_o   ?= build-bplib_o

# The "stamp" target names are associated with a file in the build dir to indicate last run time
# The "nostamp" target names do not have this, and are always executed
STAMPTGT_NAMES    := prep compile install test lcov detaildesign usersguide osalguide clean cleantest
NOSTAMPTGT_NAMES  := distclean docs

# The config names is a list of configurations to build
CONFIG_NAMES      := native eds qemurtems rpi gr712 flight osal bplib_p bplib_o

# The actual buildable targets are a combination of CONFIG.NAME (e.g. native.install)
STAMP_TARGETS     := $(foreach CFG,$(CONFIG_NAMES),$(addprefix $(CFG).,$(STAMPTGT_NAMES)))
NOSTAMP_TARGETS   := $(foreach CFG,$(CONFIG_NAMES),$(addprefix $(CFG).,$(NOSTAMPTGT_NAMES)))

# A stamp target with a ".force" suffix removes the stamp file, thereby forcing a rebuild
FORCE_TARGETS     := $(addsuffix .force,$(STAMP_TARGETS))

# The complete list of targets available per-config
ALLTGT_NAMES      := $(STAMPTGT_NAMES) $(NOSTAMPTGT_NAMES)
ALL_TARGETS       := $(STAMP_TARGETS) $(NOSTAMP_TARGETS)

# Each config/arch also gets its own list of targets, for custom variable setting
NATIVE_TARGETS    := $(addprefix native.,$(ALLTGT_NAMES))
EDS_TARGETS       := $(addprefix eds.,$(ALLTGT_NAMES))
QEMURTEMS_TARGETS := $(addprefix qemurtems.,$(ALLTGT_NAMES))
RPI_TARGETS       := $(addprefix rpi.,$(ALLTGT_NAMES))
GR712_TARGETS     := $(addprefix gr712.,$(ALLTGT_NAMES))
FLIGHT_TARGETS    := $(addprefix flight.,$(ALLTGT_NAMES))
OSAL_TARGETS      := $(addprefix osal.,$(ALLTGT_NAMES))

DISTCLEAN_TARGETS := $(addsuffix .distclean,$(CONFIG_NAMES))
DOCS_TARGETS      := $(addsuffix .docs,$(CONFIG_NAMES))

CFE_TARGETS += $(NATIVE_TARGETS)
CFE_TARGETS += $(EDS_TARGETS)
CFE_TARGETS += $(QEMURTEMS_TARGETS)
CFE_TARGETS += $(RPI_TARGETS)
CFE_TARGETS += $(GR712_TARGETS)
CFE_TARGETS += $(FLIGHT_TARGETS)

# Define the config (CFG) and build dir (O) for each target group
$(NATIVE_TARGETS):    CFG := native
$(EDS_TARGETS):       CFG := eds
$(QEMURTEMS_TARGETS): CFG := qemurtems
$(RPI_TARGETS):       CFG := rpi
$(GR712_TARGETS):     CFG := gr712
$(FLIGHT_TARGETS):    CFG := flight
$(OSAL_TARGETS):      CFG := osal

RTEMS_VERSION := 5
export RTEMS_VERSION

# Define the ARCH used for each target group
$(OSAL_TARGETS) \
$(EDS_TARGETS) \
$(NATIVE_TARGETS):    ARCH = native
$(QEMURTEMS_TARGETS): ARCH = i686-rtems$(RTEMS_VERSION)
$(RPI_TARGETS):       ARCH = arm-raspbian-linux
$(GR712_TARGETS):     ARCH = leon3-gaisler-rtems5
$(FLIGHT_TARGETS):    ARCH = mips32r2-poky-linux

$(EDS_TARGETS) \
$(NATIVE_TARGETS):    TEST_SUBDIR = default_cpu1
$(QEMURTEMS_TARGETS): TEST_SUBDIR = default_cpu1
$(RPI_TARGETS):       TEST_SUBDIR = default_cpu1
$(GR712_TARGETS):     TEST_SUBDIR = default_cpu1
$(FLIGHT_TARGETS):    TEST_SUBDIR = default_cpu1

# For all targets the O should be set to the per-config build dir
$(ALL_TARGETS):    O = $(O_$(CFG))

$(CFE_TARGETS): SUBTGT_PREFIX := mission-

# Define extra prep options for each target group
# Note that because prep can also be triggered indirectly, the PREP_OPTS
# is set for all targets, not just the prep target itself
$(ALL_TARGETS): PREP_OPTS += -DCMAKE_EXPORT_COMPILE_COMMANDS=TRUE
$(EDS_TARGETS): PREP_OPTS += -DCFE_EDS_ENABLED=ON
$(EDS_TARGETS): PREP_OPTS += -DEDSLIB_PYTHON_BUILD_STANDALONE_MODULE=ON
$(EDS_TARGETS): PREP_OPTS += -DCFE_MISSIONLIB_PYTHON_BUILD_STANDALONE_MODULE=ON
$(NATIVE_TARGETS): PREP_OPTS += -DCFE_EDS_ENABLED=OFF
$(CFE_TARGETS): PREP_OPTS += -DCMAKE_INSTALL_PREFIX=$(INSTALLPREFIX) -DSIMULATION=$(ARCH)
$(CFE_TARGETS): PREP_OPTS += -DENABLE_UNIT_TESTS=$(ENABLE_TESTS)
$(QEMURTEMS_TARGETS) \
$(OSAL_TARGETS) \
$(CFE_TARGETS): PREP_OPTS += -DCMAKE_BUILD_TYPE=$(BUILDTYPE)
$(RPI_TARGETS) \
$(FLIGHT_TARGETS): ENV_OPTS += OMIT_DEPRECATED=true
$(FLIGHT_TARGETS): PREP_OPTS += -DCMAKE_BUILD_TYPE=release

$(QEMURTEMS_TARGETS) \
$(RPI_TARGETS) \
$(GR712_TARGETS) \
$(NATIVE_TARGETS) \
$(EDS_TARGETS) \
$(FLIGHT_TARGETS): PREP_OPTS += "$(CURDIR)/cfe"

$(OSAL_TARGETS): PREP_OPTS += -DCMAKE_INSTALL_PREFIX=$(HOME)/code/local
$(OSAL_TARGETS): DESTDIR :=
$(OSAL_TARGETS): PREP_OPTS += -DENABLE_UNIT_TESTS=TRUE
$(OSAL_TARGETS): PREP_OPTS += -DOSAL_OMIT_DEPRECATED=TRUE
$(OSAL_TARGETS): PREP_OPTS += -DOSAL_SYSTEM_BSPTYPE=generic-linux
$(OSAL_TARGETS): PREP_OPTS += -DOSAL_CONFIG_DEBUG_PERMISSIVE_MODE=on
$(OSAL_TARGETS): PREP_OPTS += -DOSAL_VALIDATE_API=on
$(OSAL_TARGETS): PREP_OPTS += -DOSAL_USER_C_FLAGS='-Wall -Werror'
$(OSAL_TARGETS): PREP_OPTS += "$(CURDIR)/osal"

ifneq ($(ENABLE_ASAN),)
$(CFE_TARGETS): ENV_OPTS += ENABLE_ASAN=$(ENABLE_ASAN)
export ENABLE_ASAN
endif

# The following set of variables is exported to the sub-makes
export O
export ARCH
export CFG

# Export VERBOSE only if it was actually set to something
ifneq ($(VERBOSE),)
export VERBOSE
endif

# This is a translator from a virtual target (config.goal format) to a stamp file name
GET_STAMP_TARGET = $(O_$(basename $(1)))/stamp$(suffix $(1))

# A list of targets with a corresponding stamp file that was explicitly listed on command line
# These stamp files will be removed such that command line targets are always refreshed
COMMANDLINE_STAMPTGTS  := $(filter $(addprefix %.,$(STAMPTGT_NAMES)),$(MAKECMDGOALS))
COMMANDLINE_STAMPFILES := $(foreach GOAL,$(COMMANDLINE_STAMPTGTS),$(call GET_STAMP_TARGET,$(GOAL)))

.PHONY: $(ALLTGT_NAMES) $(ALL_TARGETS) make-buildlink rm-buildlink .force

# The "world" target is a shorthand to build and tests all items possible
world: native.lcov native.docs flight.install rpi.install qemurtems.test bplib.lcov
	@echo "WORLD BUILD COMPLETED"

$(addprefix bplib.,prep compile install test lcov clean distclean):
	$(MAKE) $(addsuffix $(suffix $(@)),bplib_o bplib_p)
	@echo "$(@) COMPLETED"

# The "bplib" install targets are currently no-ops (always success)
$(O_bplib_o)/stamp.install $(O_bplib_p)/stamp.install: %/stamp.install: %/stamp.compile
	@echo "No install for standalone bplib"
	touch "$(@)"

# The "distclean" goal removes the entire build dir, including generated makefiles
$(DISTCLEAN_TARGETS):
	[ ! -z "$(O)" ] && rm -rf "$(O)"

# The "docs" target is just a meta target for all docs (detaildesign, usersguide, osalguide)
$(DOCS_TARGETS):  %.docs: %.detaildesign
$(DOCS_TARGETS):  %.docs: %.usersguide
$(DOCS_TARGETS):  %.docs: %.osalguide

make-buildlink: rm-buildlink .force
	ln -s $(O) build-$(CFG)

rm-buildlink: .force
	rm -f build-$(CFG)

eds.prep native.prep: | make-buildlink
eds.distclean native.distclean: | rm-buildlink

# A generic pattern rule to remove a stamp file, ensuring target gets rebuilt
%.rm-stamp:
	-rm -f "$(@:%.rm-stamp=%)"

# A generic pattern rule to invoke CMake for the "prep" (makefile generation) step
%/stamp.prep:
	mkdir -p "$(O)"
	(cd "$(O)" && env $(ENV_OPTS) cmake $(PREP_OPTS) )
	echo '$(PREP_OPTS)' > "$(@)"

# A generic pattern rule to invoke a sub-make for the appliction compile only
%/stamp.compile: %/stamp.prep
	$(MAKE) --no-print-directory -C "$(O)" $(SUBTGT_PREFIX)all
	touch "$(@)"

# A generic pattern rule to invoke a sub-make for the appliction build/install to staging area
%/stamp.install: %/stamp.prep
	$(MAKE) --no-print-directory -C "$(O)" DESTDIR="$(DESTDIR)" $(SUBTGT_PREFIX)install
	touch "$(@)"

# A generic pattern rule to clean a build area
%/stamp.clean: %/stamp.prep %/stamp.compile.rm-stamp %/stamp.install.rm-stamp %/stamp.test.rm-stamp %/stamp.lcov.rm-stamp
	$(MAKE) --no-print-directory -C "$(O)" $(SUBTGT_PREFIX)clean
	touch "$(@)"

# A generic pattern rule to invoke a sub-make for staging binaries
%/stamp.cleantest: %/stamp.install
	$(MAKE) --no-print-directory -f $(CFG)-test.mk clean_lcov clean_logs
	rm -f $(O)/common-test-list.mk
	$(MAKE) $(CFG).test

# Extract the list of tests and associated commands from JSON
%/test-results/test-list.json:
	mkdir -p $(dir $(@))
	(cd $(O)/$(ARCH)/$(TEST_SUBDIR) && ctest --show-only=json-v1) | \
		jq '[.tests[] | { "name":.name, "command":.command[0], "workdir":(.properties[] | if .name == "WORKING_DIRECTORY" then .value else empty end) } ]' \
		> $(@)

%/test-results/test-command-deps.mk: %/test-results/test-list.json
	jq -r '.[] | if .workdir then "$(O)/test-results/" + .name + ".log: WORKDIR=" + .workdir else empty end' $(<) > "$(@)"
	jq -r '.[] | if .command then "$(O)/test-results/" + .name + ".log: " + .command else empty end' $(<) >> "$(@)"

%/test-results/all-test-deps.mk: %/test-results/test-list.json
	jq -r '.[] | "all_tests: $(O)/test-results/" + .name + ".log"' $(<) > "$(@)"

# A generic pattern rule to invoke a sub-make for running tests
%/stamp.test: %/stamp.install %/test-results/all-test-deps.mk %/test-results/test-command-deps.mk
	$(MAKE) --no-print-directory -f $(CURDIR)/$(CFG)-test.mk all_tests
	touch "$(@)"

# A generic pattern rule to invoke a sub-make for running coverage analysis
%/stamp.lcov: %/stamp.test
	$(MAKE) --no-print-directory -f $(CFG)-test.mk all_lcov
	touch "$(@)"

# A generic pattern rule to invoke a sub-make for building detail design doc
%/stamp.detaildesign: %/stamp.prep
	$(MAKE) --no-print-directory -C "$(O)" mission-doc
	touch "$(@)"

# A generic pattern rule to invoke a sub-make for building user guide doc
%/stamp.usersguide: %/stamp.prep
	$(MAKE) --no-print-directory -C "$(O)" cfe-usersguide
	touch "$(@)"

# A generic pattern rule to invoke a sub-make for building osal guide doc
%/stamp.osalguide: %/stamp.prep
	$(MAKE) --no-print-directory -C "$(O)" osal-apiguide
	touch "$(@)"

# also define targets which builds all configs (except world, which has custom rule above)
# This utilizes "second expansion" feature of gnu make so that $@ may be referenced in prereq list
.SECONDEXPANSION:
$(ALLTGT_NAMES):  $$(addsuffix .$$(@),$(CONFIG_NAMES))
$(STAMP_TARGETS): $$(call GET_STAMP_TARGET,$$(@))
# Ensure that stampfiles related to command-line goals are always refreshed
$(COMMANDLINE_STAMPFILES): $$(addsuffix .rm-stamp,$$(@))
