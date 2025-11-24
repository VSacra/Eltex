#define _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS
#include "PhB4.h"
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

int N = 0;
Contact* head = NULL;

Contact* addContact(Contact* NewContact) {
	// Генерируем уникальный ID
	AddID(NewContact);

	// Инициализируем высоту нового узла
	NewContact->height = 1;
	NewContact->left = NULL;
	NewContact->right = NULL;
	NewContact->parent = NULL;

	if (head == NULL) {
		head = NewContact;
		NewContact->level = 0;
		N++;
		return head;
	}

	head = insert(head, NewContact);
	N++;
	return NewContact;
}

Contact* insert(Contact* node, Contact* NewContact) {
	if (node == NULL) {
		NewContact->level = 0; 
		return NewContact;
	}

	if (NewContact->ID < node->ID) {
		node->left = insert(node->left, NewContact);
		node->left->parent = node;
	}
	else if (NewContact->ID > node->ID) {
		node->right = insert(node->right, NewContact);
		node->right->parent = node;
	}
	else {
		// Если ID совпадают, идем влево
		node->left = insert(node->left, NewContact);
		node->left->parent = node;
	}

	// Обновляем высоту текущего узла
	node->height = 1 + max(getHeight(node->left), getHeight(node->right));

	int balance = getBalance(node);

	// Балансировка

	// Левый-левый случай
	if (balance > 1 && NewContact->ID < node->left->ID) {
		return rightRotate(node);
	}

	// Правый-правый случай
	if (balance < -1 && NewContact->ID > node->right->ID) {
		return leftRotate(node);
	}

	// Левый-правый случай
	if (balance > 1 && NewContact->ID > node->left->ID) {
		node->left = leftRotate(node->left);
		return rightRotate(node);
	}

	// Правый-левый случай
	if (balance < -1 && NewContact->ID < node->right->ID) {
		node->right = rightRotate(node->right);
		return leftRotate(node);
	}

	return node;
}

Contact* editContact(int id, int k, char str[10], ...) {
	va_list factor;         //указатель va_list
	va_start(factor, str);    // устанавливаем указатель
	if (head == NULL) return NULL;
	Contact* tmp = head;

	while(tmp != NULL) {
		if (id == tmp->ID) break;
		else if (id > tmp->ID) tmp = tmp->right;
		else tmp = tmp->left;
	}

	for (int i = 0; i < k; i++) {
		switch (str[i]) {
		case 'i': {
			char* im = va_arg(factor, char*);
			free(tmp->Imya);
			tmp->Imya = (char*)malloc(strlen(im) + 1);
			strcpy(tmp->Imya, im);
			break;
		}
		case 'f': {
			char* fa = va_arg(factor, char*);
			free(tmp->Familiya);
			tmp->Familiya = (char*)malloc(strlen(fa) + 1);
			strcpy(tmp->Familiya, fa);
			break;
		}
		case 'o': {
			char* ot = va_arg(factor, char*);
			if (tmp->Otchestvo)free(tmp->Otchestvo);
			tmp->Otchestvo = (char*)malloc(strlen(ot) + 1);
			strcpy(tmp->Otchestvo, ot);
			break;
		}
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
			break;
		}
		default: { va_end(factor); return NULL; }
		}
	}
	va_end(factor);
	return tmp;
}

int deleteContact(int id) {
	if (head == NULL) return -1;

	head = del(head, id);
	N--;
	return 0;
}

Contact* del(Contact* root, int id) {
	if (root == NULL) return root;

	if (id < root->ID) {
		root->left = del(root->left, id);
	}
	else if (id > root->ID) {
		root->right = del(root->right, id);
	}
	else {
		// Найден узел для удаления
		if ((root->left == NULL) || (root->right == NULL)) {
			Contact* temp = root->left ? root->left : root->right;

			// Нет детей
			if (temp == NULL) {
				temp = root;
				root = NULL;
			}
			else {
				// Один ребенок
				*root = *temp; // Копируем содержимое
			}

			// Освобождаем память
			if (temp) {
				free(temp->Imya);
				free(temp->Familiya);
				free(temp->Otchestvo);
				free(temp);
			}
		}
		else {
			// Два ребенка
			Contact* temp = minValueNode(root->right);

			// Копируем данные
			root->ID = temp->ID;

			// Освобождаем старые строки и выделяем новую память
			free(root->Imya);
			root->Imya = malloc(strlen(temp->Imya) + 1);
			if (root->Imya) strcpy(root->Imya, temp->Imya);

			free(root->Familiya);
			root->Familiya = malloc(strlen(temp->Familiya) + 1);
			if (root->Familiya) strcpy(root->Familiya, temp->Familiya);

			free(root->Otchestvo);
			if (temp->Otchestvo) {
				root->Otchestvo = malloc(strlen(temp->Otchestvo) + 1);
				if (root->Otchestvo) strcpy(root->Otchestvo, temp->Otchestvo);
			}
			else {
				root->Otchestvo = NULL;
			}

			// Копируем номера телефонов
			for (int j = 0; j < MAX_PHONES; j++) {
				strcpy(root->Numbers[j], temp->Numbers[j]);
			}

			// Копируем соцсети
			strcpy(root->Soc.VK, temp->Soc.VK);
			strcpy(root->Soc.OK, temp->Soc.OK);
			strcpy(root->Soc.TG, temp->Soc.TG);

			// Восстанавливаем указатели 
			root->left = root->left;
			root->right = root->right;
			root->parent = root->parent;
			root->height = root->height;

			// Удаляем преемника
			root->right = del(root->right, temp->ID);
		}
	}

	if (root == NULL) return root;

	// Обновляем высоту
	root->height = 1 + max(getHeight(root->left), getHeight(root->right));

	int balance = getBalance(root);

	// Балансировка

	// Левый-левый
	if (balance > 1 && getBalance(root->left) >= 0) {
		return rightRotate(root);
	}

	// Левый-правый
	if (balance > 1 && getBalance(root->left) < 0) {
		root->left = leftRotate(root->left);
		return rightRotate(root);
	}

	// Правый-правый
	if (balance < -1 && getBalance(root->right) <= 0) {
		return leftRotate(root);
	}

	// Правый-левый
	if (balance < -1 && getBalance(root->right) > 0) {
		root->right = rightRotate(root->right);
		return leftRotate(root);
	}

	return root;
}

Contact* minValueNode(Contact* node) {
	Contact* current = node;
	while (current->left != NULL) {
		current = current->left;
	}
	return current;
}

void AddID(Contact* new_id) {
	int id = 0;
	bool exists = true;

	while (exists) {
		id = rand();
		exists = false;

		Contact* tmp = head;
		while (tmp != NULL && !exists) {
			if (tmp->ID == id) {
				exists = true;
				break;
			}
			else if (id < tmp->ID) {
				tmp = tmp->left;
			}
			else {
				tmp = tmp->right;
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
	Contact* newContact = (Contact*)malloc(sizeof(Contact));
	if (!newContact) {
		printf("Ошибка выделения памяти!\n");
		return;
	}
	memset(newContact, 0, sizeof(Contact));
	newContact->Imya = NULL;
	newContact->Familiya = NULL;
	newContact->Otchestvo = NULL;
	newContact->left = NULL;
	newContact->right = NULL;
	newContact->parent = NULL;

	// Имя
	printf("\nВведите имя контакта: ");
	char* str = ReadStr();
	if (str == NULL) {
		printf("Ошибка чтения имени!\n");
		return;
	}
	newContact->Imya = str;

	// Фамилия
	printf("\nВведите фамилию контакта: ");
	char* str2 = ReadStr();
	if (str2 == NULL) {
		printf("Ошибка чтения фамилии!\n");
		free(newContact->Imya); // Освобождаем ранее выделенную память
		free(newContact);
		return;
	}
	newContact->Familiya = str2;

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
			newContact->Otchestvo = str3;
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
			fgets(newContact->Numbers[i], MAX_STRING, stdin);
			newContact->Numbers[i][strcspn(newContact->Numbers[i], "\n")] = '\0';

			// ПРОВЕРКА НА ОБРЕЗАНИЕ
			if (strlen(newContact->Numbers[i]) == MAX_STRING - 1) {
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
			fgets(newContact->Soc.VK, MAX_STRING, stdin);
			if (strlen(newContact->Soc.VK) == MAX_STRING - 1) {
				printf("Предупреждение: адрес VK слишком длинный, обрезан до %d символов.\n", MAX_STRING - 1);
				int c;
				while ((c = getchar()) != '\n' && c != EOF);
			}
			newContact->Soc.VK[strcspn(newContact->Soc.VK, "\n")] = '\0';
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
			fgets(newContact->Soc.OK, MAX_STRING, stdin);
			if (strlen(newContact->Soc.OK) == MAX_STRING - 1) {
				printf("Предупреждение: адрес OK слишком длинный, обрезан до %d символов.\n", MAX_STRING - 1);
				int c;
				while ((c = getchar()) != '\n' && c != EOF);
			}
			newContact->Soc.OK[strcspn(newContact->Soc.OK, "\n")] = '\0';
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
			fgets(newContact->Soc.TG, MAX_STRING, stdin);
			if (strlen(newContact->Soc.TG) == MAX_STRING - 1) {
				printf("Предупреждение: адрес TG слишком длинный, обрезан до %d символов.\n", MAX_STRING - 1);
				int c;
				while ((c = getchar()) != '\n' && c != EOF);
			}
			newContact->Soc.TG[strcspn(newContact->Soc.TG, "\n")] = '\0';
		}
		else if (v != 2) {
			printf("Ошибка! TG не будет записан.\n");
		}
	}
	else if (v != 2) {
		printf("Ошибка! Соцсети не будут записаны.\n");
	}

	// Добавление контакта
	if (addContact(newContact) == NULL) {
		printf("\nОшибка. Не удалось записать контакт\n");
	}
	else {
		printf("\nКонтакт успешно добавлен.\n");
	}

}

void editConsole() {
	if (head == NULL) return;
	int des = 0, k = 0;
	char str[10] = "";
	char* Imya = NULL, * Fam = NULL, * Otch = NULL;
	int kNum = 0;
	char numArr[MAX_PHONES][MAX_STRING] = { 0 };
	int hasSocial = 0;
	if (head == NULL) {
		printf("Справочник пуст\n");
		return;
	}
	printf("\nВведите id контакта, который вы хотите изменить: ");

	View();

	int id;
	scanf("%d", &id);

	Contact* tmp = head;
	while (tmp != NULL) {
		if (id == tmp->ID) break;
		else if (id > tmp->ID) tmp = tmp->right;
		else tmp = tmp->left;
	}


	char c;
	Net Soc = tmp->Soc;
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
			if (strlen(numArr[i]) == MAX_STRING - 1) {
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
		Contact* result = NULL;

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

		if (result == tmp) {
			printf("\nКонтакт изменён.\n");
		}
		else {
			printf("\nОшибка. Контакт не был изменён.\n");
		}
	}
}

void deleteConsole() {
	if (head == NULL) {
		printf("Справочник пуст\n");
		return;
	}

	View();

	printf("\nВведите ID контакта для удаления: ");
	int id;
	scanf("%d", &id);

	if (deleteContact(id) == 0) {
		printf("Контакт успешно удалён\n");
	}
	else {
		printf("Ошибка. Контакт с ID %d не найден\n", id);
	}
}

void printTree(Contact* node, int level, char* prefix) {
    if (node == NULL) return;
  
    // Выводим правого ребенка
    char newPrefix[256];
    snprintf(newPrefix, sizeof(newPrefix), "%s    ", prefix);
    printTree(node->right, level + 1, newPrefix);
    
    // Выводим текущий узел
    printf("%s", prefix);
    printf("├── ");
    printf("[ID: %d] %s %s\n", node->ID, node->Familiya, node->Imya);
        
    // Выводим левого ребенка
    snprintf(newPrefix, sizeof(newPrefix), "%s    ", prefix);
    printTree(node->left, level + 1, newPrefix);
}

 int max(int a, int b) {
         return (a > b) ? a : b;
    }

int getHeight(Contact* node) {
	if (node == NULL) return 0;
	return node->height;
}

int getBalance(Contact* node) {
	if (node == NULL) return 0;
	return getHeight(node->left) - getHeight(node->right);
}

// Правый поворот
Contact* rightRotate(Contact* y) {
	Contact* x = y->left;
	Contact* T2 = x->right;

	// Выполняем поворот
	x->right = y;
	y->left = T2;

	// Обновляем родителей
	if (T2 != NULL) T2->parent = y;
	x->parent = y->parent;
	y->parent = x;

	// Обновляем высоты
	y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
	x->height = max(getHeight(x->left), getHeight(x->right)) + 1;

	return x;
}

// Левый поворот
Contact* leftRotate(Contact* x) {
	Contact* y = x->right;
	Contact* T2 = y->left;

	// Выполняем поворот
	y->left = x;
	x->right = T2;

	// Обновляем родителей
	if (T2 != NULL) T2->parent = x;
	y->parent = x->parent;
	x->parent = y;

	// Обновляем высоты
	x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
	y->height = max(getHeight(y->left), getHeight(y->right)) + 1;

	return y;
}

void View() {
	if (head == NULL) {
		printf("Дерево пусто\n");
		return;
	}

	printf("\n=== СТРУКТУРА ДЕРЕВА ===\n");
	printTree(head, 0, "");
	printf("\n");
}

