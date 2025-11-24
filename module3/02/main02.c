#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
	while (1) {
		printf("\nКомандный итерпритатор. Для выхода введите q.\nВведите команду: \n");
		char buff[256];
		
        	// Читаем всю строку
        	if (fgets(buff, sizeof(buff), stdin) == NULL) {
            		printf("\nНе удалось считать команду\n");
			return 1;
        	}

		buff[strcspn(buff, "\n")]=0;
		if (strlen(buff) == 0) {
			continue;
		}
        
		if (buff[0] == 'q') return 0;
		char* arg[200];
	
		int i=0;
		arg[i] = strtok(buff, " ");
		while (arg[i] != NULL && i < 200-1) {
			i++;
			arg[i] = strtok(NULL, " ");
		}
		arg[i]=NULL;
		
		pid_t pid = fork();

		switch (pid){
		case 0: {
			execvp(arg[0], arg);

			printf("\nКоманда %s не найдена.\n",arg[0]);
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
