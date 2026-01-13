CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pthread

all: client1 client2

client1: client1.c
	$(CC) $(CFLAGS) -o client1 client1.c

client2: client2.c
	$(CC) $(CFLAGS) -o client2 client2.c

run1: client1
	./client1 127.0.0.1

run2: client2
	./client2 127.0.0.1

clean:
	rm -f client1 client2

.PHONY: all run1 run2 clean
