#include <time.h>
#include <stdio.h>

int is_leap(int year) {
    if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) {
        return 1;
    } else {
        return 0;
    }
}

int get_day(int year, int month, int day) {
    int res = 0;
    for (int i = 1; i < month; i++) {
        if (i == 2) {
            res += is_leap(year) ? 29 : 28;
        } else if (i == 4 || i == 6 || i == 9 || i == 11) {
            res += 30;
        } else {
            res += 31;
        }
    }
    return res + day;
}

