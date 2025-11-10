#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "PhB6_1.h"

int main() {
	int v = 0;
	int i = 1;
	while (i) {
		printf("\nЧто хотите сделать?\n1. Добавить контакт.\n2. Редактировать контакт\n3. Удалить контакт\n4. Вывести список контактов\n");
		scanf("%d", &v);
		getchar();
		switch (v) {
		case 1: {
			addConsole();
			break;
		}
		case 2: {
			editConsole();
			break;
		}
		case 3: {
			deleteConsole();
			break;
		}
		case 4: {
			View();
			break;
		}
		default: {
			printf("\nОшибка.\n");
			return 0;
		}
		}
	}
}

