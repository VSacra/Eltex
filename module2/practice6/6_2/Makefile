CC = gcc
CFLAGS = -std=c99 -Wall -fPIC
TARGET = phonebook
LIBRARY = libphonebook.so
LDFLAGS = -L. -lphonebook -Wl,-rpath,.

# Основная цель
all: $(TARGET)

# Сборка основной программы с динамической библиотекой
$(TARGET): Main6_2.c $(LIBRARY)
	$(CC) $(CFLAGS) -o $(TARGET) Main6_2.c $(LDFLAGS)

# Создание динамической библиотеки
$(LIBRARY): PhB6_2.c
	$(CC) $(CFLAGS) -shared -o $(LIBRARY) PhB6_2.c

# Очистка
clean:
	rm -f $(TARGET) $(LIBRARY)

# Запуск
run: $(TARGET)
	./$(TARGET)

# Просмотр зависимостей
deps:
	ldd $(TARGET)

.PHONY: all clean run deps
