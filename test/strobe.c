// SPDX-License-Identifier: GPL-2.0-or-later

/* strobe - pulse specified pin for scope */

#include <stdio.h>
#include <sys/ioctl.h>
#include <linux/parport.h>
#include <linux/ppdev.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>

#define DEVICE_PATH "/dev/parport0"

void die (const char *fmt, ...)
{
    va_list ap;

    va_start (ap, fmt);
    (void)vfprintf (stderr, fmt, ap);
    va_end (ap);
    exit (1);
}

void say (const char *fmt, ...)
{
    va_list ap;

    va_start (ap, fmt);
    (void)vfprintf (stderr, fmt, ap);
    va_end (ap);
}

void usage (void)
{
    fprintf (stderr, "Usage: strobe pin\n");
    fprintf (stderr, "where pin is DATA0, DATA1, CONTROL0, CONTROL1...\n");
    exit (1);
}

/* measured 253khz in a tight loop on pi3 */
void strobe (int fd, const char *pin)
{
    unsigned char out;
    unsigned long request;
    int bit;
    int count;

    if (!strncmp (pin, "DATA", 4)) {
        bit = atoi (pin + 4);
        if (bit < 0 || bit > 7)
            usage ();
        request = PPWDATA;
    }
    else if (!strncmp (pin, "CONTROL", 7)) {
        bit = atoi (pin + 7);
        if (bit < 0 || bit > 4)
            usage ();
        request = PPFCONTROL;
    }
    else
        usage ();

    for (count = 0; ; count++) {
        if (request == PPWDATA) {
            unsigned char out = (count % 2) ? 0 : (1<<bit);

            if (ioctl (fd, request, &out) < 0)
                die ("ioctl PPWDATA: %s\n", strerror (errno));
        }
        else {
            struct ppdev_frob_struct frob = {
                .mask = 1<<bit,
                .val = (count % 2) ? 0 : (1<<bit),
            };
            if (ioctl (fd, request, &frob) < 0)
                die ("ioctl PPFCONTROL: %s\n", strerror (errno));
        }
    }
}

int main (int argc, char *argv[])
{
    int fd, mode, dir;
    const char *pin;
    int rc;

    if (argc != 2)
        usage ();
    pin = argv[1];

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
        die ("ioctl PPDATADIR 0 (out): %s\n", strerror (errno));

    strobe (fd, pin);

    if (ioctl (fd, PPRELEASE) < 0)
        die ("ioctl PPRELEASE: %s\n", strerror (errno));
    if (close(fd) < 0)
        die ("close %s: %s\n", DEVICE_PATH, strerror (errno));

   return rc;
}

/*
 * vi:tabstop=4 shiftwidth=4 expandtab
 */
