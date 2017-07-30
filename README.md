## pi-parport

This is a PC style parallel port for the Raspberry Pi.  Seventeen GPIO pins
are organized as data, status, and control registers.  Three 74AHCT541 buffers
convert to/from the Pi's 3V3 line level and the parallel port's 5V.  A
`parport-gpio` driver integrates the port with the Linux parport driver stack.

This project is a pretty bare bones parallel port.  It is unidirectional,
not interrupt driven, and only supports SPP mode.  In theory one ought to
be able to drive an early parallel printer with this, though I haven't
tried yet.

This is prototype quality code and hardware, however it's working as of
30 July 2017 for the one use case I had, which is to interface to parallel
port SBIG astronomy cameras.  SBIG provided a Linux device driver that
basically bit-bangs their protocol on the 8 data out and 5 status in
lines.  I updated it and changed it to use the parport stack so it didn't
need to hardwire legacy port addresses etc..  That made it possible to use
on non-PC hardware, provided there was a parport driver available, hence
this little project.

### Using

There is a device tree overlay for mapping specific GPIO pins to
their functions in the driver.  This must be compiled and loaded.
In the `dts` directory, run
```
$ make
$ sudo make install
```
and add the following line to /boot/config.txt:
```
dtoverlay=parport-gpio
```
After rebooting, run from the `drivers` directory:
```
$ make
$ sudo insmod parport.ko
$ sudo insmod parport_gpio.ko
```
If you were doing what I'm doing, this is where you'd go over to
the sbig driver directory and run
```
$ sudo insmod sbiglpt.ko
```
after which you could probe your camera.

### Next Steps

The first prototype board isn't offical HAT size, lacks an EEPROM,
lacks termination, and probably should have a 26-pin dual row
connector instead of the larger DB25 footprint.

The driver OOPSes in `parport_daisy_init()` if the parport stack is
built with 1284 support, so that should be run down.

Depending on what other uses are found for this project, we might
want to implement bidirectional ports, EPP mode, interrupts, etc..

One could make a hobby out of shoving Raspberry Pi's in old hardware
to make them useful again.  If anyone wants to drive this idea somewhere
fun, I'll gladly cheer them on!
