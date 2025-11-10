#include <stdlib.h>

double* sum(double Num1, double Num2) {
    double* result = malloc(sizeof(double));
    if (result) *result = Num1 + Num2;
    return result;
}