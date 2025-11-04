#define _CRT_SECURE_NO_WARNINGS
#include "chmod3_1.h"
#include <stdio.h>

int main() {
	while (1) {
		char choice = ' ';
		printf("\nЧто хотите сделать?\nТестовые комманды:\n1. Узнать битовое представление маски\n2. Тестовые chmod без файла\nРабота с файлом:\n3. Узнать права доступа файла\n4. Изменить права доступа файла\nДля выходы введите q\nВаш выбор: ");
		scanf("%c",&choice);
		switch (choice) {
		case 'q': return 0;
		case '1': {
			ToByteConsole();
			break;
		}
		case '2': {
			TestChmod();
			break;
		}
		case '3': {
			printPer();
			break;
		}
		case '4': {
			FileChmod();
			break;
		}
		default: printf("\nНеверный ввод");
		}
	} 
	return 0;
}
