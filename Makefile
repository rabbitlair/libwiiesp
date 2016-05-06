#--------------------------------------------------------------------------- 
# Licencia GPLv3
# 
# Este archivo es parte de libWiiEsp. Copyright (C) 2011 Ezequiel Vázquez de la Calle
# 
# libWiiEsp es software libre: usted puede redistribuirlo y/o modificarlo bajo los términos de la 
# Licencia Pública General GNU publicada por la Fundación para el Software Libre, ya sea la versión 3 
# de la Licencia, o (a su elección) cualquier versión posterior.
# 
# libWiiEsp se distribuye con la esperanza de que sea útil, pero SIN GARANTÍA ALGUNA; ni siquiera 
# la garantía implícita MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. Consulte los detalles de
# la Licencia Pública General GNU para obtener una información más detallada.
# 
# Debería haber recibido una copia de la Licencia Pública General GNU junto a libWiiEsp. En caso 
# contrario, consulte <http://www.gnu.org/licenses/>.
#---------------------------------------------------------------------------

#---------------------------------------------------------------------------
# Parte configurable
#---------------------------------------------------------------------------

# Información sobre bibliotecas externas
LOCALLIBS = freetype tinyxml
EXTRA = -I$(CURDIR)/lib/freetype/include

# Directorios de fuentes, cabeceras, dependencias, documentacion y objeto
BUILD = build
DEPSDIR = $(BUILD)
DOC = doc
HEADS = include
SOURCE = src

# Número de la versión
VERSION = 0.9.2

#---------------------------------------------------------------------------
# Parte estática
#---------------------------------------------------------------------------

# Limpiar las reglas implícitas
.SUFFIXES:

# Incluir las reglas básicas para Wii
include $(DEVKITPPC)/wii_rules

# Generar listas con todos los ficheros de la biblioteca (cabeceras, fuentes y objetos)
HEADERS = $(notdir $(wildcard $(HEADS)/*.h))
CPPFILES = $(notdir $(wildcard $(SOURCE)/*.cpp))
OFILES = $(CPPFILES:.cpp=.o)
OBJS = $(addprefix $(CURDIR)/$(BUILD)/,$(OFILES))

# Variables para la compilación
INCLUDE = $(foreach dir,$(LOCALLIBS),-I$(CURDIR)/lib/$(dir)) -I$(CURDIR)/$(HEADS) -I$(LIBOGC_INC) $(EXTRA)
LIBPATHS = -L$(LIBOGC_LIB) -L$(CURDIR)/$(BUILD)
VPATH = $(CURDIR)/$(HEADS)$(foreach dir,$(LOCALLIBS),:$(CURDIR)/lib/$(dir))
OUTPUT = $(BUILD)/libwiiesp
LD = $(CXX)

# Flags para la compilación
CFLAGS		= -g -ansi -Wall $(MACHDEP) $(INCLUDE) -std=c++0x
CXXFLAGS	= $(CFLAGS)
OPTIONS		= -MMD -MP -MF
LDFLAGS		= -g $(MACHDEP) -Wl,-Map,$(notdir $@).map

#---------------------------------------------------------------------------

.PHONY: all $(BUILD) libs $(LOCALLIBS) doc dist install uninstall clean doc-clean

all: $(BUILD) libs $(OUTPUT).a

$(BUILD):
	@[ -d $(BUILD) ] || mkdir -p $(BUILD)

libs: $(LOCALLIBS)
	@echo

$(LOCALLIBS):
	@echo
	@echo Compilando $@ ...
	@$(MAKE) --no-print-directory --silent -C lib/$@
	@mv lib/$@/*.a $(CURDIR)/$(BUILD)
	@echo $@ ... OK!

$(CURDIR)/$(BUILD)/%.o: $(CURDIR)/$(SOURCE)/%.cpp
	@echo Procesando $(notdir $<) ...
	@$(CXX) -MMD -MP -MF $(DEPSDIR)/$*.d $(CXXFLAGS) -c $< -o $@ $(ERROR_FILTER)

doc:
	@cd $(DOC)/doxygen ; doxygen ; cd $(CURDIR)
	@$(MAKE) --no-print-directory -C $(DOC)/manual
	@$(MAKE) --no-print-directory -C $(DOC)/memoria

dist: all
	@echo Empaquetando libWiiEsp-$(VERSION).tar.gz
	@mkdir devkitPPC libogc libogc/include libogc/lib libogc/lib/wii
	@cp -u $(CURDIR)/lib/tinyxml/*.h libogc/include
	@cp -ur $(CURDIR)/lib/freetype/include/* libogc/include
	@cp -u $(CURDIR)/$(HEADS)/*.h libogc/include
	@cp -u $(BUILD)/*.a libogc/lib/wii
	@cp -u libwiiesp.mk devkitPPC/libwiiesp_rules
	@tar -czf libWiiEsp-$(VERSION).tar.gz --exclude=*.svn* devkitPPC libogc AUTHOR LICENSE
	@$(RM) -rf devkitPPC libogc libogc/include libogc/lib libogc/lib/wii
	@$(MAKE) --no-print-directory clean
	@echo libWiiEsp-$(VERSION).tar.gz ... OK!

install: all
	@echo
	@cp -u $(CURDIR)/lib/tinyxml/*.h $(LIBOGC_INC)
	@rsync -ah --exclude="*.svn*" $(CURDIR)/lib/freetype/include/* $(LIBOGC_INC)
	@cp -u $(CURDIR)/$(HEADS)/*.h $(LIBOGC_INC)
	@cp -u $(BUILD)/*.a $(LIBOGC_LIB)
	@cp -u libwiiesp.mk $(DEVKITPPC)/libwiiesp_rules
	@echo Instalando libWiiEsp ... OK!

uninstall:
	@$(RM) -rf $(LIBOGC_INC)/tinystr.h $(LIBOGC_INC)/tinyxml.h $(LIBOGC_INC)/ft2build.h $(LIBOGC_INC)/freetype
	@$(RM) -rf $(LIBOGC_LIB)/libtinyxml.a $(LIBOGC_LIB)/libfreetype.a $(LIBOGC_LIB)/libwiiesp.a
	@for file in $(HEADERS); do $(RM) $(LIBOGC_INC)/$$file ; done
	@$(RM) -rf $(DEVKITPPC)/libwiiesp_rules
	@echo Desinstalando libWiiEsp ... OK!

clean:
	@for dir in $(LOCALLIBS); do $(MAKE) clean --no-print-directory -C lib/$$dir; done
	@$(RM) -fr $(BUILD) $(OUTPUT).a *~ $(SOURCE)/*~ $(CURDIR)/$(HEADS)/*~
	@echo Limpiando libWiiEsp ... OK!

doc-clean:
	@$(RM) -fr $(DOC)/doxygen/html $(DOC)/doxygen/latex $(DOC)/doxygen/warnings.log
	@$(MAKE) clean-all --no-print-directory -C $(DOC)/manual
	@$(MAKE) clean-all --no-print-directory -C $(DOC)/memoria
	@echo Limpiando documentacion ... OK!

#---------------------------------------------------------------------------

DEPENDS	:=	$(OBJS:.o=.d)

$(OUTPUT).a: $(OBJS)
	@echo
	@echo Empaquetando libWiiEsp ...
	@$(AR) -rc $@ $^
	@$(RM) -f *.d
	@$(RM) $(CURDIR)/$(SOURCE)/-g
	@echo libWiiEsp ... OK!

-include $(DEPENDS)

