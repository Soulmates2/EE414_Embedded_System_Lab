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

// GLobal termios structs
static struct termios old_tio;
static struct termios new_tio;

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
    if (read(0, &ch, 1) < 0) perror ("read()"); // Read one character
    return ch;
}

int main(void)
{
    char c;
    int echo;
    int status;
    int tempo;
    int running_mode;
    
    int TimeSig[4][6] = {{7, 1, 0, 0, 0, 0},
                        {7, 1, 1, 0, 0, 0},
                        {7, 1, 3, 1, 0, 0},
                        {7, 1, 1, 3, 1, 1}};

    char *TimeStr[4] = {"4/4", "6/8", "2/4", "3/4"};

    // Init termios: Disable buffered IO with arg 'echo'
    echo = 0; // Disable echo
    init_termios(echo);
    
    // Test loop
    printf (" algo_metronome_tui\n\n");
    printf (" Menu for metronome tui:\n");
    printf (" 'z': Time-signature       3/4 > 4/4 > 6/8 > 2/4 > 3/4 > ...\n");
    printf (" 'c': Dec tempo            Decrease tempo by 5\n");
    printf (" 'b': Inc tempo            Increase tempo by 5\n");
    printf (" 'm': Start/Stop           Toggle start and stop\n");
    printf (" 'q': Quit\n");
    
    status = 3;
    tempo = 90;
    running_mode = 0; // 0: stop, 1: run

    while (1) {
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
                if (running_mode == 1) running_mode = 0;
                else running_mode = 1;
        }
        if (c == 'q') break;
        printf(" Input Key: %c, Time-Sig: %s, Tempo: %d, Run: %d\n", c, TimeStr[status], tempo, running_mode);
        fflush(stdout);
    }
    printf(" Input Key: q, Quit!\n");
    fflush(stdout);

    // Reset termios
    reset_termios();
    return 0;
}
