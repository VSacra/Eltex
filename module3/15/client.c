#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/stat.h>

void error(const char *msg) 
{
    perror(msg);
    exit(0);
}

// Функция для отправки файла
void sendFile(int sockfd, const char* filename) 
{
    struct stat st;
    
    if(stat(filename, &st) != 0) 
    {
        printf("Ошибка: файл '%s' не найден\n", filename);
        return;
    }

    printf("Отправка файла '%s'\n", filename);
    
    size_t filename_len = strlen(filename);
    write(sockfd, &filename_len, sizeof(size_t));
    write(sockfd, filename, filename_len);

    off_t fileSize = st.st_size;
    write(sockfd, &fileSize, sizeof(off_t));

    FILE *fp = fopen(filename, "rb");
    if(fp == NULL) error ("Ошибка открытия файла\n");

    unsigned char buffer[1024];
    ssize_t bytes_read;
    size_t total_sent = 0;
    
    while((bytes_read = fread(buffer, 1, sizeof(buffer), fp)) > 0) 
    {
        write(sockfd, buffer, bytes_read);
        total_sent += bytes_read;
        printf("\rОтправлено: %ld/%ld байт", (long)total_sent, (long)fileSize);
        fflush(stdout);
    }
    
    printf("\nФайл отправлен!\n");
    fclose(fp);
}

int main(int argc, char *argv[])
{
    int my_sock, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[1024];
    
    printf("TCP КЛИЕНТ\n");
    
    if (argc < 3) 
    {
        fprintf(stderr, "Использование: %s <сервер> <порт>\n", argv[0]);
        fprintf(stderr, "Пример: %s localhost 1234\n", argv[0]);
        exit(0);
    }

    portno = atoi(argv[2]);
    my_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (my_sock < 0) error("Ошибка сокета");

    server = gethostbyname(argv[1]);
    if (server == NULL) 
    {
        fprintf(stderr, "Ошибка: хост не найден\n");
        exit(0);
    }
    
    bzero((char*) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char*)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(my_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("Ошибка подключения");
    
    printf("Подключено к %s:%d\n", argv[1], portno);
    
    
    // Основной цикл
    while (1)
    {
        // Получаем приглашение от сервера
        n = recv(my_sock, buffer, sizeof(buffer) - 1, 0);
        if (n > 0) {
            buffer[n] = 0;
            printf("%s", buffer);
        }
        
        // Ввод команды
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        
        // Выход
        if (strcmp(buffer, "quit") == 0) 
        {
            strcat(buffer, "\n");
            send(my_sock, buffer, strlen(buffer), 0);
            
            // Получаем прощальное сообщение
            n = recv(my_sock, buffer, sizeof(buffer) - 1, 0);
            if (n > 0) {
                buffer[n] = 0;
                printf("%s", buffer);
            }
            
            printf("Выход...\n");
            close(my_sock);
            return 0;
        }
        
        // Отправка файла
        else if (strncmp(buffer, "file ", 5) == 0)
        {
            char filename[256];
            if (sscanf(buffer + 5, "%s", filename) == 1)
            {
                // Отправляем команду
                strcat(buffer, "\n");
                send(my_sock, buffer, strlen(buffer), 0);
                
                // Получаем подтверждение
                n = recv(my_sock, buffer, sizeof(buffer) - 1, 0);
                if (n > 0) {
                    buffer[n] = 0;
                    printf("%s", buffer);
                }
                
                // Отправляем файл
                sendFile(my_sock, filename);
                
                // Получаем результат
                n = recv(my_sock, buffer, sizeof(buffer) - 1, 0);
                if (n > 0) {
                    buffer[n] = 0;
                    printf("%s", buffer);
                }
            }
            else
            {
                printf("Ошибка: укажите имя файла\n");
            }
        }
        
        // Любая другая команда (математика или текст)
        else
        {
            strcat(buffer, "\n");
            send(my_sock, buffer, strlen(buffer), 0);
            
            // Получаем ответ
            n = recv(my_sock, buffer, sizeof(buffer) - 1, 0);
            if (n > 0) {
                buffer[n] = 0;
                printf("%s", buffer);
            }
        }
    }
    
    close(my_sock);
    return 0;
}
