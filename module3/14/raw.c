#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <sys/socket.h>

// Глобальные переменные
volatile sig_atomic_t keep_running = 1; 

// Обработчик сигнала SIGINT (Ctrl+C)
void signal_handler(int sig) {
    printf("\nЗавершение работы...\n");
    keep_running = 0;
}

int main() {
    int raw_socket; //Сырой сокет
    struct sockaddr_in server_addr; //Структура для адреса отправителя
    unsigned char buffer[65536]; //Буфер (максимальный размер IP-пакета)
    socklen_t addr_len; 
    ssize_t packet_size;
    FILE* dump_file = NULL;
    int packet_count = 0;
    
    printf("UDP сниффер (порты 51000 и 51001)\n");
    printf("Для завершения нажмите Ctrl+C\n");
    
    // Настраиваем обработчик сигналов
    signal(SIGINT, signal_handler);
    
    // Открываем файл для дампа
    dump_file = fopen("udp_dump.bin", "wb");
    if (dump_file == NULL) {
        perror("Ошибка открытия файла");
        return 1;
    }
    
     /* Создание RAW сокета для перехвата UDP пакетов
     AF_INET - IPv4 адресация
     SOCK_RAW - сырой сокет (получаем полные IP пакеты)
     IPPROTO_UDP - фильтрация только UDP трафика
     */
    raw_socket = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (raw_socket == -1) {
        perror("Ошибка создания RAW сокета");
        printf("Для работы нужны права root\n");
        fclose(dump_file);
        return 1;
    }
    
    printf("Дамп пишется в udp_dump.bin\n\n");
    
    // Основной цикл перехвата
    while (keep_running) {
        addr_len = sizeof(server_addr);
        
        /* Получение пакета из сети
         recvfrom - блокирующий вызов, ждет появления пакета
         raw_socket - сокет для получения
         buffer - буфер для данных
         sizeof(buffer) - максимальный размер данных
         0 - флаги (по умолчанию)
         (struct sockaddr*)&server_addr - указатель на структуру адреса отправителя
         &addr_len - указатель на длину структуры адреса
         */
        
        packet_size = recvfrom(raw_socket, buffer, sizeof(buffer), 0,
                              (struct sockaddr*)&server_addr, &addr_len);
        
        if (packet_size == -1) {
            if (errno == EINTR) continue;
            perror("Ошибка recvfrom");
            break;
        }
        
        if (packet_size > 0) {
            packet_count++;
            printf("Пакет #%d: %zd байт\n", packet_count, packet_size);
            
            // Записываем полученные данные в файл
            fwrite(buffer, 1, packet_size, dump_file);
            fflush(dump_file);
        }
    }
    
    // Завершение работы
    printf("\nЗавершено. Пакетов: %d\n", packet_count);
    
    fclose(dump_file);
    close(raw_socket);
    
    return 0;
}