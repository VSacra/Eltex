CC = gcc
CFLAGS = -Wall -Wextra -std=c99
PROGS = producer consumer

producer: producer.c
	$(CC) $(CFLAGS) -o producer producer.c

consumer: consumer.c
	$(CC) $(CFLAGS) -o consumer consumer.c

clean:
	rm -f $(PROGS)
	-ipcrm -a 2>/dev/null || true

.PHONY: clean
