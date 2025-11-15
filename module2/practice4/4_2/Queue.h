
typedef struct Elem{
	char type;
	int priority;
	struct Elem* next;
	struct Elem* prev;
} Elem;

Elem* addElem(char,int);
Elem* removeFirst();
Elem* removeThis(int);
Elem* removeAbout(int);

int testAdd(int);

void View();
void Console(int);

