#ifndef CALC_H
#define CALC_H

#include <math.h>

#define PI M_PI
double a_cos(double);
double to_rad(double);
double day_len(double);
double eot_calc(int);
int* get_sunrise_sunset(int, double, double, int);

#endif
