#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAXLINE 256

char home_path[MAXLINE];

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

int main(int argc, char *argv[]) {
    double lat = 0.0, lon = 0.0;
    FILE *fp;
    char line[MAXLINE];
    if (get_path(home_path, MAXLINE) != 0) {
        fprintf(stderr, "Cannot get home path\n");
        return 1;
    }
    if (argc == 1) {
    #if defined (_WIN32)
        char path[MAXLINE];
        const char *appdata = getenv("APPDATA");
        if (!appdata) {
            fprintf(stderr, "APPDATA not set\n");
            return 1;
        }

        if (snprintf(path, sizeof(path), "%s\\sunweek\\config", appdata) >= sizeof(path)) {
            fprintf(stderr, "Path too long\n");
            return 1;
        }
        fp = fopen(path, "r");
        if (!fp) {
            perror("fopen");
            return 1;
        }
    #else 
        char buf[MAXLINE];
        if (snprintf(buf, sizeof(buf), "%s/.config/sunweek/config", home_path) >= sizeof(buf)) {
            fprintf(stderr, "Path too long\n");
            return 1;
        }
        fp = fopen(buf, "r");
        if (!fp) {
            perror("fopen");
            return 1;
        }
    #endif
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
    } /*else if (argc == 5) {
    #if defined (_WIN32)
        fp = fopen("%%APPDATA%\\sunweek\\config", "w");
    #else 
        fp = fopen("~/.config/sunweek/config", "w");
    #endif
        if (strcmp(*++argv))
    }
    */
    return 0;
}
