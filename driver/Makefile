KERNEL_VERSION ?= $(shell uname -r)
KERNEL_PATH ?= /lib/modules/$(KERNEL_VERSION)/build

obj-m += parport_gpio.o

all: modules

modules clean:
	make -C $(KERNEL_PATH) M=$(shell pwd) $@
