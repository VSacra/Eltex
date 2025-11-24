#include <stdlib.h>
#include <stdio.h>

int main (int argc, char* argv[]) {
    if (argv[3]!=NULL) {
        printf("\nНеверный формат!\n");
        return 1;
    }
    int sub = 0;
    int num1 = atoi(argv[1]);
    int num2 = atoi(argv[2]);
    if (num1 == 0 && *argv[1] != '0') {
        printf("\nВведены символы!\n");
        return 1;
    }
    if (num2 == 0 && *argv[2] != '0') {
        printf("\nВведены символы!\n");
        return 1;
    }
    sub = num1 - num2;
    printf("\n%d - %d = %d",num1,num2,sub);
    return 0;
}
