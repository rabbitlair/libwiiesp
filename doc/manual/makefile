# Este archivo es parte de libWiiEsp. Copyright (C) 2011 Ezequiel Vázquez de la Calle

NOM = manual
DOC = $(NOM).tex

PDF = pdflatex
LATEX = latex
DVI = dvipdf

pdf:
	$(PDF) $(DOC) && $(PDF) $(DOC)

latex: 
	$(LATEX) $(DOC) && $(LATEX) $(DOC)

dvi: 
	$(DVI) $(NOM).tex

clean: 
	@$(RM) *.aux *.toc *.lot *.lof *.log *.out *~ *\#

clean-all:
	@$(RM) *.aux *.toc *.lot *.lof *.log *.out *~ *\# $(NOM).pdf $(NOM).dvi
