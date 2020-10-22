CHIP=24c32
ADDRESS=50
I2C_BUS=0
EMBED_DTBO=0
VERSION=0x0004

all: board.eep

board.txt: board.txt.in
	sed -e '/product_ver/c\product_ver $(VERSION)' $< >$@

ifeq ($(EMBED_DTBO),1)
overlay.bin: ../dts/parport-gpio.dtbo
		cp $< $@
else
overlay.bin: overlay_name.txt
		cp $< $@
endif

board.eep: board.txt overlay.bin
	eepmake board.txt $@ overlay.bin

flash: board.eep
	sudo eepflash.sh -w -f=$< -t=$(CHIP) -d=$(I2C_BUS) -a=$(ADDRESS)

verify:
	@echo "vendor:      $(shell cat /proc/device-tree/hat/vendor)"
	@echo "product:     $(shell cat /proc/device-tree/hat/product)"
	@echo "product_id:  $(shell cat /proc/device-tree/hat/product_id)"
	@echo "product_ver: $(shell cat /proc/device-tree/hat/product_ver)"
	@echo "uuid:        $(shell cat /proc/device-tree/hat/uuid)"

clean:
	rm -f board.txt overlay.bin board.eep
