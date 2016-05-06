# Este archivo es parte de libWiiEsp. Copyright (C) 2011 Ezequiel Vázquez de la Calle
# Licencia GPLv3. Encontrará una copia de la licencia en el archivo gpl-3.0.tex

NOM = memoria
DOC = $(NOM).tex

PDF = pdflatex
BIB = bibtex
LATEX = latex
DVI = dvipdf

# Por defecto: compila usando BibTeX para las referencias bibliográficas
pdf-bib: 
	$(PDF) $(DOC) && $(BIB) $(NOM) | make pdf

# No tendremos en cuenta las referencias bibliográficas. Seguramente de error
pdf:
	$(PDF) $(DOC) && $(PDF) $(DOC)

latex-bib: 
	$(LATEX) $(DOC) && $(BIB) $(NOM) | make latex

latex: 
	$(LATEX) $(DOC) && $(LATEX) $(DOC)

# Pasa de .dvi a .pdf
dvi-pdf: 
	$(DVI) $(NOM).tex

clean: 
	@$(RM) *.aux *.toc *.log \
	*.lof *.lot *.out *.bbl \
	*.blg *~ *\#
	@$(RM) -r auto

clean-all:
	@make clean
	@$(RM) $(NOM).pdf $(NOM).dvi
