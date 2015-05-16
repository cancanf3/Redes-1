CC 		= gcc
CFLAGS 	= -Wall -g 

default:				server client

server:					multiconnect_server.c db_handler.o
						${CC} ${CFLAGS} -w  -o server multiconnect_server.c db_handler.o

db_handler.o: 			db_handler.c
						${CC} ${CFLAGS} -c db_handler.c 

client: 				client_Linux.c
						${CC} ${CFLAGS} client_Linux.c

clean:
	$(RM) server client *.o *~