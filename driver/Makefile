KERNEL_VERSION ?= $(shell uname -r)
KERNEL_PATH ?= /lib/modules/$(KERNEL_VERSION)/build
MODDIR=/lib/modules/`uname -r`

ccflags-y = -DCONFIG_PARPORT_NOT_PC -DCONFIG_PARPORT_1284

ifndef KERNEL_HAS_PARPORT
obj-m += parport/
endif

obj-m += parport_gpio.o

all: modules

modules clean:
	$(MAKE) -C $(KERNEL_PATH) M=$(shell pwd) $@

check:
	scripts/checkpatch.pl --ignore=LINUX_VERSION_CODE \
	  --no-tree -f parport_gpio.c

install: parport_gpio.ko
	mkdir -p $(MODDIR)/pi-parport
	install -m 644 $^ $(MODDIR)/pi-parport
	$(MAKE) -C parport install

uninstall:
	$(MAKE) -C parport uninstall
	rm -rf $(MODDIR)/pi-parport
