/*
        Program metro_server.c

*/
#include <termios.h>
#include <stdio.h>
#include <unistd.h> // read()
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#include "metro_server.h"

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
int new_fd;

void handler()
{
    if (led_status)
    {
        turn_on_led(TimeSig[status][location]);
        led_status = 0;
        switch(TimeSig[status][location])
        {
            case 7: 
                send(new_fd, "#", 2, 0);
                break;
            case 3: 
                send(new_fd, "+", 2, 0);
                break;
            case 1:
                send(new_fd, "!", 2, 0);
                break;
        }
    }
    else
    {
        turn_off_led(TimeSig[status][location]);
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
    timer_t t_id;
    long long time;
    char s[INET6_ADDRSTRLEN];
    
    int sockfd;         // for protocol
    struct sockaddr_storage their_addr;
    char msg[50];
    char cur_TimeSig[4];
    int recv_bytes;
    socklen_t sin_size;
    
    init_led();

    struct sigaction act;
    sigset_t set;
    
    sigemptyset(&set);
    sigaddset(&set, SIGALRM);

    act.sa_flags = 0;
    act.sa_mask = set;
    act.sa_handler = &handler;

    sigaction(SIGALRM, &act, NULL);

    init_server(&sockfd);

    while (1) {  // main accept() loop
        sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			continue;
		}

        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
		printf("server: got connection from %s\n", s);
        fflush(stdout);

        if (!fork()) {  // this is the child process
            close(sockfd);  // child doesn't need the listener
            run = 0;

            while(1) {
                memset(msg, 0, 50);
                recv_bytes = recv(new_fd, msg, 50, 0);
                if (recv_bytes <= 5) {
                    if (strcmp(msg, "Stop") == 0)
                    {
                        timer_delete(t_id);
                        turn_off_led(7);
                        run = 0;
                    }
                    else if (strcmp(msg, "Quit") == 0) {
                        run = 0;
                        break;
                    }
                }
                else {
                    sscanf(msg, "TimeSig %s , Tempo %d , Start", cur_TimeSig, &tempo);
                    
                    if (strcmp(cur_TimeSig, "4/4") == 0) 
                        status = 0;
                    else if (strcmp(cur_TimeSig, "6/8") == 0) 
                        status = 1;
                    else if (strcmp(cur_TimeSig, "2/4") == 0) 
                        status = 2;
                    else if (strcmp(cur_TimeSig, "3/4") == 0) 
                        status = 3;

                    time = 60 * NSEC_PER_SEC / (2 * tempo); 
                    // struct itimerspec tim_spec = {.it_interval= {.tv_sec=1,.tv_nsec=10000},
                    //                               .it_value = {.tv_sec=1,.tv_nsec=10000}};
                    struct itimerspec tim_spec = {.it_interval = {.tv_sec = time / NSEC_PER_SEC, .tv_nsec = (time % NSEC_PER_SEC) * 1000}, 
                                                  .it_value = {.tv_sec = time / NSEC_PER_SEC, .tv_nsec = (time % NSEC_PER_SEC) * 1000}};
                    
                    location = 0;
                    run = 1;
                    led_status = 1;
                    timer_create(CLOCK_MONOTONIC, NULL, &t_id);
                    timer_settime(t_id, 0, &tim_spec, NULL);
                    clock_gettime(CLOCK_MONOTONIC, &prev);
                    printf(" Start: Time-Sig: %s, Tempo: %d, Run: %d\n", TimeStr[status], tempo, run);
                    fflush(stdout);
                }
                fflush(stdout);
            }
            close(new_fd);
            fflush(stdout);
            exit(0);
        }
		else {      // fork
            wait(NULL);
            close(new_fd);  // parent doesn't need this
        }
    }

    if (run == 1) 
    {
        timer_delete(t_id);
        run = 0;
    }

    close(sockfd);
    exit_led();
    return 0;
}
