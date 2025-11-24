#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("\nНеверный формат!\n");
        return 1;
    }
    int maxlen = 0;
    for (int i = 0; i < argc; i++) {
        int len = strlen(argv[i]);
        if (len > maxlen) maxlen = len;
    }
    printf("\nМаксималная длина строки %d", maxlen);
    return 0;
}