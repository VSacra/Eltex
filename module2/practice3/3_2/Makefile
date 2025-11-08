CC = gcc
CFLAGS = -std=c99 -Wall -g
TARGET = IPV4
SRCS = Main.c IPV4.c
build: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)


run: build
	./$(TARGET)

#Тест
test: build
	@(echo "1"; sleep 1; echo "192.169.1.0"; sleep 1; echo "/8"; sleep 1; echo "2000"; sleep 1; echo "1"; sleep 1; echo "120.167.12.1"; sleep 1; echo "0.0.0.0"; sleep 1; echo "30"; sleep 1; echo "4"; sleep 1; echo "212.130.1.0"; sleep 1; echo "/32"; sleep 1; echo "100"; sleep 1; echo "q") |tee /dev/tty | ./$(TARGET) || true

clean:
	rm -f $(TARGET)

.PHONY: build run clean test
