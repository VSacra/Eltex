CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGETS = main02 Sub maxlen

all: $(TARGETS)

main02: main02.c
	$(CC) $(CFLAGS) -o main02 main02.c

Sub: Sub.c
	$(CC) $(CFLAGS) -o Sub Sub.c

maxlen: maxlen.c
	$(CC) $(CFLAGS) -o maxlen maxlen.c

run: all
	./main02

clean:
	rm -f $(TARGETS)

.PHONY: all run clean
