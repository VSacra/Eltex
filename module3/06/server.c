#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>  
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

// Структура сообщения
struct chat_msg {
    long mtype;      // Приоритет (ID получателя)
    int sender_id;   // ID отправителя
    char text[256];  // Текст сообщения
};

// Макросы для ID
#define SERVER_ID 10
#define MAX_CLIENTS 100
#define SHUTDOWN_MSG "shutdown"

volatile sig_atomic_t server_should_run = 1;

// Функция-обработчик сигналов
void handle_shutdown_signal(int signal_number) {
    printf("\n[СЕРВЕР] Получен сигнал завершения (%d)\n", signal_number);
    server_should_run = 0;  // Устанавливаем флаг завершения
}

int main() {
    int msg_queue_id;
    int active_clients[MAX_CLIENTS] = { 0 };  // Массив активных клиентов
    key_t queue_key;

    printf("Начинаем запуск сервера.\n");

    // SIGINT - сигнал при нажатии Ctrl+C
    // SIGTERM - сигнал завершения (от команды kill)
    signal(SIGINT, handle_shutdown_signal);
    signal(SIGTERM, handle_shutdown_signal);

    printf("[СЕРВЕР] Для завершения нажмите Ctrl+C\n");

    // Создаём ключ для очереди
    // Используем текущую директорию и ID проекта 'A'
    queue_key = ftok(".", 'A');
    if (queue_key == -1) {
        perror("Ошибка создания ключа очереди");
        return 1;
    }

    // Создаём очередь
    // IPC_CREAT - создать если не существует
    // 0666 - права на чтение и запись для всех
    msg_queue_id = msgget(queue_key, IPC_CREAT | 0666);
    if (msg_queue_id == -1) {
        perror("Ошибка создания очереди сообщений");
        return 1;
    }

    printf("Очередь создана. ID: %d\n", msg_queue_id);
    printf("Сервер готов (ID: %d)\n", SERVER_ID);
    printf("Ждем сообщения от клиентов...\n\n");

    // Проверяем флаг
    while (server_should_run) {
        struct chat_msg received_msg;
        struct chat_msg send_msg;
        int bytes_received;

        // mtype = 0 - получаем сообщение любого типа (от любого клиента)
        // Используем MSG_NOERROR чтобы не падать при ошибках размера
        bytes_received = msgrcv(msg_queue_id, &received_msg,
            sizeof(received_msg) - sizeof(long),
            0, 0);

        if (bytes_received == -1) {
            // Если msgrcv был прерван сигналом (errno == EINTR)
            if (errno == EINTR) {
                printf("[СЕРВЕР] Прием сообщений прерван сигналом\n");
                break;  // Выходим из цикла
            }
            perror("Ошибка приема сообщения");
            continue;  // Продолжаем работу
        }

        printf("[СЕРВЕР] Получено от %d: %s\n",
            received_msg.sender_id, received_msg.text);

        // Проверяем клиента
        // Если это первый раз видим этот ID, добавляем в активные
        int client_found = 0;
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (active_clients[i] == received_msg.sender_id) {
                client_found = 1;
                break;
            }
        }

        if (!client_found && received_msg.sender_id != SERVER_ID) {
            // Находим свободное место в массиве
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (active_clients[i] == 0) {
                    active_clients[i] = received_msg.sender_id;
                    printf("[СЕРВЕР] Новый клиент подключился: %d\n",
                        received_msg.sender_id);
                    break;
                }
            }
        }

        // shutdown
        if (strcmp(received_msg.text, SHUTDOWN_MSG) == 0) {
            printf("[СЕРВЕР] Клиент %d отключился\n", received_msg.sender_id);

            // Удаляем клиента из активных
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (active_clients[i] == received_msg.sender_id) {
                    active_clients[i] = 0;
                    break;
                }
            }
            continue;  // Не пересылаем shutdown другим
        }

        // Пересылаем сообщения всем
        // Копируем данные из полученного сообщения
        send_msg.sender_id = received_msg.sender_id;
        strcpy(send_msg.text, received_msg.text);

        for (int i = 0; i < MAX_CLIENTS; i++) {
            // Проверяем, не нужно ли завершить сервер
            if (!server_should_run) {
                break;
            }

            if (active_clients[i] != 0 &&
                active_clients[i] != received_msg.sender_id) {

                // Устанавливаем получателя
                send_msg.mtype = active_clients[i];

                // Отправляем сообщение
                if (msgsnd(msg_queue_id, &send_msg,
                    sizeof(send_msg) - sizeof(long), 0) == -1) {
                    perror("Ошибка отправки сообщения");
                    // Удаляем не отвечающего клиента
                    active_clients[i] = 0;
                }
                else {
                    printf("[СЕРВЕР] Отправлено клиенту %d\n",
                        active_clients[i]);
                }
            }
        }
    }


    printf("\n[СЕРВЕР] Завершение работы...\n");

    // Отправляем сообщение о завершении
    struct chat_msg shutdown_msg;
    shutdown_msg.sender_id = SERVER_ID;
    strcpy(shutdown_msg.text, "Сервер завершает работу");

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (active_clients[i] != 0) {
            shutdown_msg.mtype = active_clients[i];
            msgsnd(msg_queue_id, &shutdown_msg,
                sizeof(shutdown_msg) - sizeof(long), IPC_NOWAIT);
            // IPC_NOWAIT - не блокироваться если очередь полная
        }
    }

    // Удаляем очередь
    printf("[СЕРВЕР] Удаляю очередь сообщений...\n");
    if (msgctl(msg_queue_id, IPC_RMID, NULL) == -1) {
        perror("[СЕРВЕР] Ошибка удаления очереди");
    }
    else {
        printf("[СЕРВЕР] Очередь успешно удалена\n");
    }

    printf("[СЕРВЕР] Сервер завершил работу\n");
    return 0;
}