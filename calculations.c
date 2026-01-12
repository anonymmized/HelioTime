#include <math.h>
#include <stdio.h>

const double pi = M_PI;

double a_cos(double x) {
    return atan2(sqrt(1 - x*x), x);
}

double to_rad(double degree) {
    return (3.14 * degree) / 180;
}

int main() {
    double declination = -23.44 * cos((2 * pi * 22)/365);
    double decl_rad = to_rad(declination);
    double lat_rad = to_rad(55.75);
    printf("Declination for 12 jan: %.4f\n", declination);
    double cos_h0 = (-1) * tan(lat_rad) * tan(decl_rad);
    double h0 = a_cos(cos_h0);
    printf("Cos h0: %.4f\n", cos_h0);
    printf("h0: %.4f\n", h0);
    return 0;
}

