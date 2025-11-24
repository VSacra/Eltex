#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "PhB4_1.h"

int main() {
	int v = 0;
	int i = 1;

	FILE* file;
	file = fopen("PhB.txt", O_RDONLY | O_CREAT, 0644);
	if (file == NULL) {
		printf("\nНе удалось открыть файл.\n");
		return 1;
	}

	size_t bytes;

	char c;
	while ((c = getchar()) != EOF) {
		Contact* new = (Contact*)malloc(sizeof(Contact));
		bytes = read(file, new, sizeof(Contact));
		addContact(&new);
	}
	fclose(file);

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
			printf("\nВыход.\n");
			if (save() == -1) {
				ptintf("\nНе удалось открыть файл\n");
				return 1;
			}
			return 0;
		}
		}
	}
}


