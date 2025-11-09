CC = gcc
CFLAGS = -std=c99 -Wall
TARGET = phonebook
SRCS = Main4.c PhB4.c

build: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

run: build
	./$(TARGET)

test_add: build
	@echo "=== Тест добавления контактов ==="
	@(echo "1"; sleep 0.3; echo "Иван"; sleep 0.3; echo "Иванов"; sleep 0.3; echo "1"; sleep 0.3; echo "Иванович"; sleep 0.3; echo "1"; sleep 0.3; echo "1"; sleep 0.3; echo "88005553535"; sleep 0.3; echo "1"; sleep 0.3; echo "vk.com/ivanov"; sleep 0.3; echo "2"; sleep 0.3; echo "2"; sleep 0.3; echo "4"; sleep 0.3; echo "5") |tee /dev/tty | ./$(TARGET) || true

test_multiple: build
	@echo "=== Тест Несколько контактов ==="
	@(echo "1"; sleep 0.3; echo "Иван"; sleep 0.3; echo "Иванов"; sleep 0.3; echo "2"; sleep 0.3; echo "2"; sleep 0.3; echo "2"; sleep 0.3; echo "1"; sleep 0.3; echo "Петр"; sleep 0.3; echo "Петров"; sleep 0.3; echo "2"; sleep 0.3; echo "2"; sleep 0.3; echo "2"; sleep 0.3; echo "1"; sleep 0.3; echo "Анна"; sleep 0.3; echo "Смирнова"; sleep 0.3; echo "2"; sleep 0.3; echo "2"; sleep 0.3; echo "2"; sleep 0.3; echo "4"; sleep 0.3; echo "5") |tee /dev/tty | ./$(TARGET) || true

test_add_delete: build
	@echo "=== Тест Добавление и удаление ==="
	@(echo "1"; sleep 0.3; echo "Удалить"; sleep 0.3; echo "Меня"; sleep 0.3; echo "2"; sleep 0.3; echo "2"; sleep 0.3; echo "2"; sleep 0.3; echo "4"; sleep 0.3; echo "3"; sleep 0.3; echo "1"; sleep 0.3; echo "4"; sleep 0.3; echo "5") |tee /dev/tty | ./$(TARGET) || true

test_phones: build
	@echo "=== Тест телефонных номеров ==="
	@(echo "1"; sleep 0.3; echo "Максимум"; sleep 0.3; echo "Телефонов"; sleep 0.3; echo "2"; sleep 0.3; echo "1"; sleep 0.3; echo "5"; sleep 0.3; echo "111"; sleep 0.3; echo "222"; sleep 0.3; echo "333"; sleep 0.3; echo "444"; sleep 0.3; echo "555"; sleep 0.3; echo "2"; sleep 0.3; echo "4"; sleep 0.3; echo "5") |tee /dev/tty | ./$(TARGET) || true

test_edit: build
	@echo "=== Тест Редактирование контакта ==="
	@(echo "1"; sleep 0.3; echo "СтароеИмя"; sleep 0.3; echo "СтараяФамилия"; sleep 0.3; echo "2"; sleep 0.3; echo "2"; sleep 0.3; echo "2"; sleep 0.3; echo "4"; sleep 0.3; echo "2"; sleep 0.3; echo "1"; sleep 0.3; echo "1"; sleep 0.3; echo "НовоеИмя"; sleep 0.3; echo "2"; sleep 0.3; echo "2"; sleep 0.3; echo "2"; sleep 0.3; echo "2"; sleep 0.3; echo "4"; sleep 0.3; echo "5") |tee /dev/tty | ./$(TARGET) || true

test_empty: build
	@echo "=== Тест пустой книги ==="
	@(echo "4"; sleep 0.3; echo "5") |tee /dev/tty | ./$(TARGET) || true

test_social: build
	@echo "=== Тест соцсетей ==="
	@(echo "1"; sleep 0.3; echo "Соц"; sleep 0.3; echo "Сети"; sleep 0.3; echo "2"; sleep 0.3; echo "2"; sleep 0.3; echo "1"; sleep 0.3; echo "1"; sleep 0.3; echo "vk.com/test"; sleep 0.3; echo "1"; sleep 0.3; echo "ok.ru/test"; sleep 0.3; echo "1"; sleep 0.3; echo "t.me/test"; sleep 0.3; echo "4"; sleep 0.3; echo "5") |tee /dev/tty | ./$(TARGET) || true

# Запуск всех тестов
test_all: build
	@echo "=== ЗАПУСК ВСЕХ ТЕСТОВ ==="
	@make test_empty
	@make test_add
	@make test_multiple
	@make test_add_delete
	@make test_phones
	@make test_edit
	@make test_social
	@echo "=== ВСЕ ТЕСТЫ ЗАВЕРШЕНЫ ==="

# Быстрый тест (без задержек, для проверки)
test_quick: build
	@echo "=== Быстрый тест ==="
	@echo -e "4\n5" |tee /dev/tty | timeout 5s ./$(TARGET) || true

clean:
	rm -f $(TARGET)

.PHONY: build run clean test_add test_multiple test_add_delete test_phones test_edit test_empty test_social test_all test_quick
