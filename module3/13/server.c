#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

int nclients = 0;
void dostuff(int);

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

// Функция для математических операций
double calculate(double a, double b, char op)
{
    switch (op)
    {
        case '+':
            return a + b;
        case '-':
            return a - b;
        case '*':
            return a * b;
        case '/':
            if (b != 0.0) return a / b;
            else return INFINITY;
        default:
            return NAN;
    }
}

// Функция для разбора математического выражения
int parse_expression(const char *str, double *num1, double *num2, char *op)
{
    char *endptr;
    *num1 = strtod(str, &endptr);
    
    // Пропускаем пробелы
    while (*endptr == ' ') endptr++;
    
    // Получаем оператор
    *op = *endptr;
    endptr++;
    
    // Пропускаем пробелы
    while (*endptr == ' ') endptr++;
    
    // Получаем второе число
    *num2 = strtod(endptr, NULL);
    
    return 1;
}

// Функция для приёма файла
void receiveFile(int sockfd) 
{
    size_t filename_len;
    read(sockfd, &filename_len, sizeof(size_t));

    char filename[filename_len + 1];
    read(sockfd, filename, filename_len);
    filename[filename_len] = '\0';

    off_t fileSize;
    read(sockfd, &fileSize, sizeof(off_t));

    FILE *fp = fopen(filename, "wb");
    if(fp == NULL) error ("Ошибка открытия файла\n");

    unsigned char buffer[1024];
    ssize_t totalReceived = 0;
    while(totalReceived < fileSize) 
    {
        ssize_t bytesRead = read(sockfd, buffer, sizeof(buffer));
        fwrite(buffer, 1, bytesRead, fp);
        totalReceived += bytesRead;
    }

    fclose(fp);
    char buf[1024];
    snprintf(buf, sizeof(buf), "Файл '%s' (%ld байт) получен\n", filename, (long)fileSize);
    printf("%s", buf);
    send(sockfd, buf, strlen(buf), 0);
}

void printusers()
{
    if (nclients) 
    {
        printf("Клиентов онлайн: %d\n\n", nclients);
    }
    else 
    {
        printf("Нет подключений\n\n");
    }
}

int main(int argc, char *argv[])
{
    char buffer[1024];
    int sockfd, newsockfd;
    int portno;
    int pid;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    struct addrinfo hints, *res;
    char hostname[NI_MAXHOST];
    char service[NI_MAXSERV];

    printf("TCP СЕРВЕР \n");

    if (argc < 2) 
    {
        fprintf(stderr, "Использование: %s <порт>\n", argv[0]);
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("Ошибка сокета");

    memset(&serv_addr, 0, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
        error("Ошибка привязки");

    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    
    printf("Сервер запущен на порту %d\n", portno);
    printusers();
    
    while (1)
    {   
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if(newsockfd < 0) error("Ошибка accept");
        
        nclients++;

        getnameinfo((struct sockaddr*)&cli_addr, clilen, hostname, NI_MAXHOST, 
            service, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

        printf("+ %s [%s] подключился\n", hostname, inet_ntoa(cli_addr.sin_addr));
        printusers();

        pid = fork();
        if (pid < 0) error("Ошибка fork");
        if (pid == 0) 
        {
            close(sockfd);
            dostuff(newsockfd);
            exit(0);
        }
        else close(newsockfd);
    }

    close(sockfd);
    return 0;
}

void dostuff(int sock)
{
    int bytes_recv;
    char buffer[1024];
    
    char *help_msg = "Доступные команды:\n"
                     "10 + 5 - сложение\n"
                     "10 - 5 - вычитание\n"
                     "10 * 5 - умножение\n"
                     "10 / 5 - деление\n"
                     "file имя - отправить файл\n"
                     "quit - выход\n"
                     "help - справка\n"
                     "Введите команду: ";
    
    send(sock, help_msg, strlen(help_msg), 0);
    
    while (1)
    {
        memset(buffer, 0, sizeof(buffer));
        bytes_recv = recv(sock, buffer, sizeof(buffer), 0);
        
        if (bytes_recv <= 0) 
        {
            printf("Клиент отключился\n");
            break;
        }
        
        buffer[strcspn(buffer, "\n")] = 0;
        
        // Команда выхода
        if (strcmp(buffer, "quit") == 0)
        {
            char *msg = "До свидания!\n";
            send(sock, msg, strlen(msg), 0);
            break;
        }
        
        // Команда справки
        else if (strcmp(buffer, "help") == 0)
        {
            send(sock, help_msg, strlen(help_msg), 0);
        }
        
        // Команда отправки файла
        else if (strncmp(buffer, "file ", 5) == 0)
        {
            char filename[256];
            if (sscanf(buffer + 5, "%s", filename) == 1)
            {
                char msg[256];
                snprintf(msg, sizeof(msg), "Готов принять файл: %s\n", filename);
                send(sock, msg, strlen(msg), 0);
                
                receiveFile(sock);
            }
            else
            {
                char *msg = "Ошибка: укажите имя файла\n";
                send(sock, msg, strlen(msg), 0);
            }
        }
        
        // Математическое выражение (проверяем, содержит ли один из операторов)
        else if (strchr(buffer, '+') || strchr(buffer, '-') || 
                 strchr(buffer, '*') || strchr(buffer, '/'))
        {
            double num1, num2;
            char op;
            
            if (parse_expression(buffer, &num1, &num2, &op))
            {
                double result = calculate(num1, num2, op);
                
                if (isnan(result))
                {
                    char *msg = "Ошибка: неизвестная операция\n";
                    send(sock, msg, strlen(msg), 0);
                }
                else if (isinf(result))
                {
                    char *msg = "Ошибка: деление на ноль\n";
                    send(sock, msg, strlen(msg), 0);
                }
                else
                {
                    char response[256];
                    snprintf(response, sizeof(response), "Ответ: %.2f\n", result);
                    send(sock, response, strlen(response), 0);
                }
            }
            else
            {
                char *msg = "Ошибка: неверный формат выражения\n";
                send(sock, msg, strlen(msg), 0);
            }
        }
        
        // Любой другой текст
        else
        {
            char response[256];
            snprintf(response, sizeof(response), "Вы сказали: %s\n", buffer);
            send(sock, response, strlen(response), 0);
        }
        
        // Запрос следующей команды
        char *prompt = "Введите команду: ";
        send(sock, prompt, strlen(prompt), 0);
    }
    
    nclients--;
    printf("- Клиент отключился\n");
    printusers();
    close(sock);
}