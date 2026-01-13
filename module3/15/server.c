#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <errno.h>
#include <math.h>

#define MAX_CLIENTS 30
#define BUFFER_SIZE 1024
#define PORT 1234

void error(const char *msg) {
    perror(msg);
    exit(1);
}

// Функция для математических операций 
double calculate(double a, double b, char op) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': 
            if (b != 0.0) return a / b;
            else return INFINITY;
        default: return NAN;
    }
}

// Функция для разбора математического выражения
int parse_expression(const char *str, double *num1, double *num2, char *op) {
    char *endptr;
    *num1 = strtod(str, &endptr);
    
    while (*endptr == ' ') endptr++;
    *op = *endptr;
    endptr++;
    
    while (*endptr == ' ') endptr++;
    *num2 = strtod(endptr, NULL);
    
    return 1;
}

//Функция обработки комманд
void handle_client_command(int client_fd, char *buffer) {
    char response[BUFFER_SIZE];
    buffer[strcspn(buffer, "\n")] = 0;
    
    if (strcmp(buffer, "quit") == 0) {
        snprintf(response, sizeof(response), "До свидания!\n");
        send(client_fd, response, strlen(response), 0);
        return;
    }
    
    if (strcmp(buffer, "help") == 0) {
        snprintf(response, sizeof(response), 
            "Доступные команды:\n"
            "10 + 5 - сложение\n"
            "10 - 5 - вычитание\n"
            "10 * 5 - умножение\n"
            "10 / 5 - деление\n"
            "help - справка\n"
            "quit - выход\n");
        send(client_fd, response, strlen(response), 0);
        return;
    }
    
    if (strchr(buffer, '+') || strchr(buffer, '-') || 
        strchr(buffer, '*') || strchr(buffer, '/')) {
        
        double num1, num2;
        char op;
        
        if (parse_expression(buffer, &num1, &num2, &op)) {
            double result = calculate(num1, num2, op);
            
            if (isnan(result)) {
                snprintf(response, sizeof(response), "Ошибка: неизвестная операция\n");
            }
            else if (isinf(result)) {
                snprintf(response, sizeof(response), "Ошибка: деление на ноль\n");
            }
            else {
                snprintf(response, sizeof(response), "Ответ: %.2f\n", result);
            }
        }
        else {
            snprintf(response, sizeof(response), "Ошибка: неверный формат\n");
        }
    }
    else {
        snprintf(response, sizeof(response), "Эхо: %s\n", buffer);
    }
    
    send(client_fd, response, strlen(response), 0);
}

int main() {
    int master_socket, new_socket;
    int client_fds[MAX_CLIENTS + 1];
    struct pollfd fds[MAX_CLIENTS + 1];
    int nfds = 1;
    int i, timeout = -1;
    struct sockaddr_in address;
    char buffer[BUFFER_SIZE];
    
    printf("TCP СЕРВЕР \n");
    
    // Инициализация клиентских сокетов
    for (i = 0; i < MAX_CLIENTS; i++) {
        client_fds[i] = -1;
    }
    
    // Создание главного сокета
    if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        error("Ошибка создания сокета");
    }
    
    // Настройка сокета
    int opt = 1;
    if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0) {
        error("Ошибка setsockopt");
    }
    
    // Настройка адреса
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        error("Ошибка привязки");
    }
    
    if (listen(master_socket, 3) < 0) {
        error("Ошибка listen");
    }
    
    // Настройка poll для главного сокета
    fds[0].fd = master_socket;
    fds[0].events = POLLIN;
    
    char *welcome_msg = 
        "Добро пожаловать на сервер!\n"
        "Доступные команды:\n"
        "10 + 5 - сложение\n"
        "10 - 5 - вычитание\n"
        "10 * 5 - умножение\n"
        "10 / 5 - деление\n"
        "help - справка\n"
        "quit - выход\n"
        "Введите команду: ";
    
    while (1) {
        // Ожидаем события
        int ret = poll(fds, nfds, timeout);
        
        if (ret < 0) {
            error("Ошибка poll");
        }
        
        if (ret == 0) {
            continue; // Таймаут
        }
        
        // Проверяем все дескрипторы
        int current_size = nfds;
        for (i = 0; i < current_size; i++) {
            if (fds[i].revents == 0) {
                continue;
            }
            
            // Новое подключение на главном сокете
            if (fds[i].fd == master_socket) {
                int addrlen = sizeof(address);
                
                if ((new_socket = accept(master_socket, (struct sockaddr *)&address, 
                                        (socklen_t*)&addrlen)) < 0) {
                    error("Ошибка accept");
                }
                
                printf("Новое подключение: %s:%d\n",
                       inet_ntoa(address.sin_addr), ntohs(address.sin_port));
                
                send(new_socket, welcome_msg, strlen(welcome_msg), 0);
                
                // Добавляем новый сокет
                int j;
                for (j = 0; j < MAX_CLIENTS; j++) {
                    if (client_fds[j] == -1) {
                        client_fds[j] = new_socket;
                        
                        fds[nfds].fd = new_socket;
                        fds[nfds].events = POLLIN;
                        nfds++;
                        
                        printf("Клиент добавлен в слот %d\n\n", j);
                        break;
                    }
                }
                
                if (j == MAX_CLIENTS) {
                    char *msg = "Сервер переполнен\n";
                    send(new_socket, msg, strlen(msg), 0);
                    close(new_socket);
                }
            }
            // Данные от клиента
            else {
                int valread;
                
                if ((valread = read(fds[i].fd, buffer, BUFFER_SIZE)) <= 0) {
                    // Клиент отключился
                    struct sockaddr_in addr;
                    socklen_t addr_len = sizeof(addr);
                    getpeername(fds[i].fd, (struct sockaddr*)&addr, &addr_len);
                    
                    printf("Клиент отключился: %s:%d\n",
                           inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
                    
                    close(fds[i].fd);
                    
                    // Удаляем из массива
                    int j;
                    for (j = 0; j < MAX_CLIENTS; j++) {
                        if (client_fds[j] == fds[i].fd) {
                            client_fds[j] = -1;
                            break;
                        }
                    }
                    
                    // Удаляем из poll
                    fds[i].fd = -1;
                }
                else {
                    buffer[valread] = '\0';
                    handle_client_command(fds[i].fd, buffer);
                }
            }
        }
    }
    
    return 0;
}