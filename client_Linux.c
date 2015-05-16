#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 


void error(char *);

/* Es el main del programa, recibe los argumentos del puerto, las filas y las columnas
 * El main construye el query y administra el socket usado para la comunicacion
 * main (argumentos) -> int
 */
int main (int argc, char *argv[]) {

    int    sock_fd, port_no, n, i;
    struct sockaddr_in server_addr;
    struct hostent *server = NULL;
    char   buffer[256];
    int    rows,columns;
    int    option          = 0;
           port_no         = 0;


    if (argc == 1)
        error("Usage: client <ip-server> -p <port of service> -f <row> -c <col> ");

    while ( (option = getopt(argc, argv, "f:c:p:")) != -1) {
        switch (option) {
            case 'f' : rows = atoi(optarg);
                break;
            case 'c' : columns = atoi(optarg);
                break;
            case 'p' : port_no = atoi(optarg);
                break; 
            case '?' :
                error("Usage: client <ip-server> -p <port of service> -f <row> -c <col> ");
                break;
        }
    }
        
    if ( optind == 7)
        server = gethostbyname(argv[optind]);
    else
        error("Usage: client <ip-server> -p <port of service> -f <row> -c <col> ");



    if (server == NULL){
        printf("Error: no such host.\n");
        exit(1);
    }

    if (port_no == 0) {
        printf("Error: no port didn't specified.\n");
        exit(1);
    }

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) 
        error("Error: opening socket.");


    bzero((char *) &server_addr, sizeof(server_addr));
    bcopy((char *)server->h_addr, (char *)&server_addr.sin_addr.s_addr, server->h_length);
    server_addr.sin_family    = AF_INET;
    server_addr.sin_port      = htons(port_no);

    for (i=0;i<3;i++) {
        if (connect(sock_fd,(struct sockaddr *) &server_addr,sizeof(server_addr)) < 0) {
            //pass
            sleep(10);
            printf("Trying to Connect...\n");
            if (i == 2) 
                error("Error: connecting. | Time Out");
        }
        else
            break;
    }
    printf("\n\nProcessing request ==> Row %d Column %d \n\n", rows, columns);
    bzero(buffer,256);
    strcpy(buffer, "GET CHAIR ");
    sprintf(buffer+10, "%d %d ", rows, columns);
    n = write(sock_fd,buffer,strlen(buffer));
    if (n < 0) 
         error("Error: writing to socket.");

    bzero(buffer,256);
    n = read(sock_fd,buffer,255);
    if (n < 0) 
         error("Error: reading from socket.");

    printf("\n\n    Server Respond ==>");
    if (!strncmp(buffer," DECLINED OFFER", 13)) {
        printf(" DECLINED OFFER\n\n");
        printf("\n\n  Available Chairs <Row,Column> ==>%s\n\n\n", buffer+15);
    }
    else if (!strncmp(buffer," NONEXISTENT OFFER", 16))
        printf(" Request out of range\n\n\n");
    else
        printf("%s\n\n\n",buffer);

    close(sock_fd);
    return 0;

}

/* Funcion que Maneja los errores del servidor
 * error (string) -> void
 */
void error(char *msg) { 
    perror(msg);
    exit(1);
}