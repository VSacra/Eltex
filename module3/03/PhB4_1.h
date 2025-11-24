#include <stdlib.h>

#define MAX_PHONES 5
#define MAX_CONNECT 3
#define MAX_STRING 20

typedef struct Net {
	char VK[MAX_STRING];
	char TG[MAX_STRING];
	char OK[MAX_STRING];
} Net;

typedef struct Contact {
	char* Imya;
	char* Familiya;
	char* Otchestvo;
	char Numbers[MAX_PHONES][MAX_STRING];
	Net Soc;
	int ID;
	struct Contact* next;
	struct Contact* prev;
}  Contact;


Contact* addContact(Contact*);
Contact* editContact(int id, int k, char str[10], ...);
int deleteContact(int);

void AddID(Contact*);

void addConsole();
void editConsole();
void deleteConsole();
void View();

char* ReadStr();

int save();
