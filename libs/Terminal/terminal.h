#ifndef TERMINAL_SIZE_H
#define TERMINAL_SIZE_H
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>
#include <math.h>
int get_terminal_width_height(int fd, int *width, int *height);
int term_canon_on();
int term_canon_off();
#endif

