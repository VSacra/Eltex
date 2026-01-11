#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

// Та же структура, что и у сервера
struct chat_msg {
    long mtype;      // Приоритет (ID получателя)
    int sender_id;   // ID отправителя
    char text[256];  // Текст сообщения
};

// Макросы
#define SERVER_ID 10
#define SHUTDOWN_MSG "shutdown"

int main(int argc, char* argv[]) {
    int msg_queue_id;
    int my_id;
    key_t queue_key;
    pid_t child_pid;

    // Проверяем аргументы
    if (argc != 2) {
        printf("Использование: %s <ID_клиента>\n", argv[0]);
        printf("Пример: %s 20\n", argv[0]);
        printf("ID должно быть 20, 30, 40 и т.д. (не 10)\n");
        return 1;
    }

    my_id = atoi(argv[1]);
    if (my_id == SERVER_ID) {
        printf("Ошибка: ID %d зарезервирован для сервера\n", SERVER_ID);
        return 1;
    }

    printf("Запуск клиента ID: %d\n", my_id);

    // Подключаемся к очереди
    // Используем тот же ключ, что и сервер
    queue_key = ftok(".", 'A');
    if (queue_key == -1) {
        perror("Ошибка создания ключа");
        return 1;
    }

    msg_queue_id = msgget(queue_key, 0666);
    if (msg_queue_id == -1) {
        perror("Ошибка подключения к очереди");
        printf("Убедитесь, что сервер запущен\n");
        return 1;
    }

    printf("Подключен к очереди сообщений\n");
    printf("Для выхода введите: shutdown\n\n");

    // Дочерний процесс для приёма сообщений
    child_pid = fork();

    if (child_pid == -1) {
        perror("Ошибка создания процесса");
        return 1;
    }

    if (child_pid == 0) { //Если дочерний
        printf("[Клиент %d] Ожидаю сообщения...\n", my_id);

        while (1) {
            struct chat_msg msg;
            int bytes_received;

            // Получаем сообщения, адресованные ЭТОМУ клиенту (mtype=my_id)
            bytes_received = msgrcv(msg_queue_id, &msg,
                sizeof(msg) - sizeof(long),
                my_id, 0);

            if (bytes_received == -1) {
                perror("Ошибка приема");
                continue;
            }

            // Выводим полученное сообщение
            printf("\n[Сообщение от %d]: %s\n",
                msg.sender_id, msg.text);
            printf("[Клиент %d] > ", my_id);
            fflush(stdout);  // Обновляем вывод
        }

        exit(0);  // Дочерний процесс завершается
    }

    // Если родительский, то отправляем сообщения
    else {
        printf("[Клиент %d] Вводите сообщения:\n", my_id);

        while (1) {
            struct chat_msg msg;
            char input[256];

            // Приглашение для ввода
            printf("[Клиент %d] > ", my_id);
            fflush(stdout);

            // Читаем ввод пользователя
            if (fgets(input, sizeof(input), stdin) == NULL) {
                break;  
            }

            // Убираем символ новой строки
            input[strcspn(input, "\n")] = '\0';

            // Проверяем пустой ввод
            if (strlen(input) == 0) {
                continue;
            }
            msg.mtype = SERVER_ID;      // Отправляем серверу
            msg.sender_id = my_id;      // Указываем свой ID
            strcpy(msg.text, input);    // Копируем текст

            // Отправляем сообщение серверу
            if (msgsnd(msg_queue_id, &msg,
                sizeof(msg) - sizeof(long), 0) == -1) {
                perror("Ошибка отправки");
            }
            else {
                printf("[Клиент %d] Отправлено серверу\n", my_id);
            }

            // Проверяем отправляли ли мы сообщение о завершении работы
            if (strcmp(input, SHUTDOWN_MSG) == 0) {
                printf("[Клиент %d] Завершение работы...\n", my_id);

                // Убиваем дочерний процесс
                kill(child_pid, SIGTERM);

                // Ждем завершения дочернего процесса
                wait(NULL);

                break;  // Выходим из цикла
            }
        }
    }

    printf("[Клиент %d] Отключен\n", my_id);
    return 0;
}