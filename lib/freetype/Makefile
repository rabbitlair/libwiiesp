# Quick'n'dirty makefile [BC]

ifeq ($(strip $(DEVKITPPC)),)
  $(error "Use   export DEVKITPPC=<path to>devkitPPC   and try again")
endif

ifeq ($(strip $(DEVKITPRO)),)
  $(error "Use   export DEVKITPRO=<path to>devkitPRO   and try again")
endif

# FreeType Makefile
PROJECT       := freetype
PROJECT_TITLE := FreeType
TOP_DIR ?= .
OBJ_DIR ?= $(TOP_DIR)
include $(TOP_DIR)/builds/toplevel.mk

clean :
	@rm -f lib$(PROJECT).a ftmodule.h
