#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>

// Глобальные переменные для работы с сокетом
int sockfd;                     // Дескриптор сокета
struct sockaddr_in cliaddr;     // Адрес собеседника

// Функция для корректного завершения программы
void cleanup() {
    close(sockfd);              // Закрываем сокет
    printf("Чат завершён\n");
    exit(0);                    // Завершаем программу
}

// Поток для отправки сообщений собеседнику
void* send_message(void* arg) {
    char sendline[1000];        // Буфер для ввода сообщения
    
    // Бесконечный цикл отправки сообщений
    while(1) {
        // Читаем строку с клавиатуры
        if (!fgets(sendline, sizeof(sendline), stdin)) {
            close(sockfd);
            break;
        }

        // Получаем длину сообщения
        size_t sendlen = strlen(sendline);
        
        // Отправляем сообщение через UDP сокет
        // sendto отправляет датаграмму без установки соединения
        if (sendto(sockfd, sendline, sendlen, 0, 
                  (struct sockaddr *)&cliaddr, sizeof(cliaddr)) != (ssize_t)sendlen) {
            perror("sendto");   // Выводим ошибку если есть
            close(sockfd);
            break;
        }
        
        // Проверяем команду выхода
        if (strcmp(sendline, "exit\n") == 0) {
            cleanup();          // Корректно завершаем программу
        }
    }
    return NULL;
}

// Поток для приема сообщений от собеседника
void* read_message(void* arg) {
    char recvline[1000];        // Буфер для приема сообщения
    socklen_t client = sizeof(cliaddr);  // Размер структуры адреса
    ssize_t n;                  // Количество полученных байт
    
    // Бесконечный цикл приема сообщений
    while(1) {
        // recvfrom получает датаграмму и адрес отправителя
        n = recvfrom(sockfd, recvline, sizeof(recvline) - 1, 0, 
                    (struct sockaddr *)&cliaddr, &client);
        
        // Если получены данные
        if (n >= 0) {
            // Проверяем команду выхода от собеседника
            if (strcmp(recvline, "exit\n") == 0) {
                cleanup();
            }
            recvline[n] = '\0';  // Добавляем нулевой символ в конец строки
            
            // Выводим полученное сообщение
            printf("[Собеседник]: %s", recvline);
        }
    }
    return NULL;
}

int main(int argc, char **argv) {
    pthread_t read_th, write_th;  // Идентификаторы потоков
    
    // Проверяем количество аргументов командной строки
    if (argc != 2) {
        printf("Неверно указаны аргументы. Формат: %s <IP_адрес_клиента1>\n", argv[0]);
        printf("Пример: %s 127.0.0.1\n", argv[0]);
        return 1;
    }

    // Создание UDP сокета
    // AF_INET - семейство адресов IPv4
    // SOCK_DGRAM - тип сокета для датаграмм (UDP)
    // 0 - протокол по умолчанию для UDP
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        return 1;
    }
    
    // F_SETFL - установка флагов файлового дескриптора
    // O_NONBLOCK - неблокирующий режим
    fcntl(sockfd, F_SETFL, O_NONBLOCK);

    // Настройка адреса собеседника
    memset(&cliaddr, 0, sizeof(cliaddr));  // Обнуляем структуру
    cliaddr.sin_family = AF_INET;           // Семейство адресов IPv4
    cliaddr.sin_port = htons(51000);        // Порт клиента 1 (51000)
                                            // htons - host to network short (порядок байт)
    
    // Преобразование строки с IP адресом в бинарный формат
    if (inet_aton(argv[1], &cliaddr.sin_addr) == 0) {
        printf("Ошибка: неверный IP адрес\n");
        close(sockfd);
        return 1;
    }
    
    // Настройка своего адреса
    struct sockaddr_in myaddr;
    memset(&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(51001);         // Мой порт (51001)
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);  // Принимать со всех интерфейсов
    
    // Привязка сокета
    // bind связывает сокет с конкретным адресом и портом
    if (bind(sockfd, (struct sockaddr*)&myaddr, sizeof(myaddr)) == -1) {
        perror("bind");
        close(sockfd);
        return 1;
    }
    
    printf("Клиент 2 запущен\n");
    printf("Мой порт: 51001\n");
    printf("Собеседник: %s:51000\n", argv[1]);
    printf("Для выхода введите: exit\n\n");
    
    // Создаём потоки
    // pthread_create создает новый поток
    // 1 поток: отправка сообщений (send_message)
    // 2 поток: прием сообщений (read_message)
    pthread_create(&write_th, NULL, send_message, NULL);
    pthread_create(&read_th, NULL, read_message, NULL);
    
    // pthread_join блокирует выполнение до завершения потока
    pthread_join(read_th, NULL); 
    pthread_join(write_th, NULL); 
    
    // Закрываем сокет
    close(sockfd);
    return 0;
}