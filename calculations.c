#include <math.h>
#include <stdio.h>

const double pi = M_PI;

double a_cos(double x) {
    return atan2(sqrt(1 - x*x), x);
}

double to_rad(double degree) {
    return (3.14 * degree) / 180;
}

double calc_h0(unsigned int day, double lat) {
    double declination = to_rad(-23.44 * cos((2 * pi * 22) / 365));
    double lat_rad = to_rad(lat);
    double h0 = a_cos((-1) * tan(lat_rad) * tan(declination));
    return h0;
}

double day_len(double h0) {
    return (2 * h0) / to_rad(15.0);
}

int main() {
    unsigned int day;
    double lat, lon;
    int tz;
    printf("Enter day: ");
    scanf("%d", &day);
    printf("Enter latitude: ");
    scanf("%lf", &lat);
    printf("Enter longitude: ");
    scanf("%lf", &lon);
    printf("Enter time zone: ");
    scanf("%d", &tz);
    double h0 = calc_h0(day, lat);
    printf("H0 for moscow %d day: %.4f\n", day, h0);
    double day_len = (2 * h0) / to_rad(15.0);
    double sunrise_solar = 12 - (h0 / to_rad(15.0));
    double sunset_solar = 12 + (h0 / to_rad(15.0));
    printf("Day length: %.2f\n", day_len);
    printf("Sunrise (solar): %.2f\nSunset (solar): %.2f\n", sunrise_solar, sunset_solar);
    double lon_merid = 15.0 * tz;
    double lon_correction = 4 * (lon_merid - lon);
    printf("longitude correction: %.2f\n", lon_correction);
    return 0;
}

