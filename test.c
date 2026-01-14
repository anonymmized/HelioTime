#include <stdio.h>
#include <string.h>

#define ESC "\x1b"

static void repeat(const char *s, int n) {
    for (int i = 0; i < n; i++) fputs(s, stdout);
}

static void box_print(int width, const char *title, const char *line1, const char *line2) {
    int inner = width - 2;

    // верхняя линия
    printf("┌"); repeat("─", inner); printf("┐\n");

    // заголовок (по центру)
    char hdr[256];
    snprintf(hdr, sizeof(hdr), " %s ", title);

    int pad = inner - (int)strlen(hdr);
    int left = pad / 2;
    int right = pad - left;

    printf("│");
    repeat(" ", left);
    printf(ESC "[1m%s" ESC "[0m", hdr); // жирный заголовок
    repeat(" ", right);
    printf("│\n");

    // разделитель
    printf("├"); repeat("─", inner); printf("┤\n");

    // содержимое
    printf("│ %-*.*s │\n", inner - 2, inner - 2, line1);
    printf("│ %-*.*s │\n", inner - 2, inner - 2, line2);

    // низ
    printf("└"); repeat("─", inner); printf("┘\n");
}

int main(void) {
    // важно: чтобы Unicode нормально отображался
    // в macOS обычно уже окей, если терминал UTF-8

    // пример “неонового” цвета для рамки (простая версия — просто текст)
    box_print(64,
              "Your Messages",
              "From: MyAfWPY+LvV+bkwTCAR+XpaUkGQFceXxbgAKmNtFcH8",
              "Wed, Jan 7 2026 06:31   moooooooore :)");

    return 0;
}
