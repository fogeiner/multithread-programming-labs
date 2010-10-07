#include "terminal.h"

int get_terminal_width_height(int fd, int *width, int *height) {
    struct winsize win = {0, 0, 0, 0};
    int ret = ioctl(fd, TIOCGWINSZ, &win);

    if (height) {
        if (!win.ws_row) {
            char *s = getenv("LINES");
            if (s) win.ws_row = atoi(s);
        }
        if (win.ws_row <= 1 || win.ws_row >= 30000)
            win.ws_row = 24;
        *height = (int) win.ws_row;
    }

    if (width) {
        if (!win.ws_col) {
            char *s = getenv("COLUMNS");
            if (s) win.ws_col = atoi(s);
        }
        if (win.ws_col <= 1 || win.ws_col >= 30000)
            win.ws_col = 80;
        *width = (int) win.ws_col;
    }

    return ret;
}

