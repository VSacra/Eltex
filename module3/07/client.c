#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <errno.h>

// Имена очередей (должны совпадать с сервером)
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
    
    // Очередь клиента
    mq_unlink(QUEUE_CLIENT);  // Удаляем старую очередь если есть
    client_queue = mq_open(QUEUE_CLIENT, O_CREAT | O_RDONLY, 0666, &attr);
    if (client_queue == (mqd_t)-1) {
        perror("Ошибка создания очереди клиента");
        return 1;
    }
    printf("[КЛИЕНТ] Очередь создана: %s\n", QUEUE_CLIENT);
    
    printf("[КЛИЕНТ] Подключаюсь к серверу...\n");

    //Открываем очередь сервера

    server_queue = mq_open(QUEUE_SERVER, O_WRONLY);
    if (server_queue == (mqd_t)-1) {
        perror("Ошибка открытия очереди сервера");
        mq_close(client_queue);
        mq_unlink(QUEUE_CLIENT);
        return 1;
    }
    printf("[КЛИЕНТ] Очередь сервера открыта: %s\n", QUEUE_SERVER);
    
    printf("\n[КЛИЕНТ] Жду первое сообщение от сервера...\n");
    
    while (1) {
        // Получаем сообщение
        ssize_t bytes_read = mq_receive(client_queue, buffer, MAX_MSG_SIZE, &priority);
        
        if (bytes_read == -1) {
            perror("Ошибка приема сообщения");
            break;
        }
        
        // Проверяем приоритет завершения
        if (priority == EXIT_PRIORITY) {
            printf("[КЛИЕНТ] Сервер завершил обмен\n");
            break;
        }
        
        printf("[СЕРВЕР] > %s (приоритет: %u)\n", buffer, priority);
        
        // Проверяем, не хочет ли сервер выйти
        if (strcmp(buffer, "exit") == 0) {
            printf("[КЛИЕНТ] Сервер запросил выход\n");
            break;
        }
        
        // Отправляем ответ
        printf("[КЛИЕНТ] > ");
        fflush(stdout);
        
        // Читаем ввод пользователя
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            break;
        }
        
        // Убираем символ новой строки
        buffer[strcspn(buffer, "\n")] = '\0';
        
        // Проверяем команду выхода
        if (strcmp(buffer, "exit") == 0) {
            printf("[КЛИЕНТ] Отправляю сигнал завершения...\n");
            // Отправляем сообщение с приоритетом завершения
            if (mq_send(server_queue, buffer, strlen(buffer) + 1, EXIT_PRIORITY) == -1) {
                perror("Ошибка отправки завершения");
            }
            break;
        }
        
        // Отправляем обычное сообщение (приоритет 1)
        if (mq_send(server_queue, buffer, strlen(buffer) + 1, 1) == -1) {
            perror("Ошибка отправки сообщения");
            break;
        }
        
        msg_count++;
        printf("[КЛИЕНТ] Отправлено сообщение #%d\n", msg_count);
    }
    
    // Завершение работы
    printf("\n[КЛИЕНТ] Завершение работы...\n");
    printf("[КЛИЕНТ] Всего отправлено сообщений: %d\n", msg_count);
    
    // Закрываем и удаляем очереди
    mq_close(server_queue);
    mq_close(client_queue);
    mq_unlink(QUEUE_CLIENT);
    
    printf("[КЛИЕНТ] Очереди закрыты. Клиент завершен.\n");
    return 0;
}