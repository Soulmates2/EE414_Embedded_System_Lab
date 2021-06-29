/*
        Program metro_client.c

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
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#include "metro_client.h"

#define NSEC_PER_SEC 1000000
#define timerdiff(a,b) ((float)((a)->tv_sec - (b)->tv_sec) + \
((float)((a)->tv_nsec - (b)->tv_nsec))/1000000000L)

// Global 
struct timespec prev = {.tv_sec = 0, .tv_nsec = 0};

int TimeSig[4][12] = {{7, 1, 3, 1, 7, 1, 3, 1, 7, 1, 3, 1},
                     {7, 1, 1, 3, 1, 1, 7, 1, 1, 3, 1, 1},
                     {7, 1, 7, 1, 7, 1, 7, 1, 7, 1, 7, 1},
                     {7, 1, 1, 7, 1, 1, 7, 1, 1, 7, 1, 1}};
char *TimeStr[4] = {"4/4", "6/8", "2/4", "3/4"};
int status;
int location;
int tempo;
int run;                // 0: stop, 1: run
int led_status;         // 0: off, 1: on
int sockfd;             // for protocol

void recv_handler()
{
    char recv_led[2];   // one character + null
    while (1)
    {
        recv(sockfd, recv_led, 2, 0);
        printf("%s", recv_led);
        fflush(stdout);
    }
}

int main(int argc, char *argv[])
{
    char c;
    timer_t t_id;
    long long time;

    pthread_t pthread;
    int thread_id;
    char msg[50];

    init_client(argc, argv, &sockfd);

    thread_id = pthread_create(&pthread, NULL, recv_handler, NULL);
    if (thread_id < 0)
    {
        printf (" Fail to create thread\n");
        close(sockfd);
        return -1;
    }
    else
    {
        printf (" Create thread done!\n");
        pthread_detach(&pthread);
    }

    init_key();
    
    status = 3;
    location = 0;
    tempo = 90;
    run = 0;            // 0: stop, 1: run
    led_status = 1;     // 0: off, 1: on
    keepgoing = 1;

    printf(" Default TimeSig: %s, Tempo: %d, Run: %d\n", TimeStr[status], tempo, run); 
    printf (" ================================================\n");

    while (keepgoing){
        time = 60 * NSEC_PER_SEC / (2 * tempo); 
        // struct itimerspec tim_spec = {.it_interval= {.tv_sec=1,.tv_nsec=10000},
        //                               .it_value = {.tv_sec=1,.tv_nsec=10000}};
        struct itimerspec tim_spec = {.it_interval = {.tv_sec = time / NSEC_PER_SEC, .tv_nsec = (time % NSEC_PER_SEC) * 1000}, 
                                      .it_value = {.tv_sec = time / NSEC_PER_SEC, .tv_nsec = (time % NSEC_PER_SEC) * 1000}};
        c = getch();

        switch (c)
        {
            case 'q':
                send (sockfd, "Quit", 5, 0);
                break;
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
                    send(sockfd, "Stop", 5, 0);
                    run = 0;
                }
                else 
                {
                    sprintf(msg, "TimeSig %s , Tempo %d , Start", TimeStr[status], tempo);
                    send(sockfd, msg, 50, 0);
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
        
        printf(" Input Key: %c, Time-Sig: %s, Tempo: %d, Run: %d\n", c, TimeStr[status], tempo, run);
        fflush(stdout);
    }
    printf(" Input Key: q, Quit!\n");
    fflush(stdout);

    close(sockfd);
    exit_key();

    return 0;
}
