# Rule for traditional CFE table generation via elf2cfetbl

elf/%:
	@mkdir -pv $(dir $(@))
	cd $(dir $(@)) && $(AR) x $(abspath $(<)) $(notdir $(@))
