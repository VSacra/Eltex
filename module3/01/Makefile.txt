CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic
TARGET = program
SOURCE = Main01.c

run: $(TARGET)

# Компиляция программы
$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE)

# Запуск с тестовыми аргументами
test: $(TARGET)
	./$(TARGET) 123 45.67 test 1000 -5 3.14159

# Запуск без аргументов (для проверки сообщения)
test-empty: $(TARGET)
	./$(TARGET)

# Очистка
clean:
	rm -f $(TARGET)


.PHONY: run test test-empty clean 