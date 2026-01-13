#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <time.h>
#include <errno.h>

// Ключ для семафора и разделяемой памяти
#define KEY 12345
#define MAX_STRING_LENGTH 200
#define MAX_LINES 100

// Структура для разделяемой памяти
struct shared_data {
    char lines[MAX_LINES][MAX_STRING_LENGTH]; //двумерный массив строк
    int line_count; //Количество строк
    int processed_count; //Сколько сгенерировано
};

// Функция для ожидания, пока значение семафора станет 0
void wait_until_zero(int sem_id, int sem_num) {
    struct sembuf sem_op;
    sem_op.sem_num = sem_num;
    sem_op.sem_op = 0;  // Ждем пока значение станет 0
    sem_op.sem_flg = 0;
    
    if (semop(sem_id, &sem_op, 1) == -1) {
        perror("Ошибка: не могу дождаться нуля семафора");
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    int sem_id;
    int shm_id;
    struct shared_data *shared_mem;
    int num_strings = 10;
    
    printf("Запуск производителя\n");
    
    // Проверяем аргументы командной строки
    if (argc > 1) {
        num_strings = atoi(argv[1]);
        if (num_strings <= 0 || num_strings > MAX_LINES) {
            printf("Количество строк должно быть от 1 до %d\n", MAX_LINES);
            return 1;
        }
    }
    
    printf("Будет сгенерировано строк: %d\n", num_strings);
    
    // Инициализация генератора случайных чисел
    srand(time(NULL) + getpid());
    
    // Создаём семафор
    sem_id = semget(KEY, 2, IPC_CREAT | 0666);
    if (sem_id == -1) {
        perror("Ошибка создания семафора");
        return 1;
    }
    
    // Инициализируем семафоры
    // sem[0] = 0 (мьютекс закрыт) - будем ждать 0 перед работой
    // sem[1] = 0 (нет доступных строк)
    union semun {
        int val;
        struct semid_ds *buf;
        unsigned short *array;
    } sem_init;
    
    unsigned short init_values[2] = {0, 0};  // Оба начинаются с 0
    sem_init.array = init_values;
    
    if (semctl(sem_id, 0, SETALL, sem_init) == -1) {
        perror("Ошибка инициализации семафоров");
        semctl(sem_id, 0, IPC_RMID);
        return 1;
    }
    
    printf("Семафор создан. Начальные значения: sem[0]=%d, sem[1]=%d\n", 
           semctl(sem_id, 0, GETVAL), semctl(sem_id, 1, GETVAL));
    
    // Создаём разделяемую память
    shm_id = shmget(KEY, sizeof(struct shared_data), IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("Ошибка создания разделяемой памяти");
        semctl(sem_id, 0, IPC_RMID);
        return 1;
    }
    
    // Подключаем разделяемую память
    shared_mem = (struct shared_data *)shmat(shm_id, NULL, 0);
    if (shared_mem == (void *)-1) {
        perror("Ошибка подключения разделяемой памяти");
        semctl(sem_id, 0, IPC_RMID);
        shmctl(shm_id, IPC_RMID, NULL);
        return 1;
    }
    
    // Инициализируем разделяемую память
    shared_mem->line_count = 0;
    shared_mem->processed_count = 0;
    
    printf("Начинаю генерацию строк...\n");
    
    for (int i = 0; i < num_strings; i++) {
        // Генерируем строку со случайными числами
        char buffer[MAX_STRING_LENGTH] = {0};
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
        
        printf("Сгенерирована строка %d: %s\n", i + 1, buffer);
        
        // Ждём пока мьютекс (sem[0]) станет 0
        wait_until_zero(sem_id, 0);
        
        // Увеличиваем мьютекс (производитель работает с памятью)
        struct sembuf take_mutex = {0, 1, 0};
        if (semop(sem_id, &take_mutex, 1) == -1) {
            perror("Ошибка взятия мьютекса");
            break;
        }
        
        // Записываем строку
        if (shared_mem->line_count < MAX_LINES) {
            strcpy(shared_mem->lines[shared_mem->line_count], buffer);
            shared_mem->line_count++;
            
            printf("Строка %d записана в память\n", i + 1);
        } else {
            printf("Ошибка: превышен лимит строк\n");
        }
        
        // Освобождаем мьютекс
        struct sembuf release_mutex = {0, -1, 0};
        if (semop(sem_id, &release_mutex, 1) == -1) {
            perror("Ошибка освобождения мьютекса");
            break;
        }
        
        // Увеличиваем счётчик строк
        // Теперь потребители знают, что есть новая строка
        struct sembuf add_string = {1, 1, 0};
        if (semop(sem_id, &add_string, 1) == -1) {
            perror("Ошибка увеличения счетчика строк");
            break;
        }
        
        printf("Счетчик строк увеличен. Доступно строк: %d\n", 
               semctl(sem_id, 1, GETVAL));
        
        printf("\n");
    }
    
    printf("\nГенерация завершена. Всего строк: %d\n", num_strings);
    
    
    while (1) {
        // Ждем пока мьютекс станет 0 (потребители не работают)
        wait_until_zero(sem_id, 0);
        
        // Берем мьютекс
        struct sembuf check_mutex = {0, 1, 0};
        semop(sem_id, &check_mutex, 1);
        
        // Проверяем сколько строк осталось
        int remaining = shared_mem->line_count - shared_mem->processed_count;
        
        // Освобождаем мьютекс
        struct sembuf free_mutex = {0, -1, 0};
        semop(sem_id, &free_mutex, 1);
        
        if (remaining == 0) {
            printf("Все строки обработаны!\n");
            break;
        }
        
        printf("Осталось обработать строк: %d\n", remaining);
        sleep(1);
    }
    
    // Завершение работы
    printf("\nПроизводитель завершает работу...\n");
    
    // Даем потребителям время завершиться
    sleep(2);
    
    // Отключаем разделяемую память
    if (shmdt(shared_mem) == -1) {
        perror("Ошибка отключения разделяемой памяти");
    }
    
    // Проверяем, все ли потребители завершились
    printf("Текущие значения семафоров:\n");
    printf("  sem[0] (мьютекс): %d\n", semctl(sem_id, 0, GETVAL));
    printf("  sem[1] (строки):  %d\n", semctl(sem_id, 1, GETVAL));
    
    printf("\nПроизводитель завершил работу\n");
    return 0;
}