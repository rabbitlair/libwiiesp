#--------------------------------------------------------------------------- 
# Licencia GPLv3
# 
# Este archivo es parte de libWiiEsp, y su autor es Ezequiel Vázquez De la Calle
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
LOCALLIBS = 

# Nombre del proyecto, del ejecutable y  de los directorios directorios fuente y objeto
PROJECT = Arkanoid
TARGET = boot
BUILD = build
SOURCE = src
DEPSDIR = $(BUILD)

# Número de la versión
VERSION = 0.1

#---------------------------------------------------------------------------
# Parte estática
#---------------------------------------------------------------------------

# Limpiar las reglas implícitas
.SUFFIXES:

# Incluir las reglas básicas para Wii
include $(DEVKITPPC)/wii_rules

# Incluir las reglas de compilación de LibWiiEsp
include $(DEVKITPPC)/libwiiesp_rules

# Generar una lista con todos los ficheros objeto del proyecto
CPPFILES = $(notdir $(wildcard $(SOURCE)/*.cpp))
OFILES = $(CPPFILES:.cpp=.o)
OBJS = $(addprefix $(CURDIR)/$(BUILD)/,$(OFILES))

# Variables para la compilación
INCLUDE = $(foreach dir,$(LOCALLIBS),-I$(CURDIR)/lib/$(dir)) -I$(LIBOGC_INC)
LIBPATHS = -L$(LIBOGC_LIB) -L$(CURDIR)/$(BUILD)
VPATH = $(CURDIR)/$(SOURCE)$(foreach dir,$(LOCALLIBS),:$(CURDIR)/lib/$(dir))
LIBS = $(LIBWIIESP_LIBS)
OUTPUT = $(CURDIR)/$(TARGET)
LD = $(CXX)

# Flags para la compilación
CFLAGS		= -g -ansi -Wall $(MACHDEP) $(INCLUDE) -std=c++0x
CXXFLAGS	= $(CFLAGS)
OPTIONS		= -MMD -MP -MF
LDFLAGS		= -g $(MACHDEP) -Wl,-Map,$(notdir $@).map

#---------------------------------------------------------------------------

.PHONY: all $(BUILD) libs $(LOCALLIBS) listo run clean

all: $(BUILD) libs $(OUTPUT).dol listo

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

listo:
	@$(RM) $(CURDIR)/$(SOURCE)/-g $(OUTPUT).elf.map
	@echo 
	@echo $(PROJECT) ... OK!

run:
	wiiload $(TARGET).dol

clean:
	@for dir in $(LOCALLIBS); do $(MAKE) clean --no-print-directory -C lib/$$dir; echo Limpiando $$dir ... OK! ; done
	@$(RM) -fr $(BUILD) $(OUTPUT).elf $(OUTPUT).dol *~ $(SOURCE)/*~
	@echo Limpiando $(PROJECT) ... OK!

dist: all
	@echo Empaquetando arkanoid-$(VERSION).tar.gz ...
	@mkdir apps apps/arkanoid apps/arkanoid/xml apps/arkanoid/media
	@cp boot.dol apps/arkanoid
	@cp meta.xml apps/arkanoid
	@cp icon.png apps/arkanoid
	@rsync -ah --exclude="*.svn*" xml/* apps/arkanoid/xml
	@rsync -ah --exclude="*.svn*" media/* apps/arkanoid/media
	@tar -czf arkanoid-$(VERSION).tar.gz apps AUTHOR LICENSE
	@$(RM) -fr apps
	@echo arkanoid-$(VERSION).tar.gz ... OK!
	

#---------------------------------------------------------------------------

DEPENDS	:=	$(OBJS:.o=.d)

$(OUTPUT).dol: $(OUTPUT).elf

$(OUTPUT).elf: $(OBJS)

-include $(DEPENDS)

