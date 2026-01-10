#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/stat.h>
#endif
#include <errno.h>

#define MAXLINE 256

char hom_path[MAXLINE];

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
int get_timezone_offset_minutes(void);

int get_timezone_offset_minutes(void) {
    time_t now = time(NULL);

    struct tm local_tm;
    struct tm utc_tm;

#ifdef _WIN32
    localtime_s(&local_tm, &now);
    gmtime_s(&utc_tm, &now);
#else 
    localtim_r(&now, &local_tm);
    gmtime_r(&now, &utc_tm);
#endif
    time_t local_time = mktime(&local_tm);
    time_t utc_time = mktime(&utc_tm);
    return (int)difftime(local_time, utc_time) / 60;
}

int is_leap(int year) {
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
        return 1;
    } else {
        return 0;
    }
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
    const char *appdata = getenv("APPDATA");
    if (!appdata) {
        fprintf(stderr, "APPDATA not set\n");
        return 1;
    }
    if (snprintf(buf, size, "%s\\sunweek\\config", appdata) >= (int)size) {
        fprintf(stderr, "Path too long\n");
        return 1;
    }
    if (ensure_config_dir(buf) != 0) {
        fprintf(stderr, "Cannot find path\n");
        return 1;
    }
#else 
    if (snprintf(buf, size, "%s/.config/sunweek/config", home_path) >= (int)size) {
        fprintf(stderr, "Path too long\n");
        return 1;
    }
    if (ensure_config_dir(buf) != 0) {
        fprintf(stderr, "Cannot find path\n");
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
        printf("lat: %.3f\nlon: %.3f", lat, lon);
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
    
    }
    return 0;
}
