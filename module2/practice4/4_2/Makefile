# Компилятор и флаги
CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = queue_program
SOURCES = Main4_2.c Queue.c

build: $(TARGET)

# Основная цель
$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)

run: build
	./$(TARGET)

# Цель для тестирования
test: $(TARGET)
	@(echo "1"; sleep 0.3; echo "3"; sleep 0.3; echo "6"; sleep 0.3; echo "2"; sleep 0.3; echo "X"; sleep 0.3; echo "100"; sleep 0.3; echo "6"; sleep 0.3; echo "3"; sleep 0.3; echo "1"; sleep 0.3; echo "4"; sleep 0.3; echo "6"; sleep 0.3; echo "4"; sleep 0.3; echo "50"; sleep 0.3; echo "5"; sleep 0.3; echo "150"; sleep 0.3; echo "6"; sleep 0.3; echo "q") | tee /dev/tty | ./$(TARGET) || true

# Очистка
clean:
	rm -f $(TARGET)

.PHONY: build run test clean
