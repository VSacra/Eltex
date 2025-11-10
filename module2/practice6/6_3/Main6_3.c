#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <dlfcn.h>

// Структура для хранения загруженной функции
typedef struct {
    char symbol;        // Символ операции: '+', '-', '*', '/'
    char* lib_name;     // Имя библиотеки
    void* handle;       // Handle библиотеки
    double* (*func)(double, double); // Указатель на функцию
} Operation;

Operation operations[10];
int op_count = 0;

// Функция для загрузки библиотек из каталога
void load_libraries(const char* dir_path) {
    DIR* dir = opendir(dir_path);
    if (!dir) {
        printf("Ошибка: не могу открыть каталог %s\n", dir_path);
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        // Ищем файлы .so
        if (strstr(entry->d_name, ".so")) {
            char lib_path[256];
            snprintf(lib_path, sizeof(lib_path), "%s/%s", dir_path, entry->d_name);

            // Загружаем библиотеку
            void* handle = dlopen(lib_path, RTLD_LAZY);
            if (!handle) {
                printf("Ошибка загрузки %s: %s\n", lib_path, dlerror());
                continue;
            }

            // Определяем тип операции по имени файла
            char symbol = '?';
            if (strstr(entry->d_name, "sum")) symbol = '+';
            else if (strstr(entry->d_name, "sub")) symbol = '-';
            else if (strstr(entry->d_name, "multi")) symbol = '*';
            else if (strstr(entry->d_name, "divide")) symbol = '/';

            if (symbol != '?') {
                // Загружаем функцию
                double* (*func)(double, double) = dlsym(handle,
                    (symbol == '+') ? "sum" :
                    (symbol == '-') ? "sub" :
                    (symbol == '*') ? "multi" : "divide");

                if (func) {
                    operations[op_count].symbol = symbol;
                    operations[op_count].lib_name = strdup(entry->d_name);
                    operations[op_count].handle = handle;
                    operations[op_count].func = func;
                    op_count++;
                    printf("Загружена операция: %c из %s\n", symbol, entry->d_name);
                }
                else {
                    printf("Ошибка загрузки функции из %s: %s\n", entry->d_name, dlerror());
                    dlclose(handle);
                }
            }
            else {
                dlclose(handle);
            }
        }
    }
    closedir(dir);
}

// Функция для поиска операции по символу
double* (*find_operation(char symbol))(double, double) {
    for (int i = 0; i < op_count; i++) {
        if (operations[i].symbol == symbol) {
            return operations[i].func;
        }
    }
    return NULL;
}

// Освобождение ресурсов
void cleanup_operations() {
    for (int i = 0; i < op_count; i++) {
        free(operations[i].lib_name);
        dlclose(operations[i].handle);
    }
}

int main() {
    // Загружаем библиотеки из каталога 'libs'
    load_libraries("libs");

    if (op_count == 0) {
        printf("Не загружено ни одной операции!\n");
        return 1;
    }

    // Выводим доступные операции
    printf("\nДоступные операции: ");
    for (int i = 0; i < op_count; i++) {
        printf("%c ", operations[i].symbol);
    }
    printf("(Для выхода нажмите q)\n");

    while (1) {
        double Num1, Num2;
        char c;

        printf("Введите выражение: ");

        // Чтение первого числа
        if (scanf("%lf", &Num1) != 1) {
            c = getchar();
            if (c == 'q' || c == 'Q') {
                printf("Выход из программы.\n");
                break;
            }
            printf("Ошибка ввода числа!\n");
            while (getchar() != '\n'); // Очистка буфера
            continue;
        }

        // Пропуск пробелов и чтение операции
        while ((c = getchar()) == ' ');
        if (c == '\n') continue;

        // Поиск функции для операции
        double* (*oper)(double, double) = find_operation(c);
        if (!oper) {
            printf("Неизвестная операция: %c\n", c);
            while (getchar() != '\n');
            continue;
        }

        // Чтение второго числа
        if (scanf("%lf", &Num2) != 1) {
            printf("Ошибка ввода второго числа!\n");
            while (getchar() != '\n');
            continue;
        }

        // Очистка буфера
        while ((c = getchar()) != '\n' && c != EOF);

        // Выполнение операции
        double* result = oper(Num1, Num2);
        if (result == NULL) {
            printf("Ошибка вычисления (Деление на ноль)\n");
        }
        else {
            printf("Результат: %lf\n", *result);
            free(result);
        }
    }

    cleanup_operations();
    return 0;
}
