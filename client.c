#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <getopt.h>
#include <string.h>

void error(char *msg) { 
    perror(msg);
    exit(1);
}

int main (int argc, char *argv[]) {
    int sock_fd, port_no, n;
    struct sockaddr_in server_addr;
    struct hostent *server;

    int option = 0;
    int rows,columns;

    char buffer[256];

    if (argc == 1)
        error("Usage: cliente <ip-servidor> -p <puerto servicio> -f <fila> -c <col> ");

    while (optind < argc) {
        if ((option = getopt(argc, argv, "f:c:p:")) != -1) {
            switch (option) {
                case 'f' : rows = atoi(optarg);
                    break;
                case 'c' : columns = atoi(optarg);
                    break;
                case 'p' : port_no = atoi(optarg);
                    break;
                default: error("Usage: cliente <ip-servidor> -p <puerto servicio> -f <fila> -c <col> ");
    
            }
        }
        else {
                server = gethostbyname(argv[1]);
                if (server == NULL)
                    error("Error: no such host.");
                optind++;
        }

    }

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) 
        error("Error: opening socket.");


    bzero((char *) &server_addr, sizeof(server_addr));
    bcopy((char *)server->h_addr, (char *)&server_addr.sin_addr.s_addr, server->h_length);
    server_addr.sin_family    = AF_INET;
    server_addr.sin_port      = htons(port_no);


    if (connect(sock_fd,(struct sockaddr *) &server_addr,sizeof(server_addr)) < 0) 
        error("Error: connecting.");

    printf("Please enter the message: ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);
    n = write(sock_fd,buffer,strlen(buffer));
    if (n < 0) 
         error("Error: writing to socket.");

    bzero(buffer,256);
    n = read(sock_fd,buffer,255);
    if (n < 0) 
         error("Error: reading from socket.");


    printf("%s\n",buffer);
    close(sock_fd);
    return 0;

}