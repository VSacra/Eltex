CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -lm

all: server client

server: server.c
	$(CC) $(CFLAGS) -o server server.c $(LDFLAGS)

client: client.c
	$(CC) $(CFLAGS) -o client client.c

run-server:
	./server 1234

run-client:
	./client localhost 1234

clean:
	rm -f server client

.PHONY: all run-server run-client clean
