#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <getopt.h>

void error (const char *msg) {
    perror(msg);
    exit(1);
}

void sigHandler() {
    int childStatus;
    wait(&childStatus);
}

void dostuff (int sock) {
   int n;
   char buffer[256];
      
   bzero(buffer,256);
   n = read(sock,buffer,255);
   if (n < 0) error("ERROR reading from socket");
   printf("Here is the message: %s\n",buffer);
   n = write(sock,"I got your message",18);
   if (n < 0) error("ERROR writing to socket");
}


int main (int argc, char *argv[]) {

    signal(SIGCHLD, sigHandler);

    int sock_fd, newsock_fd, port_no, pid;
    socklen_t client_len;
    struct sockaddr_in server_addr, client_addr;

    port_no = 0;
    int option = 0;
    int rows,columns;

    if (argc == 1)
        error("Usage: server -f <filas> -c <col> [-p puerto] ");

    while ((option = getopt(argc, argv, "f:c:p:") ) != -1) {
        switch (option) {
            case 'f' : rows = atoi(optarg);
                break;
            case 'c' : columns = atoi(optarg);
                break;
            case 'p' : port_no = atoi(optarg);
                break;
            default: error("Usage: server -f <filas> -c <col> [-p puerto] ");

        }

    }

    if (port_no <= 0) {
        port_no = 7557;         // Port by default
    }

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_fd < 0)
        error("Error: Openning Socket.");

    bzero((char* ) &server_addr, sizeof(server_addr));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port        = htons(port_no);

    if(bind(sock_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0 )
        error("Error: on binding.");

    listen(sock_fd,5);
    client_len = sizeof(client_addr);

    while(1) {
        newsock_fd = accept(sock_fd, (struct sockaddr *) &client_addr, &client_len );
        if ( newsock_fd < 0)
            error("Error: on accept.");


        dostuff(newsock_fd);
        close(newsock_fd);
    }
    close(sock_fd);
    return 0;
}