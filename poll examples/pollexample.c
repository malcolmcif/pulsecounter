/* 2014-07-06
   wfi.c

   gcc -o wfi wfi.c

   ./wfi [gpio]
   https://www.raspberrypi.org/forums/viewtopic.php?f=33&t=84696
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <poll.h>

#define GPIO 17 

// taken from:
// http://www.gnu.org/software/libc/manual/html_node/Elapsed-Time.html
int
timeval_subtract (struct timeval *result, struct timeval *x, struct timeval *y)
{
      /* Perform the carry for the later subtraction by updating y. */
      if (x->tv_usec < y->tv_usec) {
              int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
                  y->tv_usec -= 1000000 * nsec;
                      y->tv_sec += nsec;
                        }
        if (x->tv_usec - y->tv_usec > 1000000) {
                int nsec = (x->tv_usec - y->tv_usec) / 1000000;
                    y->tv_usec += 1000000 * nsec;
                        y->tv_sec -= nsec;
                          }

          /* Compute the time remaining to wait.
           *      tv_usec is certainly positive. */
          result->tv_sec = x->tv_sec - y->tv_sec;
            result->tv_usec = x->tv_usec - y->tv_usec;

              /* Return 1 if result is negative. */
              return x->tv_sec < y->tv_sec;
}

int main(int argc, char *argv[])
{
   char str[256];
   struct timeval start,stop,delta;
   struct pollfd pfd;
   int fd, gpio;

   /*
      Prior calls assumed.
      sudo sh -c "echo 4      >/sys/class/gpio/export"
      sudo sh -c "echo in     >/sys/class/gpio/gpio4/direction"
      sudo sh -c "echo rising >/sys/class/gpio/gpio4/edge"
   */

   if (argc > 1) gpio = atoi(argv[1]);
   else          gpio = GPIO;

   sprintf(str, "/sys/class/gpio/gpio%d/value", gpio);

   if ((fd = open(str, O_RDONLY)) < 0)
   {
      fprintf(stderr, "Failed, gpio %d not exported.\n", gpio);
      exit(1);
   }

   pfd.fd = fd;

   pfd.events = POLLPRI;

   //clear any old data
   int count;
   char c;
   ioctl (pfd.fd, FIONREAD, &count);
   for (int i = 0 ; i < count ; ++i)
       read (pfd.fd, &c, 1);

   while (true)
   {
       gettimeofday(&start,NULL);
       count = poll(&pfd, 1, -1);         /* wait for interrupt */
       gettimeofday(&stop,NULL);

       timeval_subtract (&delta, &stop, &start);
       printf("poll_wait elapsed(%d) time on waiting for event %ld.%ld\n", 
                                            count, delta.tv_sec,delta.tv_usec);

       if (count == 1)
       {
           read(fd, &c, 1);
           lseek(fd, 0, SEEK_SET);    /* consume interrupt */
           printf("data = %c\n", c);
       }
   }

   exit(0);
}
