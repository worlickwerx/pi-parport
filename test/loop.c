/* loopback - check with ebay loopback plug
 *
 * DATA1 (3)  -  (14) CONTROL1  ~nAutoLF
 * DATA2 (4)  -  (15) STATUS3   nError
 * DATA3 (5)  -  (17) CONTROL3  ~nSelect
 * DATA4 (6)  -  (13) STATUS4   Select
 * DATA5 (7)  -  (12) STATUS5   Paperout
 * DATA6 (8)  -  (10) STATUS6   nAck
 * DATA7 (9)  -  (11) STATUS7   ~Busy
 *
 * Not wired:
 * CONTROL0 (1)
 * DATA0 (2)
 * CONTROL2 (16)
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
#define ITERATIONS 16

void die (const char *fmt, ...)
{
    va_list ap;

    va_start (ap, fmt);
    (void)vfprintf (stderr, fmt, ap);
    va_end (ap);
    exit (1);
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

    /* DATA[2,4:7] => STATUS[3,4:7]
     */
    dir = 0;
    if (ioctl (fd, PPDATADIR, &dir) < 0)
        die ("ioctl PPDATADIR 0 (out): %s\n", strerror (errno));
    printf ("data in forward mode\n");

    printf("Check DATA[2,4:7] => STATUS[3,4:7] (%d iterations)\n", ITERATIONS);

    errors = 0;
    for (iter = 0; iter < ITERATIONS; iter++) {
        for (i = 0; i < 256; i++) {
            out = i;
            if (ioctl (fd, PPWDATA, &out) < 0)
                die ("ioctl PPWDATA: %s\n", strerror (errno));
            if (ioctl (fd, PPRSTATUS, &in) < 0)
                die ("ioctl PPRSTATUS: %s\n", strerror (errno));
            if (((out >> 2) & 1) != ((in >> 3) & 1)) {
                printf ("Error: %d.%d: DATA2->STATUS3\n", iter, i);
                errors++;
            }
            if ((out & (1<<4)) != (in & (1<<4))) {
                printf ("Error: %d.%d: DATA4->STATUS4\n", iter, i);
                errors++;
            }
            if ((out & (1<<5)) != (in & (1<<5))) {
                printf ("Error: %d.%d: DATA5->STATUS5\n", iter, i);
                errors++;
            }
            if ((out & (1<<6)) != (in & (1<<6))) {
                printf ("Error: %d.%d: DATA6->STATUS6\n", iter, i);
                errors++;
            }
            if ((out & (1<<7)) == (in & (1<<7))) { // STATUS7 inverted
                printf ("Error: %d.%d: DATA7->STATUS7\n", iter, i);
                errors++;
            }
        }
    }
    printf ("%d errors\n", errors);

    /* CONTROL[1,3] => DATA[1,3]
     */
    dir = 1;
    if (ioctl (fd, PPDATADIR, &dir) < 0)
        die ("ioctl PPDATADIR 1 (in): %s\n", strerror (errno));
    printf ("data in reverse mode\n");

    printf("Check CONTROL[1,3] => DATA[1,3] (%d iterations)\n", ITERATIONS);

    errors = 0;
    for (iter = 0; iter < ITERATIONS; iter++) {
        for (i = 0; i < 16; i++) {
            out = i;
            if (ioctl (fd, PPWCONTROL, &out) < 0)
               die ("ioctl PPWCONTROL: %s\n", strerror (errno));
            if (ioctl (fd, PPRDATA, &in) < 0)
                die ("ioctl PPRDATA: %s\n", strerror (errno));
            if ((out & (1<<1)) == (in & (1<<1))) { // CONTROL1 inverted
                printf ("Error: %d.%d: CONTROL1->DATA1\n", iter, i);
                errors++;
            }
            if ((out & (1<<3)) == (in & (1<<3))) { // CONTROL3 inverted
                printf ("Error: %d.%d: CONTROL3->DATA3\n", iter, i);
                errors++;
            }
        }
    }
    printf ("%d errors\n", errors);

    if (ioctl (fd, PPRELEASE) < 0)
        die ("ioctl PPRELEASE: %s\n", strerror (errno));
    if (close(fd) < 0)
        die ("close %s: %s\n", DEVICE_PATH, strerror (errno));

   return 0;
}

/*
 * vi:tabstop=4 shiftwidth=4 expandtab
 */
