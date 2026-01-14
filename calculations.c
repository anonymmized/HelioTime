#include "calculations.h"
#include <stdlib.h>
#include <math.h>


double a_cos(double x) {
    return atan2(sqrt(1 - x*x), x);
}

double to_rad(double degree) {
    return (M_PI * degree) / 180;
}

int* get_sunrise_sunset(int day, double lat, double lon, int tz) {
    int *times = malloc(sizeof(int) * 4);
    // Calculate H0
    double lat_rad = to_rad(lat);
    double gamma = (2.0 * M_PI / 365.0) * (day - 81);
    double declination = to_rad(23.44) * sin(gamma);
    double hh = to_rad(-0.833);
    double cosH0 = (sin(hh) - sin(lat_rad) * sin(declination)) / (cos(lat_rad) * cos(declination));
    if (cosH0 > 1.0) cosH0 = 1.0;
    if (cosH0 < -1.0) cosH0 = -1.0;
    double h0 = a_cos(cosH0);
    // Calculate solar sunset/sunrise
    double day_len = (2 * h0) / to_rad(15.0);
    double sunrise_solar = 12 - (h0 / to_rad(15.0));
    double sunset_solar = 12 + (h0 / to_rad(15.0));
    // Calculate EOT
    double lon_merid = 15.0 * tz;
    double lon_correction = 4 * (lon_merid - lon);
    double B = (360.0 / 365.0) * (day - 81);
    double Brad = to_rad(B);
    double eot = 9.87 * sin(2.0 * Brad) - 7.53 * cos(Brad) - 1.5 * sin(Brad);
    // Calculate sunrise/sunset
    double C = lon_correction - eot;
    double t_sunrise = sunrise_solar + (C / 60.0);
    double t_sunset = sunset_solar + (C / 60.0);

    int sunrise_min = (int)lround(t_sunrise * 60.0);
    int sunset_min = (int)lround(t_sunset * 60.0);

    sunrise_min = (sunrise_min % 1440 + 1440) % 1440;
    sunset_min = (sunset_min % 1440 + 1440) % 1440;

    int hours1 = sunrise_min / 60;
    int minutes1 = sunrise_min % 60;

    int hours2 = sunset_min / 60;
    int minutes2 = sunset_min % 60;

    times[0] = hours1;
    times[1] = minutes1;
    times[2] = hours2;
    times[3] = minutes2;
    return times;
}
