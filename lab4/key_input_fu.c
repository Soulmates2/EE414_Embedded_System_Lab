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
#include <signal.h>
#include <sys/select.h>
#include <errno.h>

#include "userLEDmmap.h"

// GLobal termios structs
static struct termios old_tio;
static struct termios new_tio;


// Callback called when SIGINT is sent to the process (Ctrl-C)
void signal_handler(int sig) {
    printf( "\nCtrl-C pressed, cleaning up and exiting...\n" );
    keepgoing = 0;
}


// Initialize new terminal i/o settings
void init_termios(int echo)
{
    tcgetattr(0, &old_tio); // Grab old_tio terminal i/o setting
    new_tio = old_tio; // Copy old_tio to new_tio
    new_tio.c_lflag &= ~ICANON; // disable buffered i/o
    new_tio.c_lflag &= echo? ECHO : ~ECHO; // Set echo mode
    if (tcsetattr(0, TCSANOW, &new_tio) < 0) perror("tcsetattr ~ICANON");   // Set new_tio terminal i/o setting
}

// Restore old terminal i/o settings
void reset_termios(void)
{
    tcsetattr(0, TCSANOW, &old_tio);
}

// Read one character without Enter key: Blocking
char getch(void)
{
    char ch = 0;
    if (read(0, &ch, 1) < 0) 
    {
        if (errno == EINTR)
        {
            return 0;
        }
        perror ("read()"); // Read one character
    }
    return ch;
}

int key_hit()
{
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv);
}

void init_key(void)
{
    signal(SIGINT, signal_handler);

    // Init termios: Disable buffered IO with arg 'echo'
    int echo = 0; // Disable echo
    init_termios(echo);
    
    // print usage
    printf (" #### metronome_hrt #### \n\n");
    printf (" Menu for Metronome_hrt:\n");
    printf (" 'z': Time signature       2/4 > 3/4 > 4/4 > 6/8 > 2/4 > ...\n");
    printf (" 'c': Dec tempo            Decrease tempo by 5 (min tempo 30)\n");
    printf (" 'b': Inc tempo            Increase tempo by 5 (max tempo 200)\n");
    printf (" 'm': Start/Stop           Toggles start and stop\n");
    printf (" 'q': Quit\n");
}

void exit_key(void)
{
    // Reset termios
    reset_termios();
}
