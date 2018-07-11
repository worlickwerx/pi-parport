## pi-parport

This is a PC style parallel port for the Raspberry Pi.  GPIO pins
are organized as data, status, and control registers.  A simple "Pi HAT"
provides TTL level conversion, line conditioning, and a connector.
A `parport-gpio` driver integrates the port with the Linux parport driver stack.

### Why?

I wanted to use a parallel port based cooled astronomy camera,
but preferred to use the compact and low-power Raspberry Pi
over a PC at the telescope.  Parallel ports are simple devices.
Why not build one?

### Using

The Raspberry Pi Foundation isn't likely to configure their kernel with the
parallel port stack anytime soon given the lack of hardware availability.
For convenience, the generic parport code is included here so you can
build it without needing to reconfigure the kernel.  This includes the
`parport`, `lp`, and `ppdev` modules.  The original code for this project
consists of only `parport_gpio.c`.

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
You'll see the port announce itself on the console:
```
parport0: data on gpio pins [22,23,24,10,25,9,8,11]
parport0: status on gpio pins [18,17,4,3,2]
parport0: control on gpio pins [26,19,6,13]
parport0: hd on gpio pin 20
parport0: dir on gpio pin 21
```
If you wanted to bring up a printer, then you would
```
$ sudo insmod lp.ko
```
after which you should be able to configure CUPS to network-share your
printer.

#### SBIG parallel port cameras

The motivation for this project for was to support SBIG parallel port astronomy
cameras.  SBIG provided a Linux device driver that basically bit-bangs their
protocol on the 8 data out and 5 status in lines.  I updated it and changed
it to use the parport stack so it didn't need to hardwire the legacy PC port
addresses.  That made it possible to use on non-PC hardware, provided there
was a parport driver available.

To use, go over to the sbig driver directory, follow instructions for
installing its udev rules, then after loading the `parport` and `parport-gpio`
modules,
```
$ sudo insmod sbiglpt.ko
```
You'll see sbiglpt acquire the parallel port on the console:
```
sbiglpt0: using parport0
```
after which SBIG-enabled applications based on their SDK will find the LPT1
camera.

### v2 hardware

This revision of the hardware implements proper IEEE 1284 line conditioning
and bidirectional communication with the
[SN74LVC161284 19-bit bus interface with 3-state outputs](http://www.ti.com/product/SN74LVC161284).

The board is shared on [OSHPARK](https://oshpark.com/shared_projects/Padn3qhP).

### v1 hardware

Three
[74AHCT541 octal buffers/drivers with 3-state outputs](http://www.ti.com/product/SN74AHCT541)
chips convert to/from the Pi's 3V3 line level and the parallel port's 5V.
This implements a unidirectional SPP parallel port.  It doesn't include
line termination and thus is only likely to work with short cables.
