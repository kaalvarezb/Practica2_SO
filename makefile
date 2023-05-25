CC = gcc
CFLAGS = -Wall

.PHONY: all clean

all: P2-Server P2-Client

p2-server: P2-Server.c
	$(CC) $(CFLAGS) P2-Server.c -o P2-Server

p2-client: p2-Client.c
	$(CC) $(CFLAGS) P2-Client.c -o P2-Client

clean:
	rm -f P2-Server P2-Client