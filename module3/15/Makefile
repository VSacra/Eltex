CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -lm

all: server client

server: server.c
	$(CC) $(CFLAGS) -o server server.c $(LDFLAGS)

client: client.c
	$(CC) $(CFLAGS) -o client client.c

run_server:
	./server

run_client:
	./client localhost 1234

clean:
	rm -f server client

.PHONY: all run_server run_client clean
