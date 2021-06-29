/*
        Program test_single_key.c
        
        Get a key input without hitting Enter key
        using termios

        Modified from
        http://stackoverflow.com/questions/7469139/what-is-equivalent-to-getch-getche-in-linux

        Global old_termios and new_termios for efficient key inputs.
*/
#include <termios.h>
#include <stdio.h>
#include <unistd.h> // read()
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>

#include "userLEDmmap.h"


char *TimeStr[4] = {"4/4", "6/8", "2/4", "3/4"};

int main(void)
{
    char c;
    int init_result;
    run = 0; // 0: stop, 1: run
    keepgoing = 1;
    
    init_result = init_led();
    if (init_result == -1) 
      return -1;
    init_key();

    status = 3;
    location = 0;
    tempo = 90;
    run = 0; // 0: stop, 1: run

    printf (" Default TimeSig: %s, Tempo: %d, Run: %d\n", TimeStr[status], tempo, run); 

    while (keepgoing){
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
                if (run == 1) run = 0;
                else run = 1;
        }
        if (c == 'q') 
        {
            break;
        }
        printf(" Input Key: %c, Time-Sig: %s, Tempo: %d, Run: %d\n", c, TimeStr[status], tempo, run);
        fflush(stdout);
    }
    printf(" Input Key: q, Quit!\n");
    fflush(stdout);

    exit_key();
    exit_led();
    return 0;
}
