========================================================================
    PulseCounter Overview
========================================================================

PulseCounter is used to read digital pins and count the low to high transition of each pin it is configured to read.
The intention of the software was to allow a raspberry pi to be connected to the fuel injector lines and coil lines of a v6 and show that all lines have the same signal on them.

The software can be used in a headless mode where it stores the information in a text file once every second, 
this mode allows you to run the software at startup and ssh into the raspberry pi and cat the text file to view the results.

A pi 3 image can be found at kmcpatterns.com.au/ configured for the following:
1. run the software on startup
2. act as a wireless access point so you can ssh (root/root) into it and view the results of the PulseCounter program (/tmp/status.txt)
3. read-only file system so the pi can be turned on and off without corrupting the storage media

Initial Requirements:
1. Count low/high transitions from 12 gpio pins at 65Hz reliably with a standard pi kernel - this has been verified with stress testing and calculations (test environment limited ability to test this beyond a doubt)
2. Structure the software that the algorithms in it can be tested/debugged on the windows platform
3. The application must be cross compiled from windows to the raspberry pi platform using the boost build system and a raspberry pi cross compiler

Extended Requirements:
1. Allow the user to specify the physical pins to be counted
2. Allow the rest period in the polling loop to be adjusted to allow for quicker sample rates 
2.1 The rest period is used to allow the polling thread time to rest from polling, this helps reduce the cpu load which aids in reliability of the application. 
    Resting the cpu comes at the cost of needing to know how long you can afford to rest in order to not miss your signal. I did investigate the use of epoll/poll but found it to not work reliably.

Design:
1. The code uses two threads, 1 for reading the gpio register status and 1 for displaying the count results. 
1.1 Polling the gpio pins was the most reliable approach I found for processing many pins.
2. The first thread spends most of the time (configurable) sleeping so as to reduce cpu load and the second thread operates once every second
3. The code is structured such that it should be possible to optimise it should the need arise, but from tesing the O/S latency is the limiting factor.
4. The code is designed to reduce load on the processor so that the application does not overload the operating system and miss detections 

Technologies Used:
1. C++
2. Cross-platform algorithms to allow visual studio testing/debugging
3. Boost C++ libraries
4. Boost Build environment
5. modified version of http://wiringpi.com/ library
5.1 added a function to read all gpio pins as 1 registry read as opposed to reading each pin seperately
6. An interface board design is available for the Pi that performs the following:
6.1 voltage shift 12v to 3.3v
6.2 filter back EMF interference from the inputs.
6.3 further details available: http://kmcpatterns.com.au/pulsecounterinterface

Performance:
1. the fastest the software has proven to work for 12 channels is 1500Hz with no sleep (i.e. 100% cpu utilisation).
2. the fastest the software has proven to work for 2 channels is 17000Hz with no sleep (i.e. 100% cpu utilisation).
3. the worst case latency of the standard kernel is ~300us, this will impact the softwares ability to detect pulse changes that occur less than 
   300us (1E+6us/300us)=3300Hz over a long period of time.  A real time kernel should help.
------------------------------
Note: A single channel signal generator was used to provide a square wave as input at some defined frequency. 
      This signal was injected on all channels, as a consequence 1500Hz by 12 channels may be an over estimate
      of possible performance as the signal is not spaced evenly accross all configured channels.
------------------------------

Examples:
Due to starting and stopping of sampling you can expect at most a 2 sample deviation between channels. If you specify a cpurest period 100us or less you will consume 100% cpu (sleep <= 100 are busy waits)
so -1 (no sleep) might be the best option.

Kernel: Linux raspberrypi 4.1.19-v7+ #858 SMP Tue Mar 15 15:56:00 GMT 2016 armv7l GNU/Linux

** 300us(default) delay (7% cpu load): 120Hz x 12 channels - 3.5 hours **
root@raspberrypi:~# /root/PulseCounter --activepins default --cpuRestPeriod 5 --duration 12774
-- VERSION: 0.3
-- rest cpu between samples for (us) : 300
Configured with multi thread display
pullUpDownState = pin internal resistor configured to tristate(off or float)
sample forever
Signal Counts for Each Pin
    11     12     13     15        16     18     22       36     38       19     21     23 
1532944 1532944 1532944 1532944 1532944 1532944 1532945 1532945 1532945 1532945 1532945 1532945 
Statics -
   duration - 12774
   fs       - 120.00
   mean     - 1532944
   min      - 1532944
   max      - 1532945
   delta    - 1
 
** no delay (100% cpu load): 500Hz x 12 channels - 5 mins **
root@raspberrypi:~# /root/PulseCounter --activepins default --cpuRestPeriod -1 --duration 300
-- VERSION: 0.3
-- rest cpu between samples for (us) : -1
Configured with multi thread display
pullUpDownState = pin internal resistor configured to tristate(off or float)
sample for 300 seconds

Signal Counts for Each Pin
    11     12     13     15     16     18     22     36     38     19     21     23 
150086 150085 150086 150086 150086 150086 150086 150086 150086 150086 150086 150086 
Statics -
   duration - 300.17
   fs       - 500.00
   mean     - 150085
   min      - 150085
   max      - 150086
   delta    - 1

** no delay (100% cpu load): 600Hz x 12 channels - 5 mins **
root@raspberrypi:~# /root/PulseCounter --activepins default --cpuRestPeriod -1 --duration 300
-- VERSION: 0.3
-- rest cpu between samples for (us) : -1
Configured with multi thread display
pullUpDownState = pin internal resistor configured to tristate(off or float)
sample for 300 seconds
Signal Counts for Each Pin
    11     12     13     15     16     18     22     36     38     19     21     23 
180108 180107 180108 180108 180108 180108 180108 180108 180108 180108 180108 180108 
Statics -
   duration - 300.18
   fs       - 600.00
   mean     - 180107
   min      - 180107
   max      - 180108
   delta    - 1

** no delay (100% cpu load): 800Hz x 12 channels - 4 hours **
root@raspberrypi:~# /root/PulseCounter --activepins default --cpuRestPeriod -1
-- VERSION: 0.3
-- rest cpu between samples for (us) : -1
Configured with multi thread display
pullUpDownState = pin internal resistor configured to tristate(off or float)
sample forever
Signal Counts for Each Pin
    11     12     13     15     16     18     22     36     38     19     21     23 
11607786 11607786 11607786 11607786 11607786 11607786 11607787 11607787 11607786 11607787 11607787 11607787 
Statics -
   duration - 14509.75
   fs       - 800.00
   mean     - 11607786
   min      - 11607786
   max      - 11607787
   delta    - 1

** no delay (100% cpu load): 1500Hz x 12 channels - 4 hours **
root@raspberrypi:~# /root/PulseCounter --activepins default --cpuRestPeriod -1
-- VERSION: 0.3
-- rest cpu between samples for (us) : -1
Configured with multi thread display
pullUpDownState = pin internal resistor configured to tristate(off or float)
sample forever
Signal Counts for Each Pin
    11     12     13     15     16     18     22     36     38     19     21     23 
224730051 224730051 224730051 224730051 224730051 224730051 224730051 224730051 224730051 224730051 224730051 224730051 
Statics -
   duration - 14982.05
   fs       - 14999.96
   mean     - 224730051
   min      - 224730051
   max      - 224730051
   delta    - 0

** no delay (100% cpu load): 17000Hz x 2 channels - 3 hours **
root@raspberrypi:~# /root/PulseCounter --activepins 11,38 --cpuRestPeriod -1
-- VERSION: 0.3
-- rest cpu between samples for (us) : -1
Configured with multi thread display
pullUpDownState = pin internal resistor configured to tristate(off or float)
sample forever
Signal Counts for Each Pin
    11     38 
195191889 195191889 
Statics -
   duration - 11481.91
   fs       - 16999.96
   mean     - 195191889
   min      - 195191889
   max      - 195191889
   delta    - 0   
