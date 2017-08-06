# Makefile for $10 ATA schematics
# Generates sch and PCB PDFs
# Derived from the BlackfinOne Makefile - thanks Ivan

SCH_FILES = hybrid dc-dc_converter
REV     := $(shell svn info | grep Revision | sed 's/Revision: //')

SCHPDF    = $(addsuffix .pdf,$(SCH_FILES))
PWD       = $(shell pwd)

PCB_PRINT_OPTIONS = --ps-color --media A4

all: $(SCHPDF)

%.ps: %.sch
	gschem -o $@ -s gschem-print.scm $^

%.pdf: %.ps
	# sed stuff enlarges gschem font which is too small for my eyes :-)
	cat $^ | sed 's/scalefont/1.5 mul scalefont/' > tmp.ps
	ps2pdf tmp.ps $@

