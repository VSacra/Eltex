#include "IPV4.h"
#include <stdio.h>
#include <string.h>



int main() {
	while (1) {
		system("chcp 1251");
		printf("\nСимуляция отправки пакетов. Чтобы продолжить введите любую кнокку или введите q для выхода\n");
		int ch = getchar();
		while (getchar() != '\n');
		if (ch == 'q') return 0;
		char IP[16]; char Mask[16]; int N;
		printf("\nВведите IP-адрес шлюза: ");
		fgets(IP, sizeof(IP), stdin);
		IP[strcspn(IP, "\n")] = '\0';
		printf("\nВведите маску: ");
		fgets(Mask, sizeof(Mask), stdin);
		Mask[strcspn(Mask, "\n")] = '\0';
		printf("\nВведите количество пакетов: ");
		scanf_s("%d", &N);
		while (getchar() != '\n');
		printf("\nЗапускаем симуляцию.\n");
		Send(N, &IP, &Mask);
		
	}
}