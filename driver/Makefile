KERNEL_VERSION ?= $(shell uname -r)
KERNEL_PATH ?= /lib/modules/$(KERNEL_VERSION)/build

ccflags-y = -DCONFIG_PARPORT_NOT_PC -DCONFIG_PARPORT_1284

ifndef KERNEL_HAS_PARPORT
obj-m += parport/
endif

obj-m += parport_gpio.o

all: modules

modules clean:
	make -C $(KERNEL_PATH) M=$(shell pwd) $@

check:
	scripts/checkpatch.pl --no-tree -f parport_gpio.c
