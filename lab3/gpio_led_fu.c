// From: http://stackoverflow.com/questions/13124271/driving-beaglebone-gpio
// -through-dev-mem
// user contributions licensed under cc by-sa 3.0 with attribution required
// http://creativecommons.org/licenses/by-sa/3.0/
// http://blog.stackoverflow.com/2009/06/attribution-required/
// Author: madscientist159 (http://stackoverflow.com/users/3000377/madscientist159)
//
// Read one gpio pin and write it out to another using mmap.
// Be sure to set -O3 when compiling.
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <signal.h>    // Defines signal-handling functions (i.e. trap Ctrl-C)
#include <unistd.h>		// close()
#include "userLEDmmap.h"

// Global variables
int TimeSig[4][12] = {{7, 1, 3, 1, 7, 1, 3, 1, 7, 1, 3, 1},
                     {7, 1, 1, 3, 1, 1, 7, 1, 1, 3, 1, 1},
                     {7, 1, 7, 1, 7, 1, 7, 1, 7, 1, 7, 1},
                     {7, 1, 1, 7, 1, 1, 7, 1, 1, 7, 1, 1}};
pthread_t pthread;
int fd;

volatile void *gpio_addr;
volatile unsigned int *gpio_datain;
volatile unsigned int *gpio_setdataout_addr;
volatile unsigned int *gpio_cleardataout_addr;

void *turn_led(void *);

int init_led(void)
{
    int thread_id;
    fd = open("/dev/mem", O_RDWR);
    printf("Mapping %X - %X (size: %X)\n", GPIO1_START_ADDR, GPIO1_END_ADDR, GPIO1_SIZE);
    
    gpio_addr = mmap(0, GPIO1_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO1_START_ADDR);

    gpio_datain = gpio_addr + GPIO_DATAIN;
    gpio_setdataout_addr = gpio_addr + GPIO_SETDATAOUT;
    gpio_cleardataout_addr = gpio_addr + GPIO_CLEARDATAOUT;

    if(gpio_addr == MAP_FAILED) {
        printf("Unable to map GPIO\n");
        exit(1);
    }
    printf("GPIO mapped to %p\n", gpio_addr);
    printf("GPIO SETDATAOUTADDR mapped to %p\n", gpio_setdataout_addr);
    printf("GPIO CLEARDATAOUT mapped to %p\n", gpio_cleardataout_addr);

    thread_id = pthread_create(&pthread, NULL, turn_led, NULL);
    if (thread_id < 0)
    {
        printf (" Fail to create thread\n");
        munmap((void *)gpio_addr, GPIO1_SIZE);
        close(fd);
        return -1;
    }
    else
    {
        printf (" Create thread done!\n");
        return 0;
    }
}

void *turn_led(void *data) 
{
    int time = 60000000 / (tempo * 2);
    keepgoing = 1;

    printf (" ================================================\n");
    while(keepgoing)
    {
        time = 60000000 / (tempo * 2);
        if (run == 1)
        {
            *gpio_setdataout_addr = TimeSig[status][location] << 21;
            printf ("%d", TimeSig[status][location]);
            fflush (stdout);
            usleep (time);
            *gpio_cleardataout_addr = TimeSig[status][location] << 21;
            usleep (time);
            location += 1;
            if (location >= 12)
                location = 0;
        }
        else {
            usleep(100000);
            location = 0;
        }
    }
    pthread_exit (0);
}

void exit_led(void)
{
    keepgoing = 0;
    pthread_join(pthread, NULL);
    munmap((void *)gpio_addr, GPIO1_SIZE);
    close(fd);
}
