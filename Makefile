CC 		= gcc
CFLAGS 	= -Wall -g 

default:				server

server:					db_handler.o multiconnect_server.c
						$(CC) $(CFLAGS) -w  -o server db_handler.o multiconnect_server.c

db_handler.o: 			db_handler.c
						$(CC) $(CFLAGS) -c db_handler.c 


clean:
	$(RM) server *.o *~