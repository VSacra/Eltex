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
	struct Contact* left;
	struct Contact* right;
	struct Contact* parent;
	int level;
	int height;
}  Contact;


Contact* addContact(Contact*);
Contact* editContact(int id, int k, char str[10], ...);
int deleteContact(int);

Contact* insert(Contact*, Contact*);
Contact* del(Contact*, int);
Contact* minValueNode(Contact*);

void AddID(Contact*);

void addConsole();
void editConsole();
void deleteConsole();
void View();
void printTree(Contact*, int, char*);

int getHeight(Contact*);
int getBalance(Contact*);
int max(int,int);
Contact* rightRotate(Contact*);
Contact* leftRotate(Contact*);

char* ReadStr();
