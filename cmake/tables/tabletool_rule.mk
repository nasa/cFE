# Makefile for EDS-based CFE table generation
.PHONY: cfetables

cfetables:
	@echo "Table build completed"

#%.tbl: $(TBLTOOL)
staging/%.tbl:
	@mkdir -pv $(dir $(@))
	cd $(dir $(@)) && $(TBLTOOL) $(TBLTOOL_FLAGS) $(abspath $(^))
