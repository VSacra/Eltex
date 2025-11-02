#define _CRT_SECURE_NO_WARNINGS
#include "Header2_3.h"
#include <stdio.h>
double* (*oper) (double, double);

int main() {
    while (1) {
        printf("\nДействия + / - * (Для выхода нажмите q)\n");
        double Num1, Num2;
        char c;

        c = getchar();
        if (c == 'q' || c == 'Q') {
            printf("Выход из программы.\n");
            break;
        }
        ungetc(c, stdin);

        if (scanf("%lf", &Num1) != 1) {
            printf("Ошибка ввода числа!\n");
            while (getchar() != '\n'); // Очистка буфера
            continue;
        }

        // Пропуск пробелов
        c = getchar();
        if (c != ' ' && c != '\n') ungetc(c, stdin);

        // Чтение операции (как строки)
        scanf("%c", &c);
        if (c == '+') oper = sum;
        else if (c == '-') oper = sub;
        else if (c == '*') oper = multi;
        else if (c == '/') oper = divide;
        else { printf("\nОшибка! Неверная операция!"); break; }

        // Пропуск пробелов
        c = getchar();
        if (c != ' ' && c != '\n') ungetc(c, stdin);

        // Чтение второго числа
        if (scanf("%lf", &Num2) != 1) {
            printf("Ошибка ввода числа!\n");
            while (getchar() != '\n');
            continue;
        }

        // Очистка буфера после ввода
        while ((c = getchar()) != '\n' && c != EOF);

        double* result = oper(Num1, Num2);
        if (result == NULL) {
            printf("\nОшибка! Деление на ноль!\n");
        }
        else {
            printf("Результат: %lf\n", *result);
        }
        free(result);
    }
    return 0;
}
