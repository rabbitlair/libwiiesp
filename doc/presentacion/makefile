# makefile por Noelia Sales Montes

NOM = presentacion
PRES = ${NOM}.tex
PDF = presentacion.pdf

all: clean ${PDF}

pdf: ${PDF}

${PDF}: ${PRES}
	pdflatex ${PRES}
	pdflatex ${PRES}
	pdflatex ${PRES}


clean: 
	rm -f *.ps *.dvi *.aux *.toc *.idx *.ind *.ilg \
	*.log *.out *.snm *.nav *.vrb *~ ${PDF}
