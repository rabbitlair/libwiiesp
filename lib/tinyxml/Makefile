#---------------------------------------------------------------------------------
# Clear the implicit built in rules
#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITPPC)),)
$(error "Please set DEVKITPPC in your environment. export DEVKITPPC=<path to>devkitPPC")
endif

include $(DEVKITPPC)/wii_rules

#---------------------------------------------------------------------------------
# TARGET is the name of the output
#---------------------------------------------------------------------------------
TARGET		:=	libtinyxml

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------

CFLAGS	= -g -O2 -Wall $(MACHDEP)
CXXFLAGS	=	-save-temps -Xassembler -aln=$@.lst $(CFLAGS)

LDFLAGS	= -g $(MACHDEP) -Wl,-Map,$(notdir $@).map 

#---------------------------------------------------------------------------------
# any extra libraries we wish to link with the project
#---------------------------------------------------------------------------------
LIBS	:= -lfat -logc -lm

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
export OUTPUT	:=	$(CURDIR)/$(TARGET).a
export VPATH	:=	.
export DEPSDIR	:=	.

#---------------------------------------------------------------------------------
# automatically build a list of object files for our project
#---------------------------------------------------------------------------------
CFILES		:=	$(notdir $(wildcard $(CURDIR)/*.c))
CPPFILES	:=	$(notdir $(wildcard $(CURDIR)/*.cpp))

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
	export LD	:=	$(CC)
else
	export LD	:=	$(CXX)
endif

export OFILES	:=	$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(sFILES:.s=.o) $(SFILES:.S=.o)
export INCLUDE	:=	-I$(LIBOGC_INC)
export LIBPATHS	:=	-L$(LIBOGC_LIB)
export OUTPUT	:=	$(CURDIR)/$(TARGET)

#---------------------------------------------------------------------------------
.PHONY: all clean clean-dist

all: $(OUTPUT).a clean-dist

#---------------------------------------------------------------------------------
%.a:
	@$(AR) -rc $@ $^

#---------------------------------------------------------------------------------
%.o : %.cpp
	@$(CXX) -MMD -MP -MF $(DEPSDIR)/$*.d $(CFLAGS) -Wa,-mgekko -c $< -o $@

#---------------------------------------------------------------------------------
clean:
	@rm -fr $(OFILES) *.d *.ii *.o.lst *.s *~

clean-dist:
	@rm -fr *.d *.ii *.o.lst *.s *~

#---------------------------------------------------------------------------------
DEPENDS	:=	$(OFILES:.o=.d)

$(OUTPUT).a: $(OFILES)

-include $(DEPENDS)

