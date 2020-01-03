CHIP=24c32
ADDRESS=50
I2C_BUS=0

all: board.eep

board.eep: board.txt
	eepmake $< $@

flash: board.eep
	sudo eepflash.sh -w -f=$< -t=$(CHIP) -d=$(I2C_BUS) -a=$(ADDRESS)

verify:
	@echo "vendor:      $(shell cat /proc/device-tree/hat/vendor)"
	@echo "product:     $(shell cat /proc/device-tree/hat/product)"
	@echo "product_id:  $(shell cat /proc/device-tree/hat/product_id)"
	@echo "product_ver: $(shell cat /proc/device-tree/hat/product_ver)"
	@echo "uuid:        $(shell cat /proc/device-tree/hat/uuid)"

clean:
	rm -f board.eep
