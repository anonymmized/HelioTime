#include <time.h>

int get_timezone_offset_minutes(void) {
    time_t now = time(NULL);

    struct tm local_tm;
    struct tm utc_tm;

#if defined(_WIN32)
    localtime_s(&local_tm, &now);
    gmtime_s(&utc_tm, &now);
#else
    localtime_r(&now, &local_tm);
    gmtime_r(&now, &utc_tm);
#endif
    time_t local_time = mktime(&local_tm);
    time_t utc_time = mktime(&utc_tm);

    return (int)difftime(local_time, utc_time) / 60;
}
