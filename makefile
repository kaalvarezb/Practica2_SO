CC = gcc
CFLAGS = -Wall

.PHONY: all clean

all: p2-server p2-client

p2-server: p2-server.c
	$(CC) $(CFLAGS) p2-server.c -o p2-server

p2-client: p2-client.c
	$(CC) $(CFLAGS) p2-client.c -o p2-client

clean:
	rm -f p2-server p2-client