EE_BIN = runtime.elf


EE_INCS= \
		-I$(PS2ENGINE_SRC)/core/ee/include \
		-I$(PS2ENGINE_SRC)/ps2gl/ee/include \

EE_LIBS= \
		-L$(PS2ENGINE_SRC)/core/ -lcore \
		-L$(PS2ENGINE_SRC)/ps2gl/ -lps2gl \
		-lpatches -ldma -ldebug -lfileXio -lpad

EE_CFLAGS += -DNEWLIB_PORT_AWARE
EE_CXXFLAGS += -DNEWLIB_PORT_AWARE 

EE_OPTFLAGS += -O2
EE_WARNFLAGS += -Wall

all: $(EE_BIN)
#	$(EE_STRIP) $(EE_BIN)
clean: 
	rm -rf $(EE_OBJ) $(EE_BIN)

include ../Makefile.project