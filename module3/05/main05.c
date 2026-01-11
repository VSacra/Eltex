#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

// Используем стандартные типы для сигналов
// sig_atomic_t гарантирует атомарный доступ
volatile sig_atomic_t sigint_counter = 0;  // Счетчик SIGINT
volatile sig_atomic_t should_exit = 0;     // Флаг выхода
volatile sig_atomic_t got_signal = 0;      // Флаг получения сигнала
volatile sig_atomic_t last_signal = 0;     // Какой сигнал последний

// Функция-обработчик сигналов
void handle_signal(int sig_num) {
    got_signal = 1;
    last_signal = sig_num;

    if (sig_num == SIGINT) {
        sigint_counter++;
        if (sigint_counter >= 3) {
            should_exit = 1;  // После 3-го SIGINT - выходим
        }
    }
    // Для SIGQUIT ничего не меняем, только отметим
}

int main() {
    FILE* file = NULL;
    int counter = 1;
    char filename[] = "output.txt";

    file = fopen(filename, "a");  // "a" - append (дописывать в конец)
    if (file == NULL) {
        fprintf(stderr, "Ошибка открытия файла %s: %s\n",
            filename, strerror(errno));
        return 1;
    }

    // Создаем и очищаем структуру для обработки сигналов
    struct sigaction sig_settings;
    memset(&sig_settings, 0, sizeof(sig_settings));  // Обнуляем структуру

    // Указываем нашу функцию-обработчик
    sig_settings.sa_handler = handle_signal;

    // Очищаем маску сигналов (какие сигналы блокировать при обработке)
    sigemptyset(&sig_settings.sa_mask);

    // Без специальных флагов
    sig_settings.sa_flags = 0;

    // Устанавливаем обработчик для SIGINT
    if (sigaction(SIGINT, &sig_settings, NULL) == -1) {
        fprintf(stderr, "Ошибка установки обработчика SIGINT\n");
        fclose(file);
        return 1;
    }

    // Устанавливаем обработчик для SIGQUIT
    if (sigaction(SIGQUIT, &sig_settings, NULL) == -1) {
        fprintf(stderr, "Ошибка установки обработчика SIGQUIT\n");
        fclose(file);
        return 1;
    }

    printf("Программа запущена. PID: %d\n", getpid());
    printf("Нажмите Ctrl+C (SIGINT) 3 раза для выхода\n");
    printf("Или Ctrl+\\ (SIGQUIT) для записи сообщения\n");
    printf("Данные записываются в: %s\n\n", filename);

    while (1) {
        // Проверяем, не пора ли выйти
        if (should_exit) {
            printf("\nПолучено 3 сигнала SIGINT. Завершение...\n");
            break;
        }

        // Если был сигнал, обрабатываем его
        if (got_signal) {
            // Определяем название сигнала
            const char* signal_name;
            if (last_signal == SIGINT) {
                signal_name = "SIGINT";
            }
            else {
                signal_name = "SIGQUIT";
            }

            // Записываем в файл
            fprintf(file, "Обработан сигнал: %s (SIGINT всего: %d)\n",
                signal_name, (int)sigint_counter);

            // Сбрасываем флаг
            got_signal = 0;
        }

        // Записываем текущее значение счетчика
        fprintf(file, "Счетчик: %d\n", counter);

        // Выводим на экран для информации
        printf("Записано: %d\n", counter);

        counter++;  // Увеличиваем счетчик

        // Ждем 1 секунду
        sleep(1);
    }

    // Закрываем файл
    if (fclose(file) == EOF) {
        fprintf(stderr, "Ошибка закрытия файла\n");
        return 1;
    }

    printf("Файл закрыт. Программа завершена.\n");
    return 0;
}