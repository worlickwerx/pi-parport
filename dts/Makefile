all: parport-gpio.dtbo

parport-gpio.dtbo: parport-gpio.dts
	dtc -@ -I dts -O dtb -o $@ $<

install: parport-gpio.dtbo
	cp $^ /boot/overlays/

uninstall:
	rm -f /boot/overlays/parport-gpio.dtbo

clean:
	rm -f *.dtbo
