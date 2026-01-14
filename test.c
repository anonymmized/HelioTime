#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int is_leap(int year) {
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) return 1;
    else return 0;
}

int get_month_days(int year, int month) {
    if (month == 2) return is_leap(year) ? 29 : 28;
    if (month == 4 || month == 6 || month == 9 || month == 11) return 30;
    return 31;
}

int* get_date(int year, int day_of_year) {
    int max = is_leap(year) ? 366 : 365;
    if (day_of_year < 1 || day_of_year > max) return NULL;
    int *res = malloc(sizeof(int) * 3);
    if (!res) return NULL;

    int month = 1;
    int day = day_of_year;
    while (month <= 12) {
        int dim = get_month_days(year, month);
        if (day <= dim) break;
        day -= dim;
        month += 1;
    }
    res[0] = day;
    res[1] = month;
    res[2] = year;
    return res;
}

int main() {
    int year = 2026;
    int day = 186;
    int *res = get_date(year, day);
    printf("%02d.%02d.%04d", res[0], res[1], res[2]);
    return 0;
}
