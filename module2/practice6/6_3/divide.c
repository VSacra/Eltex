#include <stdlib.h>

double* divide(double Num1, double Num2) {
    if (Num2 == 0) return NULL;
    double* result = malloc(sizeof(double));
    if (result) *result = Num1 / Num2;
    return result;
}
