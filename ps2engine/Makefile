#=============================================================================
# _____     ___ ____                ____
#  ____|   |    ____| |     |    | |____|
# |     ___|   |____  |____ |____| |    |
# ----------------------------------------
# Open Source Lua Game Engine
#
# File:        Makefile
#
# Description: Makefile for ps2engine
#
#=============================================================================

EE_BIN=ps2engine.elf

EE_INCS=	-I$(PS2LUA_SRC)/ps2gl/include \
			-I$(PS2LUA_SRC)/ps2stuff/include
			
EE_LIBS=	-L$(PS2LUA_SRC)/ps2gl/ -lps2gl \
			-L$(PS2LUA_SRC)/ps2stuff/ -lps2stuff \
			-ldma -lgraph -lmath3d -lpad

EE_CFLAGS+=-DPLATFORM_PS2
EE_CXXFLAGS+=-DPLATFORM_PS2

EE_OPTFLAGS=-O0
EE_WARNFLAGS=-Wall -fno-strict-aliasing

EE_EMBED=

all: $(EE_BIN)
#	$(EE_STRIP) $(EE_BIN)

clean: 
	rm -rf $(EE_OBJ) $(EE_BIN) $(EE_EMBED)
	
%.irx: $(PS2SDK)/iop/irx/%.irx
	cp $< $@

include ../Makefile.project