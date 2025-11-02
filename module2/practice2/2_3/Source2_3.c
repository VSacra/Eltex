#include "Header2_3.h"
#include <stdio.h>

double* sum(double Num1, double Num2) {
	double* result = malloc(sizeof(double));
	if (result) *result = Num1 + Num2;
	return result;
}

double* divide(double Num1, double Num2) {
	if (Num2 == 0) return NULL;
	double* result = malloc(sizeof(double));
	if (result) *result = Num1 / Num2;
	return result;
}

double* multi(double Num1, double Num2) {
	double* result = malloc(sizeof(double));
	if (result) *result = Num1 * Num2;
	return result;
}

double* sub(double Num1, double Num2) {
	double* result = malloc(sizeof(double));
	if (result) *result = Num1 - Num2;
	return result;
}