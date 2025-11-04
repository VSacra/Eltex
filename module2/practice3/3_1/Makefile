CC = gcc
CFLAGS = -std=c99 -Wall -g
TARGET = Chmod
SRCS = Main3_1.c chmod3_1.c
build: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)


run: build
	./$(TARGET)

#Тест
test: build
	@(echo "1"; sleep 1; echo "rwxr-x---"; sleep 1; echo "2"; sleep 1; echo "000000000"; sleep 1; echo "531"; sleep 1; echo "3"; sleep 1; echo "Main3_1.c"; sleep 1; echo "4"; sleep 1;echo "chmod3_1.c"; sleep 1; echo "u+x,g-r,o=x"; sleep 1; echo "q") |tee /dev/tty | ./$(TARGET) || true

clean:
	rm -f $(TARGET)

.PHONY: build run clean test
