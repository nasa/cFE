# Makefile for EDS-based CFE table generation
.PHONY: cfetables

cfetables:
	@echo "Table build completed"

# The dependency of this rule should always be a relative path starting with elf/,
# at least with the current rule generator script, so it matches the elf/% pattern rule.
# But because elf2cfetbl only writes its output to the current working dir, it has to be run
# after changing dirs into the staging area.  Thus the path to the elf file needs to be adjusted.
# Ideally this chould be done with the $(abspath f...) function but this doesn't exist in older versions.
# As a workaround, $CURDIR is used.
staging/%.tbl:
	@mkdir -pv "$(dir $(@))"
	cd "$(dir $(@))" && $(TBLTOOL) $(TBLTOOL_FLAGS) "$(CURDIR)/$(<)"
