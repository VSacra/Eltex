#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>

// Имя разделяемой памяти
#define SHM_NAME "/random_numbers_shm"

// Максимальное количество чисел в наборе
#define MAX_NUMBERS 20

// Структура для разделяемой памяти
struct shared_data {
    int numbers[MAX_NUMBERS];  // Массив чисел
    int count;                  // Количество чисел в массиве
    int min_value;              // Минимальное число (результат)
    int max_value;              // Максимальное число (результат)
};

// Глобальные переменные для обработки сигналов
volatile sig_atomic_t keep_running = 1;
int processed_count = 0;

// Обработчик сигнала SIGINT (Ctrl+C)
void handle_signal(int sig) {
    printf("\nПолучен сигнал SIGINT. Завершаю работу...\n");
    keep_running = 0;
}

int main() {
    int shm_fd;
    struct shared_data *shared;
    pid_t pid;
    
    printf("\nСтарт работы. Для завершения нажмите Ctrl+C\n\n");
    
    // Настраиваем обработчик сигнала SIGINT
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_signal;
    sigaction(SIGINT, &sa, NULL);
    
    // Инициализация генератора случайных чисел
    srand(time(NULL));
    
    // Удаляем разделяемую память, если уже существует
    shm_unlink(SHM_NAME);
    
    // Создаем новую разделяемую память
    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Ошибка создания разделяемой памяти");
        return 1;
    }
    
    // Устанавливаем размер разделяемой памяти
    if (ftruncate(shm_fd, sizeof(struct shared_data)) == -1) {
        perror("Ошибка установки размера разделяемой памяти");
        shm_unlink(SHM_NAME);
        return 1;
    }
    
    // Отображаем разделяемую память 
    shared = mmap(NULL, sizeof(struct shared_data), 
                  PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared == MAP_FAILED) {
        perror("Ошибка отображения разделяемой памяти");
        shm_unlink(SHM_NAME);
        return 1;
    }

    // Пока не поступил сигнал
    while (keep_running) {
        
        // Генерируем случайное количество чисел (от 3 до MAX_NUMBERS)
        shared->count = rand() % (MAX_NUMBERS - 2) + 3;
        
        printf("[РОДИТЕЛЬ] Количество чисел в наборе: %d\n", shared->count);
        
        if (shared->count == 0) {
            printf("[РОДИТЕЛЬ] Ошибка: 0 чисел\n");
            continue;
        }
        
        printf("[РОДИТЕЛЬ] Числа: ");
        
        // Генерируем случайные числа
        for (int i = 0; i < shared->count; i++) {
            shared->numbers[i] = rand() % 1000;  // Числа от 0 до 999
            printf("%d ", shared->numbers[i]);
        }
        printf("\n");
        
        // Сбрасываем результаты
        shared->min_value = 0;
        shared->max_value = 0;
        
        // Создаём дочерний процесс
        pid = fork();
        
        if (pid == -1) {
            perror("Ошибка создания дочернего процесса");
            break;
        }
        
        // Если дочерний
        if (pid == 0) {
            printf("[ДОЧЕРНИЙ] Процесс запущен PID: %d\n", getpid());
            
            // Проверяем, есть ли данные для обработки
            if (shared->count > 0) {
                // Инициализируем min и max первым числом
                shared->min_value = shared->numbers[0];
                shared->max_value = shared->numbers[0];
                

                 for (int i = 1; i < shared->count; i++) {
                    if (shared->numbers[i] < shared->min_value) {
                           shared->min_value = shared->numbers[i];
                    }
                    if (shared->numbers[i] > shared->max_value) {
                        shared->max_value = shared->numbers[i];
                    }
                }
                
                printf("[ДОЧЕРНИЙ] Найдено: min = %d, max = %d\n", 
                       shared->min_value, shared->max_value);
            } else {
                printf("[ДОЧЕРНИЙ] Ошибка: нет чисел для обработки\n");
            }
            
            // Отключаем разделяемую память и завершаемся
            munmap(shared, sizeof(struct shared_data));
            close(shm_fd);
            exit(0);
        }
        // Родитель
        else {
            
            // Ждем завершения дочернего процесса
            wait(NULL);
            
            // Выводим результаты
            printf("[РОДИТЕЛЬ] Результаты обработки:\n");
            printf("[РОДИТЕЛЬ] Минимальное число: %d\n", shared->min_value);
            printf("[РОДИТЕЛЬ] Максимальное число: %d\n", shared->max_value);
            
            processed_count++;
            printf("[РОДИТЕЛЬ] Обработано наборов: %d\n\n", processed_count);
            
            printf("\n");
        }
    }
    
    // Завершение работы
    printf("\nЗавершение программы\n");
    printf("Всего обработано наборов данных: %d\n", processed_count);
    
    // Отключаем разделяемую память
    if (munmap(shared, sizeof(struct shared_data)) == -1) {
        perror("Ошибка отключения разделяемой памяти");
    }
    
    // Закрываем файловый дескриптор
    close(shm_fd);
    
    // Удаляем разделяемую память
    if (shm_unlink(SHM_NAME) == -1) {
        perror("Ошибка удаления разделяемой памяти");
    } else {
        printf("Разделяемая память удалена\n");
    }
    
    return 0;
}