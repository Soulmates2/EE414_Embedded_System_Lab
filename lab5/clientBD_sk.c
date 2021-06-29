/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <termios.h>

#include <arpa/inet.h>

#define PORT "3490" // the port client will be connecting to 

#define MAXDATASIZE 100 // max number of bytes we can get at once 

int sockfd;


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

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
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

void *child_thread (void *data)
{
   char but[30];
   for (int i = 0; i < 10; i++)
   {
       recv(sockfd, buf, 30, 0);
       printf("%s\n", buf);
       usleep(500000);
   }
}

int main(int argc, char *argv[])
{
    int numbytes;  
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];
    int thread_id;
    pthread_t pthread;
    char c;

    int echo = 0;
    init_termios(echo);

    if (argc != 2) {
        fprintf(stderr,"usage: client hostname\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }
        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
    printf("client: connecting to %s\n", s);
    freeaddrinfo(servinfo); // all done with this structure
    
    printf("Please input single key for sending\n");
    char msg[2];
    c = getch();
    sprintf(msg, "%c\0", c);
    printf("input key: %s\n", msg);

    thread_id = pthread_create(&pthread, NULL, child_thread, NULL);
    for (int i=0; i < 5; i++)
    {
        send(sockfd, msg, 2, 0);
        sleep(1);
    }
    pthread_join(pthread, NULL);
    reset_termios();
    close(sockfd);
    return 0;
}