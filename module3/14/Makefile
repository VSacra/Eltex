CC = gcc
CFLAGS = -Wall

all: raw client1 client2

raw: raw.c
	$(CC) $(CFLAGS) -o raw raw.c

client1: client1.c
	$(CC) $(CFLAGS) -o client1 client1.c

client2: client2.c
	$(CC) $(CFLAGS) -o client2 client2.c

run:
	sudo ./raw

clean:
	rm -f raw client1 client2 dump.bin

.PHONY: all run clean
