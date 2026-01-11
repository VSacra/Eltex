# Makefile для задания 5: Обработка сигналов
CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = main05
SRCS = main05.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) $(OBJS) output.txt

.PHONY: all run clean
