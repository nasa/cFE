# Command to generate PDF from the markdown file

MD_FILES := $(wildcard *.md)

PANDOC_FMT_OPTS := -f markdown
PANDOC_PDF_OPTS := -s -V documentclass=article -V geometry:margin=0.75in --smart --normalize --toc --toc-depth=2

# Options for pandoc "bleeding edge" version that you get when
# you install the version from the website:
# PANDOC_FMT_OPTS := -f markdown+smart
# PANDOC_PDF_OPTS := -s -V documentclass=article -V geometry:margin=0.75in --toc --toc-depth=2

%.pdf: %.md Makefile
	pandoc $(PANDOC_FMT_OPTS) $(PANDOC_PDF_OPTS) $(PANDOC_EXTRA_OPTS) -o $@ $<

# For version description documents (and possibly others) use -N for numbered sections
$(wildcard cFE*version_description.pdf): PANDOC_EXTRA_OPTS = -N

all: $(MD_FILES:.md=.pdf)
