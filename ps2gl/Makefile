EE_LIB=libps2gl.a

EE_INCS= -Ivu1

RENDERERS = \
	fast_nolights \
	fast \
	general_nospec_quad \
	general_nospec_tri \
	general_nospec \
	general_pv_diff_quad \
	general_pv_diff_tri \
	general_pv_diff \
	general_quad \
	general_tri \
	general \
	indexed \
	scei
	
EE_EXTRA_OBJECTS = $(addsuffix .o, $(addprefix $(EE_OBJ)/, $(RENDERERS)))

#VSM_SOURCES = $(addsuffix _vcl.vsm, $(addprefix vu1/, $(RENDERERS)))

# Disabling warnings
WARNING_FLAGS = -Wno-strict-aliasing -Wno-conversion-null 

# VU0 code is broken so disable for now
EE_CFLAGS   += $(WARNING_FLAGS) -DNO_VU0_VECTORS -DNO_ASM
EE_CXXFLAGS += $(WARNING_FLAGS) -DNO_VU0_VECTORS -DNO_ASM

all: $(EE_LIB)
clean: 
	rm -rf $(EE_OBJ) $(EE_LIB)

include ../Makefile.project

$(EE_OBJ)/%.o: vu1/%_vcl.vsm
	dvp-as -o $@ $<

vu1/%_vcl.vsm: vu1/%_pp4.vcl
	vcl -o$@ $<

vu1/%indexed_pp4.vcl: vu1/%indexed_pp3.vcl
	cat $< | cc -E -P -imacros vu1/vu1_mem_indexed.h -o $@ -

vu1/%_pp4.vcl: vu1/%_pp3.vcl
	cat $< | cc -E -P -imacros vu1/vu1_mem_linear.h -o $@ -

vu1/%_pp3.vcl: vu1/%_pp2.vcl
	cat $< | sed 's/\[\([0-9]\)\]/_\1/g ; s/\[\([w-zW-Z]\)\]/\1/g' - > $@

vu1/%_pp2.vcl: vu1/%_pp1.vcl
	gasp -c ';' -Ivu1 -o $@ $<

vu1/%_pp1.vcl: vu1/%.vcl
	cat $< | sed 's/#include[ 	]\+.\+// ; s/#define[ 	]\+.\+// ; s|\(\.include[ 	]\+\)"\([^/].\+\)"|\1"$(<D)/\2"|' - > $@