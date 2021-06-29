/*
        Program metronome_hrt.c

*/
#include <termios.h>
#include <stdio.h>
#include <unistd.h> // read()

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

#include "userLEDmmap.h"

#define NSEC_PER_SEC 1000000
#define timerdiff(a,b) ((float)((a)->tv_sec - (b)->tv_sec) + \
((float)((a)->tv_nsec - (b)->tv_nsec))/1000000000L)

// GLobal termios structs
int TimeSig[4][12] = {{7, 1, 3, 1, 7, 1, 3, 1, 7, 1, 3, 1},
                     {7, 1, 1, 3, 1, 1, 7, 1, 1, 3, 1, 1},
                     {7, 1, 7, 1, 7, 1, 7, 1, 7, 1, 7, 1},
                     {7, 1, 1, 7, 1, 1, 7, 1, 1, 7, 1, 1}};
char *TimeStr[4] = {"4/4", "6/8", "2/4", "3/4"};
struct timespec prev = {.tv_sec = 0, .tv_nsec = 0};
int status;
int location;
int tempo;
int run;                // 0: stop, 1: run
int led_status;         // 0: off, 1: on


void handler()
{
    if (led_status)
    {
        led_status = 0;
        printf("%d", TimeSig[status][location]);
        fflush(stdout);
    }
    else
    {
        led_status = 1;
        location += 1;
        if (location >= 12)
            location = 0;
        
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        // printf(" Diff time:%lf\n", timerdiff(&now, &prev));
        prev = now;
    }
}

int main(void)
{
    char c;
    int init_result;
    timer_t t_id;
    long long time;
    
    // init_result = init_led();
    // if (init_result == -1) 
    //   return -1;
    
    struct sigaction act;
    sigset_t set;
    
    sigemptyset(&set);
    sigaddset(&set, SIGALRM);

    act.sa_flags = 0;
    act.sa_mask = set;
    act.sa_handler = &handler;

    sigaction(SIGALRM, &act, NULL);
    
    init_key();

    status = 3;
    location = 0;
    tempo = 90;
    run = 0;            // 0: stop, 1: run
    led_status = 1;     // 0: off, 1: on
    keepgoing = 1;

    printf(" Default TimeSig: %s, Tempo: %d, Run: %d\n", TimeStr[status], tempo, run); 

    while (keepgoing){
        time = 60 * NSEC_PER_SEC / (2 * tempo); 
        // struct itimerspec tim_spec = {.it_interval= {.tv_sec=1,.tv_nsec=10000},
        //                               .it_value = {.tv_sec=1,.tv_nsec=10000}};
        struct itimerspec tim_spec = {.it_interval = {.tv_sec = time / NSEC_PER_SEC, .tv_nsec = (time % NSEC_PER_SEC) * 1000}, 
                                      .it_value = {.tv_sec = time / NSEC_PER_SEC, .tv_nsec = (time % NSEC_PER_SEC) * 1000}};
        c = getch();

        switch (c)
        {
            case 'z':
                status = (status + 1) % 4;
                break;
            case 'c':
                if (tempo == 30) tempo = 30;
                else tempo -= 5;
                break;
            case 'b':
                if (tempo == 200) tempo = 200;
                else tempo += 5;
                break;
            case 'm':
                if (run == 1) 
                {
                    timer_delete(t_id);
                    run = 0;
                }
                else 
                {
                    timer_create(CLOCK_MONOTONIC, NULL, &t_id);
                    run = 1;
                }
                break;
            default:
                if (c == 0) 
                {
                    continue; 
                }
                break;
        }
        if (c == 'q') 
        {
            break;
        }
        location = 0;
        led_status = 1;
        timer_settime(t_id, 0, &tim_spec, NULL);
        clock_gettime(CLOCK_MONOTONIC, &prev);
        printf(" Input Key: %c, Time-Sig: %s, Tempo: %d, Run: %d\n", c, TimeStr[status], tempo, run);
        fflush(stdout);
    }
    printf(" Input Key: q, Quit!\n");
    fflush(stdout);

    if (run == 1) 
    {
        timer_delete(t_id);
        run = 0;
    }

    exit_key();
    // exit_led();
    return 0;
}
