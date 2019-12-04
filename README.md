## pi-parport

This is a Raspberry Pi HAT that implements a PC-style parallel port.
GPIO pins are organized as data, status, and control registers.
The HAT includes a buffer chip that converts from the GPIB 3V3 logic
to signals that conform to IEEE 1284.  A 26 pin IDC header on the
board connects to your DB25 connector.  A `parport-gpio` driver and
device tree overlay integrate the HAT with the Linux parport driver stack.

### Why?

I wanted to use a parallel port based cooled astronomy camera,
but preferred to use the compact and low-power Raspberry Pi
over a PC at the telescope.  Parallel ports are simple devices.
Why not build one?

### What works?

See [wiki](https://github.com/worlickwerx/pi-parport/wiki) for notes on some
devices, and please share your experience if you try something new.

The following limitations affect some software and hardware that uses the
parallel port:
1) The software cannot read and write the x86 port addresses directly.
In user space, it must use the ppdev ioctl interface.  In the kernel, it must
use the (generic) `struct parport` operations.
2) The extra latency of GPIO multiplexing/de-multiplexing and/or system call
access may impact the direct-access latency assumed by some bit-banged drivers.
On a Pi 3, user space code can toggle a parallel port line at about 250 kHz.

### Why not use a USB Parallel Port?

Many USB parallel port devices appear to Linux as a USB printer, bypassing
the parport driver stack.  If your application is a printer, go for it.

### Building the driver

For convenience, the generic parport code is duplicated in this repo,
since the Raspberry Pi Foundation does not ship the compiled modules in
the `raspberrypi-kernel` package.

Clone this repo, then run the following with `${TOPDIR}` representing the
top level directory of the cloned repo:
```console
$ sudo apt install build-essential raspberrypi-kernel-headers
$ cd ${TOPDIR}/driver
$ make
make -C /lib/modules/4.19.75-v7+/build M=${TOPDIR}/driver modules
make[1]: Entering directory '/usr/src/linux-headers-4.19.75-v7+'
  CC [M]  ${TOPDIR}/driver/parport/share.o
  CC [M]  ${TOPDIR}/driver/parport/ieee1284.o
  CC [M]  ${TOPDIR}/driver/parport/ieee1284_ops.o
  CC [M]  ${TOPDIR}/driver/parport/procfs.o
  CC [M]  ${TOPDIR}/driver/parport/daisy.o
  CC [M]  ${TOPDIR}/driver/parport/probe.o
  LD [M]  ${TOPDIR}/driver/parport/parport.o
  CC [M]  ${TOPDIR}/driver/parport/lp.o
  CC [M]  ${TOPDIR}/driver/parport/ppdev.o
  CC [M]  ${TOPDIR}/driver/parport_gpio.o
  Building modules, stage 2.
  MODPOST 4 modules
  CC      ${TOPDIR}/driver/parport/lp.mod.o
  LD [M]  ${TOPDIR}/driver/parport/lp.ko
  CC      ${TOPDIR}/driver/parport/parport.mod.o
  LD [M]  ${TOPDIR}/driver/parport/parport.ko
  CC      ${TOPDIR}/driver/parport/ppdev.mod.o
  LD [M]  ${TOPDIR}/driver/parport/ppdev.ko
  CC      ${TOPDIR}/driver/parport_gpio.mod.o
  LD [M]  ${TOPDIR}/driver/parport_gpio.ko
make[1]: Leaving directory '/usr/src/linux-headers-4.19.75-v7+'
```

### Installing the Device Tree Overlay

The device tree overlay maps specific GPIO pins to their functions in
the driver.  This must be compiled and loaded:
```console
$ cd ${TOPDIR}/dts
$ make
$ sudo make install
```
then add the following line to `/boot/config.txt`:
```
dtoverlay=parport-gpio
```
Then reboot the pi to pick up the new config.

### Loading the Modules

After rebooting, load the base modules:
```console
$ cd ${TOPDIR}
$ sudo insmod parport/parport.ko
$ sudo insmod parport_gpio.ko
```
You'll see the port announce itself on the console:
```console
[4425200.988400] parport-gpio ppgpio@0: data on pins [22,23,24,10,25,9,8,11]
[4425200.988412] parport-gpio ppgpio@0: status on pins [18,17,4,3,2]
[4425200.988422] parport-gpio ppgpio@0: control on pins [26,19,6,13]
[4425200.988430] parport-gpio ppgpio@0: hd on pin 20
[4425200.988438] parport-gpio ppgpio@0: dir on pin 21
```
If you wish to set up a parallel printer:
```console
$ sudo insmod parport/lp.ko
```
If you wish to run user-space programs that use the parallel port:
```console
$ sudo insmod parport/ppdev.ko
```

### v3 hardware

This version is similar to v2.

It adds an ID EEPROM on `ID_SC`, `ID_SD` per the
[Raspberry Pi HAT Specification](https://github.com/raspberrypi/hats).

The schematic and board from v2 were redone using
[KiCAD](https://www.kicad-pcb.org/) to make it easier for others to improve
and remix, since KiCAD is free software.

TODO: test boards when we get them back, and add BOM

### v2 hardware

v2 addresses the line termination and data direction limitations of v1,
using a single, purpose-built chip, the
[SN74LVC161284 19-bit bus interface with 3-state outputs](http://www.ti.com/product/SN74LVC161284).

The board is shared on [OSHPARK](https://oshpark.com/shared_projects/Padn3qhP).
See also: [Schematic](hardware_v2/schematic.pdf) and [Bill of Materials](hardware_v2/BOM.md).

### v1 hardware

Three
[74AHCT541 octal buffers/drivers with 3-state outputs](http://www.ti.com/product/SN74AHCT541)
chips convert to/from the Pi's 3V3 line level and the parallel port's 5V.
This design does not implement bidirectional DATA lines, and ignores
line termination.  It worked for my intended application, but only with a
very short cable.

### Release

Share and enjoy!

#### Software License

SPDX-License-Identifier: GPL-2.0-or-later

#### Hardware License

SPDX-License-Identifier: CERN-OHL-1.2
