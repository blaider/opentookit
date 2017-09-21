DRIVER_NAME = chartest
TARGET_FILE_NAME = $(DRIVER_NAME).ko

REVISION_NUMBER=$(shell svnversion -n .)
ifneq ($(KERNELRELEASE),)
obj-m := $(DRIVER_NAME).o
$(DRIVER_NAME)-objs := chardev.o

ARCH=arm
CROSS_COMPILE = arm-linux-gnueabihf-
CC := arm-linux-gnueabihf-gcc
LD := arm-linux-gnueabihf-ld
#DEVICE_NAME :=ECU1152

EXTRA_CFLAGS += $(DEVICE_NAME:%=-D %) #-DDAQ_TRACING
else
KO_PATH ?= ../../../bin
SYMBOL_PATH ?= ../../../lib
ifndef OUT_DIR
OUT_DIR			:= $(shell pwd)/../output
endif

ifndef MKDIR
ifdef SystemRoot 						# for Windows sytem
MKDIR			:= mkdir.exe
else
MKDIR			:= mkdir
endif
endif

KERNELDIR ?= /media/kernel
all:
	@$(MKDIR) -p $(OUT_DIR)
	@echo "#define REVISION_NUMBER \""$(REVISION_NUMBER)\" > svnversion.h
	@echo "#define VERSION_NUMBER \""$(VERSION_NUMBER)\" >> svnversion.h
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-
	-cp -f $(DRIVER_NAME).ko $(OUT_DIR)/

endif
clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
	-rm -rf *.o *~ .depend .*.cmd *.ko *.mod.c .tmp_versions modules.order Module.symvers $(OUT_DIR)/$(DRIVER_NAME).ko


.PHONY: all clean
