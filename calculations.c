#include <math.h>
#include <stdio.h>

const double pi = M_PI;

double a_cos(double x) {
    return atan2(sqrt(1 - x*x), x);
}

double to_rad(double degree) {
    return (M_PI * degree) / 180;
}

double calc_h0(int day, double lat) {
    double lat_rad = to_rad(lat);
    double gamma = (2.0 * M_PI / 365.0) * (day - 81);
    double declination = to_rad(23.44) * sin(gamma);

    double cosH0 = -tan(lat_rad) * tan(declination);
    double h0 = a_cos(cosH0);
    return h0;
}

double day_len(double h0) {
    return (2 * h0) / to_rad(15.0);
}

double eot_calc(int day) {
    double B = (360.0 / 365.0) * (day - 81);
    double Brad = to_rad(B);
    return 9.87 * sin(2.0 * Brad) - 7.53 * cos(Brad) - 1.5 * sin(Brad);
}

int main() {
    int day;
    double lat = 55.75, lon = 37.62;
    int tz = 3;
    printf("Enter day: ");
    scanf("%d", &day);
    //printf("Enter latitude: ");
    //scanf("%lf", &lat);
    //printf("Enter longitude: ");
    //scanf("%lf", &lon);
    //printf("Enter time zone: ");
    //scanf("%d", &tz);
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
    double eot = eot_calc(day);
    printf("eot: %.2f\n", eot);
    double C = lon_correction - eot;
    double t_sunrise = sunrise_solar + (C / 60.0);
    int hours1 = (int)t_sunrise;
    int minutes1 = round((t_sunrise - hours1) * 60);
    if (minutes1 == 60) {
        minutes1 = 0;
        hours1 += 1;
    }

    double t_sunset = sunset_solar + (C / 60.0);
    int hours2 = (int)t_sunset;
    int minutes2 = round((t_sunset - hours2) * 60.0);
    if (minutes2 == 60) {
        minutes2 = 0;
        hours2 += 1;
    }
    printf("%d - Sunrise - %02d:%02d\n%d - Sunset - %02d:%02d\n", day, hours1, minutes1, day, hours2, minutes2);
    return 0;
}

