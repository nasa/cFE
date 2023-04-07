# Rule for traditional CFE table generation via elf2cfetbl

elf/%.o:
	@mkdir -pv $(dir $(@))
	cd $(dir $(@)) && $(AR) x $(abspath $(<)) $(notdir $(@))
