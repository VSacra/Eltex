#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
	while (1) {
		printf("\nКомандный итерпритатор. Для выхода введите q.\nВведите команду: \n");
		char buff[256];
		scanf("%s", &buff);
		if (buff[0] == 'q') return 0;
		char* arg[200];
		arg[0] = strtok(buff, " ");
		for (int i = 1; arg[i] != NULL && i < 200; i++) {
			arg[i] = strtok(NULL, " ");
		}
		pid_t pid = fork();

		switch (pid){
		case 0: {
			execvp(arg[0], arg);

			printf("\nКоманда %s не найдена.\n",args[0]);
			return 1;
			break;
		}
		case -1: {
			printf("\nНе удалось создать дочерний процесс\n");
			return 1;
			break;
		}
		default: {
			wait(NULL);
			break;
		}
		}
	}
}