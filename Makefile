# _____     ___ ____                ____
#  ____|   |    ____| |     |    | |____|
# |     ___|   |____  |____ |____| |    |
#-----------------------------------------------------------------------

PS2LUA_SRC=$(CURDIR)

SUBDIRS =

# ps2gl
SUBDIRS += ps2stuff/
SUBDIRS += ps2gl/

# engine
SUBDIRS += ps2engine/

MODULE_DIR=irx
MODULES=audsrv.irx libsd.irx sio2man.irx padman.irx ps2dev9.irx ps2atad.irx ps2hdd.irx ps2fs.irx smap.irx netman.irx audsrv.irx iomanX.irx fileXio.irx mcman.irx mcserv.irx

PACKAGE_DIR=project
PACKAGE_CONTENT=ps2engine.elf $(patsubst %.irx,$(MODULE_DIR)/%.irx,$(MODULES))
PACKAGE_FILES=$(patsubst %,$(PACKAGE_DIR)/%,$(PACKAGE_CONTENT))

#
# PACKAGE RULES
#

$(PACKAGE_DIR)/$(MODULE_DIR)/%.irx: $(PS2SDK)/iop/irx/%.irx
	@mkdir -p $(dir $@)
	cp $< $@

$(PACKAGE_DIR)/ps2engine.elf: ps2engine/ps2engine.elf
	@mkdir -p $(dir $@)
	cp $< $@
#
# BUILD PROCESS
#

.DEFAULT_GOAL := build

build: 
	make all
	make package

rebuild: 
	make clean
	make all
	make package

usb:
	make all
	make package
	rm -rf /mnt/k/$(PACKAGE_DIR)
	cp -r $(PACKAGE_DIR)/ /mnt/k/

reset:
	ps2client -h 192.168.26.59 reset

run:
	make all
	make package
	(cd $(PACKAGE_DIR) && ps2client -h 192.168.26.59 execee host:runtime.elf)

all: $(patsubst %, _dir_%, $(SUBDIRS))

$(patsubst %, _dir_%, $(SUBDIRS)):
	@+$(MAKE) -j32 -r -C $(patsubst _dir_%, %, $@) all PS2LUA_SRC=$(PS2LUA_SRC)

clean: $(patsubst %, _cleandir_%, $(SUBDIRS))
	rm -rf $(PACKAGE_FILES)

$(patsubst %, _cleandir_%, $(SUBDIRS)):
	@+$(MAKE) -j32 -C $(patsubst _cleandir_%, %, $@) clean PS2LUA_SRC=$(PS2LUA_SRC)

#
# PACKAGE PROCESS
#

package: $(PACKAGE_FILES)