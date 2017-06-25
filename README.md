## pi-parport

This is an attempt at a simple Raspberry Pi HAT that implements
the old 5V PC parallel port interface using GPIO's, with a Linux
driver that fits into the parport driver stack.

The goal is to interface with older cooled astronomy cameras from
the Santa Barbara Instrument Group
(now [Diffraction Limited](diffractionlimited.com))
that use the parallel port, like the
[ST-9E](http://www.company7.com/sbig/products/st9.html).  The
[Linux driver](http://github.com/garlick/sbig-util/tree/master/lpt_drv) for the
camera was ported to a 4.x kernel, and modified to use the parport stack.
This works on a PC with a parallel port, but those are increasingly hard
to find, and a Raspberry Pi is more convenient to locate at the telescope.

Status 25 June 2017: just built the hardware, now writing the Linux
driver.  As noted in scope trace in
[garlick/sbig-util#21](https://github.com/garlick/sbig-util/issues/21),
the adapter needs to be able to run on the order of 1 MHz, so a critical
test will be whether the GPIO's can go that fast.
