

CC=gcc
CFLAGS=-Wall -Wextra

all: net_server net_client

net_server: net_server.c
	$(CC) $(CFLAGS) -o net_server net_server.c

net_client: net_client.c
	$(CC) $(CFLAGS) -o net_client net_client.c

clean:
	rm -f net_server net_client
