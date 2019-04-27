KERNEL_VERSION ?= $(shell uname -r)
KERNEL_PATH ?= /lib/modules/$(KERNEL_VERSION)/build

ccflags-y = -DCONFIG_PARPORT_NOT_PC -DCONFIG_PARPORT_1284

obj-m += parport.o lp.o ppdev.o

parport-y = share.o ieee1284.o ieee1284_ops.o procfs.o daisy.o probe.o

all: modules

modules clean:
	make -C $(KERNEL_PATH) M=$(shell pwd) $@
