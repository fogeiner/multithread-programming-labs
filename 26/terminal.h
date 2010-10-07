#ifndef TERMINAL_SIZE_H
#define TERMINAL_SIZE_H
#include <sys/ioctl.h>
#include <stdlib.h>
#include <termios.h>
#include <math.h>
int get_terminal_width_height(int fd, int *width, int *height);
#endif
