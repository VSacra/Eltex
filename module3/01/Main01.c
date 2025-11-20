#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

int obr(char* arg,long* argI, double* argD) {
	char* endptr; //Указатель на первый символ, который не удалось преобразовать в число

	long value;

	errno = 0;

	value = strtol(arg, &endptr, 10);

	if (errno == ERANGE) return errno;
	else if (endptr == arg)  return 0;
	else if (*endptr == '\0') {
		*argI = value * 2;
		return 1;
	}
	else if (*endptr == '.') {
		double floatVal = strtod(arg, &endptr);
		if (errno == ERANGE) return ERANGE;
		if (*endptr == '\0') {
			*argD = floatVal * 2;
			return 2;
		}
	}
	return 0;
}

int main(int argc, char* argv[]) {

	if (argc == 1) {
		printf("\nНе было предано ни одного аргумента, кроме названия программы.\n");
	}
	else {
		pid_t pid = fork();

		switch (pid) {
		case 0: {
			pid_t UID = getpid();
			for (int i = 1; i < argc; i += 2) {
				long INT=0;
				double DOB=0;
				int res = obr(argv[i], &INT, &DOB);
				switch (res) {
				case ERANGE: {
					perror("\nПереполнение.\n");
					return -1;
				}
				case 0: {
					printf("\nАргумент %d обработан процессом #%d: %s\n", i + 1,UID,argv[i]);
					break;
				}
				case 1: {
					printf("\nАргумент %d обработан процессом #%d: %s %ld\n", i + 1, UID, argv[i],INT);
					break;
				}
				case 2: {
					printf("\nАргумент %d обработан процессом #%d: %s %.3f\n", i + 1, UID, argv[i], DOB);
					break;
				}
				}
			}
			break;
		}
		case -1: {
			perror("\nНе удалось создать дочерний поток.\n");
			return -1;
			break;
		}
		default: {
			pid_t UID = getpid();
			for (int i = 0; i < argc; i += 2) {
				long INT = 0;
				double DOB = 0;
				int res = obr(argv[i], &INT, &DOB);
				switch (res) {
				case ERANGE: {
					perror("\nПереполнение.\n");
					return -1;
				}
				case 0: {
					printf("\nАргумент %d обработан процессом #%d: %s\n", i + 1, UID, argv[i]);
					break;
				}
				case 1: {
					printf("\nАргумент %d обработан процессом #%d: %s %ld\n", i + 1, UID, argv[i], INT);
					break;
				}
				case 2: {
					printf("\nАргумент %d обработан процессом #%d: %s %.3f\n", i + 1, UID, argv[i], DOB);
					break;
				}
			}}
			break;}
		}
	}

	return 0;
}
