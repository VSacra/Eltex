#define _CRT_SECURE_NO_WARNINGS
#include "Queue.h"
#include <stdio.h>

int main() {
	while (1) {
		printf("\nДля выхода введите q\n1. Сгенерировать элементы\n2. Добавить элемент вручную\n3. Извлечь первый элемент\n4. Извлечь элемент с заданным приоритетом\n5. Извлечь элемент с приоритетом с заданным приоритетом или ниже\n6. Вывести очередь\n");
		char choice;
		scanf(" %c",&choice);
		if (choice == 'q') return 0;
		else Console(choice-'0');
	}
}
