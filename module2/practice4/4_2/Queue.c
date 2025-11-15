#define _CRT_SECURE_NO_WARNINGS
#include "Queue.h"
#include <stdlib.h>
#include <time.h> 
#include <stdio.h>

Elem* head = NULL;
Elem* tail = NULL;

Elem* addElem(char typeName, int priority) {
	Elem* elemToAdd= (Elem*)malloc(sizeof(Elem));
	if (!elemToAdd) {
		return NULL;
	}
	elemToAdd->type = typeName;
	elemToAdd->priority = priority;
	elemToAdd->next = NULL;
	elemToAdd->prev = NULL;
	if (head == NULL || tail == NULL) {
		head = elemToAdd;
		head->next = NULL; head->prev = NULL;
		tail = elemToAdd;
		tail->next = NULL; tail->prev = NULL;
		return elemToAdd;
	}
	else {
		elemToAdd->prev = tail;
		tail->next = elemToAdd;
		tail = elemToAdd;
		return elemToAdd;
	}
	return NULL;
}

Elem* removeThis(int priority) {
	if (head == NULL) return NULL;
	Elem* tmp = head;

	while (tmp != NULL) {
		if (tmp->priority == priority) {
			// Обновляем связи соседей
			if (tmp->prev != NULL) {
				tmp->prev->next = tmp->next;
			}
			else {
				// Удаляем первый элемент
				head = tmp->next;
			}

			if (tmp->next != NULL) {
				tmp->next->prev = tmp->prev;
			}
			else {
				// Удаляем последний элемент
				tail = tmp->prev;
			}

			// Изолируем элемент
			tmp->next = NULL;
			tmp->prev = NULL;
			return tmp;
		}
		tmp = tmp->next;
	}
	return NULL;
}

Elem* removeAbout(int priority) {
	if (head == NULL) return NULL;
	Elem* Find = NULL;
	for (; priority <= 255; priority++) {
		Find = removeThis(priority);
		if (Find != NULL) break;
	}
	return Find;
}

Elem* removeFirst() {
	if (head == NULL) return NULL;
	Elem* toRemove=removeAbout(0);
	return toRemove;
}

int testAdd(int N) {
	srand(time(NULL));
	for (int i = 0; i < N; i++) {
		int priority = rand() % 256;
		char type = 'A' + (rand() % 26);
		Elem* elem = addElem(type, priority);
		if (elem == NULL) return -1;
	}
	return 0;
}

void View() {
	if (head == NULL) {
		printf("\nОчередь пуста.\n");
		return;
	}
	Elem* tmp = head;
	printf("\nОчередь: \n");
	while (tmp != tail) {
		printf("{%c %d} ",tmp->type,tmp->priority);
		tmp = tmp->next;
	}
	printf("{%c %d} ", tmp->type, tmp->priority);
}

void Console(int n) {
	switch (n) {
	case 1: {
		printf("\nСколько элементов хотите сгенерировать?\n");
		int Num = 0, result;
		scanf("%d",&Num);
		result = testAdd(Num);
		if (result == -1) printf("\nОшибка.\n");
		break;
	}
	case 2: {
		printf("\nВведите тип элемента: ");
		char c;
		int Num = 0;
		scanf(" %c",&c);
		printf("\nВведите приоритет элемента: ");
		scanf("%d", &Num);
		Elem* newElem = addElem(c,Num);
		if (newElem==NULL) printf("\nОшибка.\n");
		break;
	}
	case 3: {
		Elem* toRemove = removeFirst();
		if (toRemove == NULL) printf("\nСписок пуст.\n");
		else {
			printf("\nИзвлеченный элемент: {%c %d}\n",toRemove->type,toRemove->priority);
		}
		break;
	}
	case 4: {
		int Num = 0;
		printf("\nВведите приоритет элемента: ");
		scanf("%d", &Num);
		Elem* toRemove = removeThis(Num);
		if (toRemove == NULL) printf("\nСписок пуст или элемент с заданным приоритетом не найден.\n");
		else {
			printf("\nИзвлеченный элемент: {%c %d}\n", toRemove->type, toRemove->priority);
		}
		break;
	}
	case 5: {
		int Num = 0;
		printf("\nВведите приоритет элемента: ");
		scanf("%d", &Num);
		Elem* toRemove = removeAbout(Num);
		if (toRemove == NULL) printf("\nСписок пуст или элемент с приоритетом не ниже %d не найден.\n",Num);
		else {
			printf("\nИзвлеченный элемент: {%c %d}\n", toRemove->type, toRemove->priority);
		}
		break;
	}
	case 6: {
		View();
		break;
	}
	default:return;
	}
}
