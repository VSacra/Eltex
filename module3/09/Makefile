CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pthread -lrt
TARGET = main09
SRC = main09.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
	-sem_unlink /producer_consumer_sem 2>/dev/null || true
	-shm_unlink /producer_consumer_shm 2>/dev/null || true

.PHONY: all clean
