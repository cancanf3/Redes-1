#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <getopt.h>
#include "db_handler.h"


char *protocol (char *);
void error(const char *);
void queryHandler(int);
int  exits;

/* Es el main del programa, recibe los argumentos del puerto y otros dejados para futuras versiones
 * El main inicializa la Base de datos y administra los sockets usados para la comunicacion
 * main (argumentos) -> int
 */

int main (int argc, char *argv[]) {

    int       sock_fd, newsock_fd, port_no;
    socklen_t client_len;
    struct    sockaddr_in server_addr, client_addr;
    int       option = 0;
            port_no  = 0;
            exits    = 0;
    

    initialize();

    if (argc == 1)
        error("Usage: server -f <filas> -c <col> [-p puerto] ");

    while ((option = getopt(argc, argv, "f:c:p:") ) != -1) {               
        switch (option) {
            case 'f' :
                break;
            case 'c' :
                break;
            case 'p' : port_no = atoi(optarg);
                break;
            case '?' : error("Usage: server -f <filas> -c <col> [-p puerto] ");
                break;

        }

    }
    if (optind < 5 || optind > 7)
        error("Usage: server -f <filas> -c <col> [-p puerto] ");

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

    if(bind(sock_fd, (struct sockaddr *) &server_addr, 
        sizeof(server_addr)) < 0 )
        error("Error: on binding.");

    listen(sock_fd,5);
    client_len = sizeof(client_addr);

    while(1) {
        newsock_fd = accept(sock_fd, (struct sockaddr *) &client_addr, 
                    &client_len );
        if ( newsock_fd < 0)
            error("Error: on accept.");

        queryHandler(newsock_fd);
        close(newsock_fd);

        if (exits == 1)
            break;

    }
    printf("Server process ends.\n");
    close(sock_fd);
    return 0;
}


/* Funcion que Maneja los errores del servidor
 * error (string) -> void
 */
void error (const char *msg) {
    perror(msg);
    exit(1);
}

/* Funcion que recibe los query y envia los reponds
 * queryHandler(socket) -> void
 */

void  queryHandler (int sock) {
    int  n;
    char buffer[256];
    char *buffer_respond;
    char *buffer_raw;
    int  respond_size;

    bzero(buffer,256);
    n = read(sock,buffer,255);
    if (n < 0) 
        error("Error: reading from socket");
   
    buffer_raw       = protocol(buffer);
    respond_size     = strtol(buffer_raw, &buffer_respond, 10);

    n = write(sock,buffer_respond,respond_size+1);
    if (n < 0) 
        error("Error: writing to socket");

    if (buffer_respond[1] == 'D')
        free(buffer_raw);

    if (buffer_respond[1] == 'I')
        exits = 1;
}

/* Funcion que trabaja los querys del protocolo y genera responds para el cliente
 * protocol(string) -> string
 */ 

char * protocol(char * msg) {
    // Query:
    //  GET CHAIR x y                                       ~ Peticion de reservacion

    // Respond:
    //  14 ACCEPTED     OFFER                               ~ Acepta la peticion
    //  ?? DECLINED     OFFER xx yy yy xx yy yy xx yy yy    ~ Rechazar reservacion (El largo del mensaje se calcula en tiempo de corrida)
    //  15 IMPOSIBLE    OFFER                               ~ No hay oferta disponible, vagon full
    //  17 NONEXISTENT  OFFER                               ~ La peticion tiene numero de asientos por fuera de los limites del vagon
    //  17 CORRUPTED    MESSAGE                             ~ La peticion no pertenece al protocolo
    Query   query;
    Query   respond;
    char    *accept         = "15 ACCEPTED OFFER";
    char    *imposible      = "15 IMPOSIBLE OFFER";
    char    *decline        = "DECLINED OFFER ";
    char    *nonexistent    = "17 NONEXISTENT OFFER";
    char    *corrupt        = "17 CORRUPTED MESSAGE";
    char    *aux;
    int     offer[2], i;

    if ( strncmp(msg,"GET CHAIR",9) ) {
        aux = corrupt;
        return aux;
    }

    query.msg       = 1;
    offer[0]        = strtol(msg+10, &aux, 10);
    offer[1]        = strtol(aux+1, NULL, 10);
    query.offer     = offer;
    respond         = db_handler(query);


    switch (respond.msg) {
        case 1  :
            aux = accept;
            break;
        case -1 :
            aux = imposible;
            break;
        case -2 :
            aux = nonexistent;
            break;
        case 0  :
            aux = (char *)malloc((sizeof(char)*2+1)*respond.size_offer
                +sizeof(decline)+sizeof(char)*4);
            sprintf(aux, "%d ", 132);
            strcat(aux, decline);
            for (i = 0 ; i < respond.size_offer; i++){
                if (respond.offer[i] < 10)
                    sprintf(aux+19+(i*3), "0%d ", respond.offer[i]);
                else
                    sprintf(aux+19+(i*3), "%d ", respond.offer[i]);
            }
            free(respond.offer);
            break;
    }

    return aux;

}