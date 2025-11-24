#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "PhB4_1.h"
#include <sys/types.h>
#include <sys/stat.h>

int main() {
	int v = 0;
	int i = 1;

	int file=0;
	file = open("PhB.txt", O_RDONLY | O_CREAT, 0644);
	if (file == -1) {
		printf("\nНе удалось открыть файл.\n");
		return 1;
	}

	size_t bytes;


	while (bytes != 0) {
		Contact* new = (Contact*)malloc(sizeof(Contact));
		bytes = read(file, new, sizeof(Contact));
		if (bytes==-1){
			printf("\nОшибка. Не удалось прочитать из файла.\n");
			return 1;
		}
		if (new->Imya==NULL)break;
		if(addContact(new)!=new){
			printf("\nОшибка добавления\n");
			return 1;
	}
}
	if(close(file)==-1) {
		printf("\nОшибка при закрытии файла\n");
		return 1;
	}

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
				printf("\nНе удалось открыть файл\n");
				return 1;
			}
			return 0;
		}
		}
	}
}


