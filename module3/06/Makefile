CC = gcc
CFLAGS = -Wall -Wextra -std=c99
SERVER = server
CLIENT = client
SERVER_SRC = server.c
CLIENT_SRC = client.c
SERVER_OBJ = $(SERVER_SRC:.c=.o)
CLIENT_OBJ = $(CLIENT_SRC:.c=.o)

all: $(SERVER) $(CLIENT)

$(SERVER): $(SERVER_OBJ)
	$(CC) $(CFLAGS) -o $(SERVER) $(SERVER_OBJ)

$(CLIENT): $(CLIENT_OBJ)
	$(CC) $(CFLAGS) -o $(CLIENT) $(CLIENT_OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run_server: $(SERVER)
	./$(SERVER)

run_client: $(CLIENT)
	@if [ -z "$(ID)" ]; then \
		echo "Использование: make run_client ID=номер"; \
		echo "Пример: make run_client ID=20"; \
		exit 1; \
	fi
	./$(CLIENT) $(ID)

clean:
	rm -f $(SERVER) $(CLIENT) $(SERVER_OBJ) $(CLIENT_OBJ)

.PHONY: all run_server run_client clean
