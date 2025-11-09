#define _CRT_SECURE_NO_WARNINGS
#include "PhB4_1.h"
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

int N = 0;
Contact* head = NULL;

Contact* addContact(Contact* NewContact) {
	if (head == NULL) {
		head = NewContact;
		NewContact->next = NULL;
		NewContact->prev = NULL;
		N++; return head;
	}
	else {
		Contact* tmp = head;
		for (int i = 0; i < N; i++) {
			if (strlen(tmp->Numbers[0]) > strlen(NewContact->Numbers[0])) {
				NewContact->next = tmp;
				NewContact->prev = tmp->prev;
				if (tmp->prev != NULL) {
					tmp->prev->next = NewContact;
				}
				else {
					head = NewContact; // если вставка в начало
				}
				tmp->prev = NewContact;
				N++;
				AddID(NewContact);
				return NewContact;
			}
			else if (strlen(tmp->Numbers[0]) == strlen(NewContact->Numbers[0])) {
				bool inserted = false;
				for (int j = 0; j < strlen(NewContact->Numbers[0]); j++) {
					if (tmp->Numbers[0][j] > NewContact->Numbers[0][j]) {
						NewContact->next = tmp;
						NewContact->prev = tmp->prev;
						if (tmp->prev != NULL) {
							tmp->prev->next = NewContact;
						}
						else {
							head = NewContact; // вставка в начало
						}
						tmp->prev = NewContact;
						N++;
						AddID(NewContact);
						return NewContact;
					}
				}
			}
			if(tmp->next!=NULL) tmp = tmp->next;
			else {
				NewContact->next = NULL;
				NewContact->prev = tmp;
				tmp->next = NewContact;
				N++; AddID(NewContact);
				return NewContact;
			}
		}
	}
	return NULL;
}

Contact* editContact(int id, int k, char str[10], ...) {
	va_list factor;         //указатель va_list
	va_start(factor, str);    // устанавливаем указатель
	if (head == NULL) return NULL;
	Contact* tmp = head;
	for (int i = 0; i < N; i++) {
		if (tmp->ID != id) {
			if (tmp->next != NULL) tmp = tmp->next;
			else { va_end(factor); return NULL; }
		}
		else break;
	}

	for (int i = 0; i < k; i++){
		switch (str[i]) {
		case 'i': {
			char* im = va_arg(factor, char*);
			free(tmp->Imya);
			tmp->Imya = (char*)malloc(strlen(im) + 1);
			strcpy(tmp->Imya, im);
			break; }
		case 'f': {
			char* fa = va_arg(factor, char*);
			free(tmp->Familiya);
			tmp->Familiya = (char*)malloc(strlen(fa) + 1);
			strcpy(tmp->Familiya, fa);
			break; }
		case 'o': {
			char* ot = va_arg(factor, char*);
			if(tmp->Otchestvo)free(tmp->Otchestvo);
			tmp->Otchestvo = (char*)malloc(strlen(ot) + 1);
			strcpy(tmp->Otchestvo, ot);
			break; }
		case 'n': {
			int kNum = va_arg(factor, int);
			char* numArr = va_arg(factor, char*); // Указатель на первый элемент

			for (int j = 0; j < kNum && j < MAX_PHONES; j++) {
				strncpy(tmp->Numbers[j], numArr + j * MAX_STRING, MAX_STRING);
				tmp->Numbers[j][MAX_STRING - 1] = '\0';
			}
			for (int j = kNum; j < MAX_PHONES; j++) {
				tmp->Numbers[j][0] = '\0';
			}
			break;
		}
		case 's': {
			Net* soc = va_arg(factor, Net*);
			if (soc) {
				// Копируем соцсети
				strncpy(tmp->Soc.VK, soc->VK, MAX_STRING);
				strncpy(tmp->Soc.OK, soc->OK, MAX_STRING);
				strncpy(tmp->Soc.TG, soc->TG, MAX_STRING);

				tmp->Soc.VK[MAX_STRING - 1] = '\0';
				tmp->Soc.OK[MAX_STRING - 1] = '\0';
				tmp->Soc.TG[MAX_STRING - 1] = '\0';
			}
			break; }
		default: {va_end(factor); return NULL; }
		}
	}
	va_end(factor);
	return &tmp;
} 

int deleteContact(int id) {
	Contact* tmp = head;
	if (head == NULL) return NULL;
	for (int i = 0; i < N; i++) {
		if (tmp->ID != id) {
			if (tmp->next != NULL) tmp = tmp->next;
			else return -1;
		}
		else {
			if (tmp->prev != NULL) { tmp->prev->next = tmp->next; 
			if (tmp->next != NULL) tmp->next->prev = tmp->prev;
			free(tmp->Imya); free(tmp->Familiya); free(tmp->Otchestvo); free(tmp); 
			N--; return 0;
			}
			else {
				if (tmp->next != NULL) { head = tmp->next; head->prev = NULL; }
				else head = NULL;
				free(tmp->Imya); free(tmp->Familiya); free(tmp->Otchestvo); free(tmp); 
				N--; return 0;
			}
		}
	}
	return -1;
}

void AddID(Contact* new_id) {
	int id = 0; bool Deny = 1;
	Contact* tmp = head;
	while (Deny != 0) {
		id = rand ();
		for (int i = 0; i <= N; i++) {
			if (tmp->ID == id) { Deny = 1; break; }
			else {
				if (tmp->next != NULL) tmp = tmp->next;
				else Deny = 0;
			}
		}
	}
	new_id->ID = id;
}

char* ReadStr() {
	char buffer[256];

	// Пропускаем \n если он первый в буфере
	int first_char = getchar();
	if (first_char == '\n') {
		// Если первый символ - \n, пропускаем его и читаем заново
		if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
			buffer[strcspn(buffer, "\n")] = '\0';

			if (strlen(buffer) > 0) {
				char* result = (char*)malloc(strlen(buffer) + 1);
				strcpy(result, buffer);
				return result;
			}
		}
	}
	else {
		// Если первый символ не \n, возвращаем его и читаем
		ungetc(first_char, stdin);
		if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
			buffer[strcspn(buffer, "\n")] = '\0';

			if (strlen(buffer) > 0) {
				char* result = (char*)malloc(strlen(buffer) + 1);
				strcpy(result, buffer);
				return result;
			}
		}
	}

	return NULL;
}

void addConsole() {
	Contact newContact = { 0 }; // Инициализируем всю структуру нулями

	// Имя
	printf("\nВведите имя контакта: ");
	char* str = ReadStr();
	if (str == NULL) {
		printf("Ошибка чтения имени!\n");
		return;
	}
	newContact.Imya = str;

	// Фамилия
	printf("\nВведите фамилию контакта: ");
	char* str2 = ReadStr();
	if (str2 == NULL) {
		printf("Ошибка чтения фамилии!\n");
		free(newContact.Imya); // Освобождаем ранее выделенную память
		return;
	}
	newContact.Familiya = str2;

	// Отчество
	printf("\nХотите ввести отчество? 1. Да 2. Нет\n");
	int v;
	scanf("%d", &v);
	getchar();
	if (v == 1) {
		printf("\nВведите отчество: ");
		char* str3 = ReadStr();
		if (str3 == NULL) {
			printf("Ошибка чтения отчества!\n");
		}
		else {
			newContact.Otchestvo = str3;
		}
	}
	else if (v != 2) {
		printf("Ошибка! Отчество не будет записано.\n");
	}


	// Номера телефонов
	printf("\nХотите ввести номер телефона? 1. Да 2. Нет\n");
	scanf("%d", &v); 
	getchar();
	int kNum = 0;
	if (v == 1) {
		printf("Сколько номеров хотите ввести? ");
		scanf("%d", &kNum);
		if (kNum > MAX_PHONES) {
			printf("\nОшибка. Превышено допустимое количество номеров, будет записано %d номеров.\n", MAX_PHONES);
			kNum = MAX_PHONES;
		}

		getchar();
		printf("\nВведите номер(а):\n");

		for (int i = 0; i < kNum; i++) {
			printf("Номер %d: ", i + 1);
			fgets(newContact.Numbers[i], MAX_STRING, stdin);
			newContact.Numbers[i][strcspn(newContact.Numbers[i], "\n")] = '\0';

			// ПРОВЕРКА НА ОБРЕЗАНИЕ
			if (strlen(newContact.Numbers[i]) == MAX_STRING - 1) {
				printf("Предупреждение: номер слишком длинный, обрезан до %d символов.\n", MAX_STRING - 1);
				// Очищаем остаток буфера
				int c;
				while ((c = getchar()) != '\n' && c != EOF);
			}
		}
	}
	else if (v != 2) {
		printf("Ошибка! Номер не будет записан.\n");
	}

	// Соцсети
	printf("\nХотите ввести адреса соцсетей? 1. Да 2. Нет\n");
	scanf("%d", &v);
	getchar();
	if (v == 1) {
		// VK
		printf("\nХотите ввести адрес VK? 1. Да 2. Нет: ");
		scanf("%d", &v);
		getchar();
		if (v == 1) {
			printf("Введите адрес VK: ");
			fgets(newContact.Soc.VK, MAX_STRING, stdin);
			if (strlen(newContact.Soc.VK) == MAX_STRING - 1) {
				printf("Предупреждение: адрес VK слишком длинный, обрезан до %d символов.\n", MAX_STRING - 1);
				int c;
				while ((c = getchar()) != '\n' && c != EOF);
			}
			newContact.Soc.VK[strcspn(newContact.Soc.VK, "\n")] = '\0';
		}
		else if (v != 2) {
			printf("Ошибка! VK не будет записан.\n");
		}

		// OK
		printf("\nХотите ввести адрес OK? 1. Да 2. Нет: ");
		scanf("%d", &v);
		getchar();
		if (v == 1) {
			printf("Введите адрес OK: ");
			fgets(newContact.Soc.OK, MAX_STRING, stdin);
			if (strlen(newContact.Soc.OK) == MAX_STRING - 1) {
				printf("Предупреждение: адрес OK слишком длинный, обрезан до %d символов.\n", MAX_STRING - 1);
				int c;
				while ((c = getchar()) != '\n' && c != EOF);
			}
			newContact.Soc.OK[strcspn(newContact.Soc.OK, "\n")] = '\0';
		}
		else if (v != 2) {
			printf("Ошибка! OK не будет записан.\n");
		}

		// TG
		printf("\nХотите ввести адрес TG? 1. Да 2. Нет: ");
		scanf("%d", &v);
		getchar();
		if (v == 1) {
			printf("Введите адрес TG: ");
			fgets(newContact.Soc.TG, MAX_STRING, stdin);
			if (strlen(newContact.Soc.TG) == MAX_STRING - 1) {
				printf("Предупреждение: адрес TG слишком длинный, обрезан до %d символов.\n", MAX_STRING - 1);
				int c;
				while ((c = getchar()) != '\n' && c != EOF);
			}
			newContact.Soc.TG[strcspn(newContact.Soc.TG, "\n")] = '\0';
		}
		else if (v != 2) {
			printf("Ошибка! TG не будет записан.\n");
		}
	}
	else if (v != 2) {
		printf("Ошибка! Соцсети не будут записаны.\n");
	}

	// Добавление контакта
	if (addContact(&newContact)==NULL) {
		printf("\nОшибка. Не удалось записать контакт\n");
	}
	else {
		printf("\nКонтакт успешно добавлен.\n");
	}

}

void editConsole() {
	View();
	if (head == NULL) return;
	int Num, id;
	int des = 0, k=0;
	char str[10] ="";
	char* Imya = NULL, * Fam = NULL, * Otch = NULL;
	int kNum = 0;
	char numArr[MAX_PHONES][MAX_STRING] = { 0 };
	int hasSocial = 0;
	printf("\nВведите номер контакта, который вы хотите изменить: ");
	scanf("%d",&Num);
	int c;
	while ((c = getchar()) != '\n' && c != EOF) {}
	Contact* tmp = head;
	for (int i = 0; i <= Num; i++) {
		tmp = tmp->next;
	}
	if (tmp->ID == -1) printf("\nОшибка. Нет такого номера.");
	Net Soc = tmp->Soc;
	id = tmp->ID;
	printf("\nХотите изменить имя? 1. Да 2. Нет\n");
	scanf("%d", &des);
	while ((c = getchar()) != '\n' && c != EOF) {}
	if (des == 1) {
		printf("Введите новое имя: ");
		Imya = ReadStr();
		strcat(str, "i");
        k++;
	}
	if (des != 1 && des != 2)printf("Ошибка. Имя не будет изменено.\n"); 
	des = 0;
	printf("\nХотите изменить фамилию? 1. Да 2. Нет\n");
	scanf("%d", &des);
	if (des == 1) {
		printf("Введите новую фамилию: ");
		Fam = ReadStr();
		strcat(str, "f"); k++;
	}
	if (des != 1 && des != 2)printf("Ошибка. Фамилия не будет изменена.\n"); 
	des = 0;
	printf("\nХотите изменить отчество? 1. Да 2. Нет\n");
	scanf("%d", &des);
	if (des == 1) {
		printf("Введите новое отчество: ");
		Otch = ReadStr();
		strcat(str, "o"); k++;
	}
	if (des != 1 && des != 2)printf("Ошибка. Отчество не будет изменено.\n"); 
	des = 0;
	printf("\nХотите изменить список номеров? 1. Да 2. Нет\n");
	scanf("%d", &des);
	if (des == 1) {
		printf("Сколько номеров хотите ввести? ");
		scanf("%d", &kNum);

		if (kNum > MAX_PHONES) {
			printf("\nОшибка. Превышено допустимое количество номеров, будет записано %d номеров.\n", MAX_PHONES);
			kNum = MAX_PHONES;
		}

		getchar(); // очистка буфера
		printf("\nВведите номер(а):\n");

		for (int i = 0; i < kNum; i++) {
			printf("Номер %d: ", i + 1);
			fgets(numArr[i], MAX_STRING, stdin);

			// Удаляем символ новой строки
			numArr[i][strcspn(numArr[i], "\n")] = '\0';

			// Проверяем длину
			if (strlen(numArr[i]) == MAX_STRING-1) {
				printf("Предупреждение: номер слишком длинный, обрезан.\n");
				int c;
				while ((c = getchar()) != '\n' && c != EOF) {}
			}
		}
		strcat(str, "n");
		k++;
	}
	if (des != 1 && des != 2)printf("Ошибка. Список номеров не будет изменён.\n"); 
	des = 0;
	printf("\nХотите изменить список адресов соцсетей? 1. Да 2. Нет\n");
	scanf("%d", &des);
	if (des == 1) {
		int v = 0;
		printf("\nХотите ввести адрес VK? 1. Да 2. Нет: ");
		scanf("%d", &v);
		getchar(); // очистка буфера после scanf

		if (v == 1) {
			printf("Введите адрес VK: ");
			fgets(Soc.VK, MAX_STRING, stdin);
			Soc.VK[strcspn(Soc.VK, "\n")] = '\0';

			// ПРОВЕРКА НА ОБРЕЗАНИЕ
			if (strlen(Soc.VK) == MAX_STRING - 1) {
				printf("Предупреждение: адрес VK слишком длинный, обрезан до %d символов.\n", MAX_STRING - 1);
				// Очищаем остаток буфера
				int c;
				while ((c = getchar()) != '\n' && c != EOF);
			}
			hasSocial = 1;
		}
		else if (v != 2) {
			printf("Ошибка! VK не будет записан.\n");
		}

		printf("\nХотите ввести адрес OK? 1. Да 2. Нет: ");
		scanf("%d", &v);
		getchar(); // очистка буфера после scanf

		if (v == 1) {
			printf("Введите адрес OK: ");
			fgets(Soc.OK, MAX_STRING, stdin);
			Soc.OK[strcspn(Soc.OK, "\n")] = '\0';

			// ПРОВЕРКА НА ОБРЕЗАНИЕ
			if (strlen(Soc.OK) == MAX_STRING - 1) {
				printf("Предупреждение: адрес OK слишком длинный, обрезан до %d символов.\n", MAX_STRING - 1);
				int c;
				while ((c = getchar()) != '\n' && c != EOF);
			}
			hasSocial = 1;
		}
		else if (v != 2) {
			printf("Ошибка! OK не будет записан.\n");
		}

		printf("\nХотите ввести адрес TG? 1. Да 2. Нет: ");
		scanf("%d", &v);
		getchar(); // очистка буфера после scanf

		if (v == 1) {
			printf("Введите адрес TG: ");
			fgets(Soc.TG, MAX_STRING, stdin);
			Soc.TG[strcspn(Soc.TG, "\n")] = '\0';

			// ПРОВЕРКА НА ОБРЕЗАНИЕ
			if (strlen(Soc.TG) == MAX_STRING - 1) {
				printf("Предупреждение: адрес TG слишком длинный, обрезан до %d символов.\n", MAX_STRING - 1);
				int c;
				while ((c = getchar()) != '\n' && c != EOF);
			}
			hasSocial = 1;
		}
		else if (v != 2) {
			printf("Ошибка! TG не будет записан.\n");
		}
		if (hasSocial) {
			strcat(str, "s");
			k++;
		}
	}
	else if (des != 2) {
		printf("Ошибка. Список адресов соцсетей не будет изменён.\n");
	}
	if (k > 0) {
		Contact* result=NULL;

		// Определяем какие параметры передавать на основе содержимого str
		if (strcmp(str, "i") == 0) {
			result = editContact(id, k, str, Imya);
		}
		else if (strcmp(str, "f") == 0) {
			result = editContact(id, k, str, Fam);
		}
		else if (strcmp(str, "o") == 0) {
			result = editContact(id, k, str, Otch);
		}
		else if (strcmp(str, "n") == 0) {
			result = editContact(id, k, str, kNum, (char*)numArr);
		}
		else if (strcmp(str, "s") == 0) {
			result = editContact(id, k, str, hasSocial ? &Soc : NULL);
		}
		else if (strcmp(str, "if") == 0) {
			result = editContact(id, k, str, Imya, Fam);
		}
		else if (strcmp(str, "io") == 0) {
			result = editContact(id, k, str, Imya, Otch);
		}
		else if (strcmp(str, "in") == 0) {
			result = editContact(id, k, str, Imya, kNum, (char*)numArr);
		}
		else if (strcmp(str, "is") == 0) {
			result = editContact(id, k, str, Imya, hasSocial ? &Soc : NULL);
		}
		else if (strcmp(str, "fo") == 0) {
			result = editContact(id, k, str, Fam, Otch);
		}
		else if (strcmp(str, "fn") == 0) {
			result = editContact(id, k, str, Fam, kNum, (char*)numArr);
		}
		else if (strcmp(str, "fs") == 0) {
			result = editContact(id, k, str, Fam, hasSocial ? &Soc : NULL);
		}
		else if (strcmp(str, "on") == 0) {
			result = editContact(id, k, str, Otch, kNum, (char*)numArr);
		}
		else if (strcmp(str, "os") == 0) {
			result = editContact(id, k, str, Otch, hasSocial ? &Soc : NULL);
		}
		else if (strcmp(str, "ns") == 0) {
			result = editContact(id, k, str, kNum, (char*)numArr, hasSocial ? &Soc : NULL);
		}
		else if (strcmp(str, "ifo") == 0) {
			result = editContact(id, k, str, Imya, Fam, Otch);
		}
		else if (strcmp(str, "ifn") == 0) {
			result = editContact(id, k, str, Imya, Fam, kNum, (char*)numArr);
		}
		else if (strcmp(str, "ifs") == 0) {
			result = editContact(id, k, str, Imya, Fam, hasSocial ? &Soc : NULL);
		}
		else if (strcmp(str, "ion") == 0) {
			result = editContact(id, k, str, Imya, Otch, kNum, (char*)numArr);
		}
		else if (strcmp(str, "ios") == 0) {
			result = editContact(id, k, str, Imya, Otch, hasSocial ? &Soc : NULL);
		}
		else if (strcmp(str, "fon") == 0) {
			result = editContact(id, k, str, Fam, Otch, kNum, (char*)numArr);
		}
		else if (strcmp(str, "fos") == 0) {
			result = editContact(id, k, str, Fam, Otch, hasSocial ? &Soc : NULL);
		}
		else if (strcmp(str, "ins") == 0) {
			result = editContact(id, k, str, Imya, kNum, (char*)numArr, hasSocial ? &Soc : NULL);
		}
		else if (strcmp(str, "fns") == 0) {
			result = editContact(id, k, str, Fam, kNum, (char*)numArr, hasSocial ? &Soc : NULL);
		}
		else if (strcmp(str, "ons") == 0) {
			result = editContact(id, k, str, Otch, kNum, (char*)numArr, hasSocial ? &Soc : NULL);
		}
		else if (strcmp(str, "ifon") == 0) {
			result = editContact(id, k, str, Imya, Fam, Otch, kNum, (char*)numArr);
		}
		else if (strcmp(str, "ifos") == 0) {
			result = editContact(id, k, str, Imya, Fam, Otch, hasSocial ? &Soc : NULL);
		}
		else if (strcmp(str, "ifns") == 0) {
			result = editContact(id, k, str, Imya, Fam, kNum, (char*)numArr, hasSocial ? &Soc : NULL);
		}
		else if (strcmp(str, "ions") == 0) {
			result = editContact(id, k, str, Imya, Otch, kNum, (char*)numArr, hasSocial ? &Soc : NULL);
		}
		else if (strcmp(str, "fons") == 0) {
			result = editContact(id, k, str, Fam, Otch, kNum, (char*)numArr, hasSocial ? &Soc : NULL);
		}
		else if (strcmp(str, "ifons") == 0) {
			result = editContact(id, k, str, Imya, Fam, Otch, kNum, (char*)numArr, hasSocial ? &Soc : NULL);
		}
		else {
			printf("Ошибка: неизвестная комбинация флагов '%s'\n", str);
			return;
		}

		if (result==tmp) {
			printf("\nКонтакт изменён.\n");
		}
		else {
			printf("\nОшибка. Контакт не был изменён.\n");
		}
	}
}

void deleteConsole() {
	View();
	if (head == NULL) return;
	Contact* tmp = head;
	printf("\nВведите номер контакта, который хотите удалить: ");
	int num; scanf("%d",  &num);
	if (num<1 || num>N+1) { printf("\nОшибка. Такого номера не сущесвует\n"); return; }
	for (int i = 0; i < num-1; i++) tmp = tmp->next;
	if (deleteContact(tmp->ID) == 0) printf("\nКонтакт был успешно удалён\n");
	else printf("\nОшибка. Не удалось удалить контакт\n");
}

void View() {
	printf("Список контактов: \n");

	if (head==NULL) {
		printf("Телефонная книга пуста.\n");
		return;
	}

	Contact* tmp = head;

	for (int i = 0; i < N; i++) {
		printf("\n%d. ", i + 1);

		// ФИО с пробелами
		printf("%s ", tmp->Imya);
	    printf("%s ", tmp->Familiya);
		if (tmp->Otchestvo && tmp->Otchestvo[0] != '\0') printf("%s", tmp->Otchestvo);
		printf("\n");

		// Номера телефонов (только непустые)
		printf("Телефоны: ");
		int hasPhones = 0;
		for (int j = 0; j < MAX_PHONES; j++) {
			if (tmp->Numbers[j][0] != '\0') { // Проверяем что номер не пустой
				if (hasPhones) printf(", ");
				printf("%s", tmp->Numbers[j]); // Выводим строку целиком
				hasPhones = 1;
			}
		}
		printf("\n");

		// Соцсети (только непустые)
		printf("Соцсети: ");
		int hasSocial = 0;
		if (tmp->Soc.VK[0] != '\0') {
			printf("VK: %s", tmp->Soc.VK);
			hasSocial = 1;
		}
		if (tmp->Soc.OK[0] != '\0') {
			if (hasSocial) printf(", ");
			printf("OK: %s", tmp->Soc.OK);
			hasSocial = 1;
		}
		if (tmp->Soc.TG[0] != '\0') {
			if (hasSocial) printf(", ");
			printf("TG: %s", tmp->Soc.TG);
			hasSocial = 1;
		}

		printf("\nID: %d\n", tmp->ID);
		if (tmp->next != NULL) tmp = tmp->next;
	}
}
