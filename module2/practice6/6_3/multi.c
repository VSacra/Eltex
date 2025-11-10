#include <stdlib.h>

double* multi(double Num1, double Num2) {
    double* result = malloc(sizeof(double));
    if (result) *result = Num1 * Num2;
    return result;
}