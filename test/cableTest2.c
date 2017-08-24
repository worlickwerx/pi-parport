/* cableTest2.c - borrowed from here:
   http://www.labbookpages.co.uk/electronics/parallelPort.html
*/

#include <stdio.h>
#include <sys/ioctl.h>
#include <linux/parport.h>
#include <linux/ppdev.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

int running = 1;

void signalHandler(int sig)
{
   running = 0;
}

int main(void)
{
   int fd;
   int result;

   printf("Parallel Port Interface Test\n");

   // Set ctrl-c handler
   signal(SIGINT, signalHandler);

   // Open the parallel port for reading and writing
   fd = open("/dev/parport0", O_RDWR);

   if (fd == -1)
   {
      perror("Could not open parallel port");
      return 1;
   }

   // Try to claim port
   if (ioctl(fd, PPCLAIM, NULL))
   {
      perror("Could not claim parallel port");
      close(fd);
      return 1;
   }

   // Set the Mode
   int mode = IEEE1284_MODE_BYTE;
   if (ioctl(fd, PPSETMODE, &mode))
   {
      perror("Could not set mode");
      ioctl(fd, PPRELEASE);
      close(fd);
      return 1;
   }

   // Set data pins to output
   int dir = 0x00;
   if (ioctl(fd, PPDATADIR, &dir))
   {
      perror("Could not set parallel port direction");
      ioctl(fd, PPRELEASE);
      close(fd);
      return 1;
   }
   char dataH = 0xFF;
   char dataL = 0x00;

   // This loop will keep running until ctrl-c is pressed
   while(running)
   {
      // Output some data
      // Note that data is passed by a pointer
      printf("PPWDATA %x\n", dataH);
      ioctl(fd, PPWDATA, &dataH);
      printf("PPWDATA %x\n", dataL);
      ioctl(fd, PPWDATA, &dataL);
   }

   printf("Shutting down\n");

   // Release and close the parallel port
   ioctl(fd, PPRELEASE);
   close(fd);

   return 0;
}
