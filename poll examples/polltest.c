// code adapted from https://gist.github.com/jadonk/2587524
// changed event trigger to edge trigger because I am 
// not interested in the data just the edge
// // hints aboutr poll taken from: 
// https://banu.com/blog/2/how-to-use-poll-a-complete-example-in-c/

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>


// Assumptions:
//  gpio pin 17 and 18 have been exported (gpio pin 17 example):
//    echo 17      > /sys/class/gpio/export
//    echo in      > /sys/class/gpio/gpio17/direction
//    echo rising  > /sys/class/gpio/gpio17/edge
//
// compiled via g++ 

void openAndAddToPoll(int gpioPin, pollfd &polls)
{
   char str[256];

   sprintf(str, "/sys/class/gpio/gpio%d/value", gpioPin);

   if ((polls.fd = open(str, O_RDWR)) < 0)
   {
      fprintf(stderr, "Failed, gpio %d not exported.\n", gpioPin);
      exit(1);
   }

   polls.events = POLLPRI;

   //clear any old data
   int count;
   char c;
   ioctl (polls.fd, FIONREAD, &count);
   for (int i = 0 ; i < count ; ++i)
       read (polls.fd, &c, 1);

   printf("added %d(%s) to poll on fd(%d)\n", gpioPin, str, polls.fd);
}

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

void displayEvent(int fd, int revent)
{
    printf("fd(%d) events(%x) = ",fd, revent);

    if (revent == 0)
        printf("EMPTY ");

    if (revent & POLLIN)
        printf("POLLIN ");

    if (revent & POLLRDNORM)
        printf("POLLRDNORM ");

    if (revent & POLLRDBAND)
        printf("POLLRDBAND ");

    if (revent & POLLPRI)
        printf("POLLPRI ");

    if (revent & POLLOUT)
        printf("POLLOUT ");

    if (revent & POLLWRNORM)
        printf("POLLWRNORM ");

    if (revent & POLLWRBAND)
        printf("POLLWRBAND ");

    if (revent & POLLERR)
        printf("POLLERR ");

    if (revent & POLLHUP)
        printf("POLLHUP ");

    if (revent & POLLNVAL)
        printf("POLLNVAL ");

    printf("\n");
}

int main(int argc, char** argv) 
{
    const int MAX_FDS = 2;
    const int gpioPins[MAX_FDS] = {17,18};
    struct pollfd polls[MAX_FDS] ;

    for (int i=0;i<MAX_FDS;i++)
    {
        openAndAddToPoll(gpioPins[i], polls[i]);
    }

    struct timeval start,stop,delta;
    int ch;

    while(1) 
    {
        printf("poll_wait on %d files for rising edge\n", MAX_FDS);

        gettimeofday(&start,NULL);
        int triggers_detected = poll(polls, MAX_FDS, -1);
        gettimeofday(&stop,NULL);

        timeval_subtract (&delta, &stop, &start);
        printf("poll_wait elapsed time on waiting for edge %ld.%ld\n", delta.tv_sec,delta.tv_usec);

        if (triggers_detected == -1) 
        {
            perror("poll_wait");
            exit(EXIT_FAILURE);
        }

        printf("poll detected on %d files\n", triggers_detected);

        for (int i = 0; i < triggers_detected; i++)
        {
            displayEvent(polls[i].fd, polls[i].revents);
            if(polls[i].revents & POLLPRI)
            {
                (void)read (polls[i].fd, &ch, 1) ;
                lseek (polls[i].fd, 0, SEEK_SET) ;
                printf("gpio %d change, value = %c\n",polls[i].fd, ch);
            }
        }
    }

  return(0);
}
