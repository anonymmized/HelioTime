#include <stdio.h>
#include <string.h>
#include <time.h>
#include "calculations.h"
#include <stdlib.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/stat.h>
#endif
#include <errno.h>

#define MAXLINE 256
#define COLOR_RESET "\x1b[0m"
#define COLOR_RED "\x1b[31m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_GRAY  "\033[90m"
#define ESC "\x1b"

char home_path[MAXLINE];
const char *names[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

// ===MAIN===

double lat = 0.0, lon = 0.0;
int tz_offset_mins = 0;
int day_num = 0;

// === === ===

int get_path(char *buf, size_t size);
void usage(void);
int ensure_config_dir(const char *path);
int get_config_path(char *buf, size_t size);
int is_leap(int year);
int get_day(int year, int month, int day);
int get_timezone_offset(double);
int get_month_days(int year, int month);
int* get_date(int year, int month);
int get_weekday(int year, int day_num);
static void repeat(const char *s, int n);
static void box_print(int width, char *lines[], int nlines);

static void repeat(const char *s, int n) {
    for (int i = 0; i < n; i++) fputs(s, stdout);
}

static void box_print(int width, char *lines[], int nlines) {
    int inner = width - 2;

    int w1 = 5;
    int w2 = 13;
    int w3 = 9;
    int w4 = inner - (w1 + w2 + w3 + 3);

    printf("┌"); 
    repeat("─", inner); 
    printf("┐\n");

    char hdr[256];
    char tz[256];

    snprintf(hdr, sizeof(hdr), " %s ", lines[0]);
    snprintf(tz, sizeof(tz), " %s ", lines[1]);

    int pad1 = inner - (int)strlen(hdr);
    if (pad1 < 0) pad1 = 0;
    int left1 = pad1 / 2;
    int right1 = pad1 - left1;

    int pad2 = inner - (int)strlen(tz);
    if (pad2 < 0) pad2 = 0;
    int left2 = pad2 / 2;
    int right2 = pad2 - left2;

    printf("│");
    repeat(" ", left1);
    printf("%s", hdr);
    repeat(" ", right1);
    printf("│\n");

    printf("│");
    repeat(" ", left2);
    printf("%s", tz);
    repeat(" ", right2);
    printf("│\n");

    printf("├"); repeat("─", w1); printf("┬");
    repeat("─", w2); printf("┬");
    repeat("─", w3); printf("┬");
    repeat("─", w4); printf("┤\n");

    for (int i = 2; i < nlines; i++) {
        if (i == 3) {
            printf("├"); repeat("─", w1); printf("┼");
            repeat("─", w2); printf("┼");
            repeat("─", w3); printf("┼");
            repeat("─", w4); printf("┤\n");
            continue;
        }
        char tmp[256];
        strncpy(tmp, lines[i], sizeof(tmp) - 1);
        char *c1 = strtok(tmp, "│");
        char *c2 = strtok(NULL, "│");
        char *c3 = strtok(NULL, "│");
        char *c4 = strtok(NULL, "│");

        if (!c1) c1 = "";
        if (!c2) c2 = "";
        if (!c3) c3 = "";
        if (!c4) c4 = "";
        
        while (*c1 == ' ') c1++;
        while (*c2 == ' ') c2++;
        while (*c3 == ' ') c3++;
        while (*c4 == ' ') c4++;
        
        printf("│ %-*.*s│ %-*.*s│ %-*.*s│ %-*.*s│\n", w1 - 1, w1 - 1, c1,
                w2 - 1, w2 - 1, c2,
                w3 - 1, w3 - 1, c3,
                w4 - 1, w4 - 1, c4);
    }
    printf("└"); repeat("─", w1); printf("┴");
    repeat("─", w2); printf("┴");
    repeat("─", w3); printf("┴");
    repeat("─", w4); printf("┘\n");
}

int get_weekday(int year, int day_num) {
    struct tm t = {0};
    t.tm_year = year - 1900;
    t.tm_mon = 0;
    t.tm_mday = day_num;

    mktime(&t);

    return t.tm_wday;
}

int get_timezone_offset(double lon) {
    return (int)(lon / 15.0 + (lon >= 0 ? 0.5 : -0.5));
}

int is_leap(int year) {
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
        return 1;
    } else {
        return 0;
    }
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

int get_day(int year, int month, int day) {
    int res = 0;
    for (int i = 1; i < month; i++) {
        if (i == 2) {
            res += (is_leap(year)) ? 29 : 28;
        } else if (i == 4 || i == 6 || i == 9 || i == 11) {
            res += 30;
        } else {
            res += 31;
        }
    }
    return res + day;
}


int get_path(char *buf, size_t size) {
#ifdef _WIN32
    const char *home = getenv("USERPROFILE");
#else
    const char *home = getenv("HOME");
#endif
    if (!home) return -1;
    if (strlen(home) + 1 > size) return -1;

    strcpy(buf, home);
    return 0;
}

void usage(void) {
    fprintf(stdout, "Usage:\n");
    fprintf(stdout, "./prog --setloc --lat fnum1 --lon fnum2\n");
    fprintf(stdout, "./prog --setloc --lon fnum1 --lat fnum2\n");
}

int ensure_config_dir(const char *path) {
#ifdef _WIN32
    DWORD attr = GetFileAttributesA(path);
    if (attr != INVALID_FILE_ATTRIBUTES) {
        if (attr & FILE_ATTRIBUTE_DIRECTORY) return 0;
        return -1;
    }

    if (CreateDirectoryA(path, NULL)) return 0;

    if (GetLastError() == ERROR_ALREADY_EXISTS) return 0;

    return -1;
#else 
    struct stat st;
    if (stat(path, &st) == 0) {
        if (S_ISDIR(st.st_mode)) return 0;
        return -1;
    }

    if (mkdir(path, 0755) == -1) return -1;
    return 0;
#endif
} 

int get_config_path(char *buf, size_t size) {
#ifdef _WIN32
    char dir[MAXLINE];
    const char *appdata = getenv("APPDATA");
    if (!appdata) {
        fprintf(stderr, "APPDATA not set\n");
        return 1;
    }
    if (snprintf(dir, sizeof(dir), "%s\\sunweek", appdata) >= (int)sizeof(dir)) {
        fprintf(stderr, "Path to dir too long\n");
        return 1;
    }

    if (ensure_config_dir(dir) != 0) {
        fprintf(stderr, "Cannot find path\n");
        return 1;
    }

    if (snprintf(buf, size, "%s\\config", dir) >= (int)size) {
        fprintf(stderr, "Path too long\n");
        return 1;
    }
#else 
    char dir[MAXLINE];
    if (snprintf(dir, sizeof(dir), "%s/.config/sunweek", home_path) >= (int)sizeof(dir)) {
        fprintf(stderr, "Path to dir too long\n");
        return 1;
    }

    if (ensure_config_dir(dir) != 0) {
        fprintf(stderr, "Cannot find path\n");
        return 1;
    }

    if (snprintf(buf, size, "%s/config", dir) >= (int)size) {
        fprintf(stderr, "Path too long\n");
        return 1;
    }
#endif
    return 0;
}



int main(int argc, char *argv[]) {
    
    FILE *fp = NULL;
    char line[MAXLINE];
    if (get_path(home_path, MAXLINE) != 0) {
        fprintf(stderr, "Cannot get home path\n");
        return 1;
    }
    if (argc == 1) {
        char path[MAXLINE];
        if (get_config_path(path, sizeof(path)) == 1) {
            fprintf(stderr, "Error with connecting config file\n");
            return 1;
        }
        fp = fopen(path, "r");
        if (!fp) {
            perror("fopen");
            return 1;
        }

        while (fgets(line, sizeof(line), fp) != NULL) {
            if (strncmp(line, "lat", 3) == 0) {
                char *eq = strchr(line, '=');
                if (eq) lat = atof(eq + 1);
            } else if (strncmp(line, "lon", 3) == 0) {
                char *eq = strchr(line, '=');
                if (eq) lon = atof(eq + 1);
            }
        }
        fclose(fp);
    } else if (argc == 6 && strcmp(argv[1], "--setloc") == 0) {
        if ((strcmp(argv[2], "--lat") == 0) && (strcmp(argv[4], "--lon") == 0)) {
            lat = atof(argv[3]);
            lon = atof(argv[5]);
        } else if ((strcmp(argv[2], "--lon") == 0) && (strcmp(argv[4], "--lat") == 0)) {
            lon = atof(argv[3]);
            lat = atof(argv[5]);
        } else {
            fprintf(stderr, "Wrong arguments\n");
            usage();
            return 1;
        }

        if (lat < -90.0 || lat > 90.0 || lon < -180.0 || lon > 180.0) {
            fprintf(stderr, "Invalid coordinates\n");
            return 1;
        }

        char lat_buf[64];
        char lon_buf[64];
        snprintf(lat_buf, sizeof(lat_buf), "%.2f", lat);
        snprintf(lon_buf, sizeof(lon_buf), "%.2f", lon);
        char path[MAXLINE];
        if (get_config_path(path, sizeof(path)) == 1) {
            fprintf(stderr, "Error with connecting config file\n");
            return 1;
        }
        fp = fopen(path, "w");
        if (!fp) {
            perror("fopen");
            return 1;
        }
        fprintf(fp, "#sunweek config\n");
        fprintf(fp, "lat = %s\n", lat_buf);
        fprintf(fp, "lon = %s\n", lon_buf);
        fclose(fp);
    } else {
        usage();
        return 1;
    }

    time_t noww = time(NULL);
    struct tm *t = localtime(&noww);
    int year = t->tm_year + 1900;
    int month = t->tm_mon + 1;
    int day = t->tm_mday;
    int day_num_conf = get_day(year, month, day);
    int w_conf = get_weekday(year, day_num_conf);
    int first_w_day = (w_conf+6) % 7;
    tz_offset_mins = get_timezone_offset(lon);

    char *lines[12];
    char buf[12][256];


    snprintf(buf[0], sizeof(buf[0]), "SUN REPORT (THIS WEEK)");
    lines[0] = buf[0];

    snprintf(buf[1], sizeof(buf[1]), "Time Zone %d", tz_offset_mins);
    lines[1] = buf[1];

    snprintf(buf[2], sizeof(buf[2]), "Day │ Date        │ Sunrise │ Sunset");
    lines[2] = buf[2];

    snprintf(buf[3], sizeof(buf[3]), "--- │  ---------- │ ------- │ ------");
    lines[3] = buf[3];


    for (int i = 0; i < 7; i++) {
        int day_num = get_day(year, month, day - first_w_day + i);
        int *date = get_date(year, day_num);
        int w = get_weekday(year, day_num);
        int *times = get_sunrise_sunset(day_num, lat, lon, tz_offset_mins);

        if (w == w_conf) {
            snprintf(buf[4 + i], sizeof(buf[4 + i]), "%-3s │ %02d.%02d.%04d  │ %02d:%02d   │ %02d:%02d  < Current",
                    names[w], date[0], date[1], date[2], times[0], times[1], times[2], times[3]);
        } else {
            snprintf(buf[4 + i], sizeof(buf[4 + i]), "%-3s │ %02d.%02d.%04d  │ %02d:%02d   │ %02d:%02d", 
                    names[w], date[0], date[1], date[2], times[0], times[1], times[2], times[3]);
        }
        lines[4 + i] = buf[4 + i];
        
        free(date);
        free(times);
    }

    box_print(61, lines, 11);

    return 0;
}
