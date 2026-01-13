#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <errno.h>

// Имена очередей
#define QUEUE_SERVER "/server_queue"
#define QUEUE_CLIENT "/client_queue"
#define MAX_MSG_SIZE 256
#define EXIT_PRIORITY 255  // Приоритет для завершения

int main() {
    mqd_t server_queue, client_queue;
    struct mq_attr attr;
    char buffer[MAX_MSG_SIZE];
    unsigned int priority;
    int msg_count = 0;
    
    // Атрибуты очереди
    attr.mq_flags = 0;           // Блокирующий режим
    attr.mq_maxmsg = 10;         // Максимум сообщений в очереди
    attr.mq_msgsize = MAX_MSG_SIZE; // Максимальный размер сообщения
    attr.mq_curmsgs = 0;         // Текущее количество сообщений
    
    // Очередь для сервера
    mq_unlink(QUEUE_SERVER);  // Удаляем старую очередь если есть
    server_queue = mq_open(QUEUE_SERVER, O_CREAT | O_RDONLY, 0666, &attr);
    if (server_queue == (mqd_t)-1) {
        perror("Ошибка создания очереди сервера");
        return 1;
    }
    printf("[СЕРВЕР] Очередь создана: %s\n", QUEUE_SERVER);
    
    // Очередь клиента
    printf("[СЕРВЕР] Жду создания очереди клиента...\n");
    sleep(2);  // Даем время клиенту создать очередь
    
    client_queue = mq_open(QUEUE_CLIENT, O_WRONLY);
    if (client_queue == (mqd_t)-1) {
        perror("Ошибка открытия очереди клиента");
        mq_close(server_queue);
        mq_unlink(QUEUE_SERVER);
        return 1;
    }
    printf("[СЕРВЕР] Очередь клиента открыта: %s\n", QUEUE_CLIENT);
    
    printf("\n[СЕРВЕР] Начинаю обмен. Введите сообщения:\n");
    printf("[СЕРВЕР] Для завершения введите: 'exit'\n\n");
    
    while (1) {
        //Сообщение клиенту
        printf("[СЕРВЕР] > ");
        fflush(stdout);
        
        // Читаем ввод пользователя
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            break;
        }
        
        // Убираем символ новой строки
        buffer[strcspn(buffer, "\n")] = '\0';
        
        // Проверяем команду выхода
        if (strcmp(buffer, "exit") == 0) {
            printf("[СЕРВЕР] Отправляю сигнал завершения...\n");
            // Отправляем сообщение с приоритетом завершения
            if (mq_send(client_queue, buffer, strlen(buffer) + 1, EXIT_PRIORITY) == -1) {
                perror("Ошибка отправки завершения");
            }
            break;
        }
        
        // Отправляем обычное сообщение (приоритет 1)
        if (mq_send(client_queue, buffer, strlen(buffer) + 1, 1) == -1) {
            perror("Ошибка отправки сообщения");
            break;
        }
        
        msg_count++;
        printf("[СЕРВЕР] Отправлено сообщение #%d\n", msg_count);
        
        // Ответ от клиента
        printf("[СЕРВЕР] Жду ответ от клиента...\n");
        ssize_t bytes_read = mq_receive(server_queue, buffer, MAX_MSG_SIZE, &priority);
        
        if (bytes_read == -1) {
            perror("Ошибка приема сообщения");
            break;
        }
        
        // Проверяем приоритет завершения
        if (priority == EXIT_PRIORITY) {
            printf("[СЕРВЕР] Клиент завершил обмен\n");
            break;
        }
        
        printf("[КЛИЕНТ] > %s (приоритет: %u)\n", buffer, priority);
        
        // Проверяем, не хочет ли клиент выйти
        if (strcmp(buffer, "exit") == 0) {
            printf("[СЕРВЕР] Клиент запросил выход\n");
            break;
        }
    }
    
    // Завершение работы
    printf("\n[СЕРВЕР] Завершение работы...\n");
    printf("[СЕРВЕР] Всего отправлено сообщений: %d\n", msg_count);
    
    // Закрываем и удаляем очереди
    mq_close(server_queue);
    mq_close(client_queue);
    mq_unlink(QUEUE_SERVER);
    
    printf("[СЕРВЕР] Очереди закрыты. Сервер завершен.\n");
    return 0;
}