/* walk - visual LED output test
 *
 * DATA0 (2)
 * DATA1 (3)
 * DATA2 (4)
 * DATA3 (5)
 * DATA4 (6)
 * DATA5 (7)
 * DATA6 (8)
 * DATA7 (9)
 * CONTROL0 (1) ~nStrobe
 * CONTROL1 (14) ~nAutoLF
 * CONTROL2 (16) nInitialize
 * CONTROL3 (17) ~nSelect
 */

#include <stdio.h>
#include <sys/ioctl.h>
#include <linux/parport.h>
#include <linux/ppdev.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>

#define DEVICE_PATH "/dev/parport0"

const int invert_outputs = 1; // set to 1 if LED's tied to 5V, 0 if tied to GND

void die (const char *fmt, ...)
{
    va_list ap;

    va_start (ap, fmt);
    (void)vfprintf (stderr, fmt, ap);
    va_end (ap);
    exit (1);
}

void out_data (int fd, int out)
{
    if (invert_outputs)
        out = ~out;
    if (ioctl (fd, PPWDATA, &out) < 0)
        die ("ioctl PPWDATA: %s\n", strerror (errno));
}

void wiggle_data (int fd, int out)
{
    int i;
    int d = 0;
    out_data (fd, 0);
    for (i = 0; i < 16; i++) {
        out_data (fd, out & d);
        usleep (100*1000); // 100ms
        d = ~d;
    }
    out_data (fd, 0);
}

void out_control (int fd, int out)
{
    out ^= (1<<0); // invert ~nStrope
    out ^= (1<<1); // invert ~nAutolf
    out ^= (1<<3); // invert ~nSelect
    if (invert_outputs)
        out = ~out;
    out &= 0x0f; // don't set upper 4 bits of CONTROL
    if (ioctl (fd, PPWCONTROL, &out) < 0)
        die ("ioctl PPWCONTROL: %s\n", strerror (errno));
}

void wiggle_control (int fd, int out)
{
    int i;
    int d = 0;
    out_control (fd, 0);
    for (i = 0; i < 16; i++) {
        out_control (fd, out & d);
        usleep (100*1000); // 100ms
        d = ~d;
    }
    out_control (fd, 0);
}

int main (int argc, char *argv[])
{
    int fd, mode, dir;
    unsigned char out;
    unsigned char in;
    int iter, i, j;
    int errors;

    fd = open (DEVICE_PATH, O_RDWR);
    if (fd < 0)
        die ("%s: %s\n", DEVICE_PATH, strerror (errno));
    if (ioctl (fd, PPCLAIM, NULL) < 0)
        die ("ioctl PPCLAIM: %s\n", strerror (errno));

    mode = IEEE1284_MODE_BYTE;
    if (ioctl (fd, PPSETMODE, &mode) < 0)
        die ("ioctl PPSETMODE IEEE1284_MODE_BYTE: %s\n", strerror (errno));
    dir = 0;
    if (ioctl (fd, PPDATADIR, &dir) < 0)
        die ("ioctl PPDATADIR 0 (set output): %s\n", strerror (errno));

    out_data (fd, 0);
    out_control (fd, 0);

    sleep (2);

    for (i = 0; i < 8; i++)
        wiggle_data (fd, 1<<i);
    for (i = 0; i < 4; i++)
        wiggle_control (fd, 1<<i);

    sleep (2);

    if (ioctl (fd, PPRELEASE) < 0)
        die ("ioctl PPRELEASE: %s\n", strerror (errno));
    if (close(fd) < 0)
        die ("close %s: %s\n", DEVICE_PATH, strerror (errno));

   return 0;
}

/*
 * vi:tabstop=4 shiftwidth=4 expandtab
 */
