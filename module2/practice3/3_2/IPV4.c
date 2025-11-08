#include "IPV4.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h> 
#include <time.h>   

uint32_t* getGate(char* IP,uint32_t* adress) {
	int  count = 0;
	char c = ' ';
	uint8_t octet=0;
	int octetCount = 0;
	int len = strlen(IP);
	for (int i = 0; i <= len; i++) {
		c = IP[i];
		if (c >= '0' && c <= '9') {
			count++;
			if (count > 3) return NULL;
			octet = octet * 10 + (c - '0');
		}
		else if (c == '.' || c == '\0') {
			count = 0;
			if (octet < 0 || octet>255) return NULL;
			*adress = (*adress <<8) | octet; 
			octet = 0;
			octetCount++;
			if (octetCount > 4) return NULL;
		}
		else return NULL;
	}
	return adress;
}

uint32_t* getMask(char* Mask, uint32_t* mask) {
	int m = 0;
	if (Mask[0] == '/') { //Найти маску числом
		for (int i = 1; Mask[i] != '\0'; i++) {
			char c = Mask[i];
			if (c < '0' || c > '9') return NULL;
			else m = m * 10 + c - '0';
		}
		if (m < 0 || m > 32) return NULL;
		int octetres = 255;
		int octetemp = 0;
		for (int i = 0; i<=4;i++){
			if (i <= m / 8) {
				*mask = (*mask << 8) | octetres;
			}
			else *mask = (*mask << 8) | octetemp;
		}
	}
	else {
		int  count = 0;
		char c = ' ';
		uint8_t octet = 0;
		int octetCount = 0;
		int len = strlen(Mask);
		for (int i = 0; i <= len; i++) {
			c = Mask[i];
			if (c >= '0' && c <= '9') {
				count++;
				if (count > 3) return NULL;
				octet = octet * 10 + (c - '0');
			}
			else if (c == '.' || c == '\0') {
				count = 0;
				if (octet < 0 || octet>255) {return NULL;}
				*mask = (*mask << 8) | octet; octet = 0;
				octetCount++;
				if (octetCount > 4) return NULL;
			}
			else return NULL;
		}
	}
	return mask;
}

int genIP(int N,uint32_t Gate, uint32_t Mask) {
	srand(time(NULL));
	int Count = 0;
	for (int i = 0; i < N; i++) {
		uint32_t adress = 0;
		uint8_t octet = 0;
		uint32_t podSet = Gate & Mask;
		for (int j = 0; j <= 4; j++) {
			octet = rand() % 256;
			adress = (adress << 8) | octet;
		}
		uint32_t pack = adress & Mask;
		if (podSet == pack) Count++;
	}
	return Count;
}

void printIP(uint32_t ip) {
	printf("%u.%u.%u.%u\n",
		(ip >> 24) & 0xFF,
		(ip >> 16) & 0xFF,
		(ip >> 8) & 0xFF,
		ip & 0xFF);
}

void Send(int N,char* IP,char* Mask) {
	uint32_t ad = 0; uint32_t ms = 0;
	if (getGate(IP, &ad)==NULL) { printf("\nОшибка. Неверно указан IP-адрес шлюза.\n"); return; }
	if (getMask(Mask, &ms)==NULL) { printf("\nОшибка. Неверно указана маска.\n"); return; }
	int count = genIP(N, ad, ms);
	printf("\nОтправка пакетов завршена. Статистика:\nIP-адрес подсети: ");
	printIP(ad);
	printf("\nМаска: ");
	printIP(ms);
	if(count)printf("\nПакетов в подсети: %d\nВ процентном соотношении: %.2f %% \n", count, ((float)count / (float)N * 100));
	else printf("\nПакетов в подсети: %d\nВ процентном соотношении: 0 %% \n", count);
}
