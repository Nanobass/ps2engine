
EE_BIN = runtime.elf


EE_INCS= \
		-I$(PS2LUA_SRC)/ps2gl/ee/include \
		-I$(PS2LUA_SRC)/core/ee/include \

EE_LIBS= \
		-L$(PS2LUA_SRC)/ps2gl/ -lps2gl \
		-L$(PS2LUA_SRC)/core/ -lcore \
		-lpatches -ldma -ldebug -lfileXio -lpad -lmath3d -lpng -lz

EE_CFLAGS += -DPLATFORM_PS2 -DNEWLIB_PORT_AWARE
EE_CXXFLAGS += -DPLATFORM_PS2 -DNEWLIB_PORT_AWARE -std=c++23 

EE_OPTFLAGS += -O2
EE_WARNFLAGS += -Wno-deprecated

all: $(EE_BIN)
#	$(EE_STRIP) $(EE_BIN)
clean: 
	rm -rf $(EE_OBJ) $(EE_BIN)

include ../Makefile.project