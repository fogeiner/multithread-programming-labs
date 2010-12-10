#include "Terminal.h"

static struct termios saved_tty, changed_tty;

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

int term_save_state(){
	if(tcgetattr(STDIN_FILENO, &saved_tty) == -1){
		return -1;
	}
}

int term_restore_state(){
		if(tcsetattr(STDIN_FILENO, TCSANOW, &saved_tty) == -1){
			return -1;
		}
}

int term_canon_off(){
	// making changes
	changed_tty = saved_tty;
	changed_tty.c_lflag &= ~(ICANON | ECHO);
	changed_tty.c_cc[VMIN] = 1;
	changed_tty.c_cc[VTIME] = 0;

	// applying it to a device
	if(tcsetattr(STDIN_FILENO, TCSANOW, &changed_tty) == -1){
		return -1;
	}
}



