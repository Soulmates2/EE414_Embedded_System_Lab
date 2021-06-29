#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

#define NSEC_PER_SEC 1000000000L

#define MAX_TESTS    10000

#define timerdiff(a,b) (((a)->tv_sec - (b)->tv_sec) * NSEC_PER_SEC + \
(((a)->tv_nsec - (b)->tv_nsec)))

static struct timespec prev = {.tv_sec=0,.tv_nsec=0};
static int count = MAX_TESTS;
unsigned long diff_times[MAX_TESTS];

void handler(int signo)
{
    struct timespec now;
    register int i, correct=0;

    if(count >= 0)
    {
        clock_gettime(CLOCK_MONOTONIC, &now);
        diff_times[count]=timerdiff(&now, &prev);
        prev = now;
        count --;
    }

    else
    {
        for(i=0; i<MAX_TESTS; ++i)
        {
            if(diff_times[i]/1000 < 510 && diff_times[i]/1000 > 490)
            {
                printf("%d->\t", i);
                correct++;
            }
            printf("%lu\n", diff_times[i]);
        }
        printf("total corrects -> %d / %d\n", correct, MAX_TESTS);
        exit(0);
    }
}

int main(int argc, char *argv[])
{
    int i = 0;
    timer_t t_id;

    struct itimerspec tim_spec = {.it_interval= {.tv_sec=0,.tv_nsec=500000},
                    .it_value = {.tv_sec=1,.tv_nsec=0}};

    struct sigaction act;
    sigset_t set;

    sigemptyset(&set);
    sigaddset(&set, SIGALRM);

    act.sa_flags = 0;
    act.sa_mask = set;
    act.sa_handler = &handler;

    sigaction(SIGALRM, &act, NULL);

    if (timer_create(CLOCK_MONOTONIC, NULL, &t_id))
        perror("timer_create");

    clock_gettime(CLOCK_MONOTONIC, &prev);

    if (timer_settime(t_id, 0, &tim_spec, NULL))
        perror("timer_settime");

    while(1);

    return 0;
}