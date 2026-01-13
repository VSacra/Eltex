CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -D_POSIX_C_SOURCE=199309L -lrt
TARGET = main11
SRC = main11.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
	-shm_unlink /random_numbers_shm 2>/dev/null || true

.PHONY: all clean
