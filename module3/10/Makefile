CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = main10
SRC = main10.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
	-ipcrm -m 12345 2>/dev/null || true

.PHONY: all clean
