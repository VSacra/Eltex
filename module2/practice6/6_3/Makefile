CC = gcc
CFLAGS = -std=c99 -Wall -fPIC
TARGET = calculator
LIB_DIR = libs
LIBS = $(LIB_DIR)/libsum.so $(LIB_DIR)/libsub.so $(LIB_DIR)/libmulti.so $(LIB_DIR)/libdivide.so
LDFLAGS = -ldl

# Основная цель
all: $(LIBS) $(TARGET)

# Создание каталога для библиотек
$(LIB_DIR):
	mkdir -p $(LIB_DIR)

# Компиляция отдельных библиотек
$(LIB_DIR)/libsum.so: sum.c $(LIB_DIR)
	$(CC) $(CFLAGS) -shared -o $@ sum.c

$(LIB_DIR)/libsub.so: sub.c $(LIB_DIR)
	$(CC) $(CFLAGS) -shared -o $@ sub.c

$(LIB_DIR)/libmulti.so: multi.c $(LIB_DIR)
	$(CC) $(CFLAGS) -shared -o $@ multi.c

$(LIB_DIR)/libdivide.so: divide.c $(LIB_DIR)
	$(CC) $(CFLAGS) -shared -o $@ divide.c

# Компиляция основной программы
$(TARGET): Main6_3.c
	$(CC) $(CFLAGS) -o $(TARGET) Main6_3.c $(LDFLAGS)

# Очистка
clean:
	rm -f $(TARGET)
	rm -rf $(LIB_DIR)

# Запуск
run: all
	./$(TARGET)

# Просмотр загруженных библиотек
test: all
	@echo "=== Проверка библиотек в каталоге ==="
	@ls -la $(LIB_DIR)/
	@echo "=== Запуск программы ==="
	./$(TARGET)

.PHONY: all clean run test
