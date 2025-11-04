#define _CRT_SECURE_NO_WARNINGS
#include "chmod3_1.h"
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h> 

#ifdef _WIN32
// Windows definitions
#include <io.h>
#define S_IRUSR _S_IREAD
#define S_IWUSR _S_IWRITE
#define S_IXUSR _S_IEXEC
#define S_IRGRP (S_IRUSR >> 3)
#define S_IWGRP (S_IWUSR >> 3)
#define S_IXGRP (S_IXUSR >> 3)
#define S_IROTH (S_IRUSR >> 6)
#define S_IWOTH (S_IWUSR >> 6)
#define S_IXOTH (S_IXUSR >> 6)
#else
#endif

struct stat per;

int getPerByte(char* FilePath) {
	if (stat(FilePath, &per) == -1) return - 1;

	int Permission = (per.st_mode & S_IRUSR) | (per.st_mode & S_IWUSR) | (per.st_mode & S_IXUSR) |
		(per.st_mode & S_IRGRP) | (per.st_mode & S_IWGRP) | (per.st_mode & S_IXGRP) |
		(per.st_mode & S_IROTH) | (per.st_mode & S_IWOTH) | (per.st_mode & S_IXOTH);

	return Permission;
}

void printPerRWX(int mode) {
	printf("%c%c%c", (mode & S_IRUSR) ? 'r' : '-',
		(mode & S_IWUSR) ? 'w' : '-',
		(mode & S_IXUSR) ? 'x' : '-');
	printf("%c%c%c", (mode & S_IRGRP) ? 'r' : '-',
		(mode & S_IWGRP) ? 'w' : '-',
		(mode & S_IXGRP) ? 'x' : '-');
	printf("%c%c%c", (mode & S_IROTH) ? 'r' : '-',
		(mode & S_IWOTH) ? 'w' : '-',
		(mode & S_IXOTH) ? 'x' : '-');
}

void printPerNum(int mode) {
	int u = 0, g = 0, o = 0;
	if (mode & S_IRUSR) u += 4;
	if (mode & S_IWUSR) u += 2;
	if (mode & S_IXUSR) u += 1;
	if (mode & S_IRGRP) g += 4;
	if (mode & S_IWGRP) g += 2;
	if (mode & S_IXGRP) g += 1;
	if (mode & S_IROTH) o += 4;
	if (mode & S_IWOTH) o += 2;
	if (mode & S_IXOTH) o += 1;
	printf("%d%d%d",u,g,o);
}

void printPer() {
	int c;
	while ((c = getchar()) != '\n' && c != EOF);
	char path[256];
	printf("\nВведите название файла: ");
	fgets(path, sizeof(path), stdin);
	path[strcspn(path, "\n")] = '\0';
	int mode = getPerByte(path);
	if (mode==-1) {
		printf("\nОшибка чтения файла: %s", path);
		return;
	}
	printf("\nБитовое представление: ");
	for (int i = 8; i >= 0; i--) {
		printf("%d", (mode >> i) & 1);
		if (i == 6 || i == 3) printf(" ");
	}
	printf("\nRWX-представление: ");
	printPerRWX(mode);
	printf("\nЧисленное представление: ");
	printPerNum(mode);
}

int ToByte(char* mode) {
	if (mode == NULL || strlen(mode) < 3) return -1;
	int per = 0;
	if (isdigit(mode[0]) && isdigit(mode[1]) && isdigit(mode[2])) {
		for (int i = 0; i < 3; i++) {
			if (mode[i] < '0' || mode[i] > '7') return -1;
		}
		per = ((mode[0] - '0') << 6) | ((mode[1] - '0') << 3) | (mode[2] - '0');
	}
	else {
		if (strlen(mode) < 9) return -1;
		if (mode[0] == 'r' || mode[0] == 'R') per |= S_IRUSR;
		if (mode[1] == 'w' || mode[1] == 'W') per |= S_IWUSR;
		if (mode[2] == 'x' || mode[2] == 'X') per |= S_IXUSR;

		if (mode[3] == 'r' || mode[0] == 'R') per |= S_IRGRP;
		if (mode[4] == 'w' || mode[1] == 'W') per |= S_IWGRP;
		if (mode[5] == 'x' || mode[2] == 'X') per |= S_IXGRP;

		if (mode[6] == 'r' || mode[0] == 'R') per |= S_IROTH;
		if (mode[7] == 'w' || mode[1] == 'W') per |= S_IWOTH;
		if (mode[8] == 'x' || mode[2] == 'X') per |= S_IXOTH;
	}
	return per;
}

void ToByteConsole() {
	int c;
	while ((c = getchar()) != '\n' && c != EOF);
	printf("\nВведите представление без пробелов: ");
	char mode[11];
	fgets(mode, sizeof(mode), stdin);
	int result = ToByte(mode);
	if (result == -1) printf("\nОшибка! Данные были введены неккоректно.\n");
	else for (int i = 8; i >= 0; i--) {
		printf("%d", (result >> i) & 1);
		if (i == 6 || i == 3) printf(" ");
	}
}

int getwho(char* who) {
	if (who == NULL) return -1;
	int result = 0;

	for (int i = 0; i < 3 && who[i] != '\0'; i++) {
		switch (who[i]) {
		case 'u': result |= (S_IRUSR | S_IWUSR | S_IXUSR); break;
		case 'g': result |= (S_IRGRP | S_IWGRP | S_IXGRP); break;
		case 'o': result |= (S_IROTH | S_IWOTH | S_IXOTH); break;
		case 'a': result |= (S_IRUSR | S_IWUSR | S_IXUSR |
			S_IRGRP | S_IWGRP | S_IXGRP |
			S_IROTH | S_IWOTH | S_IXOTH); break;
		}
	}
	return result;
}

int getwhat(char* what) {
	if (what == NULL) return -1;
	int result = 0;

	for (int i = 0; i < 3 && what[i] != '\0'; i++) {
		switch (what[i]) {
		case 'r': result |= (S_IRUSR | S_IRGRP | S_IROTH); break;
		case 'w': result |= (S_IWUSR | S_IWGRP | S_IWOTH); break;
		case 'x': result |= (S_IXUSR | S_IXGRP | S_IXOTH); break;
		}
	}
	return result;
}

int* chmode(int* per, char* command) {
	if (per == NULL || command == NULL) {
		return NULL;
	}

	// Числовой формат
	if (isdigit(command[0]) && isdigit(command[1]) && isdigit(command[2])) {
		*per = ToByte(command);
		return per;
	}

	// Символьный формат 
	char operations[3][20] = { "" }; // Максимум 3 операции
	int op_count = 0;

	// Разделяем команду по запятым
	char* token = strtok(command, ",");
	while (token != NULL && op_count < 3) {
		strncpy(operations[op_count], token, sizeof(operations[op_count]) - 1);
		operations[op_count][sizeof(operations[op_count]) - 1] = '\0';
		op_count++;
		token = strtok(NULL, ",");
	}

	// Обрабатываем каждую операцию
	for (int i = 0; i < op_count; i++) {
		char who[10] = "";
		char what[10] = "";
		char how = ' ';

		// Разбираем отдельную операцию 
		for (size_t j = 0; j < strlen(operations[i]); j++) {
			char c = operations[i][j];
			if (c == '+' || c == '-' || c == '=') {
				how = c;
			}
			else {
				char temp[2] = { c, '\0' };
				if (how == ' ') {
					if (strlen(who) < sizeof(who) - 1) {
						strcat(who, temp);
					}
				}
				else {
					if (strlen(what) < sizeof(what) - 1) {
						strcat(what, temp);
					}
				}
			}
		}

		if (how == ' ') {
			return NULL; // Оператор не найден
		}

		int whob = getwho(who);
		int whatb = getwhat(what);

		if (whob == -1 || whatb == -1) {
			return NULL;
		}

		int mask = whob & whatb;

		switch (how) {
		case '+': *per |= mask; break;
		case '-': *per &= ~mask; break;
		case '=':
			*per &= ~whob;
			*per |= mask;
			break;
		default: return NULL;
		}
	}

	return per;
}

void TestChmod() {
	int c;
	while ((c = getchar()) != '\n' && c != EOF);
	printf("\nВведите представление без пробелов: ");
	char mode[11]; int per = 0;
	fgets(mode, sizeof(mode), stdin);
	mode[strcspn(mode, "\n")] = '\0';
	if (strlen(mode) < 3) printf("\nОшибка чтения представления.\n");
	if (strlen(mode) < 5) per = ToByte(mode);
	else {
		if (!isdigit(mode[0])) per = ToByte(mode);
		else for (int i = 0; i < 9; i++) {
			per = per << 1;
			per += mode[i] - '0';
		}
	}
	printf("\nВведите вашу команду:\nchmod ");
	char command[20];
	fgets(command, sizeof(command), stdin);
	command[strcspn(command, "\n")] = '\0';
	if (strlen(command) < 3) printf("\nОшибка чтения команды.\n");
	if (chmode(&per, command) == NULL) {
		printf("\nОшибка выполнения команды!\n");
	}
	else {
		printf("\nПрава были изменены.\nБитовое представление: ");
		for (int i = 8; i >= 0; i--) {
			printf("%d", (per >> i) & 1);
			if (i == 6 || i == 3) printf(" ");
		}
		printf("\nRWX-представление: ");
		printPerRWX(per);
		printf("\nВосьмеричное представление: ");
		printPerNum(per);
	}
}

void FileChmod() {
	int c;
	while ((c = getchar()) != '\n' && c != EOF);
	char path[256];
	printf("\nВведите название файла: ");
	fgets(path, sizeof(path), stdin);
	path[strcspn(path, "\n")] = '\0';
	int mode = getPerByte(path);
	if (mode == -1) {
		printf("\nОшибка чтения файла: %s", path);
		return;
	}
	printf("\nБитовое представление: ");
	for (int i = 8; i >= 0; i--) {
		printf("%d", (mode >> i) & 1);
		if (i == 6 || i == 3) printf(" ");
	}
	printf("\nRWX-представление: ");
	printPerRWX(mode);
	printf("\nЧисленное представление: ");
	printPerNum(mode);
	printf("\nВведите вашу команду:\nchmod ");
	char command[20];
	fgets(command, sizeof(command), stdin);
	command[strcspn(command, "\n")] = '\0';
	if (strlen(command) < 3) printf("\nОшибка чтения команды.\n");
	if (chmode(&mode, command) == NULL) {
		printf("\nОшибка выполнения команды!\n");
	}
	else {
		printf("\nПрава были изменены.\nБитовое представление: ");
		for (int i = 8; i >= 0; i--) {
			printf("%d", (mode >> i) & 1);
			if (i == 6 || i == 3) printf(" ");
		}
		printf("\nRWX-представление: ");
		printPerRWX(mode);
		printf("\nВосьмеричное представление: ");
		printPerNum(mode);
	}
}
