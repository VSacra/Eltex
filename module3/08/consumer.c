#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <errno.h>

#define KEY 12345
#define MAX_STRING_LENGTH 200

struct shared_data {
    char lines[100][MAX_STRING_LENGTH];
    int line_count;
    int processed_count;
};

// Ждем пока значение семафора станет 0
void wait_until_zero(int sem_id, int sem_num) {
    struct sembuf sem_op;
    sem_op.sem_num = sem_num;
    sem_op.sem_op = 0;         // Ждем пока значение = 0
    sem_op.sem_flg = 0;
    
    if (semop(sem_id, &sem_op, 1) == -1) {
        perror("Ошибка: не могу дождаться нуля семафора");
        exit(1);
    }
}

int main() {
    int sem_id;
    int shm_id;
    struct shared_data *shared_mem;
    int consumer_id = getpid();
    
    printf("Запуск потребителя PID: %d\n", consumer_id);
    
    // Подключаемся к семафору
    sem_id = semget(KEY, 2, 0666);
    if (sem_id == -1) {
        perror("Ошибка подключения к семафору");
        return 1;
    }
    
    // Подключаемся к разделяемой памяти
    shm_id = shmget(KEY, sizeof(struct shared_data), 0666);
    if (shm_id == -1) {
        perror("Ошибка подключения к разделяемой памяти");
        return 1;
    }
    
    shared_mem = (struct shared_data *)shmat(shm_id, NULL, 0);
    if (shared_mem == (void *)-1) {
        perror("Ошибка подключения разделяемой памяти");
        return 1;
    }
    
    printf("Ожидаю строки для обработки...\n\n");
    
    int processed_by_me = 0;
    
    while (1) {
        // Ждём пока будет доступна строка (sem[1] > 0)
        struct sembuf wait_for_string = {1, -1, 0};
        if (semop(sem_id, &wait_for_string, 1) == -1) {
            perror("Ошибка ожидания строки");
            break;
        }
        
        // Ждём пока мьютекс станет 0 (производитель или другие потребители не работают)
        wait_until_zero(sem_id, 0);
        
        // 3ахват мьютекса
        struct sembuf take_mutex = {0, 1, 0};
        if (semop(sem_id, &take_mutex, 1) == -1) {
            perror("Ошибка взятия мьютекса");
            break;
        }
        
        if (shared_mem->processed_count >= shared_mem->line_count) {
            // Строк больше нет, возвращаем взятое
            struct sembuf return_string = {1, 1, 0};
            semop(sem_id, &return_string, 1);
            struct sembuf release_mutex = {0, -1, 0};
            semop(sem_id, &release_mutex, 1);
            break;
        }
        
        int index = shared_mem->processed_count; //Сколько уже проверили
        char *line = shared_mem->lines[index]; //Берём первую непроверенную
        shared_mem->processed_count++;
        
        // Освобождаем мьютекс
        struct sembuf release_mutex = {0, -1, 0};
        if (semop(sem_id, &release_mutex, 1) == -1) {
            perror("Ошибка освобождения мьютекса");
            break;
        }
        
        // Обрабатываю строку (вне критической секции)
        printf("[Потребитель %d] Обрабатываю строку %d: %s\n", 
               consumer_id, index + 1, line);
        
        // Разделяем строку по " "
        char *token = strtok(line, " ");
        int first = 1;
        int min_val, max_val, count = 0;
        
        while (token) {
            int num = atoi(token);
            if (first) {
                min_val = max_val = num;
                first = 0;
            } else {
                if (num < min_val) min_val = num;
                if (num > max_val) max_val = num;
            }
            count++;
            token = strtok(NULL, " ");
        }
        
        if (count > 0) {
            printf("[Потребитель %d] Результат: чисел = %d, min = %d, max = %d\n\n",
                   consumer_id, count, min_val, max_val);
        }
        
        processed_by_me++;
        sleep(2 + rand() % 3);
    }
    
    printf("[Потребитель %d] Завершил работу. Обработано: %d строк\n", 
           consumer_id, processed_by_me);
    
    shmdt(shared_mem);
    return 0;
}
