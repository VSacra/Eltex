#include <stdlib.h>

double* sum(double, double);
double* divide(double, double);
double* multi(double, double);
double* sub(double, double);

double* (*oper) (double, double);