#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>

// Макросы
#define MAX_STRINGS 100
#define MAX_STRING_LEN 200
#define SHM_NAME "/producer_consumer_shm"
#define SEM_NAME "/producer_consumer_sem"

// Структура для разделяемой памяти
struct shared_data {
    char strings[MAX_STRINGS][MAX_STRING_LEN];
    int total_strings;
    int processed_strings;
    int should_exit;
};

int main(int argc, char *argv[]) {
    int num_strings = 10;
    sem_t *sem;
    int shm_fd;
    struct shared_data *shared;
    pid_t pid;
    
    // Проверка аргументов
    if (argc > 1) {
        num_strings = atoi(argv[1]);
        if (num_strings <= 0 || num_strings > MAX_STRINGS) {
            printf("Количество строк должно быть от 1 до %d\n", MAX_STRINGS);
            return 1;
        }
    }
    
    printf("Будет сгенерировано строк: %d\n\n", num_strings);
    
    // Создаём и открываем разделяемую память
    // Удаляем если уже существует
    shm_unlink(SHM_NAME);
    
    // Создаем новую разделяемую память
    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Ошибка создания разделяемой памяти");
        return 1;
    }
    
    // Устанавливаем размер
    if (ftruncate(shm_fd, sizeof(struct shared_data)) == -1) {
        perror("Ошибка установки размера разделяемой памяти");
        shm_unlink(SHM_NAME);
        return 1;
    }
    
    // Отображаем в память
    shared = mmap(NULL, sizeof(struct shared_data), 
                  PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared == MAP_FAILED) {
        perror("Ошибка отображения разделяемой памяти");
        shm_unlink(SHM_NAME);
        return 1;
    }
    
    // Инициализируем разделяемую память
    shared->total_strings = 0;
    shared->processed_strings = 0;
    shared->should_exit = 0;
    
    // Создаём семафор
    // Удаляем если уже существует
    sem_unlink(SEM_NAME);
    
    // Создаем новый семафор с начальным значением 1
    sem = sem_open(SEM_NAME, O_CREAT, 0666, 1);
    if (sem == SEM_FAILED) {
        perror("Ошибка создания семафора");
        munmap(shared, sizeof(struct shared_data));
        shm_unlink(SHM_NAME);
        return 1;
    }
    
    printf("Семафор создан\n\n");
    
    // Инициализация генератора случайных чисел
    srand(time(NULL));
    
    // Создаём дочерний процесс
    pid = fork();
    
    if (pid == -1) {
        perror("Ошибка создания дочернего процесса");
        sem_close(sem);
        sem_unlink(SEM_NAME);
        munmap(shared, sizeof(struct shared_data));
        shm_unlink(SHM_NAME);
        return 1;
    }
    
    // Дочерний процесс - потребитель
    if (pid == 0) {
        printf("[ДОЧЕРНИЙ] Процесс потребитель запущен PID: %d\n", getpid());
        
        int processed_count = 0;
        
        while (1) {
            // Ждем семафора (уменьшаем значение)
            if (sem_wait(sem) == -1) {
                perror("[ДОЧЕРНИЙ] Ошибка sem_wait");
                break;
            }
            
            // Проверяем, нужно ли завершать
            if (shared->should_exit && 
                shared->processed_strings >= shared->total_strings) {
                printf("[ДОЧЕРНИЙ] Получен сигнал завершения\n");
                sem_post(sem);
                break;
            }
            
            // Проверяем, есть ли необработанные строки
            if (shared->processed_strings < shared->total_strings) {
                // Берем следующую строку
                int idx = shared->processed_strings;
                char *str = shared->strings[idx];
                shared->processed_strings++;
                
                // Освобождаем семафор (увеличиваем значение)
                sem_post(sem);
                
                // Обрабатываем строку (вне критической секции)
                printf("[ДОЧЕРНИЙ] Обрабатываю строку %d: %s\n", 
                       idx + 1, str);
                
                // Ищем минимальное и максимальное число
                char *token = strtok(str, " ");
                int first = 1;
                int min_val, max_val;
                int num_count = 0;
                
                while (token != NULL) {
                    int num = atoi(token);
                    
                    if (first) {
                        min_val = max_val = num;
                        first = 0;
                    } else {
                        if (num < min_val) min_val = num;
                        if (num > max_val) max_val = num;
                    }
                    
                    num_count++;
                    token = strtok(NULL, " ");
                }
                
                if (num_count > 0) {
                    printf("[ДОЧЕРНИЙ] Результат: чисел = %d, min = %d, max = %d\n\n",
                           num_count, min_val, max_val);
                }
                
                processed_count++;
                
            } else {
                // Нет строк для обработки, освобождаем семафор
                sem_post(sem);
            }
        }
        
        printf("[ДОЧЕРНИЙ] Завершаю работу. Обработано строк: %d\n", 
               processed_count);
        
        // Закрываем семафор
        sem_close(sem);
        
        // Отключаем разделяемую память
        munmap(shared, sizeof(struct shared_data));
        
        exit(0);
    }
    
    // Родительский процесс - производитель
    else {
        printf("[РОДИТЕЛЬ] Процесс производитель PID: %d\n", getpid());
        
        for (int i = 0; i < num_strings; i++) {
            // Генерируем строку со случайными числами
            char buffer[MAX_STRING_LEN] = {0};
            int num_count = rand() % 10 + 1;  // От 1 до 10 чисел
            
            for (int j = 0; j < num_count; j++) {
                int num = rand() % 1000;  // Числа от 0 до 999
                char num_str[20];
                sprintf(num_str, "%d", num);
                
                if (j > 0) {
                    strcat(buffer, " ");
                }
                strcat(buffer, num_str);
            }
            
            printf("[РОДИТЕЛЬ] Сгенерирована строка %d: %s\n", i + 1, buffer);
            
            // Захватываем семафор для записи в разделяемую память
            if (sem_wait(sem) == -1) {
                perror("[РОДИТЕЛЬ] Ошибка sem_wait");
                break;
            }
            
            // Записываем строку в разделяемую память
            if (shared->total_strings < MAX_STRINGS) {
                strcpy(shared->strings[shared->total_strings], buffer);
                shared->total_strings++;
            }
            
            // Освобождаем семафор
            sem_post(sem);
            
        }
        
        printf("\n[РОДИТЕЛЬ] Генерация завершена. Всего строк: %d\n", num_strings);
        
        // Ждем пока дочерний процесс обработает все строки
        printf("[РОДИТЕЛЬ] Ожидаю обработки всех строк...\n");
        
        int last_processed = -1;
        while (1) {
            // Захватываем семафор для чтения состояния
            sem_wait(sem);
            
            int remaining = shared->total_strings - shared->processed_strings;
            
            if (remaining == 0) {
                // Все строки обработаны, отмечаем для выхода
                shared->should_exit = 1;
                sem_post(sem);
                break;
            }
            
            // Выводим прогресс если изменилось
            if (shared->processed_strings != last_processed) {
                printf("[РОДИТЕЛЬ] Обработано: %d/%d строк\n", 
                       shared->processed_strings, shared->total_strings);
                last_processed = shared->processed_strings;
            }
            
            sem_post(sem);
            sleep(1);
        }
        
        // Ждем завершения дочернего процесса
        printf("[РОДИТЕЛЬ] Ожидаю завершения дочернего процесса...\n");
        wait(NULL);
        
        // Закрываем и удаляем семафор
        sem_close(sem);
        sem_unlink(SEM_NAME);
        
        // Отключаем и удаляем разделяемую память
        munmap(shared, sizeof(struct shared_data));
        shm_unlink(SHM_NAME);
        
        printf("[РОДИТЕЛЬ] Все ресурсы очищены\n");
        printf("[РОДИТЕЛЬ] Программа завершена\n");
    }
    
    return 0;
}