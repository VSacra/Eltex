#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>

#define MAX_ARGS 200
#define MAX_PIPES 100

void parse_redirects(char* token, char** infile, char** outfile, int* append) {
    *infile = NULL;
    *outfile = NULL;
    *append = 0;

    char* in = strstr(token, "<"); //Прочитать
    char* out = strstr(token, ">"); //Переписать
    char* append = strstr(token, ">>"); //Добавить

    // Обработка перенаправления ввода
    if (in) {
        *in = '\0'; //Обрываем строку
        *infile = in + 1; //Записываем начало того, откуда читаем
        // Убираем возможные пробелы
        while (**infile == ' ') (*infile)++; //Если были пробелы пропускаем
    }

    // Обработка перенаправления вывода (>> для дописывания)
    if (append) {
        *append = '\0';
        *outfile = append + 2;
        *append = 1;
        while (**outfile == ' ') (*outfile)++;
    }
    // Обработка перенаправления вывода (> для перезаписи)
    else if (out) {
        *out = '\0';
        *outfile = out + 1;
        *append = 0;
        while (**outfile == ' ') (*outfile)++;
    }
}

int main() {
    while (1) {
        printf("\nКомандный интерпретатор с конвейерами. Для выхода введите q.\nВведите команду: \n");
        char buff[1024];  

        if (fgets(buff, sizeof(buff), stdin) == NULL) {
            printf("\nНе удалось считать команду\n");
            return 1;
        }

        //Очищаем буффер от переносов
        buff[strcspn(buff, "\n")] = 0;
        if (strlen(buff) == 0) {
            continue;
        }

        if (buff[0] == 'q') return 0;

        // Разбиваем на команды по конвейерам
        char* commands[MAX_PIPES];
        int num_commands = 0;

        commands[num_commands] = strtok(buff, "|"); //Первое разделение по |
        while (commands[num_commands] != NULL && num_commands < MAX_PIPES - 1) {
            num_commands++; 
            commands[num_commands] = strtok(NULL, "|");
        }

        if (num_commands == 0) continue;

        int pipes[MAX_PIPES][2]; //По два канала на каждый | - ввод и вывод
        pid_t pids[MAX_PIPES];

        // Создаем пайпы для всех команд, кроме последней
        for (int i = 0; i < num_commands - 1; i++) {
            if (pipe(pipes[i]) == -1) {
                perror("pipe");
                return 1;
            }
        }

        // Запускаем каждую команду
        for (int i = 0; i < num_commands; i++) {
            pids[i] = fork();

            if (pids[i] == -1) {
                perror("fork");
                return 1;
            }

            if (pids[i] == 0) {  // Дочерний процесс
                char* infile = NULL;
                char* outfile = NULL;
                int append = 0;

                // Парсим перенаправления для этой команды
                parse_redirects(commands[i], &infile, &outfile, &append);

                // Обработка перенаправления ввода из файла
                if (infile != NULL && *infile != '\0') {
                    int fd = open(infile, O_RDONLY);
                    if (fd == -1) {
                        perror("open input file");
                        exit(1);
                    }
                    if (dup2(fd, STDIN_FILENO) == -1) { //Перенаправление ввода из файла
                        fprintf(stderr, "Ошибка перенаправления ввода из файла '%s': %s\n",
                            infile, strerror(errno));
                        close(fd);
                        exit(1);
                    } 
                    close(fd);
                }
                // Перенаправление ввода из предыдущего пайпа (если не первая команда)
                else if (i > 0) {
                    if (dup2(pipes[i - 1][0], STDIN_FILENO) == -1) {
                        fprintf(stderr, "Ошибка перенаправления ввода из пайпа: %s\n",
                            strerror(errno));
                        exit(1);
                    }
                }

                // Обработка перенаправления вывода в файл
                if (outfile != NULL && *outfile != '\0') {
                    int flags = O_WRONLY | O_CREAT;
                    flags |= append ? O_APPEND : O_TRUNC;
                    int fd = open(outfile, flags, 0644);
                    if (fd == -1) {
                        perror("open output file");
                        exit(1);
                    }
                    if (dup2(fd, STDOUT_FILENO) == -1) { //Перенаправление вывода в файл
                        fprintf(stderr, "Ошибка перенаправления вывода в файл '%s': %s\n",
                            outfile, strerror(errno));
                        close(fd);
                        exit(1);
                    } 
                    close(fd);
                }
                // Перенаправление вывода в следующий пайп (если не последняя команда)
                else if (i < num_commands - 1) {
                    if (dup2(pipes[i][1], STDOUT_FILENO) == -1) {
                        fprintf(stderr, "Ошибка перенаправления вывода в пайп: %s\n",
                            strerror(errno));
                        exit(1);
                    }
                }

                // Закрываем все пайпы в дочернем процессе
                for (int j = 0; j < num_commands - 1; j++) {
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                }

                // Разбиваем команду на аргументы
                char* args[MAX_ARGS];
                int arg_count = 0;

                // Убираем лишние пробелы в начале команды
                char* cmd = commands[i];
                while (*cmd == ' ') cmd++;

                args[arg_count] = strtok(cmd, " ");
                while (args[arg_count] != NULL && arg_count < MAX_ARGS - 1) {
                    arg_count++;
                    args[arg_count] = strtok(NULL, " ");
                }
                args[arg_count] = NULL;

                if (args[0] == NULL) {
                    exit(0);
                }

                execvp(args[0], args);
                perror("execvp");
                exit(1);
            }
        }

        // Закрываем все пайпы в родительском процессе
        for (int i = 0; i < num_commands - 1; i++) {
            close(pipes[i][0]);
            close(pipes[i][1]);
        }

        // Ждем завершения всех дочерних процессов
        for (int i = 0; i < num_commands; i++) {
            waitpid(pids[i], NULL, 0);
        }
    }

    return 0;
}