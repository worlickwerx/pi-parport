## pi-parport

This is a PC style parallel port for the Raspberry Pi.  Seventeen GPIO pins
are organized as data, status, and control registers.  Three
[74AHCT541 octal buffers/drivers with 3-state outputs](http://www.ti.com/product/SN74AHCT541)
chips convert to/from the Pi's 3V3 line level and the parallel port's 5V.  A
`parport-gpio` driver integrates the port with the Linux parport driver stack.

This project is a pretty bare bones parallel port.  Since it's unidirectional,
it won't support scanners, zip drives, or devices that require ECP/EPP modes.
It will, however, work with some parallel printers - at least it worked with
the one I had on hand, an HP Deskjet 1220C.

The application I built this for, however, was parallel port SBIG astronomy
cameras.  SBIG provided a Linux device driver that basically bit-bangs their
protocol on the 8 data out and 5 status in lines.  I updated it and changed
it to use the parport stack so it didn't need to hardwire the legacy PC port
addresses.  That made it possible to use on non-PC hardware, provided there
was a parport driver available, hence this little project.

### Using

Since the Raspberry Pi Foundation isn't likely to configure the general
parallel port code without hardware availability, that code (`parport.ko`
and `lp.ko`) is included here in the drivers directory for convenience.
The original code for this project consists of only `parport_gpio.c`.

There is a device tree overlay for mapping specific GPIO pins to
their functions in the driver.  This must be compiled and loaded.
In the `dts` directory, run
```
$ make
$ sudo make install
```
and add the following line to `/boot/config.txt`:
```
dtoverlay=parport-gpio
```
After rebooting, run from the `drivers` directory:
```
$ make
$ sudo insmod parport.ko
$ sudo insmod parport_gpio.ko
```
If you wanted to bring up a printer, then you would
```
$ sudo insmod lp.ko
```
after which you should be able to configure CUPS to network-share your
printer.

Or if you want to play with the SBIG cameras, you would go over to
the sbig driver directory and run
```
$ sudo insmod sbiglpt.ko
```
after which SBIG-enabled applications will find the LPT1 camera.

### Next Steps

A version 2 of the hardware might have the following improvements:
* reduce size to official
[Raspberry Pi HAT](https://github.com/raspberrypi/hats) dimensions
* include an EEPROM to automate the device tree overlay/driver loading
* implement bidirectional data port
* add termination as defined in IEEE 1284
* use the [SN74LVC161284 19-bit bus interface with 3-state outputs](http://www.ti.com/product/SN74LVC161284)
