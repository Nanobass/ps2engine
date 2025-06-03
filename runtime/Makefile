EE_BIN = runtime.elf


EE_INCS= \
		-I$(PS2LUA_SRC)/ps2gl/ee/include \
		-I$(PS2LUA_SRC)/core/ee/include \

EE_LIBS= \
		-L$(PS2LUA_SRC)/ps2gl/ -lps2gl \
		-L$(PS2LUA_SRC)/core/ -lcore \
		-lpatches -ldma -ldebug -lfileXio -lpad -lmath3d -lpng -lz

EE_CFLAGS += -DNEWLIB_PORT_AWARE
EE_CXXFLAGS += -DNEWLIB_PORT_AWARE 

EE_OPTFLAGS += -O2
EE_WARNFLAGS += -Wall

all: $(EE_BIN)
#	$(EE_STRIP) $(EE_BIN)
clean: 
	rm -rf $(EE_OBJ) $(EE_BIN)

include ../Makefile.project