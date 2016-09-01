// code adapted from https://gist.github.com/jadonk/2587524
// changed event trigger to edge trigger because I am 
// not interested in the data just the edge
// // hints aboutr epoll taken from: 
// https://banu.com/blog/2/how-to-use-epoll-a-complete-example-in-c/

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

// Assumptions:
//  gpio pin 17 and 18 have been exported (gpio pin 17 example):
//    echo 17      > /sys/class/gpio/export
//    echo in      > /sys/class/gpio/gpio17/direction
//    echo rising  > /sys/class/gpio/gpio17/edge
//
// compiled via g++ 

void openAndAddToPoll(int epfd, int gpioPin, int &fd, epoll_event& eventToAdd)
{
   char str[256];

   sprintf(str, "/sys/class/gpio/gpio%d/value", gpioPin);

   if ((fd = open(str, O_RDONLY | O_NONBLOCK)) < 0)
   {
      fprintf(stderr, "Failed, gpio %d not exported.\n", gpioPin);
      exit(1);
   }

   eventToAdd.events = EPOLLET;
   eventToAdd.data.u32 = gpioPin;

   if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &eventToAdd) == -1)
   {
       perror("epoll_ctl failed: ");
       exit(-1);
   }
   printf("added %d(%s) to epoll\n", gpioPin, str);
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

int main(int argc, char** argv) 
{
    const int MAX_FDS = 2;
    const int gpioPins[MAX_FDS] = {17,18};

    // I assume we need to manage the life of epoll_events we add via epoll_ctl
    epoll_event eventToAdd[MAX_FDS];

    int fds[MAX_FDS];

    int epfd = epoll_create(MAX_FDS);

    for (int i=0;i<MAX_FDS;i++)
    {
        openAndAddToPoll(epfd, gpioPins[i], fds[i], eventToAdd[i]);
    }

    struct epoll_event events[MAX_FDS];
    struct timeval start,stop,delta;
    while(1) 
    {
        printf("epoll_wait on %d files for rising edge\n", MAX_FDS);

        gettimeofday(&start,NULL);
        int edges_detected = epoll_wait(epfd, events, MAX_FDS, -1);
        gettimeofday(&stop,NULL);

        timeval_subtract (&delta, &stop, &start);
        printf("epoll_wait elapsed time on waiting for edge %ld.%ld\n", delta.tv_sec,delta.tv_usec);

        if (edges_detected == -1) 
        {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }

        printf("epoll detected %d rising edges\n", edges_detected);

        for (int i = 0; i < edges_detected; i++)
        {
            if (events[i].events & EPOLLHUP)
            {
                // need to look at possible errors with files
                fprintf (stderr, "epoll hup error fd(%d)\n", events[i].data.fd);
                close (events[i].data.fd);
                continue;
            }
            // assuming gpio only listed in events due to rising edge
            printf("gpio %d high, event mask 0x(%x)\n",events[i].data.u32, events[i].events);
        }
    }

  return(0);
}
